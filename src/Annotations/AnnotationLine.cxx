
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

#define __ANNOTATION_LINE_DECLARE__
#include "AnnotationLine.h"
#undef __ANNOTATION_LINE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationLine 
 * \brief An annotation line.
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
AnnotationLine::AnnotationLine()
: AnnotationOneDimensionalShape(AnnotationTypeEnum::LINE)
{
    m_displayStartArrow = false;
    m_displayEndArrow = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_displayStartArrow",
                          &m_displayStartArrow);
    m_sceneAssistant->add("m_displayEndArrow",
                          &m_displayEndArrow);
}

/**
 * Destructor.
 */
AnnotationLine::~AnnotationLine()
{
    delete m_sceneAssistant;
}

/**
 * @return Is the arrow at the line's start coordinate displayed?
 */
bool
AnnotationLine::isDisplayStartArrow() const
{
    return m_displayStartArrow;
}

/**
 * Set the display status of the arrow at the line's start coordinate.
 *
 * @param displayArrow
 *     New status.
 */
void
AnnotationLine::setDisplayStartArrow(const bool displayArrow)
{
    m_displayStartArrow = displayArrow;
}

/**
 * @return Is the arrow at the line's start coordinate displayed?
 */
bool
AnnotationLine::isDisplayEndArrow() const
{
    return m_displayEndArrow;
}

/**
 * Set the display status of the arrow at the line's start coordinate.
 *
 * @param displayArrow
 *     New status.
 */
void
AnnotationLine::setDisplayEndArrow(const bool displayArrow)
{
    m_displayEndArrow = displayArrow;
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
AnnotationLine::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationOneDimensionalShape::saveSubClassDataToScene(sceneAttributes,
                                                           sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
AnnotationLine::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationOneDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
                                                  sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

