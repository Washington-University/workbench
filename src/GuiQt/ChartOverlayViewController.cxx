
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

#define __CHART_OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "ChartOverlayViewController.h"
#undef __CHART_OVERLAY_VIEW_CONTROLLER_DECLARE__

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

#include "CaretAssert.h"
using namespace caret;

#include "AnnotationColorBar.h"
#include "CaretMappableDataFile.h"
#include "ChartOverlay.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "UsernamePasswordWidget.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

/**
 * \class caret::ChartOverlayViewController 
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
 * @param parent
 *    The parent widget.
 */
ChartOverlayViewController::ChartOverlayViewController(const Qt::Orientation orientation,
                                                       const int32_t browserWindowIndex,
                                                       const int32_t chartOverlayIndex,
                                                       QObject* parent)
: QObject(parent),
m_browserWindowIndex(browserWindowIndex),
m_chartOverlayIndex(chartOverlayIndex),
m_chartOverlay(NULL)
{
    int minComboBoxWidth = 200;
    int maxComboBoxWidth = 100000; //400;
    if (orientation == Qt::Horizontal) {
        minComboBoxWidth = 50;
        maxComboBoxWidth = 100000;
    }
    const QComboBox::SizeAdjustPolicy comboSizePolicy = QComboBox::AdjustToContentsOnFirstShow; //QComboBox::AdjustToContents;
    
    /*
     * Enabled Check Box
     */
    const QString checkboxText = ((orientation == Qt::Horizontal) ? " " : " ");
    m_enabledCheckBox = new QCheckBox(checkboxText);
    QObject::connect(m_enabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(enabledCheckBoxClicked(bool)));
    m_enabledCheckBox->setToolTip("Enables display of this layer");
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);
    
    m_settingsAction = WuQtUtilities::createAction("S",
                                                   "Edit settings for this chart",
                                                   this,
                                                   this,
                                                   SLOT(settingsActionTriggered()));
    if (settingsIconValid) {
        m_settingsAction->setIcon(settingsIcon);
    }
    m_settingsToolButton = new QToolButton();
    m_settingsToolButton->setDefaultAction(m_settingsAction);
    
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    m_colorBarAction = WuQtUtilities::createAction("CB",
                                                       "Display color bar for this overlay",
                                                       this,
                                                       this,
                                                       SLOT(colorBarActionTriggered(bool)));
    m_colorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        m_colorBarAction->setIcon(colorBarIcon);
    }
    m_colorBarToolButton = new QToolButton();
    m_colorBarToolButton->setDefaultAction(m_colorBarAction);
    
     /*
     * Construction Tool Button
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                               constructionIcon);
    m_constructionAction = WuQtUtilities::createAction("M",
                                                           "Add/Move/Remove Layers",
                                                           this);
    if (constructionIconValid) {
        m_constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton);
    m_constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(m_constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    
    /*
     * History button
     */
    m_historyToolButton = new QToolButton();
    m_historyAction = WuQtUtilities::createAction("",
                                                  "Show history for line chart file",
                                                  this,
                                                  this,
                                                  SLOT(historyActionTriggered(bool)));
    QPixmap historyPixmap = createHistoryPixmap(m_historyToolButton);
    m_historyAction->setIcon(historyPixmap);
    m_historyToolButton->setDefaultAction(m_historyAction);
    //m_historyToolButton->setIconSize(historyPixmap.size());
    
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
    m_mapYokingGroupComboBox = new MapYokingGroupComboBox(this);
    m_mapYokingGroupComboBox->getWidget()->setStatusTip("Synchronize enabled status and map indices)");
    m_mapYokingGroupComboBox->getWidget()->setToolTip("Yoke to Overlay Mapped Files");
#ifdef CARET_OS_MACOSX
    m_mapYokingGroupComboBox->getWidget()->setFixedWidth(m_mapYokingGroupComboBox->getWidget()->sizeHint().width() - 20);
#endif // CARET_OS_MACOSX
    QObject::connect(m_mapYokingGroupComboBox, SIGNAL(itemActivated()),
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
    
    /*
     * Map Index Spin Box
     */
    m_mapIndexSpinBox = WuQFactory::newSpinBox();
    QObject::connect(m_mapIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(mapIndexSpinBoxValueChanged(int)));
    m_mapIndexSpinBox->setToolTip("Select map by its index");
    
    /*
     * Map Name Combo Box
     */
    m_mapNameComboBox = WuQFactory::newComboBox();
    m_mapNameComboBox->setMinimumWidth(minComboBoxWidth);
    m_mapNameComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(m_mapNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapNameComboBoxSelected(int)));
    m_mapNameComboBox->setToolTip("Select map by its name");
    m_mapNameComboBox->setSizeAdjustPolicy(comboSizePolicy);
}

/**
 * Destructor.
 */
ChartOverlayViewController::~ChartOverlayViewController()
{
}

/*
 * If this overlay ins an overlay settings editor, update its content
 */
void
ChartOverlayViewController::updateOverlaySettingsEditor()
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile = NULL;
    int32_t mapIndex = -1;
    m_chartOverlay->getSelectionData(mapFile,
                              mapIndex);
    
    if ((mapFile != NULL)
        && (mapIndex >= 0)) {
        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_OVERLAY_MAP_CHANGED,
                                                     m_browserWindowIndex,
                                                     m_chartOverlay,
                                                     mapFile,
                                                     mapIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void
ChartOverlayViewController::fileComboBoxSelected(int indx)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    void* pointer = m_mapFileComboBox->itemData(indx).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    m_chartOverlay->setSelectionData(file, 0);
    
    validateYokingSelection();
    
    //validateYokingSelection(overlay->getYokingGroup());
    // not needed with call to validateYokingSelection: this->updateViewController(this->overlay);
    
    // called inside validateYokingSelection();  this->updateUserInterfaceAndGraphicsWindow();
    
    updateOverlaySettingsEditor();
    updateViewController(m_chartOverlay);
    updateGraphicsWindow();
    updateUserInterface();
    
//    if (file != NULL) {
//        if (file->isVolumeMappable()) {
//            /*
//             * Need to update slice indices/coords in toolbar.
//             */
//            EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBar().getPointer());
//        }
//    }
}

/**
 * Called when a selection is made from the map index spin box.
 * @parm indx
 *    Index of selection.
 */
void
ChartOverlayViewController::mapIndexSpinBoxValueChanged(int indx)
{
    if (m_chartOverlay == NULL)
    {
        //TSC: not sure how to put the displayed integer back to 0 where it starts when opening without data files
        return;
    }
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = m_mapFileComboBox->currentIndex();
    void* pointer = m_mapFileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    /*
     * Overlay indices range [0, N-1] but spin box shows [1, N].
     */
    const int overlayIndex = indx - 1;
    
    m_chartOverlay->setSelectionData(file, overlayIndex);
    
    const MapYokingGroupEnum::Enum mapYoking = m_chartOverlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               file,
                                               overlayIndex,
                                               m_chartOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update map name combo box.
     */
    m_mapNameComboBox->blockSignals(true);
    if ((overlayIndex >= 0)
        && (overlayIndex < m_mapNameComboBox->count())) {
        m_mapNameComboBox->setCurrentIndex(overlayIndex);
    }
    m_mapNameComboBox->blockSignals(false);
    
    this->updateUserInterface();
    this->updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when a selection is made from the map name combo box.
 * @parm indx
 *    Index of selection.
 */
void
ChartOverlayViewController::mapNameComboBoxSelected(int indx)
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
                                               indx,
                                               m_chartOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update map index spin box.
     * Note that the map index spin box ranges [1, N].
     */
    m_mapIndexSpinBox->blockSignals(true);
    m_mapIndexSpinBox->setValue(indx + 1);
    m_mapIndexSpinBox->blockSignals(false);
    
    this->updateUserInterface();
    this->updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when enabled checkbox state is changed
 * @parm checked
 *    Checked status
 */
void
ChartOverlayViewController::enabledCheckBoxClicked(bool checked)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    m_chartOverlay->setEnabled(checked);
    
    const MapYokingGroupEnum::Enum mapYoking = m_chartOverlay->getMapYokingGroup();
    if (mapYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        CaretMappableDataFile* myFile = NULL;
        int32_t myIndex = -1;
        m_chartOverlay->getSelectionData(myFile,
                                        myIndex);
        
        EventMapYokingSelectMap selectMapEvent(mapYoking,
                                               myFile,
                                               myIndex,
                                               m_chartOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    this->updateUserInterface();
    
    this->updateGraphicsWindow();
}

/**
 * Called when colorbar toolbutton is toggled.
 * @param status
 *    New status.
 */
void
ChartOverlayViewController::colorBarActionTriggered(bool status)
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
ChartOverlayViewController::allMapsCheckBoxClicked(bool status)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    m_chartOverlay->setAllMapsSelected(status);
    
    this->updateGraphicsWindow();
}

/**
 * Validate yoking when there are changes made to the overlay.
 */
void
ChartOverlayViewController::validateYokingSelection()
{
    m_mapYokingGroupComboBox->validateYokingChange(m_chartOverlay);
    updateViewController(m_chartOverlay);
    updateGraphicsWindow();
    //EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the yoking group is changed.
 */
void
ChartOverlayViewController::yokingGroupActivated()
{
    MapYokingGroupEnum::Enum yokingGroup = m_mapYokingGroupComboBox->getMapYokingGroup();
    
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
ChartOverlayViewController::settingsActionTriggered()
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile;
    int32_t mapIndex = -1;
    m_chartOverlay->getSelectionData(mapFile,
                                    mapIndex);
    if (mapFile != NULL) {
        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_SHOW_EDITOR,
                                                     m_browserWindowIndex,
                                                     m_chartOverlay,
                                                     mapFile,
                                                     mapIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Called when the history action is selected.
 */
void
ChartOverlayViewController::historyActionTriggered(bool)
{
    if (m_chartOverlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile;
    int32_t mapIndex = -1;
    m_chartOverlay->getSelectionData(mapFile,
                                     mapIndex);
    
    CaretAssertToDoWarning();
//    if (mapFile != NULL) {
//        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_SHOW_EDITOR,
//                                                     m_browserWindowIndex,
//                                                     m_chartOverlay,
//                                                     mapFile,
//                                                     mapIndex);
//        EventManager::get()->sendEvent(pcme.getPointer());
//    }
}


/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void
ChartOverlayViewController::updateViewController(ChartOverlay* chartOverlay)
{
    m_chartOverlay = chartOverlay;
    
    m_mapFileComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    //AString selectedMapUniqueID = "";
    int32_t selectedMapIndex = -1;
    bool enableMapSelectionControlsFlag = false;
    
    if (m_chartOverlay != NULL) {
        m_chartOverlay->getSelectionData(dataFiles,
                                        selectedFile,
                                        //selectedMapUniqueID,
                                        selectedMapIndex);
        
        const ChartTwoDataTypeEnum::Enum chartDataType = m_chartOverlay->getChartDataType();
        switch (chartDataType) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                enableMapSelectionControlsFlag = true;
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                break;
        }
        
    }
    
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(dataFiles,
                                                                            displayNames);
    CaretAssert(dataFiles.size() == displayNames.size());
    
    /*
     * Load the file selection combo box.
     */
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
     * Load the map selection combo box
     */
    int32_t numberOfMaps = 0;
    m_mapNameComboBox->blockSignals(true);
    m_mapNameComboBox->clear();
    if (enableMapSelectionControlsFlag) {
        if (selectedFile != NULL) {
            numberOfMaps = selectedFile->getNumberOfMaps();
            for (int32_t i = 0; i < numberOfMaps; i++) {
                m_mapNameComboBox->addItem(selectedFile->getMapName(i));
            }
            m_mapNameComboBox->setCurrentIndex(selectedMapIndex);
        }
    }
    m_mapNameComboBox->blockSignals(false);
    
    /*
     * Load the map index spin box that ranges [1, N].
     */
    m_mapIndexSpinBox->blockSignals(true);
    if (enableMapSelectionControlsFlag) {
        m_mapIndexSpinBox->setRange(1, numberOfMaps);
        if (selectedFile != NULL) {
            m_mapIndexSpinBox->setValue(selectedMapIndex + 1);
        }
    }
    else {
        m_mapIndexSpinBox->setRange(0, 0);
        m_mapIndexSpinBox->setValue(0);
    }
    m_mapIndexSpinBox->blockSignals(false);
    
    /*
     * Update enable check box
     */
    Qt::CheckState enabledCheckState = Qt::Unchecked;
    bool allMapsCheckedFlag = false;
    MapYokingGroupEnum::Enum mapYoking = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    if (m_chartOverlay != NULL) {
        if (m_chartOverlay->isEnabled()) {
            enabledCheckState = Qt::Checked;
        }
        allMapsCheckedFlag = m_chartOverlay->isAllMapsSelected();
        mapYoking = m_chartOverlay->getMapYokingGroup();
    }
    m_enabledCheckBox->setCheckState(enabledCheckState);
    
    m_mapYokingGroupComboBox->setMapYokingGroup(mapYoking);
    
    m_allMapsCheckBox->setChecked(allMapsCheckedFlag);
    
    
    m_colorBarAction->blockSignals(true);
    m_colorBarAction->setChecked(m_chartOverlay->getColorBar()->isDisplayed());
    m_colorBarAction->blockSignals(false);
    
    const bool haveFile = (selectedFile != NULL);
    bool haveMultipleMaps = false;
    bool dataIsMappedWithPalette = false;
    bool dataIsMappedWithLabelTable = false;
    bool dataIsMappedWithRGBA = false;
    bool haveOpacity = false;
    if (haveFile) {
        dataIsMappedWithPalette = selectedFile->isMappedWithPalette();
        dataIsMappedWithLabelTable = selectedFile->isMappedWithLabelTable();
        dataIsMappedWithRGBA    = selectedFile->isMappedWithRGBA();
        haveMultipleMaps = (selectedFile->getNumberOfMaps() > 1);
        haveOpacity = (dataIsMappedWithLabelTable
                       || dataIsMappedWithPalette
                       || dataIsMappedWithRGBA);
    }
    
    /**
     * Yoking is enabled when either:
     * (1) The file maps to both surface and volumes
     * (2) The file has multiple maps.
     */
    bool haveYoking = false;
    if (haveFile) {
        if (selectedFile->isSurfaceMappable()
            && selectedFile->isVolumeMappable()) {
            haveYoking = true;
        }
        if (haveMultipleMaps) {
            haveYoking = true;
        }
    }
    
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
                                   + m_mapNameComboBox->currentText());
    }
    m_mapFileComboBox->setToolTip(fileComboBoxToolTip);
    m_mapNameComboBox->setToolTip(nameComboBoxToolTip);
    
    /*
     * Make sure items are enabled at the appropriate time
     * Note: First overlay is ALWAYS on enabled checkbox is not selectable by user
     */
    m_enabledCheckBox->setEnabled(haveFile
                                  && (m_chartOverlayIndex > 0));
    m_enabledCheckBox->setVisible(m_chartOverlayIndex > 0);
    m_settingsAction->setEnabled(true);
    m_colorBarAction->setEnabled(dataIsMappedWithPalette);
    m_constructionAction->setEnabled(true);
    m_historyAction->setEnabled(haveMultipleMaps);
    m_mapFileComboBox->setEnabled(haveFile);
    m_mapYokingGroupComboBox->getWidget()->setEnabled(haveYoking);
    m_allMapsCheckBox->setEnabled(haveMultipleMaps
                                  && enableMapSelectionControlsFlag);
    m_mapIndexSpinBox->setEnabled(haveMultipleMaps
                                  && enableMapSelectionControlsFlag);
    m_mapNameComboBox->setEnabled(haveFile
                                  && enableMapSelectionControlsFlag);
}

/**
 * Update graphics and GUI after selections made
 */
void
ChartOverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    updateUserInterface();
    updateGraphicsWindow();
}

/**
 * Update graphics and GUI after selections made
 */
void
ChartOverlayViewController::updateUserInterface()
{
    if (m_chartOverlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).getPointer());
    }
}

/**
 * Update graphics after selections made
 */
void
ChartOverlayViewController::updateGraphicsWindow()
{
    if (m_chartOverlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
}

/**
 * Create the construction menu.
 * @param parent
 *    Parent widget.
 */
QMenu*
ChartOverlayViewController::createConstructionMenu(QWidget* parent)
{
    QMenu* menu = new QMenu(parent);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(menuConstructionAboutToShow()));
    
    menu->addAction("Add Overlay Above",
                    this,
                    SLOT(menuAddOverlayAboveTriggered()));
    
    menu->addAction("Add Overlay Below",
                    this,
                    SLOT(menuAddOverlayBelowTriggered()));
    
    menu->addSeparator();
    
    menu->addAction("Move Overlay Up",
                    this,
                    SLOT(menuMoveOverlayUpTriggered()));
    
    menu->addAction("Move Overlay Down",
                    this,
                    SLOT(menuMoveOverlayDownTriggered()));
    
    menu->addSeparator();
    
    menu->addAction("Remove This Overlay",
                    this,
                    SLOT(menuRemoveOverlayTriggered()));
    
    menu->addSeparator();
    
    m_constructionReloadFileAction = menu->addAction("Reload Selected File",
                                                     this,
                                                     SLOT(menuReloadFileTriggered()));
    
    menu->addSeparator();
    
    menu->addAction("Copy Path and File Name to Clipboard",
                    this,
                    SLOT(menuCopyFileNameToClipBoard()));
    
    menu->addAction("Copy Map Name to Clipboard",
                    this,
                    SLOT(menuCopyMapNameToClipBoard()));
    
    return menu;
    
}

/**
 * Called when construction menu is about to be displayed.
 */
void
ChartOverlayViewController::menuConstructionAboutToShow()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        m_chartOverlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        QString menuText = "Reload Selected File";
        if (caretDataFile != NULL) {
            if (caretDataFile->isModified()) {
                QString suffix = " (MODIFIED)";
                if (caretDataFile->isModifiedPaletteColorMapping()) {
                    if ( ! caretDataFile->isModifiedExcludingPaletteColorMapping()) {
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
ChartOverlayViewController::menuAddOverlayAboveTriggered()
{
    emit requestAddOverlayAbove(m_chartOverlayIndex);
}

/**
 * Add an overlay below this overlay.
 */
void
ChartOverlayViewController::menuAddOverlayBelowTriggered()
{
    emit requestAddOverlayBelow(m_chartOverlayIndex);
}

/**
 * Remove this overlay.
 */
void
ChartOverlayViewController::menuRemoveOverlayTriggered()
{
    emit requestRemoveOverlay(m_chartOverlayIndex);
}

/**
 * Move this overlay down.
 */
void
ChartOverlayViewController::menuMoveOverlayDownTriggered()
{
    emit requestMoveOverlayDown(m_chartOverlayIndex);
}

/**
 * Move this overlay down.
 */
void
ChartOverlayViewController::menuMoveOverlayUpTriggered()
{
    emit requestMoveOverlayUp(m_chartOverlayIndex);
}

/**
 * Copy the file name to the clip board.
 */
void
ChartOverlayViewController::menuCopyFileNameToClipBoard()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        m_chartOverlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        if (caretDataFile != NULL) {
            QApplication::clipboard()->setText(caretDataFile->getFileName().trimmed(),
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Copy the map name to the clip board.
 */
void
ChartOverlayViewController::menuCopyMapNameToClipBoard()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        m_chartOverlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        if (caretDataFile != NULL) {
            if ((mapIndex >= 0)
                && (mapIndex < caretDataFile->getNumberOfMaps())) {
                QApplication::clipboard()->setText(caretDataFile->getMapName(mapIndex).trimmed(),
                                                   QClipboard::Clipboard);
            }
        }
    }
}

/**
 * Reload the file in the overlay.
 */
void
ChartOverlayViewController::menuReloadFileTriggered()
{
    if (m_chartOverlay != NULL) {
        CaretMappableDataFile* caretDataFile = NULL;
        int32_t mapIndex = -1;
        m_chartOverlay->getSelectionData(caretDataFile,
                                        mapIndex);
        
        if (caretDataFile != NULL) {
            AString username;
            AString password;
            
            if (DataFile::isFileOnNetwork(caretDataFile->getFileName())) {
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
                                            caretDataFile);
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
 * Create a history pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    Pixmap for history.
 */
QPixmap
ChartOverlayViewController::createHistoryPixmap(QWidget* widget)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const qreal iconSize = 24.0;
    const qreal centerX = iconSize / 2.0;
    const qreal centerY = iconSize / 2.0;
    
    QPixmap pixmap(static_cast<int>(iconSize),
                   static_cast<int>(iconSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    
    QPen pen = painter->pen();
    pen.setWidthF(2.0);
    painter->setPen(pen);
    
    const QPointF circleCenterXY(centerX, centerY);
    const qreal   circleRadius = iconSize * 0.45;
    
    painter->drawEllipse(circleCenterXY,
                         circleRadius, circleRadius);
    
    const qreal nineOClockX = circleRadius * 0.5; //0.25 * iconSize;
    const qreal twelveOClockY = 0.15 * iconSize;
    painter->drawLine(QPointF(centerX, centerY),
                      QPointF(nineOClockX, centerY));
    painter->drawLine(QPointF(centerX, centerY),
                      QPointF(centerY, twelveOClockY));
    
//        painter->drawLine(QLineF(xStart,
//                                 y,
//                                 xEnd,
//                                 y));
    
    return pixmap;
    
}



