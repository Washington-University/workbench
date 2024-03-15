
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

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __HISTOLOGY_OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "HistologyOverlayViewController.h"
#undef __HISTOLOGY_OVERLAY_VIEW_CONTROLLER_DECLARE__

#include "BrowserTabContent.h"
#include "CaretMappableDataFile.h"
#include "EventBrowserTabGet.h"
#include "EventDataFileReload.h"
#include "EventGraphicsPaintNowOneWindow.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventGraphicsPaintSoonOneWindow.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "HistologyOverlay.h"
#include "HistologyOverlaySettingsMenu.h"
#include "HistologySlicesFile.h"
#include "UsernamePasswordWidget.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQGridLayoutGroup.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;
    
/**
 * \class caret::HistologyOverlayViewController
 * \brief View Controller for a histology overlay.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param orientation
 *    Orientation of overlay (horizontal/vertical)
 * @param gridLayout
 *    Layout for widegets
 * @param browserWindowIndex
 *    Index of browser window in which this view controller resides.
 * @param overlayIndex
 *    Index of the overlay
 * @param parentObjectName
 *    Name of parent object for macros
 * @param parent
 *    The parent widget.
 */
HistologyOverlayViewController::HistologyOverlayViewController(const Qt::Orientation orientation,
                                             QGridLayout* gridLayout,
                                             const int32_t browserWindowIndex,
                                             const int32_t overlayIndex,
                                             const QString& parentObjectName,
                                             QObject* parent)
: QObject(parent),
  m_browserWindowIndex(browserWindowIndex),
  m_overlayIndex(overlayIndex),
  m_parentObjectName(parentObjectName)
{
    m_histologyOverlay = NULL;
    m_constructionReloadFileAction = NULL;
    
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
                                       + ":HistologyOverlay%1"
                                       + ":").arg((int)(overlayIndex + 1), 2, 10, QLatin1Char('0'));
    const QString descriptivePrefix = QString("overlay %1").arg(overlayIndex + 1);
    /*
     * Enabled Check Box
     */
    const QString checkboxText = ((orientation == Qt::Horizontal) ? " " : " ");
    m_enabledCheckBox = new QCheckBox(checkboxText);
    m_enabledCheckBox->setObjectName(objectNamePrefix
                                         + "OnOff");
    QObject::connect(m_enabledCheckBox, &QCheckBox::clicked,
                     this, &HistologyOverlayViewController::enabledCheckBoxClicked);
    m_enabledCheckBox->setToolTip("Enables display of this overlay");
    macroManager->addMacroSupportToObject(m_enabledCheckBox,
                                          "Enable " + descriptivePrefix);
    
    /*
     * File Selection Combo Box
     */
    m_fileComboBox = WuQFactory::newComboBox();
    m_fileComboBox->setObjectName(objectNamePrefix
                                      + "FileSelection");
    m_fileComboBox->setMinimumWidth(minComboBoxWidth);
    m_fileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(m_fileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                     this, &HistologyOverlayViewController::fileComboBoxSelected);
    m_fileComboBox->setToolTip("Selects file for this overlay");
    m_fileComboBox->setSizeAdjustPolicy(comboSizePolicy);
    macroManager->addMacroSupportToObject(m_fileComboBox,
                                          ("Select file in " + descriptivePrefix));
    
    /*
     * Slice Index Spin Box
     */
    m_sliceIndexSpinBox = WuQFactory::newSpinBox();
    m_sliceIndexSpinBox->setMinimumWidth(50);
    m_sliceIndexSpinBox->setObjectName(objectNamePrefix
                                         + "SliceIndex");
    QObject::connect(m_sliceIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &HistologyOverlayViewController::sliceIndexSpinBoxValueChanged);
    WuQtUtilities::setWordWrappedToolTip(m_sliceIndexSpinBox,
                                         "Select slice by its index");
    macroManager->addMacroSupportToObject(m_sliceIndexSpinBox,
                                          ("Select " + descriptivePrefix + " slice index"));
    
    /*
     * Slice Number Combo Box
     */
    m_nameToolTipText = ("Select slice by its number");
    m_sliceNameComboBox = WuQFactory::newComboBox();
    m_sliceNameComboBox->setObjectName(objectNamePrefix
                                      + "SliceNameSelection");
    m_sliceNameComboBox->setMinimumWidth(minComboBoxWidth);
    m_sliceNameComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(m_sliceNameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                     this, &HistologyOverlayViewController::sliceNameComboBoxSelected);
    WuQtUtilities::setWordWrappedToolTip(m_sliceNameComboBox,
                                         m_nameToolTipText);
    m_sliceNameComboBox->setSizeAdjustPolicy(comboSizePolicy);
    macroManager->addMacroSupportToObject(m_sliceNameComboBox,
                                          ("Select " + descriptivePrefix + " slice number"));
    
    /*
     * Opacity double spin box
     */
    m_opacityDoubleSpinBox = WuQFactory::newDoubleSpinBox();
    m_opacityDoubleSpinBox->setObjectName(objectNamePrefix
                                              + "Opacity");
    m_opacityDoubleSpinBox->setMinimum(0.0);
    m_opacityDoubleSpinBox->setMaximum(1.0);
    m_opacityDoubleSpinBox->setSingleStep(0.05);
    m_opacityDoubleSpinBox->setDecimals(2);
    m_opacityDoubleSpinBox->setFixedWidth(50);
    QObject::connect(m_opacityDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &HistologyOverlayViewController::opacityDoubleSpinBoxValueChanged);
    m_opacityDoubleSpinBox->setToolTip("Opacity (0.0=transparent, 1.0=opaque)");
    macroManager->addMacroSupportToObject(m_opacityDoubleSpinBox,
                                          ("Set " + descriptivePrefix + " opacity"));
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);

    m_settingsAction = WuQtUtilities::createAction("S",
                                                   "Edit settings for this slice and overlay",
                                                   this,
                                                   this,
                                                   SLOT(settingsActionTriggered()));
    m_settingsAction->setObjectName(objectNamePrefix
                                        + "ShowSettingsDialog");
    if (settingsIconValid) {
        m_settingsAction->setIcon(settingsIcon);
    }
    m_settingsToolButton = new QToolButton();
    m_settingsToolButton->setDefaultAction(m_settingsAction);
    macroManager->addMacroSupportToObject(m_settingsAction,
                                          ("Display " + descriptivePrefix + " palette settings"));
    
    /*
     * Construction Tool Button
     * Note: macro support is on each action in menu in 'createConstructionMenu'
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                           constructionIcon);
    m_constructionAction = WuQtUtilities::createAction("M",
                                                           "Add/Move/Remove Overlays", 
                                                           this);
    if (constructionIconValid) {
        m_constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton,
                                                     descriptivePrefix,
                                                     (objectNamePrefix
                                                      + "ConstructionMenu:"));
    m_constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(m_constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    
    const AString yokeToolTip("Select a yoking group.\n"
                              "\n"
                              "When files with more than one slice are yoked,\n"
                              "the seleted slices are synchronized by slice index.\n"
                              "If the SAME FILE is in yoked in multiple overlays,\n"
                              "the overlay enabled statuses are synchronized.\n"
                              "This control is disabled if file contains one slice.");

    /*
     * Yoking Group
     * Note: macro support is in the class MapYokingGroupComboBox
     */
    m_sliceYokingGroupComboBox = new MapYokingGroupComboBox(this,
                                                          (objectNamePrefix
                                                           + "SliceYokingSelection"),
                                                          descriptivePrefix);
    m_sliceYokingGroupComboBox->getWidget()->setStatusTip("Synchronize enabled status and slice indices)");
    m_sliceYokingGroupComboBox->getWidget()->setToolTip(yokeToolTip);
    QObject::connect(m_sliceYokingGroupComboBox, &MapYokingGroupComboBox::itemActivated,
                     this, &HistologyOverlayViewController::yokingGroupActivated);

    /*
     * Use layout group so that items can be shown/hidden
     */
    m_gridLayoutGroup = new WuQGridLayoutGroup(gridLayout, this);
    
    if (orientation == Qt::Horizontal) {
        int row = m_gridLayoutGroup->rowCount();
        m_gridLayoutGroup->addWidget(m_enabledCheckBox,
                                         row, 0,
                                         Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_settingsToolButton,
                                         row, 1,
                                         Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_constructionToolButton,
                                         row, 3);
        m_gridLayoutGroup->addWidget(m_opacityDoubleSpinBox,
                                         row, 4);
        m_gridLayoutGroup->addWidget(m_fileComboBox,
                                         row, 5);
        m_gridLayoutGroup->addWidget(m_sliceYokingGroupComboBox->getWidget(),
                                         row, 6,
                                         Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(m_sliceIndexSpinBox,
                                         row, 7);
        m_gridLayoutGroup->addWidget(m_sliceNameComboBox,
                                         row, 8);
        
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        QLabel* fileLabel = new QLabel("File");
        QLabel* sliceLabel = new QLabel("Slice ");
        QLabel* yokeLabel = new QLabel("Yoke");
        
        int row = m_gridLayoutGroup->rowCount();
        m_gridLayoutGroup->addWidget(m_enabledCheckBox,
                                         row, 0);
        m_gridLayoutGroup->addWidget(m_settingsToolButton,
                                         row, 1);
        m_gridLayoutGroup->addWidget(m_constructionToolButton,
                                         row, 2);
        m_gridLayoutGroup->addWidget(yokeLabel,
                                         row, 3, Qt::AlignHCenter);
        m_gridLayoutGroup->addWidget(fileLabel,
                                         row, 4);
        m_gridLayoutGroup->addWidget(m_fileComboBox,
                                         row, 5, 1, 2);
        
        row++;
        m_gridLayoutGroup->addWidget(m_opacityDoubleSpinBox,
                                         row, 0,
                                         1, 3,
                                         Qt::AlignCenter);
        m_gridLayoutGroup->addWidget(m_sliceYokingGroupComboBox->getWidget(),
                                         row, 3);
        m_gridLayoutGroup->addWidget(sliceLabel,
                                         row, 4);
        m_gridLayoutGroup->addWidget(m_sliceIndexSpinBox,
                                         row, 5);
        m_gridLayoutGroup->addWidget(m_sliceNameComboBox,
                                         row, 6);
    
        row++;
        m_gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
                                         row, 0, 1, -1);
    }
}

/**
 * Destructor.
 */
HistologyOverlayViewController::~HistologyOverlayViewController()
{
    
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
HistologyOverlayViewController::setVisible(bool visible)
{
    m_gridLayoutGroup->setVisible(visible);
}

/*
 * If this overlay ins an overlay settings editor, update its content
 */
void
HistologyOverlayViewController::updateOverlaySettingsEditor()
{
    if (m_histologyOverlay == NULL) {
        return;
    }

    const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
    
    if ((selectionData.m_selectedFile != NULL)
        && (selectionData.m_selectedSliceIndex >= 0)) {
//        CaretAssertToDoWarning();
//        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_OVERLAY_MAP_CHANGED,
//                                                     m_browserWindowIndex,
//                                                     m_histologyOverlay,
//                                                     mediaFile,
//                                                     frameIndex);
//        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
HistologyOverlayViewController::fileComboBoxSelected(int indx)
{
    if (m_histologyOverlay == NULL) {
        return;
    }
    
    void* pointer = m_fileComboBox->itemData(indx).value<void*>();
    HistologySlicesFile* file = (HistologySlicesFile*)pointer;
    m_histologyOverlay->setSelectionData(file, 0);
    
    validateYokingSelection();
    
    updateOverlaySettingsEditor();
    updateViewController(m_histologyOverlay);
    
    updateGraphicsWindow();
}

/**
 * Called when a selection is made from the slice index spin box.
 * @parm indx
 *    Index of selection.
 */
void
HistologyOverlayViewController::sliceIndexSpinBoxValueChanged(int indx)
{
    if (m_histologyOverlay == NULL) {
        return;
    }
    
    /*
     * Get the selection information for the overlay.
     */
    HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
    
    /*
     * spin box is 1..N but slice indices are 0..N-1
     */
    const int32_t sliceIndex(indx - 1);
    m_histologyOverlay->setSelectionData(selectionData.m_selectedFile,
                                         sliceIndex);
    
    /*
     * Selecting index implies Media all frames is OFF
     */
    const MapYokingGroupEnum::MediaAllFramesStatus mediaAllFramesStatus(MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_OFF);

    const MapYokingGroupEnum::Enum sliceYoking = m_histologyOverlay->getMapYokingGroup();
    if (sliceYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        EventMapYokingSelectMap selectMapEvent(sliceYoking,
                                               NULL,
                                               NULL,
                                               selectionData.m_selectedFile,
                                               NULL,
                                               sliceIndex,
                                               mediaAllFramesStatus,
                                               m_histologyOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    updateViewController(m_histologyOverlay);
    
    updateUserInterface();
    updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when a selection is made from the slice number combo box.
 * @parm itemIndex
 *    Index of selection.
 */
void 
HistologyOverlayViewController::sliceNameComboBoxSelected(int itemIndex)
{
    if (m_histologyOverlay == NULL) {
        return;
    }
    
    /*
     * Get the selection information for the overlay.
     */
    HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());

    /*
     * Slice number is in combo box item data
     */
    if ((itemIndex >= 0)
        && (itemIndex < m_sliceNameComboBox->count())) {
        MapYokingGroupEnum::MediaAllFramesStatus mediaAllFramesStatus(MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_NO_CHANGE);
        const int32_t sliceIndex = m_sliceNameComboBox->itemData(itemIndex).toInt();
        m_histologyOverlay->setSelectionData(selectionData.m_selectedFile,
                                             sliceIndex);
        
        const MapYokingGroupEnum::Enum sliceYoking = m_histologyOverlay->getMapYokingGroup();
        if (sliceYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            EventMapYokingSelectMap selectMapEvent(sliceYoking,
                                                   NULL,
                                                   NULL,
                                                   selectionData.m_selectedFile,
                                                   NULL,
                                                   sliceIndex,
                                                   mediaAllFramesStatus,
                                                   m_histologyOverlay->isEnabled());
            EventManager::get()->sendEvent(selectMapEvent.getPointer());
        }
    }

    
    updateViewController(m_histologyOverlay);

    updateUserInterface();
    updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when enabled checkbox state is changed
 * @parm checked
 *    Checked status
 */
void 
HistologyOverlayViewController::enabledCheckBoxClicked(bool checked)
{
    if (m_histologyOverlay == NULL) {
        return;
    }
    m_histologyOverlay->setEnabled(checked);
    
    const MapYokingGroupEnum::Enum sliceYoking = m_histologyOverlay->getMapYokingGroup();
    if (sliceYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());

        const MapYokingGroupEnum::MediaAllFramesStatus mediaAllFramesStatus(MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_NO_CHANGE);
        EventMapYokingSelectMap selectMapEvent(sliceYoking,
                                               NULL,
                                               NULL,
                                               selectionData.m_selectedFile,
                                               NULL,
                                               selectionData.m_selectedSliceIndex,
                                               mediaAllFramesStatus,
                                               m_histologyOverlay->isEnabled());
        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    updateUserInterface();

    updateGraphicsWindow();
}


/**
 * Called when opacity value is changed.
 * @param value
 *    New value.
 */
void 
HistologyOverlayViewController::opacityDoubleSpinBoxValueChanged(double value)
{
    if (m_histologyOverlay == NULL) {
        return;
    }
    
    m_histologyOverlay->setOpacity(value);
    
    updateGraphicsWindow();
}

/**
 * Validate yoking when there are changes made to the overlay.
 */
void
HistologyOverlayViewController::validateYokingSelection()
{
    m_sliceYokingGroupComboBox->validateYokingChange(m_histologyOverlay);
    updateViewController(m_histologyOverlay);
    updateGraphicsWindow();
}

/**
 * Called when the yoking group is changed.
 */
void
HistologyOverlayViewController::yokingGroupActivated()
{
    MapYokingGroupEnum::Enum yokingGroup = m_sliceYokingGroupComboBox->getMapYokingGroup();
   
    /*
     * Has yoking group changed?
     * TSC: overlay can be null when opened without loaded files
     */
    if (m_histologyOverlay != NULL && yokingGroup != m_histologyOverlay->getMapYokingGroup()) {
        validateYokingSelection();
    }
}


/**
 * Called when the settings action is selected.
 */
void 
HistologyOverlayViewController::settingsActionTriggered()
{
    if (m_histologyOverlay == NULL) {
        return;
    }
    
    const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
    if (selectionData.m_selectedFile != NULL) {
        HistologyOverlaySettingsMenu menu(m_histologyOverlay,
                                          m_parentObjectName);
        menu.exec(m_settingsToolButton->mapToGlobal(QPoint(0,0)));
//        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_SHOW_EDITOR,
//                                                     m_browserWindowIndex,
//                                                     m_histologyOverlay,
//                                                     mediaFile,
//                                                     frameIndex);
//        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void 
HistologyOverlayViewController::updateViewController(HistologyOverlay* overlay)
{
    m_histologyOverlay = overlay;

    /*
     * Get the selection information for the overlay.
     */
    HistologyOverlay::SelectionData selectionData;
    if (m_histologyOverlay != NULL) {
        selectionData = m_histologyOverlay->getSelectionData();
    }
    
    m_fileComboBox->clear();
    
    std::vector<CaretDataFile*> caretDataFiles(selectionData.m_allFiles.begin(),
                                               selectionData.m_allFiles.end());
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(caretDataFiles,
                                                                            displayNames);
    CaretAssert(selectionData.m_allFiles.size() == displayNames.size());

    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(selectionData.m_allFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        HistologySlicesFile* dataFile = selectionData.m_allFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toOverlayTypeName(dataFile->getDataFileType());
        CaretAssertVectorIndex(displayNames, i);
        m_fileComboBox->addItem(displayNames[i],
                                    QVariant::fromValue((void*)dataFile));
        if (dataFile == selectionData.m_selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        m_fileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the slice number selection combo box
     */
    int32_t numberOfSlices = 0;
    m_sliceNameComboBox->blockSignals(true);
    m_sliceNameComboBox->clear();
    if (selectionData.m_selectedFile != NULL) {
        numberOfSlices = selectionData.m_selectedFile->getNumberOfHistologySlices();
        
        /*
         * User data of combo box items contains either ALL_FRAMES
         * or selected slice index
         */
        int32_t selectedComboBoxIndex(0);
        for (int32_t sliceIndex = 0; sliceIndex < numberOfSlices; sliceIndex++) {
            const AString sliceName(selectionData.m_selectedFile->getSliceNameBySliceIndex(sliceIndex));
            const AString sliceNameText(sliceName);
            m_sliceNameComboBox->addItem(sliceNameText,
                                           sliceIndex);
            if (sliceIndex == selectionData.m_selectedSliceIndex) {
                selectedComboBoxIndex = m_sliceNameComboBox->count() - 1;
            }
        }
        
        if (selectedComboBoxIndex < m_sliceNameComboBox->count()) {
            m_sliceNameComboBox->setCurrentIndex(selectedComboBoxIndex);
        }
    }
    m_sliceNameComboBox->blockSignals(false);
    
    m_sliceIndexSpinBox->blockSignals(true);
    m_sliceIndexSpinBox->setRange(1, numberOfSlices);
    if (selectionData.m_selectedFile != NULL) {
        /*
         * Spin box is 1..N but slice index is 0..N-1
         */
        m_sliceIndexSpinBox->setValue(selectionData.m_selectedSliceIndex + 1);
    }
    m_sliceIndexSpinBox->blockSignals(false);

    /*
     * Update enable check box
     */
    Qt::CheckState checkState = Qt::Unchecked;
    if (m_histologyOverlay != NULL) {
        if (m_histologyOverlay->isEnabled()) {
            checkState = Qt::Checked;
        }
    }
    m_enabledCheckBox->setCheckState(checkState);
    
    m_sliceYokingGroupComboBox->setMapYokingGroup(overlay->getMapYokingGroup());
    
    m_opacityDoubleSpinBox->blockSignals(true);
    m_opacityDoubleSpinBox->setValue(m_histologyOverlay->getOpacity());
    m_opacityDoubleSpinBox->blockSignals(false);

    const bool haveFile = (selectionData.m_selectedFile != NULL);
    bool haveMultipleSlices = false;
    bool haveOpacity = false;
    if (haveFile) {
        haveMultipleSlices = (selectionData.m_selectedFile->getNumberOfHistologySlices() > 1);
    }
    
    /**
     * Yoking is enabled when file has multiple slices
     */
    bool haveYoking = false;
    if (haveFile) {
        if (haveMultipleSlices) {
            haveYoking = true;
        }
    }
    
    /*
     * Update tooltips with full path to file and name of slice
     * as names may be too long to fit into combo boxes
     */
    AString fileComboBoxToolTip("Select file for this overlay");
    AString nameComboBoxToolTip(m_nameToolTipText);
    if (selectionData.m_selectedFile != NULL) {
        FileInformation fileInfo(selectionData.m_selectedFile->getFileName());
        fileComboBoxToolTip.append(":\n"
                                   + fileInfo.getFileName()
                                   + "\n"
                                   + fileInfo.getPathName()
                                   + "\n\n"
                                   + "Copy File Name/Path to Clipboard with Construction Menu");
        
        const QString sliceName(m_sliceNameComboBox->currentText());
        if (sliceName.length() > 15) {
            nameComboBoxToolTip.append(":\n"
                                       + sliceName);
        }
    }
    m_fileComboBox->setToolTip(fileComboBoxToolTip);
    WuQtUtilities::setWordWrappedToolTip(m_sliceNameComboBox,
                                         nameComboBoxToolTip);

    /*
     * Make sure items are enabled at the appropriate time
     */
    m_fileComboBox->setEnabled(haveFile);
    m_sliceNameComboBox->setEnabled(haveMultipleSlices);
    m_sliceIndexSpinBox->setEnabled(haveMultipleSlices);
    m_enabledCheckBox->setEnabled(haveFile);
    m_constructionAction->setEnabled(true);
    m_opacityDoubleSpinBox->setEnabled(haveOpacity);
    m_sliceYokingGroupComboBox->getWidget()->setEnabled(haveYoking);
    m_settingsAction->setEnabled(true);
    
    m_sliceIndexSpinBox->setEnabled(false);
    m_sliceNameComboBox->setEnabled(false);
}

/**
 * Update graphics and GUI after selections made
 */
void 
HistologyOverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    updateUserInterface();
    updateGraphicsWindow();
}

/**
 * Update graphics and GUI after selections made
 */
void
HistologyOverlayViewController::updateUserInterface()
{
    if (m_sliceYokingGroupComboBox->getWidget()->isEnabled()
        && (m_sliceYokingGroupComboBox->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF)) {
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
HistologyOverlayViewController::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    if (m_sliceYokingGroupComboBox->getWidget()->isEnabled()
        && (m_sliceYokingGroupComboBox->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF)) {
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(m_browserWindowIndex).getPointer());
    }
}

/**
 * Create the construction menu.
 * @param parent
 *    Parent widget.
 * @param descriptivePrefix
 *    Descriptive prefix
 * @param menuActionNamePrefix
 *    Prefix for macros
 */
QMenu* 
HistologyOverlayViewController::createConstructionMenu(QWidget* parent,
                                              const AString& descriptivePrefix,
                                              const AString& menuActionNamePrefix)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    QMenu* menu = new QMenu(parent);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &HistologyOverlayViewController::menuConstructionAboutToShow);
    
    QAction* addAboveAction = menu->addAction("Add Overlay Above",
                                              this,
                                              SLOT(menuAddOverlayAboveTriggered()));
    addAboveAction->setObjectName(menuActionNamePrefix
                                  + "AddOverlayAbove");
    addAboveAction->setToolTip("Add an overlay above this overlay");
    macroManager->addMacroSupportToObject(addAboveAction,
                                          ("Add overlay above " + descriptivePrefix));
    
    QAction* addBelowAction = menu->addAction("Add Overlay Below",
                                              this,
                                              SLOT(menuAddOverlayBelowTriggered()));
    addBelowAction->setObjectName(menuActionNamePrefix
                                  + "AddOverlayBelow");
    addBelowAction->setToolTip("Add an overlay below this overlay");
    macroManager->addMacroSupportToObject(addBelowAction,
                                          ("Add overlay below " + descriptivePrefix));
    
    menu->addSeparator();
    
    QAction* moveUpAction = menu->addAction("Move Overlay Up",
                                            this,
                                            SLOT(menuMoveOverlayUpTriggered()));
    moveUpAction->setObjectName(menuActionNamePrefix
                                + "MoveOverlayUp");
    moveUpAction->setToolTip("Move this overlay up");
    macroManager->addMacroSupportToObject(moveUpAction,
                                          ("Move " + descriptivePrefix + " up"));
    
    QAction* moveDownAction = menu->addAction("Move Overlay Down",
                                              this,
                                              SLOT(menuMoveOverlayDownTriggered()));
    moveDownAction->setObjectName(menuActionNamePrefix
                                  + "MoveOverlayDown");
    moveDownAction->setToolTip("Move this overlay down");
    macroManager->addMacroSupportToObject(moveDownAction,
                                          ("Move " + descriptivePrefix + " down"));
    
    menu->addSeparator();
    
    QAction* removeAction = menu->addAction("Remove This Overlay",
                                            this,
                                            SLOT(menuRemoveOverlayTriggered()));
    removeAction->setObjectName(menuActionNamePrefix
                                + "RemoveOverlay");
    removeAction->setToolTip("Remove this overlay");
    macroManager->addMacroSupportToObject(removeAction,
                                          ("Remove " + descriptivePrefix));
    
    menu->addSeparator();
    
    m_constructionReloadFileAction = menu->addAction("Reload Selected File",
                                                     this,
                                                     SLOT(menuReloadFileTriggered()));
    m_constructionReloadFileAction->setObjectName(menuActionNamePrefix
                                                  + "ReloadSelectedFile");
    m_constructionReloadFileAction->setToolTip("Reload file in this overlay");
    macroManager->addMacroSupportToObject(m_constructionReloadFileAction,
                                          ("Reload file in " + descriptivePrefix));
    
    menu->addSeparator();
    
    m_copyPathAndFileNameToClipboardAction = menu->addAction("Copy Path and File Name to Clipboard",
                                                             this,
                                                             SLOT(menuCopyFileNameToClipBoard()));
    m_copyPathAndFileNameToClipboardAction->setObjectName(menuActionNamePrefix
                                                          + "CopyPathAndFileNameToClipboard");
    m_copyPathAndFileNameToClipboardAction->setToolTip("Copy path and file name of file in this overlay to clipboard");
    macroManager->addMacroSupportToObject(m_copyPathAndFileNameToClipboardAction,
                                          ("Copy path and filename from " + descriptivePrefix + " to clipboard"));
    
    QAction* copySliceNameAction = menu->addAction("Copy Slice Number to Clipboard",
                                                 this,
                                                 SLOT(menuCopySliceNameToClipBoard()));
    copySliceNameAction->setObjectName(menuActionNamePrefix
                                     + "CopySliceNameToClipboard");
    copySliceNameAction->setToolTip("Copy number of selected slice to the clipboard");
    macroManager->addMacroSupportToObject(copySliceNameAction,
                                          ("Copy slice number in " + descriptivePrefix + " to clipboard"));
    
    return menu;
    
}

/**
 * Called when construction menu is about to be displayed.
 */
void
HistologyOverlayViewController::menuConstructionAboutToShow()
{
    if (m_histologyOverlay != NULL) {
        const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
    
        QString menuText = "Reload Selected File";
        if (selectionData.m_selectedFile != NULL) {
            if (selectionData.m_selectedFile->isModified()) {
                QString suffix = " (MODIFIED)";
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
HistologyOverlayViewController::menuAddOverlayAboveTriggered()
{
    emit requestAddOverlayAbove(m_overlayIndex);
}

/**
 * Add an overlay below this overlay.
 */
void 
HistologyOverlayViewController::menuAddOverlayBelowTriggered()
{
    emit requestAddOverlayBelow(m_overlayIndex);
}

/**
 * Remove this overlay.
 */
void 
HistologyOverlayViewController::menuRemoveOverlayTriggered()
{
    emit requestRemoveOverlay(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
HistologyOverlayViewController::menuMoveOverlayDownTriggered()
{
    emit requestMoveOverlayDown(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
HistologyOverlayViewController::menuMoveOverlayUpTriggered()
{
    emit requestMoveOverlayUp(m_overlayIndex);
}

/**
 * Copy the file name to the clip board.
 */
void
HistologyOverlayViewController::menuCopyFileNameToClipBoard()
{
    if (m_histologyOverlay != NULL) {
        const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
        
        if (selectionData.m_selectedFile != NULL) {
            QApplication::clipboard()->setText(selectionData.m_selectedFile->getFileName().trimmed(),
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Copy the slice number to the clip board.
 */
void
HistologyOverlayViewController::menuCopySliceNameToClipBoard()
{
    if (m_histologyOverlay != NULL) {
        const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
        if (selectionData.m_selectedFile != NULL) {
            QApplication::clipboard()->setText(selectionData.m_selectedSliceName,
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Reload the file in the overlay.
 */
void HistologyOverlayViewController::menuReloadFileTriggered()
{
    if (m_histologyOverlay != NULL) {
        const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
        
        if (selectionData.m_selectedFile != NULL) {
            AString username;
            AString password;
            
            if (DataFile::isFileOnNetwork(selectionData.m_selectedFile->getFileName())) {
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
                                            selectionData.m_selectedFile);
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
 * Reset the view since displayed data has changed
 */
void
HistologyOverlayViewController::resetUserView()
{
    if (m_histologyOverlay != NULL) {
        const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());
        EventBrowserTabGet tabEvent(selectionData.m_tabIndex);
        EventManager::get()->sendEvent(tabEvent.getPointer());
        BrowserTabContent* btc(tabEvent.getBrowserTab());
        if (btc != NULL) {
            EventGraphicsPaintNowOneWindow graphicsEvent(m_browserWindowIndex);
            EventManager::get()->sendEvent(graphicsEvent.getPointer());

            btc->resetView();
            updateGraphicsWindow();
        }
    }
}

