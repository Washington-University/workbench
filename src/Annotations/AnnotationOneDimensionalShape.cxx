
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

#include <cmath>

#include "AnnotationCoordinate.h"
#include "AnnotationSpatialModification.h"
#include "CaretAssert.h"
#include "MathFunctions.h"
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
 * @param type
 *    Type of annotation.
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationOneDimensionalShape::AnnotationOneDimensionalShape(const AnnotationTypeEnum::Enum type,
                                                             const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: Annotation(type,
             attributeDefaultType)
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
//    m_sceneAssistant->add("m_startCoordinate",
//                          "AnnotationCoordinate",
//                          m_startCoordinate);
//    m_sceneAssistant->add("m_endCoordinate",
//                          "AnnotationCoordinate",
//                          m_endCoordinate);
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
 * Apply the coordinates, size, and rotation from the given annotation
 * to this annotation.
 * 
 * @param otherAnnotation
 *     The other annotation from which attributes are obtained.
 */
void
AnnotationOneDimensionalShape::applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation)
{
    CaretAssert(otherAnnotation);
    const AnnotationOneDimensionalShape* otherOneDim = dynamic_cast<const AnnotationOneDimensionalShape*>(otherAnnotation);
    CaretAssert(otherOneDim);
    
    AnnotationCoordinate* startCoord = getStartCoordinate();
    const AnnotationCoordinate* otherStartCoord = otherOneDim->getStartCoordinate();
    *startCoord = *otherStartCoord;
    
    AnnotationCoordinate* endCoord = getEndCoordinate();
    const AnnotationCoordinate* otherEndCoord = otherOneDim->getEndCoordinate();
    *endCoord = *otherEndCoord;
    
    setCoordinateSpace(otherAnnotation->getCoordinateSpace());
    setTabIndex(otherAnnotation->getTabIndex());
    setWindowIndex(otherAnnotation->getWindowIndex());
}

/**
 * Get the rotation angle from the one-dimensional annotation.
 * 0 is horizontal.
 *
 * @param viewportWidth
 *     Width of viewport.
 * @param viewportHeight
 *     Height of viewport.
 * @return
 *     Rotation angle of the annotation.
 */
float
AnnotationOneDimensionalShape::getRotationAngle(const float viewportWidth,
                                                const float viewportHeight) const
{
    if ( ! isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        return 0.0;
    }
    
    float vpOneX = 0.0;
    float vpOneY = 0.0;
    float vpTwoX = 0.0;
    float vpTwoY = 0.0;
    getStartCoordinate()->getViewportXY(viewportWidth, viewportHeight, vpOneX, vpOneY);
    getEndCoordinate()->getViewportXY(viewportWidth, viewportHeight, vpTwoX, vpTwoY);
    
    const float dx = vpTwoX - vpOneX;
    const float dy = vpTwoY - vpOneY;
    
    float angle = 180.0 - MathFunctions::toDegrees(std::atan2(dy, dx));
    if (angle < 0.0) {
        angle = angle + 360.0;
    }
    else if (angle > 360.0) {
        angle = angle - 360.0;
    }
    return angle;
}

/**
 * Set the rotation angle from the one-dimensional annotation.
 * 0 is horizontal.
 *
 * @param viewportWidth
 *     Width of viewport.
 * @param viewportHeight
 *     Height of viewport.
 * @param rotationAngle
 *     Rotation angle for the annotation.
 */
void
AnnotationOneDimensionalShape::setRotationAngle(const float viewportWidth,
                                                const float viewportHeight,
                                                const float rotationAngle)
{
    if ( ! isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        return;
    }
//    bool allowRotationFlag = false;
//    switch (getCoordinateSpace()) {
//        case AnnotationCoordinateSpaceEnum::PIXELS:
//            break;
//        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
//            break;
//        case AnnotationCoordinateSpaceEnum::SURFACE:
//            break;
//        case AnnotationCoordinateSpaceEnum::TAB:
//            allowRotationFlag = true;
//            break;
//        case AnnotationCoordinateSpaceEnum::WINDOW:
//            allowRotationFlag = true;
//            break;
//    }
//    
//    if ( ! allowRotationFlag) {
//        return;
//    }
    
    float annOneX = 0.0;
    float annOneY = 0.0;
    float annTwoX = 0.0;
    float annTwoY = 0.0;
    getStartCoordinate()->getViewportXY(viewportWidth, viewportHeight, annOneX, annOneY);
    getEndCoordinate()->getViewportXY(viewportWidth, viewportHeight, annTwoX, annTwoY);
    
    const float midPointXYZ[3] = {
        (annOneX + annTwoX) / 2.0,
        (annOneY + annTwoY) / 2.0,
        0.0
    };
    
    const float vpOneXYZ[3] = { annOneX, annOneY, 0.0 };
    const float lengthMidToOne = MathFunctions::distance3D(midPointXYZ, vpOneXYZ);
    const float newRotationAngle = 180.0 - rotationAngle;
    
    const float angleRadians = MathFunctions::toRadians(newRotationAngle);
    const float dy = lengthMidToOne * std::sin(angleRadians);
    const float dx = lengthMidToOne * std::cos(angleRadians);
    annOneX = midPointXYZ[0] - dx;
    annOneY = midPointXYZ[1] - dy;
    
    annTwoX = midPointXYZ[0] + dx;
    annTwoY = midPointXYZ[1] + dy;
    
    getStartCoordinate()->setXYZFromViewportXYZ(viewportWidth, viewportHeight, annOneX, annOneY);
    getEndCoordinate()->setXYZFromViewportXYZ(viewportWidth, viewportHeight, annTwoX, annTwoY);
}

/**
 * Is the given sizing handle valid for this annotation?
 *
 * @sizingHandle
 *    The sizing handle.
 * @return
 *    True if sizing handle valid, else false.
 */
bool
AnnotationOneDimensionalShape::isSizeHandleValid(const AnnotationSizingHandleTypeEnum::Enum sizingHandle) const
{
    bool pixelsFlag      = false;
    bool tabWindowFlag   = false;
    bool stereotaxicFlag = false;
    bool surfaceFlag     = false;
    
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            pixelsFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            stereotaxicFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            surfaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            tabWindowFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            tabWindowFlag = true;
            break;
    }
    
    bool validFlag = false;
    
    if ( ! pixelsFlag) {
        switch (sizingHandle) {
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
                validFlag = true;
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
                validFlag = true;
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
                if (tabWindowFlag) {
                    validFlag = true;
                }
                break;
            case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
                if (tabWindowFlag) {
                    validFlag = true;
                }
                break;
        }
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation in surface space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationOneDimensionalShape::applySpatialModificationSurfaceSpace(const AnnotationSpatialModification& spatialModification)
{
    bool validFlag = false;
    
    switch (spatialModification.m_sizingHandleType) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfNodes  = -1;
            int32_t surfaceNodeIndex      = -1;
            
            m_endCoordinate->getSurfaceSpace(structure,
                                               surfaceNumberOfNodes,
                                               surfaceNodeIndex);
            if (spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNodeValid) {
                if ((spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceStructure == structure)
                    && (spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes == surfaceNumberOfNodes)) {
                    m_endCoordinate->setSurfaceSpace(spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceStructure,
                                                     spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes,
                                                     spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNodeIndex);
                    validFlag = true;
                }
            }
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfNodes  = -1;
            int32_t surfaceNodeIndex      = -1;
            
            m_startCoordinate->getSurfaceSpace(structure,
                                               surfaceNumberOfNodes,
                                               surfaceNodeIndex);
            if (spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNodeValid) {
                if ((spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceStructure == structure)
                    && (spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes == surfaceNumberOfNodes)) {
                    m_startCoordinate->setSurfaceSpace(spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceStructure,
                                                       spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes,
                                                       spatialModification.m_surfaceCoordinateAtMouseXY.m_surfaceNodeIndex);
                    validFlag = true;
                }
            }
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
    }
    
    
    if (validFlag) {
        setModified();
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation in tab or window space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationOneDimensionalShape::applySpatialModificationTabOrWindowSpace(const AnnotationSpatialModification& spatialModification)
{
    float xyz1[3];
    float xyz2[3];
    m_startCoordinate->getXYZ(xyz1);
    m_endCoordinate->getXYZ(xyz2);
    
    float newX1 = xyz1[0];
    float newY1 = xyz1[1];
    float newX2 = xyz2[0];
    float newY2 = xyz2[1];
    
    const float spaceDX = 100.0 * ((spatialModification.m_viewportWidth != 0.0)
                                   ? (spatialModification.m_mouseDX / spatialModification.m_viewportWidth)
                                   : 0.0);
    const float spaceDY = 100.0 * ((spatialModification.m_viewportHeight != 0.0)
                                   ? (spatialModification.m_mouseDY / spatialModification.m_viewportHeight)
                                   : 0.0);
    bool validFlag = false;
    switch (spatialModification.m_sizingHandleType) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
                newX2 += spaceDX;
                newY2 += spaceDY;
                validFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
                newX1 += spaceDX;
                newY1 += spaceDY;
                validFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
                newX1 += spaceDX;
                newY1 += spaceDY;
                newX2 += spaceDX;
                newY2 += spaceDY;
                validFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
        {
            CaretPointer<AnnotationOneDimensionalShape> shapeCopy(dynamic_cast<AnnotationOneDimensionalShape*>(this->clone()));
            const float oldRotationAngle = shapeCopy->getRotationAngle(spatialModification.m_viewportWidth,
                                                                      spatialModification.m_viewportHeight);
            
            /*
             * Determine rotation direction by mouse movement relative to where
             * mouse was first pressed.
             */
            const float previousMouseXY[3] = {
                spatialModification.m_mouseX - spatialModification.m_mouseDX,
                spatialModification.m_mouseY - spatialModification.m_mouseDY,
                0.0
            };
            const float currentMouseXY[3] = {
                spatialModification.m_mouseX,
                spatialModification.m_mouseY,
                0.0
            };
            const float pressXY[3] = {
                spatialModification.m_mousePressX,
                spatialModification.m_mousePressY,
                0.0
            };
            
            float normalVector[3];
            MathFunctions::normalVector(pressXY,
                                        currentMouseXY,
                                        previousMouseXY,
                                        normalVector);
            
            
            float delta = std::sqrt(spatialModification.m_mouseDX*spatialModification.m_mouseDX
                                    + spatialModification.m_mouseDY*spatialModification.m_mouseDY);
            if (normalVector[2] < 0.0) {
                delta = -delta;
            }
            
            float rotationAngle = oldRotationAngle + delta;
            
            if (rotationAngle != oldRotationAngle) {
                if (rotationAngle > 360.0) {
                    rotationAngle -= 360.0;
                }
                if (rotationAngle < 0.0) {
                    rotationAngle += 360.0;
                }
                
                shapeCopy->setRotationAngle(spatialModification.m_viewportWidth,
                                 spatialModification.m_viewportHeight,
                                 rotationAngle);
                
                const float* xyzOne = shapeCopy->getStartCoordinate()->getXYZ();
                const float* xyzTwo = shapeCopy->getEndCoordinate()->getXYZ();
                newX1 = xyzOne[0];
                newY1 = xyzOne[1];
                newX2 = xyzTwo[0];
                newY2 = xyzTwo[1];
                validFlag = true;
            }
        }
            break;
    }
    
    if (validFlag) {
        if ((newX1 >= 0.0)
            && (newX1 <= 100.0)
            && (newY1 >= 0.0)
            && (newY1 <= 100.0)
            && (newX2 >= 0.0)
            && (newX2 <= 100.0)
            && (newY2 >= 0.0)
            && (newY2 <= 100.0)) {
            xyz1[0] = newX1;
            xyz1[1] = newY1;
            m_startCoordinate->setXYZ(xyz1);
            xyz2[0] = newX2;
            xyz2[1] = newY2;
            m_endCoordinate->setXYZ(xyz2);
        }
        else {
            validFlag = false;
        }
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation in stereotaxic space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationOneDimensionalShape::applySpatialModificationStereotaxicSpace(const AnnotationSpatialModification& spatialModification)
{
    bool validFlag = false;
    
    switch (spatialModification.m_sizingHandleType) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
            if (spatialModification.m_stereotaxicCoordinateAtMouseXY.m_stereotaxicValid) {
                m_endCoordinate->setXYZ(spatialModification.m_stereotaxicCoordinateAtMouseXY.m_stereotaxicXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            if (spatialModification.m_stereotaxicCoordinateAtMouseXY.m_stereotaxicValid) {
                m_startCoordinate->setXYZ(spatialModification.m_stereotaxicCoordinateAtMouseXY.m_stereotaxicXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
    }
    
    if (validFlag) {
        setModified();
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationOneDimensionalShape::applySpatialModification(const AnnotationSpatialModification& spatialModification)
{
    if ( ! isSizeHandleValid(spatialModification.m_sizingHandleType)) {
        return false;
    }
    
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            return applySpatialModificationStereotaxicSpace(spatialModification);
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            return applySpatialModificationSurfaceSpace(spatialModification);
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            return applySpatialModificationTabOrWindowSpace(spatialModification);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            return applySpatialModificationTabOrWindowSpace(spatialModification);
            break;
    }
    
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

