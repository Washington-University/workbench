
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

#define __CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartMatrixParcelSelectionViewController.h"
#undef __CHART_MATRIX_PARCEL_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QToolButton>

#include "AnnotationColorBar.h"
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
#include "ChartableMatrixSeriesInterface.h"
#include "ChartModel.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "DeveloperFlagsEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventChartMatrixParcelYokingValidation.h"
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

/**
 * \class caret::ChartMatrixParcelSelectionViewController 
 * \brief Handles selection of charts displayed in chart model.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartMatrixParcelSelectionViewController::ChartMatrixParcelSelectionViewController(const Qt::Orientation orientation,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_matrixParcelChartWidget = createMatrixParcelChartWidget(orientation);
    m_parcelRemappingGroupBox = createParcelRemappingWidget(orientation);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 1, 0);
    layout->addWidget(m_matrixParcelChartWidget);
    layout->addWidget(m_parcelRemappingGroupBox);
    //layout->addStretch();
        
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
//    /*
//     * ColorBar Tool Button
//     */
//    QIcon colorBarIcon;
//    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
//                                                           colorBarIcon);
//    m_matrixParcelColorBarAction = WuQtUtilities::createAction("CB",
//                                                               "Display color bar for this overlay",
//                                                               this,
//                                                               this,
//                                                               SLOT(matrixParcelColorBarActionTriggered(bool)));
//    m_matrixParcelColorBarAction->setCheckable(true);
//    if (colorBarIconValid) {
//        m_matrixParcelColorBarAction->setIcon(colorBarIcon);
//    }
//    QToolButton* colorBarToolButton = new QToolButton();
//    colorBarToolButton->setDefaultAction(m_matrixParcelColorBarAction);
//    
//    /*
//     * Settings Tool Button
//     */
//    QLabel* settingsLabel = new QLabel("Settings");
//    QIcon settingsIcon;
//    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
//                                                           settingsIcon);
//    
//    m_matrixParcelSettingsAction = WuQtUtilities::createAction("S",
//                                                               "Edit settings for this map and overlay",
//                                                               this,
//                                                               this,
//                                                               SLOT(matrixParcelSettingsActionTriggered()));
//    if (settingsIconValid) {
//        m_matrixParcelSettingsAction->setIcon(settingsIcon);
//    }
//    QToolButton* settingsToolButton = new QToolButton();
//    settingsToolButton->setDefaultAction(m_matrixParcelSettingsAction);
//    
//    
//    QLabel* fileLabel = new QLabel("Matrix File");
//    m_matrixParcelFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
//    QObject::connect(m_matrixParcelFileSelectionComboBox, SIGNAL(fileSelected(CaretDataFile*)),
//                     this, SLOT(matrixParcelFileSelected(CaretDataFile*)));
//    
//    QLabel* loadDimensionLabel = new QLabel("Load By");
//    m_matrixParcelLoadByColumnRowComboBox = new EnumComboBoxTemplate(this);
//    m_matrixParcelLoadByColumnRowComboBox->setup<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>();
//    QObject::connect(m_matrixParcelLoadByColumnRowComboBox, SIGNAL(itemActivated()),
//                     this, SLOT(matrixParcelFileLoadingComboBoxActivated()));
//    
//    
//    QLabel* yokeLabel = new QLabel("Yoke ");
//    m_matrixParcelYokingGroupComboBox = new EnumComboBoxTemplate(this);
//    m_matrixParcelYokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
//    QObject::connect(m_matrixParcelYokingGroupComboBox, SIGNAL(itemActivated()),
//                     this, SLOT(matrixParcelYokingGroupEnumComboBoxActivated()));
//    
//    m_matrixParcelChartWidget = new QGroupBox("Matrix Loading");
//    QGridLayout* matrixLayout = new QGridLayout(m_matrixParcelChartWidget);
//    
//    switch (orientation) {
//        case Qt::Horizontal:
//        {
//            WuQtUtilities::setLayoutSpacingAndMargins(matrixLayout, 2, 0);
//            matrixLayout->setColumnStretch(0, 0);
//            matrixLayout->setColumnStretch(1, 0);
//            matrixLayout->setColumnStretch(2, 0);
//            matrixLayout->setColumnStretch(3, 0);
//            matrixLayout->setColumnStretch(4, 100);
//            
//            matrixLayout->addWidget(loadDimensionLabel,
//                                      0, 0,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(settingsLabel,
//                                      0, 1,
//                                      1, 2,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(yokeLabel,
//                                      0, 3,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(fileLabel,
//                                      0, 4,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(m_matrixParcelLoadByColumnRowComboBox->getWidget(),
//                                      1, 0);
//            matrixLayout->addWidget(settingsToolButton,
//                                      1, 1);
//            matrixLayout->addWidget(colorBarToolButton,
//                                      1, 2);
//            matrixLayout->addWidget(m_matrixParcelYokingGroupComboBox->getWidget(),
//                                      1, 3);
//            matrixLayout->addWidget(m_matrixParcelFileSelectionComboBox->getWidget(),
//                                      1, 4);
//        }
//            break;
//        case Qt::Vertical:
//        {
//            WuQtUtilities::setLayoutSpacingAndMargins(matrixLayout, 2, 0);
//            matrixLayout->setColumnStretch(0, 0);
//            matrixLayout->setColumnStretch(1, 0);
//            matrixLayout->setColumnStretch(2, 0);
//            matrixLayout->setColumnStretch(3, 0);
//            matrixLayout->setColumnStretch(4, 100);
//            
//            matrixLayout->addWidget(loadDimensionLabel,
//                                      0, 0,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(settingsLabel,
//                                      0, 1,
//                                      1, 2,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(yokeLabel,
//                                      0, 3,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(m_matrixParcelLoadByColumnRowComboBox->getWidget(),
//                                      1, 0);
//            matrixLayout->addWidget(settingsToolButton,
//                                      1, 1);
//            matrixLayout->addWidget(colorBarToolButton,
//                                      1, 2);
//            matrixLayout->addWidget(m_matrixParcelYokingGroupComboBox->getWidget(),
//                                      1, 3);
//            matrixLayout->addWidget(fileLabel,
//                                      2, 0, 1, 4,
//                                      Qt::AlignHCenter);
//            matrixLayout->addWidget(m_matrixParcelFileSelectionComboBox->getWidget(),
//                                      3, 0, 1, 4);
//        }
//            break;
//        default:
//            CaretAssert(0);
//            break;
//    }
//    
//    m_parcelReorderingEnabledCheckBox = new QCheckBox("");
//    QObject::connect(m_parcelReorderingEnabledCheckBox, SIGNAL(clicked(bool)),
//                     this, SLOT(parcelLabelFileRemappingFileSelectorChanged()));
//    
//    m_parcelLabelFileRemappingFileSelector = new CaretMappableDataFileAndMapSelectorObject(DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL,
//                                                                                           CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
//                                                                                           this);
//    QObject::connect(m_parcelLabelFileRemappingFileSelector, SIGNAL(selectionWasPerformed()),
//                     this, SLOT(parcelLabelFileRemappingFileSelectorChanged()));
//    
//    QLabel* parcelCheckBoxLabel = new QLabel("On");
//    QLabel* parcelFileLabel = new QLabel("Parcel Label File");
//    QLabel* parcelFileMapLabel = new QLabel("Map");
//    QLabel* parcelFileMapIndexLabel = new QLabel("Index");
//    QWidget* mapFileComboBox = NULL;
//    QWidget* mapIndexSpinBox = NULL;
//    QWidget* mapNameComboBox = NULL;
//    m_parcelLabelFileRemappingFileSelector->getWidgetsForAddingToLayout(mapFileComboBox,
//                                                                        mapIndexSpinBox,
//                                                                        mapNameComboBox);
//    m_parcelRemappingGroupBox = new QGroupBox("Parcel Reordering");
//    m_parcelRemappingGroupBox->setFlat(true);
//    m_parcelRemappingGroupBox->setAlignment(Qt::AlignHCenter);
//    QGridLayout* parcelMapFileLayout = new QGridLayout(m_parcelRemappingGroupBox);
//    switch (orientation) {
//        case Qt::Horizontal:
//        {
//            WuQtUtilities::setLayoutSpacingAndMargins(parcelMapFileLayout, 2, 0);
//            parcelMapFileLayout->setColumnStretch(0,   0);
//            parcelMapFileLayout->setColumnStretch(1, 100);
//            parcelMapFileLayout->setColumnStretch(2,   0);
//            parcelMapFileLayout->setColumnStretch(3, 100);
//            parcelMapFileLayout->addWidget(parcelCheckBoxLabel, 0, 0, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(parcelFileLabel, 0, 1, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(parcelFileMapLabel, 0, 2, 1, 2, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(m_parcelReorderingEnabledCheckBox, 1,0);
//            parcelMapFileLayout->addWidget(mapFileComboBox, 1, 1);
//            parcelMapFileLayout->addWidget(mapIndexSpinBox, 1, 2);
//            parcelMapFileLayout->addWidget(mapNameComboBox, 1, 3);
//        }
//            break;
//        case Qt::Vertical:
//        {
//            WuQtUtilities::setLayoutSpacingAndMargins(parcelMapFileLayout, 2, 0);
//            parcelMapFileLayout->setColumnStretch(0,   0);
//            parcelMapFileLayout->setColumnStretch(1, 100);
//            parcelMapFileLayout->addWidget(parcelCheckBoxLabel, 0, 0, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(parcelFileLabel, 0, 1, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(m_parcelReorderingEnabledCheckBox, 1,0, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(mapFileComboBox, 1, 1);
//            parcelMapFileLayout->addWidget(parcelFileMapIndexLabel, 2, 0, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(parcelFileMapLabel, 2, 1, Qt::AlignHCenter);
//            parcelMapFileLayout->addWidget(mapIndexSpinBox, 3, 0);
//            parcelMapFileLayout->addWidget(mapNameComboBox, 3, 1);
//        }
//            break;
//        default:
//            CaretAssert(0);
//    }
//    
//    
//    QWidget* widget = new QWidget(this);
//    QVBoxLayout* layout = new QVBoxLayout(widget);
//    WuQtUtilities::setLayoutSpacingAndMargins(layout, 1, 0);
//    layout->addWidget(m_matrixParcelChartWidget);
//    layout->addWidget(m_parcelRemappingGroupBox);
//    //layout->addStretch();
//    
//    /*
//     * TEMP TODO
//     * FINISH IMPLEMENTATION OF LOADING AND YOKING
//     */
//    const bool hideLoadControls = false;
//    const bool hideYokeControls = false;
//    if (hideLoadControls) {
//        loadDimensionLabel->hide();
//        m_matrixParcelLoadByColumnRowComboBox->getWidget()->hide();
//    }
//    if (hideYokeControls) {
//        yokeLabel->hide();
//        m_matrixParcelYokingGroupComboBox->getWidget()->hide();
//    }
//    
//    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartMatrixParcelSelectionViewController::~ChartMatrixParcelSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the view controller.
 */
void
ChartMatrixParcelSelectionViewController::updateSelectionViewController()
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
    
    if (chartDataType == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER) {
            updateMatrixParcelChartWidget(brain,
                                    modelChart,
                                    browserTabIndex);
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartMatrixParcelSelectionViewController::receiveEvent(Event* event)
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
 * Called when a matrix file is selected.
 *
 * @param caretDataFile
 *    Caret data file that was selected.
 */
void
ChartMatrixParcelSelectionViewController::matrixParcelFileSelected(CaretDataFile* /*caretDataFile*/)
{
    updateSelectionViewController();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Gets called when matrix loading combo box is changed.
 */
void
ChartMatrixParcelSelectionViewController::matrixParcelFileLoadingComboBoxActivated()
{
    CaretMappableDataFile*          caretMappableDataFile        = NULL;
    ChartableMatrixInterface*       chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties*   chartMatrixDisplayProperties = NULL;
    ChartableMatrixParcelInterface* chartableMatrixParcelInterface = NULL;
    ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface = NULL;
    int32_t browserTabIndex = -1;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartableMatrixParcelInterface,
                                       chartableMatrixSeriesInterface,
                                       chartMatrixDisplayProperties,
                                       browserTabIndex)) {
        return;
    }
    
    CaretAssert(chartableMatrixParcelInterface);
    
    chartableMatrixParcelInterface->setMatrixLoadingDimension(m_matrixParcelLoadByColumnRowComboBox->getSelectedItem<ChartMatrixLoadingDimensionEnum,
                                                   ChartMatrixLoadingDimensionEnum::Enum>());
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Gets called when yoking gruup is changed.
 */
void
ChartMatrixParcelSelectionViewController::matrixParcelYokingGroupEnumComboBoxActivated()
{
    CaretMappableDataFile*          caretMappableDataFile        = NULL;
    ChartableMatrixInterface*       chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties*   chartMatrixDisplayProperties = NULL;
    ChartableMatrixParcelInterface* chartableMatrixParcelInterface = NULL;
    ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface = NULL;
    int32_t browserTabIndex = -1;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartableMatrixParcelInterface,
                                       chartableMatrixSeriesInterface,
                                       chartMatrixDisplayProperties,
                                       browserTabIndex)) {
        return;
    }
    
    CaretAssert(chartableMatrixParcelInterface);
    
    YokingGroupEnum::Enum newYokingGroup = m_matrixParcelYokingGroupComboBox->getSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>();
    int32_t selectedRowColumnIndex = -1;
    if (newYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
        const YokingGroupEnum::Enum previousYokingGroup = chartableMatrixParcelInterface->getYokingGroup();
        
        EventChartMatrixParcelYokingValidation yokeEvent(chartableMatrixParcelInterface,
                                                newYokingGroup);
        EventManager::get()->sendEvent(yokeEvent.getPointer());
        AString message;
        if ( ! yokeEvent.isValidateYokingCompatible(message,
                                                    selectedRowColumnIndex)) {
            message = WuQtUtilities::createWordWrappedToolTipText(message);
            
            WuQMessageBox::YesNoCancelResult result =
            WuQMessageBox::warningYesNoCancel(m_matrixParcelYokingGroupComboBox->getWidget(),
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
    m_matrixParcelYokingGroupComboBox->setSelectedItem<YokingGroupEnum,YokingGroupEnum::Enum>(newYokingGroup);
    
    chartableMatrixParcelInterface->setYokingGroup(newYokingGroup);
    
    /*
     * If yoking changed update the file's selected row or column
     */
    if (newYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
        if (selectedRowColumnIndex >= 0) {
            CiftiMappableConnectivityMatrixDataFile* matrixFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(chartableMatrixInterface);
            if (matrixFile != NULL) {
                switch (chartableMatrixParcelInterface->getMatrixLoadingDimension()) {
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
ChartMatrixParcelSelectionViewController::matrixParcelColorBarActionTriggered(bool status)
{
    CaretMappableDataFile*          caretMappableDataFile        = NULL;
    ChartableMatrixInterface*       chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties*   chartMatrixDisplayProperties = NULL;
    ChartableMatrixParcelInterface* chartableMatrixParcelInterface = NULL;
    ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface = NULL;
    int32_t browserTabIndex = -1;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartableMatrixParcelInterface,
                                       chartableMatrixSeriesInterface,
                                       chartMatrixDisplayProperties,
                                       browserTabIndex)) {
        return;
    }
    
    chartMatrixDisplayProperties->getColorBar()->setDisplayed(status);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when settings icon button is clicked to display palette editor.
 */
void
ChartMatrixParcelSelectionViewController::matrixParcelSettingsActionTriggered()
{
    CaretMappableDataFile*          caretMappableDataFile        = NULL;
    ChartableMatrixInterface*       chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties*   chartMatrixDisplayProperties = NULL;
    ChartableMatrixParcelInterface* chartableMatrixParcelInterface = NULL;
    ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface = NULL;
    int32_t browserTabIndex = -1;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartableMatrixParcelInterface,
                                       chartableMatrixSeriesInterface,
                                       chartMatrixDisplayProperties,
                                       browserTabIndex)) {
        return;
    }

    const int32_t mapIndex = 0;
    EventPaletteColorMappingEditorDialogRequest dialogEvent(m_browserWindowIndex,
                                                            caretMappableDataFile,
                                                            mapIndex);
    EventManager::get()->sendEvent(dialogEvent.getPointer());
}

/**
 * @param orientation
 *     Orientation for the widget.
 * @return 
 *     The matrix chart widget.
 */
QGroupBox*
ChartMatrixParcelSelectionViewController::createMatrixParcelChartWidget(const Qt::Orientation orientation)
{
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    m_matrixParcelColorBarAction = WuQtUtilities::createAction("CB",
                                                       "Display color bar for this overlay",
                                                       this,
                                                       this,
                                                       SLOT(matrixParcelColorBarActionTriggered(bool)));
    m_matrixParcelColorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        m_matrixParcelColorBarAction->setIcon(colorBarIcon);
    }
    QToolButton* colorBarToolButton = new QToolButton();
    colorBarToolButton->setDefaultAction(m_matrixParcelColorBarAction);
    
    /*
     * Settings Tool Button
     */
    QLabel* settingsLabel = new QLabel("Settings");
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);
    
    m_matrixParcelSettingsAction = WuQtUtilities::createAction("S",
                                                       "Edit settings for this map and overlay",
                                                       this,
                                                       this,
                                                       SLOT(matrixParcelSettingsActionTriggered()));
    if (settingsIconValid) {
        m_matrixParcelSettingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(m_matrixParcelSettingsAction);
    
    
    QLabel* fileLabel = new QLabel("Matrix File");
    m_matrixParcelFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_matrixParcelFileSelectionComboBox, SIGNAL(fileSelected(CaretDataFile*)),
                     this, SLOT(matrixParcelFileSelected(CaretDataFile*)));
    
    QLabel* loadDimensionLabel = new QLabel("Load By");
    m_matrixParcelLoadByColumnRowComboBox = new EnumComboBoxTemplate(this);
    m_matrixParcelLoadByColumnRowComboBox->setup<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>();
    QObject::connect(m_matrixParcelLoadByColumnRowComboBox, SIGNAL(itemActivated()),
                     this, SLOT(matrixParcelFileLoadingComboBoxActivated()));

    
    QLabel* yokeLabel = new QLabel("Yoke ");
    m_matrixParcelYokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_matrixParcelYokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
    QObject::connect(m_matrixParcelYokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(matrixParcelYokingGroupEnumComboBoxActivated()));
    
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
            fileYokeLayout->addWidget(m_matrixParcelLoadByColumnRowComboBox->getWidget(),
                                      1, 0);
            fileYokeLayout->addWidget(settingsToolButton,
                                      1, 1);
            fileYokeLayout->addWidget(colorBarToolButton,
                                      1, 2);
            fileYokeLayout->addWidget(m_matrixParcelYokingGroupComboBox->getWidget(),
                                      1, 3);
            fileYokeLayout->addWidget(m_matrixParcelFileSelectionComboBox->getWidget(),
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
            fileYokeLayout->addWidget(m_matrixParcelLoadByColumnRowComboBox->getWidget(),
                                      1, 0);
            fileYokeLayout->addWidget(settingsToolButton,
                                      1, 1);
            fileYokeLayout->addWidget(colorBarToolButton,
                                      1, 2);
            fileYokeLayout->addWidget(m_matrixParcelYokingGroupComboBox->getWidget(),
                                      1, 3);
            fileYokeLayout->addWidget(fileLabel,
                                      2, 0, 1, 4,
                                      Qt::AlignHCenter);
            fileYokeLayout->addWidget(m_matrixParcelFileSelectionComboBox->getWidget(),
                                      3, 0, 1, 4);
        }
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    return fileYokeGroupBox;
}

/**
 * @param orientation
 *     Orientation for the widget.
 * @return
 *     The parcel remapping widget.
 */
QGroupBox*
ChartMatrixParcelSelectionViewController::createParcelRemappingWidget(const Qt::Orientation orientation)
{
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
    QGroupBox* groupBox = new QGroupBox("Parcel Reordering");
    groupBox->setFlat(true);
    groupBox->setAlignment(Qt::AlignHCenter);
    QGridLayout* parcelMapFileLayout = new QGridLayout(groupBox);
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
    }
    
    return groupBox;
}

/**
 * Get the matrix related files and properties in this view controller.
 *
 * @param caretMappableDataFileOut
 *    Output with selected caret mappable data file.
 * @param chartableMatrixInterfaceOut
 *    Output with ChartableMatrixInterface implemented by the caret
 *    mappable data file.
 * @param chartableMatrixParcelInterfaceOut
 *    Output with ChartableMatrixParcelInterfaceOut implemented by the
 *    caret mappable data file (may be NULL).
 * @param chartableMatrixSeriesInterfaceOut
 *    Output with ChartableMatrixSeriesInterfaceOut implemented by the 
 *    caret mappable data file (may be NULL).
 * @param browserTabIndexOut
 *    Index selected tab.
 * @param chartMatrixDisplayPropertiesOut
 *    Matrix display properties from the ChartableMatrixInterface.
 * @return True if all output values are valid, else false.
 */
bool
ChartMatrixParcelSelectionViewController::getChartMatrixAndProperties(CaretMappableDataFile* &caretMappableDataFileOut,
                                                          ChartableMatrixInterface* & chartableMatrixInterfaceOut,
                                                          ChartableMatrixParcelInterface* &chartableMatrixParcelInterfaceOut,
                                                          ChartableMatrixSeriesInterface* &chartableMatrixSeriesInterfaceOut,
                                                          ChartMatrixDisplayProperties* &chartMatrixDisplayPropertiesOut,
                                                          int32_t& browserTabIndexOut)
{
    caretMappableDataFileOut          = NULL;
    chartableMatrixInterfaceOut       = NULL;
    chartableMatrixParcelInterfaceOut = NULL;
    chartableMatrixSeriesInterfaceOut = NULL;
    chartMatrixDisplayPropertiesOut   = NULL;
    browserTabIndexOut                = -1;
    
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return false;
    }
    browserTabIndexOut = browserTabContent->getTabNumber();
    
    if (browserTabIndexOut < 0) {
        return false;
    }
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        switch (modelChart->getSelectedChartOneDataType(browserTabIndexOut)) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
            {
                CaretDataFileSelectionModel* parcelFileSelectionModel = modelChart->getChartableMatrixParcelFileSelectionModel(browserTabIndexOut);
                //m_matrixParcelFileSelectionComboBox->updateComboBox(parcelFileSelectionModel);
                CaretDataFile* caretParcelFile = parcelFileSelectionModel->getSelectedFile();
                
                if (caretParcelFile != NULL) {
                    chartableMatrixInterfaceOut = dynamic_cast<ChartableMatrixInterface*>(caretParcelFile);
                    if (chartableMatrixInterfaceOut != NULL) {
                        chartableMatrixParcelInterfaceOut = dynamic_cast<ChartableMatrixParcelInterface*>(caretParcelFile);
                        chartMatrixDisplayPropertiesOut = chartableMatrixInterfaceOut->getChartMatrixDisplayProperties(browserTabIndexOut);
                        caretMappableDataFileOut = chartableMatrixInterfaceOut->getMatrixChartCaretMappableDataFile();
                        return true;
                    }
                }
            }
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
            {
                CaretDataFileSelectionModel* seriesFileSelectionModel = modelChart->getChartableMatrixSeriesFileSelectionModel(browserTabIndexOut);
                CaretDataFile* caretSeriesFile = seriesFileSelectionModel->getSelectedFile();
                
                
                if (caretSeriesFile != NULL) {
                    chartableMatrixInterfaceOut = dynamic_cast<ChartableMatrixInterface*>(caretSeriesFile);
                    if (chartableMatrixInterfaceOut != NULL) {
                        chartableMatrixSeriesInterfaceOut = dynamic_cast<ChartableMatrixSeriesInterface*>(caretSeriesFile);
                        chartMatrixDisplayPropertiesOut = chartableMatrixInterfaceOut->getChartMatrixDisplayProperties(browserTabIndexOut);
                        caretMappableDataFileOut = chartableMatrixInterfaceOut->getMatrixChartCaretMappableDataFile();
                        return true;
                    }
                }
            }
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                break;
        }
        
//        CaretDataFileSelectionModel* parcelFileSelectionModel = modelChart->getChartableMatrixParcelFileSelectionModel(browserTabIndexOut);
//        //m_matrixParcelFileSelectionComboBox->updateComboBox(parcelFileSelectionModel);
//        CaretDataFile* caretParcelFile = parcelFileSelectionModel->getSelectedFile();
//
//        CaretDataFileSelectionModel* seriesFileSelectionModel = modelChart->getChartableMatrixSeriesFileSelectionModel(browserTabIndexOut);
//        CaretDataFile* caretSeriesFile = seriesFileSelectionModel->getSelectedFile();
//        
//        if (caretParcelFile != NULL) {
//            chartableMatrixInterfaceOut = dynamic_cast<ChartableMatrixInterface*>(caretParcelFile);
//            if (chartableMatrixInterfaceOut != NULL) {
//                chartableMatrixParcelInterfaceOut = dynamic_cast<ChartableMatrixParcelInterface*>(caretParcelFile);
//                chartableMatrixSeriesInterfaceOut = dynamic_cast<ChartableMatrixSeriesInterface*>(caretParcelFile);
//                chartMatrixDisplayPropertiesOut = chartableMatrixInterfaceOut->getChartMatrixDisplayProperties(browserTabIndexOut);
//                caretMappableDataFileOut = chartableMatrixInterfaceOut->getMatrixChartCaretMappableDataFile();
//                return true;
//            }
//        }
    }
    
    return false;
}

/**
 * Gets called when a change is made in the parcel label file remapping 
 * selections.
 */
void
ChartMatrixParcelSelectionViewController::parcelLabelFileRemappingFileSelectorChanged()
{
    CaretMappableDataFile*          caretMappableDataFile        = NULL;
    ChartableMatrixInterface*       chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties*   chartMatrixDisplayProperties = NULL;
    ChartableMatrixParcelInterface* chartableMatrixParcelInterface = NULL;
    ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface = NULL;
    int32_t browserTabIndex = -1;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartableMatrixParcelInterface,
                                       chartableMatrixSeriesInterface,
                                       chartMatrixDisplayProperties,
                                       browserTabIndex)) {
        return;
    }
    
    CaretAssert(chartableMatrixParcelInterface);
    
    const bool remappingEnabled = m_parcelReorderingEnabledCheckBox->isChecked();
    
    CaretMappableDataFileAndMapSelectionModel* model = m_parcelLabelFileRemappingFileSelector->getModel();
    CiftiParcelLabelFile* parcelLabelFile = model->getSelectedFileOfType<CiftiParcelLabelFile>();
    int32_t parcelLabelFileMapIndex = model->getSelectedMapIndex();
    
    chartableMatrixParcelInterface->setSelectedParcelLabelFileAndMapForReordering(parcelLabelFile,
                                                                   parcelLabelFileMapIndex,
                                                                   remappingEnabled);
    
    
    if (remappingEnabled) {
            AString errorMessage;
            if ( ! chartableMatrixParcelInterface->createParcelReordering(parcelLabelFile,
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
ChartMatrixParcelSelectionViewController::updateMatrixParcelChartWidget(Brain* /* brain */,
                                                      ModelChart* modelChart,
                                                      const int32_t /*browserTabIndex*/)
{
    CaretMappableDataFile*          caretMappableDataFile        = NULL;
    ChartableMatrixInterface*       chartableMatrixInterface     = NULL;
    ChartMatrixDisplayProperties*   chartMatrixDisplayProperties = NULL;
    ChartableMatrixParcelInterface* chartableMatrixParcelInterface = NULL;
    ChartableMatrixSeriesInterface* chartableMatrixSeriesInterface = NULL;
    int32_t browserTabIndex = -1;
    if ( ! getChartMatrixAndProperties(caretMappableDataFile,
                                       chartableMatrixInterface,
                                       chartableMatrixParcelInterface,
                                       chartableMatrixSeriesInterface,
                                       chartMatrixDisplayProperties,
                                       browserTabIndex)) {
        return;
    }
    
    if (chartableMatrixParcelInterface != NULL) {
        CaretDataFileSelectionModel* fileSelectionModel = modelChart->getChartableMatrixParcelFileSelectionModel(browserTabIndex);
        m_matrixParcelFileSelectionComboBox->updateComboBox(fileSelectionModel);
        const ChartMatrixLoadingDimensionEnum::Enum loadType = chartableMatrixParcelInterface->getMatrixLoadingDimension();
        m_matrixParcelLoadByColumnRowComboBox->setSelectedItem<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>(loadType);
        
        const YokingGroupEnum::Enum yokingGroup = chartableMatrixParcelInterface->getYokingGroup();
        m_matrixParcelYokingGroupComboBox->setSelectedItem<YokingGroupEnum,YokingGroupEnum::Enum>(yokingGroup);
        m_matrixParcelColorBarAction->blockSignals(true);
        m_matrixParcelColorBarAction->setChecked(chartMatrixDisplayProperties->getColorBar()->isDisplayed());
        m_matrixParcelColorBarAction->blockSignals(false);
        
        m_matrixParcelYokingGroupComboBox->getWidget()->setEnabled(chartableMatrixParcelInterface->isSupportsLoadingAttributes());
        m_matrixParcelLoadByColumnRowComboBox->getWidget()->setEnabled(chartableMatrixParcelInterface->isSupportsLoadingAttributes());
        
        /*
         * Update palette reordering.
         */
        std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
        CiftiParcelLabelFile* parcelLabelFile = NULL;
        int32_t parcelLabelFileMapIndex = -1;
        bool remappingEnabled = false;
        chartableMatrixParcelInterface->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                                      parcelLabelFile,
                                                                                      parcelLabelFileMapIndex,
                                                                                      remappingEnabled);
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
        
        bool reorderCheckBoxEnabledFlag = false;
        if (model->getSelectedFile() != NULL) {
            if ((model->getSelectedMapIndex() >= 0)
                && (model->getSelectedMapIndex() < model->getSelectedFile()->getNumberOfMaps())) {
                reorderCheckBoxEnabledFlag = true;
            }
        }
        m_parcelReorderingEnabledCheckBox->setEnabled(reorderCheckBoxEnabledFlag);
        
        m_matrixParcelColorBarAction->setEnabled(caretMappableDataFile->isMappedWithPalette());
        m_matrixParcelSettingsAction->setEnabled(caretMappableDataFile->isMappedWithPalette());
        
        m_parcelRemappingGroupBox->setVisible(true);
    }
    
    m_parcelRemappingGroupBox->setEnabled(chartableMatrixParcelInterface != NULL);
}


