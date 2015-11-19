
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

#include <cmath>

#define __ANNOTATION_TWO_DIMENSIONAL_SHAPE_DECLARE__
#include "AnnotationTwoDimensionalShape.h"
#undef __ANNOTATION_TWO_DIMENSIONAL_SHAPE_DECLARE__

#include "AnnotationCoordinate.h"
#include "AnnotationSpatialModification.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTwoDimensionalShape 
 * \brief Class for annotations that are two dimensional (width and height).
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param type
 *    Type of annotation
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationTwoDimensionalShape::AnnotationTwoDimensionalShape(const AnnotationTypeEnum::Enum type,
                                                             const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: Annotation(type,
             attributeDefaultType)
{
    initializeMembersAnnotationTwoDimensionalShape();
}

/**
 * Destructor.
 */
AnnotationTwoDimensionalShape::~AnnotationTwoDimensionalShape()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationTwoDimensionalShape::AnnotationTwoDimensionalShape(const AnnotationTwoDimensionalShape& obj)
: Annotation(obj)
{
    initializeMembersAnnotationTwoDimensionalShape();
    this->copyHelperAnnotationTwoDimensionalShape(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationTwoDimensionalShape&
AnnotationTwoDimensionalShape::operator=(const AnnotationTwoDimensionalShape& obj)
{
    if (this != &obj) {
        Annotation::operator=(obj);
        this->copyHelperAnnotationTwoDimensionalShape(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationTwoDimensionalShape::copyHelperAnnotationTwoDimensionalShape(const AnnotationTwoDimensionalShape& obj)
{
    *m_coordinate   = *obj.m_coordinate;
    m_width         = obj.m_width;
    m_height        = obj.m_height;
    m_rotationAngle = obj.m_rotationAngle;
}

/**
 * Initialize members of this class.
 */
void
AnnotationTwoDimensionalShape::initializeMembersAnnotationTwoDimensionalShape()
{
    m_coordinate.grabNew(new AnnotationCoordinate());

    switch (m_attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            m_width  = 25.0;
            m_height = 25.0;
            m_rotationAngle = 0.0;
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
            m_width  = s_userDefaultWidth;
            m_height = s_userDefaultHeight;
            m_rotationAngle = s_userDefaultRotationAngle;
            break;
    }
    
    
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add("m_coordinate",
                          "AnnotationCoordinate",
                          m_coordinate);
    m_sceneAssistant->add("m_width",
                          &m_width);
    m_sceneAssistant->add("m_height",
                          &m_height);
    m_sceneAssistant->add("m_rotationAngle",
                          &m_rotationAngle);
}

/**
 * @return The coordinate for the two dimensional shape.
 */
AnnotationCoordinate*
AnnotationTwoDimensionalShape::getCoordinate()
{
    return m_coordinate;
}

/**
 * @return The start coordinate for the two dimensional shape.
 */
const AnnotationCoordinate*
AnnotationTwoDimensionalShape::getCoordinate() const
{
    return m_coordinate;
}

/**
 * @return Height for "two-dimensional" annotations in percentage zero to one-hundred.
 */
float
AnnotationTwoDimensionalShape::getHeight() const
{
    return m_height;
}

/**
 * Set the height for "two-dimensional" annotations in percentage zero to one-hundred.
 *
 * @param height
 *    New value for height of the annotation.
 */
void
AnnotationTwoDimensionalShape::setHeight(const float height)
{
    if (height != m_height) {
        m_height = height;
        setModified();
    }
}

/**
 * @return Width for "two-dimensional" annotations in percentage zero to one-hundred.
 */
float
AnnotationTwoDimensionalShape::getWidth() const
{
    return m_width;
}

/**
 * Set the width for "two-dimensional" annotations in percentage zero to one-hundred.
 *
 * @param width
 *    New value for width of the annotation.
 */
void
AnnotationTwoDimensionalShape::setWidth(const float width)
{
    if (width != m_width) {
        m_width = width;
        setModified();
    }
}

/**
 * @return The rotation angle, in degrees, clockwise, from vertical at the top (12 o'clock).
 */
float
AnnotationTwoDimensionalShape::getRotationAngle() const
{
    return m_rotationAngle;
}

/**
 * The rotation angle, in degrees, clockwise, from vertical at the top (12 o'clock).
 *
 * @param rotationAngle
 *     New value rotation angle.
 */
void
AnnotationTwoDimensionalShape::setRotationAngle(const float rotationAngle)
{
    if (rotationAngle != m_rotationAngle) {
        m_rotationAngle = rotationAngle;
        setModified();
    }
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool
AnnotationTwoDimensionalShape::isModified() const
{
    if (Annotation::isModified()) {
        return true;
    }
    
    if (m_coordinate->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * Set the status to unmodified.
 */
void
AnnotationTwoDimensionalShape::clearModified()
{
    Annotation::clearModified();
    
    m_coordinate->clearModified();
}

/**
 * Apply the coordinates, size, and rotation from the given annotation
 * to this annotation.
 *
 * @param otherAnnotation
 *     The other annotation from which attributes are obtained.
 */
void
AnnotationTwoDimensionalShape::applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation)
{
    CaretAssert(otherAnnotation);
    const AnnotationTwoDimensionalShape* otherTwoDim = dynamic_cast<const AnnotationTwoDimensionalShape*>(otherAnnotation);
    CaretAssert(otherTwoDim);
    
    AnnotationCoordinate* coord = getCoordinate();
    const AnnotationCoordinate* otherCoord = otherTwoDim->getCoordinate();
    *coord = *otherCoord;
    
    setWidth(otherTwoDim->getWidth());
    setHeight(otherTwoDim->getHeight());
    setRotationAngle(otherTwoDim->getRotationAngle());
    
    setCoordinateSpace(otherAnnotation->getCoordinateSpace());
    setTabIndex(otherAnnotation->getTabIndex());
    setWindowIndex(otherAnnotation->getWindowIndex());
}

/**
 * Apply a spatial modification to an annotation.
 *
 * @param spatialModification
 *     Contains information about the spatial modification.
 */
void
AnnotationTwoDimensionalShape::applySpatialModification(const AnnotationSpatialModification& spatialModification)
{
    float xyz[3];
    m_coordinate->getXYZ(xyz);
    
    float viewportXYZ[3] = {
        (xyz[0] / 100.0) * spatialModification.m_viewportWidth,
        (xyz[1] / 100.0) * spatialModification.m_viewportHeight,
        xyz[2]
    };
    
    float bottomLeftXYZ[3];
    float bottomRightXYZ[3];
    float topLeftXYZ[3];
    float topRightXYZ[3];
    const bool validBounds = getShapeBounds(spatialModification.m_viewportWidth,
                                            spatialModification.m_viewportHeight,
                                            viewportXYZ,
                                            bottomLeftXYZ,
                                            bottomRightXYZ,
                                            topRightXYZ,
                                            topLeftXYZ);
    if ( ! validBounds) {
        CaretAssert(0);
        return;
    }
    
    float leftToRightUnitVector[3];
    MathFunctions::createUnitVector(bottomLeftXYZ, bottomRightXYZ, leftToRightUnitVector);
    float bottomToTopUnitVector[3];
    MathFunctions::createUnitVector(bottomLeftXYZ, topLeftXYZ, bottomToTopUnitVector);
    
    /*
     * Find size adjustment for side (not corner) sizing handles
     */
    float sideHandleDX = 0.0;
    float sideHandleDY = 0.0;
    getSideHandleMouseDelta(spatialModification.m_sizingHandleType,
                            leftToRightUnitVector,
                            bottomToTopUnitVector,
                            spatialModification.m_mouseDX,
                            spatialModification.m_mouseDY,
                            sideHandleDX,
                            sideHandleDY);
    /*
     * When a resize handle is moved, update the corners of the shape
     */
    switch (spatialModification.m_sizingHandleType) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
        {
            addToXYZWithXY(bottomLeftXYZ,  sideHandleDX, sideHandleDY);
            addToXYZWithXY(bottomRightXYZ, sideHandleDX, sideHandleDY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
        {
            /*
             * Bottom left is now at the mouse XY
             */
            bottomLeftXYZ[0] = spatialModification.m_mouseX;
            bottomLeftXYZ[1] = spatialModification.m_mouseY;
            
            /*
             * Unit vector from bottom left to updated top right
             */
            float bottomLeftToTopRightUnitVector[3];
            MathFunctions::createUnitVector(bottomLeftXYZ, topRightXYZ, bottomLeftToTopRightUnitVector);
            
            /*
             * We have a right triangle where:
             *    The hypotnuse is from bottom left corner to new top right corner
             *    A right angle is at top left corner
             *    Want angle at bottom left but vector angle is at top right (all
             *    angles add up to PI=180).
             */
            const float oppositeAngle = MathFunctions::angle(topLeftXYZ,
                                                             topRightXYZ,
                                                             bottomLeftXYZ);
            const float angle = (M_PI / 2.0) - oppositeAngle;
            const float hypotnuseLength = MathFunctions::distance3D(bottomLeftXYZ,
                                                                    topRightXYZ);
            
            const float newWidth  = std::sin(angle) * hypotnuseLength;
            const float newHeight = std::cos(angle) * hypotnuseLength;
            
            topLeftXYZ[0] = bottomLeftXYZ[0] + bottomToTopUnitVector[0] * newHeight;
            topLeftXYZ[1] = bottomLeftXYZ[1] + bottomToTopUnitVector[1] * newHeight;
            
            bottomRightXYZ[0] = bottomLeftXYZ[0] + leftToRightUnitVector[0] * newWidth;
            bottomRightXYZ[1] = bottomLeftXYZ[1] + leftToRightUnitVector[1] * newWidth;
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
        {
            /*
             * Bottom right is now at the mouse XY
             */
            bottomRightXYZ[0] = spatialModification.m_mouseX;
            bottomRightXYZ[1] = spatialModification.m_mouseY;
            
            /*
             * Unit vector from top left to updated bottom right
             */
            float topLeftToBottomRightUnitVector[3];
            MathFunctions::createUnitVector(topLeftXYZ, bottomRightXYZ, topLeftToBottomRightUnitVector);
            
            /*
             * We have a right triangle where:
             *    The hypotnuse is from top left corner to new bottom right corner
             *    A right angle is at top right corner
             */
            const float angle = MathFunctions::angle(topRightXYZ,
                                                     topLeftXYZ,
                                                     bottomRightXYZ);
            const float hypotnuseLength = MathFunctions::distance3D(topLeftXYZ,
                                                                    bottomRightXYZ);
            
            const float newWidth  = std::cos(angle) * hypotnuseLength;
            const float newHeight = std::sin(angle) * hypotnuseLength;
            
            topRightXYZ[0] = topLeftXYZ[0] + leftToRightUnitVector[0] * newWidth;
            topRightXYZ[1] = topLeftXYZ[1] + leftToRightUnitVector[1] * newWidth;
            
            bottomLeftXYZ[0] = topLeftXYZ[0] - bottomToTopUnitVector[0] * newHeight;
            bottomLeftXYZ[1] = topLeftXYZ[1] - bottomToTopUnitVector[1] * newHeight;
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
        {
            addToXYZWithXY(topLeftXYZ,    sideHandleDX, sideHandleDY);
            addToXYZWithXY(bottomLeftXYZ, sideHandleDX, sideHandleDY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
        {
            addToXYZWithXY(topRightXYZ,    sideHandleDX, sideHandleDY);
            addToXYZWithXY(bottomRightXYZ, sideHandleDX, sideHandleDY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
        {
            addToXYZWithXY(topLeftXYZ,  sideHandleDX, sideHandleDY);
            addToXYZWithXY(topRightXYZ, sideHandleDX, sideHandleDY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
        {
            /*
             * Top left is now at the mouse XY
             */
            topLeftXYZ[0] = spatialModification.m_mouseX;
            topLeftXYZ[1] = spatialModification.m_mouseY;
            
            /*
             * Unit vector from top left to updated bottom right
             */
            float topLeftToBottomRightUnitVector[3];
            MathFunctions::createUnitVector(topLeftXYZ, bottomRightXYZ, topLeftToBottomRightUnitVector);
            
            /*
             * We have a right triangle where:
             *    The hypotnuse is from top left corner to new bottom right corner
             *    A right angle is at top right corner
             */
            const float oppositeAngle = MathFunctions::angle(topLeftXYZ,
                                                             bottomRightXYZ,
                                                             bottomLeftXYZ);
            const float angle = (M_PI / 2.0) - oppositeAngle;
            const float hypotnuseLength = MathFunctions::distance3D(topLeftXYZ,
                                                                    bottomRightXYZ);
            
            const float newWidth  = std::sin(angle) * hypotnuseLength;
            const float newHeight = std::cos(angle) * hypotnuseLength;
            
            topRightXYZ[0] = topLeftXYZ[0] + leftToRightUnitVector[0] * newWidth;
            topRightXYZ[1] = topLeftXYZ[1] + leftToRightUnitVector[1] * newWidth;
            
            bottomLeftXYZ[0] = topLeftXYZ[0] - bottomToTopUnitVector[0] * newHeight;
            bottomLeftXYZ[1] = topLeftXYZ[1] - bottomToTopUnitVector[1] * newHeight;
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
        {
            /*
             * Top right is now at the mouse XY
             */
            topRightXYZ[0] = spatialModification.m_mouseX;
            topRightXYZ[1] = spatialModification.m_mouseY;
            
            /*
             * Unit vector from updated top right to bottom left
             */
            float topRightToBottomLeftUnitVector[3];
            MathFunctions::createUnitVector(topRightXYZ, bottomLeftXYZ, topRightToBottomLeftUnitVector);
            
            /*
             * We have a right triangle where:
             *    The hypotnuse is from bottom left corner to new top right corner
             *    A right angle is at top left corner
             */
            const float oppositeAngle = MathFunctions::angle(topLeftXYZ,
                                                             bottomLeftXYZ,
                                                             topRightXYZ);
            const float angle = (M_PI / 2.0) - oppositeAngle;
            const float hypotnuseLength = MathFunctions::distance3D(topRightXYZ,
                                                                    bottomLeftXYZ);
            
            const float newWidth  = std::cos(angle) * hypotnuseLength;
            const float newHeight = std::sin(angle) * hypotnuseLength;
            
            topLeftXYZ[0] = topRightXYZ[0] - leftToRightUnitVector[0] * newWidth;
            topLeftXYZ[1] = topRightXYZ[1] - leftToRightUnitVector[1] * newWidth;
            
            bottomRightXYZ[0] = topRightXYZ[0] - bottomToTopUnitVector[0] * newHeight;
            bottomRightXYZ[1] = topRightXYZ[1] - bottomToTopUnitVector[1] * newHeight;
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            if (isMovableOrResizableFromGUI()) {
                addToXYZWithXY(bottomLeftXYZ,  spatialModification.m_mouseDX, spatialModification.m_mouseDY);
                addToXYZWithXY(bottomRightXYZ, spatialModification.m_mouseDX, spatialModification.m_mouseDY);
                addToXYZWithXY(topRightXYZ,    spatialModification.m_mouseDX, spatialModification.m_mouseDY);
                addToXYZWithXY(topLeftXYZ,     spatialModification.m_mouseDX, spatialModification.m_mouseDY);
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
        {
            switch (getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                case AnnotationCoordinateSpaceEnum::SURFACE:
                {
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
                    
                    
                    float delta = std::sqrt(spatialModification.m_mouseDX*spatialModification.m_mouseDX + spatialModification.m_mouseDY*spatialModification.m_mouseDY);
                    if (normalVector[2] < 0.0) {
                        delta = -delta;
                    }
                    m_rotationAngle += delta;
                }
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                case AnnotationCoordinateSpaceEnum::WINDOW:
                {
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
                    const float shapeXY[3] = {
                        viewportXYZ[0],
                        viewportXYZ[1],
                        0.0
                    };
                    
                    float normalVector[3];
                    MathFunctions::normalVector(shapeXY,
                                                currentMouseXY,
                                                previousMouseXY,
                                                normalVector);
                    
                    
                    float delta = std::sqrt(spatialModification.m_mouseDX*spatialModification.m_mouseDX + spatialModification.m_mouseDY*spatialModification.m_mouseDY);
                    if (normalVector[2] < 0.0) {
                        delta = -delta;
                    }
                    m_rotationAngle += delta;
                }
                    break;
            }
            if (m_rotationAngle > 360.0) {
                m_rotationAngle -= 360.0;
            }
            if (m_rotationAngle < 0.0) {
                m_rotationAngle += 360.0;
            }
            
            setModified();
            
            return;
        }
            break;
    }
    
    /*
     * Using the updated corners of the annotation, convert back to normalized x, y, width, and aspect ratio
     */
    float newViewportXYZ[3];
    MathFunctions::averageOfFourCoordinates(bottomLeftXYZ, bottomRightXYZ, topRightXYZ, topLeftXYZ, newViewportXYZ);
    const float newX = 100.0 * (newViewportXYZ[0] / spatialModification.m_viewportWidth);
    const float newY = 100.0 * (newViewportXYZ[1] / spatialModification.m_viewportHeight);
    const float newShapeViewportWidth = MathFunctions::distance3D(bottomLeftXYZ, bottomRightXYZ);
    const float newWidth = 100.0 * (newShapeViewportWidth / spatialModification.m_viewportWidth);
    const float newShapeViewportHeight = MathFunctions::distance3D(bottomLeftXYZ, topLeftXYZ);
    const float newHeight = 100.0 * (newShapeViewportHeight / spatialModification.m_viewportHeight);
    
    /*
     * Note:
     *    Coordinates are relative (range 0 to 100)
     *    Width is relative (range 0 to 100)
     *    Aspect ratio must only be greater than zero (when < 1, horizontal rectangle, when > 1 vertical rectangle)
     */
    if ((newX >= 0.0)
        && (newX <= 100.0)
        && (newY >= 0.0)
        && (newY <= 100.0)
        && (newWidth > 0.01)
        && (newWidth <= 100.0)
        && (newHeight > 0.01)
        && (newHeight <= 100.0)) {
        xyz[0] = newX;
        xyz[1] = newY;
        m_coordinate->setXYZ(xyz);
        m_width  = newWidth;
        m_height = newHeight;
        setModified();
    }
}

/**
 * When adjusting one of the "side handles" of a selected shape, set and adjust the change in the shape
 * so that the shape changes in the same direction as the mouse is moved.
 *
 * @param sizeHandle
      Size handle that is being adjusted.
 * @param leftToRightShapeVector
 *    Vector running from left to right of shape accounting for any rotation.
 * @param bottomToTopShapeVector
 *    Vector running from bottom to top of shape accounting for any rotation.
 * @param mouseDX
 *    Mouse movement in X.
 * @param mouseDY
 *    Mouse movement in Y.
 * @param shapeDxOut
 *    Suggested change in shape (signed).
 * @param shapeDyOut
 *    Suggested change in shape (signed).
 */
void
AnnotationTwoDimensionalShape::getSideHandleMouseDelta(const AnnotationSizingHandleTypeEnum::Enum sizeHandle,
                              const float leftToRightShapeVector[3],
                              const float bottomToTopShapeVector[3],
                              const float mouseDX,
                              const float mouseDY,
                              float& shapeDxOut,
                              float& shapeDyOut)
{
    shapeDxOut = 0.0;
    shapeDyOut = 0.0;
    
    bool useLeftRightFlag = false;
    bool useBottomTopFlag = false;
    bool posToNegFlag     = false;
    switch (sizeHandle) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
            useBottomTopFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
            useLeftRightFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
            useLeftRightFlag = true;
            posToNegFlag     = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
            useBottomTopFlag = true;
            posToNegFlag     = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
    }

    float shapeVector[3];
    if (useLeftRightFlag) {
        shapeVector[0] = leftToRightShapeVector[0];
        shapeVector[1] = leftToRightShapeVector[1];
        shapeVector[2] = 0.0;
    }
    else if (useBottomTopFlag) {
        shapeVector[0] = bottomToTopShapeVector[0];
        shapeVector[1] = bottomToTopShapeVector[1];
        shapeVector[2] = 0.0;
    }
    else {
        return;
    }
    if (posToNegFlag) {
        shapeVector[0] = -shapeVector[0];
        shapeVector[1] = -shapeVector[1];
    }
    
    MathFunctions::normalizeVector(shapeVector);
    
    float mouseVector[3] = { mouseDX, mouseDY, 0.0 };
    float mouseDelta = MathFunctions::normalizeVector(mouseVector);

    const float cosineAngle = MathFunctions::dotProduct(mouseVector,
                                                        shapeVector);
    if (cosineAngle < 0.0) {
        mouseDelta = -mouseDelta;
    }
    
    shapeDxOut = (shapeVector[0] * mouseDelta);
    shapeDyOut = (shapeVector[1] * mouseDelta);
}

/**
 * Add the given X and Y values to the three-dimensional coordinate.
 *
 * @param xyz
 *     Coordinate that has values added to it.
 * @param addX
 *     Value that is added coordinate's X.
 * @param addY
 *     Value that is added coordinate's Y.
 */
void
AnnotationTwoDimensionalShape::addToXYZWithXY(float xyz[3],
                                              const float addX,
                                              const float addY)
{
    xyz[0] += addX;
    xyz[1] += addY;
}

/**
 * Get the bounds for a two-dimensional shape annotation.
 *
 * @param viewportWidth
 *     Width of the viewport.
 * @param viewportHeight
 *     Height of the viewport.
 * @param viewportXYZ
 *     Viewport coordinates of the annotation.
 * @param bottomLeftOut
 *     The bottom left corner of the annotation absolute bounds.
 * @param bottomRightOut
 *     The bottom right corner of the annotation absolute bounds.
 * @param topRightOut
 *     The top right corner of the annotation absolute bounds.
 * @param topLeftOut
 *     The top left corner of the annotation absolute bounds.
 */
bool
AnnotationTwoDimensionalShape::getShapeBounds(const float viewportWidth,
                                              const float viewportHeight,
                                              const float viewportXYZ[3],
                                              float bottomLeftOut[3],
                                              float bottomRightOut[3],
                                              float topRightOut[3],
                                              float topLeftOut[3]) const
{
    /*
     * NOTE: Annotation's height and width are 'relative' ([0.0, 100.0] percentage) of window size.
     * So want HALF of width/height
     */
    const float halfWidth  = (getWidth()  / 200.0) * viewportWidth;
    const float halfHeight = (getHeight() / 200.0) * viewportHeight;
    
    bottomLeftOut[0]  = viewportXYZ[0] - halfWidth;
    bottomLeftOut[1]  = viewportXYZ[1] - halfHeight;
    bottomLeftOut[2]  = viewportXYZ[2];
    bottomRightOut[0] = viewportXYZ[0] + halfWidth;
    bottomRightOut[1] = viewportXYZ[1] - halfHeight;
    bottomRightOut[2] = viewportXYZ[2];
    topRightOut[0]    = viewportXYZ[0] + halfWidth;
    topRightOut[1]    = viewportXYZ[1] + halfHeight;
    topRightOut[2]    = viewportXYZ[2];
    topLeftOut[0]     = viewportXYZ[0] - halfWidth;
    topLeftOut[1]     = viewportXYZ[1] + halfHeight;
    topLeftOut[2]     = viewportXYZ[2];
    
    if (m_rotationAngle != 0) {
        Matrix4x4 matrix;
        matrix.translate(-viewportXYZ[0], -viewportXYZ[1], -viewportXYZ[2]);
        matrix.rotateZ(-m_rotationAngle);
        matrix.translate(viewportXYZ[0], viewportXYZ[1], viewportXYZ[2]);
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
    }
    return true;
}

/**
 * Set the width and height of the shape from bounding coordinates.
 *
 * @param xyzOne
 *     First bounding coordinate in absolute tab coordinates
 * @param xyzTwo
 *     Second bounding coordinate in absolute tab coordinates
 * @param spaceWidth
 *     Width of space.
 * @param spaceHeight
 *     Height of space.
 */
void
AnnotationTwoDimensionalShape::setWidthAndHeightFromBounds(const float xyzOne[3],
                                                           const float xyzTwo[3],
                                                           const float /*spaceWidth*/,
                                                           const float /*spaceHeight*/)
{
    const float minX = std::min(xyzOne[0],
                                xyzTwo[0]);
    const float maxX = std::max(xyzOne[0],
                                xyzTwo[0]);
    
    const float minY = std::min(xyzOne[1],
                                xyzTwo[1]);
    const float maxY = std::max(xyzOne[1],
                                xyzTwo[1]);
    
    const float width  = maxX - minX;
    const float height = maxY - minY;

    setWidth(width);
    setHeight(height);
    
//    const float relativeWidth  = 100.0 * (width  / static_cast<float>(spaceWidth));
//    const float relativeHeight = 100.0 * (height / static_cast<float>(spaceHeight));
//    
//    setWidth(relativeWidth);
//    setHeight(relativeHeight);
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
AnnotationTwoDimensionalShape::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
AnnotationTwoDimensionalShape::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Set the default value for height
 *
 * @param height
 *     Default for newly created text annotations.
 */
void
AnnotationTwoDimensionalShape::setUserDefaultHeight(const float height)
{
    s_userDefaultHeight = height;
}

/**
 * Set the default value for width
 *
 * @param width
 *     Default for newly created annotations.
 */
void
AnnotationTwoDimensionalShape::setUserDefaultWidth(const float width)
{
    s_userDefaultWidth = width;
}

/**
 * Set the default value for rotation angle
 *
 * @param rotation
 *     Default for newly created annotations.
 */
void
AnnotationTwoDimensionalShape::setUserDefaultRotationAngle(const float rotationAngle)
{
    s_userDefaultRotationAngle = rotationAngle;
}


