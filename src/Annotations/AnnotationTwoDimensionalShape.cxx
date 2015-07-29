
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
 */
AnnotationTwoDimensionalShape::AnnotationTwoDimensionalShape(const AnnotationTypeEnum::Enum type)
: Annotation(type)
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
    m_width  = 0.25;
    m_height = 0.25;
    
    if (isUseHeightAsAspectRatio()) {
        m_height = 1.0;
    }
    
    m_rotationAngle = 0.0;
    
    
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
 * @return Height for "two-dimensional" annotations in pixels.
 */
float
AnnotationTwoDimensionalShape::getHeight() const
{
    return m_height;
}

/**
 * Set the height for "two-dimensional" annotations in pixels.
 *
 * @param height
 *    New value for height of the annotation.
 */
void
AnnotationTwoDimensionalShape::setHeight(const float height)
{
    if ( ! isUseHeightAsAspectRatio()) {
        if ((height < 0.0)
            || (height > 1.0)) {
            CaretLogWarning("Annotation height for non-text annotation should range [0.0, 1.0], "
                            " a relative value, but is "
                            + AString::number(height));
        }
    }
    
    if (height != m_height) {
        m_height = height;
        setModified();
    }
}

/**
 * @return Width for "two-dimensional" annotations in pixels.
 */
float
AnnotationTwoDimensionalShape::getWidth() const
{
    return m_width;
}

/**
 * Set the width for "two-dimensional" annotations in pixels.
 *
 * @param width
 *    New value for width of the annotation.
 */
void
AnnotationTwoDimensionalShape::setWidth(const float width)
{
    if (! isUseHeightAsAspectRatio()) {
        if ((width < 0.0)
            || (width > 1.0)) {
            CaretLogWarning("Annotation width for non-text annotation should range [0.0, 1.0], "
                            " a relative value, but is "
                            + AString::number(width));
        }
    }

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
 * Apply a move or resize operation received from the GUI.
 *
 * @param handleSelected
 *     Annotatoion handle that is being dragged by the user.
 * @param viewportWidth
 *     Width of viewport
 * @param viewportHeight
 *     Height of viewport
 * @param mouseX
 *     Mouse X-coordinate.
 * @param mouseY
 *     Mouse Y-coordinate.
 * @param mouseDX
 *     Change in mouse X-coordinate.
 * @param mouseDY
 *     Change in mouse Y-coordinate.
 */
void
AnnotationTwoDimensionalShape::applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                                        const float viewportWidth,
                                                        const float viewportHeight,
                                                        const float mouseX,
                                                        const float mouseY,
                                                        const float mouseDX,
                                                        const float mouseDY)
{
//    bool resizableSpaceFlag = false;
//    switch (getCoordinateSpace()) {
//        case AnnotationCoordinateSpaceEnum::MODEL:
//            break;
//        case AnnotationCoordinateSpaceEnum::PIXELS:
//            break;
//        case AnnotationCoordinateSpaceEnum::SURFACE:
//            break;
//        case AnnotationCoordinateSpaceEnum::TAB:
//            resizableSpaceFlag = true;
//            break;
//        case AnnotationCoordinateSpaceEnum::WINDOW:
//            resizableSpaceFlag = true;
//            break;
//    }
//    
//    if ( ! resizableSpaceFlag) {
//        return;
//    }
    
    float xyz[3];
    m_coordinate->getXYZ(xyz);
    
    float viewportXYZ[3] = {
        xyz[0] * viewportWidth,
        xyz[1] * viewportHeight,
        xyz[2]
    };
    
    float bottomLeftXYZ[3];
    float bottomRightXYZ[3];
    float topLeftXYZ[3];
    float topRightXYZ[3];
    const bool validBounds = getShapeBounds(viewportWidth,
                                            viewportHeight,
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
    //    const AString infoTxt("ORIG: Bottom=" + AString::number(bottomXYZ[1], 'f', 4)
    //                          + " Top=" + AString::number(topXYZ[1], 'f', 4)
    //                          + " Y=" + AString::number(xyz[1], 'f', 6)
    //                          + " aspect=" + AString::number(m_height, 'f', 6));
    //    std::cout << qPrintable(infoTxt) << std::endl;
    
    /*
     * Find size adjustment for side (not corner) sizing handles
     */
    float sideHandleDX = 0.0;
    float sideHandleDY = 0.0;
    getSideHandleMouseDelta(handleSelected,
                            leftToRightUnitVector,
                            bottomToTopUnitVector,
                            mouseDX,
                            mouseDY,
                            sideHandleDX,
                            sideHandleDY);
    /*
     * When a resize handle is moved, update the corners of the shape
     */
    switch (handleSelected) {
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
            bottomLeftXYZ[0] = mouseX;
            bottomLeftXYZ[1] = mouseY;
            
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
            bottomRightXYZ[0] = mouseX;
            bottomRightXYZ[1] = mouseY;
            
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
            topLeftXYZ[0] = mouseX;
            topLeftXYZ[1] = mouseY;
            
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
            topRightXYZ[0] = mouseX;
            topRightXYZ[1] = mouseY;
            
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
                addToXYZWithXY(bottomLeftXYZ,  mouseDX, mouseDY);
                addToXYZWithXY(bottomRightXYZ, mouseDX, mouseDY);
                addToXYZWithXY(topRightXYZ,    mouseDX, mouseDY);
                addToXYZWithXY(topLeftXYZ,     mouseDX, mouseDY);
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
        {
            const float previousMouseXY[3] = {
                mouseX - mouseDX,
                mouseY - mouseDY,
                0.0
            };
            const float currentMouseXY[3] = {
                mouseX,
                mouseY,
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

            
            float delta = std::sqrt(mouseDX*mouseDX + mouseDY*mouseDY);
            if (normalVector[2] < 0.0) {
                delta = -delta;
            }
            m_rotationAngle += delta;
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
    const float newX = newViewportXYZ[0] / viewportWidth;
    const float newY = newViewportXYZ[1] / viewportHeight;
    const float newShapeViewportWidth = MathFunctions::distance3D(bottomLeftXYZ, bottomRightXYZ);
    const float newWidth = newShapeViewportWidth / viewportWidth;
    const float newShapeViewportHeight = MathFunctions::distance3D(bottomLeftXYZ, topLeftXYZ);
    const float newAspectRatio = ((newShapeViewportWidth != 0.0)
                                  ? (newShapeViewportHeight / newShapeViewportWidth)
                                  : 0.0);
    
    /*
     * Note:
     *    Coordinates are relative (range 0 to 1)
     *    Width is relative (range 0 to 1)
     *    Aspect ratio must only be greater than zero (when < 1, horizontal rectangle, when > 1 vertical rectangle)
     */
    if ((newX >= 0.0)
        && (newX <= 1.0)
        && (newY >= 0.0)
        && (newY <= 1.0)
        && (newWidth > 0.01)
        && (newWidth <= 1.0)
        && (newAspectRatio > 0.01)) {
        xyz[0] = newX;
        xyz[1] = newY;
        m_coordinate->setXYZ(xyz);
        m_width  = newWidth;
        m_height = newAspectRatio;
        setModified();
        
        //        {
        //            const float h = m_width * m_height;
        //            const float b = xyz[1] - (h / 2.0);
        //            const float t = xyz[1] + (h / 2.0);
        //            const AString infoTxt("NEW:  Bottom=" + AString::number(b, 'f', 4)
        //                                  + " Top=" + AString::number(t, 'f', 4)
        //                                  + " Y=" + AString::number(xyz[1], 'f', 6)
        //                                  + " aspect=" + AString::number(m_height, 'f', 6));
        //            std::cout << qPrintable(infoTxt) << std::endl;
        //            std::cout << std::endl;
        //        }
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
     * NOTE: Annotation's height and width are 'relative' ([0.0, 1.0] percentage) of window size.
     */
    const float halfWidth  = (getWidth()  / 2.0) * viewportWidth;
    float halfHeight = (getHeight() / 2.0) * viewportHeight;
    if (isUseHeightAsAspectRatio()) {
        halfHeight = halfWidth * getHeight();
    }
    
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
                                                           const float spaceWidth,
                                                           const float spaceHeight)
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
    const float aspectRatio = ((width > 0.0)
                               ? (height / width)
                               : 1.0);
    
    const float relativeWidth  = (width  / static_cast<float>(spaceWidth));
    const float relativeHeight = (height / static_cast<float>(spaceHeight));
    
    setWidth(relativeWidth);
    if (isUseHeightAsAspectRatio()) {
        setHeight(aspectRatio);
    }
    else {
        setHeight(relativeHeight);
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

