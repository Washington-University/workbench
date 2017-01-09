
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

#define __CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartMatrixSeriesSelectionViewController.h"
#undef __CHART_MATRIX_SERIES_SELECTION_VIEW_CONTROLLER_DECLARE__

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
#include "ChartModel.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiScalarDataSeriesFile.h"
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
#include "MapYokingGroupComboBox.h"
#include "ModelChart.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ChartMatrixSeriesSelectionViewController 
 * \brief Handles selection of charts displayed in chart model.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartMatrixSeriesSelectionViewController::ChartMatrixSeriesSelectionViewController(const Qt::Orientation orientation,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    m_matrixSeriesColorBarAction = WuQtUtilities::createAction("CB",
                                                               "Display color bar for this overlay",
                                                               this,
                                                               this,
                                                               SLOT(matrixSeriesColorBarActionTriggered(bool)));
    m_matrixSeriesColorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        m_matrixSeriesColorBarAction->setIcon(colorBarIcon);
    }
    QToolButton* colorBarToolButton = new QToolButton();
    colorBarToolButton->setDefaultAction(m_matrixSeriesColorBarAction);
    
    /*
     * Settings Tool Button
     */
    QLabel* settingsLabel = new QLabel("Settings");
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);
    
    m_matrixSeriesSettingsAction = WuQtUtilities::createAction("S",
                                                               "Edit settings for this map and overlay",
                                                               this,
                                                               this,
                                                               SLOT(matrixSeriesSettingsActionTriggered()));
    if (settingsIconValid) {
        m_matrixSeriesSettingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(m_matrixSeriesSettingsAction);
    
    
    QLabel* fileLabel = new QLabel("Matrix File");
    m_matrixSeriesFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_matrixSeriesFileSelectionComboBox, SIGNAL(fileSelected(CaretDataFile*)),
                     this, SLOT(matrixSeriesFileSelected(CaretDataFile*)));
    
    /*
     * Yoking Group
     */
    QLabel* yokeLabel = new QLabel("Yoke ");
    m_matrixSeriesYokingComboBox = new MapYokingGroupComboBox(this);
    m_matrixSeriesYokingComboBox->getWidget()->setStatusTip("Synchronize enabled status and map indices)");
    m_matrixSeriesYokingComboBox->getWidget()->setToolTip("Yoke to Overlay Mapped Files");
#ifdef CARET_OS_MACOSX
    m_matrixSeriesYokingComboBox->getWidget()->setFixedWidth(m_matrixSeriesYokingComboBox->getWidget()->sizeHint().width() - 20);
#endif // CARET_OS_MACOSX
    QObject::connect(m_matrixSeriesYokingComboBox, SIGNAL(itemActivated()),
                     this, SLOT(matrixSeriesYokingGroupActivated()));

    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    
    switch (orientation) {
        case Qt::Horizontal:
        {
            WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
            gridLayout->setColumnStretch(0, 0);
            gridLayout->setColumnStretch(1, 0);
            gridLayout->setColumnStretch(2, 0);
            gridLayout->setColumnStretch(3, 100);
            
            gridLayout->addWidget(settingsLabel,
                                      0, 0,
                                      1, 2,
                                      Qt::AlignHCenter);
            gridLayout->addWidget(yokeLabel,
                                      0, 2,
                                      Qt::AlignHCenter);
            gridLayout->addWidget(fileLabel,
                                      0, 3,
                                      Qt::AlignHCenter);
            gridLayout->addWidget(settingsToolButton,
                                      1, 0);
            gridLayout->addWidget(colorBarToolButton,
                                      1, 1);
            gridLayout->addWidget(m_matrixSeriesYokingComboBox->getWidget(),
                                      1, 2);
            gridLayout->addWidget(m_matrixSeriesFileSelectionComboBox->getWidget(),
                                      1, 3);
        }
            break;
        case Qt::Vertical:
        {
            WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
            gridLayout->setColumnStretch(0, 0);
            gridLayout->setColumnStretch(1, 0);
            gridLayout->setColumnStretch(2, 0);
            gridLayout->setColumnStretch(3, 100);
            
            gridLayout->addWidget(settingsLabel,
                                      0, 0,
                                      1, 2,
                                      Qt::AlignHCenter);
            gridLayout->addWidget(yokeLabel,
                                      0, 2,
                                      Qt::AlignHCenter);
            gridLayout->addWidget(settingsToolButton,
                                      1, 0);
            gridLayout->addWidget(colorBarToolButton,
                                      1, 1);
            gridLayout->addWidget(m_matrixSeriesYokingComboBox->getWidget(),
                                      1, 2);
            gridLayout->addWidget(fileLabel,
                                      2, 0, 1, 4,
                                      Qt::AlignHCenter);
            gridLayout->addWidget(m_matrixSeriesFileSelectionComboBox->getWidget(),
                                      3, 0, 1, 4);
        }
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    gridWidget->setSizePolicy(gridWidget->sizePolicy().horizontalPolicy(),
                              QSizePolicy::Fixed);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 1, 0);
    layout->addWidget(gridWidget);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartMatrixSeriesSelectionViewController::~ChartMatrixSeriesSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the view controller.
 */
void
ChartMatrixSeriesSelectionViewController::updateSelectionViewController()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    ChartVersionOneDataTypeEnum::Enum chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartDataType = modelChart->getSelectedChartOneDataType(browserTabIndex);
    }
    
    if (chartDataType == ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES) {
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
        
        if (chartableMatrixSeriesInterface != NULL) {
            CaretDataFileSelectionModel* fileSelectionModel = modelChart->getChartableMatrixSeriesFileSelectionModel(browserTabIndex);
            m_matrixSeriesFileSelectionComboBox->updateComboBox(fileSelectionModel);
            
            const MapYokingGroupEnum::Enum yokingGroup = chartableMatrixSeriesInterface->getMatrixRowColumnMapYokingGroup(browserTabIndex);
            m_matrixSeriesYokingComboBox->setMapYokingGroup(yokingGroup);
            
            m_matrixSeriesColorBarAction->blockSignals(true);
            m_matrixSeriesColorBarAction->setChecked(chartMatrixDisplayProperties->getColorBar()->isDisplayed());
            m_matrixSeriesColorBarAction->blockSignals(false);
            
            m_matrixSeriesColorBarAction->setEnabled(caretMappableDataFile->isMappedWithPalette());
            m_matrixSeriesSettingsAction->setEnabled(caretMappableDataFile->isMappedWithPalette());
        }
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartMatrixSeriesSelectionViewController::receiveEvent(Event* event)
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
ChartMatrixSeriesSelectionViewController::getChartMatrixAndProperties(CaretMappableDataFile* &caretMappableDataFileOut,
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
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
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
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
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
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                break;
        }
    }
    
    return false;
}

/**
 * Called when a matrix series file is selected.
 *
 * @param caretDataFile
 *    Caret data file that was selected.
 */
void
ChartMatrixSeriesSelectionViewController::matrixSeriesFileSelected(CaretDataFile* /*caretDataFile*/)
{
    updateSelectionViewController();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Called when colorbar icon button is clicked for matrix series file.
 */
void
ChartMatrixSeriesSelectionViewController::matrixSeriesColorBarActionTriggered(bool status)
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
    
    if (chartableMatrixSeriesInterface != NULL) {
        chartMatrixDisplayProperties->getColorBar()->setDisplayed(status);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when settings icon button is clicked to display palette editor
 * for matrix series file.
 */
void
ChartMatrixSeriesSelectionViewController::matrixSeriesSettingsActionTriggered()
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
    
    if (chartableMatrixSeriesInterface != NULL) {
        const int32_t mapIndex = 0;
        EventPaletteColorMappingEditorDialogRequest dialogEvent(m_browserWindowIndex,
                                                                caretMappableDataFile,
                                                                mapIndex);
        EventManager::get()->sendEvent(dialogEvent.getPointer());
    }
}

/**
 * Called when matrix series yoking group is changed.
 */
void
ChartMatrixSeriesSelectionViewController::matrixSeriesYokingGroupActivated()
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
    
    m_matrixSeriesYokingComboBox->validateYokingChange(chartableMatrixSeriesInterface,
                                                       browserTabIndex);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

