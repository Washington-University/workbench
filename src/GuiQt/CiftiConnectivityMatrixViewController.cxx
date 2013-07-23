
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#define __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__
#include "CiftiConnectivityMatrixViewController.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QSignalMapper>

#include "Brain.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberTrajectoryMapProperties.h"
#include "GuiManager.h"
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
 */
CiftiConnectivityMatrixViewController::CiftiConnectivityMatrixViewController(const Qt::Orientation /*orientation*/,
                                                                             QWidget* parent)
: QWidget(parent)
{
    m_gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridLayout, 2, 2);
    m_gridLayout->setColumnStretch(0, 0);
    m_gridLayout->setColumnStretch(1, 0);
    m_gridLayout->setColumnStretch(2, 100);
    const int titleRow = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QLabel("On"),
                            titleRow, COLUMN_ENABLE_CHECKBOX);
    m_gridLayout->addWidget(new QLabel("Copy"),
                            titleRow, COLUMN_COPY_BUTTON);
    m_gridLayout->addWidget(new QLabel("File"),
                            titleRow, COLUMN_NAME_LINE_EDIT);
    
    m_signalMapperFileEnableCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(enabledCheckBoxClicked(int)));
    
    m_signalMapperFileCopyToolButton = new QSignalMapper(this);
    QObject::connect(m_signalMapperFileCopyToolButton, SIGNAL(mapped(int)),
                     this, SLOT(copyToolButtonClicked(int)));
    
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
    
    
    const int32_t numFiles = static_cast<int32_t>(files.size());

    for (int32_t i = 0; i < numFiles; i++) {
        QCheckBox* checkBox = NULL;
        QLineEdit* lineEdit = NULL;
        QToolButton* copyToolButton = NULL;
        
        if (i < static_cast<int32_t>(m_fileEnableCheckBoxes.size())) {
            checkBox = m_fileEnableCheckBoxes[i];
            lineEdit = m_fileNameLineEdits[i];
        }
        else {
            checkBox = new QCheckBox("");
            m_fileEnableCheckBoxes.push_back(checkBox);
            
            lineEdit = new QLineEdit();
            m_fileNameLineEdits.push_back(lineEdit);
            
            copyToolButton = new QToolButton();
            copyToolButton->setText("Copy");
            copyToolButton->setToolTip("Copy loaded row data to a new CIFTI Scalar File");
            m_fileCopyToolButtons.push_back(copyToolButton);
            
            QObject::connect(copyToolButton, SIGNAL(clicked()),
                             m_signalMapperFileCopyToolButton, SLOT(map()));
            m_signalMapperFileCopyToolButton->setMapping(copyToolButton, i);
            
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_signalMapperFileEnableCheckBox, SLOT(map()));
            m_signalMapperFileEnableCheckBox->setMapping(checkBox, i);
            
            const int row = m_gridLayout->rowCount();
            m_gridLayout->addWidget(checkBox,
                                    row, COLUMN_ENABLE_CHECKBOX);
            m_gridLayout->addWidget(copyToolButton,
                                    row, COLUMN_COPY_BUTTON);
            m_gridLayout->addWidget(lineEdit,
                                    row, COLUMN_NAME_LINE_EDIT);
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
        lineEdit->setText(files[i]->getFileName());
    }


    const int32_t numItems = static_cast<int32_t>(m_fileEnableCheckBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        const bool showIt = (i < numFiles);
        
        m_fileEnableCheckBoxes[i]->setVisible(showIt);
        m_fileCopyToolButtons[i]->setVisible(showIt);
        m_fileNameLineEdits[i]->setVisible(showIt);
    }
    
//    m_ciftiConnectivityMatrixDataFile = ciftiConnectivityMatrixFile;
//    if (m_ciftiConnectivityMatrixDataFile != NULL) {
//        Qt::CheckState enabledState = Qt::Unchecked;
//        if (m_ciftiConnectivityMatrixDataFile->isMapDataLoadingEnabled(0)) {
//            enabledState = Qt::Checked;
//        }
//        m_enabledCheckBox->setCheckState(enabledState);
//        
//        m_fileNameLineEdit->setText(m_ciftiConnectivityMatrixDataFile->getFileName());
//        
//    }
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
    
    void* ptr = m_fileEnableCheckBoxes[indx]->property(FILE_POINTER_PROPERTY_NAME).value<void*>();
    CiftiMappableDataFile* mapFilePointer = (CiftiMappableDataFile*)ptr;
    
    CiftiMappableConnectivityMatrixDataFile* matrixFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(mapFilePointer);
    CiftiFiberTrajectoryFile* trajFile = dynamic_cast<CiftiFiberTrajectoryFile*>(mapFilePointer);
    
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
    
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<CiftiMappableConnectivityMatrixDataFile*> files;
    brain->getAllCiftiConnectivityMatrixFiles(files);
    
    CaretAssertVectorIndex(files, indx);
    
    try {
        brain->convertCiftiMatrixFileToCiftiScalarFile(files[indx]);
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    catch (const DataFileException& dfe) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(m_fileCopyToolButtons[indx],
                               dfe.whatString());
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



