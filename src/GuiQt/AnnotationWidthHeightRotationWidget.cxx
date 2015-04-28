
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_WIDTH_HEIGHT_ROTATION_WIDGET_DECLARE__
#include "AnnotationWidthHeightRotationWidget.h"
#undef __ANNOTATION_WIDTH_HEIGHT_ROTATION_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpinBox>

#include "AnnotationTwoDimensionalShape.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "StructureEnumComboBox.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationWidthHeightRotationWidget 
 * \brief Widget for editing annotation coordinate, size, and rotation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationWidthHeightRotationWidget::AnnotationWidthHeightRotationWidget(const int32_t browserWindowIndex,
                                                                         QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_annotation2D = NULL;
    
    QLabel* widthLabel = new QLabel(" W:");
    m_widthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                    this, SLOT(widthValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_widthSpinBox,
                                         "Width of 2D Shapes (Box, Image, Oval)");

    QLabel* heightLabel = new QLabel(" H:");
    m_heightSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                    this, SLOT(heightValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_heightSpinBox,
                                         "Height of 2D Shapes (Box, Image, Oval)");

    QLabel* rotationLabel = new QLabel(" R:");
    m_rotationSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 360, 1.0, 0,
                                                                                    this, SLOT(rotationValueChanged(double)));
    m_rotationSpinBox->setWrapping(true);
    WuQtUtilities::setWordWrappedToolTip(m_rotationSpinBox,
                                         "Rotation, clockwise in degrees, of annotation");

    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(widthLabel);
    layout->addWidget(m_widthSpinBox);
    layout->addWidget(heightLabel);
    layout->addWidget(m_heightSpinBox);
    layout->addWidget(rotationLabel);
    layout->addWidget(m_rotationSpinBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
AnnotationWidthHeightRotationWidget::~AnnotationWidthHeightRotationWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationWidthHeightRotationWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Update with the given annotation.
 *
 * @param annotation2D.
 *    Two dimensional annotation.
 */
void
AnnotationWidthHeightRotationWidget::updateContent(AnnotationTwoDimensionalShape* annotation2D)
{
    m_annotation2D = annotation2D;
    
    if (m_annotation2D != NULL) {
        m_widthSpinBox->setValue(m_annotation2D->getWidth());
        m_heightSpinBox->setValue(m_annotation2D->getHeight());
        double rotation = m_annotation2D->getRotationAngle();
        if (rotation < 0.0) {
            rotation += 360.0;
        }
        m_rotationSpinBox->setValue(rotation);
        
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
}

/**
 * Gets called when height value is changed.
 *
 * @param value
 *
 */
void
AnnotationWidthHeightRotationWidget::heightValueChanged(double value)
{
    if (m_annotation2D != NULL) {
        m_annotation2D->setHeight(value);
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
}

/**
 * Gets called when width value is changed.
 *
 * @param value
 *
 */
void
AnnotationWidthHeightRotationWidget::widthValueChanged(double value)
{
    if (m_annotation2D != NULL) {
        m_annotation2D->setWidth(value);
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
}

/**
 * Gets called when rotation value is changed.
 *
 * @param value
 *
 */
void
AnnotationWidthHeightRotationWidget::rotationValueChanged(double value)
{
    if (m_annotation2D != NULL) {
        m_annotation2D->setRotationAngle(value);
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
}

