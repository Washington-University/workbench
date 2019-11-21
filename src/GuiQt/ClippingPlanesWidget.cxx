
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

#define __CLIPPING_PLANES_WIDGET_DECLARE__
#include "ClippingPlanesWidget.h"
#undef __CLIPPING_PLANES_WIDGET_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUpdateYokedWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::ClippingPlanesWidget
 * \brief Dialog for adjusting clipping planes transformation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ClippingPlanesWidget::ClippingPlanesWidget(const QString& objectNamePrefix,
                                           QWidget* parent)
: QWidget(parent)
{
    m_objectNamePrefix = (objectNamePrefix
                          + ":ClippingPlanesWidget");
    setObjectName(m_objectNamePrefix);

    /*------------------------------------------------------------------------*/
    /*
     * Create widgets
     */

    QWidget* clippingBoxWidget = createClippingBoxWidget();
    QWidget* clippingAxesWidget = createClippingAxesWidget(); // xyz plane selected
    QWidget* clippingDataTypesWidget = createClippingDataTypeWidget();  // surface/volume/features

    /*------------------------------------------------------------------------*/
    QHBoxLayout* boxLayout = new QHBoxLayout();
    boxLayout->addWidget(clippingAxesWidget);
    boxLayout->addStretch();
    boxLayout->addWidget(clippingDataTypesWidget);

    /*
     * Finish up
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 4);
    layout->addLayout(boxLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(clippingBoxWidget);
    
    /*
     * Remove apply button by using an empty name
     */
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
ClippingPlanesWidget::~ClippingPlanesWidget()
{
}

/**
 * Called when a user (added) push button is clicked.
 *
 * @param userPushButton
 *     Button that was clicked.
 */
void
ClippingPlanesWidget::resetButtonClicked()
{
    BrowserTabContent* btc = getBrowserTabContent();
    if (btc != NULL) {
        btc->resetClippingPlaneTransformation();
        updateContent(btc->getTabNumber());
        updateGraphicsWindow();
    }
}

/**
 * Called when a clipping value is changed.
 */
void
ClippingPlanesWidget::clippingValueChanged()
{
    BrowserTabContent* browserTabContent = getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
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
    browserTabContent->setClippingPlaneTransformation(panning,
                                                      rotation,
                                                      thickness,
                                                      m_displayClippingBoxCheckBox->isChecked());
    
    browserTabContent->setClippingPlaneEnabled(m_xClippingEnabledCheckBox->isChecked(),
                                               m_yClippingEnabledCheckBox->isChecked(),
                                               m_zClippingEnabledCheckBox->isChecked(),
                                               m_surfaceClippingEnabledCheckBox->isChecked(),
                                               m_volumeClippingEnabledCheckBox->isChecked(),
                                               m_featuresClippingEnabledCheckBox->isChecked());
    
    if (browserTabContent->isBrainModelYoked()) {
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        updateGraphicsWindow();
    }
}


/**
 * Update the selected graphics window.
 */
void
ClippingPlanesWidget::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * @return Browser tab content for current tab index or NULL if invalid tab index
 */
BrowserTabContent*
ClippingPlanesWidget::getBrowserTabContent()
{
    EventBrowserTabGet tabEvent(m_tabIndex);
    EventManager::get()->sendEvent(tabEvent.getPointer());
    BrowserTabContent* tabContent(tabEvent.getBrowserTab());
    return tabContent;
}


/**
 * Update the content in the dialog
 * @param browserWindowIndexIn
 *    Index of the browser window.
 */
void
ClippingPlanesWidget::updateContent(const int32_t tabIndex)
{
    m_tabIndex = tabIndex;
    
    BrowserTabContent* browserTabContent = getBrowserTabContent();
    if (browserTabContent == NULL) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
    
    QSignalBlocker xPanBlocker(m_xPanDoubleSpinBox);
    QSignalBlocker yPanBlocker(m_yPanDoubleSpinBox);
    QSignalBlocker zPanBlocker(m_zPanDoubleSpinBox);

    QSignalBlocker xRotateBlocker(m_xRotateDoubleSpinBox);
    QSignalBlocker yRotateBlocker(m_yRotateDoubleSpinBox);
    QSignalBlocker zRotateBlocker(m_zRotateDoubleSpinBox);

    QSignalBlocker xThicknessBlocker(m_xThicknessDoubleSpinBox);
    QSignalBlocker yThicknessBlocker(m_yThicknessDoubleSpinBox);
    QSignalBlocker zThicknessBlocker(m_zThicknessDoubleSpinBox);

    float panning[3];
    float rotation[3];
    float thickness[3];
    bool displayClippingBox;
    browserTabContent->getClippingPlaneTransformation(panning,
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
    
    bool xEnabled;
    bool yEnabled;
    bool zEnabled;
    bool surfaceEnabled;
    bool volumeEnabled;
    bool featuresEnabled;
    browserTabContent->getClippingPlaneEnabled(xEnabled,
                                               yEnabled,
                                               zEnabled,
                                               surfaceEnabled,
                                               volumeEnabled,
                                               featuresEnabled);
    
    m_xClippingEnabledCheckBox->setChecked(xEnabled);
    m_yClippingEnabledCheckBox->setChecked(yEnabled);
    m_zClippingEnabledCheckBox->setChecked(zEnabled);
    
    m_surfaceClippingEnabledCheckBox->setChecked(surfaceEnabled);
    m_volumeClippingEnabledCheckBox->setChecked(volumeEnabled);
    m_featuresClippingEnabledCheckBox->setChecked(featuresEnabled);
}

/**
 * @return Instance of the clipping box widget
 */
QWidget*
ClippingPlanesWidget::createClippingBoxWidget()
{
    /*
     * Show clipping box checkbox
     */
    m_displayClippingBoxCheckBox = new QCheckBox("Show Clipping Box Outline");
    QObject::connect(m_displayClippingBoxCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    
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

    QToolButton* resetToolButton = new QToolButton();
    WuQtUtilities::setToolButtonStyleForQt5Mac(resetToolButton);
    resetToolButton->setText("Reset");
    QObject::connect(resetToolButton, &QToolButton::clicked,
                     this, &ClippingPlanesWidget::resetButtonClicked);
    
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
    
    QGroupBox* groupBox = new QGroupBox("Clipping Box");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
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
    gridLayout->addWidget(m_displayClippingBoxCheckBox,
                          row, 0, 1, 2, Qt::AlignLeft);
    gridLayout->addWidget(resetToolButton,
                          row, 2, 1, 2, Qt::AlignCenter);
    row++;

    return groupBox;
}

/**
 * @return Instance of the clipping axes widget
 */
QWidget*
ClippingPlanesWidget::createClippingAxesWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_xClippingEnabledCheckBox = new QCheckBox("X");
    QObject::connect(m_xClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    m_xClippingEnabledCheckBox->setToolTip("Enable X clipping plane");
    m_xClippingEnabledCheckBox->setObjectName(m_objectNamePrefix
                                              + ":EnableX");
    macroManager->addMacroSupportToObject(m_xClippingEnabledCheckBox,
                                          "Enable X clipping plane");
    
    m_yClippingEnabledCheckBox = new QCheckBox("Y");
    QObject::connect(m_yClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    m_yClippingEnabledCheckBox->setToolTip("Enable Y clipping plane");
    m_yClippingEnabledCheckBox->setObjectName(m_objectNamePrefix
                                              + ":EnableY");
    macroManager->addMacroSupportToObject(m_yClippingEnabledCheckBox,
                                          "Enable Y clipping plane");
    
    m_zClippingEnabledCheckBox = new QCheckBox("Z");
    QObject::connect(m_zClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    m_zClippingEnabledCheckBox->setToolTip("Enable Z clipping plane");
    m_zClippingEnabledCheckBox->setObjectName(m_objectNamePrefix
                                              + ":EnableZ");
    macroManager->addMacroSupportToObject(m_zClippingEnabledCheckBox,
                                          "Enable Z clipping");
    
    QWidget* groupBox = new QGroupBox("Enable Clipping Planes");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 4);
    layout->addWidget(m_xClippingEnabledCheckBox);
    layout->addWidget(m_yClippingEnabledCheckBox);
    layout->addWidget(m_zClippingEnabledCheckBox);

    return groupBox;
}

/**
 * @return Instance of the clipping data type widget
 */
QWidget*
ClippingPlanesWidget::createClippingDataTypeWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_surfaceClippingEnabledCheckBox = new QCheckBox("Surface");
    QObject::connect(m_surfaceClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    m_surfaceClippingEnabledCheckBox->setToolTip("Enable Clipping of Surface");
    m_surfaceClippingEnabledCheckBox->setObjectName(m_objectNamePrefix
                                                    + ":EnableSurface");
    macroManager->addMacroSupportToObject(m_surfaceClippingEnabledCheckBox,
                                          "Enable surface clipping");
    
    m_volumeClippingEnabledCheckBox = new QCheckBox("Volume");
    QObject::connect(m_volumeClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    m_volumeClippingEnabledCheckBox->setToolTip("Enable Clipping of Volume Slices");
    m_volumeClippingEnabledCheckBox->setObjectName(m_objectNamePrefix
                                                   + ":EnableVolume");
    macroManager->addMacroSupportToObject(m_volumeClippingEnabledCheckBox,
                                          "Enable volume clipping");
    
    m_featuresClippingEnabledCheckBox = new QCheckBox("Features");
    QObject::connect(m_featuresClippingEnabledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(clippingValueChanged()));
    m_featuresClippingEnabledCheckBox->setToolTip("Enable Clipping of Features");
    m_featuresClippingEnabledCheckBox->setObjectName(m_objectNamePrefix
                                                     + ":EnableFeatures");
    macroManager->addMacroSupportToObject(m_featuresClippingEnabledCheckBox,
                                          "Enable features clipping");
    
    QWidget* groupBox = new QGroupBox("Clipping Affects");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 4);
    layout->addWidget(m_surfaceClippingEnabledCheckBox);
    layout->addWidget(m_volumeClippingEnabledCheckBox);
    layout->addWidget(m_featuresClippingEnabledCheckBox);
    
    return groupBox;
}


