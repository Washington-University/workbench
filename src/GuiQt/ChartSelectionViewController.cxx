
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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

#define __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartSelectionViewController.h"
#undef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSignalMapper>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixInterface.h"
#include "ChartableMatrixFileSelectionModel.h"
#include "ChartModel.h"
#include "ChartableBrainordinateInterface.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "WuQtUtilities.h"

using namespace caret;

static const char* BRAINORDINATE_FILE_POINTER_PROPERTY_NAME = "brainordinateFilePointer";
static const char* MATRIX_FILE_POINTER_PROPERTY_NAME        = "matrixFilePointer";

/**
 * \class caret::ChartSelectionViewController 
 * \brief Handles selection of charts displayed in chart model.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartSelectionViewController::ChartSelectionViewController(const Qt::Orientation /*orientation*/,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_mode = MODE_INVALID;
    
    m_selectionRadioButtonGroup = new QButtonGroup(this);
    QObject::connect(m_selectionRadioButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(selectionRadioButtonClicked(int)));
    
    /*
     * In the grid layout, there are columns for the checkboxes (used
     * for brainordinate charts) and radio buttons (used for matrix 
     * charts).   Display of checkboxes and radiobuttons is mutually
     * exclusive.  The "Select" column title is over both the checkbox
     * and radio button columns.
     */
    m_gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridLayout, 4, 2);
    m_gridLayout->setColumnStretch(COLUMN_CHECKBOX, 0);
    m_gridLayout->setColumnStretch(COLUMN_RADIOBUTTON, 0);
    m_gridLayout->setColumnStretch(COLUMN_LINE_EDIT, 100);
    const int titleRow = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QLabel("Select"),
                            titleRow, COLUMN_CHECKBOX,
                            1, 2, /* 1 row, 2 columns (checkbox and radio button) */
                            Qt::AlignHCenter);
    m_gridLayout->addWidget(new QLabel("Charting File"),
                            titleRow, COLUMN_LINE_EDIT,
                            Qt::AlignHCenter);
    
    m_signalMapperFileEnableCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(selectionCheckBoxClicked(int)));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(m_gridLayout);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartSelectionViewController::~ChartSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the view controller.
 */
void
ChartSelectionViewController::updateSelectionViewController()
{
    m_mode = MODE_INVALID;
    
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

//    ChartModel* chartModel = NULL;
    
    ChartDataTypeEnum::Enum chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartDataType = modelChart->getSelectedChartDataType(browserTabIndex);
//        chartModel = modelChart->getSelectedChartModel(browserTabIndex);
//        if (chartModel != NULL) {
//            chartDataType = chartModel->getChartDataType();
//        }
    }
    
    switch (chartDataType) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            m_mode = MODE_BRAINORDINATE;
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            m_mode = MODE_MATRIX;
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            m_mode = MODE_BRAINORDINATE;
            break;
    }
    
    std::vector<ChartableBrainordinateInterface*> chartableBrainordinateFilesVector;
    
    std::vector<ChartableMatrixInterface*> chartableMatrixFilesVector;
    
    ChartableMatrixFileSelectionModel* chartableMatrixFileSelectionModel = NULL;
    ChartableMatrixInterface* selectedChartMatrixFile = NULL;
    
    int32_t numChartableFiles = 0;
    switch (m_mode) {
        case MODE_INVALID:
            break;
        case MODE_BRAINORDINATE:
            brain->getAllChartableBrainordinateDataFilesForChartDataType(chartDataType,
                                                                         chartableBrainordinateFilesVector);
            numChartableFiles = static_cast<int32_t>(chartableBrainordinateFilesVector.size());
            break;
        case MODE_MATRIX:
            if (modelChart != NULL) {
                chartableMatrixFileSelectionModel = modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
                chartableMatrixFilesVector = chartableMatrixFileSelectionModel->getAvailableFiles();
                numChartableFiles = static_cast<int32_t>(chartableMatrixFilesVector.size());
                selectedChartMatrixFile = chartableMatrixFileSelectionModel->getSelectedFile();
            }
            break;
    }
    
    for (int32_t i = 0; i < numChartableFiles; i++) {
        QCheckBox* checkBox = NULL;
        QLineEdit* lineEdit = NULL;
        QRadioButton* radioButton = NULL;
        
        if (i < static_cast<int32_t>(m_fileEnableCheckBoxes.size())) {
            radioButton = m_fileSelectionRadioButtons[i];
            checkBox    = m_fileEnableCheckBoxes[i];
            lineEdit    = m_fileNameLineEdits[i];
        }
        else {
            checkBox = new QCheckBox("");
            m_fileEnableCheckBoxes.push_back(checkBox);
            
            radioButton = new QRadioButton("");
            m_fileSelectionRadioButtons.push_back(radioButton);
            m_selectionRadioButtonGroup->addButton(radioButton, i);
            
            lineEdit = new QLineEdit();
            lineEdit->setReadOnly(true);
            m_fileNameLineEdits.push_back(lineEdit);
            
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_signalMapperFileEnableCheckBox, SLOT(map()));
            m_signalMapperFileEnableCheckBox->setMapping(checkBox, i);
            
            const int row = m_gridLayout->rowCount();
            m_gridLayout->addWidget(checkBox,
                                    row, COLUMN_CHECKBOX,
                                    Qt::AlignRight);
            m_gridLayout->addWidget(radioButton,
                                    row, COLUMN_RADIOBUTTON,
                                    Qt::AlignLeft);
            m_gridLayout->addWidget(lineEdit,
                                    row, COLUMN_LINE_EDIT);
        }
        
        QVariant brainordinateFilePointerVariant;
        QVariant matrixFilePointerVariant;
        CaretMappableDataFile* caretMappableDataFile = NULL;
        bool checkBoxStatus = false;
        bool radioButtonStatus = false;
        
        switch (m_mode) {
            case MODE_INVALID:
                break;
            case MODE_BRAINORDINATE:
            {
                CaretAssertVectorIndex(chartableBrainordinateFilesVector, i);
                ChartableBrainordinateInterface* chartBrainFile = chartableBrainordinateFilesVector[i];
                CaretAssert(chartBrainFile);
                checkBoxStatus = chartBrainFile->isChartingEnabled(browserTabIndex);

                brainordinateFilePointerVariant = qVariantFromValue((void*)chartBrainFile);
                
                caretMappableDataFile = chartBrainFile->getCaretMappableDataFile();
            }
                break;
            case MODE_MATRIX:
            {
                CaretAssertVectorIndex(chartableMatrixFilesVector, i);
                ChartableMatrixInterface* chartMatrixFile = chartableMatrixFilesVector[i];
                CaretAssert(chartMatrixFile);
                
                if (chartMatrixFile == selectedChartMatrixFile) {
                    radioButtonStatus = true;
                }

                matrixFilePointerVariant = qVariantFromValue((void*)chartMatrixFile);
                
                caretMappableDataFile = chartMatrixFile->getCaretMappableDataFile();
            }
                break;
        }
        
        checkBox->blockSignals(true);
        checkBox->setChecked(checkBoxStatus);
        checkBox->blockSignals(false);
        
        checkBox->setProperty(BRAINORDINATE_FILE_POINTER_PROPERTY_NAME,
                              brainordinateFilePointerVariant);
        
        /*
         * Only "set" radio button.
         * button group will ensure mutual exclusion.
         */
        if (radioButtonStatus) {
            radioButton->blockSignals(true);
            radioButton->setChecked(radioButtonStatus);
            radioButton->blockSignals(false);
        }
        radioButton->setProperty(MATRIX_FILE_POINTER_PROPERTY_NAME,
                              matrixFilePointerVariant);
        
        CaretAssert(caretMappableDataFile);
        lineEdit->setText(caretMappableDataFile->getFileName());
    }
    
    
    const int32_t numItems = static_cast<int32_t>(m_fileEnableCheckBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        bool showCheckBox    = false;
        bool showLineEdit    = false;
        bool showRadioButton = false;
        
        if (i < numChartableFiles) {
            showLineEdit = true;
            switch (m_mode) {
                case MODE_INVALID:
                    break;
                case MODE_BRAINORDINATE:
                    showCheckBox = true;
                    break;
                case MODE_MATRIX:
                    showRadioButton = true;
                    break;
            }
        }
        
        m_fileEnableCheckBoxes[i]->setVisible(showCheckBox);
        m_fileNameLineEdits[i]->setVisible(showLineEdit);
        m_fileSelectionRadioButtons[i]->setVisible(showRadioButton);
    }
}

/**
 * Called when an enabled check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
ChartSelectionViewController::selectionCheckBoxClicked(int indx)
{
    switch (m_mode) {
        case MODE_INVALID:
            CaretAssertMessage(0, "Checkbox should never be clicked when mode is invalid.");
            return;
            break;
        case MODE_BRAINORDINATE:
            break;
        case MODE_MATRIX:
            CaretAssertMessage(0, "Checkbox should never be clicked when mode is matrix.");
            return;
            break;
    }
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    const bool newStatus = m_fileEnableCheckBoxes[indx]->isChecked();
    
    ChartableBrainordinateInterface* chartFile = getBrainordinateFileAtIndex(indx);
    CaretAssert(chartFile);

    if (chartFile != NULL) {
        chartFile->setChartingEnabled(browserTabIndex,
                                      newStatus);
    }
}

/**
 * Called when a selection radio button is clicked.
 *
 * @param indx
 *    Index of radio that was clicked.
 */
void
ChartSelectionViewController::selectionRadioButtonClicked(int indx)
{
    switch (m_mode) {
        case MODE_INVALID:
            CaretAssertMessage(0, "Radiobutton should never be clicked when mode is invalid.");
            return;
            break;
        case MODE_BRAINORDINATE:
            CaretAssertMessage(0, "Radiobutton should never be clicked when mode is matrix.");
            return;
            break;
        case MODE_MATRIX:
            break;
    }
    
    ChartableMatrixInterface* chartMatrixFile = getMatrixFileAtIndex(indx);
    CaretAssert(chartMatrixFile);
    
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        ChartableMatrixFileSelectionModel* fileSelector =
           modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
        fileSelector->setSelectedFile(chartMatrixFile);
    }
}

/**
 * Get the brainordinate file associated with the given index.
 *
 * @param indx
 *    The index.
 * @return 
 *    Brainordinate chartable file associated with the given index or NULL
 *    if not valid
 */
ChartableBrainordinateInterface*
ChartSelectionViewController::getBrainordinateFileAtIndex(const int32_t indx)
{
    ChartableBrainordinateInterface* filePointer = NULL;
    
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    const QVariant filePointerVariant = m_fileEnableCheckBoxes[indx]->property(BRAINORDINATE_FILE_POINTER_PROPERTY_NAME);
    if (filePointerVariant.isValid()) {
        void* ptr = filePointerVariant.value<void*>();
        filePointer = (ChartableBrainordinateInterface*)ptr;
    }
    
    return filePointer;
}

/**
 * Get the brainordinate file associated with the given index.
 *
 * @param indx
 *    The index.
 * @return
 *    Brainordinate chartable file associated with the given index or NULL
 *    if not valid
 */
ChartableMatrixInterface*
ChartSelectionViewController::getMatrixFileAtIndex(const int32_t indx)
{
    ChartableMatrixInterface* filePointer = NULL;
    
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    const QVariant filePointerVariant = m_fileEnableCheckBoxes[indx]->property(MATRIX_FILE_POINTER_PROPERTY_NAME);
    if (filePointerVariant.isValid()) {
        void* ptr = filePointerVariant.value<void*>();
        filePointer = (ChartableMatrixInterface*)ptr;
    }
    
    return filePointer;
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartSelectionViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)
            || uiEvent->isToolBoxUpdate()) {
            this->updateSelectionViewController();
            uiEvent->setEventProcessed();
        }
    }
}

///**
// * @return Chart model selected in the selected tab (NULL if not valid)
// */
//ChartModel*
//ChartSelectionViewController::getSelectedChartModel()
//{
//    Brain* brain = GuiManager::get()->getBrain();
//    
//    BrowserTabContent* browserTabContent =
//    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
//    if (browserTabContent == NULL) {
//        return NULL;
//    }
//    const int32_t browserTabIndex = browserTabContent->getTabNumber();
//    
//    ChartModel* chartModel = NULL;
//    
//    ModelChart* modelChart = brain->getChartModel();
//    if (modelChart != NULL) {
//        chartModel = modelChart->getSelectedChartModel(browserTabIndex);
//    }
//    
//    return chartModel;
//}
