
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

#define __ANNOTATION_BOX_DECLARE__
#include "AnnotationBox.h"
#undef __ANNOTATION_BOX_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationBox 
 * \brief An annotation box.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationBox::AnnotationBox(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationTwoDimensionalShape(AnnotationTypeEnum::BOX,
                                attributeDefaultType)
{
    initializeMembersAnnotationBox();
}

/**
 * Destructor.
 */
AnnotationBox::~AnnotationBox()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationBox::AnnotationBox(const AnnotationBox& obj)
: AnnotationTwoDimensionalShape(obj)
{
    this->initializeMembersAnnotationBox();
    this->copyHelperAnnotationBox(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationBox&
AnnotationBox::operator=(const AnnotationBox& obj)
{
    if (this != &obj) {
        AnnotationTwoDimensionalShape::operator=(obj);
        this->copyHelperAnnotationBox(obj);
    }
    return *this;
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationBox::initializeMembersAnnotationBox()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
        
    }
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationBox::copyHelperAnnotationBox(const AnnotationBox& /*obj*/)
{
    /* nothing to copy here */
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
AnnotationBox::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::saveSubClassDataToScene(sceneAttributes,
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
AnnotationBox::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

