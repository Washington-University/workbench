
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
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "OverlayViewController.h"
#undef __OVERLAY_VIEW_CONTROLLER_DECLARE__

#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventOverlayYokingGroupGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FilePathNamePrefixCompactor.h"
#include "Overlay.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQGridLayoutGroup.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::OverlayViewController 
 * \brief View Controller for an overlay.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window in which this view controller resides.
 * @param showTopHorizontalLine
 *    If true, display a horizontal line above the controls.
 * @param parent
 *    The parent widget.
 */
OverlayViewController::OverlayViewController(const Qt::Orientation orientation,
                                             QGridLayout* gridLayout,
                                             const int32_t browserWindowIndex,
                                             const int32_t overlayIndex,
                                             QObject* parent)
: QObject(parent),
  browserWindowIndex(browserWindowIndex),
  m_overlayIndex(overlayIndex)
{
    this->overlay = NULL;
    
    int minComboBoxWidth = 200;
    int maxComboBoxWidth = 100000; //400;
    if (orientation == Qt::Horizontal) {
        minComboBoxWidth = 50;
        maxComboBoxWidth = 100000;
    }

    /*
     * Enabled Check Box
     */
    const QString checkboxText = ((orientation == Qt::Horizontal) ? " " : " ");
    this->enabledCheckBox = new QCheckBox(checkboxText);
    QObject::connect(this->enabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(enabledCheckBoxClicked(bool)));
    this->enabledCheckBox->setToolTip("Enables display of this overlay");
    
    /*
     * File Selection Check Box
     */
    this->fileComboBox = WuQFactory::newComboBox();
    this->fileComboBox->setMinimumWidth(minComboBoxWidth);
    this->fileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    this->fileComboBox->setToolTip("Selects file for this overlay");
    
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
    this->mapNameComboBox = WuQFactory::newComboBox();
    this->mapNameComboBox->setMinimumWidth(minComboBoxWidth);
    this->mapNameComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->mapNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapNameComboBoxSelected(int)));
    this->mapNameComboBox->setToolTip("Select map by its name");
    
    /*
     * Opacity double spin box
     */
    this->opacityDoubleSpinBox = WuQFactory::newDoubleSpinBox();
    this->opacityDoubleSpinBox->setMinimum(0.0);
    this->opacityDoubleSpinBox->setMaximum(1.0);
    this->opacityDoubleSpinBox->setSingleStep(0.10);
    this->opacityDoubleSpinBox->setDecimals(1);
    this->opacityDoubleSpinBox->setFixedWidth(50);
    QObject::connect(this->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(opacityDoubleSpinBoxValueChanged(double)));
    this->opacityDoubleSpinBox->setToolTip("Opacity (0.0=transparent, 1.0=opaque)");
    
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    this->colorBarAction = WuQtUtilities::createAction("CB", 
                                                       "Display color bar for this overlay", 
                                                       this, 
                                                       this, 
                                                       SLOT(colorBarActionTriggered(bool)));
    this->colorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        this->colorBarAction->setIcon(colorBarIcon);
    }
    QToolButton* colorBarToolButton = new QToolButton();
    colorBarToolButton->setDefaultAction(this->colorBarAction);
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/LayersPanel/wrench.png",
                                                           settingsIcon);

    this->settingsAction = WuQtUtilities::createAction("S",
                                                          "Edit settings for this map and overlay", 
                                                          this, 
                                                          this, 
                                                          SLOT(settingsActionTriggered()));
    if (settingsIconValid) {
        this->settingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(this->settingsAction);
    
    /*
     * Construction Tool Button
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
    QToolButton* constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(constructionToolButton);
    this->constructionAction->setMenu(constructionMenu);
    constructionToolButton->setDefaultAction(this->constructionAction);
    constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    
    /*
     * Yoking Group
     */
    m_yokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_yokingGroupComboBox->setup<OverlayYokingGroupEnum, OverlayYokingGroupEnum::Enum>();
    m_yokingGroupComboBox->getWidget()->setStatusTip("Select a yoking group (synchronized map selections)");
    m_yokingGroupComboBox->getWidget()->setToolTip(("Select a yoking group (synchronized map selections).\n"
                                                    "Overlays yoked to a yoking group all maintain\n"
                                                    "the same selected map index."));
#ifdef CARET_OS_MACOSX
    m_yokingGroupComboBox->getComboBox()->setFixedWidth(m_yokingGroupComboBox->getComboBox()->sizeHint().width() - 20);
#endif // CARET_OS_MACOSX
    QObject::connect(m_yokingGroupComboBox, SIGNAL(itemActivated()),
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
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 5);
        this->gridLayoutGroup->addWidget(this->m_yokingGroupComboBox->getWidget(),
                                         row, 6,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(m_mapIndexSpinBox,
                                         row, 7);
        this->gridLayoutGroup->addWidget(this->mapNameComboBox,
                                         row, 8);
        
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        QLabel* fileLabel = new QLabel("File");
        QLabel* mapLabel = new QLabel("Map");
        
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0);
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(fileLabel,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 5, 1, 2);
        
        row++;
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 0,
                                         1, 2,
                                         Qt::AlignCenter);
        this->gridLayoutGroup->addWidget(this->m_yokingGroupComboBox->getWidget(),
                                         row, 2,
                                         1, 2);
        this->gridLayoutGroup->addWidget(mapLabel,
                                         row, 4);
        this->gridLayoutGroup->addWidget(m_mapIndexSpinBox,
                                         row, 5);
        this->gridLayoutGroup->addWidget(this->mapNameComboBox,
                                         row, 6);
        
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
                                         row, 0, 1, -1);
//        int row = this->gridLayoutGroup->rowCount();
//        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
//                                         row, 0,
//                                         2, 1);
//        this->gridLayoutGroup->addWidget(settingsToolButton,
//                                         row, 1);
//        this->gridLayoutGroup->addWidget(colorBarToolButton,
//                                         row, 2);
//        this->gridLayoutGroup->addWidget(constructionToolButton,
//                                         row, 3);
//        this->gridLayoutGroup->addWidget(fileLabel,
//                              row, 4);
//        this->gridLayoutGroup->addWidget(this->fileComboBox,
//                              row, 5, 1, 2);
//        
//        row++;
//        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
//                                         row, 1,
//                                         1, 2,
//                                         Qt::AlignCenter);
//        this->gridLayoutGroup->addWidget(this->m_yokingGroupComboBox->getWidget(),
//                                         row, 3);
//        this->gridLayoutGroup->addWidget(mapLabel,
//                              row, 4);
//        this->gridLayoutGroup->addWidget(m_mapIndexSpinBox,
//                                         row, 5);
//        this->gridLayoutGroup->addWidget(this->mapNameComboBox,
//                              row, 6);
//        
//        row++;
//        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
//                                         row, 0, 1, -1);
    }
    //this->setFixedHeight(this->sizeHint().height());
}

/**
 * Destructor.
 */
OverlayViewController::~OverlayViewController()
{
    
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
OverlayViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::fileComboBoxSelected(int indx)
{
    if (overlay == NULL) {
        return;
    }
    
    void* pointer = this->fileComboBox->itemData(indx).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, 0);
    
    validateYokingSelection();
    // not needed with call to validateYokingSelection: this->updateViewController(this->overlay);
    
    // called inside validateYokingSelection();  this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when a selection is made from the map index spin box.
 * @parm indx
 *    Index of selection.
 */
void
OverlayViewController::mapIndexSpinBoxValueChanged(int indx)
{
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    /*
     * Overlay indices range [0, N-1] but spin box shows [1, N].
     */
    const int overlayIndex = indx - 1;
    
    overlay->setSelectionData(file, overlayIndex);
    
    /*
     * Need to update map name combo box.
     */
    mapNameComboBox->blockSignals(true);
    if ((overlayIndex >= 0)
        && (overlayIndex < mapNameComboBox->count())) {        
        mapNameComboBox->setCurrentIndex(overlayIndex);
    }
    mapNameComboBox->blockSignals(false);
    
    this->updateUserInterfaceIfYoked();
    this->updateGraphicsWindow();
}

/**
 * Called when a selection is made from the map name combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::mapNameComboBoxSelected(int indx)
{
    if (overlay == NULL) {
        return;
    }
    
    /*
     * Get the file that is selected from the file combo box
     */
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    
    overlay->setSelectionData(file, indx);
    
    /*
     * Need to update map index spin box.
     * Note that the map index spin box ranges [1, N].
     */
    m_mapIndexSpinBox->blockSignals(true);
    m_mapIndexSpinBox->setValue(indx + 1);
    m_mapIndexSpinBox->blockSignals(false);
    
    this->updateUserInterfaceIfYoked();
    this->updateGraphicsWindow();
}

/**
 * Called when enabled checkbox state is changed
 * @parm checked
 *    Checked status
 */
void 
OverlayViewController::enabledCheckBoxClicked(bool checked)
{
    if (overlay == NULL) {
        return;
    }
    overlay->setEnabled(checked);
    
    this->updateGraphicsWindow();
}

/**
 * Called when colorbar toolbutton is toggled.
 * @param status
 *    New status.
 */
void 
OverlayViewController::colorBarActionTriggered(bool status)
{
    if (overlay == NULL) {
        return;
    }
    
    this->overlay->setPaletteDisplayEnabled(status);
    
    this->updateGraphicsWindow();
}

/**
 * Called when opacity value is changed.
 * @param value
 *    New value.
 */
void 
OverlayViewController::opacityDoubleSpinBoxValueChanged(double value)
{
    if (overlay == NULL) {
        return;
    }
    
    this->overlay->setOpacity(value);
    
    this->updateGraphicsWindow();
}

/**
 * Validate and possibly change the yoking group selection.
 */
void
OverlayViewController::validateYokingSelection()
{
    OverlayYokingGroupEnum::Enum yokingGroup = m_yokingGroupComboBox->getSelectedItem<OverlayYokingGroupEnum, OverlayYokingGroupEnum::Enum>();
    if (yokingGroup != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        CaretMappableDataFile* selectedFile = NULL;
        int32_t selectedMapIndex;
        overlay->getSelectionData(selectedFile,
                                  selectedMapIndex);
        if ((selectedFile != NULL)
            && (selectedMapIndex >= 0)) {
            /*
             * Get info on overlay yoked to the selected yoking group
             */
            EventOverlayYokingGroupGet yokedOverlaysEvent(yokingGroup);
            EventManager::get()->sendEvent(yokedOverlaysEvent.getPointer());
            const int32_t numOverlaysYoked = yokedOverlaysEvent.getNumberOfYokedOverlays();
            
            /*
             * Check compatibility based (number of maps in yoked overlays match)
             * and warn use if there is an incompatibility.
             */
            AString message;
            if (yokedOverlaysEvent.validateCompatibility(selectedFile,
                                                         message) == false) {
                message.appendWithNewLine("");
                message.appendWithNewLine("Allow yoking?");
                
                message = WuQtUtilities::createWordWrappedToolTipText(message);
                if (WuQMessageBox::warningYesNo(m_yokingGroupComboBox->getWidget(),
                                                message) == false) {
                    yokingGroup = OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF;
                }
            }
            
            overlay->setYokingGroup(yokingGroup);
            if (yokingGroup != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
                if (numOverlaysYoked <= 0) {
                    OverlayYokingGroupEnum::setSelectedMapIndex(yokingGroup,
                                                                selectedMapIndex);
                }
            }
        }
    }
    else {
        overlay->setYokingGroup(yokingGroup);
    }
    
    updateViewController(overlay);
    
    this->updateUserInterfaceAndGraphicsWindow();
}


/**
 * Called when the yoking group is changed.
 */
void
OverlayViewController::yokingGroupActivated()
{
    OverlayYokingGroupEnum::Enum yokingGroup = m_yokingGroupComboBox->getSelectedItem<OverlayYokingGroupEnum, OverlayYokingGroupEnum::Enum>();
   
    /*
     * Has yoking group changed?
     */
    if (yokingGroup != overlay->getYokingGroup()) {
        validateYokingSelection();
    }
}


/**
 * Called when the settings action is selected.
 */
void 
OverlayViewController::settingsActionTriggered()
{
    if (overlay == NULL) {
        return;
    }
    
    CaretMappableDataFile* mapFile;
    int32_t mapIndex = -1;
    this->overlay->getSelectionData(mapFile, 
                                    mapIndex);
    if (mapFile != NULL) {
        EventOverlaySettingsEditorDialogRequest pcme(this->browserWindowIndex,
                                                 this->overlay,
                                                 mapFile,
                                                 mapIndex);
        EventManager::get()->sendEvent(pcme.getPointer());
    }
}

/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void 
OverlayViewController::updateViewController(Overlay* overlay)
{
    this->overlay = overlay;

//    this->widgetsGroup->blockAllSignals(true);
    
    this->fileComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    //AString selectedMapUniqueID = "";
    int32_t selectedMapIndex = -1;
    if (this->overlay != NULL) {
        this->overlay->getSelectionData(dataFiles, 
                                  selectedFile, 
                                  //selectedMapUniqueID,
                                  selectedMapIndex);
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
        this->fileComboBox->addItem(displayNames[i],
                                    qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the map selection combo box
     */
    int32_t numberOfMaps = 0;
    this->mapNameComboBox->blockSignals(true);
    this->mapNameComboBox->clear();
    if (selectedFile != NULL) {
        numberOfMaps = selectedFile->getNumberOfMaps();
        for (int32_t i = 0; i < numberOfMaps; i++) {
            this->mapNameComboBox->addItem(selectedFile->getMapName(i));
        }
        this->mapNameComboBox->setCurrentIndex(selectedMapIndex);
    }
    this->mapNameComboBox->blockSignals(false);
    
    /*
     * Load the map index spin box that ranges [1, N].
     */
    m_mapIndexSpinBox->blockSignals(true);
    m_mapIndexSpinBox->setRange(1, numberOfMaps);
    if (selectedFile != NULL) {
        m_mapIndexSpinBox->setValue(selectedMapIndex + 1);
    }
    m_mapIndexSpinBox->blockSignals(false);

    /*
     * Update enable check box
     */
    Qt::CheckState checkState = Qt::Unchecked;
    if (this->overlay != NULL) {
        if (this->overlay->isEnabled()) {
            checkState = Qt::Checked;
        }
    }
    this->enabledCheckBox->setCheckState(checkState);
    
    m_yokingGroupComboBox->blockSignals(true);
    m_yokingGroupComboBox->setSelectedItem<OverlayYokingGroupEnum,OverlayYokingGroupEnum::Enum>(this->overlay->getYokingGroup());
    m_yokingGroupComboBox->blockSignals(false);
    
    this->colorBarAction->blockSignals(true);
    this->colorBarAction->setChecked(overlay->isPaletteDisplayEnabled());
    this->colorBarAction->blockSignals(false);
    
    this->opacityDoubleSpinBox->blockSignals(true);
    this->opacityDoubleSpinBox->setValue(overlay->getOpacity());
    this->opacityDoubleSpinBox->blockSignals(false);
//    this->widgetsGroup->blockAllSignals(false);
//    this->widgetsGroup->setEnabled(this->overlay != NULL);

    const bool haveFile = (selectedFile != NULL);
    bool haveMultipleMaps = false;
    bool dataIsMappedWithPalette = false;
    bool dataIsMappedWithLabelTable = false;
    bool haveOpacity = false;
    if (haveFile) {
        dataIsMappedWithPalette = selectedFile->isMappedWithPalette();
        dataIsMappedWithLabelTable = selectedFile->isMappedWithLabelTable();
        haveMultipleMaps = (selectedFile->getNumberOfMaps() > 1);
        haveOpacity = (dataIsMappedWithLabelTable
                       || dataIsMappedWithPalette);
    }
    
    /*
     * Make sure items are enabled at the appropriate time
     */
    this->fileComboBox->setEnabled(haveFile);
    this->mapNameComboBox->setEnabled(haveFile);
    this->m_mapIndexSpinBox->setEnabled(haveMultipleMaps);
    this->enabledCheckBox->setEnabled(haveFile);
    this->constructionAction->setEnabled(true);
    this->opacityDoubleSpinBox->setEnabled(haveOpacity);
    this->m_yokingGroupComboBox->getWidget()->setEnabled(haveMultipleMaps);
    this->colorBarAction->setEnabled(dataIsMappedWithPalette);
    this->settingsAction->setEnabled(true);
}

/**
 * Update graphics and GUI after selections made
 */
void 
OverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    if (this->overlay->getYokingGroup() != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    }
    
    updateGraphicsWindow();
}

/**
 * Update graphics and GUI after selections made
 */
void
OverlayViewController::updateUserInterfaceIfYoked()
{
    if (this->overlay->getYokingGroup() != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

/**
 * Update graphics after selections made
 */
void
OverlayViewController::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    if (this->overlay->getYokingGroup() != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
    }
}

/**
 * Create the construction menu.
 * @param parent
 *    Parent widget.
 */
QMenu* 
OverlayViewController::createConstructionMenu(QWidget* parent)
{
    QMenu* menu = new QMenu(parent);
    
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
    
    return menu;
    
}

/**
 * Add an overlay above this overlay.
 */
void 
OverlayViewController::menuAddOverlayAboveTriggered()
{
    emit requestAddOverlayAbove(m_overlayIndex);
}

/**
 * Add an overlay below this overlay.
 */
void 
OverlayViewController::menuAddOverlayBelowTriggered()
{
    emit requestAddOverlayBelow(m_overlayIndex);
}

/**
 * Remove this overlay.
 */
void 
OverlayViewController::menuRemoveOverlayTriggered()
{
    emit requestRemoveOverlay(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
OverlayViewController::menuMoveOverlayDownTriggered()
{
    emit requestMoveOverlayDown(m_overlayIndex);
}

/**
 * Move this overlay down.
 */
void 
OverlayViewController::menuMoveOverlayUpTriggered()
{
    emit requestMoveOverlayUp(m_overlayIndex);
}
