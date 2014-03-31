
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __CLIPPING_PLANES_DIALOG_DECLARE__
#include "ClippingPlanesDialog.h"
#undef __CLIPPING_PLANES_DIALOG_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ClippingPlaneGroup.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ClippingPlanesDialog 
 * \brief Dialog for adjusting clipping planes transformation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ClippingPlanesDialog::ClippingPlanesDialog(QWidget* parent)
: WuQDialogNonModal("Clipping Planes",
                    parent)
{
    m_browserWindowIndex = -1;
    m_blockDialogUpdate = true;
    
    /*------------------------------------------------------------------------*/
    /*
     * Create widgets
     */

    /*
     * X, Y, Z column labels
     */
    QLabel* xColumnLabel = new QLabel("X");
    QLabel* yColumnLabel = new QLabel("Y");
    QLabel* zColumnLabel = new QLabel("Z");
    
    const int spinBoxWidth = 90;
    /*
     * Panning
     */
    const double panStep = 1.0;
    QLabel* panLabel = new QLabel("Panning:");
    m_xPanDoubleSpinBox = new QDoubleSpinBox;
    m_xPanDoubleSpinBox->setMinimum(-100000.0);
    m_xPanDoubleSpinBox->setMaximum( 100000.0);
    m_xPanDoubleSpinBox->setSingleStep(panStep);
    m_xPanDoubleSpinBox->setDecimals(2);
    m_xPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    m_yPanDoubleSpinBox = new QDoubleSpinBox;
    m_yPanDoubleSpinBox->setMinimum(-100000.0);
    m_yPanDoubleSpinBox->setMaximum( 100000.0);
    m_yPanDoubleSpinBox->setSingleStep(panStep);
    m_yPanDoubleSpinBox->setDecimals(2);
    m_yPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    m_zPanDoubleSpinBox = new QDoubleSpinBox;
    m_zPanDoubleSpinBox->setMinimum(-100000.0);
    m_zPanDoubleSpinBox->setMaximum( 100000.0);
    m_zPanDoubleSpinBox->setSingleStep(panStep);
    m_zPanDoubleSpinBox->setDecimals(2);
    m_zPanDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zPanDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    
    
    /*
     * Rotation
     */
    const double rotationMinimum = -360.0;
    const double rotationMaximum =  360.0;
    const double rotateStep = 1.0;
    QLabel* rotateLabel = new QLabel("Rotation: ");
    m_xRotateDoubleSpinBox = new QDoubleSpinBox;
    m_xRotateDoubleSpinBox->setWrapping(true);
    m_xRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_xRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_xRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_xRotateDoubleSpinBox->setDecimals(2);
    m_xRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    m_yRotateDoubleSpinBox = new QDoubleSpinBox;
    m_yRotateDoubleSpinBox->setWrapping(true);
    m_yRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_yRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_yRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_yRotateDoubleSpinBox->setDecimals(2);
    m_yRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    m_zRotateDoubleSpinBox = new QDoubleSpinBox;
    m_zRotateDoubleSpinBox->setWrapping(true);
    m_zRotateDoubleSpinBox->setMinimum(rotationMinimum);
    m_zRotateDoubleSpinBox->setMaximum(rotationMaximum);
    m_zRotateDoubleSpinBox->setSingleStep(rotateStep);
    m_zRotateDoubleSpinBox->setDecimals(2);
    m_zRotateDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    
    /*
     * Thickness
     */
    const double thicknessMinimum = 0.0;
    const double thicknessMaximum = 1000000.0;
    const double thicknessStep    = 1.0;
    QLabel* thicknessLabel = new QLabel("Thickness (mm)");
    
    m_xThicknessDoubleSpinBox = new QDoubleSpinBox();
    m_xThicknessDoubleSpinBox->setMinimum(thicknessMinimum);
    m_xThicknessDoubleSpinBox->setMaximum(thicknessMaximum);
    m_xThicknessDoubleSpinBox->setSingleStep(thicknessStep);
    m_xThicknessDoubleSpinBox->setDecimals(2);
    m_xThicknessDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_xThicknessDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    m_yThicknessDoubleSpinBox = new QDoubleSpinBox();
    m_yThicknessDoubleSpinBox->setMinimum(thicknessMinimum);
    m_yThicknessDoubleSpinBox->setMaximum(thicknessMaximum);
    m_yThicknessDoubleSpinBox->setSingleStep(thicknessStep);
    m_yThicknessDoubleSpinBox->setDecimals(2);
    m_yThicknessDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_yThicknessDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    m_zThicknessDoubleSpinBox = new QDoubleSpinBox();
    m_zThicknessDoubleSpinBox->setMinimum(thicknessMinimum);
    m_zThicknessDoubleSpinBox->setMaximum(thicknessMaximum);
    m_zThicknessDoubleSpinBox->setSingleStep(thicknessStep);
    m_zThicknessDoubleSpinBox->setDecimals(2);
    m_zThicknessDoubleSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_zThicknessDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(clippingValueChanged()));
    
    /*------------------------------------------------------------------------*/
    /*
     * Layout widgets
     */
    /*
     * Columns for grid layout
     */
    int column = 0;
    const int COLUMN_LABEL  = column++;
    const int COLUMN_X      = column++;
    const int COLUMN_Y      = column++;
    const int COLUMN_Z      = column++;
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 4);
    int row = 0;
    gridLayout->addWidget(xColumnLabel,
                          row,
                          COLUMN_X,
                          Qt::AlignHCenter);
    gridLayout->addWidget(yColumnLabel,
                          row,
                          COLUMN_Y,
                          Qt::AlignHCenter);
    gridLayout->addWidget(zColumnLabel,
                          row,
                          COLUMN_Z,
                          Qt::AlignHCenter);
    row++;
    gridLayout->addWidget(panLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xPanDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yPanDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zPanDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(rotateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xRotateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yRotateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zRotateDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(thicknessLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xThicknessDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yThicknessDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zThicknessDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    
    /*------------------------------------------------------------------------*/
    /*
     * Finish up
     */
    /*
     * Remove apply button by using an empty name
     */
    setApplyButtonText("");
    
    setCentralWidget(gridWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    /*
     * No auto default button processing (Qt highlights button)
     */
    disableAutoDefaultForAllPushButtons();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    m_blockDialogUpdate = false;
}

/**
 * Destructor.
 */
ClippingPlanesDialog::~ClippingPlanesDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

void
ClippingPlanesDialog::clippingValueChanged()
{
    const float panning[3] = {
        m_xPanDoubleSpinBox->value(),
        m_yPanDoubleSpinBox->value(),
        m_zPanDoubleSpinBox->value()
    };
    
    const float rotation[3] = {
        m_xRotateDoubleSpinBox->value(),
        m_yRotateDoubleSpinBox->value(),
        m_zRotateDoubleSpinBox->value()
    };
    
    const float thickness[3] ={
        m_xThicknessDoubleSpinBox->value(),
        m_yThicknessDoubleSpinBox->value(),
        m_zThicknessDoubleSpinBox->value()
    };
    
    /*
     * Update, set, and validate selected browser window
     */
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            ClippingPlaneGroup* clippingGroup = btc->getClippingPlaneGroup();
            
            clippingGroup->setTranslation(panning);
            clippingGroup->setRotationAngles(rotation);
            clippingGroup->setThickness(thickness);
            
            updateGraphicsWindow();
        }
    }
}


/**
 * Update the selected graphics window.
 */
void
ClippingPlanesDialog::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Gets called when the dialog gains focus.
 */
void
ClippingPlanesDialog::focusGained()
{
    updateDialog();
}

/**
 * Update the dialog.
 */
void
ClippingPlanesDialog::updateDialog()
{
    updateContent(m_browserWindowIndex);
}


/**
 * Update the dialog.
 */
void
ClippingPlanesDialog::updateContent(const int32_t browserWindowIndex)
{
    m_browserWindowIndex = browserWindowIndex;
    
    /*
     * May get updates when graphics are redrawn by this dialog
     * and not doing this could result in infinite loop
     */
    if (m_blockDialogUpdate) {
        return;
    }
    
    /*
     * Update, set, and validate selected browser window
     */
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            ClippingPlaneGroup* clippingGroup = btc->getClippingPlaneGroup();
            
            float panning[3];
            clippingGroup->getTranslation(panning);
            m_xPanDoubleSpinBox->setValue(panning[0]);
            m_yPanDoubleSpinBox->setValue(panning[1]);
            m_zPanDoubleSpinBox->setValue(panning[2]);
            
            float rotationAngles[3];
            clippingGroup->getRotationAngles(rotationAngles);
            m_xRotateDoubleSpinBox->setValue(rotationAngles[0]);
            m_yRotateDoubleSpinBox->setValue(rotationAngles[1]);
            m_zRotateDoubleSpinBox->setValue(rotationAngles[2]);
            
            float thickness[3];
            clippingGroup->getThickness(thickness);
            m_xThicknessDoubleSpinBox->setValue(thickness[0]);
            m_yThicknessDoubleSpinBox->setValue(thickness[1]);
            m_zThicknessDoubleSpinBox->setValue(thickness[2]);
        }
    }
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
ClippingPlanesDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* updateEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(updateEvent);
        
        updateEvent->setEventProcessed();
        
        updateDialog();
    }
}
