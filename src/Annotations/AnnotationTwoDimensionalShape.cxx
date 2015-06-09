
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
 * Apply a move or resize operation received from the GUI.
 *
 * @param handleSelected
 *     Annotation handle that is being dragged by the user.
 * @param viewportWidth
 *     Width of viewport
 * @param viewportHeight
 *     Height of viewport
 * @param viewportDX
 *     Change in viewport X-coordinate.
 * @param viewportDY
 *     Change in viewport Y-coordinate.
 */
void
AnnotationTwoDimensionalShape::applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                                        const float viewportWidth,
                                                        const float viewportHeight,
                                                        const float viewportDX,
                                                        const float viewportDY)
{
    bool resizableSpaceFlag = false;
    switch (getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::MODEL:
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            resizableSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            resizableSpaceFlag = true;
            break;
    }
    
    if ( ! resizableSpaceFlag) {
        return;
    }
    
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
     * When a resize handle is moved, update the corners of the shape
     */
    switch (handleSelected) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
        {
            const float moveX = (bottomToTopUnitVector[0] * viewportDY);
            const float moveY = (bottomToTopUnitVector[1] * viewportDY);
            addToXYZWithXY(bottomLeftXYZ, moveX, moveY);
            addToXYZWithXY(bottomRightXYZ, moveX, moveY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
        {
            const float moveX = (leftToRightUnitVector[0] * viewportDX);
            const float moveY = (leftToRightUnitVector[1] * viewportDX);
            addToXYZWithXY(topLeftXYZ,    moveX, moveY);
            addToXYZWithXY(bottomLeftXYZ, moveX, moveY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
        {
            const float moveX = (leftToRightUnitVector[0] * viewportDX);
            const float moveY = (leftToRightUnitVector[1] * viewportDX);
            addToXYZWithXY(topRightXYZ,    moveX, moveY);
            addToXYZWithXY(bottomRightXYZ, moveX, moveY);
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
        {
            const float moveX = (bottomToTopUnitVector[0] * viewportDY);
            const float moveY = (bottomToTopUnitVector[1] * viewportDY);
            addToXYZWithXY(topLeftXYZ, moveX, moveY);
            addToXYZWithXY(topRightXYZ, moveX, moveY);
        }
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
            addToXYZWithXY(bottomLeftXYZ,  viewportDX, viewportDY);
            addToXYZWithXY(bottomRightXYZ, viewportDX, viewportDY);
            addToXYZWithXY(topRightXYZ,    viewportDX, viewportDY);
            addToXYZWithXY(topLeftXYZ,     viewportDX, viewportDY);
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
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

///**
// * Apply a move or resize operation received from the GUI.
// *
// * @param handleSelected
// *     Annotation handle that is being dragged by the user.
// * @param viewportWidth
// *     Width of viewport
// * @param viewportHeight
// *     Height of viewport
// * @param viewportDX
// *     Change in viewport X-coordinate.
// * @param viewportDY
// *     Change in viewport Y-coordinate.
// */
//void
//AnnotationTwoDimensionalShape::applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
//                                                        const float viewportWidth,
//                                                        const float viewportHeight,
//                                                        const float viewportDX,
//                                                        const float viewportDY)
//{
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
//    
//    float xyz[3];
//    m_coordinate->getXYZ(xyz);
//    
//    float viewportXYZ[3] = {
//        xyz[0] * viewportWidth,
//        xyz[1] * viewportHeight,
//        xyz[2]
//    };
//    
//    float bottomLeftXYZ[3];
//    float bottomRightXYZ[3];
//    float topLeftXYZ[3];
//    float topRightXYZ[3];
//    const bool validBounds = getShapeBounds(viewportWidth,
//                                            viewportHeight,
//                                            viewportXYZ,
//                                            bottomLeftXYZ,
//                                            bottomRightXYZ,
//                                            topRightXYZ,
//                                            topLeftXYZ);
//    if ( ! validBounds) {
//        CaretAssert(0);
//        return;
//    }
//    
//    float leftXYZ[3];
//    MathFunctions::averageOfTwoCoordinates(bottomLeftXYZ, topLeftXYZ, leftXYZ);
//    float rightXYZ[3];
//    MathFunctions::averageOfTwoCoordinates(bottomRightXYZ, topRightXYZ, rightXYZ);
//    float bottomXYZ[3];
//    MathFunctions::averageOfTwoCoordinates(bottomLeftXYZ, bottomRightXYZ, bottomXYZ);
//    float topXYZ[3];
//    MathFunctions::averageOfTwoCoordinates(topLeftXYZ, topLeftXYZ, topXYZ);
//    
//    float leftToRightUnitVector[3];
//    MathFunctions::createUnitVector(bottomLeftXYZ, bottomRightXYZ, leftToRightUnitVector);
//    float bottomToTopUnitVector[3];
//    MathFunctions::createUnitVector(bottomLeftXYZ, topLeftXYZ, bottomToTopUnitVector);
//    
//    float viewportX = viewportXYZ[0];
//    float viewportY = viewportXYZ[1];
//    float width     = MathFunctions::distance3D(bottomLeftXYZ, bottomRightXYZ);
//    float height    = MathFunctions::distance3D(bottomLeftXYZ, topLeftXYZ);
//    
////    const AString infoTxt("ORIG: Bottom=" + AString::number(bottomXYZ[1], 'f', 4)
////                          + " Top=" + AString::number(topXYZ[1], 'f', 4)
////                          + " Y=" + AString::number(xyz[1], 'f', 6)
////                          + " aspect=" + AString::number(m_height, 'f', 6));
////    std::cout << qPrintable(infoTxt) << std::endl;
//
//    switch (handleSelected) {
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
//        {
//            height    -= viewportDY;
//            viewportY += (viewportDY / 2.0);
//        }
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
//        {
//            width     -= viewportDX;
//            viewportX += (viewportDX / 2.0);
//        }
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
//        {
//            const float newWidth = width + viewportDX;
//            const float differenceVectorXYZ[3] = {
//                leftToRightUnitVector[0] * newWidth,
//                leftToRightUnitVector[1] * newWidth,
//                0.0
//            };
//            const float newTopRight[3] = {
//                topLeftXYZ[0] + differenceVectorXYZ[0],
//                topLeftXYZ[1] + differenceVectorXYZ[1],
//                topRightXYZ[2]
//            };
//            width = MathFunctions::distance3D(topLeftXYZ, newTopRight);
//            viewportX += differenceVectorXYZ[0] / 2.0;
//            viewportY += differenceVectorXYZ[1] / 2.0;
//            
//            
////            width     += viewportDX;
////            viewportX += (viewportDX / 2.0);
//        }
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
//        {
//            height    += viewportDY;
//            viewportY += (viewportDY / 2.0);
//        }
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
//            viewportX += viewportDX;
//            viewportY += viewportDY;
//            break;
//        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
//            break;
//    }
//    
//    const float newX = viewportX / viewportWidth;
//    const float newY = viewportY / viewportHeight;
//    const float newWidth = width / viewportWidth;
//    const float newAspectRatio = ((width != 0.0)
//                                  ? (height / width)
//                                  : 0.0);
//    
//    /*
//     * Note:
//     *    Coordinates are relative (range 0 to 1)
//     *    Width is relative (range 0 to 1)
//     *    Aspect ratio must only be greater than zero (when < 1, horizontal rectangle, when > 1 vertical rectangle)
//     */
//    if ((newX >= 0.0)
//        && (newX <= 1.0)
//        && (newY >= 0.0)
//        && (newY <= 1.0)
//        && (newWidth > 0.01)
//        && (newWidth <= 1.0)
//        && (newAspectRatio > 0.01)) {
//        xyz[0] = newX;
//        xyz[1] = newY;
//        m_coordinate->setXYZ(xyz);
//        m_width  = newWidth;
//        m_height = newAspectRatio;
//        
////        {
////            const float h = m_width * m_height;
////            const float b = xyz[1] - (h / 2.0);
////            const float t = xyz[1] + (h / 2.0);
////            const AString infoTxt("NEW:  Bottom=" + AString::number(b, 'f', 4)
////                                  + " Top=" + AString::number(t, 'f', 4)
////                                  + " Y=" + AString::number(xyz[1], 'f', 6)
////                                  + " aspect=" + AString::number(m_height, 'f', 6));
////            std::cout << qPrintable(infoTxt) << std::endl;
////            std::cout << std::endl;
////        }
//    }
//}

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

