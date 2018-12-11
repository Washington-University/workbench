
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

#include <iostream>

#define __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__
#include "CiftiConnectivityMatrixViewController.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QSignalMapper>

#include "Brain.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberTrajectoryMapProperties.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

static const char* FILE_POINTER_PROPERTY_NAME = "filePointer";

/**
 * \class caret::CiftiConnectivityMatrixViewController 
 * \brief View-Controller connectivity files
 * \ingroup GuiQt
 */
/**
 * Constructor.
 *
 * @param parentObjectName
 *    Name of parent object for macros
 * @param parent
 *    The parent widget
 */
CiftiConnectivityMatrixViewController::CiftiConnectivityMatrixViewController(const QString& parentObjectName,
                                                                             QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":Connectivity")
{
    m_gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridLayout, 2, 2);
    m_gridLayout->setColumnStretch(COLUMN_ENABLE_CHECKBOX, 0);
    m_gridLayout->setColumnStretch(COLUMN_LAYER_CHECKBOX, 0);
    m_gridLayout->setColumnStretch(COLUMN_COPY_BUTTON, 0);
    m_gridLayout->setColumnStretch(COLUMN_NAME_LINE_EDIT, 100);
    m_gridLayout->setColumnStretch(COLUMN_ORIENTATION_FILE_COMBO_BOX, 100);
    const int titleRow = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QLabel("Load"),
                            titleRow, COLUMN_ENABLE_CHECKBOX);
    m_gridLayout->addWidget(new QLabel("Layer"),
                            titleRow, COLUMN_LAYER_CHECKBOX);
    m_gridLayout->addWidget(new QLabel("Copy"),
                            titleRow, COLUMN_COPY_BUTTON);
    m_gridLayout->addWidget(new QLabel("Connectivity File"),
                            titleRow, COLUMN_NAME_LINE_EDIT);
    m_gridLayout->addWidget(new QLabel("Fiber Orientation File"),
                            titleRow, COLUMN_ORIENTATION_FILE_COMBO_BOX);
    
    m_signalMapperFileEnableCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(enabledCheckBoxClicked(int)));
    
    m_signalMapperLayerCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperLayerCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(layerCheckBoxClicked(int)));
    
    m_signalMapperFileCopyToolButton = new QSignalMapper(this);
    QObject::connect(m_signalMapperFileCopyToolButton, SIGNAL(mapped(int)),
                     this, SLOT(copyToolButtonClicked(int)));
    
    m_signalMapperFiberOrientationFileComboBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperFiberOrientationFileComboBox, SIGNAL(mapped(int)),
                     this, SLOT(fiberOrientationFileComboBoxActivated(int)));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(m_gridLayout);
    layout->addStretch();
    
    s_allCiftiConnectivityMatrixViewControllers.insert(this);

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixViewController::~CiftiConnectivityMatrixViewController()
{
    s_allCiftiConnectivityMatrixViewControllers.erase(this);
    
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update this view controller.
 */
void 
CiftiConnectivityMatrixViewController::updateViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    std::vector<CaretMappableDataFile*> files;
    
    std::vector<CiftiFiberTrajectoryFile*> trajFiles;
    brain->getConnectivityFiberTrajectoryFiles(trajFiles);
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator trajIter = trajFiles.begin();
         trajIter != trajFiles.end();
         trajIter++) {
        files.push_back(*trajIter);
    }
    
    std::vector<CiftiMappableConnectivityMatrixDataFile*> matrixFiles;
    brain->getAllCiftiConnectivityMatrixFiles(matrixFiles);
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator matrixIter = matrixFiles.begin();
         matrixIter != matrixFiles.end();
         matrixIter++) {
        files.push_back(*matrixIter);
    }

    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    const int32_t numFiles = static_cast<int32_t>(files.size());

    for (int32_t i = 0; i < numFiles; i++) {
        QCheckBox* checkBox = NULL;
        QCheckBox* layerCheckBox = NULL;
        QLineEdit* lineEdit = NULL;
        QToolButton* copyToolButton = NULL;
        QComboBox* comboBox = NULL;
        
        if (i < static_cast<int32_t>(m_fileEnableCheckBoxes.size())) {
            checkBox = m_fileEnableCheckBoxes[i];
            layerCheckBox = m_layerCheckBoxes[i];
            lineEdit = m_fileNameLineEdits[i];
            copyToolButton = m_fileCopyToolButtons[i];
            comboBox = m_fiberOrientationFileComboBoxes[i];
        }
        else {
            
            const QString objectNamePrefix(m_objectNamePrefix
                                           + QString("%1").arg((int)i+1, 2, 10, QLatin1Char('0'))
                                           + ":");
            checkBox = new QCheckBox("");
            checkBox->setToolTip("When selected, load data during\n"
                                 "an identification operation");
            m_fileEnableCheckBoxes.push_back(checkBox);
            checkBox->setObjectName(objectNamePrefix
                                         + "Enable");
            macroManager->addMacroSupportToObject(checkBox);
            
            const AString dynToolTip("This option is enabled only for .dynconn.nii (dynamic connectivity) files.  "
                                     "When checked, this dynamic connectivity file will appear in the Overlay Layers' File selection combo box.  "
                                     "Dynamic connectivity files do not explicitly exist but allow dynamic "
                                     "computation of connectivity from a dense data series (.dtseries) file.  "
                                     "Dynamic connectivity allows one to view connectivity for a brainordinate without creation of an "
                                     "extremely large dense connectivity (.dconn.nii) file.  "
                                     "In Preferences, one may set the default to show/hide .dynconn.nii files.");
            layerCheckBox = new QCheckBox("");
            WuQtUtilities::setWordWrappedToolTip(layerCheckBox, dynToolTip);
            m_layerCheckBoxes.push_back(layerCheckBox);
            layerCheckBox->setObjectName(objectNamePrefix
                                         + "EnableLayer");
            macroManager->addMacroSupportToObject(layerCheckBox);
            
            lineEdit = new QLineEdit();
            lineEdit->setReadOnly(true);
            m_fileNameLineEdits.push_back(lineEdit);
            
            copyToolButton = new QToolButton();
            copyToolButton->setText("Copy");
            copyToolButton->setToolTip("Copy loaded row data to a new CIFTI Scalar File");
            m_fileCopyToolButtons.push_back(copyToolButton);
            copyToolButton->setObjectName(objectNamePrefix
                                         + "CopyButton");
            macroManager->addMacroSupportToObject(copyToolButton);
            
            comboBox = new QComboBox();
            m_fiberOrientationFileComboBoxes.push_back(comboBox);
            comboBox->setToolTip("Select Fiber Orientation File");
            comboBox->setObjectName(objectNamePrefix
                                         + "FiberOrientationFile");
            macroManager->addMacroSupportToObject(comboBox);
            
            QObject::connect(copyToolButton, SIGNAL(clicked()),
                             m_signalMapperFileCopyToolButton, SLOT(map()));
            m_signalMapperFileCopyToolButton->setMapping(copyToolButton, i);
            
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_signalMapperFileEnableCheckBox, SLOT(map()));
            m_signalMapperFileEnableCheckBox->setMapping(checkBox, i);
            
            QObject::connect(layerCheckBox, SIGNAL(clicked(bool)),
                             m_signalMapperLayerCheckBox, SLOT(map()));
            m_signalMapperLayerCheckBox->setMapping(layerCheckBox, i);
            
            QObject::connect(comboBox, SIGNAL(activated(int)),
                             m_signalMapperFiberOrientationFileComboBox, SLOT(map()));
            m_signalMapperFiberOrientationFileComboBox->setMapping(comboBox, i);
            
            const int row = m_gridLayout->rowCount();
            m_gridLayout->addWidget(checkBox,
                                    row, COLUMN_ENABLE_CHECKBOX);
            m_gridLayout->addWidget(layerCheckBox,
                                    row, COLUMN_LAYER_CHECKBOX);
            m_gridLayout->addWidget(copyToolButton,
                                    row, COLUMN_COPY_BUTTON);
            m_gridLayout->addWidget(lineEdit,
                                    row, COLUMN_NAME_LINE_EDIT);
            m_gridLayout->addWidget(comboBox,
                                    row, COLUMN_ORIENTATION_FILE_COMBO_BOX);
        }
        
        const CiftiMappableConnectivityMatrixDataFile* matrixFile = dynamic_cast<const CiftiMappableConnectivityMatrixDataFile*>(files[i]);
        const CiftiFiberTrajectoryFile* trajFile = dynamic_cast<const CiftiFiberTrajectoryFile*>(files[i]);
        
        bool checkStatus = false;
        if (matrixFile != NULL) {
            checkStatus = matrixFile->isMapDataLoadingEnabled(0);
        }
        else if (trajFile != NULL) {
            checkStatus = trajFile->isDataLoadingEnabled();
        }
        else {
            CaretAssertMessage(0, "Has a new file type been added?");
        }
        
        checkBox->setChecked(checkStatus);
        checkBox->setProperty(FILE_POINTER_PROPERTY_NAME,
                              qVariantFromValue((void*)files[i]));

        const CiftiConnectivityMatrixDenseDynamicFile* dynConnFile = dynamic_cast<const CiftiConnectivityMatrixDenseDynamicFile*>(files[i]);
        if (dynConnFile != NULL) {
            layerCheckBox->setChecked(dynConnFile->isEnabledAsLayer());
        }
        else {
            layerCheckBox->setChecked(false);
        }
        
        lineEdit->setText(files[i]->getFileName());  // displayNames[i]);
    }


    const int32_t numItems = static_cast<int32_t>(m_fileEnableCheckBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        bool layerCheckBoxValid = false;
        bool showRow = false;
        bool showOrientationComboBox = false;
        if (i < numFiles) {
            showRow = true;
            if (dynamic_cast<CiftiFiberTrajectoryFile*>(files[i]) != NULL) {
                showOrientationComboBox = true;
            }
            
            if (dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(files[i]) != NULL) {
                layerCheckBoxValid = true;
            }
        }
        
        m_fileEnableCheckBoxes[i]->setVisible(showRow);
        m_layerCheckBoxes[i]->setVisible(showRow);
        m_layerCheckBoxes[i]->setEnabled(layerCheckBoxValid);
        m_fileCopyToolButtons[i]->setVisible(showRow);
        m_fileNameLineEdits[i]->setVisible(showRow);
        m_fiberOrientationFileComboBoxes[i]->setVisible(showOrientationComboBox);
        m_fiberOrientationFileComboBoxes[i]->setEnabled(showOrientationComboBox);
    }
    
    updateFiberOrientationComboBoxes();
}

/**
 * Update the fiber orientation combo boxes.
 */
void
CiftiConnectivityMatrixViewController::updateFiberOrientationComboBoxes()
{
    std::vector<CiftiFiberOrientationFile*> orientationFiles;
    GuiManager::get()->getBrain()->getConnectivityFiberOrientationFiles(orientationFiles);
    const int32_t numOrientationFiles = static_cast<int32_t>(orientationFiles.size());
    
    const int32_t numItems = static_cast<int32_t>(m_fiberOrientationFileComboBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        QComboBox* comboBox = m_fiberOrientationFileComboBoxes[i];
        CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
        CiftiFiberTrajectoryFile* trajFile = NULL;
        
        if (comboBox->isEnabled()) {
            getFileAtIndex(i,
                           matrixFile,
                           trajFile);
        }
        
        if (trajFile != NULL) {
            int32_t selectedIndex = 0;
            CiftiFiberOrientationFile* selectedOrientationFile = trajFile->getMatchingFiberOrientationFile();
            
            comboBox->clear();
            
            for (int32_t iOrient = 0; iOrient < numOrientationFiles; iOrient++) {
                CiftiFiberOrientationFile* orientFile = orientationFiles[iOrient];
                if (trajFile->isFiberOrientationFileCombatible(orientFile)) {
                    if (orientFile == selectedOrientationFile) {
                        selectedIndex = iOrient;
                    }
                    
                    comboBox->addItem(orientFile->getFileNameNoPath(),
                                      qVariantFromValue((void*)orientFile));
                }
            }
            
            if ((selectedIndex >= 0)
                && (selectedIndex < comboBox->count())) {
                comboBox->setCurrentIndex(selectedIndex);
            }
        }
        else {
            comboBox->clear();
        }
        
//        const bool showComboBox = (trajFile != NULL);
//        m_fiberOrientationFileComboBoxes[i]->setVisible(showComboBox);
//        m_fiberOrientationFileComboBoxes[i]->setEnabled(showComboBox);
//        std::cout << "Show Orientation File Combo Box: "
//        << i
//        << ": "
//        << qPrintable(AString::fromBool(showComboBox))
//        << std::endl;
    }
}

/**
 * Called when an enabled check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
CiftiConnectivityMatrixViewController::enabledCheckBoxClicked(int indx)
{
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    const bool newStatus = m_fileEnableCheckBoxes[indx]->isChecked();
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiFiberTrajectoryFile* trajFile = NULL;
    
    getFileAtIndex(indx,
                   matrixFile,
                   trajFile);
    
    if (matrixFile != NULL) {
        matrixFile->setMapDataLoadingEnabled(0,
                                             newStatus);
    }
    else if (trajFile != NULL) {
        trajFile->setDataLoadingEnabled(newStatus);
    }
    else {
        CaretAssertMessage(0, "Has a new file type been added?");
    }
    
    updateOtherCiftiConnectivityMatrixViewControllers();
}

/**
 * Called when a layer check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
CiftiConnectivityMatrixViewController::layerCheckBoxClicked(int indx)
{
    CaretAssertVectorIndex(m_layerCheckBoxes, indx);
    const bool newStatus = m_layerCheckBoxes[indx]->isChecked();
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiFiberTrajectoryFile* trajFile = NULL;
    
    getFileAtIndex(indx,
                   matrixFile,
                   trajFile);
    
    if (matrixFile != NULL) {
        CiftiConnectivityMatrixDenseDynamicFile* dynConnFile = dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(matrixFile);
        if (dynConnFile != NULL) {
            dynConnFile->setEnabledAsLayer(newStatus);
        }
    }
    else if (trajFile != NULL) {
        CaretAssertMessage(0, "Should never get caled for fiber trajectory file");
    }
    else {
        CaretAssertMessage(0, "Has a new file type been added?");
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    //updateOtherCiftiConnectivityMatrixViewControllers();
}

/**
 * Get the file associated with the given index.  One of the output files
 * will be NULL and the other will be non-NULL.
 *
 * @param indx
 *    The index.
 * @param ciftiMatrixFileOut
 *    If there is a CIFTI matrix file at the given index, this will be non-NULL.
 * @param ciftiTrajFileOut
 *    If there is a CIFTI trajectory file at the given index, this will be non-NULL.
 */
void
CiftiConnectivityMatrixViewController::getFileAtIndex(const int32_t indx,
                                                      CiftiMappableConnectivityMatrixDataFile* &ciftiMatrixFileOut,
                                                      CiftiFiberTrajectoryFile* &ciftiTrajFileOut)
{
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    void* ptr = m_fileEnableCheckBoxes[indx]->property(FILE_POINTER_PROPERTY_NAME).value<void*>();
    CaretMappableDataFile* mapFilePointer = (CaretMappableDataFile*)ptr;
    
    ciftiMatrixFileOut = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(mapFilePointer);
    ciftiTrajFileOut   = dynamic_cast<CiftiFiberTrajectoryFile*>(mapFilePointer);
    
    AString name = "";
    if (mapFilePointer != NULL) {
        name = mapFilePointer->getFileNameNoPath();
    }

//    std::cout << "File at index: "
//    << indx
//    << " name: "
//    << qPrintable(name)
//    << " cifti-matrix-ptr: "
//    << (long)ciftiMatrixFileOut
//    << " cifti-traj-ptr: "
//    << (long)ciftiTrajFileOut
//    << std::endl;
    
    if (ciftiMatrixFileOut != NULL) {
        /* OK */
    }
    else if (ciftiTrajFileOut != NULL) {
        /* OK */
    }
    else {
        CaretAssertMessage(0,
                           "Has a new file type been added?");
    }
}

/**
 * Called when fiber orientation file combo box changed.
 *
 * @param indx
 *    Index of combo box that was changed.
 */
void
CiftiConnectivityMatrixViewController::fiberOrientationFileComboBoxActivated(int indx)
{
    CaretAssertVectorIndex(m_fiberOrientationFileComboBoxes, indx);
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiFiberTrajectoryFile* trajFile = NULL;
    
    getFileAtIndex(indx,
                   matrixFile,
                   trajFile);
    
    CaretAssertMessage(trajFile,
                       "Selected orientation file but trajectory file is invalid.");
    
    QComboBox* cb = m_fiberOrientationFileComboBoxes[indx];
    void* ptr = cb->itemData(indx,
                             Qt::UserRole).value<void*>();
    CaretAssert(ptr);
    CiftiFiberOrientationFile* orientFile = (CiftiFiberOrientationFile*)ptr;
    
    std::vector<CiftiFiberOrientationFile*> orientationFiles;
    GuiManager::get()->getBrain()->getConnectivityFiberOrientationFiles(orientationFiles);
    if (std::find(orientationFiles.begin(),
                  orientationFiles.end(),
                  orientFile) == orientationFiles.end()) {
        CaretAssertMessage(0,
                           "Selected fiber orientation file is no longer valid.");
        return;
    }
    
    trajFile->setMatchingFiberOrientationFile(orientFile);
    
    updateOtherCiftiConnectivityMatrixViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Called when copy tool button is clicked.
 *
 * @param indx
 *    Index of copy tool button that was clicked.
 */
void
CiftiConnectivityMatrixViewController::copyToolButtonClicked(int indx)
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = NULL;
    CiftiFiberTrajectoryFile* trajFile = NULL;
    
    getFileAtIndex(indx,
                   matrixFile,
                   trajFile);
    
    
    bool errorFlag = false;
    AString errorMessage;
    
    const AString directoryName = GuiManager::get()->getBrain()->getCurrentDirectory();
    if (matrixFile != NULL) {
        CiftiBrainordinateScalarFile* scalarFile =
        CiftiBrainordinateScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(matrixFile,
                                                                                      directoryName,
                                                                                      errorMessage);
        if (scalarFile != NULL) {
            EventDataFileAdd dataFileAdd(scalarFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else if (trajFile != NULL) {
        CiftiFiberTrajectoryFile* newTrajFile = trajFile->newFiberTrajectoryFileFromLoadedRowData(directoryName,
                                                                                                  errorMessage);

        if (newTrajFile != NULL) {
            EventDataFileAdd dataFileAdd(newTrajFile);
            EventManager::get()->sendEvent(dataFileAdd.getPointer());
            
            if (dataFileAdd.isError()) {
                errorMessage = dataFileAdd.getErrorMessage();
                errorFlag = true;
            }
        }
        else {
            errorFlag = true;
        }
    }
    else {
        CaretAssertMessage(0,
                           "Has a new file type been added?");
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    cursor.restoreCursor();
    
    if (errorFlag) {
        WuQMessageBox::errorOk(m_fileCopyToolButtons[indx],
                               errorMessage);
    }
}

///**
// * Update graphics and GUI after
// */
//void 
//CiftiConnectivityMatrixViewController::updateUserInterfaceAndGraphicsWindow()
//{
//    updateOtherCiftiConnectivityMatrixViewControllers();
//    
//    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
//    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//}

/**
 * Update other connectivity view controllers other than 'this' instance
 * that contain the same connectivity file.
 */
void 
CiftiConnectivityMatrixViewController::updateOtherCiftiConnectivityMatrixViewControllers()
{
    for (std::set<CiftiConnectivityMatrixViewController*>::iterator iter = s_allCiftiConnectivityMatrixViewControllers.begin();
         iter != s_allCiftiConnectivityMatrixViewControllers.end();
         iter++) {
        CiftiConnectivityMatrixViewController* clvc = *iter;
        if (clvc != this) {
            clvc->updateViewController();
        }
    }
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
CiftiConnectivityMatrixViewController::receiveEvent(Event* event)
{
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
//        if (uiEvent->isUpdateForWindow(this->browserWindowIndex)) {
            if (uiEvent->isConnectivityUpdate()
                || uiEvent->isToolBoxUpdate()) {
                this->updateViewController();
                uiEvent->setEventProcessed();
            }
//        }
    }
}



