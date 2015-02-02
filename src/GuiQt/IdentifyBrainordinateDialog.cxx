
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
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
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
: WuQDialogNonModal("Identify Brainordinate",
                    parent)
{
    const int INDEX_SPIN_BOX_WIDTH = 100;
    
    /*
     * Surface Vertex widgets
     */
    m_vertexRadioButton = new QRadioButton("Surface Vertex");
    
    m_vertexStructureLabel = new QLabel("Structure");
    m_vertexStructureComboBox = new StructureEnumComboBox(this);
    m_vertexStructureComboBox->listOnlyValidStructures();
    
    m_vertexIndexLabel = new QLabel("Vertex Index");
    m_vertexIndexSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0,
                                                                std::numeric_limits<int>::max(),
                                                                1);
    m_vertexIndexSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    
    /*
     * CIFTI file row widgets
     */
    m_ciftiFileRadioButton = new QRadioButton("Cifti File Row");
    
    std::vector<DataFileTypeEnum::Enum> allDataFileTypes;
    DataFileTypeEnum::getAllEnums(allDataFileTypes);
    
    std::vector<DataFileTypeEnum::Enum> supportedDataFileTypes;

    for (std::vector<DataFileTypeEnum::Enum>::iterator dtIter = allDataFileTypes.begin();
         dtIter != allDataFileTypes.end();
         dtIter++) {
        const DataFileTypeEnum::Enum dataFileType = *dtIter;
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
            supportedDataFileTypes.push_back(dataFileType);
        }
    }
    
    /*
     *
     */
    m_ciftiFileLabel = new QLabel("File");
    m_ciftiFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(GuiManager::get()->getBrain(),
                                                                                             supportedDataFileTypes);
    m_ciftiFileComboBox = new CaretDataFileSelectionComboBox(this);
    
    m_ciftiFileRowIndexLabel = new QLabel("Row Index");
    m_ciftiFileRowIndexSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0,
                                                                      std::numeric_limits<int>::max(),
                                                                      1);
    m_ciftiFileRowIndexSpinBox->setFixedWidth(INDEX_SPIN_BOX_WIDTH);
    
    /*
     * Button group to keep radio buttons mutually exclusive
     */
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_vertexRadioButton);
    buttGroup->addButton(m_ciftiFileRadioButton);
    
    
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
    gridLayout->addWidget(m_vertexStructureLabel, row, 1);
    gridLayout->addWidget(m_vertexStructureComboBox->getWidget(), row, 2);
    row++;
    gridLayout->addWidget(m_vertexIndexLabel, row, 1);
    gridLayout->addWidget(m_vertexIndexSpinBox, row, 2, Qt::AlignLeft);
    row++;
    gridLayout->setRowMinimumHeight(row, 10);
    row++;
    gridLayout->addWidget(m_ciftiFileRadioButton, row, 0, 1, 3);
    row++;
    gridLayout->addWidget(m_ciftiFileLabel, row, 1);
    gridLayout->addWidget(m_ciftiFileComboBox->getWidget(), row, 2);
    row++;
    gridLayout->addWidget(m_ciftiFileRowIndexLabel, row, 1);
    gridLayout->addWidget(m_ciftiFileRowIndexSpinBox, row, 2, Qt::AlignLeft);
    
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);

    updateDialog();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
IdentifyBrainordinateDialog::~IdentifyBrainordinateDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
IdentifyBrainordinateDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();
        updateDialog();
    }
}

/**
 * Update the dialog.
 */
void
IdentifyBrainordinateDialog::updateDialog()
{
    m_ciftiFileComboBox->updateComboBox(m_ciftiFileSelectionModel);
    
    //m_vertexStructureComboBox->setSelectedStructure(s_lastSelectedStructure);

    /*
     * Disable surface option if no surfaces loaded
     */
    if (m_vertexStructureComboBox->count() == 0) {
        m_vertexRadioButton->setEnabled(false);
        m_vertexStructureLabel->setEnabled(false);
        m_vertexStructureComboBox->getWidget()->setEnabled(false);
        m_vertexIndexLabel->setEnabled(false);
        m_vertexIndexSpinBox->setEnabled(false);
    }
    
    /*
     * Disable CIFTI option if no CIFTI files loaded
     */
    if (m_ciftiFileSelectionModel->getAvailableFiles().empty()) {
        m_ciftiFileRadioButton->setEnabled(false);
        m_ciftiFileLabel->setEnabled(false);
        m_ciftiFileComboBox->getWidget()->setEnabled(false);
        m_ciftiFileRowIndexLabel->setEnabled(false);
        m_ciftiFileRowIndexSpinBox->setEnabled(false);
    }
}


/**
 * Gets called when Apply button is clicked.
 */
void
IdentifyBrainordinateDialog::applyButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    AString errorMessage;
    
    SelectionManager* selectionManager = brain->getSelectionManager();
    selectionManager->reset();
    
    if (m_vertexRadioButton->isChecked()) {
        const StructureEnum::Enum selectedStructure   = m_vertexStructureComboBox->getSelectedStructure();
        const int32_t selectedVertexIndex = m_vertexIndexSpinBox->value();
        
        BrainStructure* bs = brain->getBrainStructure(selectedStructure,
                                                      false);
        if (bs != NULL) {
            if (selectedVertexIndex < bs->getNumberOfNodes()) {
                Surface* surface = bs->getVolumeInteractionSurface();
                if (surface != NULL) {
                    SelectionItemSurfaceNode* nodeID = selectionManager->getSurfaceNodeIdentification();
                    nodeID->setBrain(brain);
                    nodeID->setNodeNumber(selectedVertexIndex);
                    nodeID->setSurface(surface);
                    const float* fxyz = surface->getCoordinate(selectedVertexIndex);
                    const double xyz[3] = { fxyz[0], fxyz[1], fxyz[2] };
                    nodeID->setModelXYZ(xyz);
                    GuiManager::get()->processIdentification(-1,
                                                             selectionManager,
                                                             this);
                }
                else {
                    errorMessage = ("PROGRAM ERROR: Volume Interaction Surface not found for structure "
                                    + StructureEnum::toName(selectedStructure));
                }
            }
            else {
                errorMessage = ("Vertex Index "
                                + AString::number(selectedVertexIndex)
                                + " is out of range.  Maximum vertex index is "
                                + AString::number(bs->getNumberOfNodes()));
            }
        }
        else {
            errorMessage = ("Structure "
                            + StructureEnum::toName(selectedStructure)
                            + " not found.");
        }
    }
    else if (m_ciftiFileRadioButton->isChecked()) {
        CaretDataFile* dataFile = m_ciftiFileSelectionModel->getSelectedFile();
        if (dataFile != NULL) {
            CiftiMappableDataFile*    ciftiMapFile  = dynamic_cast<CiftiMappableDataFile*>(dataFile);
            CiftiFiberTrajectoryFile* ciftiTrajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(dataFile);
            
            const int32_t selectedCiftiRowIndex = m_ciftiFileRowIndexSpinBox->value();
            
            try {
                StructureEnum::Enum surfaceStructure;
                int32_t surfaceNodeIndex;
                int32_t surfaceNumberOfNodes;
                bool surfaceNodeValid = false;
                int64_t voxelIJK[3];
                float voxelXYZ[3];
                bool voxelValid = false;
                if (ciftiMapFile != NULL) {
                    ciftiMapFile->getBrainordinateFromRowIndex(selectedCiftiRowIndex,
                                                               surfaceStructure,
                                                               surfaceNodeIndex,
                                                               surfaceNumberOfNodes,
                                                               surfaceNodeValid,
                                                               voxelIJK,
                                                               voxelXYZ,
                                                               voxelValid);
                }
                else if (ciftiTrajFile != NULL) {
                    ciftiTrajFile->getBrainordinateFromRowIndex(selectedCiftiRowIndex,
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
    
    WuQDialogNonModal::applyButtonClicked();
}

