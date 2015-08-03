
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
    initializeMembersAnnotationLine();
}

/**
 * Destructor.
 */
AnnotationLine::~AnnotationLine()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationLine::AnnotationLine(const AnnotationLine& obj)
: AnnotationOneDimensionalShape(obj)
{
    this->initializeMembersAnnotationLine();
    this->copyHelperAnnotationLine(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationLine&
AnnotationLine::operator=(const AnnotationLine& obj)
{
    if (this != &obj) {
        AnnotationOneDimensionalShape::operator=(obj);
        this->copyHelperAnnotationLine(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationLine::copyHelperAnnotationLine(const AnnotationLine& obj)
{
    m_displayEndArrow   = obj.m_displayEndArrow;
    m_displayStartArrow = obj.m_displayStartArrow;
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationLine::initializeMembersAnnotationLine()
{
    m_displayStartArrow = false;
    m_displayEndArrow = false;
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add("m_displayStartArrow",
                          &m_displayStartArrow);
    m_sceneAssistant->add("m_displayEndArrow",
                          &m_displayEndArrow);
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
 * @return Is background color supported?
 * Most annotations support a background color.
 * Annotations that do not support a background color
 * must override this method and return a value of false.
 */
bool
AnnotationLine::isBackgroundColorSupported() const
{
    return false;
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

