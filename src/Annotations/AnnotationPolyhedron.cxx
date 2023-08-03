
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

#define __ANNOTATION_POLYHEDRON_DECLARE__
#include "AnnotationPolyhedron.h"
#undef __ANNOTATION_POLYHEDRON_DECLARE__

#include "AnnotationCoordinate.h"
#include "CaretAssert.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationPolyhedron
 * \brief An annotation poly line
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationPolyhedron::AnnotationPolyhedron(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationMultiPairedCoordinateShape(AnnotationTypeEnum::POLYHEDRON,
                                       attributeDefaultType)
{
    initializeMembersAnnotationPolyhedron();
}

/**
 * Destructor.
 */
AnnotationPolyhedron::~AnnotationPolyhedron()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationPolyhedron::AnnotationPolyhedron(const AnnotationPolyhedron& obj)
: AnnotationMultiPairedCoordinateShape(obj)
{
    this->initializeMembersAnnotationPolyhedron();
    this->copyHelperAnnotationPolyhedron(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationPolyhedron&
AnnotationPolyhedron::operator=(const AnnotationPolyhedron& obj)
{
    if (this != &obj) {
        AnnotationMultiPairedCoordinateShape::operator=(obj);
        this->copyHelperAnnotationPolyhedron(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationPolyhedron::copyHelperAnnotationPolyhedron(const AnnotationPolyhedron& /*obj*/)
{
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationPolyhedron::initializeMembersAnnotationPolyhedron()
{
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
}

/**
 * @return Cast to polyhedron (NULL if NOT polygon)
 */
AnnotationPolyhedron*
AnnotationPolyhedron::castToPolyhedron()
{
    return this;
}

/**
 * @return Cast to polyhedron (NULL if NOT polygon) const method
 */
const AnnotationPolyhedron*
AnnotationPolyhedron::castToPolyhedron() const
{
    return this;
}

/**
 * Finish creation of a polyhedron using the given depth
 * @param plane
 *    Plane of slice
 * @param polyhedronDepth
 *    Depth of polyhedron
 * @param errorMessageOut
 *    Error information output
 * @return True if successful, else false and see errorMessageOut
 */
bool
AnnotationPolyhedron::finishNewPolyhedron(const Plane& plane,
                                          const float polyhedronDepth,
                                          AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const int32_t numCoords(getNumberOfCoordinates());
    if (numCoords < 3) {
        errorMessageOut = ("Polyhedron must contain at least three coordinates.");
        return false;
    }
    
    if ( ! plane.isValidPlane()) {
        errorMessageOut = ("Plane for volume slice is invalid.");
        return false;
    }
    
    const bool debugFlag(false);
    
    const Vector3D normalVector(-plane.getNormalVector());
    if (debugFlag) std::cout << "Normal Vector: " << normalVector.toString() << std::endl;
    const Vector3D offsetVector(normalVector * polyhedronDepth);
    for (int32_t i = 0; i < numCoords; i++) {
        AnnotationCoordinate* newCoord(new AnnotationCoordinate(*getCoordinate(i)));
        Vector3D xyz;
        newCoord->getXYZ(xyz);
        if (debugFlag) std::cout << "   Orig: " << xyz.toString();
        xyz += offsetVector;
        newCoord->setXYZ(xyz);
        if (debugFlag) std::cout << ", New: " << xyz.toString() << std::endl;
        addCoordinate(newCoord);
    }
    
    if (debugFlag) {
        std::cout << "Created annotation: " << this->toString() << std::endl;
        const int32_t n(getNumberOfCoordinates());
        for (int32_t i = 0; i < n; i++) {
            Vector3D xyz;
            getCoordinate(i)->getXYZ(xyz);
            std::cout << "   " << i << ": " << xyz.toString() << std::endl;
        }
    }
    
    setDrawingNewAnnotationStatus(false);
    
    return true;
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
AnnotationPolyhedron::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationMultiPairedCoordinateShape::saveSubClassDataToScene(sceneAttributes,
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
AnnotationPolyhedron::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                   const SceneClass* sceneClass)
{
    AnnotationMultiPairedCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                  sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}
