
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

#define __ANNOTATION_ONE_DIMENSIONAL_SHAPE_DECLARE__
#include "AnnotationOneDimensionalShape.h"
#undef __ANNOTATION_ONE_DIMENSIONAL_SHAPE_DECLARE__

#include "AnnotationCoordinate.h"
#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationOneDimensionalShape 
 * \brief Class for annotations that are one-dimensional (lines)
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
AnnotationOneDimensionalShape::AnnotationOneDimensionalShape(const AnnotationTypeEnum::Enum type)
: Annotation(type)
{
    initializeMembersAnnotationOneDimensionalShape();
}

/**
 * Destructor.
 */
AnnotationOneDimensionalShape::~AnnotationOneDimensionalShape()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationOneDimensionalShape::AnnotationOneDimensionalShape(const AnnotationOneDimensionalShape& obj)
: Annotation(obj)
{
    initializeMembersAnnotationOneDimensionalShape();
    
    this->copyHelperAnnotationOneDimensionalShape(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationOneDimensionalShape&
AnnotationOneDimensionalShape::operator=(const AnnotationOneDimensionalShape& obj)
{
    if (this != &obj) {
        Annotation::operator=(obj);
        this->copyHelperAnnotationOneDimensionalShape(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationOneDimensionalShape::copyHelperAnnotationOneDimensionalShape(const AnnotationOneDimensionalShape& obj)
{
    *m_startCoordinate = *obj.m_startCoordinate;
    *m_endCoordinate   = *obj.m_endCoordinate;
}

/**
 * Initialize members of this class.
 */
void
AnnotationOneDimensionalShape::initializeMembersAnnotationOneDimensionalShape()
{
    m_startCoordinate.grabNew(new AnnotationCoordinate());
    m_endCoordinate.grabNew(new AnnotationCoordinate());
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add("m_startCoordinate",
                          "AnnotationCoordinate",
                          m_startCoordinate);
    m_sceneAssistant->add("m_endCoordinate",
                          "AnnotationCoordinate",
                          m_endCoordinate);
}

/**
 * @return The start coordinate for the one dimensional shape.
 */
AnnotationCoordinate*
AnnotationOneDimensionalShape::getStartCoordinate()
{
    return m_startCoordinate;
}

/**
 * @return The start coordinate for the one dimensional shape.
 */
const AnnotationCoordinate*
AnnotationOneDimensionalShape::getStartCoordinate() const
{
    return m_startCoordinate;
}

/**
 * @return The end coordinate for the one dimensional shape.
 */
AnnotationCoordinate*
AnnotationOneDimensionalShape::getEndCoordinate()
{
    return m_endCoordinate;
}

/**
 * @return The end coordinate for the one dimensional shape.
 */
const AnnotationCoordinate*
AnnotationOneDimensionalShape::getEndCoordinate() const
{
    return m_endCoordinate;
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool
AnnotationOneDimensionalShape::isModified() const
{
    if (Annotation::isModified()) {
        return true;
    }
    
    if (m_startCoordinate->isModified()) {
        return true;
    }
    
    if (m_endCoordinate->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * Set the status to unmodified.
 */
void
AnnotationOneDimensionalShape::clearModified()
{
    Annotation::clearModified();
    
    m_startCoordinate->clearModified();
    m_endCoordinate->clearModified();
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
AnnotationOneDimensionalShape::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
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
AnnotationOneDimensionalShape::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

