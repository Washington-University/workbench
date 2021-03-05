
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

#define __ANNOTATION_POLY_LINE_DECLARE__
#include "AnnotationPolyLine.h"
#undef __ANNOTATION_POLY_LINE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationPolyLine
 * \brief An annotation poly line
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationPolyLine::AnnotationPolyLine(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationMultiCoordinateShape(AnnotationTypeEnum::POLYLINE,
                                attributeDefaultType)
{
    initializeMembersAnnotationPolyLine();
}

/**
 * Destructor.
 */
AnnotationPolyLine::~AnnotationPolyLine()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationPolyLine::AnnotationPolyLine(const AnnotationPolyLine& obj)
: AnnotationMultiCoordinateShape(obj)
{
    this->initializeMembersAnnotationPolyLine();
    this->copyHelperAnnotationPolyLine(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationPolyLine&
AnnotationPolyLine::operator=(const AnnotationPolyLine& obj)
{
    if (this != &obj) {
        AnnotationMultiCoordinateShape::operator=(obj);
        this->copyHelperAnnotationPolyLine(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationPolyLine::copyHelperAnnotationPolyLine(const AnnotationPolyLine& /*obj*/)
{
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationPolyLine::initializeMembersAnnotationPolyLine()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
}

/**
 * @return Cast to polyline (NULL if NOT polyline)
 */
AnnotationPolyLine*
AnnotationPolyLine::castToPolyline()
{
    return this;
}

/**
 * @return Cast to polyline (NULL if NOT polyline) const method
 */
const AnnotationPolyLine*
AnnotationPolyLine::castToPolyline() const
{
    return this;
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
AnnotationPolyLine::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationMultiCoordinateShape::saveSubClassDataToScene(sceneAttributes,
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
AnnotationPolyLine::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationMultiCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                  sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}
