
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

#define __ANNOTATION_POLYGON_DECLARE__
#include "AnnotationPolygon.h"
#undef __ANNOTATION_POLYGON_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationPolygon
 * \brief An annotation poly line
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationPolygon::AnnotationPolygon(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationMultiCoordinateShape(AnnotationTypeEnum::POLYGON,
                                attributeDefaultType)
{
    initializeMembersAnnotationPolygon();
}

/**
 * Destructor.
 */
AnnotationPolygon::~AnnotationPolygon()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationPolygon::AnnotationPolygon(const AnnotationPolygon& obj)
: AnnotationMultiCoordinateShape(obj)
{
    this->initializeMembersAnnotationPolygon();
    this->copyHelperAnnotationPolygon(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationPolygon&
AnnotationPolygon::operator=(const AnnotationPolygon& obj)
{
    if (this != &obj) {
        AnnotationMultiCoordinateShape::operator=(obj);
        this->copyHelperAnnotationPolygon(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationPolygon::copyHelperAnnotationPolygon(const AnnotationPolygon& obj)
{
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationPolygon::initializeMembersAnnotationPolygon()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
}

/**
 * @return Cast to polygon (NULL if NOT polygon)
 */
AnnotationPolygon*
AnnotationPolygon::castToPolygon()
{
    return this;
}

/**
 * @return Cast to polygon (NULL if NOT polygon) const method
 */
const AnnotationPolygon*
AnnotationPolygon::castToPolygon() const
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
AnnotationPolygon::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
AnnotationPolygon::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationMultiCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                  sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}
