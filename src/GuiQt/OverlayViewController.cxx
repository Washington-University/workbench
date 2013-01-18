
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapSettingsEditorDialogRequest.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "Overlay.h"
#include "WuQtUtilities.h"
#include "WuQGridLayoutGroup.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::OverlayViewController 
 * \brief View Controller for an overlay.
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
    int maxComboBoxWidth = 400;
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
    this->fileComboBox = new QComboBox();
    this->fileComboBox->setMinimumWidth(minComboBoxWidth);
    this->fileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    this->fileComboBox->setToolTip("Selects file for this overlay");
    
    /*
     * Map Selection Check Box
     */
    this->mapComboBox = new QComboBox();
    this->mapComboBox->setMinimumWidth(minComboBoxWidth);
    this->mapComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->mapComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapComboBoxSelected(int)));
    this->mapComboBox->setToolTip("Selects map within the selected file");
    
    /*
     * Opacity double spin box
     */
    this->opacityDoubleSpinBox = new QDoubleSpinBox();
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
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/overlay_colorbar.png",
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
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/overlay_wrench.png",
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
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/overlay_construction.png",
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
        this->gridLayoutGroup->addWidget(this->mapComboBox,
                                         row, 6);
        
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
                                         row, 0,
                                         2, 1);
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(constructionToolButton,
                                         row, 3);
        this->gridLayoutGroup->addWidget(fileLabel,
                              row, 4);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                              row, 5);
        
        row++;
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 1,
                                         1, 3,
                                         Qt::AlignCenter);
        this->gridLayoutGroup->addWidget(mapLabel,
                              row, 4);
        this->gridLayoutGroup->addWidget(this->mapComboBox,
                              row, 5);
        
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
                                         row, 0, 1, -1);
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
    
    this->updateViewController(this->overlay);
    
    this->updateUserInterfaceAndGraphicsWindow();    
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::mapComboBoxSelected(int indx)
{
    if (overlay == NULL) {
        return;
    }
    
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, indx);
    
    this->updateUserInterfaceAndGraphicsWindow();
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
    
    this->updateUserInterfaceAndGraphicsWindow();
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
    
    this->updateUserInterfaceAndGraphicsWindow();
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
    
    this->updateUserInterfaceAndGraphicsWindow();
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
        EventMapSettingsEditorDialogRequest pcme(this->browserWindowIndex,
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
    this->mapComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    AString selectedMapUniqueID = "";
    int32_t selectedMapIndex = -1;
    if (this->overlay != NULL) {
        this->overlay->getSelectionData(dataFiles, 
                                  selectedFile, 
                                  selectedMapUniqueID, 
                                  selectedMapIndex);
    }
    
    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(dataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        CaretMappableDataFile* dataFile = dataFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toOverlayTypeName(dataFile->getDataFileType());
//        switch (dataFile->getDataFileType()) {
//            case DataFileTypeEnum::CONNECTIVITY_DENSE:
//                dataTypeName = "CONNECTIVITY";
//                break;
//            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
//                dataTypeName = "DATA_SERIES";
//                break;
//            default:
//                break;
//        }
        AString name = dataTypeName
        + " "
        + dataFile->getFileNameNoPath();
        this->fileComboBox->addItem(name,
                                    qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the column selection combo box.
     */
    if (selectedFile != NULL) {
        const int32_t numMaps = selectedFile->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            this->mapComboBox->addItem(selectedFile->getMapName(i));
        }
        this->mapComboBox->setCurrentIndex(selectedMapIndex);
    }
    
    Qt::CheckState checkState = Qt::Unchecked;
    if (this->overlay != NULL) {
        if (this->overlay->isEnabled()) {
            checkState = Qt::Checked;
        }
    }
    
    this->enabledCheckBox->setCheckState(checkState);
    
    this->colorBarAction->blockSignals(true);
    this->colorBarAction->setChecked(overlay->isPaletteDisplayEnabled());
    this->colorBarAction->blockSignals(false);
    
    this->opacityDoubleSpinBox->blockSignals(true);
    this->opacityDoubleSpinBox->setValue(overlay->getOpacity());
    this->opacityDoubleSpinBox->blockSignals(false);
//    this->widgetsGroup->blockAllSignals(false);
//    this->widgetsGroup->setEnabled(this->overlay != NULL);

    const bool haveFile = (selectedFile != NULL);
    bool dataIsMappedWithPalette = false;
    if (haveFile) {
        dataIsMappedWithPalette = selectedFile->isMappedWithPalette();
    }
    
    /*
     * Make sure items are enabled at the appropriate time
     */
    this->fileComboBox->setEnabled(haveFile);
    this->mapComboBox->setEnabled(haveFile);
    this->enabledCheckBox->setEnabled(haveFile);
    this->constructionAction->setEnabled(haveFile);
    this->opacityDoubleSpinBox->setEnabled(haveFile);
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
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
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
