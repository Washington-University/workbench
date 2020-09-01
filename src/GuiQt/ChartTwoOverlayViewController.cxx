
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __CHART_TWO_OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "ChartTwoOverlayViewController.h"
#undef __CHART_TWO_OVERLAY_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QSpinBox>
#include <QToolButton>
#include <QWidgetAction>

#include "AnnotationColorBar.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CardinalDirectionEnumMenu.h"
#include "CaretAssert.h"
#include "CaretColorToolButton.h"
#include "CaretColorEnumMenu.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoLineLayerNormalizationWidget.h"
#include "ChartTwoOverlay.h"
#include "ChartableTwoFileDelegate.h"
#include "CursorDisplayScoped.h"
#include "ElapsedTimer.h"
#include "EnumComboBoxTemplate.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "EventProgressUpdate.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "ProgressReportingDialog.h"
#include "UsernamePasswordWidget.h"
#include "WuQDoubleSpinBox.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ChartTwoOverlayViewController 
 * \brief View controller for a chart overlay
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param orientation
 *    Orientation of view controller.
 * @param browserWindowIndex
 *    Index of browser window in which this view controller resides.
 * @param chartOverlayIndex
 *    Index of this overlay view controller.
 * @param parentObjectName
 *    Name of parent object for macros
 * @param parent
 *    The parent widget.
 */
ChartTwoOverlayViewController::ChartTwoOverlayViewController(const Qt::Orientation orientation,
                                                             const int32_t browserWindowIndex,
                                                             const int32_t chartOverlayIndex,
                                                             const QString& parentObjectName,
                                                             QObject* parent)
: QObject(parent),
m_browserWindowIndex(browserWindowIndex),
m_chartOverlayIndex(chartOverlayIndex),
m_chartOverlay(NULL),
m_parentObjectName(parentObjectName)
{
    int minComboBoxWidth = 200;
    int maxComboBoxWidth = 100000;
    if (orientation == Qt::Horizontal) {
        minComboBoxWidth = 50;
        maxComboBoxWidth = 100000;
    }
    const QComboBox::SizeAdjustPolicy comboSizePolicy = QComboBox::AdjustToContentsOnFirstShow;
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    QString objectNamePrefix = QString(parentObjectName
                                       + ":ChartOverlay%1"
                                       + ":").arg((int)(chartOverlayIndex + 1), 2, 10, QLatin1Char('0'));
    QString descriptivePrefix = QString("chart overlay "
                                        + QString::number(chartOverlayIndex + 1));

    /*
     * Enabled Check Box
     */
    const QString enabledCheckboxText = ((orientation == Qt::Horizontal) ? " " : "On ");
    m_enabledCheckBox = new QCheckBox(enabledCheckboxText);
    m_enabledCheckBox->setObjectName(objectNamePrefix
                                         + "OnOff");
    QObject::connect(m_enabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(enabledCheckBoxClicked(bool)));
    m_enabledCheckBox->setToolTip("Display line charts from the selected file");
    macroManager->addMacroSupportToObject(m_enabledCheckBox,
                                          "Enable line chart display for " + descriptivePrefix);
    
    /*
     * Line Series Enabled Check Box
     */
    const QString loadingCheckboxText = ((orientation == Qt::Horizontal) ? " " : "Load ");
    m_lineSeriesLoadingEnabledCheckBox = new QCheckBox(loadingCheckboxText);
    QObject::connect(m_lineSeriesLoadingEnabledCheckBox, &QCheckBox::clicked,
                     this, &ChartTwoOverlayViewController::lineSeriesLoadingEnabledCheckBoxClicked);
    m_lineSeriesLoadingEnabledCheckBox->setToolTip("Enable loading of line charts for the selected file");
    m_lineSeriesLoadingEnabledCheckBox->setObjectName(objectNamePrefix
                                                      + "EnableLoading");
    macroManager->addMacroSupportToObject(m_lineSeriesLoadingEnabledCheckBox,
                                          "Enable line chart loading for " + descriptivePrefix);
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);
    
    m_settingsToolButton = new QToolButton();
    m_settingsAction = WuQtUtilities::createAction("S",
                                                   "Edit settings for this chart",
                                                   m_settingsToolButton,
                                                   this,
                                                   SLOT(settingsActionTriggered()));
    if (settingsIconValid) {
        m_settingsAction->setIcon(settingsIcon);
    }
    m_settingsAction->setObjectName(objectNamePrefix
                                    + "ShowSettingsDialog");
    macroManager->addMacroSupportToObject(m_settingsAction,
                                          "Show settings dialog for " + descriptivePrefix);
    m_settingsToolButton->setDefaultAction(m_settingsAction);
    
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    m_colorBarToolButton = new QToolButton();
    m_colorBarAction = WuQtUtilities::createAction("CB",
                                                       "Display color bar for this overlay",
                                                       m_colorBarToolButton,
                                                       this,
                                                       SLOT(colorBarActionTriggered(bool)));
    m_colorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        m_colorBarAction->setIcon(colorBarIcon);
    }
    m_colorBarAction->setObjectName(objectNamePrefix
                                        + "ShowColorBar");
    macroManager->addMacroSupportToObject(m_colorBarAction,
                                          "Enable color bar for " + descriptivePrefix);
    m_colorBarToolButton->setDefaultAction(m_colorBarAction);
    
    /*
     * Construction Tool Button
     * Note: macro support is on each action in menu in 'createConstructionMenu'
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                               constructionIcon);
    m_constructionAction = WuQtUtilities::createAction("C",
                                                           "Add/Move/Remove Layers",
                                                           this);
    if (constructionIconValid) {
        m_constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton,
                                                     (objectNamePrefix
                                                      + "ConstructionMenu:"),
                                                     descriptivePrefix);
    m_constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(m_constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    
    /*
     * Matrix triangular view mode button
     * Note: macro support is on each action in menu in createMatrixTriangularViewModeMenu
     */
    m_matrixTriangularViewModeToolButton = new QToolButton();
    m_matrixTriangularViewModeAction = WuQtUtilities::createAction("M",
                                                       "Select a triangular view of the matrix",
                                                       m_matrixTriangularViewModeToolButton);
    QMenu* matrixTriangularViewModeMenu = createMatrixTriangularViewModeMenu(m_matrixTriangularViewModeToolButton,
                                                                             (objectNamePrefix
                                                                              + "TriangularViewMenu:"),
                                                                             descriptivePrefix);
    m_matrixTriangularViewModeAction->setMenu(matrixTriangularViewModeMenu);
    m_matrixTriangularViewModeToolButton->setDefaultAction(m_matrixTriangularViewModeAction);
    m_matrixTriangularViewModeToolButton->setPopupMode(QToolButton::InstantPopup);

    /*
     * Matrix opacity spin box
     */
    m_matrixOpacitySpinBox = new WuQDoubleSpinBox(this);
    m_matrixOpacitySpinBox->setToolTip("Set opacity for matrix");
    m_matrixOpacitySpinBox->setRange(0.0, 1.0);
    m_matrixOpacitySpinBox->setDecimals(1);
    m_matrixOpacitySpinBox->setSingleStep(0.1);
    QObject::connect(m_matrixOpacitySpinBox, &WuQDoubleSpinBox::valueChanged,
                     this, &ChartTwoOverlayViewController::matrixOpacityValueChanged);

    /*
     * Line layer color tool button
     */
    m_lineLayerColorToolButton = new CaretColorToolButton(CaretColorToolButton::CustomColorMode::EDITABLE,
                                                          CaretColorToolButton::NoneColorMode::DISABLED);
    QObject::connect(m_lineLayerColorToolButton, &CaretColorToolButton::colorSelected,
                     this, &ChartTwoOverlayViewController::lineLayerColorSelected);
    m_lineLayerColorToolButton->setToolTip("Set color for line layer charts");

    /*
     * Line layer tooltip offset button
     */
    const QString offTT("Set offset of tooltip containing (index, x, y) from selected point "
                        "with cardinal and ordinal directions");
    m_lineLayerToolTipOffsetToolButton = new QToolButton();
    if (m_useIconInLineLayerToolTipOffsetButtonFlag) {
        m_lineLayerToolTipOffsetToolButton->setIcon(createCardinalDirectionPixmap(m_lineLayerToolTipOffsetToolButton));
    }
    WuQtUtilities::setWordWrappedToolTip(m_lineLayerToolTipOffsetToolButton,
                                         offTT);
    QObject::connect(m_lineLayerToolTipOffsetToolButton, &QToolButton::clicked,
                     this, &ChartTwoOverlayViewController::lineLayerToolTipOffsetToolButtonClicked);
    
     
    /*
     * Line layer normalization button
     */
    m_lineLayerNormalizationToolButton = new QToolButton();
    m_lineLayerNormalizationToolButton->setText("N");
    m_lineLayerNormalizationToolButton->setToolTip("Normalize line");
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_lineLayerNormalizationToolButton);
    QObject::connect(m_lineLayerNormalizationToolButton, &QToolButton::clicked,
                     this, &ChartTwoOverlayViewController::lineLayerNormalizationToolButtonClicked);
    
    /*
     * Line layer normalization widget and menu
     */
    m_lineLayerNormalizationWidget = new ChartTwoLineLayerNormalizationWidget();
    QWidgetAction* normalizationWidgetAction = new QWidgetAction(m_lineLayerNormalizationToolButton);
    normalizationWidgetAction->setDefaultWidget(m_lineLayerNormalizationWidget);
    
    m_lineLayerNormalizationMenu = new QMenu(m_lineLayerNormalizationToolButton);
    m_lineLayerNormalizationMenu->addAction(normalizationWidgetAction);
    QObject::connect(m_lineLayerNormalizationMenu, &QMenu::aboutToShow,
                     this, &ChartTwoOverlayViewController::lineLayerNormalizationMenuAboutToShow);

    
    /*
     * Match button sizes
     */
    std::vector<QWidget*> toolButtons {
        m_lineLayerColorToolButton,
        m_lineLayerToolTipOffsetToolButton,
        m_lineLayerNormalizationToolButton
    } ;
    WuQtUtilities::matchWidgetSizes(toolButtons);
    
    /*
     * Line layer width
     */
    m_lineLayerWidthSpinBox = new WuQDoubleSpinBox(this);
    m_lineLayerWidthSpinBox->setToolTip("Set line width for line layer charts");
    m_lineLayerWidthSpinBox->setRangePercentage(0.0, 100.0);
    m_lineLayerWidthSpinBox->setSingleStepPercentage(0.1);
    m_lineLayerWidthSpinBox->setDecimals(1);
    m_lineLayerWidthSpinBox->getWidget()->setFixedWidth(60);
    QObject::connect(m_lineLayerWidthSpinBox, &WuQDoubleSpinBox::valueChanged,
                     this, &ChartTwoOverlayViewController::lineLayerLineWidthChanged);
    
    /*
     * Seledted point check box and spin box
     */
    const QString spinToolTipText("Set index of selected point.  Index may also be set "
                                  "by clicking the mouse over the line in the chart "
                                  "graphics.  Index can be "
                                  "incremented by placing mouse over the "
                                  "line in the chart graphics and pressing the right or up "
                                  "arrow keys and decremented using the left or down "
                                  "arrow keys (it may be necessary to click in the "
                                  "chart graphics for the arrow keys to function).");
    const QString activeToolTip("OFF - No symbol displayed\n"
                                "ON  - Ring drawn at selected point\n"
                                "ACTIVE - Circle drawn at selected point\n"
                                "         arrow right/left arrow keys\n"
                                "         in chart drawing region\n"
                                "         increment/decrement point index");
    m_lineLayerActiveComboBox = new EnumComboBoxTemplate(this);
    m_lineLayerActiveComboBox->setup<ChartTwoOverlayActiveModeEnum,ChartTwoOverlayActiveModeEnum::Enum>();
    QObject::connect(m_lineLayerActiveComboBox, SIGNAL(itemActivated()),
                     this, SLOT(lineLayerActiveModeEnumComboBoxItemActivated()));
    m_lineLayerActiveComboBox->setToolTip(activeToolTip);
    
    m_selectedPointIndexSpinBox = new QSpinBox();
    m_selectedPointIndexSpinBox->setToolTip("Set index of selected point");
    WuQtUtilities::setWordWrappedToolTip(m_selectedPointIndexSpinBox,
                                         spinToolTipText);
    m_selectedPointIndexSpinBox->setSingleStep(1);
    QObject::connect(m_selectedPointIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &ChartTwoOverlayViewController::selectedPointIndexSpinBoxValueChanged);
    
    /*
     * Map file Selection Check Box
     */
    m_mapFileComboBox = WuQFactory::newComboBox();
    m_mapFileComboBox->setMinimumWidth(minComboBoxWidth);
    m_mapFileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(m_mapFileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    m_mapFileComboBox->setToolTip("Selects file for this overlay");
    m_mapFileComboBox->setSizeAdjustPolicy(comboSizePolicy);
    m_mapFileComboBox->setObjectName(objectNamePrefix
                                     + "FileSelection");
    macroManager->addMacroSupportToObject(m_mapFileComboBox,
                                          "Select file in " + descriptivePrefix);
    
    /*
     * Yoking Group
     */
    const AString yokeToolTip = ("Select a yoking group.\n"
                                 "\n"
                                 "When files with more than one map are yoked,\n"
                                 "the seleted maps are synchronized by map index.\n"
                                 "\n"
                                 "If the SAME FILE is in yoked in multiple overlays,\n"
                                 "the overlay enabled statuses are synchronized.\n");
    m_mapRowOrColumnYokingGroupComboBox = new MapYokingGroupComboBox(this);
    m_mapRowOrColumnYokingGroupComboBox->getWidget()->setStatusTip("Synchronize enabled status and map indices)");
    m_mapRowOrColumnYokingGroupComboBox->getWidget()->setToolTip("Yoke to Overlay Mapped Files");
    QObject::connect(m_mapRowOrColumnYokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokingGroupActivated()));
    
    /*
     * All maps check box
     */
    m_allMapsCheckBox = new QCheckBox("All Maps");
    m_allMapsCheckBox->setToolTip("Show histogram of all maps");
    if (orientation == Qt::Horizontal) {
        m_allMapsCheckBox->setText(" ");
    }
    QObject::connect(m_allMapsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(allMapsCheckBoxClicked(bool)));
    m_allMapsCheckBox->setObjectName(objectNamePrefix
                                     + "AllMaps");
    macroManager->addMacroSupportToObject(m_allMapsCheckBox,
                                          "Enable all maps in " + descriptivePrefix);
    
    /*
     * Map/Row/Column Index Spin Box
     */
    m_mapRowOrColumnIndexSpinBox = WuQFactory::newSpinBox();
    QObject::connect(m_mapRowOrColumnIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(mapRowOrColumnIndexSpinBoxValueChanged(int)));
    m_mapRowOrColumnIndexSpinBox->setToolTip("Select map/row/column by its index");
    m_mapRowOrColumnIndexSpinBox->setRange(1, 9999); // fix size for 4 digits
    m_mapRowOrColumnIndexSpinBox->setFixedSize(m_mapRowOrColumnIndexSpinBox->sizeHint());
    m_mapRowOrColumnIndexSpinBox->setRange(1, 1);
    m_mapRowOrColumnIndexSpinBox->setValue(1);
    m_mapRowOrColumnIndexSpinBox->setObjectName(objectNamePrefix
                                           + "MapIndex");
    macroManager->addMacroSupportToObject(m_mapRowOrColumnIndexSpinBox,
                                          "Select map by index in " + descriptivePrefix);

    
    /*
     * Map/Row/Column Name Combo Box
     */
    m_mapRowOrColumnNameComboBox = WuQFactory::newComboBox();
    m_mapRowOrColumnNameComboBox->setMinimumWidth(minComboBoxWidth);
    m_mapRowOrColumnNameComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(m_mapRowOrColumnNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapRowOrColumnNameComboBoxSelected(int)));
    m_mapRowOrColumnNameComboBox->setToolTip("Select map/row/column by its name");
    m_mapRowOrColumnNameComboBox->setSizeAdjustPolicy(comboSizePolicy);
    m_mapRowOrColumnNameComboBox->setObjectName(objectNamePrefix
                                         + "MapSelection");
    macroManager->addMacroSupportToObject(m_mapRowOrColumnNameComboBox,
                                          "Select map name in " + descriptivePrefix);
}

/**
 * Destructor.
 */
ChartTwoOverlayViewController::~ChartTwoOverlayViewController()
{
}

/*
 * If this overlay ins an overlay settings editor, update its content
 */
void
ChartTwoOverlayViewController::updateOverlaySettingsEditor()
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile = NULL;
    ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    m_chartOverlay->getSelectionData(mapFile,
                                     selectedIndexType,
                                     selectedIndex);
    
    if ((mapFile != NULL)
        && (selectedIndex >= 0)) {
        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_OVERLAY_MAP_CHANGED,
                                                     m_browserWindowIndex,
                                                     m_chartOverlay,
                                                     mapFile,
                                                     selectedIndexType,
                                                     selectedIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void
ChartTwoOverlayViewController::fileComboBoxSelected(int indx)
{
    if (m_chartOverlay == NULL) {
        return;
    }

    void* pointer = m_mapFileComboBox->itemData(indx).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    m_chartOverlay->setSelectionData(file, -1);
    
    updateViewController(m_chartOverlay);
    m_mapRowOrColumnYokingGroupComboBox->validateYokingChange(m_chartOverlay);
    updateUserInterfaceAndGraphicsWindow();
    updateOverlaySettingsEditor();
    
    /*
     * User interface update may cause loss of focus so restore it
     */
    m_mapFileComboBox->setFocus();
}

/**
 * Called when a selection is made from the map index spin box.
 * @parm indxIn
 *    Index of selection.
 */
void
ChartTwoOverlayViewController::mapRowOrColumnIndexSpinBoxValueChanged(int indxIn)
{
    if (m_chartOverlay == NULL)
    {
        //TSC: not sure how to put the displayed integer back to 0 where it starts when opening without data files
        return;
    }

    const bool focusFlag = m_mapRowOrColumnIndexSpinBox->hasFocus();
    
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = m_mapFileComboBox->currentIndex();
    void* pointer = m_mapFileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    /*
     * Spin box may range [0, N-1] or [1, N] but in source code
     * indices are always [0, N-1]
     */
    const int32_t indx = indxIn - m_mapRowOrColumnIndexSpinBox->minimum();
    m_chartOverlay->setSelectionData(file, indx);
    
    const MapYokingGroupEnum::Enum mapYoking = m_chartOverlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               file,
                                               NULL,
                                               indx,
                                               m_chartOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update map name combo box.
     */
    m_mapRowOrColumnNameComboBox->blockSignals(true);
    if ((indx >= 0)
        && (indx < m_mapRowOrColumnNameComboBox->count())) {
        m_mapRowOrColumnNameComboBox->setCurrentIndex(indx);
    }
    m_mapRowOrColumnNameComboBox->blockSignals(false);
    
    this->updateUserInterfaceAndGraphicsWindow();
    
    updateOverlaySettingsEditor();
    
    /* 
     * User interface update may cause loss of focus so restore it
     */
    if (focusFlag) {
        m_mapRowOrColumnIndexSpinBox->setFocus();
    }
}

/**
 * Called when a selection is made from the map name combo box.
 * @parm indx
 *    Index of selection.
 */
void
ChartTwoOverlayViewController::mapRowOrColumnNameComboBoxSelected(int indx)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = m_mapFileComboBox->currentIndex();
    void* pointer = m_mapFileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    m_chartOverlay->setSelectionData(file, indx);
    
    const MapYokingGroupEnum::Enum mapYoking = m_chartOverlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               file,
                                               NULL,
                                               indx,
                                               m_chartOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update map index spin box.
     * Spin box may range [0, N-1] or [1, N] but in source code
     * indices are always [0, N-1]
     */
    const int spinBoxIndex = indx + m_mapRowOrColumnIndexSpinBox->minimum();
    m_mapRowOrColumnIndexSpinBox->blockSignals(true);
    m_mapRowOrColumnIndexSpinBox->setValue(spinBoxIndex);
    m_mapRowOrColumnIndexSpinBox->blockSignals(false);
    
    this->updateUserInterfaceAndGraphicsWindow();
    
    updateOverlaySettingsEditor();
    
    /*
     * User interface update may cause loss of focus so restore it
     */
    m_mapRowOrColumnNameComboBox->setFocus();
}

/**
 * Called when enabled checkbox state is changed
 * @parm checked
 *    Checked status
 */
void
ChartTwoOverlayViewController::enabledCheckBoxClicked(bool checked)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    m_chartOverlay->setEnabled(checked);
    updateViewController(m_chartOverlay);

    const MapYokingGroupEnum::Enum mapYoking = m_chartOverlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        m_chartOverlay->getSelectionData(mapFile,
                                         selectedIndexType,
                                         selectedIndex);
        
        if (selectedIndexType == ChartTwoOverlay::SelectedIndexType::MAP) {
            EventMapYokingSelectMap selectMapEvent(mapYoking,
                                                   mapFile,
                                                   NULL,
                                                   selectedIndex,
                                                   m_chartOverlay->isEnabled());
            EventManager::get()->sendEvent(selectMapEvent.getPointer());
        }
    }
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when line-series loading enabled checkbox status is changed
 * @parm checked
 *    Checked status
 */
void
ChartTwoOverlayViewController::lineSeriesLoadingEnabledCheckBoxClicked(bool checked)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    m_chartOverlay->setLineSeriesLoadingEnabled(checked);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when colorbar toolbutton is toggled.
 * @param status
 *    New status.
 */
void
ChartTwoOverlayViewController::colorBarActionTriggered(bool status)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    m_chartOverlay->getColorBar()->setDisplayed(status);
    
    this->updateGraphicsWindow();
}

/**
 * Called when colorbar toolbutton is toggled.
 * @param status
 *    New status.
 */
void
ChartTwoOverlayViewController::allMapsCheckBoxClicked(bool status)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile = NULL;
    ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    m_chartOverlay->getSelectionData(mapFile,
                                     selectedIndexType,
                                     selectedIndex);

    bool doAllMapsFlag = true;
    if (mapFile != NULL) {
        if (status) {
            doAllMapsFlag = WuQMessageBox::warningLargeFileSizeOkCancel(m_allMapsCheckBox,
                                                                        mapFile);
        }
        if (doAllMapsFlag) {
            mapFile->invalidateHistogramChartColoring();
        }
    }
    
    if (doAllMapsFlag) {
        m_chartOverlay->setAllMapsSelected(status);
    }
    else {
        QSignalBlocker blocker(m_allMapsCheckBox);
        m_allMapsCheckBox->setChecked(false);
    }
    
    this->updateGraphicsWindow();
}

/**
 * Validate yoking when there are changes made to the overlay.
 */
void
ChartTwoOverlayViewController::validateYokingSelection()
{
    m_mapRowOrColumnYokingGroupComboBox->validateYokingChange(m_chartOverlay);
    updateViewController(m_chartOverlay);
    updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when the yoking group is changed.
 */
void
ChartTwoOverlayViewController::yokingGroupActivated()
{
    MapYokingGroupEnum::Enum yokingGroup = m_mapRowOrColumnYokingGroupComboBox->getMapYokingGroup();
    
    /*
     * Has yoking group changed?
     * TSC: overlay can be null when opened without loaded files
     */
    if (m_chartOverlay != NULL && yokingGroup != m_chartOverlay->getMapYokingGroup()) {
        validateYokingSelection();
    }
}


/**
 * Called when the settings action is selected.
 */
void
ChartTwoOverlayViewController::settingsActionTriggered()
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile = NULL;
    ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    m_chartOverlay->getSelectionData(mapFile,
                                     selectedIndexType,
                                     selectedIndex);
    if (mapFile != NULL) {
        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_SHOW_EDITOR,
                                                     m_browserWindowIndex,
                                                     m_chartOverlay,
                                                     mapFile,
                                                     selectedIndexType,
                                                     selectedIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void
ChartTwoOverlayViewController::updateViewController(ChartTwoOverlay* chartOverlay)
{
    m_chartOverlay = chartOverlay;
    
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    std::vector<AString> selectedFileMapNames;
    ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    if (m_chartOverlay != NULL) {
        m_chartOverlay->getSelectionData(dataFiles,
                                         selectedFile,
                                         selectedFileMapNames,
                                         selectedIndexType,
                                         selectedIndex);
    }
    
    /*
     * Setup names of file for display in combo box
     */
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(dataFiles,
                                                                            displayNames);
    CaretAssert(dataFiles.size() == displayNames.size());
    
    /*
     * Update tooltips with full path to file and name of map
     * as names may be too long to fit into combo boxes
     */
    AString fileComboBoxToolTip("Select file for this overlay");
    AString nameComboBoxToolTip("Select map by its name");
    if (selectedFile != NULL) {
        FileInformation fileInfo(selectedFile->getFileName());
        fileComboBoxToolTip.append(":\n"
                                   + fileInfo.getFileName()
                                   + "\n"
                                   + fileInfo.getPathName()
                                   + "\n\n"
                                   + "Copy File Name/Path to Clipboard with Construction Menu");
        
        nameComboBoxToolTip.append(":\n"
                                   + m_mapRowOrColumnNameComboBox->currentText());
    }
    m_mapFileComboBox->setToolTip(fileComboBoxToolTip);
    m_mapRowOrColumnNameComboBox->setToolTip(nameComboBoxToolTip);
    
    /*
     * Load the file selection combo box.
     */
    m_mapFileComboBox->clear();
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(dataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        CaretMappableDataFile* dataFile = dataFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toOverlayTypeName(dataFile->getDataFileType());
        CaretAssertVectorIndex(displayNames, i);
        m_mapFileComboBox->addItem(displayNames[i],
                                    qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        m_mapFileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the map name selection combo box and map index spin box
     */
    m_mapRowOrColumnNameComboBox->setEnabled(false);
    m_mapRowOrColumnNameComboBox->blockSignals(true);
    m_mapRowOrColumnNameComboBox->clear();
    m_mapRowOrColumnIndexSpinBox->setEnabled(false);
    m_mapRowOrColumnIndexSpinBox->blockSignals(true);
    m_mapRowOrColumnIndexSpinBox->setRange(1, 1);
    m_mapRowOrColumnIndexSpinBox->setValue(1);
    const int32_t numberOfMaps = static_cast<int32_t>(selectedFileMapNames.size());
    if (numberOfMaps > 0) {
        m_mapRowOrColumnNameComboBox->setEnabled(true);
        m_mapRowOrColumnIndexSpinBox->setEnabled(true);

        for (int32_t i = 0; i < numberOfMaps; i++) {
            CaretAssertVectorIndex(selectedFileMapNames, i);
            m_mapRowOrColumnNameComboBox->addItem(selectedFileMapNames[i]);
        }
        m_mapRowOrColumnNameComboBox->setCurrentIndex(selectedIndex);
        
        /*
         * Spin box ranges [0, N-1] or [1, N] depending upon data
         */
        int32_t mapIndexMinimumValue = 1;
        int32_t mapIndexMaximumValue = numberOfMaps;
        switch (m_chartOverlay->getChartTwoDataType()) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                break;
        }
        
        m_mapRowOrColumnIndexSpinBox->setRange(mapIndexMinimumValue, mapIndexMaximumValue);
        const int spinBoxIndex = selectedIndex + m_mapRowOrColumnIndexSpinBox->minimum();
        m_mapRowOrColumnIndexSpinBox->setValue(spinBoxIndex);
    }
    m_mapRowOrColumnNameComboBox->blockSignals(false);
    m_mapRowOrColumnIndexSpinBox->blockSignals(false);
    
    const bool validOverlayAndFileFlag = ((m_chartOverlay != NULL)
                                          && (selectedFile != NULL));
    
    /*
     * Update enabled checkbox
     */
    m_enabledCheckBox->setEnabled(false);
    m_enabledCheckBox->setChecked(false);
    if (validOverlayAndFileFlag) {
        m_enabledCheckBox->setEnabled(true);
        m_enabledCheckBox->setChecked(m_chartOverlay->isEnabled());
    }
    
    /*
     * Update lines series loading checkbox
     */
    m_lineSeriesLoadingEnabledCheckBox->setEnabled(false);
    m_lineSeriesLoadingEnabledCheckBox->setChecked(false);
    if ((validOverlayAndFileFlag)
        && (m_chartOverlay->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES)) {
        m_lineSeriesLoadingEnabledCheckBox->setEnabled(true);
        m_lineSeriesLoadingEnabledCheckBox->setChecked(m_chartOverlay->isLineSeriesLoadingEnabled());
    }
    
    /*
     * Update yoking
     */
    m_mapRowOrColumnYokingGroupComboBox->getWidget()->setEnabled(false);
    m_mapRowOrColumnYokingGroupComboBox->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    if (validOverlayAndFileFlag) {
        if (m_chartOverlay->isMapYokingSupported()) {
            m_mapRowOrColumnYokingGroupComboBox->getWidget()->setEnabled(true);
            m_mapRowOrColumnYokingGroupComboBox->setMapYokingGroup(m_chartOverlay->getMapYokingGroup());
        }
    }
    
    /*
     * Update all maps
     */
    m_allMapsCheckBox->setEnabled(false);
    m_allMapsCheckBox->setChecked(false);
    if (validOverlayAndFileFlag) {
        m_allMapsCheckBox->setEnabled(m_chartOverlay->isAllMapsSupported());
        if (m_chartOverlay->isAllMapsSupported()) {
            m_allMapsCheckBox->setChecked(m_chartOverlay->isAllMapsSelected());
        }
    }
    
    /*
     * Update settings (wrench) button
     */
    bool enableSettingsActionFlag = validOverlayAndFileFlag;
    switch (m_chartOverlay->getChartTwoDataType()) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            enableSettingsActionFlag = false;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    m_settingsAction->setEnabled(enableSettingsActionFlag);
    
    /*
     * Update color bar button
     */
    m_colorBarAction->blockSignals(true);
    m_colorBarAction->setEnabled(false);
    m_colorBarAction->setChecked(false);
    if (validOverlayAndFileFlag) {
        if (selectedFile->isMappedWithPalette()) {
            m_colorBarAction->setEnabled(true);
            m_colorBarAction->setChecked(m_chartOverlay->getColorBar()->isDisplayed());
        }
    }
    m_colorBarAction->blockSignals(false);
    
    /*
     * Update construction button
     */
    m_constructionAction->setEnabled(true);
    
    /*
     * Update matrix triangular view mode
     */
    m_matrixTriangularViewModeAction->setEnabled(false);
    if (validOverlayAndFileFlag) {
        const ChartTwoMatrixTriangularViewingModeEnum::Enum viewMode = m_chartOverlay->getMatrixTriangularViewingMode();
        
        for (auto& mvmd : m_matrixViewMenuData) {
            if (std::get<0>(mvmd) == viewMode) {
                std::get<1>(mvmd)->setChecked(true);
                updateMatrixTriangularViewModeAction(viewMode);
                break;
            }
        }
        
        if (m_chartOverlay->isMatrixTriangularViewingModeSupported()) {
            m_matrixTriangularViewModeAction->setEnabled(true);
        }
    }
        
    /*
     * Update matrix opacity
     */
    if (validOverlayAndFileFlag) {
        m_matrixOpacitySpinBox->setValue(m_chartOverlay->getMatrixOpacity());
    }
    
    /*
     * Update line layer color, offset, and width tool button
     */
    m_lineLayerColorToolButton->setEnabled(false);
    m_lineLayerWidthSpinBox->getWidget()->setEnabled(false);
    m_lineLayerToolTipOffsetToolButton->setEnabled(false);
    m_lineLayerNormalizationToolButton->setEnabled(false);
    if (validOverlayAndFileFlag) {
        m_lineLayerColorToolButton->setSelectedColor(m_chartOverlay->getLineLayerColor());
        m_lineLayerWidthSpinBox->setValue(m_chartOverlay->getLineLayerLineWidth());
        if (m_chartOverlay->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER) {
            m_lineLayerColorToolButton->setEnabled(true);
            m_lineLayerWidthSpinBox->getWidget()->setEnabled(true);
            m_lineLayerToolTipOffsetToolButton->setEnabled(true);
            m_lineLayerNormalizationToolButton->setEnabled(true);
        }
    }
    updateLineLayerToolTipOffsetToolButton();
    
    /*
     * Update selected point checkbox and index
     */
    bool pointValidFlag(false);
    if (validOverlayAndFileFlag) {
        m_lineLayerActiveComboBox->setSelectedItem<ChartTwoOverlayActiveModeEnum,ChartTwoOverlayActiveModeEnum::Enum>(m_chartOverlay->getLineChartActiveMode());
        m_selectedPointIndexSpinBox->setRange(0, m_chartOverlay->getSelectedLineChartNumberOfPoints() - 1);
        QSignalBlocker spinBlocker(m_selectedPointIndexSpinBox);
        m_selectedPointIndexSpinBox->setValue(m_chartOverlay->getSelectedLineChartPointIndex());
        if (m_chartOverlay->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER) {
            pointValidFlag = true;
        }
    }
    m_lineLayerActiveComboBox->getWidget()->setEnabled(pointValidFlag);
    m_selectedPointIndexSpinBox->setEnabled(pointValidFlag);

    bool showAllMapsCheckbBoxFlag(false);
    bool showColorBarButtonFlag(false);
    bool showLineLayerColorButtonFlag(false);
    bool showLineLayerOffsetButtonFlag(false);
    bool showLineLayerWidthButtonFlag(false);
    bool showLineLayerNormalizationButtonFlag(false);
    bool showSelectedPointControlsFlag(false);
    bool showLineSeriesLoadingCheckBoxFlag(false);
    bool showMatrixDiagonalButtonFlag(false);
    bool showMatrixOpacityFlag(false);
    bool showSettingsButtonFlag(false);
    switch (m_chartOverlay->getChartTwoDataType()) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            showAllMapsCheckbBoxFlag = true;
            showColorBarButtonFlag = true;
            showSettingsButtonFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            showLineLayerColorButtonFlag = true;
            showLineLayerOffsetButtonFlag = true;
            showLineLayerWidthButtonFlag = true;
            showLineLayerNormalizationButtonFlag = true;
            showSelectedPointControlsFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            showLineSeriesLoadingCheckBoxFlag = true;
            showSettingsButtonFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            showColorBarButtonFlag = true;
            showMatrixDiagonalButtonFlag = true;
            showMatrixOpacityFlag = true;
            showSettingsButtonFlag = true;
            break;
    }
    m_allMapsCheckBox->setVisible(showAllMapsCheckbBoxFlag);
    m_colorBarToolButton->setVisible(showColorBarButtonFlag);
    m_lineLayerColorToolButton->setVisible(showLineLayerColorButtonFlag);
    m_lineLayerToolTipOffsetToolButton->setVisible(showLineLayerOffsetButtonFlag);
    m_lineLayerNormalizationToolButton->setVisible(showLineLayerNormalizationButtonFlag);
    m_lineLayerWidthSpinBox->getWidget()->setVisible(showLineLayerWidthButtonFlag);
    m_lineSeriesLoadingEnabledCheckBox->setVisible(showLineSeriesLoadingCheckBoxFlag);
    m_matrixTriangularViewModeToolButton->setVisible(showMatrixDiagonalButtonFlag);
    m_matrixOpacitySpinBox->getWidget()->setVisible(showMatrixOpacityFlag);
    m_matrixOpacitySpinBox->getWidget()->setEnabled(false); /* Matrix opacity not supported at this time */
    m_lineLayerActiveComboBox->getWidget()->setVisible(showSelectedPointControlsFlag);
    m_selectedPointIndexSpinBox->setVisible(showSelectedPointControlsFlag);
    m_settingsToolButton->setVisible(showSettingsButtonFlag);
}

/**
 * Update the matrix triangular view mode button.
 *
 * @param matrixViewMode
 *     Matrix triangular view mode.
 */
void
ChartTwoOverlayViewController::updateMatrixTriangularViewModeAction(const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode)
{
    CaretAssert(m_matrixTriangularViewModeAction);
    m_matrixTriangularViewModeAction->blockSignals(true);
    for (auto& mvmd : m_matrixViewMenuData) {
        if (std::get<0>(mvmd) == matrixViewMode) {
            QPixmap pixmap = std::get<2>(mvmd);
            if ( ! pixmap.isNull()) {
                m_matrixTriangularViewModeAction->setIcon(pixmap);
                m_matrixTriangularViewModeAction->setText("");
            }
            else {
                m_matrixTriangularViewModeAction->setText("M");
            }
            break;
        }
    }
    m_matrixTriangularViewModeAction->blockSignals(false);
}

/**
 * Update graphics and GUI after selections made
 */
void
ChartTwoOverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    updateUserInterface();
    updateGraphicsWindow();
}

/**
 * Update graphics and GUI after selections made
 */
void
ChartTwoOverlayViewController::updateUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Update graphics after selections made
 */
void
ChartTwoOverlayViewController::updateGraphicsWindow()
{
    if (m_chartOverlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
}

/**
 * Create the matrix triangular view mode menu.
 * @param parent
 *    Parent widget.
 * @param parentObjectName
 *    Name of parent object for macros
 * @param descriptivePrefix
 *    Descriptive prefix for macros
 */
QMenu*
ChartTwoOverlayViewController::createMatrixTriangularViewModeMenu(QWidget* parent,
                                                                  const QString& parentObjectName,
                                                                  const QString& descriptivePrefix)
{
    std::vector<ChartTwoMatrixTriangularViewingModeEnum::Enum> allViewModes;
    ChartTwoMatrixTriangularViewingModeEnum::getAllEnums(allViewModes);
    
    QMenu* menu = new QMenu(parent);
    QObject::connect(menu, &QMenu::triggered,
                     this, &ChartTwoOverlayViewController::menuMatrixTriangularViewModeTriggered);

    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    
    for (auto viewMode: allViewModes) {
        QAction* action = menu->addAction(ChartTwoMatrixTriangularViewingModeEnum::toGuiName(viewMode));
        action->setCheckable(true);
        action->setData((int)ChartTwoMatrixTriangularViewingModeEnum::toIntegerCode(viewMode));
        QPixmap pixmap = createMatrixTriangularViewModePixmap(menu, viewMode);
        action->setIcon(pixmap);
        actionGroup->addAction(action);
        
        QString objName = (parentObjectName
                           + ChartTwoMatrixTriangularViewingModeEnum::toGuiName(viewMode));
        objName = objName.replace(" ", "");
        action->setObjectName(objName);
        WuQMacroManager::instance()->addMacroSupportToObject(action,
                                                             "Set triangular view in " + descriptivePrefix);
        
        m_matrixViewMenuData.push_back(std::make_tuple(viewMode, action, pixmap));
    }
    
    return menu;
}

/**
 * Called when an item is selected on matrix triangular view mode menu.
 *
 * @action
 *     Action of menu item selected.
 */
void
ChartTwoOverlayViewController::menuMatrixTriangularViewModeTriggered(QAction* action)
{
    const QVariant itemData = action->data();
    CaretAssert(itemData.isValid());
    bool valid = false;
    ChartTwoMatrixTriangularViewingModeEnum::Enum viewMode = ChartTwoMatrixTriangularViewingModeEnum::fromIntegerCode(itemData.toInt(), &valid);
    
    if (valid) {
        m_chartOverlay->setMatrixTriangularViewingMode(viewMode);
        updateMatrixTriangularViewModeAction(viewMode);
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        m_chartOverlay->getSelectionData(mapFile,
                                         selectedIndexType,
                                         selectedIndex);
        if (mapFile != NULL) {
            mapFile->updateScalarColoringForAllMaps();
        }
        this->updateGraphicsWindow();
    }
}

/**
 * Called when matrix opacity changed
 * @param value
 *    New opacity value
 */
void
ChartTwoOverlayViewController::matrixOpacityValueChanged(double value)
{
    if (m_chartOverlay != NULL) {
        m_chartOverlay->setMatrixOpacity(value);
        this->updateGraphicsWindow();
    }
}

/**
 * Called when line layer color is changed.
 * @param color
 *     New color
 */
void
ChartTwoOverlayViewController::lineLayerColorSelected(const CaretColor& caretColor)
{
    if (m_chartOverlay != NULL) {
        m_chartOverlay->setLineLayerColor(caretColor);
        this->updateGraphicsWindow();
    }
}

/**
 * Called when line layer line width is changed.
 * @param lineWidth
 *     New line width
 */
void
ChartTwoOverlayViewController::lineLayerLineWidthChanged(const float lineWidth)
{
    if (m_chartOverlay != NULL) {
        m_chartOverlay->setLineLayerLineWidth(lineWidth);
        this->updateGraphicsWindow();
    }
}

/**
 * Called when line cardinal direction tool button clicked
 */
void
ChartTwoOverlayViewController::lineLayerToolTipOffsetToolButtonClicked()
{
    if (m_chartOverlay != NULL) {
        QWidget* parentWidget(m_lineLayerToolTipOffsetToolButton->parentWidget());
        QPoint menuXY(m_lineLayerToolTipOffsetToolButton->pos());

        std::set<CardinalDirectionEnum::Options> options { CardinalDirectionEnum::Options::INCLUDE_AUTO };
        CardinalDirectionEnumMenu menu(options);
        menu.setSelectedCardinalDirection(m_chartOverlay->getSelectedLineChartTextOffset());
        QAction* actionSelected = menu.exec(parentWidget->mapToGlobal(menuXY));
        if (actionSelected != NULL) {
            m_chartOverlay->setSelectedLineChartTextOffset(menu.getSelectedCardinalDirection());
            updateLineLayerToolTipOffsetToolButton();
            this->updateGraphicsWindow();
        }
    }
}

/**
 * Update the line layer tooltip offset button text
 */
void
ChartTwoOverlayViewController::updateLineLayerToolTipOffsetToolButton()
{
    if (m_chartOverlay != NULL) {
        if (m_useIconInLineLayerToolTipOffsetButtonFlag) {
            m_lineLayerToolTipOffsetToolButton->setText("");
        }
        else {
            const AString txt = CardinalDirectionEnum::toGuiShortName(m_chartOverlay->getSelectedLineChartTextOffset());
            m_lineLayerToolTipOffsetToolButton->setText(txt);
        }
    }
}

/**
 * Called when selected point display check box changed
 * @param selected
 *    New selection status
 */
void
ChartTwoOverlayViewController::lineLayerActiveModeEnumComboBoxItemActivated()
{
    if (m_chartOverlay != NULL) {
        const ChartTwoOverlayActiveModeEnum::Enum mode = m_lineLayerActiveComboBox->getSelectedItem<ChartTwoOverlayActiveModeEnum,ChartTwoOverlayActiveModeEnum::Enum>();
        m_chartOverlay->setLineChartActiveMode(mode);
        updateViewController(m_chartOverlay);
        updateGraphicsWindow();
    }
}

/**
 * Called when line layer normalization button is clicked
 */
void
ChartTwoOverlayViewController::lineLayerNormalizationToolButtonClicked()
{
    m_lineLayerNormalizationMenu->exec(m_lineLayerNormalizationToolButton->mapToGlobal(QPoint(0, m_lineLayerNormalizationToolButton->height())));
}

/**
 * Called when line layer normalization button is clicked
 */
void
ChartTwoOverlayViewController::lineLayerNormalizationMenuAboutToShow()
{
    m_lineLayerNormalizationWidget->updateContent(m_chartOverlay);
}

/**
 * Called when selected point index spin box value changed
 * @param index
 *    New point index
 */
void
ChartTwoOverlayViewController::selectedPointIndexSpinBoxValueChanged(int index)
{
    if (m_chartOverlay != NULL) {
        m_chartOverlay->setSelectedLineChartPointIndex(index);

        if (m_chartOverlay->isEnabled()) {
            /*
             * Graphics updates are normally asynchronous (a graphics update is
             * 'scheduled' by Qt and may take place after the graphics update
             * event returns).   Since we are getting the window position of
             * the selected point, and this window position is set in the
             * graphics code, we do a 'repaint' which is synchronous
             * (the event will not return until after the graphics have updated).
             * If we did not do this, the window position may be 'stale' (from
             * a previous graphics update)
             */
            const bool doRepaintFlag(true);
            EventGraphicsUpdateOneWindow graphicsEvent(m_browserWindowIndex,
                                                       doRepaintFlag);
            EventManager::get()->sendEvent(graphicsEvent.getPointer());
        }
    }
}

/**
 * Create the construction menu.
 * @param parent
 *    Parent widget.
 * @param parentObjectName
 *    Name of parent object for macros
 * @param descriptivePrefix
 *    Descriptive name for macros
 */
QMenu*
ChartTwoOverlayViewController::createConstructionMenu(QWidget* parent,
                                                      const QString& menuActionNamePrefix,
                                                      const QString& descriptivePrefix)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    QMenu* menu = new QMenu(parent);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(menuConstructionAboutToShow()));
    
    QAction* addAboveAction = menu->addAction("Add Overlay Above",
                    this,
                    SLOT(menuAddOverlayAboveTriggered()));
    addAboveAction->setObjectName(menuActionNamePrefix
                                  + "AddOverlayAbove");
    addAboveAction->setToolTip("Add an overlay above this overlay");
    macroManager->addMacroSupportToObject(addAboveAction,
                                          "Add overlay above " + descriptivePrefix);
    
    QAction* addBelowAction = menu->addAction("Add Overlay Below",
                    this,
                    SLOT(menuAddOverlayBelowTriggered()));
    addBelowAction->setObjectName(menuActionNamePrefix
                                  + "AddOverlayBelow");
    addBelowAction->setToolTip("Add an overlay below this overlay");
    macroManager->addMacroSupportToObject(addBelowAction,
                                          "Add overlay below " + descriptivePrefix);
    
    menu->addSeparator();
    
    QAction* moveUpAction = menu->addAction("Move Overlay Up",
                    this,
                    SLOT(menuMoveOverlayUpTriggered()));
    moveUpAction->setObjectName(menuActionNamePrefix
                                + "MoveOverlayUp");
    moveUpAction->setToolTip("Move this overlay up");
    macroManager->addMacroSupportToObject(moveUpAction,
                                          "Move " + descriptivePrefix + " up");
    
    QAction* moveDownAction = menu->addAction("Move Overlay Down",
                    this,
                    SLOT(menuMoveOverlayDownTriggered()));
    moveDownAction->setObjectName(menuActionNamePrefix
                                  + "MoveOverlayDown");
    moveDownAction->setToolTip("Move this overlay down");
    macroManager->addMacroSupportToObject(moveDownAction,
                                          "Move " + descriptivePrefix + " down");
    
    menu->addSeparator();
    
    QAction* removeAction = menu->addAction("Remove This Overlay",
                    this,
                    SLOT(menuRemoveOverlayTriggered()));
    removeAction->setObjectName(menuActionNamePrefix
                                + "RemoveOverlay");
    removeAction->setToolTip("Remove this overlay");
    macroManager->addMacroSupportToObject(removeAction,
                                          "Remove " + descriptivePrefix + " overlay");
    
    menu->addSeparator();
    
    m_constructionReloadFileAction = menu->addAction("Reload Selected File",
                                                     this,
                                                     SLOT(menuReloadFileTriggered()));
    m_constructionReloadFileAction->setObjectName(menuActionNamePrefix
                                                  + "ReloadSelectedFile");
    m_constructionReloadFileAction->setToolTip("Reload file in this overlay");
    macroManager->addMacroSupportToObject(m_constructionReloadFileAction,
                                          "Reload file in " + descriptivePrefix);
    
    menu->addSeparator();
    
    QAction* copyPathFileNameAction = menu->addAction("Copy Path and File Name to Clipboard",
                    this,
                    SLOT(menuCopyFileNameToClipBoard()));
    copyPathFileNameAction->setObjectName(menuActionNamePrefix
                                                          + "CopyPathAndFileNameToClipboard");
    copyPathFileNameAction->setToolTip("Copy path and file name of file in this overlay to clipboard");
    macroManager->addMacroSupportToObject(copyPathFileNameAction,
                                          "Copy path and name to clipboard from " + descriptivePrefix);
    
    QAction* copyMapNameAction = menu->addAction("Copy Map Name to Clipboard",
                    this,
                    SLOT(menuCopyMapNameToClipBoard()));
    copyMapNameAction->setObjectName(menuActionNamePrefix
                                     + "CopyMapNameToClipboard");
    copyMapNameAction->setToolTip("Copy name of selected map to the clipboard");
    macroManager->addMacroSupportToObject(copyMapNameAction,
                                          "Copy map name to clipboard from " + descriptivePrefix);
    
    menu->addSeparator();
    QAction* preColorAllFilesAction = menu->addAction("Pre-Color All Files");
    QObject::connect(preColorAllFilesAction, &QAction::triggered,
                     this, &ChartTwoOverlayViewController::menuConstructionPreColorAllFiles);
    preColorAllFilesAction->setObjectName(menuActionNamePrefix
                                          + "PreColorAllFiles");
    macroManager->addMacroSupportToObject(preColorAllFilesAction,
                                          "Pre-Color All Files In Overlay");

    return menu;
    
}

/**
 * Called when construction menu is about to be displayed.
 */
void
ChartTwoOverlayViewController::menuConstructionAboutToShow()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        m_chartOverlay->getSelectionData(mapFile,
                                         selectedIndexType,
                                         selectedIndex);
        
        QString menuText = "Reload Selected File";
        if (mapFile != NULL) {
            if (mapFile->isModified()) {
                QString suffix = " (MODIFIED)";
                if (mapFile->isModifiedPaletteColorMapping()) {
                    if ( ! mapFile->isModifiedExcludingPaletteColorMapping()) {
                        suffix = " (MODIFIED PALETTE)";
                    }
                }
                menuText += suffix;
            }
        }
        m_constructionReloadFileAction->setText(menuText);
    }
}

/**
 * Add an overlay above this overlay.
 */
void
ChartTwoOverlayViewController::menuAddOverlayAboveTriggered()
{
    emit requestAddOverlayAbove(m_chartOverlayIndex);
}

/**
 * Add an overlay below this overlay.
 */
void
ChartTwoOverlayViewController::menuAddOverlayBelowTriggered()
{
    emit requestAddOverlayBelow(m_chartOverlayIndex);
}

/**
 * Remove this overlay.
 */
void
ChartTwoOverlayViewController::menuRemoveOverlayTriggered()
{
    emit requestRemoveOverlay(m_chartOverlayIndex);
}

/**
 * Move this overlay down.
 */
void
ChartTwoOverlayViewController::menuMoveOverlayDownTriggered()
{
    emit requestMoveOverlayDown(m_chartOverlayIndex);
}

/**
 * Move this overlay down.
 */
void
ChartTwoOverlayViewController::menuMoveOverlayUpTriggered()
{
    emit requestMoveOverlayUp(m_chartOverlayIndex);
}

/**
 * Copy the file name to the clip board.
 */
void
ChartTwoOverlayViewController::menuCopyFileNameToClipBoard()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        m_chartOverlay->getSelectionData(mapFile,
                                         selectedIndexType,
                                         selectedIndex);
        
        if (mapFile != NULL) {
            QApplication::clipboard()->setText(mapFile->getFileName().trimmed(),
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Copy the map name to the clip board.
 */
void
ChartTwoOverlayViewController::menuCopyMapNameToClipBoard()
{
    if (m_chartOverlay != NULL) {
        std::vector<CaretMappableDataFile*> allMapFiles;
        std::vector<AString> indexNames;
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        m_chartOverlay->getSelectionData(allMapFiles,
                                         mapFile,
                                         indexNames,
                                         selectedIndexType,
                                         selectedIndex);
        
        if (mapFile != NULL) {
            if ((selectedIndex >= 0)
                && (selectedIndex < static_cast<int32_t>(indexNames.size()))) {
                QApplication::clipboard()->setText(indexNames[selectedIndex],
                                                   QClipboard::Clipboard);
            }
        }
    }
}

/**
 * Reload the file in the overlay.
 */
void
ChartTwoOverlayViewController::menuReloadFileTriggered()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        m_chartOverlay->getSelectionData(mapFile,
                                         selectedIndexType,
                                         selectedIndex);
        
        if (mapFile != NULL) {
            AString username;
            AString password;
            
            if (DataFile::isFileOnNetwork(mapFile->getFileName())) {
                const QString msg("This file is on the network.  "
                                  "If accessing the file requires a username and "
                                  "password, enter it here.  Otherwise, remove any "
                                  "text from the username and password fields.");
                
                
                if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(m_constructionToolButton,
                                                                           "Username and Password",
                                                                           msg,
                                                                           username,
                                                                           password)) {
                    /* nothing */
                }
                else {
                    return;
                }
            }
            
            EventDataFileReload reloadEvent(GuiManager::get()->getBrain(),
                                            mapFile);
            reloadEvent.setUsernameAndPassword(username,
                                               password);
            EventManager::get()->sendEvent(reloadEvent.getPointer());
            
            if (reloadEvent.isError()) {
                WuQMessageBox::errorOk(m_constructionToolButton,
                                       reloadEvent.getErrorMessage());
            }
            
            updateOverlaySettingsEditor();
            
            updateUserInterfaceAndGraphicsWindow();
        }
    }
}

/**
 * Create a matrix view mode pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param matrixViewMode
 *    Matrix view mode represented by the icon.
 * @return
 *    Pixmap for matrix view mode.
 */
QPixmap
ChartTwoOverlayViewController::createMatrixTriangularViewModePixmap(QWidget* widget,
                                                                 const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const qreal iconSize = 24.0;
    const qreal minValue = 2.0;
    const qreal maxValue = iconSize - minValue;
    const QPointF bottomLeft(minValue, minValue);
    const QPointF bottomRight(maxValue, minValue);
    const QPointF topRight(maxValue, maxValue);
    const QPointF topLeft(minValue, maxValue);
    
    QPixmap pixmap(static_cast<int>(iconSize),
                   static_cast<int>(iconSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);

    QPen pen = painter->pen();
    pen.setWidthF(2.0);
    painter->setPen(pen);

    QPolygonF polygon;
    switch (matrixViewMode) {
        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
        {
            polygon.push_back(bottomLeft);
            polygon.push_back(bottomRight);
            polygon.push_back(topRight);
            polygon.push_back(topLeft);
        }
            break;
        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
        {
            polygon.push_back(bottomLeft);
            polygon.push_back(bottomRight);
            polygon.push_back(topRight);
            polygon.push_back(topLeft);
            
            painter->drawLine(topLeft, bottomRight);
        }
            break;
        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
        {
            polygon.push_back(bottomLeft);
            polygon.push_back(bottomRight);
            polygon.push_back(topLeft);
        }
            break;
        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
        {
            polygon.push_back(bottomRight);
            polygon.push_back(topRight);
            polygon.push_back(topLeft);
        }
            break;
    }
    

    painter->drawPolygon(polygon);
    
    return pixmap;
}

/**
 * Create a cardinal direction pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    Pixmap for cardinal direction.
 */
QPixmap
ChartTwoOverlayViewController::createCardinalDirectionPixmap(QWidget* widget)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const qreal iconSize = 32.0;
    const qreal minValue = 1.0;
    const qreal maxX((iconSize - minValue) / 2.0);
    const qreal maxY(maxX);
    const QPointF left(-maxX, 0.0);
    const QPointF right(maxX, 0.0);
    const QPointF top(0.0, maxY);
    const QPointF bottom(0.0, -maxY);
    
    QPixmap pixmap(static_cast<int>(iconSize),
                   static_cast<int>(iconSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap);
    
    QPen pen = painter->pen();
    pen.setWidthF(2.0);
    painter->setPen(pen);
    
    /*
     * Horizontal and vertical lines
     */
    painter->drawLine(left, right);
    painter->drawLine(bottom, top);
    
    /*
     * Arrow tips
     */
    const float tip(4.0);
    painter->drawLine(left.x(), left.y(), left.x() + tip, left.y() + tip);
    painter->drawLine(left.x(), left.y(), left.x() + tip, left.y() - tip);
    painter->drawLine(right.x(), right.y(), right.x() - tip, right.y() + tip);
    painter->drawLine(right.x(), right.y(), right.x() - tip, right.y() - tip);
    painter->drawLine(bottom.x(), bottom.y(), bottom.x() - tip, bottom.y() + tip);
    painter->drawLine(bottom.x(), bottom.y(), bottom.x() + tip, bottom.y() + tip);
    painter->drawLine(top.x(), top.y(), top.x() - tip, top.y() - tip);
    painter->drawLine(top.x(), top.y(), top.x() + tip, top.y() - tip);

    return pixmap;
}

/**
 * Pre-color all files.  Some files, such as large scalar matrix files, may take
 * time the first time they are displayed that is spent coloring the matrix
 * and creating an OpenGL texture used when drawing.
 */
void
ChartTwoOverlayViewController::menuConstructionPreColorAllFiles()
{
    QWidget* parentWidget(m_mapFileComboBox);
    
    if ( ! m_enabledCheckBox->isChecked()) {
        WuQMessageBox::errorOk(parentWidget,
                               "The layer must be On (checkbox on left)");
        return;
    }
    
    const int32_t numFiles = m_mapFileComboBox->count();
    if (numFiles <= 0) {
        return;
    }
    
    QPoint dialogXY(parentWidget->x() + parentWidget->width(),
                    parentWidget->y());
    ProgressReportingDialog progressDialog("Pre-Color Files",
                                           "Starting",
                                           parentWidget);
    progressDialog.move(parentWidget->mapToGlobal(dialogXY));
    
    const int32_t currentFileIndex = m_mapFileComboBox->currentIndex();
    
    for (int32_t i = 0; i < numFiles; i++) {
        EventProgressUpdate progEvent(1,
                                      numFiles,
                                      (i + 1),
                                      ("Coloring: "
                                       + m_mapFileComboBox->currentText()));
        EventManager::get()->sendEvent(progEvent.getPointer());
        
        if (progressDialog.wasCanceled()) {
            break;
        }
        
        QSignalBlocker blocker(m_mapFileComboBox);
        m_mapFileComboBox->setCurrentIndex(i);
        fileComboBoxSelected(i);
        
        /*
         * Need to to a graphics update with a repaint (a synchronous
         * graphics update) so that file is actually drawn.
         */
        EventGraphicsUpdateOneWindow graphicsEvent(m_browserWindowIndex,
                                                   true);
        EventManager::get()->sendEvent(graphicsEvent.getPointer());
        QApplication::processEvents();
    }
    
    QSignalBlocker blocker(m_mapFileComboBox);
    m_mapFileComboBox->setCurrentIndex(currentFileIndex);
    fileComboBoxSelected(currentFileIndex);
    
    m_mapFileComboBox->clearFocus();
}
