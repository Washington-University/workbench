
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

#define __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartSelectionViewController.h"
#undef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QButtonGroup>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSignalMapper>
#include <QStackedWidget>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixInterface.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartMatrixLoadingTypeEnum.h"
#include "ChartModel.h"
#include "ChartableBrainordinateInterface.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "WuQtUtilities.h"

using namespace caret;

static const char* BRAINORDINATE_FILE_POINTER_PROPERTY_NAME = "brainordinateFilePointer";

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
    
    m_brainordinateChartWidget = createBrainordinateChartWidget();
    
    m_matrixChartWidget = createMatrixChartWidget();
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_brainordinateChartWidget);
    m_stackedWidget->addWidget(m_matrixChartWidget);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_stackedWidget);
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
    
    switch (m_mode) {
        case MODE_INVALID:
            break;
        case MODE_BRAINORDINATE:
            m_stackedWidget->setCurrentWidget(m_brainordinateChartWidget);
            updateBrainordinateChartWidget(brain,
                                           modelChart,
                                           browserTabIndex);
            break;
        case MODE_MATRIX:
            m_stackedWidget->setCurrentWidget(m_matrixChartWidget);
            updateMatrixChartWidget(brain,
                                    modelChart,
                                    browserTabIndex);
            break;
    }
}

/**
 * Called when an enabled check box changes state.
 *
 * @param indx
 *    Index of checkbox that was clicked.
 */
void
ChartSelectionViewController::brainordinateSelectionCheckBoxClicked(int indx)
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
    
    CaretAssertVectorIndex(m_brainordinateFileEnableCheckBoxes, indx);
    const bool newStatus = m_brainordinateFileEnableCheckBoxes[indx]->isChecked();
    
    ChartableBrainordinateInterface* chartFile = getBrainordinateFileAtIndex(indx);
    CaretAssert(chartFile);

    if (chartFile != NULL) {
        chartFile->setBrainordinateChartingEnabled(browserTabIndex,
                                      newStatus);
    }
}

///**
// * Called when a selection radio button is clicked.
// *
// * @param indx
// *    Index of radio that was clicked.
// */
//void
//ChartSelectionViewController::brainordinateSelectionRadioButtonClicked(int indx)
//{
//    switch (m_mode) {
//        case MODE_INVALID:
//            CaretAssertMessage(0, "Radiobutton should never be clicked when mode is invalid.");
//            return;
//            break;
//        case MODE_BRAINORDINATE:
//            CaretAssertMessage(0, "Radiobutton should never be clicked when mode is matrix.");
//            return;
//            break;
//        case MODE_MATRIX:
//            break;
//    }
//    
//    ChartableMatrixInterface* chartMatrixFile = getMatrixFileAtIndex(indx);
//    CaretAssert(chartMatrixFile);
//    
//    Brain* brain = GuiManager::get()->getBrain();
//    
//    BrowserTabContent* browserTabContent =
//    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
//    if (browserTabContent == NULL) {
//        return;
//    }
//    const int32_t browserTabIndex = browserTabContent->getTabNumber();
//    
//    ModelChart* modelChart = brain->getChartModel();
//    if (modelChart != NULL) {
//        CaretDataFileSelectionModel* fileSelector =
//           modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
//        fileSelector->setSelectedFile(chartMatrixFile->getCaretMappableDataFile());
//        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
//    }
//}

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
    
    CaretAssertVectorIndex(m_brainordinateFileEnableCheckBoxes, indx);
    const QVariant filePointerVariant = m_brainordinateFileEnableCheckBoxes[indx]->property(BRAINORDINATE_FILE_POINTER_PROPERTY_NAME);
    if (filePointerVariant.isValid()) {
        void* ptr = filePointerVariant.value<void*>();
        filePointer = (ChartableBrainordinateInterface*)ptr;
    }
    
    return filePointer;
}

///**
// * Get the brainordinate file associated with the given index.
// *
// * @param indx
// *    The index.
// * @return
// *    Brainordinate chartable file associated with the given index or NULL
// *    if not valid
// */
//ChartableMatrixInterface*
//ChartSelectionViewController::getMatrixFileAtIndex(const int32_t indx)
//{
//    ChartableMatrixInterface* filePointer = NULL;
//    
//    CaretAssertVectorIndex(m_fileSelectionRadioButtons, indx);
//    const QVariant filePointerVariant = m_fileSelectionRadioButtons[indx]->property(MATRIX_FILE_POINTER_PROPERTY_NAME);
//    if (filePointerVariant.isValid()) {
//        void* ptr = filePointerVariant.value<void*>();
//        filePointer = (ChartableMatrixInterface*)ptr;
//    }
//    
//    return filePointer;
//}
//

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
 * @return The Brainordinate chart widget.
 */
QWidget*
ChartSelectionViewController::createBrainordinateChartWidget()
{
    
    /*
     * In the grid layout, there are columns for the checkboxes (used
     * for brainordinate charts) and radio buttons (used for matrix
     * charts).   Display of checkboxes and radiobuttons is mutually
     * exclusive.  The "Select" column title is over both the checkbox
     * and radio button columns.
     */
    QWidget* widget = new QWidget();
    m_brainordinateGridLayout = new QGridLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(m_brainordinateGridLayout, 4, 2);
    m_brainordinateGridLayout->setColumnStretch(COLUMN_CHECKBOX, 0);
    m_brainordinateGridLayout->setColumnStretch(COLUMN_LINE_EDIT, 100);
    const int titleRow = m_brainordinateGridLayout->rowCount();
    m_brainordinateGridLayout->addWidget(new QLabel("Select"),
                            titleRow, COLUMN_CHECKBOX,
                            Qt::AlignHCenter);
    m_brainordinateGridLayout->addWidget(new QLabel("Charting File"),
                            titleRow, COLUMN_LINE_EDIT,
                            Qt::AlignHCenter);
    
    m_signalMapperBrainordinateFileEnableCheckBox = new QSignalMapper(this);
    QObject::connect(m_signalMapperBrainordinateFileEnableCheckBox, SIGNAL(mapped(int)),
                     this, SLOT(brainordinateSelectionCheckBoxClicked(int)));
    
    return widget;
}

/**
 * Update the brainordiante chart widget.
 *
 * @param brain
 *     The Brain.
 * @param modelChart
 *     The Model for charts.
 * @param browserTabIndex
 *     Index of the browser tab.
 */
void
ChartSelectionViewController::updateBrainordinateChartWidget(Brain* brain,
                                                             ModelChart* modelChart,
                                                             const int32_t browserTabIndex)
{
    std::vector<ChartableBrainordinateInterface*> chartableBrainordinateFilesVector;
    
    const ChartDataTypeEnum::Enum chartDataType = modelChart->getSelectedChartDataType(browserTabIndex);

    brain->getAllChartableBrainordinateDataFilesForChartDataType(chartDataType,
                                                                 chartableBrainordinateFilesVector);
    const int32_t numChartableFiles = static_cast<int32_t>(chartableBrainordinateFilesVector.size());
    
    for (int32_t i = 0; i < numChartableFiles; i++) {
        QCheckBox* checkBox = NULL;
        QLineEdit* lineEdit = NULL;
        
        if (i < static_cast<int32_t>(m_brainordinateFileEnableCheckBoxes.size())) {
            checkBox    = m_brainordinateFileEnableCheckBoxes[i];
            lineEdit    = m_brainordinateFileNameLineEdits[i];
        }
        else {
            checkBox = new QCheckBox("");
            m_brainordinateFileEnableCheckBoxes.push_back(checkBox);
            
            lineEdit = new QLineEdit();
            lineEdit->setReadOnly(true);
            m_brainordinateFileNameLineEdits.push_back(lineEdit);
            
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_signalMapperBrainordinateFileEnableCheckBox, SLOT(map()));
            m_signalMapperBrainordinateFileEnableCheckBox->setMapping(checkBox, i);
            
            const int row = m_brainordinateGridLayout->rowCount();
            m_brainordinateGridLayout->addWidget(checkBox,
                                    row, COLUMN_CHECKBOX,
                                    Qt::AlignHCenter);
            m_brainordinateGridLayout->addWidget(lineEdit,
                                    row, COLUMN_LINE_EDIT);
        }
        
        CaretAssertVectorIndex(chartableBrainordinateFilesVector, i);
        ChartableBrainordinateInterface* chartBrainFile = chartableBrainordinateFilesVector[i];
        CaretAssert(chartBrainFile);
        const bool checkBoxStatus = chartBrainFile->isBrainordinateChartingEnabled(browserTabIndex);
        
        QVariant brainordinateFilePointerVariant = qVariantFromValue((void*)chartBrainFile);
        
        CaretMappableDataFile* caretMappableDataFile = chartBrainFile->getBrainordinateChartCaretMappableDataFile();
        
        checkBox->blockSignals(true);
        checkBox->setChecked(checkBoxStatus);
        checkBox->blockSignals(false);
        
        checkBox->setProperty(BRAINORDINATE_FILE_POINTER_PROPERTY_NAME,
                              brainordinateFilePointerVariant);
        
        CaretAssert(caretMappableDataFile);
        lineEdit->setText(caretMappableDataFile->getFileName());
    }
    
    
    const int32_t numItems = static_cast<int32_t>(m_brainordinateFileEnableCheckBoxes.size());
    for (int32_t i = 0; i < numItems; i++) {
        bool showCheckBox    = false;
        bool showLineEdit    = false;
        
        if (i < numChartableFiles) {
            showLineEdit = true;
            showCheckBox = true;
        }
        
        m_brainordinateFileEnableCheckBoxes[i]->setVisible(showCheckBox);
        m_brainordinateFileNameLineEdits[i]->setVisible(showLineEdit);
    }
}

/**
 * Called when a matrix file is selected.
 *
 * @param caretDataFile
 *    Caret data file that was selected.
 */
void
ChartSelectionViewController::matrixFileSelected(CaretDataFile* /*caretDataFile*/)
{
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Gets called when matrix loading button is clicked.
 */
void
ChartSelectionViewController::matrixFileLoadingButtonClicked()
{
    ChartMatrixDisplayProperties* displayProperties = getChartMatrixDisplayProperties();
    if (displayProperties != NULL) {
        if (m_matrixLoadByColumnRadioButton->isChecked()) {
            displayProperties->setMatrixLoadingType(ChartMatrixLoadingTypeEnum:: CHART_MATRIX_LOAD_BY_COLUMN);
        }
        else if (m_matrixLoadByRowRadioButton->isChecked()) {
            displayProperties->setMatrixLoadingType(ChartMatrixLoadingTypeEnum::CHART_MATRIX_LOAD_BY_ROW);
        }
        else {
            CaretAssert(0);
        }
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


/**
 * Gets called when yoking gruup is changed.
 */
void
ChartSelectionViewController::matrixYokingGroupEnumComboBoxActivated()
{
    ChartMatrixDisplayProperties* displayProperties = getChartMatrixDisplayProperties();
    if (displayProperties != NULL) {
        displayProperties->setYokingGroup(m_matrixYokingGroupComboBox->getSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


/**
 * @return The matrix chart widget.
 */
QWidget*
ChartSelectionViewController::createMatrixChartWidget()
{
    QLabel* fileLabel = new QLabel("File ");
    m_matrixFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_matrixFileSelectionComboBox, SIGNAL(fileSelected(CaretDataFile*)),
                     this, SLOT(matrixFileSelected(CaretDataFile*)));
    m_matrixFileSelectionComboBox->getWidget()->setSizePolicy(QSizePolicy::MinimumExpanding,
                                                              m_matrixFileSelectionComboBox->getWidget()->sizePolicy().verticalPolicy());
    
    m_matrixLoadByColumnRadioButton = new QRadioButton("Column");
    m_matrixLoadByRowRadioButton    = new QRadioButton("Row");
    
    QButtonGroup* matrixLoadButtonGroup = new QButtonGroup(this);
    matrixLoadButtonGroup->addButton(m_matrixLoadByColumnRadioButton);
    matrixLoadButtonGroup->addButton(m_matrixLoadByRowRadioButton);
    matrixLoadButtonGroup->setExclusive(true);
    QObject::connect(matrixLoadButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(matrixFileLoadingButtonClicked()));
    
    QGroupBox* matrixLoadGroupBox = new QGroupBox("Load by");
    QVBoxLayout* matrixLoadLayout = new QVBoxLayout(matrixLoadGroupBox);
    matrixLoadLayout->addWidget(m_matrixLoadByColumnRadioButton);
    matrixLoadLayout->addWidget(m_matrixLoadByRowRadioButton);
    matrixLoadGroupBox->setSizePolicy(QSizePolicy::Fixed,
                                      QSizePolicy::Fixed);
    
    QLabel* yokeLabel = new QLabel("Yoke ");
    m_matrixYokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_matrixYokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
    QObject::connect(m_matrixYokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(matrixYokingGroupEnumComboBoxActivated()));
    
    QGridLayout* fileYokeLayout = new QGridLayout();
    fileYokeLayout->setColumnStretch(0, 0);
    fileYokeLayout->setColumnStretch(1, 0);
    fileYokeLayout->setColumnStretch(2, 100);
    fileYokeLayout->addWidget(fileLabel,
                      0, 0);
    fileYokeLayout->addWidget(m_matrixFileSelectionComboBox->getWidget(),
                      0, 1,
                      1, 2);
    fileYokeLayout->addWidget(yokeLabel,
                      1, 0);
    fileYokeLayout->addWidget(m_matrixYokingGroupComboBox->getWidget(),
                      1, 1);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(matrixLoadGroupBox, 0);
    layout->addLayout(fileYokeLayout, 100);
    
    
    /*
     * HIDE LOAD BY AND YOKE CONTROLS UNTIL IMPLEMENTATION OF 
     * THEIR FUNCTIONALITY TAKES PLACE
     */
    matrixLoadGroupBox->setHidden(true);
    yokeLabel->setHidden(true);
    m_matrixYokingGroupComboBox->getWidget()->setHidden(true);
    
    
    
    
    return widget;
}

/**
 * @return Chart matrix display properties for the selected matrix file.
 *         Value may be NULL!
 */
ChartMatrixDisplayProperties*
ChartSelectionViewController::getChartMatrixDisplayProperties()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return NULL;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        CaretDataFileSelectionModel* fileSelectionModel = modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
        m_matrixFileSelectionComboBox->updateComboBox(fileSelectionModel);
        
        CaretDataFile* caretFile = fileSelectionModel->getSelectedFile();
        if (caretFile != NULL) {
            ChartableMatrixInterface* matrixFile = dynamic_cast<ChartableMatrixInterface*>(caretFile);
            if (matrixFile != NULL) {
                ChartMatrixDisplayProperties* displayProperties = matrixFile->getChartMatrixDisplayProperties(browserTabIndex);
                return displayProperties;
            }
        }
    }
    
    return NULL;
}

/**
 * Update the matrix chart widget.
 *
 * @param brain
 *     The Brain.
 * @param modelChart
 *     The Model for charts.
 * @param browserTabIndex
 *     Index of the browser tab.
 */
void
ChartSelectionViewController::updateMatrixChartWidget(Brain* /* brain */,
                                                      ModelChart* modelChart,
                                                      const int32_t browserTabIndex)
{
    CaretDataFileSelectionModel* fileSelectionModel = modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
    m_matrixFileSelectionComboBox->updateComboBox(fileSelectionModel);
    
    const ChartMatrixDisplayProperties* displayProperties = getChartMatrixDisplayProperties();
    if (displayProperties != NULL) {
        const ChartMatrixLoadingTypeEnum::Enum loadType = displayProperties->getMatrixLoadingType();
        
        switch (loadType) {
            case ChartMatrixLoadingTypeEnum:: CHART_MATRIX_LOAD_BY_COLUMN:
                m_matrixLoadByColumnRadioButton->setChecked(true);
                break;
            case ChartMatrixLoadingTypeEnum::CHART_MATRIX_LOAD_BY_ROW:
                m_matrixLoadByRowRadioButton->setChecked(true);
                break;
        }
        
        const YokingGroupEnum::Enum yokingGroup = displayProperties->getYokingGroup();
        m_matrixYokingGroupComboBox->setSelectedItem<YokingGroupEnum,YokingGroupEnum::Enum>(yokingGroup);
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
