
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

#define __ANNOTATION_COORDINATE_DECLARE__
#include "AnnotationCoordinate.h"
#undef __ANNOTATION_COORDINATE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinate 
 * \brief Coordinate for an annotation.
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
AnnotationCoordinate::AnnotationCoordinate()
: CaretObjectTracksModification()
{
    initializeAnnotationCoordinateMembers();
    
}

/**
 * Destructor.
 */
AnnotationCoordinate::~AnnotationCoordinate()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationCoordinate::AnnotationCoordinate(const AnnotationCoordinate& obj)
: CaretObjectTracksModification(obj),
SceneableInterface(obj)
{
    initializeAnnotationCoordinateMembers();
    this->copyHelperAnnotationCoordinate(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationCoordinate&
AnnotationCoordinate::operator=(const AnnotationCoordinate& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperAnnotationCoordinate(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationCoordinate::copyHelperAnnotationCoordinate(const AnnotationCoordinate& obj)
{
    m_xyz[0]              = obj.m_xyz[0];
    m_xyz[1]              = obj.m_xyz[1];
    m_xyz[2]              = obj.m_xyz[2];
    
    m_surfaceSpaceStructure     = obj.m_surfaceSpaceStructure;
    m_surfaceSpaceNumberOfNodes = obj.m_surfaceSpaceNumberOfNodes;
    m_surfaceSpaceNodeIndex     = obj.m_surfaceSpaceNodeIndex;
    m_surfaceOffsetLength       = obj.m_surfaceOffsetLength;
    m_surfaceOffsetVectorType   = obj.m_surfaceOffsetVectorType;
}

/**
 * Initialize members for an instance of this class.
 */
void
AnnotationCoordinate::initializeAnnotationCoordinateMembers()
{
    m_xyz[0] = 0.0;
    m_xyz[1] = 0.0;
    m_xyz[2] = 0.0;
    
    m_surfaceSpaceStructure     = StructureEnum::INVALID;
    m_surfaceSpaceNumberOfNodes = -1;
    m_surfaceSpaceNodeIndex     = -1;
    m_surfaceOffsetLength       = getDefaultSurfaceOffsetLength();
    m_surfaceOffsetVectorType   = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addArray("m_xyz",
                               m_xyz, 3, 0.0);

    m_sceneAssistant->add<StructureEnum, StructureEnum::Enum>("m_surfaceSpaceStructure",
                                         &m_surfaceSpaceStructure);
    m_sceneAssistant->add("m_surfaceSpaceNumberOfNodes",
                          &m_surfaceSpaceNumberOfNodes);
    m_sceneAssistant->add("m_surfaceSpaceNodeIndex",
                          &m_surfaceSpaceNodeIndex);
    m_sceneAssistant->add("m_surfaceOffsetLength",
                          &m_surfaceOffsetLength);
    m_sceneAssistant->add<AnnotationSurfaceOffsetVectorTypeEnum, AnnotationSurfaceOffsetVectorTypeEnum::Enum>("m_surfaceOffsetVectorType",
                                                                                                              &m_surfaceOffsetVectorType);
    
}

/**
 * @return The annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 */
const float*
AnnotationCoordinate::getXYZ() const
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
AnnotationCoordinate::getXYZ(float xyzOut[3]) const
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
AnnotationCoordinate::setXYZ(const float xyz[3])
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
AnnotationCoordinate::setXYZ(const double xyz[3])
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
AnnotationCoordinate::setXYZ(const float x,
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
AnnotationCoordinate::getSurfaceSpace(StructureEnum::Enum& structureOut,
                                      int32_t& surfaceNumberOfNodesOut,
                                      int32_t& surfaceNodeIndexOut) const
{
    structureOut            = m_surfaceSpaceStructure;
    surfaceNumberOfNodesOut = m_surfaceSpaceNumberOfNodes;
    surfaceNodeIndexOut     = m_surfaceSpaceNodeIndex;
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
 * @param surfaceOffsetLengthOut
 *     Offset of annotation from surface
 * @param surfaceOffsetVectorTypeOut
 *     Offset from surface vector type.
 */
void
AnnotationCoordinate::getSurfaceSpace(StructureEnum::Enum& structureOut,
                                      int32_t& surfaceNumberOfNodesOut,
                                      int32_t& surfaceNodeIndexOut,
                                      float& surfaceOffsetLengthOut,
                                      AnnotationSurfaceOffsetVectorTypeEnum::Enum& surfaceOffsetVectorTypeOut) const
{
    structureOut               = m_surfaceSpaceStructure;
    surfaceNumberOfNodesOut    = m_surfaceSpaceNumberOfNodes;
    surfaceNodeIndexOut        = m_surfaceSpaceNodeIndex;
    surfaceOffsetLengthOut     = m_surfaceOffsetLength;
    surfaceOffsetVectorTypeOut = m_surfaceOffsetVectorType;
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
AnnotationCoordinate::setSurfaceSpace(const StructureEnum::Enum structure,
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
 * Set the surface space data.
 *
 * @param structure
 *     The surface structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param surfaceNodeIndex
 *     Index of surface node.
 * @param surfaceOffsetLength
 *     Offset of annotation from surface
 * @param surfaceOffsetVectorType
 *     Offset from surface vector type.
 */
void
AnnotationCoordinate::setSurfaceSpace(const StructureEnum::Enum structure,
                                      const int32_t surfaceNumberOfNodes,
                                      const int32_t surfaceNodeIndex,
                                      const float surfaceOffsetLength,
                                      const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType)
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
    if (surfaceOffsetLength != m_surfaceOffsetLength) {
        m_surfaceOffsetLength = surfaceOffsetLength;
        setModified();
    }
    if (surfaceOffsetVectorType != m_surfaceOffsetVectorType) {
        m_surfaceOffsetVectorType = surfaceOffsetVectorType;
        setModified();
    }
}
/**
 * @return The default surface offset length.
 */
float
AnnotationCoordinate::getDefaultSurfaceOffsetLength()
{
    return 5.0;
}

/**
 * @return The surface offset length.
 */
float
AnnotationCoordinate::getSurfaceOffsetLength() const
{
    return m_surfaceOffsetLength;
}

/**
 * @return Type of surface offset.
 */
AnnotationSurfaceOffsetVectorTypeEnum::Enum
AnnotationCoordinate::getSurfaceOffsetVectorType() const
{
    return m_surfaceOffsetVectorType;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationCoordinate::toString() const
{
    return "AnnotationCoordinate";
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
AnnotationCoordinate::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationCoordinate",
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
AnnotationCoordinate::restoreFromScene(const SceneAttributes* sceneAttributes,
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

