
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

#define __ANNOTATION_DECLARE__
#include "Annotation.h"
#undef __ANNOTATION_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::Annotation 
 * \brief Abstract class for annotations.
 * \ingroup Annotations
 */

/**
 * Constructor for an annotation.
 * 
 * @param drawingType
 *    Type of annotation for drawing.
 * @param shapeDimension
 *    Dimension of the annotation's shape.
 */
Annotation::Annotation(const AnnotationTypeEnum::Enum type)
: CaretObjectTracksModification(),
m_type(type)
{
    initializeAnnotationMembers();
}

/**
 * Destructor.
 */
Annotation::~Annotation()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Annotation::Annotation(const Annotation& obj)
: CaretObjectTracksModification(obj),
m_type(obj.m_type)
{
    initializeAnnotationMembers();
    this->copyHelperAnnotation(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Annotation&
Annotation::operator=(const Annotation& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperAnnotation(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Annotation::copyHelperAnnotation(const Annotation& obj)
{
    m_shapeDimension      = obj.m_shapeDimension;
    m_coordinateSpace     = obj.m_coordinateSpace;
    m_xyz[0]              = obj.m_xyz[0];
    m_xyz[1]              = obj.m_xyz[1];
    m_xyz[2]              = obj.m_xyz[2];
    m_width2D             = obj.m_width2D;
    m_height2D            = obj.m_height2D;
    m_length1D            = obj.m_length1D;
    m_rotationAngle       = obj.m_rotationAngle;
    m_surfaceSpaceStructure     = obj.m_surfaceSpaceStructure;
    m_surfaceSpaceNumberOfNodes = obj.m_surfaceSpaceNumberOfNodes;
    m_surfaceSpaceNodeIndex     = obj.m_surfaceSpaceNodeIndex;
    m_tabIndex            = obj.m_tabIndex;
    m_colorBackground[0]  = obj.m_colorBackground[0];
    m_colorBackground[1]  = obj.m_colorBackground[1];
    m_colorBackground[2]  = obj.m_colorBackground[2];
    m_colorBackground[3]  = obj.m_colorBackground[3];
    m_colorForeground[0]  = obj.m_colorForeground[0];
    m_colorForeground[1]  = obj.m_colorForeground[1];
    m_colorForeground[2]  = obj.m_colorForeground[2];
    m_colorForeground[3]  = obj.m_colorForeground[3];
 
    /*
     * Selected status is NOT copied.
     */
    m_selectedFlag = false;
}

/**
 * Initialize members of this class.
 */
void
Annotation::initializeAnnotationMembers()
{
    switch (m_type) {
        case AnnotationTypeEnum::ARROW:
            m_shapeDimension = AnnotationShapeDimensionEnum::ONE_DIMENSIONAL;
            break;
        case AnnotationTypeEnum::BOX:
            m_shapeDimension = AnnotationShapeDimensionEnum::TWO_DIMENSIONAL;
            break;
        case AnnotationTypeEnum::IMAGE:
            m_shapeDimension = AnnotationShapeDimensionEnum::TWO_DIMENSIONAL;
            break;
        case AnnotationTypeEnum::LINE:
            m_shapeDimension = AnnotationShapeDimensionEnum::ONE_DIMENSIONAL;
            break;
        case AnnotationTypeEnum::OVAL:
            m_shapeDimension = AnnotationShapeDimensionEnum::TWO_DIMENSIONAL;
            break;
        case AnnotationTypeEnum::SHAPE:
            break;
        case AnnotationTypeEnum::TEXT:
            m_shapeDimension = AnnotationShapeDimensionEnum::TWO_DIMENSIONAL;
            break;
    }
    

    m_coordinateSpace = AnnotationCoordinateSpaceEnum::TAB;
    
    m_selectedFlag = false;
    
    m_xyz[0] = 0.0;
    m_xyz[1] = 0.0;
    m_xyz[2] = 0.0;
    
    m_width2D  = 0.0;
    m_height2D = 0.0;
    m_length1D = 0.0;
    m_rotationAngle = 0.0;
    
    m_surfaceSpaceStructure     = StructureEnum::INVALID;
    m_surfaceSpaceNumberOfNodes = -1;
    m_surfaceSpaceNodeIndex     = -1;
    
    m_tabIndex = -1;
    
    m_colorBackground[0]  = 0.0;
    m_colorBackground[1]  = 0.0;
    m_colorBackground[2]  = 0.0;
    m_colorBackground[3]  = 0.0;
    
    m_colorForeground[0]  = 1.0;
    m_colorForeground[1]  = 1.0;
    m_colorForeground[2]  = 1.0;
    m_colorForeground[3]  = 1.0;
    
    /*
     * Note: The 'const' members are not saved to the scene as they 
     * are set by constructor.
     *
     * The 'selected' status is not saved to the scene.
     */
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<AnnotationCoordinateSpaceEnum>("m_coordinateSpace",
                                                         &m_coordinateSpace);
    m_sceneAssistant->addArray("m_xyz",
                               m_xyz, 3, 0.0);
    m_sceneAssistant->add("m_width2D",
                          &m_width2D);
    m_sceneAssistant->add("m_height2D",
                          &m_height2D);
    m_sceneAssistant->add("m_length1D",
                          &m_length1D);
    m_sceneAssistant->add("m_rotationAngle",
                          &m_rotationAngle);
    
    m_sceneAssistant->add<StructureEnum>("m_surfaceSpaceStructure",
                                         &m_surfaceSpaceStructure);
    m_sceneAssistant->add("m_surfaceSpaceNumberOfNodes",
                          &m_surfaceSpaceNumberOfNodes);
    m_sceneAssistant->add("m_surfaceSpaceNodeIndex",
                          &m_surfaceSpaceNodeIndex);
    
    m_sceneAssistant->add("m_tabIndex",
                          &m_tabIndex);
    m_sceneAssistant->addArray("m_colorBackground",
                               m_colorBackground, 4, 0.0);
    m_sceneAssistant->addArray("m_colorForeground",
                               m_colorForeground, 4, 1.0);
}


/**
 * @return The annotation drawing type.
 */
AnnotationTypeEnum::Enum
Annotation::getType() const
{
    return m_type;
}

/**
 * @return The coordinate space.
 */
AnnotationCoordinateSpaceEnum::Enum
Annotation::getCoordinateSpace() const
{
    return m_coordinateSpace;
}

/**
 * Set the coordinate space.
 *
 * @param coordinateSpace
 *     New value for coordinate space.
 */
void
Annotation::setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace)
{
    if (m_coordinateSpace != coordinateSpace) {
        m_coordinateSpace = coordinateSpace;
        setModified();
    }
}


/**
 * @return The annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 */
const float*
Annotation::getXYZ() const
{
    return m_xyz;
}

/**
 * Get the annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 *
 * @param xyzOut
 */
void
Annotation::getXYZ(float xyzOut[3]) const
{
    xyzOut[0] = m_xyz[0];
    xyzOut[1] = m_xyz[1];
    xyzOut[2] = m_xyz[2];
}


/**
 * Set the annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 *
 * @param xyz
 *     New coordinate for the annotation.
 */
void
Annotation::setXYZ(const float xyz[3])
{
    setXYZ(xyz[0], xyz[1], xyz[2]);
}

/**
 * Set the annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 *
 * @param xyz
 *     New coordinate for the annotation.
 */
void
Annotation::setXYZ(const double xyz[3])
{
    setXYZ(xyz[0], xyz[1], xyz[2]);
}

/**
 * Set the annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 *
 * @param x
 *     New X-coordinate for the annotation.
 * @param y
 *     New Y-coordinate for the annotation.
 * @param z
 *     New Z-coordinate for the annotation.
 */
void
Annotation::setXYZ(const float x,
                   const float y,
                   const float z)
{
    if ((x != m_xyz[0])
        || (y != m_xyz[1])
        || (z != m_xyz[2])) {
        m_xyz[0] = x;
        m_xyz[1] = y;
        m_xyz[2] = z;
        setModified();
    }
}

/**
 * @return Height for "two-dimensional" annotations in pixels.
 */
float
Annotation::getHeight2D() const
{
    return m_height2D;
}

/**
 * Set the height for "two-dimensional" annotations in pixels.
 *
 * @param height2D
 *    New value for height of the annotation.
 */
void
Annotation::setHeight2D(const float height2D)
{
    if (m_type != AnnotationTypeEnum::TEXT) {
        if (height2D > 1.0) {
            CaretLogWarning("Annotation height for non-text annotation should range [0.0, 1.0], "
                            " a relative value, but is "
                            + AString::number(height2D));
        }
    }
    
    if (height2D != m_height2D) {
        m_height2D = height2D;
        setModified();
    }
}

/**
 * @return Width for "two-dimensional" annotations in pixels.
 */
float
Annotation::getWidth2D() const
{
    return m_width2D;
}

/**
 * Set the width for "two-dimensional" annotations in pixels.
 *
 * @param width
 *    New value for width of the annotation.
 */
void
Annotation::setWidth2D(const float width2D)
{
    if (m_type != AnnotationTypeEnum::TEXT) {
        if (width2D > 1.0) {
            CaretLogWarning("Annotation width for non-text annotation should range [0.0, 1.0], "
                            " a relative value, but is "
                            + AString::number(width2D));
        }
    }
    
    if (width2D != m_width2D) {
        m_width2D = width2D;
        setModified();
    }
}

/**
 * @return The length for "one-dimensional" annotations in pixels.
 */
float
Annotation::getLength1D() const
{
    return m_length1D;
}

/**
 * Set the length for "one-dimensional" annotations in pixels.
 *
 * @param length1D
 *     New length value.
 */
void
Annotation::setLength1D(const float length1D)
{
    if (length1D != m_length1D) {
        m_length1D = length1D;
        setModified();
    }
}

/**
 * @return The rotation angle, in degrees, from vertical at the top (12 o'clock).
 */
float
Annotation::getRotationAngle() const
{
    return m_rotationAngle;
}

/**
 * Set the rotation angle, in degrees, from vertical at the top (12 o'clock).
 *
 * @param rotationAngle
 *     New value rotation angle.
 */
void
Annotation::setRotationAngle(const float rotationAngle)
{
    if (rotationAngle != m_rotationAngle) {
        m_rotationAngle = rotationAngle;
        setModified();
    }
}

/**
 * @param Return the shape's dimension 2D (ie: box) or 1D (ie: line).
 */
AnnotationShapeDimensionEnum::Enum
Annotation::getShapeDimension() const
{
    return m_shapeDimension;
}


/**
 * Get the surface space data.
 *
 * @param structureOut
 *     The surface structure.
 * @param surfaceNumberOfNodesOut
 *     Number of nodes in surface.
 * @param surfaceNodeIndexOut
 *     Index of surface node.
 */
void
Annotation::getSurfaceSpace(StructureEnum::Enum& structureOut,
                            int32_t& surfaceNumberOfNodesOut,
                            int32_t& surfaceNodeIndexOut) const
{
    structureOut            = m_surfaceSpaceStructure;
    surfaceNumberOfNodesOut = m_surfaceSpaceNumberOfNodes;
    surfaceNodeIndexOut     = m_surfaceSpaceNodeIndex;
}

/**
 * Set the surface space data.
 *
 * @param structure
 *     The surface structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param surfaceNodeIndex
 *     Index of surface node.
 */
void
Annotation::setSurfaceSpace(const StructureEnum::Enum structure,
                           const int32_t surfaceNumberOfNodes,
                           const int32_t surfaceNodeIndex)
{
    if (structure != m_surfaceSpaceStructure) {
        m_surfaceSpaceStructure = structure;
        setModified();
    }
    if (surfaceNumberOfNodes != m_surfaceSpaceNumberOfNodes) {
        m_surfaceSpaceNumberOfNodes = surfaceNumberOfNodes;
        setModified();
    }
    if (surfaceNodeIndex != m_surfaceSpaceNodeIndex) {
        m_surfaceSpaceNodeIndex = surfaceNodeIndex;
        setModified();
    }
}


/**
 * @return The tab index.  Valid only for tab coordinate space annotations.
 */
int32_t
Annotation::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Set tab index.  Valid only for tab coordinate space annotations.
 *
 * @param tabIndex
 */
void
Annotation::setTabIndex(const int32_t tabIndex)
{
    if (tabIndex != m_tabIndex) {
        m_tabIndex = tabIndex;
        setModified();
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
Annotation::toString() const
{
    return "Annotation";
}

/**
 * @return
 *    Foreground color RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
const float*
Annotation::getForegroundColor() const
{
    return m_colorForeground;
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getForegroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_colorForeground[0];
    rgbaOut[1] = m_colorForeground[1];
    rgbaOut[2] = m_colorForeground[2];
    rgbaOut[3] = m_colorForeground[3];
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getForegroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_colorForeground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_colorForeground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_colorForeground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_colorForeground[3] * 255.0);
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setForegroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_colorForeground[i]) {
            m_colorForeground[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the foreground color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setForegroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_colorForeground[i]) {
            m_colorForeground[i] = component;
            setModified();
        }
    }
}

/**
 * @return
 *    Background color RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 *    The background color is applied only when its alpha component is greater than zero.
 */
const float*
Annotation::getBackgroundColor() const
{
    return m_colorBackground;
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::getBackgroundColor(float rgbaOut[4]) const
{
    rgbaOut[0] = m_colorBackground[0];
    rgbaOut[1] = m_colorBackground[1];
    rgbaOut[2] = m_colorBackground[2];
    rgbaOut[3] = m_colorBackground[3];
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::getBackgroundColor(uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = static_cast<uint8_t>(m_colorBackground[0] * 255.0);
    rgbaOut[1] = static_cast<uint8_t>(m_colorBackground[1] * 255.0);
    rgbaOut[2] = static_cast<uint8_t>(m_colorBackground[2] * 255.0);
    rgbaOut[3] = static_cast<uint8_t>(m_colorBackground[3] * 255.0);
}

/**
 * Set the background color with floats.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
Annotation::setBackgroundColor(const float rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        if (rgba[i] != m_colorBackground[i]) {
            m_colorBackground[i] = rgba[i];
            setModified();
        }
    }
}

/**
 * Set the background color with bytes.
 * The background color is applied only when its alpha component is greater than zero.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
Annotation::setBackgroundColor(const uint8_t rgba[4])
{
    for (int32_t i = 0; i < 4; i++) {
        const float component = rgba[i] / 255.0;
        if (component != m_colorBackground[i]) {
            m_colorBackground[i] = component;
            setModified();
        }
    }
}

/**
 * @return The annotation's selected status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 */
bool
Annotation::isSelected() const
{
    return m_selectedFlag;
}

/**
 * Set the annotation's selected status.
 *
 * Note: (1) The selection status is never saved to a scene
 * or file.  (2) Changing the selection status DOES NOT
 * alter the annotation's modified status.
 */
void
Annotation::setSelected(const bool selectedStatus) const
{
    m_selectedFlag = selectedStatus;
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
Annotation::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Annotation",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
Annotation::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

