
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
 *     Annotatoion handle that is being dragged by the user.
 * @param spaceDX
 *     Change in space X-coordinate.
 * @param spaceDY
 *     Change in space Y-coordinate.
 */
void
AnnotationTwoDimensionalShape::applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                                        const float spaceDX,
                                                        const float spaceDY)
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
    
    float newX = xyz[0];
    float newY = xyz[1];
    float newWidth = m_width;
    float newAspectRatio  = m_height;
    float normalizedHeight = m_width * m_height;
    
    std::cout << "Y=" << newY << "aspect=" << m_height << std::endl;
    
    const float halfWidth = m_width / 2.0;
    const float halfNormalizedHeight = normalizedHeight / 2.0;
    float right  = xyz[0] + halfWidth;
    float left   = xyz[0] - halfWidth;
    float top    = xyz[1] + halfNormalizedHeight;
    std::cout << "   Top=" << top << std::endl;
    float bottom = xyz[1] - halfNormalizedHeight;
    switch (handleSelected) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
        {
            bottom    += spaceDY;
            newY      = (bottom + top) / 2.0;
            const float newNormlizedHeight = top - bottom;
            if (newNormlizedHeight > 0.0) {
                newAspectRatio = (newNormlizedHeight / newWidth) * 2;
            }
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
        {
            left     += spaceDX;
            newX     = (left + right) / 2.0;
            newWidth = right - left;
            if (newWidth > 0.0) {
                newAspectRatio = normalizedHeight / newWidth;
            }
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
//            newWidth += (spaceDX / 2.0);
//            if (newWidth > 0.0) {
//                newAspectRatio = heightPixels / newWidth;
//            }
//            newX += (spaceDX / 2.0);
        {
            right    += spaceDX;
            newX     = (left + right) / 2.0;
            newWidth = right - left;
            if (newWidth > 0.0) {
                newAspectRatio = normalizedHeight / newWidth;
            }
        }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
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
            newX += spaceDX;
            newY += spaceDY;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            break;
    }
    
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
        
        std::cout << "   newY=" << newY << " newAspect=" << newAspectRatio << std::endl;
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

