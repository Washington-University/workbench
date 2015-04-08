
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
    m_alignmentHorizontal = obj.m_alignmentHorizontal;
    m_alignmentVertical   = obj.m_alignmentVertical;
    m_coordinateSpace     = obj.m_coordinateSpace;
    m_xyz[0]              = obj.m_xyz[0];
    m_xyz[1]              = obj.m_xyz[1];
    m_xyz[2]              = obj.m_xyz[2];
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
    
}

/**
 * Initialize members of this class.
 */
void
Annotation::initializeAnnotationMembers()
{
    m_alignmentHorizontal = AnnotationAlignHorizontalEnum::CENTER;
    m_alignmentVertical   = AnnotationAlignVerticalEnum::CENTER;
    m_coordinateSpace     = AnnotationCoordinateSpaceEnum::TAB;
    
    m_xyz[0] = 0.0;
    m_xyz[1] = 0.0;
    m_xyz[2] = 0.0;
    
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
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<AnnotationAlignHorizontalEnum>("m_alignmentHorizontal",
                                                         &m_alignmentHorizontal);
    m_sceneAssistant->add<AnnotationAlignVerticalEnum>("m_alignmentVertical",
                                                       &m_alignmentVertical);
    m_sceneAssistant->add<AnnotationCoordinateSpaceEnum>("m_coordinateSpace",
                                                         &m_coordinateSpace);
    m_sceneAssistant->addArray("m_xyz",
                               m_xyz, 3, 0.0);
    
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
 * @return The horizontal alignment.
 */
AnnotationAlignHorizontalEnum::Enum
Annotation::getHorizontalAlignment() const
{
    return m_alignmentHorizontal;
}

/**
 * Set the horizontal alignment.
 *
 * @param alignment
 *    New value for horizontal alignment.
 */
void
Annotation::setHorizontalAlignment(const AnnotationAlignHorizontalEnum::Enum alignment)
{
    if (m_alignmentHorizontal != alignment) {
        m_alignmentHorizontal = alignment;
        setModified();
    }
}

/**
 * @return The vertical alignment.
 */
AnnotationAlignVerticalEnum::Enum
Annotation::getVerticalAlignment() const
{
    return m_alignmentVertical;
}

/**
 * Set the vertical alignment.
 *
 * @param alignment
 *    New value for vertical alignment.
 */
void
Annotation::setVerticalAlignment(const AnnotationAlignVerticalEnum::Enum alignment)
{
    if (m_alignmentVertical != alignment) {
        m_alignmentVertical = alignment;
        setModified();
    }
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
Annotation::seturfaceSpace(const StructureEnum::Enum structure,
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

