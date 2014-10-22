
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

#include <QAction>
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
#include <QToolButton>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "ChartableMatrixInterface.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartMatrixLoadingDimensionEnum.h"
#include "ChartModel.h"
#include "ChartableBrainordinateInterface.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "DeveloperFlagsEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventChartMatrixYokingValidation.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventPaletteColorMappingEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
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
ChartSelectionViewController::ChartSelectionViewController(const Qt::Orientation orientation,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_mode = MODE_INVALID;
    
    m_brainordinateChartWidget = createBrainordinateChartWidget();
    
    m_matrixChartWidget = createMatrixChartWidget(orientation);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_brainordinateChartWidget);
    m_stackedWidget->addWidget(m_matrixChartWidget);

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
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

    ChartDataTypeEnum::Enum chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartDataType = modelChart->getSelectedChartDataType(browserTabIndex);
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
    m_brainordinateGridLayout = new QGridLayout();
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
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(m_brainordinateGridLayout);
    layout->addStretch();
    
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
    updateSelectionViewController();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Gets called when matrix loading combo box is changed.
 */
void
ChartSelectionViewController::matrixFileLoadingComboBoxActivated()
{
    CaretMappableDataFile*        caretMappableDataFile        = NULL;
    ChartableMatrixInterface*     chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties* chartMatrixDisplayProperties = NULL;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartMatrixDisplayProperties)) {
        return;
    }
    
    chartableMatrixInterface->setMatrixLoadingDimension(m_matrixLoadByColumnRowComboBox->getSelectedItem<ChartMatrixLoadingDimensionEnum,
                                                   ChartMatrixLoadingDimensionEnum::Enum>());
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Gets called when yoking gruup is changed.
 */
void
ChartSelectionViewController::matrixYokingGroupEnumComboBoxActivated()
{
    CaretMappableDataFile*        caretMappableDataFile        = NULL;
    ChartableMatrixInterface*     chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties* chartMatrixDisplayProperties = NULL;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartMatrixDisplayProperties)) {
        return;
    }
    
    YokingGroupEnum::Enum newYokingGroup = m_matrixYokingGroupComboBox->getSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>();
    int32_t selectedRowColumnIndex = -1;
    if (newYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
        const YokingGroupEnum::Enum previousYokingGroup = chartableMatrixInterface->getYokingGroup();
        
        EventChartMatrixYokingValidation yokeEvent(chartableMatrixInterface,
                                                newYokingGroup);
        EventManager::get()->sendEvent(yokeEvent.getPointer());
        AString message;
        if ( ! yokeEvent.isValidateYokingCompatible(message,
                                                    selectedRowColumnIndex)) {
            message = WuQtUtilities::createWordWrappedToolTipText(message);
            
            WuQMessageBox::YesNoCancelResult result =
            WuQMessageBox::warningYesNoCancel(m_matrixYokingGroupComboBox->getWidget(),
                                              message,
                                              "");
            switch (result) {
                case WuQMessageBox::RESULT_YES:
                    break;
                case WuQMessageBox::RESULT_NO:
                    newYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
                    selectedRowColumnIndex = -1;
                    break;
                case WuQMessageBox::RESULT_CANCEL:
                    newYokingGroup = previousYokingGroup;
                    selectedRowColumnIndex = -1;
                    break;
            }
        }
    }
    
    /*
     * Need to update combo box since user may have changed mind and 
     * the combo box status needs to change
     */
    m_matrixYokingGroupComboBox->setSelectedItem<YokingGroupEnum,YokingGroupEnum::Enum>(newYokingGroup);
    
    chartableMatrixInterface->setYokingGroup(newYokingGroup);
    
    /*
     * If yoking changed update the file's selected row or column
     */
    if (newYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
        if (selectedRowColumnIndex >= 0) {
            CiftiMappableConnectivityMatrixDataFile* matrixFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(chartableMatrixInterface);
            if (matrixFile != NULL) {
                switch (chartableMatrixInterface->getMatrixLoadingDimension()) {
                    case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                        matrixFile->loadDataForColumnIndex(selectedRowColumnIndex);
                        break;
                    case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                        matrixFile->loadDataForRowIndex(selectedRowColumnIndex);
                        break;
                }
            }
        }
    }
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when colorbar icon button is clicked.
 */
void
ChartSelectionViewController::matrixColorBarActionTriggered(bool status)
{
    CaretMappableDataFile*        caretMappableDataFile        = NULL;
    ChartableMatrixInterface*     chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties* chartMatrixDisplayProperties = NULL;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartMatrixDisplayProperties)) {
        return;
    }
    
    chartMatrixDisplayProperties->setColorBarDisplayed(status);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when settings icon button is clicked to display palette editor.
 */
void
ChartSelectionViewController::matrixSettingsActionTriggered()
{
    CaretMappableDataFile*        caretMappableDataFile        = NULL;
    ChartableMatrixInterface*     chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties* chartMatrixDisplayProperties = NULL;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartMatrixDisplayProperties)) {
        return;
    }

    const int32_t mapIndex = 0;
    EventPaletteColorMappingEditorDialogRequest dialogEvent(m_browserWindowIndex,
                                                            caretMappableDataFile,
                                                            mapIndex);
    EventManager::get()->sendEvent(dialogEvent.getPointer());
}

/**
 * @return The matrix chart widget.
 */
QWidget*
ChartSelectionViewController::createMatrixChartWidget(const Qt::Orientation orientation)
{
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    m_matrixColorBarAction = WuQtUtilities::createAction("CB",
                                                       "Display color bar for this overlay",
                                                       this,
                                                       this,
                                                       SLOT(matrixColorBarActionTriggered(bool)));
    m_matrixColorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        m_matrixColorBarAction->setIcon(colorBarIcon);
    }
    QToolButton* colorBarToolButton = new QToolButton();
    colorBarToolButton->setDefaultAction(m_matrixColorBarAction);
    
    /*
     * Settings Tool Button
     */
    QLabel* settingsLabel = new QLabel("Settings");
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);
    
    m_matrixSettingsAction = WuQtUtilities::createAction("S",
                                                       "Edit settings for this map and overlay",
                                                       this,
                                                       this,
                                                       SLOT(matrixSettingsActionTriggered()));
    if (settingsIconValid) {
        m_matrixSettingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(m_matrixSettingsAction);
    
    
    QLabel* fileLabel = new QLabel("Matrix File");
    m_matrixFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_matrixFileSelectionComboBox, SIGNAL(fileSelected(CaretDataFile*)),
                     this, SLOT(matrixFileSelected(CaretDataFile*)));
//    m_matrixFileSelectionComboBox->getWidget()->setSizePolicy(QSizePolicy::Expanding,
//                                                              m_matrixFileSelectionComboBox->getWidget()->sizePolicy().verticalPolicy());
    
    QLabel* loadDimensionLabel = new QLabel("Load By");
    m_matrixLoadByColumnRowComboBox = new EnumComboBoxTemplate(this);
    m_matrixLoadByColumnRowComboBox->setup<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>();
    QObject::connect(m_matrixLoadByColumnRowComboBox, SIGNAL(itemActivated()),
                     this, SLOT(matrixFileLoadingComboBoxActivated()));

    
    QLabel* yokeLabel = new QLabel("Yoke ");
    m_matrixYokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_matrixYokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
    QObject::connect(m_matrixYokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(matrixYokingGroupEnumComboBoxActivated()));
    
    QGroupBox* fileYokeGroupBox = new QGroupBox("Matrix Loading");
    fileYokeGroupBox->setFlat(true);
    fileYokeGroupBox->setAlignment(Qt::AlignHCenter);
    QGridLayout* fileYokeLayout = new QGridLayout(fileYokeGroupBox);
    
    switch (orientation) {
        case Qt::Horizontal:
        {
            WuQtUtilities::setLayoutSpacingAndMargins(fileYokeLayout, 2, 0);
            fileYokeLayout->setColumnStretch(0, 0);
            fileYokeLayout->setColumnStretch(1, 0);
            fileYokeLayout->setColumnStretch(2, 0);
            fileYokeLayout->setColumnStretch(3, 0);
            fileYokeLayout->setColumnStretch(4, 100);
            
            fileYokeLayout->addWidget(loadDimensionLabel,
                                      0, 0,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(settingsLabel,
                                      0, 1,
                                      1, 2,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(yokeLabel,
                                      0, 3,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(fileLabel,
                                      0, 4,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(m_matrixLoadByColumnRowComboBox->getWidget(),
                                      1, 0);
            fileYokeLayout->addWidget(settingsToolButton,
                                      1, 1);
            fileYokeLayout->addWidget(colorBarToolButton,
                                      1, 2);
            fileYokeLayout->addWidget(m_matrixYokingGroupComboBox->getWidget(),
                                      1, 3);
            fileYokeLayout->addWidget(m_matrixFileSelectionComboBox->getWidget(),
                                      1, 4);
        }
            break;
        case Qt::Vertical:
        {
            WuQtUtilities::setLayoutSpacingAndMargins(fileYokeLayout, 2, 0);
            fileYokeLayout->setColumnStretch(0, 0);
            fileYokeLayout->setColumnStretch(1, 0);
            fileYokeLayout->setColumnStretch(2, 0);
            fileYokeLayout->setColumnStretch(3, 0);
            fileYokeLayout->setColumnStretch(4, 100);
            
            fileYokeLayout->addWidget(loadDimensionLabel,
                                      0, 0,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(settingsLabel,
                                      0, 1,
                                      1, 2,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(yokeLabel,
                                      0, 3,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(m_matrixLoadByColumnRowComboBox->getWidget(),
                                      1, 0);
            fileYokeLayout->addWidget(settingsToolButton,
                                      1, 1);
            fileYokeLayout->addWidget(colorBarToolButton,
                                      1, 2);
            fileYokeLayout->addWidget(m_matrixYokingGroupComboBox->getWidget(),
                                      1, 3);
            fileYokeLayout->addWidget(fileLabel,
                                      2, 0, 1, 4,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(m_matrixFileSelectionComboBox->getWidget(),
                                      3, 0, 1, 4);
        }
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    m_parcelReorderingEnabledCheckBox = new QCheckBox("");
    QObject::connect(m_parcelReorderingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(parcelLabelFileRemappingFileSelectorChanged()));
    
    m_parcelLabelFileRemappingFileSelector = new CaretMappableDataFileAndMapSelectorObject(DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL,
                                                                                           CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                                           this);
    QObject::connect(m_parcelLabelFileRemappingFileSelector, SIGNAL(selectionWasPerformed()),
                     this, SLOT(parcelLabelFileRemappingFileSelectorChanged()));
    
    QLabel* parcelCheckBoxLabel = new QLabel("On");
    QLabel* parcelFileLabel = new QLabel("Parcel Label File");
    QLabel* parcelFileMapLabel = new QLabel("Map");
    QLabel* parcelFileMapIndexLabel = new QLabel("Index");
    QWidget* mapFileComboBox = NULL;
    QWidget* mapIndexSpinBox = NULL;
    QWidget* mapNameComboBox = NULL;
    m_parcelLabelFileRemappingFileSelector->getWidgetsForAddingToLayout(mapFileComboBox,
                                                                        mapIndexSpinBox,
                                                                        mapNameComboBox);
    m_parcelRemappingGroupBox = new QGroupBox("Parcel Reordering");
    m_parcelRemappingGroupBox->setFlat(true);
    m_parcelRemappingGroupBox->setAlignment(Qt::AlignHCenter);
    QGridLayout* parcelMapFileLayout = new QGridLayout(m_parcelRemappingGroupBox);
    switch (orientation) {
        case Qt::Horizontal:
        {
            WuQtUtilities::setLayoutSpacingAndMargins(parcelMapFileLayout, 2, 0);
            parcelMapFileLayout->setColumnStretch(0,   0);
            parcelMapFileLayout->setColumnStretch(1, 100);
            parcelMapFileLayout->setColumnStretch(2,   0);
            parcelMapFileLayout->setColumnStretch(3, 100);
            parcelMapFileLayout->addWidget(parcelCheckBoxLabel, 0, 0, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(parcelFileLabel, 0, 1, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(parcelFileMapLabel, 0, 2, 1, 2, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(m_parcelReorderingEnabledCheckBox, 1,0);
            parcelMapFileLayout->addWidget(mapFileComboBox, 1, 1);
            parcelMapFileLayout->addWidget(mapIndexSpinBox, 1, 2);
            parcelMapFileLayout->addWidget(mapNameComboBox, 1, 3);
        }
            break;
        case Qt::Vertical:
        {
            WuQtUtilities::setLayoutSpacingAndMargins(parcelMapFileLayout, 2, 0);
            parcelMapFileLayout->setColumnStretch(0,   0);
            parcelMapFileLayout->setColumnStretch(1, 100);
            parcelMapFileLayout->addWidget(parcelCheckBoxLabel, 0, 0, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(parcelFileLabel, 0, 1, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(m_parcelReorderingEnabledCheckBox, 1,0, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(mapFileComboBox, 1, 1);
            parcelMapFileLayout->addWidget(parcelFileMapIndexLabel, 2, 0, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(parcelFileMapLabel, 2, 1, Qt::AlignHCenter);
            parcelMapFileLayout->addWidget(mapIndexSpinBox, 3, 0);
            parcelMapFileLayout->addWidget(mapNameComboBox, 3, 1);
        }
            break;
        default:
            CaretAssert(0);
    }

    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 1, 0);
    layout->addWidget(fileYokeGroupBox);
    layout->addWidget(m_parcelRemappingGroupBox);
    layout->addStretch();
    
    /*
     * TEMP TODO
     * FINISH IMPLEMENTATION OF LOADING AND YOKING
     */
    const bool hideLoadControls = false;
    const bool hideYokeControls = false;
    if (hideLoadControls) {
        loadDimensionLabel->hide();
        m_matrixLoadByColumnRowComboBox->getWidget()->hide();
    }
    if (hideYokeControls) {
        yokeLabel->hide();
        m_matrixYokingGroupComboBox->getWidget()->hide();
    }
    
    return widget;
}

/**
 * Get the matrix related files and properties in this view controller.
 *
 * @param caretMappableDataFileOut
 *    Output with selected caret mappable data file.
 * @param chartableMatrixInterfaceOut
 *    Output with ChartableMatrixInterface implemented by the caret
 *    mappable data file.
 * @param chartMatrixDisplayPropertiesOut
 *    Matrix display properties from the ChartableMatrixInterface.
 * @return True if all output values are valid, else false.
 */
bool
ChartSelectionViewController::getChartMatrixAndProperties(CaretMappableDataFile* &caretMappableDataFileOut,
                                                          ChartableMatrixInterface* & chartableMatrixInterfaceOut,
                                                          ChartMatrixDisplayProperties* &chartMatrixDisplayPropertiesOut)
{
    caretMappableDataFileOut        = NULL;
    chartableMatrixInterfaceOut     = NULL;
    chartMatrixDisplayPropertiesOut = NULL;
    
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return false;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        CaretDataFileSelectionModel* fileSelectionModel = modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
        m_matrixFileSelectionComboBox->updateComboBox(fileSelectionModel);
        
        CaretDataFile* caretFile = fileSelectionModel->getSelectedFile();
        if (caretFile != NULL) {
            chartableMatrixInterfaceOut = dynamic_cast<ChartableMatrixInterface*>(caretFile);
            if (chartableMatrixInterfaceOut != NULL) {
                chartMatrixDisplayPropertiesOut = chartableMatrixInterfaceOut->getChartMatrixDisplayProperties(browserTabIndex);
                caretMappableDataFileOut = chartableMatrixInterfaceOut->getMatrixChartCaretMappableDataFile();
                return true;
            }
        }
    }
    
    return false;
}

/**
 * Gets called when a change is made in the parcel label file remapping 
 * selections.
 */
void
ChartSelectionViewController::parcelLabelFileRemappingFileSelectorChanged()
{
    CaretMappableDataFile*        caretMappableDataFile        = NULL;
    ChartableMatrixInterface*     chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties* chartMatrixDisplayProperties = NULL;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartMatrixDisplayProperties)) {
        return;
    }
    
    const bool remappingEnabled = m_parcelReorderingEnabledCheckBox->isChecked();
    
    CaretMappableDataFileAndMapSelectionModel* model = m_parcelLabelFileRemappingFileSelector->getModel();
    CiftiParcelLabelFile* parcelLabelFile = model->getSelectedFileOfType<CiftiParcelLabelFile>();
    int32_t parcelLabelFileMapIndex = model->getSelectedMapIndex();
    
    chartableMatrixInterface->setSelectedParcelLabelFileAndMapForReordering(parcelLabelFile,
                                                                   parcelLabelFileMapIndex,
                                                                   remappingEnabled);
    
    
    if (remappingEnabled) {
            AString errorMessage;
            if ( ! chartableMatrixInterface->createParcelReordering(parcelLabelFile,
                                                                    parcelLabelFileMapIndex,
                                                                    errorMessage)) {
                WuQMessageBox::errorOk(this,
                                       errorMessage);
            }
    }
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
    CaretMappableDataFile*        caretMappableDataFile        = NULL;
    ChartableMatrixInterface*     chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties* chartMatrixDisplayProperties = NULL;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartMatrixDisplayProperties)) {
        return;
    }
    
    
    
    
    
    CaretDataFileSelectionModel* fileSelectionModel = modelChart->getChartableMatrixFileSelectionModel(browserTabIndex);
    m_matrixFileSelectionComboBox->updateComboBox(fileSelectionModel);
//    m_parcelLabelFileRemappingFileSelector->updateFileAndMapSelector(parcelLabelReMapModel);

//    bool enablePaletteOptions = false;
//    const ChartMatrixDisplayProperties* displayProperties = getChartMatrixDisplayProperties();
//    if (displayProperties != NULL) {
    const ChartMatrixLoadingDimensionEnum::Enum loadType = chartableMatrixInterface->getMatrixLoadingDimension();
    m_matrixLoadByColumnRowComboBox->setSelectedItem<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>(loadType);
        
        const YokingGroupEnum::Enum yokingGroup = chartableMatrixInterface->getYokingGroup();
        m_matrixYokingGroupComboBox->setSelectedItem<YokingGroupEnum,YokingGroupEnum::Enum>(yokingGroup);
        m_matrixColorBarAction->blockSignals(true);
        m_matrixColorBarAction->setChecked(chartMatrixDisplayProperties->isColorBarDisplayed());
        m_matrixColorBarAction->blockSignals(false);
    
    m_matrixYokingGroupComboBox->getWidget()->setEnabled(chartableMatrixInterface->isSupportsLoadingAttributes());
    m_matrixLoadByColumnRowComboBox->getWidget()->setEnabled(chartableMatrixInterface->isSupportsLoadingAttributes());
//    }
    
    /*
     * Update palette reordering.
     */
    std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
    CiftiParcelLabelFile* parcelLabelFile = NULL;
    int32_t parcelLabelFileMapIndex = -1;
    bool remappingEnabled = false;
//    CaretDataFile* cdf = fileSelectionModel->getSelectedFile();
//    if (cdf != NULL) {
//        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(cdf);
//        if (cmdf != NULL) {
//            enablePaletteOptions = cmdf->isMappedWithPalette();
//            
//            ChartableMatrixInterface* matrixInterface = dynamic_cast<ChartableMatrixInterface*>(cmdf);
//            if (matrixInterface != NULL) {
                chartableMatrixInterface->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                                        parcelLabelFile,
                                                                               parcelLabelFileMapIndex,
                                                                               remappingEnabled);
//            }
//        }
//    }
    std::vector<CaretMappableDataFile*> caretMapDataFiles;
    if ( ! parcelLabelFiles.empty()) {
        caretMapDataFiles.insert(caretMapDataFiles.end(),
                              parcelLabelFiles.begin(),
                              parcelLabelFiles.end());
    }
    m_parcelReorderingEnabledCheckBox->setChecked(remappingEnabled);
    CaretMappableDataFileAndMapSelectionModel* model = m_parcelLabelFileRemappingFileSelector->getModel();
    model->overrideAvailableDataFiles(caretMapDataFiles);
    model->setSelectedFile(parcelLabelFile);
    model->setSelectedMapIndex(parcelLabelFileMapIndex);
    m_parcelLabelFileRemappingFileSelector->updateFileAndMapSelector(model);
    
    m_matrixColorBarAction->setEnabled(caretMappableDataFile->isMappedWithPalette());
    m_matrixSettingsAction->setEnabled(caretMappableDataFile->isMappedWithPalette());
    
    const bool showParcelGUI = DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::FLAG_PARCEL_REORDERING);
    m_parcelRemappingGroupBox->setVisible(showParcelGUI);
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
