
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

#define __ANNOTATION_MARKER_DECLARE__
#include "AnnotationMarker.h"
#undef __ANNOTATION_MARKER_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMarker 
 * \brief An annotation box.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationMarker::AnnotationMarker(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationOneCoordinateShape(AnnotationTypeEnum::MARKER,
                                attributeDefaultType)
{
    initializeMembersAnnotationMarker();
}

/**
 * Destructor.
 */
AnnotationMarker::~AnnotationMarker()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationMarker::AnnotationMarker(const AnnotationMarker& obj)
: AnnotationOneCoordinateShape(obj)
{
    this->initializeMembersAnnotationMarker();
    this->copyHelperAnnotationMarker(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationMarker&
AnnotationMarker::operator=(const AnnotationMarker& obj)
{
    if (this != &obj) {
        AnnotationOneCoordinateShape::operator=(obj);
        this->copyHelperAnnotationMarker(obj);
    }
    return *this;
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationMarker::initializeMembersAnnotationMarker()
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
AnnotationMarker::copyHelperAnnotationMarker(const AnnotationMarker& /*obj*/)
{
    /* nothing to copy here */
}

/**
 * @return Marker is fixed aspect (same size horizontally and vertically).
 */
bool
AnnotationMarker::isFixedAspectRatio() const
{
    return true;
}

/**
 * @return The aspect ratio for annotations that have a fixed aspect ratio.
 * This method may be overridden by annotations that require a fixed aspect ratio
 * (such as an image annotaiton).
 *
 * Marker is same pixed dimensions horizontally and vertically.
 */
float
AnnotationMarker::getFixedAspectRatio() const
{
    return 1.0;
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
AnnotationMarker::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
AnnotationMarker::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    AnnotationOneCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                                sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

