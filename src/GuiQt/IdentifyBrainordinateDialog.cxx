
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
#include "CiftiFiberTrajectoryFile.h"
#include "CaretMappableDataFile.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryManager.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

#include <limits>

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
    std::vector<CaretMappableDataFile*> caretMappableFiles;
    brain->getAllMappableDataFiles(caretMappableFiles);
    
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
    const int32_t numCiftiFiles = static_cast<int32_t>(caretMappableFiles.size());
    for (int32_t i = 0; i < numCiftiFiles; i++) {
        CaretMappableDataFile* mapFile = caretMappableFiles[i];
        const DataFileTypeEnum::Enum dataFileType = mapFile->getDataFileType();
        
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
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                useIt = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                useIt = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                useIt = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::IMAGE:
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
            const AString name = mapFile->getFileNameNoPath();
            
            if (mapFile == s_lastSelectedCaretMappableDataFile) {
                defaultIndex = m_ciftiFileComboBox->count();
            }
            
            m_ciftiFileComboBox->addItem(name,
                                         qVariantFromValue((void*)mapFile));
        }
    }
    
    if (defaultIndex >= 0) {
        m_ciftiFileComboBox->setCurrentIndex(defaultIndex);
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
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 4);
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
    
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);

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
                    GuiManager::get()->processIdentification(-1,
                                                             selectionManager,
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
                                + AString::number(bs->getNumberOfNodes()));
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
            s_lastSelectedCaretMappableDataFile = (CaretMappableDataFile*)ptr;
            
            s_lastSelectedCiftiRowIndex = m_ciftiFileRowIndexSpinBox->value();
            
            CiftiFiberTrajectoryFile* ciftiTrajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(s_lastSelectedCaretMappableDataFile);
            CiftiMappableDataFile* ciftiMapFile = dynamic_cast<CiftiMappableDataFile*>(s_lastSelectedCaretMappableDataFile);
            
                try {
                    StructureEnum::Enum surfaceStructure;
                    int32_t surfaceNodeIndex;
                    int32_t surfaceNumberOfNodes;
                    bool surfaceNodeValid;
                    int64_t voxelIJK[3];
                    float voxelXYZ[3];
                    bool voxelValid;
                    if (ciftiMapFile != NULL) {
                        ciftiMapFile->getBrainordinateFromRowIndex(s_lastSelectedCiftiRowIndex,
                                                                   surfaceStructure,
                                                                   surfaceNodeIndex,
                                                                   surfaceNumberOfNodes,
                                                                   surfaceNodeValid,
                                                                   voxelIJK,
                                                                   voxelXYZ,
                                                                   voxelValid);
                    }
                    else if (ciftiTrajFile != NULL) {
                        ciftiTrajFile->getBrainordinateFromRowIndex(s_lastSelectedCiftiRowIndex,
                                                                   surfaceStructure,
                                                                   surfaceNodeIndex,
                                                                   surfaceNumberOfNodes,
                                                                   surfaceNodeValid,
                                                                   voxelIJK,
                                                                   voxelXYZ,
                                                                   voxelValid);
                    }
                    else {
                        errorMessage = "Neither CIFTI Mappable nor CIFTI Trajectory file.  Has new file type been added?";
                    }
                    
                    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
                    idManager->reset();
                    idManager->setAllSelectionsEnabled(true);
                    
                    if (surfaceNodeValid) {
                        SelectionItemSurfaceNode* surfaceID = idManager->getSurfaceNodeIdentification();
                        const Surface* surface = brain->getVolumeInteractionSurfaceForStructure(surfaceStructure);
                        if (surface != NULL) {
                            if ((surfaceNodeIndex >= 0)
                                && (surfaceNodeIndex < surface->getNumberOfNodes())) {
                                surfaceID->setSurface(const_cast<Surface*>(surface));
                                surfaceID->setBrain(brain);
                                const float* xyz = surface->getCoordinate(surfaceNodeIndex);
                                const double doubleXYZ[3] = { xyz[0], xyz[1], xyz[2] };
                                surfaceID->setModelXYZ(doubleXYZ);
                                surfaceID->setNodeNumber(surfaceNodeIndex);
                                
                                GuiManager::get()->processIdentification(-1, // invalid tab index
                                                                         selectionManager,
                                                                         this);
                            }
                            else {
                                errorMessage = ("Surface vertex index "
                                                + AString::number(surfaceNodeIndex)
                                                + " is not valid for surface "
                                                + surface->getFileNameNoPath());
                            }
                        }
                        else{
                            errorMessage = ("No surfaces are loaded for structure "
                                            + StructureEnum::toGuiName(surfaceStructure));
                        }
                        
                    }
                    else if (voxelValid) {
                        SelectionItemVoxel* voxelID = idManager->getVoxelIdentification();
                        voxelID->setBrain(brain);
                        voxelID->setEnabledForSelection(true);
                        voxelID->setVoxelIdentification(brain,
                                                        ciftiMapFile,
                                                        voxelIJK,
                                                        0.0);
                        const double doubleXYZ[3] = { voxelXYZ[0], voxelXYZ[1], voxelXYZ[2] };
                        voxelID->setModelXYZ(doubleXYZ);
                        
                        GuiManager::get()->processIdentification(-1, // invalid tab index
                                                                 selectionManager,
                                                                 this);
                    }
                }
                catch (const DataFileException& dfe) {
                    errorMessage = dfe.whatString();
                }
//            }
//            else if (trajFile != NULL) {
//                try {
//                    trajFile->loadDataForRowIndex(s_lastSelectedCiftiRowIndex);
//                }
//                catch (const DataFileException& dfe) {
//                    errorMessage = ("Error loading data for row "
//                                    + AString::number(s_lastSelectedCiftiRowIndex)
//                                    + ": "
//                                    + dfe.whatString());
//                }
//            }
//            else {
//                CaretAssertMessage(0,
//                                   "Neither matrix nor trajectory file.  Has new file type been added?");
//            }
            
//            if (errorMessage.isEmpty()) {
//                if (ciftiMapFile != NULL) {
//                    try {
//                        StructureEnum::Enum surfaceStructure;
//                        int32_t surfaceNodeIndex;
//                        int32_t surfaceNumberOfNodes;
//                        bool surfaceNodeValid;
//                        int64_t voxelIJK[3];
//                        float voxelXYZ[3];
//                        bool voxelValid;
//                        ciftiMapFile->getBrainordinateFromRowIndex(s_lastSelectedCiftiRowIndex,
//                                                                   surfaceStructure,
//                                                                   surfaceNodeIndex,
//                                                                   surfaceNumberOfNodes,
//                                                                   surfaceNodeValid,
//                                                                   voxelIJK,
//                                                                   voxelXYZ,
//                                                                   voxelValid);
//                        
//                        if (surfaceNodeValid) {
//                            IdentifiedItemNode* identifiedNode = new IdentifiedItemNode("",
//                                                                                        surfaceStructure,
//                                                                                        surfaceNumberOfNodes,
//                                                                                        surfaceNodeIndex);
//                            IdentificationManager* idManager = brain->getIdentificationManager();
//                            idManager->addIdentifiedItem(identifiedNode);
//                            
//                            informationMessage.appendWithNewLine(ciftiMapFile->getFileNameNoPath()
//                                                                 + " node index="
//                                                                 + AString::number(surfaceNodeIndex)
//                                                                 + " row index="
//                                                                 + AString::number(s_lastSelectedCiftiRowIndex)
//                                                                 + " structure="
//                                                                 + StructureEnum::toGuiName(surfaceStructure));
//                        }
//                        else if (voxelValid) {
//                            informationMessage.appendWithNewLine(ciftiMapFile->getFileNameNoPath()
//                                                                 + " voxel IJK=("
//                                                                 + AString::fromNumbers(voxelIJK, 3, ",")
//                                                                 + ") XYZ=("
//                                                                 + AString::fromNumbers(voxelXYZ, 3, ",")
//                                                                 + ") row index="
//                                                                 + AString::number(s_lastSelectedCiftiRowIndex));
//                            
//                            EventIdentificationHighlightLocation idLocation(-1, // ALL tabs,
//                                                                            voxelXYZ,
//                                                                            EventIdentificationHighlightLocation::LOAD_FIBER_ORIENTATION_SAMPLES_MODE_NO);
//                            EventManager::get()->sendEvent(idLocation.getPointer());
//                        }
//                    }
//                    catch (const DataFileException& dfe) {
//                        errorMessage = dfe.whatString();
//                    }
//                }
//            }
//            
//            if ( ! informationMessage.isEmpty()) {
//                IdentifiedItem* textItem = new IdentifiedItem();
//                textItem->appendText(informationMessage);
//                IdentificationManager* idManager = brain->getIdentificationManager();
//                idManager->addIdentifiedItem(textItem);
//                EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
//            }
//        }
        }
    }
    else {
        errorMessage = ("Choose "
                        + m_vertexRadioButton->text()
                        + " or "
                        + m_ciftiFileRadioButton->text());
    }
    
//    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
//    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addToolBar().addToolBox().getPointer());
    
    if (errorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}

