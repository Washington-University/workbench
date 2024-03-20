
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

#define __ANNOTATION_TWO_COORDINATE_SHAPE_DECLARE__
#include "AnnotationTwoCoordinateShape.h"
#undef __ANNOTATION_TWO_COORDINATE_SHAPE_DECLARE__

#include <cmath>

#include "AnnotationCoordinate.h"
#include "AnnotationSpatialModification.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTwoCoordinateShape 
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
AnnotationTwoCoordinateShape::AnnotationTwoCoordinateShape(const AnnotationTypeEnum::Enum type,
                                                             const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: Annotation(type,
             attributeDefaultType)
{
    initializeMembersAnnotationTwoCoordinateShape();
}

/**
 * Destructor.
 */
AnnotationTwoCoordinateShape::~AnnotationTwoCoordinateShape()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationTwoCoordinateShape::AnnotationTwoCoordinateShape(const AnnotationTwoCoordinateShape& obj)
: Annotation(obj)
{
    initializeMembersAnnotationTwoCoordinateShape();
    
    this->copyHelperAnnotationTwoCoordinateShape(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationTwoCoordinateShape&
AnnotationTwoCoordinateShape::operator=(const AnnotationTwoCoordinateShape& obj)
{
    if (this != &obj) {
        Annotation::operator=(obj);
        this->copyHelperAnnotationTwoCoordinateShape(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationTwoCoordinateShape::copyHelperAnnotationTwoCoordinateShape(const AnnotationTwoCoordinateShape& obj)
{
    *m_startCoordinate = *obj.m_startCoordinate;
    *m_endCoordinate   = *obj.m_endCoordinate;
}

/**
 * Initialize members of this class.
 */
void
AnnotationTwoCoordinateShape::initializeMembersAnnotationTwoCoordinateShape()
{
    m_startCoordinate.grabNew(new AnnotationCoordinate(m_attributeDefaultType));
    m_endCoordinate.grabNew(new AnnotationCoordinate(m_attributeDefaultType));
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
        m_sceneAssistant->add("m_startCoordinate",
                              "AnnotationCoordinate",
                              m_startCoordinate);
        m_sceneAssistant->add("m_endCoordinate",
                              "AnnotationCoordinate",
                              m_endCoordinate);
    }
}

/**
 * @return 'this' as a one-dimensional shape. NULL if this is not a one-dimensional shape.
 */
AnnotationTwoCoordinateShape*
AnnotationTwoCoordinateShape::castToTwoCoordinateShape()
{
    return this;
}

/**
 * @return 'this' as a one-dimensional shape. NULL if this is not a one-dimensional shape.
 */
const AnnotationTwoCoordinateShape*
AnnotationTwoCoordinateShape::castToTwoCoordinateShape() const
{
    return this;
}

/**
 * @return The start coordinate for the one dimensional shape.
 */
AnnotationCoordinate*
AnnotationTwoCoordinateShape::getStartCoordinate()
{
    return m_startCoordinate;
}

/**
 * @return The start coordinate for the one dimensional shape.
 */
const AnnotationCoordinate*
AnnotationTwoCoordinateShape::getStartCoordinate() const
{
    return m_startCoordinate;
}

/**
 * @return The end coordinate for the one dimensional shape.
 */
AnnotationCoordinate*
AnnotationTwoCoordinateShape::getEndCoordinate()
{
    return m_endCoordinate;
}

/**
 * @return The end coordinate for the one dimensional shape.
 */
const AnnotationCoordinate*
AnnotationTwoCoordinateShape::getEndCoordinate() const
{
    return m_endCoordinate;
}

/**
 * @return Number of coordinates in this annotation
 */
int32_t
AnnotationTwoCoordinateShape::getNumberOfCoordinates() const
{
    return 2;
}

/**
 * @return Coordinate at the given index (NULL if index invalid)
 * @param index
 *    Inde of the coordinate
 */
AnnotationCoordinate*
AnnotationTwoCoordinateShape::getCoordinate(const int32_t index)
{
    if (index == 0) {
        return getStartCoordinate();
    }
    else if (index == 1) {
        return getEndCoordinate();
    }
    
    CaretAssertMessage(0, "Index must be 0 or 1");
    return NULL;
}

/**
 * @return Coordinate at the given index
 * @param index
 *    Inde of the coordinate
 */
const AnnotationCoordinate*
AnnotationTwoCoordinateShape::getCoordinate(const int32_t index) const
{
    if (index == 0) {
        return getStartCoordinate();
    }
    else if (index == 1) {
        return getEndCoordinate();
    }
    
    CaretAssertMessage(0, "Index must be 0 or 1");
    return NULL;
}

/**
 * Replace all coordinates in this annotation with copies of the given coordinates
 * @param coordinates
 *    Coordinates that are copied into this annotation
 */
void
AnnotationTwoCoordinateShape::replaceAllCoordinates(const std::vector<std::unique_ptr<const AnnotationCoordinate>>& coordinates)
{
    if (coordinates.size() == 2) {
        CaretAssertVectorIndex(coordinates, 1);
        *m_startCoordinate = *coordinates[0];
        *m_endCoordinate   = *coordinates[1];
    }
    else {
        const QString msg("Replacing two coordinate annotation; coordinates wrong size="
                          + AString::number(coordinates.size()));
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
}

/**
 * @return The surface offset vector type for this annotation.
 */
AnnotationSurfaceOffsetVectorTypeEnum::Enum
AnnotationTwoCoordinateShape::getSurfaceOffsetVectorType() const
{
    CaretAssert(m_startCoordinate);
    return m_startCoordinate->getSurfaceOffsetVectorType();
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool
AnnotationTwoCoordinateShape::isModified() const
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
AnnotationTwoCoordinateShape::clearModified()
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
AnnotationTwoCoordinateShape::applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation)
{
    CaretAssert(otherAnnotation);
    const AnnotationTwoCoordinateShape* otherOneDim = dynamic_cast<const AnnotationTwoCoordinateShape*>(otherAnnotation);
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
AnnotationTwoCoordinateShape::getRotationAngle(const float viewportWidth,
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
AnnotationTwoCoordinateShape::setRotationAngle(const float viewportWidth,
                                                const float viewportHeight,
                                                const float rotationAngle)
{
    if ( ! isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        return;
    }

    float annOneX = 0.0;
    float annOneY = 0.0;
    float annTwoX = 0.0;
    float annTwoY = 0.0;
    getStartCoordinate()->getViewportXY(viewportWidth, viewportHeight, annOneX, annOneY);
    getEndCoordinate()->getViewportXY(viewportWidth, viewportHeight, annTwoX, annTwoY);
    
    const float midPointXYZ[3] = {
        (annOneX + annTwoX) / 2.0f,
        (annOneY + annTwoY) / 2.0f,
        0.0f
    };
    
    const float vpOneXYZ[3] = { annOneX, annOneY, 0.0f };
    const float lengthMidToOne = MathFunctions::distance3D(midPointXYZ, vpOneXYZ);
    const float newRotationAngle = 180.0f - rotationAngle;
    
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
AnnotationTwoCoordinateShape::isSizeHandleValid(const AnnotationSizingHandleTypeEnum::Enum sizingHandle) const
{
    bool xyPlaneFlag = false;
    
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            xyPlaneFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            xyPlaneFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            xyPlaneFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            xyPlaneFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            xyPlaneFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            xyPlaneFlag = true;
            break;
    }
    
    bool validFlag = false;
    
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
            if (xyPlaneFlag) {
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            if (xyPlaneFlag) {
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
            break;
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
AnnotationTwoCoordinateShape::applySpatialModificationSurfaceSpace(const AnnotationSpatialModification& spatialModification)
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
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
            break;
    }
    
    
    if (validFlag) {
        setModified();
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation in spacer tab space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationTwoCoordinateShape::applySpatialModificationSpacerTabSpace(const AnnotationSpatialModification& spatialModification)
{
    return applySpatialModificationTabOrWindowSpace(spatialModification);
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
AnnotationTwoCoordinateShape::applySpatialModificationTabOrWindowSpace(const AnnotationSpatialModification& spatialModification)
{
    float xyz1[3];
    float xyz2[3];
    m_startCoordinate->getXYZ(xyz1);
    m_endCoordinate->getXYZ(xyz2);
    
    float newX1 = xyz1[0];
    float newY1 = xyz1[1];
    float newX2 = xyz2[0];
    float newY2 = xyz2[1];
    
    const float spaceDX = 100.0f * ((spatialModification.m_viewportWidth != 0.0f)
                                   ? (spatialModification.m_mouseDX / spatialModification.m_viewportWidth)
                                   : 0.0f);
    const float spaceDY = 100.0f * ((spatialModification.m_viewportHeight != 0.0f)
                                   ? (spatialModification.m_mouseDY / spatialModification.m_viewportHeight)
                                   : 0.0f);
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
            float vpOneXYZ[3] = { 0.0, 0.0, 0.0 };
            relativeXYZToViewportXYZ(xyz1,
                                     spatialModification.m_viewportWidth,
                                     spatialModification.m_viewportHeight,
                                     vpOneXYZ);
            float vpTwoXYZ[3] = { 0.0, 0.0, 0.0 };
            relativeXYZToViewportXYZ(xyz2,
                                     spatialModification.m_viewportWidth,
                                     spatialModification.m_viewportHeight,
                                     vpTwoXYZ);
            
            float vpXYZ[3] = {
                (vpOneXYZ[0] + vpTwoXYZ[0]) / 2.0f,
                (vpOneXYZ[1] + vpTwoXYZ[1]) / 2.0f,
                (vpOneXYZ[2] + vpTwoXYZ[2]) / 2.0f
            };
            
            /*
             * Rotation angle is a formed by the triangle
             * (Mouse XY, Annotation XY, Positive X-axis).
             */
            const float dy = spatialModification.m_mouseY - vpXYZ[1];
            const float dx = spatialModification.m_mouseX - vpXYZ[0];
            
            const float angleRadians = std::atan2(dy, dx);
            const float angleDegrees = MathFunctions::toDegrees(angleRadians);
            float rotationAngle = -angleDegrees;
            if (rotationAngle > 360.0) {
                rotationAngle -= 360.0;
            }
            else if (rotationAngle < 0.0) {
                rotationAngle += 360.0;
            }
            
            CaretPointer<AnnotationTwoCoordinateShape> shapeCopy(dynamic_cast<AnnotationTwoCoordinateShape*>(this->clone()));
            shapeCopy->setRotationAngle(spatialModification.m_viewportWidth,
                                        spatialModification.m_viewportHeight,
                                        rotationAngle);
            
            const Vector3D xyzOne = shapeCopy->getStartCoordinate()->getXYZ();
            const Vector3D xyzTwo = shapeCopy->getEndCoordinate()->getXYZ();
            newX1 = xyzOne[0];
            newY1 = xyzOne[1];
            newX2 = xyzTwo[0];
            newY2 = xyzTwo[1];
            validFlag = true;
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
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
 * Apply a spatial modification to an annotation in chart space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationTwoCoordinateShape::applySpatialModificationChartSpace(const AnnotationSpatialModification& spatialModification)
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
            if (spatialModification.m_chartCoordAtMouseXY.m_chartXYZValid) {
                m_endCoordinate->setXYZ(spatialModification.m_chartCoordAtMouseXY.m_chartXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            if (spatialModification.m_chartCoordAtMouseXY.m_chartXYZValid) {
                m_startCoordinate->setXYZ(spatialModification.m_chartCoordAtMouseXY.m_chartXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            if (spatialModification.m_chartCoordAtMouseXY.m_chartXYZValid
                && spatialModification.m_chartCoordAtPreviousMouseXY.m_chartXYZValid) {
                const float dx = spatialModification.m_chartCoordAtMouseXY.m_chartXYZ[0] - spatialModification.m_chartCoordAtPreviousMouseXY.m_chartXYZ[0];
                const float dy = spatialModification.m_chartCoordAtMouseXY.m_chartXYZ[1] - spatialModification.m_chartCoordAtPreviousMouseXY.m_chartXYZ[1];
                const float dz = spatialModification.m_chartCoordAtMouseXY.m_chartXYZ[2] - spatialModification.m_chartCoordAtPreviousMouseXY.m_chartXYZ[2];
                
                m_startCoordinate->addToXYZ(dx, dy, dz);
                m_endCoordinate->addToXYZ(dx, dy, dz);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
            break;
    }
    
    if (validFlag) {
        setModified();
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation in histology space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationTwoCoordinateShape::applySpatialModificationHistologySpace(const AnnotationSpatialModification& spatialModification)
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
            if (spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZValid) {
                m_endCoordinate->setXYZ(spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            if (spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZValid) {
                m_startCoordinate->setXYZ(spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            if (spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZValid
                && spatialModification.m_histologyCoordAtPreviousMouseXY.m_histologyXYZValid) {
                const float dx = spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZ[0] - spatialModification.m_histologyCoordAtPreviousMouseXY.m_histologyXYZ[0];
                const float dy = spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZ[1] - spatialModification.m_histologyCoordAtPreviousMouseXY.m_histologyXYZ[1];
                const float dz = spatialModification.m_histologyCoordAtMouseXY.m_histologyXYZ[2] - spatialModification.m_histologyCoordAtPreviousMouseXY.m_histologyXYZ[2];
                
                m_startCoordinate->addToXYZ(dx, dy, dz);
                m_endCoordinate->addToXYZ(dx, dy, dz);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
            break;
    }
    
    if (validFlag) {
        setModified();
    }
    
    return validFlag;
}

/**
 * Apply a spatial modification to an annotation in chart space.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 * @return
 *     True if the annotation was modified, else false.
 */
bool
AnnotationTwoCoordinateShape::applySpatialModificationMediaSpace(const AnnotationSpatialModification& spatialModification)
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
            if (spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZValid) {
                m_endCoordinate->setXYZ(spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            if (spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZValid) {
                m_startCoordinate->setXYZ(spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZ);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            if (spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZValid
                && spatialModification.m_mediaCoordAtPreviousMouseXY.m_mediaXYZValid) {
                const float dx = spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZ[0] - spatialModification.m_mediaCoordAtPreviousMouseXY.m_mediaXYZ[0];
                const float dy = spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZ[1] - spatialModification.m_mediaCoordAtPreviousMouseXY.m_mediaXYZ[1];
                const float dz = spatialModification.m_mediaCoordAtMouseXY.m_mediaXYZ[2] - spatialModification.m_mediaCoordAtPreviousMouseXY.m_mediaXYZ[2];
                
                m_startCoordinate->addToXYZ(dx, dy, dz);
                m_endCoordinate->addToXYZ(dx, dy, dz);
                validFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
            break;
    }
    
    if (validFlag) {
        setModified();
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
AnnotationTwoCoordinateShape::applySpatialModificationStereotaxicSpace(const AnnotationSpatialModification& spatialModification)
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
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_ONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_POLYHEDRON_TEXT_COORDINATE_TWO:
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
AnnotationTwoCoordinateShape::applySpatialModification(const AnnotationSpatialModification& spatialModification)
{
    if ( ! isSizeHandleValid(spatialModification.m_sizingHandleType)) {
        return false;
    }
    
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            return applySpatialModificationChartSpace(spatialModification);
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            return applySpatialModificationHistologySpace(spatialModification);
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            return applySpatialModificationMediaSpace(spatialModification);
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            return applySpatialModificationSpacerTabSpace(spatialModification);
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
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
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
AnnotationTwoCoordinateShape::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
AnnotationTwoCoordinateShape::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

