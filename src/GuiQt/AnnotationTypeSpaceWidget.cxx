
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

#define __ANNOTATION_TYPE_SPACE_WIDGET_DECLARE__
#include "AnnotationTypeSpaceWidget.h"
#undef __ANNOTATION_TYPE_SPACE_WIDGET_DECLARE__

#include <QGridLayout>
#include <QLabel>

#include "Annotation.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTypeSpaceWidget 
 * \brief Selects annotation's type and space.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationTypeSpaceWidget::AnnotationTypeSpaceWidget(QWidget* parent)
: QWidget(parent)
{
    m_annotation = NULL;
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> coordSpaces;
    coordSpaces.push_back(AnnotationCoordinateSpaceEnum::MODEL);
    coordSpaces.push_back(AnnotationCoordinateSpaceEnum::SURFACE);
    coordSpaces.push_back(AnnotationCoordinateSpaceEnum::TAB);
    coordSpaces.push_back(AnnotationCoordinateSpaceEnum::WINDOW);
    
    QLabel* spaceLabel = new QLabel("Space:");
    m_coordinateSpaceComboBox = new EnumComboBoxTemplate(this);
    m_coordinateSpaceComboBox->setupWithItems<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(coordSpaces);
    
    QObject::connect(m_coordinateSpaceComboBox, SIGNAL(itemActivated()),
                     this, SLOT(coordinateSpaceEnumChanged()));
    
    QLabel* typeLabel = new QLabel("Type:");
    m_typeComboBox = new EnumComboBoxTemplate(this);
    m_typeComboBox->setup<AnnotationTypeEnum,AnnotationTypeEnum::Enum>();
    
    QObject::connect(m_typeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(typeEnumChanged()));

    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(typeLabel, 0, 0);
    layout->addWidget(m_typeComboBox->getWidget(), 0, 1);
    layout->addWidget(spaceLabel, 1, 0);
    layout->addWidget(m_coordinateSpaceComboBox->getWidget(), 1, 1);
    

    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
AnnotationTypeSpaceWidget::~AnnotationTypeSpaceWidget()
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
AnnotationTypeSpaceWidget::receiveEvent(Event* event)
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
 * @param annotation.
 */
void
AnnotationTypeSpaceWidget::updateContent(Annotation* annotation)
{
    m_annotation = annotation;
    
    if (m_annotation != NULL) {
        m_coordinateSpaceComboBox->setSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(m_annotation->getCoordinateSpace());
        m_typeComboBox->setSelectedItem<AnnotationTypeEnum, AnnotationTypeEnum::Enum>(m_annotation->getType());
    }
}

/**
 * Gets called when the coordinate space is changed.
 */
void
AnnotationTypeSpaceWidget::coordinateSpaceEnumChanged()
{
    if (m_annotation != NULL) {
        m_annotation->setCoordinateSpace(m_coordinateSpaceComboBox->getSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>());
    }
}

/**
 * Gets called when the type is changed.
 */
void
AnnotationTypeSpaceWidget::typeEnumChanged()
{
    const AnnotationTypeEnum::Enum annType = m_typeComboBox->getSelectedItem<AnnotationTypeEnum,AnnotationTypeEnum::Enum>();
}


