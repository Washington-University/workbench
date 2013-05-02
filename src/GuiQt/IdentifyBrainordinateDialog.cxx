
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__
#include "IdentifyBrainordinateDialog.h"
#undef __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__

using namespace caret;

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "GuiManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
    
/**
 * \class caret::IdentifyBrainordinateDialog 
 * \brief Dialog that allows user to enter brainordinate and get info on it
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
IdentifyBrainordinateDialog::IdentifyBrainordinateDialog(QWidget* parent)
: WuQDialogModal("Identify Brainordinate",
                 parent)
{
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<CiftiMappableConnectivityMatrixDataFile*> matrixFiles;
    brain->getAllCiftiConnectivityMatrixFiles(matrixFiles);
    
    /*
     * Surface Vertex widgets
     */
    m_vertexRadioButton = new QRadioButton("Surface Vertex");
    
    QLabel* vertexStructureLabel = new QLabel("Structure");
    m_vertexStructureComboBox = new StructureEnumComboBox(this);
    m_vertexStructureComboBox->listOnlyValidStructures();
    m_vertexStructureComboBox->setSelectedStructure(s_lastSelectedStructure);
    
    QLabel* vertexIndexLabel = new QLabel("Vertex Index");
    m_vertexIndexSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0,
                                                                std::numeric_limits<int>::max(),
                                                                1);
    m_vertexIndexSpinBox->setValue(s_lastSelectedVertexIndex);
    
    /*
     * Disable surface option if no surfaces loaded
     */
    if (m_vertexStructureComboBox->count() == 0) {
        m_vertexRadioButton->setEnabled(false);
        vertexStructureLabel->setEnabled(false);
        m_vertexStructureComboBox->getWidget()->setEnabled(false);
        vertexIndexLabel->setEnabled(false);
        m_vertexIndexSpinBox->setEnabled(false);
    }
    
    /*
     * CIFTI file row widgets
     */
    m_ciftiFileRadioButton = new QRadioButton("Cifti File Row");
    
    QLabel* ciftiFileLabel = new QLabel("File");
    m_ciftiFileComboBox = WuQFactory::newComboBox();
    
    QLabel* ciftiFileRowIndexLabel = new QLabel("Row Index");
    m_ciftiFileRowIndexSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0,
                                                                      std::numeric_limits<int>::max(),
                                                                      1);
    m_ciftiFileRowIndexSpinBox->setValue(s_lastSelectedCiftiRowIndex);
    
    /*
     * Button group to keep radio buttons mutually exclusive
     */
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_vertexRadioButton);
    buttGroup->addButton(m_ciftiFileRadioButton);
    
    /*
     * Load CIFTI files into combo box but only those that
     * have brainordinates mapped to the rows
     */
    int32_t defaultIndex = -1;
    const int32_t numCiftiFiles = static_cast<int32_t>(matrixFiles.size());
    for (int32_t i = 0; i < numCiftiFiles; i++) {
        CiftiMappableConnectivityMatrixDataFile* cmf = matrixFiles[i];
        const DataFileTypeEnum::Enum dataFileType = cmf->getDataFileType();
        
        bool useIt = false;
        switch (dataFileType) {
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                useIt = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                useIt = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                break;
            case DataFileTypeEnum::VOLUME:
                break;
        }
        
        if (useIt) {
            const AString name = cmf->getFileNameNoPath();
            
            if (cmf == s_lastSelectedCiftiFile) {
                defaultIndex = m_ciftiFileComboBox->count();
            }
            
            m_ciftiFileComboBox->addItem(name,
                                         qVariantFromValue((void*)cmf));
        }
    }
    
    /*
     * Disable CIFTI option if no CIFTI files loaded
     */
    if (m_ciftiFileComboBox->count() == 0) {
        ciftiFileLabel->setEnabled(false);
        m_ciftiFileComboBox->setEnabled(false);
        m_ciftiFileComboBox->setEnabled(false);
        ciftiFileRowIndexLabel->setEnabled(false);
        m_ciftiFileRowIndexSpinBox->setEnabled(false);
    }
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 4);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 100);
    gridLayout->setColumnMinimumWidth(0, 20);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_vertexRadioButton, row, 0, 1, 3);
    row++;
    gridLayout->addWidget(vertexStructureLabel, row, 1);
    gridLayout->addWidget(m_vertexStructureComboBox->getWidget(), row, 2);
    row++;
    gridLayout->addWidget(vertexIndexLabel, row, 1);
    gridLayout->addWidget(m_vertexIndexSpinBox, row, 2);
    row++;
    gridLayout->setRowMinimumHeight(row, 10);
    row++;
    gridLayout->addWidget(m_ciftiFileRadioButton, row, 0, 1, 3);
    row++;
    gridLayout->addWidget(ciftiFileLabel, row, 1);
    gridLayout->addWidget(m_ciftiFileComboBox, row, 2);
    row++;
    gridLayout->addWidget(ciftiFileRowIndexLabel, row, 1);
    gridLayout->addWidget(m_ciftiFileRowIndexSpinBox, row, 2);
    
    setCentralWidget(widget);

    switch (s_lastMode) {
        case MODE_CIFTI_ROW:
            m_ciftiFileRadioButton->setChecked(true);
            break;
        case MODE_NONE:
            break;
        case MODE_SURFACE_VERTEX:
            m_vertexRadioButton->setChecked(true);
            break;
    }
}

/**
 * Destructor.
 */
IdentifyBrainordinateDialog::~IdentifyBrainordinateDialog()
{
    
}

/**
 * Gets called when OK button is clicked.
 */
void
IdentifyBrainordinateDialog::okButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    AString errorMessage;
    
    SelectionManager* selectionManager = brain->getSelectionManager();
    selectionManager->reset();
    
    if (m_vertexRadioButton->isChecked()) {
        s_lastMode = MODE_SURFACE_VERTEX;
        s_lastSelectedStructure   = m_vertexStructureComboBox->getSelectedStructure();
        s_lastSelectedVertexIndex = m_vertexIndexSpinBox->value();
        
        BrainStructure* bs = brain->getBrainStructure(s_lastSelectedStructure,
                                                      false);
        if (bs != NULL) {
            if (s_lastSelectedVertexIndex < bs->getNumberOfNodes()) {
                Surface* surface = bs->getVolumeInteractionSurface();
                if (surface != NULL) {
                    SelectionItemSurfaceNode* nodeID = selectionManager->getSurfaceNodeIdentification();
                    nodeID->setBrain(brain);
                    nodeID->setNodeNumber(s_lastSelectedVertexIndex);
                    nodeID->setSurface(surface);
                    const float* fxyz = surface->getCoordinate(s_lastSelectedVertexIndex);
                    const double xyz[3] = { fxyz[0], fxyz[1], fxyz[2] };
                    nodeID->setModelXYZ(xyz);
                    GuiManager::get()->processIdentification(selectionManager,
                                                             this);
                    
//                    m_brainordinateEntryDialogPosition[0] = ded.x();
//                    m_brainordinateEntryDialogPosition[1] = ded.y();
                }
                else {
                    errorMessage = ("PROGRAM ERROR: Volume Interaction Surface not found for structure "
                                    + StructureEnum::toName(s_lastSelectedStructure));
                }
            }
            else {
                errorMessage = ("Vertex Index "
                                + AString::number(s_lastSelectedVertexIndex)
                                + " is out of range.  Maximum vertex index is "
                                + bs->getNumberOfNodes());
            }
        }
        else {
            errorMessage = ("Structure "
                            + StructureEnum::toName(s_lastSelectedStructure)
                            + " not found.");
        }
    }
    else if (m_ciftiFileRadioButton->isChecked()) {
        s_lastMode = MODE_CIFTI_ROW;
        const int indx = m_ciftiFileComboBox->currentIndex();
        if (indx >= 0) {
            void* ptr = m_ciftiFileComboBox->itemData(indx).value<void*>();
            s_lastSelectedCiftiFile = (CiftiMappableConnectivityMatrixDataFile*)ptr;
            
            s_lastSelectedCiftiRowIndex = m_ciftiFileRowIndexSpinBox->value();
            
            StructureEnum::Enum structure;
            int64_t nodeIndex;
            bool valid = s_lastSelectedCiftiFile->getStructureAndNodeIndexFromRowIndex(s_lastSelectedCiftiRowIndex,
                                                                   structure,
                                                                   nodeIndex);
            if (valid) {
                CaretLogSevere(s_lastSelectedCiftiFile->getFileNameNoPath()
                             + " Row index "
                             + AString::number(s_lastSelectedCiftiRowIndex)
                             + " corresponds to surface "
                             + StructureEnum::toName(structure)
                             + " node index "
                             + AString::number(nodeIndex));

                BrainStructure* bs = brain->getBrainStructure(structure,
                                                              false);
                if (bs != NULL) {
                    if (nodeIndex < bs->getNumberOfNodes()) {
                        Surface* surface = bs->getVolumeInteractionSurface();
                        if (surface != NULL) {
                            SelectionItemSurfaceNode* nodeID = selectionManager->getSurfaceNodeIdentification();
                            nodeID->setBrain(brain);
                            nodeID->setNodeNumber(nodeIndex);
                            nodeID->setSurface(surface);
                            const float* fxyz = surface->getCoordinate(nodeIndex);
                            const double xyz[3] = { fxyz[0], fxyz[1], fxyz[2] };
                            nodeID->setModelXYZ(xyz);
                            GuiManager::get()->processIdentification(selectionManager,
                                                                     this);
                        }
                        else {
                            errorMessage = ("PROGRAM ERROR: Volume Interaction Surface not found for structure "
                                            + StructureEnum::toName(s_lastSelectedStructure));
                        }
                    }
                    else {
                        errorMessage = ("Vertex Index "
                                        + AString::number(s_lastSelectedVertexIndex)
                                        + " is out of range.  Maximum vertex index is "
                                        + bs->getNumberOfNodes());
                    }
                }
                else {
                    errorMessage = ("Structure "
                                    + StructureEnum::toName(s_lastSelectedStructure)
                                    + " not found.");
                }
            }
            else {
                int64_t ijk[3];
                float fxyz[3];
                valid = s_lastSelectedCiftiFile->getVoxelIndexAndCoordinateFromRowIndex(s_lastSelectedCiftiRowIndex,
                                                                    ijk,
                                                                    fxyz);
                if (valid) {
                    CaretLogSevere(s_lastSelectedCiftiFile->getFileNameNoPath()
                                 + " Row index "
                                 + AString::number(s_lastSelectedCiftiRowIndex)
                                 + " corresponds to voxel "
                                 + AString::fromNumbers(ijk, 3, ",")
                                 + " at "
                                 + AString::fromNumbers(fxyz, 3, ","));
                    
                    const double xyz[3] = { fxyz[0], fxyz[1], fxyz[2] };
                    
                    SelectionItemVoxel* voxelID = selectionManager->getVoxelIdentification();
                    voxelID->setBrain(brain);
                    voxelID->setVoxelIJK(ijk);
                    voxelID->setModelXYZ(xyz);
                    voxelID->setVolumeFile(s_lastSelectedCiftiFile);
                    GuiManager::get()->processIdentification(selectionManager,
                                                             this);
                }
                else {
                    errorMessage = ("Row index "
                                    + AString::number(s_lastSelectedCiftiRowIndex)
                                    + " is invalid for file "
                                    + s_lastSelectedCiftiFile->getFileNameNoPath());
                }
            }
        }
        else {
            CaretAssert(0);
        }
    }
    else {
        errorMessage = ("Choose "
                        + m_vertexRadioButton->text()
                        + " or "
                        + m_ciftiFileRadioButton->text());
    }
    
    if (errorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}

