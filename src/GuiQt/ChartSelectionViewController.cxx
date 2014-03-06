
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

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSignalMapper>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartModel.h"
#include "ChartableInterface.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "WuQtUtilities.h"

using namespace caret;

static const char* FILE_POINTER_PROPERTY_NAME = "filePointer";
    
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
    m_gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridLayout, 4, 2);
    m_gridLayout->setColumnStretch(COLUMN_ENABLE_CHECKBOX, 0);
    m_gridLayout->setColumnStretch(COLUMN_NAME_LINE_EDIT, 100);
    const int titleRow = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QLabel("On"),
                            titleRow, COLUMN_ENABLE_CHECKBOX,
                            Qt::AlignHCenter);
    m_gridLayout->addWidget(new QLabel("Charting File"),
                            titleRow, COLUMN_NAME_LINE_EDIT,
                            Qt::AlignHCenter);
    
    m_signalMapperFileEnableCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(enabledCheckBoxClicked(int)));
    
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
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    ChartModel* chartModel = NULL;
    
    ChartDataTypeEnum::Enum chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartModel = modelChart->getSelectedChartModel(browserTabIndex);
        if (chartModel != NULL) {
            chartDataType = chartModel->getChartDataType();
        }
    }
    
    std::vector<ChartableInterface*> chartableFilesVector;
    brain->getAllChartableDataFilesForChartDataType(chartDataType,
                                                    chartableFilesVector);
    
    const int32_t numChartableFiles = static_cast<int32_t>(chartableFilesVector.size());
    
    for (int32_t i = 0; i < numChartableFiles; i++) {
        QCheckBox* checkBox = NULL;
        QLineEdit* lineEdit = NULL;
        
        if (i < static_cast<int32_t>(m_fileEnableCheckBoxes.size())) {
            checkBox = m_fileEnableCheckBoxes[i];
            lineEdit = m_fileNameLineEdits[i];
        }
        else {
            checkBox = new QCheckBox("");
            m_fileEnableCheckBoxes.push_back(checkBox);
            
            lineEdit = new QLineEdit();
            lineEdit->setReadOnly(true);
            m_fileNameLineEdits.push_back(lineEdit);
            
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_signalMapperFileEnableCheckBox, SLOT(map()));
            m_signalMapperFileEnableCheckBox->setMapping(checkBox, i);
            
            const int row = m_gridLayout->rowCount();
            m_gridLayout->addWidget(checkBox,
                                    row, COLUMN_ENABLE_CHECKBOX);
            m_gridLayout->addWidget(lineEdit,
                                    row, COLUMN_NAME_LINE_EDIT);
        }
        
        const ChartableInterface* chartFile = chartableFilesVector[i];
        CaretAssertVectorIndex(chartableFilesVector, i);
        CaretAssert(chartFile);
        
        const CaretMappableDataFile* caretMappableDataFile = chartFile->getCaretMappableDataFile();
        CaretAssert(caretMappableDataFile);
        
        const bool checkStatus = chartFile->isChartingEnabled(browserTabIndex);
        
        checkBox->setChecked(checkStatus);
        checkBox->setProperty(FILE_POINTER_PROPERTY_NAME,
                              qVariantFromValue((void*)chartFile));
        lineEdit->setText(caretMappableDataFile->getFileName());
    }
    
    
    const int32_t numItems = static_cast<int32_t>(m_fileEnableCheckBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        bool showRow = false;
        if (i < numChartableFiles) {
            showRow = true;
        }
        
        m_fileEnableCheckBoxes[i]->setVisible(showRow);
        m_fileNameLineEdits[i]->setVisible(showRow);
    }
}

/**
 * Called when an enabled check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
ChartSelectionViewController::enabledCheckBoxClicked(int indx)
{
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    const bool newStatus = m_fileEnableCheckBoxes[indx]->isChecked();
    
    ChartableInterface* chartFile = NULL;
    
    getFileAtIndex(indx,
                   chartFile);

    chartFile->setChartingEnabled(browserTabIndex,
                                  newStatus);
}

/**
 * Get the file associated with the given index.
 *
 * @param indx
 *    The index.
 * @param chartFileOut
 *    Chartable files associated with the given index.
 */
void
ChartSelectionViewController::getFileAtIndex(const int32_t indx,
                                           ChartableInterface* &chartFileOut)
{
    CaretAssertVectorIndex(m_fileEnableCheckBoxes, indx);
    void* ptr = m_fileEnableCheckBoxes[indx]->property(FILE_POINTER_PROPERTY_NAME).value<void*>();
    chartFileOut = (ChartableInterface*)ptr;
    CaretAssert(chartFileOut);
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

/**
 * @return Chart model selected in the selected tab (NULL if not valid)
 */
ChartModel*
ChartSelectionViewController::getSelectedChartModel()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return NULL;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    ChartModel* chartModel = NULL;
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartModel = modelChart->getSelectedChartModel(browserTabIndex);
    }
    
    return chartModel;
}
