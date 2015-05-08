
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

#define __ANNOTATION_ROTATION_WIDGET_DECLARE__
#include "AnnotationRotationWidget.h"
#undef __ANNOTATION_ROTATION_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>


#include "AnnotationTwoDimensionalShape.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationRotationWidget 
 * \brief Widget for adjusting annotation's rotation angle.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window.
 * @param parent
 *    Parent of this widget.
 */
AnnotationRotationWidget::AnnotationRotationWidget(const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* rotationLabel = new QLabel(" R:");
    m_rotationSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 359, 1.0, 0,
                                                                                       this, SLOT(rotationValueChanged(double)));
    m_rotationSpinBox->setWrapping(true);
    WuQtUtilities::setWordWrappedToolTip(m_rotationSpinBox,
                                         "Rotation, clockwise in degrees");
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(rotationLabel);
    layout->addWidget(m_rotationSpinBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
AnnotationRotationWidget::~AnnotationRotationWidget()
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
AnnotationRotationWidget::receiveEvent(Event* event)
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
AnnotationRotationWidget::updateContent(AnnotationTwoDimensionalShape* annotation2D)
{
    m_annotation2D = annotation2D;
    
    if (m_annotation2D != NULL) {
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
 * Gets called when rotation value is changed.
 *
 * @param value
 *
 */
void
AnnotationRotationWidget::rotationValueChanged(double value)
{
    if (m_annotation2D != NULL) {
        m_annotation2D->setRotationAngle(value);
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
}
