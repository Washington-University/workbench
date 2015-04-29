
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

#define __ANNOTATION_LINE_SIZE_WIDGET_DECLARE__
#include "AnnotationLineSizeWidget.h"
#undef __ANNOTATION_LINE_SIZE_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>

#include "AnnotationOneDimensionalShape.h"
#include "AnnotationTwoDimensionalShape.h"
#include "BrainOpenGL.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationLineSizeWidget 
 * \brief Widget for controlling annotation line size outline width
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget.
 */
AnnotationLineSizeWidget::AnnotationLineSizeWidget(const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_annotation = NULL;
    
    float minimumLineWidth = 0.0;
    float maximumLineWidth = 1.0;
    
    BrainOpenGL::getMinMaxLineWidth(minimumLineWidth,
                                    maximumLineWidth);
    QLabel* sizeLabel = new QLabel("S:");
    m_lineSizeSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 10.0, 1.0, 0,
                                                                                    this, SLOT(lineSizeSpinBoxValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_lineSizeSpinBox,
                                         "Line width for arrow or line\n"
                                         "Outline width for box or oval");
    m_lineSizeSpinBox->setFixedWidth(45);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(sizeLabel);
    layout->addWidget(m_lineSizeSpinBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationLineSizeWidget::~AnnotationLineSizeWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

void
AnnotationLineSizeWidget::lineSizeSpinBoxValueChanged(double value)
{
    if (m_annotation != NULL) {
        AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotation);
        if (oneDimAnn != NULL) {
            oneDimAnn->setLineWidth(value);
        }
        else {
            AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotation);
            if (twoDimAnn != NULL) {
                twoDimAnn->setOutlineWidth(value);
            }
        }
    }
}

/**
 * Update with the given annotation.
 *
 * @param annotation2D.
 *    Two dimensional annotation.
 */
void
AnnotationLineSizeWidget::updateContent(Annotation* annotation)
{
    m_annotation = annotation;
    
    double value = 0.0;
    
    bool widgetEnabled = false;
    
    if (m_annotation != NULL) {
        AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotation);
        if (oneDimAnn != NULL) {
            value = oneDimAnn->getLineWidth();
            widgetEnabled = true;
        }
        else {
            AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotation);
            if (twoDimAnn != NULL) {
                value = twoDimAnn->getOutlineWidth();
                widgetEnabled = true;
            }
        }
    }
    
    m_lineSizeSpinBox->blockSignals(true);
    m_lineSizeSpinBox->setValue(value);
    m_lineSizeSpinBox->blockSignals(false);
    
    setEnabled(widgetEnabled);
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationLineSizeWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

