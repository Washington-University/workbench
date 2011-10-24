
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __OVERLAY_SELECTION_CONTROL_LAYER_DECLARE__
#include "OverlaySelectionControlLayer.h"
#undef __OVERLAY_SELECTION_CONTROL_LAYER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QToolButton>

#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GiftiTypeFile.h"
#include "GuiManager.h"
#include "SurfaceOverlay.h"
#include "SurfaceOverlaySet.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


/**
 * Contructs a single layer.
 * @param browserWindowIndex
 *    Index of browser window containing this
 *    Layer.
 * @param overlaySelectionControl
 *    Parent that contains this layer.
 * @param dataType
 *    Type of data for overlay selection.
 * @param layerIndex
 *    Index of this layer.
 */
OverlaySelectionControlLayer::OverlaySelectionControlLayer(const int32_t browserWindowIndex,
                                      OverlaySelectionControl* overlaySelectionControl,
                                      OverlaySelectionControl::DataType dataType,
                                      const int32_t layerIndex)
{
    this->overlaySelectionControl = overlaySelectionControl;
    this->browserWindowIndex = browserWindowIndex;
    this->layerIndex = layerIndex;
    this->dataType   = dataType;
    
    this->enabledCheckBox = new QCheckBox("");
    this->enabledCheckBox->setVisible(true);
    QObject::connect(this->enabledCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(enableCheckBoxToggled(bool)));
    
    //const int comboBoxWidth = 200;
    
    this->fileSelectionComboBox = new QComboBox();
    //this->fileSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    QObject::connect(this->fileSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(fileSelected(int)));
    //this->fileSelectionComboBox->setFixedWidth(comboBoxWidth);
    this->columnSelectionComboBox = new QComboBox();
    //this->columnSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(this->columnSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(columnSelected(int)));
    //this->columnSelectionComboBox->setFixedWidth(comboBoxWidth);
    
    this->histogramAction = WuQtUtilities::createAction("H",
                                                        "Show histogram of selected data",
                                                        this,
                                                        this,
                                                        SLOT(histogramToolButtonPressed()));
    this->histogramToolButton = new QToolButton();
    this->histogramToolButton->setDefaultAction(this->histogramAction);
    
    this->settingsAction = WuQtUtilities::createAction("S",
                                                        "Show control for adjusting settings that controls data coloring",
                                                        this,
                                                        this,
                                                        SLOT(settingsToolButtonPressed()));
    this->settingsToolButton  = new QToolButton();
    this->settingsToolButton->setDefaultAction(this->settingsAction);
    
    this->metadataAction = WuQtUtilities::createAction("M",
                                                        "Show metadata for the selected data",
                                                        this,
                                                        this,
                                                        SLOT(metadataToolButtonPressed()));
    this->metadataToolButton  = new QToolButton();
    this->metadataToolButton->setDefaultAction(this->metadataAction);
    
    this->opacityDoubleSpinBox = new QDoubleSpinBox();
    this->opacityDoubleSpinBox->setMinimum(0.0);
    this->opacityDoubleSpinBox->setMaximum(1.0);
    this->opacityDoubleSpinBox->setValue(1.0);
    this->opacityDoubleSpinBox->setSingleStep(0.1);
    this->opacityDoubleSpinBox->setToolTip("Adjust opacity, 0=>transparent, 1=opaque");
    this->opacityDoubleSpinBox->setFixedWidth(60);
    QObject::connect(this->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(opacityValueChanged(double)));
    
    this->removeAction = WuQtUtilities::createAction("X",
                                                        "Remove this layer",
                                                        this,
                                                        this,
                                                        SLOT(removeLayerToolButtonPressed()));
    this->deleteToolButton = new QToolButton();
    this->deleteToolButton->setDefaultAction(this->removeAction);
    
    this->moveUpAction = WuQtUtilities::createAction("",
                                                        "Move this layer up",
                                                        this,
                                                        this,
                                                        SLOT(moveLayerUpToolButtonPressed()));
    this->upArrowToolButton = new QToolButton();
    this->upArrowToolButton->setDefaultAction(this->moveUpAction);
    this->upArrowToolButton->setArrowType(Qt::UpArrow);
    
    this->moveDownAction = WuQtUtilities::createAction("",
                                                        "Move this layer down",
                                                        this,
                                                        this,
                                                        SLOT(moveLayerDownToolButtonPressed()));
    this->downArrowToolButton = new QToolButton();
    this->downArrowToolButton->setDefaultAction(this->moveDownAction);
    this->downArrowToolButton->setArrowType(Qt::DownArrow);
    
    this->widgetGroup = new WuQWidgetObjectGroup(overlaySelectionControl);
    this->widgetGroup->add(this->enabledCheckBox);
    this->widgetGroup->add(this->fileSelectionComboBox);
    this->widgetGroup->add(this->columnSelectionComboBox);
    this->widgetGroup->add(this->histogramToolButton);
    this->widgetGroup->add(this->settingsToolButton);
    this->widgetGroup->add(this->metadataToolButton);
    this->widgetGroup->add(this->opacityDoubleSpinBox);            
    
    this->widgetGroup->add(this->deleteToolButton);
    this->widgetGroup->add(this->upArrowToolButton);
    this->widgetGroup->add(this->downArrowToolButton);
}

/**
 * Destroys this layer.
 */
OverlaySelectionControlLayer::~OverlaySelectionControlLayer()
{
    
}

/**
 * Called when enable checkbox is toggled.
 * @param toggled
 *   New enabled status.
 */
void 
OverlaySelectionControlLayer::enableCheckBoxToggled(bool toggled)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            browserTabContent->invalidateSurfaceColoring();
            
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            SurfaceOverlay* overlay = surfaceOverlaySet->getOverlay(this->layerIndex);
            overlay->setEnabled(toggled);
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }

    this->updateControl(browserTabContent);
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Move this layer up one level.
 */ 
void 
OverlaySelectionControlLayer::moveLayerUpToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            browserTabContent->invalidateSurfaceColoring();
            
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            surfaceOverlaySet->moveDisplayedOverlayUp(this->layerIndex);
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    
    this->updateControl(browserTabContent);
    
    this->overlaySelectionControl->updateControl();
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Move this layer down one level.
 */ 
void 
OverlaySelectionControlLayer::moveLayerDownToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            browserTabContent->invalidateSurfaceColoring();
            
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            surfaceOverlaySet->moveDisplayedOverlayDown(this->layerIndex);
            
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    
    this->updateControl(browserTabContent);
    
    this->overlaySelectionControl->updateControl();
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Remove this layer up one level.
 */ 
void 
OverlaySelectionControlLayer::removeLayerToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            browserTabContent->invalidateSurfaceColoring();
            
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            surfaceOverlaySet->removeDisplayedOverlay(this->layerIndex);
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    
    this->updateControl(browserTabContent);
    
    this->overlaySelectionControl->updateControl();
    
    emit controlRemoved();
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

void 
OverlaySelectionControlLayer::settingsToolButtonPressed()
{
    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    QMessageBox::information(this->overlaySelectionControl, "", "Settings!");
}

void 
OverlaySelectionControlLayer::metadataToolButtonPressed()
{
    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    QMessageBox::information(this->overlaySelectionControl, "", "Metadata!");
}

void 
OverlaySelectionControlLayer::histogramToolButtonPressed()
{
    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    QMessageBox::information(this->overlaySelectionControl, "", "Histogram!");
}

/**
 * Called when opacity is changed.
 * @param value
 *    New value for opacity.
 */
void 
OverlaySelectionControlLayer::opacityValueChanged(double value)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            browserTabContent->invalidateSurfaceColoring();
            
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            SurfaceOverlay* overlay = surfaceOverlaySet->getOverlay(this->layerIndex);
            overlay->setOpacity(value);
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    

    this->updateControl(browserTabContent);
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Called when a file selection is made.
 * @param fileIndex
 *    Index of selected file.
 */
void 
OverlaySelectionControlLayer::fileSelected(int fileIndex)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            browserTabContent->invalidateSurfaceColoring();
            
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            SurfaceOverlay* overlay = surfaceOverlaySet->getOverlay(this->layerIndex);
            
            void* pointer = this->fileSelectionComboBox->itemData(fileIndex).value<void*>();
            GiftiTypeFile* file = (GiftiTypeFile*)pointer;
            overlay->setSelectionData(file, 0);
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    
    this->updateControl(browserTabContent);
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Called when a column selection is made.
 * @param columnIndex
 *    Index of selected column.
 */
void 
OverlaySelectionControlLayer::columnSelected(int columnIndex)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
        {
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            SurfaceOverlay* overlay = surfaceOverlaySet->getOverlay(this->layerIndex);
            
            const int32_t fileIndex = this->fileSelectionComboBox->currentIndex();
            void* pointer = this->fileSelectionComboBox->itemData(fileIndex).value<void*>();
            GiftiTypeFile* file = (GiftiTypeFile*)pointer;
            overlay->setSelectionData(file, columnIndex);
        }
            break;
        case OverlaySelectionControl::VOLUME:
        {
            
        }
            break;
    }
    
    browserTabContent->invalidateSurfaceColoring();
    
    
    this->updateControl(browserTabContent);
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Update this control.
 * @param browserTabContent
 *     Content in the current browser tab.
 */
void 
OverlaySelectionControlLayer::updateControl(BrowserTabContent* browserTabContent)
{
    int numberOfDisplayedLayers = 3;
    this->widgetGroup->blockSignals(true);
    switch (this->dataType) {
        case OverlaySelectionControl::SURFACE:
            this->updateSurfaceControl(browserTabContent);
            numberOfDisplayedLayers = browserTabContent->getSurfaceOverlaySet()->getNumberOfDisplayedOverlays();
            break;
        case OverlaySelectionControl::VOLUME:
            this->updateVolumeControl(browserTabContent);
            break;
    }
    if (numberOfDisplayedLayers < 3) {
        numberOfDisplayedLayers = 3;
    }
    
    int32_t lastDisplayedIndex = numberOfDisplayedLayers - 1;
    bool isMoveUpEnabled = false;
    bool isMoveDownEnabled = false;
    bool isRemoveEnabled = false;
    
    if (this->layerIndex > 0) {
        isMoveUpEnabled = true;
    }
    if (this->layerIndex < lastDisplayedIndex) {
        isMoveDownEnabled = true;
    }
    if (numberOfDisplayedLayers > 3) {
        isRemoveEnabled = true;
    }
    
    this->moveDownAction->setEnabled(isMoveDownEnabled);
    this->moveUpAction->setEnabled(isMoveUpEnabled);
    this->removeAction->setEnabled(isRemoveEnabled);
    
    this->widgetGroup->blockSignals(false);
}

/**
 * Update this surface control.
 * @param browserTabContent
 *     Content in the current browser tab.
 */
void 
OverlaySelectionControlLayer::updateSurfaceControl(BrowserTabContent* browserTabContent)
{
    SurfaceOverlay* so = browserTabContent->getSurfaceOverlaySet()->getOverlay(this->layerIndex);
    
    this->fileSelectionComboBox->clear();
    this->columnSelectionComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<GiftiTypeFile*> dataFiles;
    GiftiTypeFile* selectedFile = NULL;
    AString selectedColumnName = "";
    int32_t selectedColumnIndex = -1;
    so->getSelectionData(browserTabContent,
                         dataFiles, 
                         selectedFile, 
                         selectedColumnName, 
                         selectedColumnIndex);
    
    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(dataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        GiftiTypeFile* dataFile = dataFiles[i];
        
        AString name = DataFileTypeEnum::toName(dataFile->getDataFileType())
        + " "
        + dataFile->getFileNameNoPath();
        this->fileSelectionComboBox->addItem(name,
                                             qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileSelectionComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the column selection combo box.
     */
    if (selectedFile != NULL) {
        const int32_t numColumns = selectedFile->getNumberOfColumns();
        for (int32_t i = 0; i < numColumns; i++) {
            this->columnSelectionComboBox->addItem(selectedFile->getColumnName(i));
        }
        this->columnSelectionComboBox->setCurrentIndex(selectedColumnIndex);
    }
    
    this->enabledCheckBox->setChecked(so->isEnabled());
    this->opacityDoubleSpinBox->setValue(so->getOpacity());
}

/**
 * Update this volume control.
 * @param browserTabContent
 *     Content in the current browser tab.
 */
void 
OverlaySelectionControlLayer::updateVolumeControl(BrowserTabContent* browserTabContent)
{
}

/**
 * Add a widget to a widget group so that they can be
 * hidden/displayed as a group.
 * @param w
 *   Widget that is added.
 */
void 
OverlaySelectionControlLayer::addWidget(QWidget* w)
{
    this->widgetGroup->add(w);
}

/**
 * @return Is this layer visible?
 */
bool
OverlaySelectionControlLayer::isVisible() const 
{
    return this->enabledCheckBox->isVisible();    
}

/**
 * Set the visibility of this layer.
 * @param visible
 *    New visibility status.
 */
void
OverlaySelectionControlLayer::setVisible(const bool visible)
{
    this->widgetGroup->setVisible(visible);
}
