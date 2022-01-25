
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

#define __MEDIA_OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "MediaOverlayViewController.h"
#undef __MEDIA_OVERLAY_VIEW_CONTROLLER_DECLARE__

#include "BrowserTabContent.h"
#include "CaretMappableDataFile.h"
#include "EventBrowserTabGet.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "MapYokingGroupComboBox.h"
#include "MediaFile.h"
#include "MediaOverlay.h"
#include "MediaOverlaySettingsMenu.h"
#include "UsernamePasswordWidget.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQGridLayoutGroup.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;
    
/**
 * \class caret::MediaOverlayViewController
 * \brief View Controller for a media overlay.
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
MediaOverlayViewController::MediaOverlayViewController(const Qt::Orientation orientation,
                                             QGridLayout* gridLayout,
                                             const int32_t browserWindowIndex,
                                             const int32_t overlayIndex,
                                             const QString& parentObjectName,
                                             QObject* parent)
: QObject(parent),
  browserWindowIndex(browserWindowIndex),
  m_overlayIndex(overlayIndex),
  m_parentObjectName(parentObjectName)
{
    m_mediaOverlay = NULL;
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
                                       + ":MediaOverlay%1"
                                       + ":").arg((int)(overlayIndex + 1), 2, 10, QLatin1Char('0'));
    const QString descriptivePrefix = QString("overlay %1").arg(overlayIndex + 1);
    /*
     * Enabled Check Box
     */
    const QString checkboxText = ((orientation == Qt::Horizontal) ? " " : " ");
    this->enabledCheckBox = new QCheckBox(checkboxText);
    this->enabledCheckBox->setObjectName(objectNamePrefix
                                         + "OnOff");
    QObject::connect(this->enabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(enabledCheckBoxClicked(bool)));
    this->enabledCheckBox->setToolTip("Enables display of this overlay");
    macroManager->addMacroSupportToObject(this->enabledCheckBox,
                                          "Enable " + descriptivePrefix);
    
    /*
     * File Selection Combo Box
     */
    this->fileComboBox = WuQFactory::newComboBox();
    this->fileComboBox->setObjectName(objectNamePrefix
                                      + "FileSelection");
    this->fileComboBox->setMinimumWidth(minComboBoxWidth);
    this->fileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    this->fileComboBox->setToolTip("Selects file for this overlay");
    this->fileComboBox->setSizeAdjustPolicy(comboSizePolicy);
    macroManager->addMacroSupportToObject(this->fileComboBox,
                                          ("Select file in " + descriptivePrefix));
    
    /*
     * All scenes CZI checkbox
     */
    m_cziAllScenesCheckBox = new QCheckBox("");
    QObject::connect(m_cziAllScenesCheckBox, &QCheckBox::clicked,
                     this, &MediaOverlayViewController::cziAllScenesCheckBoxClicked);
    
    /*
     * Frame Index Spin Box
     */
    m_frameIndexSpinBox = WuQFactory::newSpinBox();
    m_frameIndexSpinBox->setMinimumWidth(50);
    this->m_frameIndexSpinBox->setObjectName(objectNamePrefix
                                         + "FrameIndex");
    QObject::connect(m_frameIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(frameIndexSpinBoxValueChanged(int)));
    m_frameIndexSpinBox->setToolTip("Select frame by its index");
    macroManager->addMacroSupportToObject(m_frameIndexSpinBox,
                                          ("Select " + descriptivePrefix + " frame index"));
    
    /*
     * Frame Name Combo Box
     */
    this->frameNameComboBox = WuQFactory::newComboBox();
    this->frameNameComboBox->setObjectName(objectNamePrefix
                                      + "FrameSelection");
    this->frameNameComboBox->setMinimumWidth(minComboBoxWidth);
    this->frameNameComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->frameNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(frameNameComboBoxSelected(int)));
    this->frameNameComboBox->setToolTip("Select frame by its name");
    this->frameNameComboBox->setSizeAdjustPolicy(comboSizePolicy);
    macroManager->addMacroSupportToObject(this->frameNameComboBox,
                                          ("Select " + descriptivePrefix + " frame name"));
    
    /*
     * Opacity double spin box
     */
    this->opacityDoubleSpinBox = WuQFactory::newDoubleSpinBox();
    this->opacityDoubleSpinBox->setObjectName(objectNamePrefix
                                              + "Opacity");
    this->opacityDoubleSpinBox->setMinimum(0.0);
    this->opacityDoubleSpinBox->setMaximum(1.0);
    this->opacityDoubleSpinBox->setSingleStep(0.05);
    this->opacityDoubleSpinBox->setDecimals(2);
    this->opacityDoubleSpinBox->setFixedWidth(50);
    QObject::connect(this->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(opacityDoubleSpinBoxValueChanged(double)));
    this->opacityDoubleSpinBox->setToolTip("Opacity (0.0=transparent, 1.0=opaque)");
    macroManager->addMacroSupportToObject(this->opacityDoubleSpinBox,
                                          ("Set " + descriptivePrefix + " opacity"));
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);

    this->settingsAction = WuQtUtilities::createAction("S",
                                                          "Edit settings for this frame and overlay",
                                                          this, 
                                                          this, 
                                                          SLOT(settingsActionTriggered()));
    this->settingsAction->setObjectName(objectNamePrefix
                                        + "ShowSettingsDialog");
    if (settingsIconValid) {
        this->settingsAction->setIcon(settingsIcon);
    }
    m_settingsToolButton = new QToolButton();
    m_settingsToolButton->setDefaultAction(this->settingsAction);
    macroManager->addMacroSupportToObject(this->settingsAction,
                                          ("Display " + descriptivePrefix + " palette settings"));
    
    /*
     * Construction Tool Button
     * Note: macro support is on each action in menu in 'createConstructionMenu'
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                           constructionIcon);
    this->constructionAction = WuQtUtilities::createAction("M", 
                                                           "Add/Move/Remove Overlays", 
                                                           this);
    if (constructionIconValid) {
        this->constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton,
                                                     descriptivePrefix,
                                                     (objectNamePrefix
                                                      + "ConstructionMenu:"));
    this->constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(this->constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    
    const AString yokeToolTip =
    ("Select a yoking group.\n"
     "\n"
     "When files with more than one frame are yoked,\n"
     "the seleted frames are synchronized by frame index.\n"
     "\n"
     "If the SAME FILE is in yoked in multiple overlays,\n"
     "the overlay enabled statuses are synchronized.\n");
    
    /*
     * Yoking Group
     * Note: macro support is in the class MapYokingGroupComboBox
     */
    m_frameYokingGroupComboBox = new MapYokingGroupComboBox(this,
                                                          (objectNamePrefix
                                                           + "FrameYokingSelection"),
                                                          descriptivePrefix);
    m_frameYokingGroupComboBox->getWidget()->setStatusTip("Synchronize enabled status and frame indices)");
    m_frameYokingGroupComboBox->getWidget()->setToolTip("Yoke to Files");
    QObject::connect(m_frameYokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokingGroupActivated()));

    /*
     * Use layout group so that items can be shown/hidden
     */
    this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout, this);
    
    if (orientation == Qt::Horizontal) {
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(m_settingsToolButton,
                                         row, 1,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(m_constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 5);
        this->gridLayoutGroup->addWidget(this->m_frameYokingGroupComboBox->getWidget(),
                                         row, 6,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(this->m_cziAllScenesCheckBox,
                                         row, 7);
        this->gridLayoutGroup->addWidget(m_frameIndexSpinBox,
                                         row, 8);
        this->gridLayoutGroup->addWidget(this->frameNameComboBox,
                                         row, 9);
        
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        QLabel* fileLabel = new QLabel("File");
        QLabel* frameLabel = new QLabel("Frame");
        
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0);
        this->gridLayoutGroup->addWidget(m_settingsToolButton,
                                         row, 1);
        this->gridLayoutGroup->addWidget(m_constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(fileLabel,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 5, 1, 3);
        
        row++;
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 0,
                                         1, 2,
                                         Qt::AlignCenter);
        this->gridLayoutGroup->addWidget(this->m_frameYokingGroupComboBox->getWidget(),
                                         row, 2,
                                         1, 2);
        this->gridLayoutGroup->addWidget(frameLabel,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->m_cziAllScenesCheckBox,
                                         row, 5);
        this->gridLayoutGroup->addWidget(m_frameIndexSpinBox,
                                         row, 6);
        this->gridLayoutGroup->addWidget(this->frameNameComboBox,
                                         row, 7);
        
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
                                         row, 0, 1, -1);
    }
}

/**
 * Destructor.
 */
MediaOverlayViewController::~MediaOverlayViewController()
{
    
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
MediaOverlayViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
}

/*
 * If this overlay ins an overlay settings editor, update its content
 */
void
MediaOverlayViewController::updateOverlaySettingsEditor()
{
    if (m_mediaOverlay == NULL) {
        return;
    }

    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
    
    if ((selectionData.m_selectedMediaFile != NULL)
        && (selectionData.m_selectedFrameIndex >= 0)) {
//        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_OVERLAY_MAP_CHANGED,
//                                                     this->browserWindowIndex,
//                                                     m_mediaOverlay,
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
MediaOverlayViewController::fileComboBoxSelected(int indx)
{
    if (m_mediaOverlay == NULL) {
        return;
    }
    
    void* pointer = this->fileComboBox->itemData(indx).value<void*>();
    MediaFile* file = (MediaFile*)pointer;
    m_mediaOverlay->setSelectionData(file, 0);
    
    validateYokingSelection();
    
    //validateYokingSelection(overlay->getYokingGroup());
    // not needed with call to validateYokingSelection: this->updateViewController(this->overlay);
    
    // called inside validateYokingSelection();  this->updateUserInterfaceAndGraphicsWindow();

    updateOverlaySettingsEditor();
    updateViewController(m_mediaOverlay);
    
    updateGraphicsWindow();
}

/**
 * Called when a selection is made from the frame index spin box.
 * @parm indx
 *    Index of selection.
 */
void
MediaOverlayViewController::frameIndexSpinBoxValueChanged(int indx)
{
    if (m_mediaOverlay == NULL)
    {
        //TSC: not sure how to put the displayed integer back to 0 where it starts when opening without data files
        return;
    }
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    MediaFile* file = (MediaFile*)pointer;
    
    /*
     * Overlay indices range [0, N-1] but spin box shows [1, N].
     */
    const int overlayIndex = indx - 1;
    
    m_mediaOverlay->setSelectionData(file, overlayIndex);
    
    const MapYokingGroupEnum::Enum frameYoking = m_mediaOverlay->getMapYokingGroup();
    if (frameYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        EventMapYokingSelectMap selectMapEvent(frameYoking,
//                                               file,
//                                               NULL,
//                                               overlayIndex,
//                                               m_mediaOverlay->isEnabled());
//        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update frame name combo box.
     */
    frameNameComboBox->blockSignals(true);
    if ((overlayIndex >= 0)
        && (overlayIndex < frameNameComboBox->count())) {
        frameNameComboBox->setCurrentIndex(overlayIndex);
    }
    frameNameComboBox->blockSignals(false);
    
    this->updateUserInterface();
    this->updateGraphicsWindow();
    
    updateOverlaySettingsEditor();
}

/**
 * Called when a selection is made from the frame name combo box.
 * @parm indx
 *    Index of selection.
 */
void 
MediaOverlayViewController::frameNameComboBoxSelected(int indx)
{
    if (m_mediaOverlay == NULL) {
        return;
    }
    
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    MediaFile* file = (MediaFile*)pointer;
    
    m_mediaOverlay->setSelectionData(file, indx);
    
    const MapYokingGroupEnum::Enum frameYoking = m_mediaOverlay->getMapYokingGroup();
    if (frameYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        EventMapYokingSelectMap selectMapEvent(frameYoking,
//                                               file,
//                                               NULL,
//                                               indx,
//                                               m_mediaOverlay->isEnabled());
//        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    /*
     * Need to update frame index spin box.
     * Note that the frame index spin box ranges [1, N].
     */
    m_frameIndexSpinBox->blockSignals(true);
    m_frameIndexSpinBox->setValue(indx + 1);
    m_frameIndexSpinBox->blockSignals(false);
    
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
MediaOverlayViewController::enabledCheckBoxClicked(bool checked)
{
    if (m_mediaOverlay == NULL) {
        return;
    }
    m_mediaOverlay->setEnabled(checked);
    
    const MapYokingGroupEnum::Enum frameYoking = m_mediaOverlay->getMapYokingGroup();
    if (frameYoking != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());

//        EventMapYokingSelectMap selectMapEvent(frameYoking,
//                                               myFile,
//                                               NULL,
//                                               myIndex,
//                                               m_mediaOverlay->isEnabled());
//        EventManager::get()->sendEvent(selectMapEvent.getPointer());
    }
    
    this->updateUserInterface();

    this->updateGraphicsWindow();
}


/**
 * Called when opacity value is changed.
 * @param value
 *    New value.
 */
void 
MediaOverlayViewController::opacityDoubleSpinBoxValueChanged(double value)
{
    if (m_mediaOverlay == NULL) {
        return;
    }
    
    m_mediaOverlay->setOpacity(value);
    
    this->updateGraphicsWindow();
}

/**
 * Called when CZI all scenes check box is clicked
 * @param checked
 *    New checked
 */
void
MediaOverlayViewController::cziAllScenesCheckBoxClicked(bool checked)
{
    if (m_mediaOverlay == NULL) {
        return;
    }
    
    m_mediaOverlay->setCziAllScenesSelected(checked);
    this->updateGraphicsWindow();
}


/**
 * Validate yoking when there are changes made to the overlay.
 */
void
MediaOverlayViewController::validateYokingSelection()
{
//    m_frameYokingGroupComboBox->validateYokingChange(m_mediaOverlay);
    updateViewController(m_mediaOverlay);
    updateGraphicsWindow();
}

/**
 * Called when the yoking group is changed.
 */
void
MediaOverlayViewController::yokingGroupActivated()
{
    MapYokingGroupEnum::Enum yokingGroup = m_frameYokingGroupComboBox->getMapYokingGroup();
   
    /*
     * Has yoking group changed?
     * TSC: overlay can be null when opened without loaded files
     */
    if (m_mediaOverlay != NULL && yokingGroup != m_mediaOverlay->getMapYokingGroup()) {
        validateYokingSelection();
    }
}


/**
 * Called when the settings action is selected.
 */
void 
MediaOverlayViewController::settingsActionTriggered()
{
    if (m_mediaOverlay == NULL) {
        return;
    }
    
    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
    if (selectionData.m_selectedMediaFile != NULL) {
        MediaOverlaySettingsMenu menu(m_mediaOverlay,
                                      m_parentObjectName);
        menu.exec(m_settingsToolButton->mapToGlobal(QPoint(0,0)));
//        EventOverlaySettingsEditorDialogRequest pcme(EventOverlaySettingsEditorDialogRequest::MODE_SHOW_EDITOR,
//                                                     this->browserWindowIndex,
//                                                     m_mediaOverlay,
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
MediaOverlayViewController::updateViewController(MediaOverlay* overlay)
{
    m_mediaOverlay = overlay;

    /*
     * Get the selection information for the overlay.
     */
    MediaOverlay::SelectionData selectionData;
    if (m_mediaOverlay != NULL) {
        selectionData = m_mediaOverlay->getSelectionData();
    }
    
    this->fileComboBox->clear();
    
    std::vector<CaretDataFile*> caretDataFiles(selectionData.m_mediaFiles.begin(),
                                               selectionData.m_mediaFiles.end());
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(caretDataFiles,
                                                                            displayNames);
    CaretAssert(selectionData.m_mediaFiles.size() == displayNames.size());

    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(selectionData.m_mediaFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        MediaFile* dataFile = selectionData.m_mediaFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toOverlayTypeName(dataFile->getDataFileType());
        CaretAssertVectorIndex(displayNames, i);
        this->fileComboBox->addItem(displayNames[i],
                                    QVariant::fromValue((void*)dataFile));
        if (dataFile == selectionData.m_selectedMediaFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the frame selection combo box
     */
    int32_t numberOfFrames = 0;
    this->frameNameComboBox->blockSignals(true);
    this->frameNameComboBox->clear();
    if (selectionData.m_selectedMediaFile != NULL) {
        numberOfFrames = selectionData.m_selectedMediaFile->getNumberOfFrames();
        for (int32_t i = 0; i < numberOfFrames; i++) {
            this->frameNameComboBox->addItem(selectionData.m_selectedMediaFile->getFrameName(i));
        }
        this->frameNameComboBox->setCurrentIndex(selectionData.m_selectedFrameIndex);
    }
    this->frameNameComboBox->blockSignals(false);
    
    /*
     * Load the frame index spin box that ranges [1, N].
     */
    m_frameIndexSpinBox->blockSignals(true);
    m_frameIndexSpinBox->setRange(1, numberOfFrames);
    if (selectionData.m_selectedMediaFile != NULL) {
        m_frameIndexSpinBox->setValue(selectionData.m_selectedFrameIndex + 1);
    }
    m_frameIndexSpinBox->blockSignals(false);

    /*
     * All CZI check box
     */
    m_cziAllScenesCheckBox->setChecked(selectionData.m_allFramesSelectedFlag);
    
    /*
     * Update enable check box
     */
    Qt::CheckState checkState = Qt::Unchecked;
    if (m_mediaOverlay != NULL) {
        if (m_mediaOverlay->isEnabled()) {
            checkState = Qt::Checked;
        }
    }
    this->enabledCheckBox->setCheckState(checkState);
    
    m_frameYokingGroupComboBox->setMapYokingGroup(overlay->getMapYokingGroup());
    
    this->opacityDoubleSpinBox->blockSignals(true);
    this->opacityDoubleSpinBox->setValue(m_mediaOverlay->getOpacity());
    this->opacityDoubleSpinBox->blockSignals(false);

    const bool haveFile = (selectionData.m_selectedMediaFile != NULL);
    bool haveMultipleFrames = false;
    bool haveOpacity = false;
    if (haveFile) {
        haveMultipleFrames = (selectionData.m_selectedMediaFile->getNumberOfFrames() > 1);
    }
    
    /**
     * Yoking is enabled when either:
     * (1) The file frames to both surface and volumes
     * (2) The file has multiple frames.
     */
    bool haveYoking = false;
    if (haveFile) {
        if (haveMultipleFrames) {
            haveYoking = true;
        }
    }
    
    /*
     * Update tooltips with full path to file and name of frame
     * as names may be too long to fit into combo boxes
     */
    AString fileComboBoxToolTip("Select file for this overlay");
    AString nameComboBoxToolTip("Select frame by its name");
    if (selectionData.m_selectedMediaFile != NULL) {
        FileInformation fileInfo(selectionData.m_selectedMediaFile->getFileName());
        fileComboBoxToolTip.append(":\n"
                                   + fileInfo.getFileName()
                                   + "\n"
                                   + fileInfo.getPathName()
                                   + "\n\n"
                                   + "Copy File Name/Path to Clipboard with Construction Menu");
        
        nameComboBoxToolTip.append(":\n"
                                   + this->frameNameComboBox->currentText());
    }
    this->fileComboBox->setToolTip(fileComboBoxToolTip);
    this->frameNameComboBox->setToolTip(nameComboBoxToolTip);

    /*
     * Make sure items are enabled at the appropriate time
     */
    this->fileComboBox->setEnabled(haveFile);
    this->frameNameComboBox->setEnabled(haveFile);
    this->m_frameIndexSpinBox->setEnabled(haveMultipleFrames);
    this->enabledCheckBox->setEnabled(haveFile);
    this->constructionAction->setEnabled(true);
    this->opacityDoubleSpinBox->setEnabled(haveOpacity);
    this->m_frameYokingGroupComboBox->getWidget()->setEnabled(haveYoking);
    this->settingsAction->setEnabled(true);
}

/**
 * Update graphics and GUI after selections made
 */
void 
MediaOverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    updateUserInterface();
    updateGraphicsWindow();
}

/**
 * Update graphics and GUI after selections made
 */
void
MediaOverlayViewController::updateUserInterface()
{
//    if (this->overlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//    }
//    else {
//        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
//    }
}

/**
 * Update graphics after selections made
 */
void
MediaOverlayViewController::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
//    if (this->overlay->getMapYokingGroup() != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//    }
//    else {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
//    }
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
MediaOverlayViewController::createConstructionMenu(QWidget* parent,
                                              const AString& descriptivePrefix,
                                              const AString& menuActionNamePrefix)
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
    
    QAction* copyFrameNameAction = menu->addAction("Copy Frame Name to Clipboard",
                                                 this,
                                                 SLOT(menuCopyFrameNameToClipBoard()));
    copyFrameNameAction->setObjectName(menuActionNamePrefix
                                     + "CopyFrameNameToClipboard");
    copyFrameNameAction->setToolTip("Copy name of selected frame to the clipboard");
    macroManager->addMacroSupportToObject(copyFrameNameAction,
                                          ("Copy frame namne in " + descriptivePrefix + " to clipboard"));
    
    return menu;
    
}

/**
 * Called when construction menu is about to be displayed.
 */
void
MediaOverlayViewController::menuConstructionAboutToShow()
{
    if (m_mediaOverlay != NULL) {
        const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
    
        QString menuText = "Reload Selected File";
        if (selectionData.m_selectedMediaFile != NULL) {
            if (selectionData.m_selectedMediaFile->isModified()) {
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
MediaOverlayViewController::menuAddOverlayAboveTriggered()
{
    emit requestAddOverlayAbove(m_overlayIndex);
}

/**
 * Add an overlay below this overlay.
 */
void 
MediaOverlayViewController::menuAddOverlayBelowTriggered()
{
    emit requestAddOverlayBelow(m_overlayIndex);
}

/**
 * Remove this overlay.
 */
void 
MediaOverlayViewController::menuRemoveOverlayTriggered()
{
    emit requestRemoveOverlay(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
MediaOverlayViewController::menuMoveOverlayDownTriggered()
{
    emit requestMoveOverlayDown(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
MediaOverlayViewController::menuMoveOverlayUpTriggered()
{
    emit requestMoveOverlayUp(m_overlayIndex);
}

/**
 * Copy the file name to the clip board.
 */
void
MediaOverlayViewController::menuCopyFileNameToClipBoard()
{
    if (m_mediaOverlay != NULL) {
        const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
        
        if (selectionData.m_selectedMediaFile != NULL) {
            QApplication::clipboard()->setText(selectionData.m_selectedMediaFile->getFileName().trimmed(),
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Copy the frame name to the clip board.
 */
void
MediaOverlayViewController::menuCopyFrameNameToClipBoard()
{
    if (m_mediaOverlay != NULL) {
        const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
        if (selectionData.m_selectedMediaFile != NULL) {
            QApplication::clipboard()->setText(selectionData.m_selectedFrameName,
                                               QClipboard::Clipboard);
        }
    }
}

/**
 * Reload the file in the overlay.
 */
void MediaOverlayViewController::menuReloadFileTriggered()
{
    if (m_mediaOverlay != NULL) {
        const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
        
        if (selectionData.m_selectedMediaFile != NULL) {
            AString username;
            AString password;
            
            if (DataFile::isFileOnNetwork(selectionData.m_selectedMediaFile->getFileName())) {
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
                                            selectionData.m_selectedMediaFile);
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
 * Reset the view since displayed data (frame or all frames has changed)
 */
void
MediaOverlayViewController::resetUserView()
{
    if (m_mediaOverlay != NULL) {
        const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
        EventBrowserTabGet tabEvent(selectionData.m_tabIndex);
        EventManager::get()->sendEvent(tabEvent.getPointer());
        BrowserTabContent* btc(tabEvent.getBrowserTab());
        if (btc != NULL) {
            const bool doRepaintFlag(true);
            EventGraphicsUpdateOneWindow graphicsEvent(this->browserWindowIndex,
                                                       doRepaintFlag);
            EventManager::get()->sendEvent(graphicsEvent.getPointer());

            btc->resetView();
            updateGraphicsWindow();
        }
    }
}

