
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

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

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
    m_blockDialogUpdate = true;
    
    /*------------------------------------------------------------------------*/
    /*
     * Create widgets
     */

    /*
     * Window number
     */
    QLabel* windowLabel = new QLabel("Workbench Window: ");
    m_browserWindowComboBox = new BrainBrowserWindowComboBox(BrainBrowserWindowComboBox::STYLE_NUMBER,
                                                             this);
    m_browserWindowComboBox->getWidget()->setFixedWidth(50);
    QObject::connect(m_browserWindowComboBox, SIGNAL(browserWindowSelected(BrainBrowserWindow*)),
                     this, SLOT(browserWindowComboBoxValueChanged(BrainBrowserWindow*)));
    QHBoxLayout* windowLayout = new QHBoxLayout();
    windowLayout->addWidget(windowLabel);
    windowLayout->addWidget(m_browserWindowComboBox->getWidget());
    windowLayout->addStretch();
    
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
    QLabel* panLabel = new QLabel("Pan:");
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
    QLabel* rotateLabel = new QLabel("Rotate: ");
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
    
    
    m_clippingWidgetGroup = new WuQWidgetObjectGroup(this);
    m_clippingWidgetGroup->add(m_xPanDoubleSpinBox);
    m_clippingWidgetGroup->add(m_yPanDoubleSpinBox);
    m_clippingWidgetGroup->add(m_zPanDoubleSpinBox);
    m_clippingWidgetGroup->add(m_xRotateDoubleSpinBox);
    m_clippingWidgetGroup->add(m_yRotateDoubleSpinBox);
    m_clippingWidgetGroup->add(m_zRotateDoubleSpinBox);
    m_clippingWidgetGroup->add(m_xThicknessDoubleSpinBox);
    m_clippingWidgetGroup->add(m_yThicknessDoubleSpinBox);
    m_clippingWidgetGroup->add(m_zThicknessDoubleSpinBox);
    
    /*
     * Show clipping box checkbox
     */
    m_displayClippingBoxCheckBox = new QCheckBox("Show Clipping Box Outline");
    QObject::connect(m_displayClippingBoxCheckBox, SIGNAL(clicked(bool)),
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
    
    /*------------------------------------------------------------------------*/
    /*
     * Finish up
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 4);
    layout->addLayout(windowLayout);
    layout->addWidget(m_displayClippingBoxCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(gridWidget);
    widget->setFixedSize(widget->sizeHint());
    
    /*
     * Remove apply button by using an empty name
     */
    setApplyButtonText("");

    m_resetPushButton = addUserPushButton("Reset", QDialogButtonBox::NoRole);
    
    setCentralWidget(widget,
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

/**
 * Called when a user (added) push button is clicked.
 *
 * @param userPushButton
 *     Button that was clicked.
 */
WuQDialogNonModal::DialogUserButtonResult
ClippingPlanesDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_resetPushButton) {
        BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
        if (bbw != NULL) {
            BrowserTabContent* btc = bbw->getBrowserTabContent();
            if (btc != NULL) {
                btc->resetClippingPlaneTransformation();
                updateContent(btc->getTabNumber());
                updateGraphicsWindow();
            }
        }
    }
    else {
        CaretAssert(0);
    }
    
    return WuQDialogNonModal::RESULT_NONE;
}

/**
 * Called when window number combo box value changed.
 */
void
ClippingPlanesDialog::browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow)
{
    int32_t windowIndex = -1;
    if (browserWindow != NULL) {
        windowIndex = browserWindow->getBrowserWindowIndex();
    }
    
    updateContent(windowIndex);
}


/**
 * Called when a clipping value is changed.
 */
void
ClippingPlanesDialog::clippingValueChanged()
{
    const float panning[3] = {
        (float)m_xPanDoubleSpinBox->value(),
        (float)m_yPanDoubleSpinBox->value(),
        (float)m_zPanDoubleSpinBox->value()
    };
    
    const float rotation[3] = {
        (float)m_xRotateDoubleSpinBox->value(),
        (float)m_yRotateDoubleSpinBox->value(),
        (float)m_zRotateDoubleSpinBox->value()
    };
    
    const float thickness[3] ={
        (float)m_xThicknessDoubleSpinBox->value(),
        (float)m_yThicknessDoubleSpinBox->value(),
        (float)m_zThicknessDoubleSpinBox->value()
    };
    
    /*
     * Update, set, and validate selected browser window
     */
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            btc->setClippingPlaneTransformation(panning,
                                                rotation,
                                                thickness,
                                                m_displayClippingBoxCheckBox->isChecked());
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
    m_browserWindowComboBox->updateComboBox();
    updateContent(m_browserWindowComboBox->getSelectedBrowserWindowIndex());
}


/**
 * Update the content in the dialog
 * @param browserWindowIndexIn
 *    Index of the browser window.
 */
void
ClippingPlanesDialog::updateContent(const int32_t browserWindowIndexIn)
{
    
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
    m_browserWindowComboBox->updateComboBox();
    m_browserWindowComboBox->setBrowserWindowByIndex(browserWindowIndexIn);
    const int32_t browserWindowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
    if (bbw != NULL) {
        BrowserTabContent* btc = bbw->getBrowserTabContent();
        if (btc != NULL) {
            m_clippingWidgetGroup->blockAllSignals(true);
            
            float panning[3];
            float rotation[3];
            float thickness[3];
            bool displayClippingBox;
            btc->getClippingPlaneTransformation(panning,
                                                rotation,
                                                thickness,
                                                displayClippingBox);
            
            m_xPanDoubleSpinBox->setValue(panning[0]);
            m_yPanDoubleSpinBox->setValue(panning[1]);
            m_zPanDoubleSpinBox->setValue(panning[2]);
            
            m_xRotateDoubleSpinBox->setValue(rotation[0]);
            m_yRotateDoubleSpinBox->setValue(rotation[1]);
            m_zRotateDoubleSpinBox->setValue(rotation[2]);
            
            m_xThicknessDoubleSpinBox->setValue(thickness[0]);
            m_yThicknessDoubleSpinBox->setValue(thickness[1]);
            m_zThicknessDoubleSpinBox->setValue(thickness[2]);
            
            m_displayClippingBoxCheckBox->setChecked(displayClippingBox);

            m_clippingWidgetGroup->blockAllSignals(false);
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
