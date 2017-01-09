
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

#define __CHART_LINES_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartLinesSelectionViewController.h"
#undef __CHART_LINES_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSignalMapper>
#include <QToolButton>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartModel.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "ChartableMatrixSeriesInterface.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventPaletteColorMappingEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "ModelChart.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

static const char* BRAINORDINATE_FILE_POINTER_PROPERTY_NAME = "brainordinateFilePointer";

/**
 * \class caret::ChartLinesSelectionViewController 
 * \brief Handles selection of charts displayed in chart model.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartLinesSelectionViewController::ChartLinesSelectionViewController(const Qt::Orientation /*orientation */,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    /*
     * In the grid layout, there are columns for the checkboxes (used
     * for brainordinate charts) and radio buttons (used for matrix
     * charts).   Display of checkboxes and radiobuttons is mutually
     * exclusive.  The "Select" column title is over both the checkbox
     * and radio button columns.
     */
    m_brainordinateGridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_brainordinateGridLayout, 4, 2);
    m_brainordinateGridLayout->setColumnStretch(BRAINORDINATE_COLUMN_CHECKBOX, 0);
    m_brainordinateGridLayout->setColumnStretch(BRAINORDINATE_COLUMN_YOKING_COMBO_BOX, 0);
    m_brainordinateGridLayout->setColumnStretch(BRAINORDINATE_COLUMN_LINE_EDIT, 100);
    const int titleRow = m_brainordinateGridLayout->rowCount();
    m_brainordinateGridLayout->addWidget(new QLabel("Select"),
                                         titleRow, BRAINORDINATE_COLUMN_CHECKBOX,
                                         Qt::AlignHCenter);
    m_brainordinateGridLayout->addWidget(new QLabel("Yoke"),
                                         titleRow, BRAINORDINATE_COLUMN_YOKING_COMBO_BOX,
                                         Qt::AlignHCenter);
    m_brainordinateGridLayout->addWidget(new QLabel("Charting File"),
                                         titleRow, BRAINORDINATE_COLUMN_LINE_EDIT,
                                         Qt::AlignHCenter);
    
    m_signalMapperBrainordinateFileEnableCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperBrainordinateFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(brainordinateSelectionCheckBoxClicked(int)));
    
    m_signalMapperBrainordinateYokingComboBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperBrainordinateYokingComboBox, SIGNAL(mapped(int)),
                     this, SLOT(brainordinateYokingComboBoxActivated(int)));
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(m_brainordinateGridLayout);
    layout->addStretch();
}

/**
 * Destructor.
 */
ChartLinesSelectionViewController::~ChartLinesSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the view controller.
 */
void
ChartLinesSelectionViewController::updateSelectionViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    ChartOneDataTypeEnum::Enum chartDataType = ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartDataType = modelChart->getSelectedChartOneDataType(browserTabIndex);
    }
    
    bool validFlag = false;
    switch (chartDataType) {
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            validFlag = true;
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            validFlag = true;
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
            validFlag = true;
            break;
    }
    
    if (validFlag) {
        std::vector<ChartableLineSeriesInterface*> chartableLineSeriesFilesVector;
        
        const ChartOneDataTypeEnum::Enum chartDataType = modelChart->getSelectedChartOneDataType(browserTabIndex);
        
        brain->getAllChartableLineSeriesDataFilesForChartDataType(chartDataType,
                                                                  chartableLineSeriesFilesVector);
        const int32_t numChartableFiles = static_cast<int32_t>(chartableLineSeriesFilesVector.size());
        
        for (int32_t i = 0; i < numChartableFiles; i++) {
            QCheckBox* checkBox = NULL;
            QLineEdit* lineEdit = NULL;
            MapYokingGroupComboBox* yokeComboBox = NULL;
            
            if (i < static_cast<int32_t>(m_fileInfoRows.size())) {
                checkBox    = m_fileInfoRows[i].m_fileEnableCheckBox;//   m_brainordinateFileEnableCheckBoxes[i];
                lineEdit    = m_fileInfoRows[i].m_fileNameLineEdit; //   m_brainordinateFileNameLineEdits[i];
                yokeComboBox = m_fileInfoRows[i].m_fileYokingComboBox; // m_brainordinateSeriesYokingComboBoxes[i];
            }
            else {
                checkBox = new QCheckBox("");
                QObject::connect(checkBox, SIGNAL(clicked(bool)),
                                 m_signalMapperBrainordinateFileEnableCheckBox, SLOT(map()));
                m_signalMapperBrainordinateFileEnableCheckBox->setMapping(checkBox, i);
                
                yokeComboBox = new MapYokingGroupComboBox(this);
                yokeComboBox->getWidget()->setStatusTip("Synchronize enabled status and map indices)");
                yokeComboBox->getWidget()->setToolTip("Yoke to Overlay Mapped Files");
#ifdef CARET_OS_MACOSX
                yokeComboBox->getWidget()->setFixedWidth(yokeComboBox->getWidget()->sizeHint().width() - 20);
#endif // CARET_OS_MACOSX
                QObject::connect(yokeComboBox, SIGNAL(itemActivated()),
                                 m_signalMapperBrainordinateYokingComboBox, SLOT(map()));
                m_signalMapperBrainordinateYokingComboBox->setMapping(yokeComboBox, i);
                
                lineEdit = new QLineEdit();
                lineEdit->setReadOnly(true);
                
                FileInfoRow rowInfo;
                rowInfo.m_fileEnableCheckBox = checkBox;
                rowInfo.m_fileNameLineEdit   = lineEdit;
                rowInfo.m_fileYokingComboBox = yokeComboBox;
                rowInfo.m_lineSeriesFile     = NULL;
                m_fileInfoRows.push_back(rowInfo);
                
                const int row = m_brainordinateGridLayout->rowCount();
                m_brainordinateGridLayout->addWidget(checkBox,
                                                     row, BRAINORDINATE_COLUMN_CHECKBOX,
                                                     Qt::AlignHCenter);
                m_brainordinateGridLayout->addWidget(yokeComboBox->getWidget(),
                                                     row, BRAINORDINATE_COLUMN_YOKING_COMBO_BOX,
                                                     Qt::AlignHCenter);
                m_brainordinateGridLayout->addWidget(lineEdit,
                                                     row, BRAINORDINATE_COLUMN_LINE_EDIT);
            }
            
            CaretAssertVectorIndex(chartableLineSeriesFilesVector, i);
            ChartableLineSeriesInterface* chartBrainFile = chartableLineSeriesFilesVector[i];
            CaretAssert(chartBrainFile);
            
            m_fileInfoRows[i].m_lineSeriesFile = chartBrainFile;
            
            const bool checkBoxStatus = chartBrainFile->isLineSeriesChartingEnabled(browserTabIndex);
            
            QVariant brainordinateFilePointerVariant = qVariantFromValue((void*)chartBrainFile);
            
            CaretMappableDataFile* caretMappableDataFile = chartBrainFile->getLineSeriesChartCaretMappableDataFile();
            
            checkBox->blockSignals(true);
            checkBox->setChecked(checkBoxStatus);
            checkBox->blockSignals(false);
            
            checkBox->setProperty(BRAINORDINATE_FILE_POINTER_PROPERTY_NAME,
                                  brainordinateFilePointerVariant);
            
            /*
             * Could be line chart for matrix series file
             */
            ChartableMatrixSeriesInterface* matrixFile = dynamic_cast<ChartableMatrixSeriesInterface*>(caretMappableDataFile);
            if (matrixFile != NULL) {
                yokeComboBox->getWidget()->setEnabled(true);
                yokeComboBox->setMapYokingGroup(matrixFile->getMatrixRowColumnMapYokingGroup(browserTabIndex));
            }
            else {
                yokeComboBox->getWidget()->setEnabled(false);
            }
            
            CaretAssert(caretMappableDataFile);
            lineEdit->setText(caretMappableDataFile->getFileName());
        }
        
        
        const int32_t numItems = static_cast<int32_t>(m_fileInfoRows.size());
        for (int32_t i = 0; i < numItems; i++) {
            bool showCheckBox    = false;
            bool showYokeComboBox = false;
            bool showLineEdit    = false;
            
            if (i < numChartableFiles) {
                showLineEdit = true;
                showYokeComboBox = true;
                showCheckBox = true;
            }
            else {
                m_fileInfoRows[i].m_lineSeriesFile = NULL;
            }
            
            m_fileInfoRows[i].m_fileEnableCheckBox->setVisible(showCheckBox);
            m_fileInfoRows[i].m_fileNameLineEdit->setVisible(showLineEdit);
            m_fileInfoRows[i].m_fileYokingComboBox->getWidget()->setVisible(showYokeComboBox);
        }
    }
}

/**
 * Called when a brainordinate yoking combo box changes.
 *
 * @param indx
 *    Index of yoking combo box that was clicked.
 */
void
ChartLinesSelectionViewController::brainordinateYokingComboBoxActivated(int indx)
{
    CaretAssertVectorIndex(m_fileInfoRows, indx);
    ChartableMatrixSeriesInterface* matrixFile = dynamic_cast<ChartableMatrixSeriesInterface*>(m_fileInfoRows[indx].m_lineSeriesFile);
    if (matrixFile != NULL) {
        BrowserTabContent* browserTabContent =
        GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
        if (browserTabContent == NULL) {
            return;
        }
        const int32_t browserTabIndex = browserTabContent->getTabNumber();
        
        matrixFile->setMatrixRowColumnMapYokingGroup(browserTabIndex,
                                                     m_fileInfoRows[indx].m_fileYokingComboBox->getMapYokingGroup());
    }
}

/**
 * Called when a brainordinate enabled check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
ChartLinesSelectionViewController::brainordinateSelectionCheckBoxClicked(int indx)
{
    CaretAssertVectorIndex(m_fileInfoRows, indx);
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    const bool newStatus = m_fileInfoRows[indx].m_fileEnableCheckBox->isChecked();
    
    if (m_fileInfoRows[indx].m_lineSeriesFile != NULL) {
        m_fileInfoRows[indx].m_lineSeriesFile->setLineSeriesChartingEnabled(browserTabIndex,
                                                                            newStatus);
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartLinesSelectionViewController::receiveEvent(Event* event)
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
