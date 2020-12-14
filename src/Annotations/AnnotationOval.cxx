
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

#define __ANNOTATION_OVAL_DECLARE__
#include "AnnotationOval.h"
#undef __ANNOTATION_OVAL_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationOval 
 * \brief An annotation oval.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationOval::AnnotationOval(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationOneCoordinateShape(AnnotationTypeEnum::OVAL,
                                attributeDefaultType)
{
    initializeMembersAnnotationOval();
}

/**
 * Destructor.
 */
AnnotationOval::~AnnotationOval()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationOval::AnnotationOval(const AnnotationOval& obj)
: AnnotationOneCoordinateShape(obj)
{
    this->initializeMembersAnnotationOval();
    this->copyHelperAnnotationOval(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationOval&
AnnotationOval::operator=(const AnnotationOval& obj)
{
    if (this != &obj) {
        AnnotationOneCoordinateShape::operator=(obj);
        this->copyHelperAnnotationOval(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationOval::copyHelperAnnotationOval(const AnnotationOval& /*obj*/)
{
    /* nothing to copy */
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationOval::initializeMembersAnnotationOval()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
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
AnnotationOval::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationOneCoordinateShape::saveSubClassDataToScene(sceneAttributes,
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
AnnotationOval::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationOneCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

