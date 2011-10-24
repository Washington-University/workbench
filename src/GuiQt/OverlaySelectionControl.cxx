
/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include <iostream>

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QStringList>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GiftiTypeFile.h"
#include "GuiManager.h"
#include "OverlaySelectionControl.h"
#include "OverlaySelectionControlLayer.h"
#include "SurfaceOverlay.h"
#include "SurfaceOverlaySet.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;

/**
 * \class OverlaySelectionControl
 * \brief Control for selection of overlay.
 */

/**
 * Constructs a control for selection of overlays.
 * 
 * @param orientation
 *    Orientation of the controls (horizontal/vertical).
 * @param dataType
 *    Type of data (surface/volume)
 * @param parent
 *    Parent of the control.
 */
OverlaySelectionControl::OverlaySelectionControl(const int32_t browserWindowIndex,
                                                 const Qt::Orientation orientation,
                                                 const DataType dataType,
                                                 QWidget* parent)
: QWidget(parent)
{
    this->browserWindowIndex = browserWindowIndex;
    this->dataType = dataType;
    this->orientation = orientation;
    
    this->addLayerPushButton = new QPushButton("Add Layer");
    this->addLayerPushButton->setFixedSize(this->addLayerPushButton->sizeHint());
    
    this->layersGridWidget = this->createLayers();
    this->updateLayersGridWidgetSize();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(this->layersGridWidget);  //, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(this->addLayerPushButton); //, Qt::AlignHCenter | Qt::AlignTop);
    //layout->addStretch();
    
    this->blockSignals(true);
    //this->addLayer();
    //this->addLayer();
    
    this->blockSignals(false);

    QObject::connect(this->addLayerPushButton, SIGNAL(clicked()),
                     this, SLOT(addLayer()));
}

/**
 * Destroy overlay selection control.
 */
OverlaySelectionControl::~OverlaySelectionControl()
{
    
}

/**
 * Create the layers.
 */
QWidget*
OverlaySelectionControl::createLayers()
{
    const bool smallArrowButtonsFlag = true;
    const bool verticalCompactFlag = true;
    
    QGroupBox* gridWidget = new QGroupBox("");
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    gridLayout->setVerticalSpacing(0);
    if (verticalCompactFlag) {
        gridLayout->setVerticalSpacing(4);
    }
    gridLayout->setContentsMargins(0, 0, 0, 0);
    
    switch (orientation) {
        case Qt::Horizontal:
        {
            int row = 0;
            gridLayout->addWidget(new QLabel("On"), row, 0);
            switch (dataType) {
                case SURFACE:
                    gridLayout->addWidget(new QLabel("File"), row, 1, Qt::AlignHCenter);
                    gridLayout->addWidget(new QLabel("Map"), row, 2, Qt::AlignHCenter);
                    break;
                case VOLUME:
                    gridLayout->addWidget(new QLabel("Volume"), row, 1, Qt::AlignHCenter);
                    gridLayout->addWidget(new QLabel("Type"), row, 2, Qt::AlignHCenter);
                    break;
            }
            gridLayout->addWidget(new QLabel("Hist"), row, 3);
            gridLayout->addWidget(new QLabel("Settings"), row, 4);
            gridLayout->addWidget(new QLabel("Meta"), row, 5);
            gridLayout->addWidget(new QLabel("Opacity"), row, 6);

            if (smallArrowButtonsFlag) {
                gridLayout->addWidget(new QLabel("Move"), row, 7, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Remove"), row, 8);
            }
            else {            
                gridLayout->addWidget(new QLabel("Move"), row, 7, 1, 2, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Remove"), row, 9);
            }
            
            gridLayout->setColumnStretch(0, 0);
            gridLayout->setColumnStretch(1, 100);
            gridLayout->setColumnStretch(2, 100);
            gridLayout->setColumnStretch(3, 0);
            gridLayout->setColumnStretch(4, 0);
            gridLayout->setColumnStretch(5, 0);
            gridLayout->setColumnStretch(6, 0);
            gridLayout->setColumnStretch(7, 0);
            gridLayout->setColumnStretch(8, 0);

            if (smallArrowButtonsFlag == false) {
                gridLayout->setColumnStretch(9, 0);
            }
        }
            break;
        case Qt::Vertical:
        {
            int row = 0;
            gridLayout->addWidget(new QLabel("On"), row, 0);
            switch (dataType) {
                case SURFACE:
                    gridLayout->addWidget(new QLabel("File/Map"), row, 1, Qt::AlignHCenter);
                    break;
                case VOLUME:
                    gridLayout->addWidget(new QLabel("Volume/Type"), row, 1, Qt::AlignHCenter);
                    break;
            }
            if (verticalCompactFlag == false) {
                gridLayout->addWidget(new QLabel("Adjust"), row, 2, 1, 3, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Move"), row, 5);
                gridLayout->addWidget(new QLabel("Remove"), row, 6);
            }
            
            row++;
/*            
            QFrame* horizLine = new QFrame();
            horizLine->setFrameStyle(QFrame::HLine | QFrame::Plain);
            horizLine->setLineWidth(1);
            horizLine->setMidLineWidth(1);
            gridLayout->addWidget(horizLine, row, 0, 1, 7);
            
            row++;                
*/            
            gridLayout->setColumnStretch(0, 0);
            gridLayout->setColumnStretch(1, 100);
            if (verticalCompactFlag == false) {
                gridLayout->setColumnStretch(3, 0);
                gridLayout->setColumnStretch(4, 0);
                gridLayout->setColumnStretch(5, 0);
                gridLayout->setColumnStretch(6, 0);
            }
        };
            break;
    }
    
    const Qt::Alignment ALIGN_LEFT_VERTICAL_CENTER = Qt::AlignLeft | Qt::AlignVCenter;
    const Qt::Alignment ALIGN_TOP_HORIZONTAL_CENTER = Qt::AlignTop | Qt::AlignHCenter;
    const Qt::Alignment ALIGN_BOTTOM_HORIZONTAL_CENTER = Qt::AlignBottom | Qt::AlignHCenter;
    
    
    for (int32_t i = 0; i < MAXIMUM_NUMBER_OF_LAYERS; i++) {
        OverlaySelectionControlLayer* layer = new OverlaySelectionControlLayer(this->browserWindowIndex,
                                 this, this->dataType, 
                                 i);
        this->layers.append(layer);
        
        QObject::connect(layer, SIGNAL(controlRemoved()),
                         this, SIGNAL(controlRemoved()));
        
        switch (orientation) {
            case Qt::Horizontal:
            if (smallArrowButtonsFlag) {
                QSize arrowButtonSize = layer->upArrowToolButton->sizeHint();
                arrowButtonSize.setWidth(arrowButtonSize.width() / 2);
                arrowButtonSize.setHeight(arrowButtonSize.height() / 2);
                layer->upArrowToolButton->setFixedSize(arrowButtonSize);
                layer->downArrowToolButton->setFixedSize(arrowButtonSize);
                
                int row = gridLayout->rowCount();
                
                gridLayout->addWidget(layer->enabledCheckBox, row, 0, 2, 1, Qt::AlignCenter);
                gridLayout->addWidget(layer->fileSelectionComboBox, row, 1, 2, 1);
                gridLayout->addWidget(layer->columnSelectionComboBox, row, 2, 2, 1);
                gridLayout->addWidget(layer->histogramToolButton, row, 3, 2, 1, Qt::AlignCenter);
                gridLayout->addWidget(layer->settingsToolButton, row, 4, 2, 1, Qt::AlignCenter);
                gridLayout->addWidget(layer->metadataToolButton, row, 5, 2, 1, Qt::AlignCenter);
                gridLayout->addWidget(layer->opacityDoubleSpinBox, row, 6, 2, 1, ALIGN_LEFT_VERTICAL_CENTER);
                
                gridLayout->addWidget(layer->upArrowToolButton, row, 7, ALIGN_TOP_HORIZONTAL_CENTER);
                gridLayout->addWidget(layer->downArrowToolButton, row + 1, 7, ALIGN_BOTTOM_HORIZONTAL_CENTER);
                gridLayout->addWidget(layer->deleteToolButton, row, 8, 2, 1, Qt::AlignCenter);
            }
            else {
                int row = gridLayout->rowCount();
                
                gridLayout->addWidget(layer->enabledCheckBox, row, 0);
                gridLayout->addWidget(layer->fileSelectionComboBox, row, 1);
                gridLayout->addWidget(layer->columnSelectionComboBox, row, 2);
                gridLayout->addWidget(layer->histogramToolButton, row, 3);
                gridLayout->addWidget(layer->settingsToolButton, row, 4, Qt::AlignHCenter);
                gridLayout->addWidget(layer->metadataToolButton, row, 5);
                gridLayout->addWidget(layer->opacityDoubleSpinBox, row, 6);
                
                gridLayout->addWidget(layer->upArrowToolButton, row, 7);
                gridLayout->addWidget(layer->downArrowToolButton, row, 8);
                gridLayout->addWidget(layer->deleteToolButton, row, 9, Qt::AlignHCenter);
            }
                break;
            case Qt::Vertical:
            {
                if (verticalCompactFlag) {
                    int row = gridLayout->rowCount();
                    
                    const int row1 = row;
                    const int row2 = row1 + 1;
                    const int row3 = row2 + 1;
                    const int row4 = row3 + 1;
                    
                    QFrame* horizLine = new QFrame();
                    horizLine->setFrameStyle(QFrame::HLine | QFrame::Plain);
                    horizLine->setLineWidth(1);
                    horizLine->setMidLineWidth(1);
                    gridLayout->addWidget(horizLine, row1, 0, 1, 2);  
                    layer->addWidget(horizLine);
                    
                    gridLayout->addWidget(layer->enabledCheckBox, row2, 0, 3, 1);
                    gridLayout->addWidget(layer->fileSelectionComboBox, row2, 1);
                    gridLayout->addWidget(layer->columnSelectionComboBox, row3, 1);
                    
                    QHBoxLayout* buttLayout = new QHBoxLayout();
                    buttLayout->addWidget(layer->histogramToolButton);
                    buttLayout->addWidget(layer->settingsToolButton);
                    buttLayout->addWidget(layer->metadataToolButton);
                    buttLayout->addStretch();
                    buttLayout->addWidget(layer->opacityDoubleSpinBox);
                    buttLayout->addStretch();
                    buttLayout->addWidget(layer->upArrowToolButton);
                    buttLayout->addWidget(layer->downArrowToolButton);
                    buttLayout->addWidget(layer->deleteToolButton);
                    
                    gridLayout->addLayout(buttLayout, row4, 1);

                }
                else {
                    int row = gridLayout->rowCount();
                    
                    const int row1 = row;
                    const int row2 = row1 + 1;
                    const int row3 = row2 + 1;
                    
                    QFrame* horizLine = new QFrame();
                    horizLine->setFrameStyle(QFrame::HLine | QFrame::Plain);
                    horizLine->setLineWidth(1);
                    horizLine->setMidLineWidth(1);
                    gridLayout->addWidget(horizLine, row1, 0, 1, 7);  
                    layer->addWidget(horizLine);
                    
                    gridLayout->addWidget(layer->enabledCheckBox, row2, 0, 2, 1);
                    gridLayout->addWidget(layer->fileSelectionComboBox, row2, 1);
                    gridLayout->addWidget(layer->columnSelectionComboBox, row3, 1);
                    gridLayout->addWidget(layer->opacityDoubleSpinBox, row2, 2, 1, 3);
                    gridLayout->addWidget(layer->histogramToolButton, row3, 2);
                    gridLayout->addWidget(layer->settingsToolButton, row3, 3);
                    gridLayout->addWidget(layer->metadataToolButton, row3, 4);
                    
                    gridLayout->addWidget(layer->upArrowToolButton, row2, 5, Qt::AlignHCenter);
                    gridLayout->addWidget(layer->downArrowToolButton, row3, 5, Qt::AlignHCenter);
                    gridLayout->addWidget(layer->deleteToolButton, row2, 6, 2, 1, Qt::AlignHCenter);
                }
                
            };
                break;
        }
     
        layer->setVisible(false);
    }
    
    return gridWidget;
}

/**
 * Add a layer.
 */
void 
OverlaySelectionControl::addLayer() 
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
    surfaceOverlaySet->addDisplayedOverlay();
    
    this->updateControl();
    /*
    EventBrowserTabGet eventBrowserTab(windowTabNumber);
    EventManager::get()->sendEvent(eventBrowserTab.getPointer());
    
    BrowserTabContent* browserTabContent = eventBrowserTab.getBrowserTab();
    CaretAssert(browserTabContent);
    
    this->updateLayersGridWidgetSize();
*/    
    /*
    switch (orientation) {
        case HORIZONTAL:
            this->setMaximumHeight(this->sizeHint().height());
            break;
        case VERTICAL:
            this->setMaximumWidth(this->sizeHint().width());
            break;
    }
    */
    
    emit layersChanged();
}

/**
 * 
 */
void 
OverlaySelectionControl::updateLayersGridWidgetSize()
{
    //this->layersGridWidget->setFixedSize(layersGridWidget->sizeHint());
    //this->setFixedSize(this->sizeHint());
}

/**
 * Update the control.
 */
void 
OverlaySelectionControl::updateControl()
{
    BrowserTabContent* browserTabContent = 
        GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    int32_t numberOfDisplayedOverlays = 0;
    
    switch (dataType) {
        case SURFACE:
        {
            SurfaceOverlaySet* surfaceOverlaySet = browserTabContent->getSurfaceOverlaySet();
            numberOfDisplayedOverlays = surfaceOverlaySet->getNumberOfDisplayedOverlays();
        }
            break;
        case VOLUME:
        {
            //VolumeOverlaySet* volumeOverlaySet = browserTabContent->getVolumeOverlaySet();
            //numberOfDisplayedOverlays = volumeOverlaySet->getNumberOfDisplayedOverlays();
        }
            break;
    }
    for (int32_t i = 0; i < this->layers.size(); i++) {
        if (i < numberOfDisplayedOverlays) {
            this->layers[i]->setVisible(true);
            this->layers[i]->updateControl(browserTabContent);
        }
        else {
            this->layers[i]->setVisible(false);
        }
    }    
}

