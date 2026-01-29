
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
#include "MathFunctions.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinate 
 * \brief Coordinate for an annotation.
 * \ingroup Annotations
 */

/**
 * @return Default value for offset polar angle
 */
float
AnnotationCoordinate::getSurfaceTextOffsetPolarAngleDefaultValue()
{
    return 90.0;
}

/**
 * @return Default value for offset polar radius
 */
float
AnnotationCoordinate::getSurfaceTextOffsetPolarRadiusDefaultValue()
{
    return 10.0;
}


/**
 * Constructor.
 */
AnnotationCoordinate::AnnotationCoordinate(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: CaretObjectTracksModification(),
SceneableInterface(),
m_attributeDefaultType(attributeDefaultType)
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
SceneableInterface(obj),
m_attributeDefaultType(obj.m_attributeDefaultType)
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
    
    m_surfaceSpaceStructure        = obj.m_surfaceSpaceStructure;
    m_surfaceSpaceNumberOfNodes    = obj.m_surfaceSpaceNumberOfNodes;
    m_surfaceSpaceNodeIndex        = obj.m_surfaceSpaceNodeIndex;
    m_surfaceOffsetLength          = obj.m_surfaceOffsetLength;
    m_surfaceOffsetVectorType      = obj.m_surfaceOffsetVectorType;
    m_surfaceTextOffsetPolarAngle  = obj.m_surfaceTextOffsetPolarAngle;
    m_surfaceTextOffsetPolarRadius = obj.m_surfaceTextOffsetPolarRadius;
    m_mediaFileName                = obj.m_mediaFileName;
    m_histologySpaceKey            = obj.m_histologySpaceKey;
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
    
    m_surfaceSpaceStructure        = StructureEnum::INVALID;
    m_surfaceSpaceNumberOfNodes    = -1;
    m_surfaceSpaceNodeIndex        = -1;
    m_surfaceTextOffsetPolarAngle  = getSurfaceTextOffsetPolarAngleDefaultValue();
    m_surfaceTextOffsetPolarRadius = getSurfaceTextOffsetPolarRadiusDefaultValue();
    
    switch (m_attributeDefaultType) {
        case AnnotationAttributesDefaultTypeEnum::NORMAL:
            m_surfaceOffsetLength       = getDefaultSurfaceOffsetLength();
            m_surfaceOffsetVectorType   = AnnotationSurfaceOffsetVectorTypeEnum::TANGENT;
            break;
        case AnnotationAttributesDefaultTypeEnum::USER:
            m_surfaceOffsetLength     = s_userDefaultSurfaceOffsetLength;
            m_surfaceOffsetVectorType = s_userDefaultSurfaceOffsetVectorType;
            break;
    }
    
    m_mediaFileName = "";
    m_histologySpaceKey = HistologySpaceKey();
    
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
    m_sceneAssistant->add("m_surfaceTextOffsetPolarAngle",
                          &m_surfaceTextOffsetPolarAngle);
    m_sceneAssistant->add("m_surfaceTextOffsetPolarRadius",
                          &m_surfaceTextOffsetPolarRadius);

    m_sceneAssistant->add("m_mediaFileName",
                          &m_mediaFileName);
    m_sceneAssistant->add("m_histologySpaceKey",
                          "HistologySpaceKey",
                          &m_histologySpaceKey);
}

/**
 * @return True if the XYZ coordinates of this and the given XYZ are equal
 * @param xyz
 *    XYZ for testing equality
 */
bool
AnnotationCoordinate::equalXYZ(const Vector3D& xyz) const
{
    if (m_xyz[0] != xyz[0]) return false;
    if (m_xyz[1] != xyz[1]) return false;
    if (m_xyz[2] != xyz[2]) return false;
    return true;
}


/**
 * @return The annotation's coordinate.
 *
 * For tab and window spaces, the Z value is a depth for ordering.
 */
const Vector3D
AnnotationCoordinate::getXYZ() const
{
    return Vector3D(m_xyz);
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
 * Add to the annotation's XYZ coordinate.
 *
 * @param dx
 *     Change in X-coordinate for the annotation.
 * @param dy
 *     Change in Y-coordinate for the annotation.
 * @param dz
 *     Change in Z-coordinate for the annotation.
 */
void
AnnotationCoordinate::addToXYZ(const float dx,
                               const float dy,
                               const float dz)
{
    setXYZ(m_xyz[0] + dx,
           m_xyz[1] + dy,
           m_xyz[2] + dz);
}


/**
 * Set the window or tab XYZ from viewport X, Y.
 *
 * @param viewportWidth
 *     Width of viewport.
 * @param viewportHeight
 *     Height of viewport.
 * @param viewportX 
 *     X viewport coordinate.
 * @param viewportY
 *     Y viewport coordinate.
 */
void
AnnotationCoordinate::setXYZFromViewportXYZ(const float viewportWidth,
                                            const float viewportHeight,
                                            const float viewportX,
                                            const float viewportY)
{
    const float x = MathFunctions::limitRange(((viewportX / viewportWidth) * 100.0), 0.0, 100.0);
    const float y = MathFunctions::limitRange(((viewportY / viewportHeight) * 100.0), 0.0, 100.0);
    
    setXYZ(x, y, m_xyz[2]);
}

/**
 * Get the viewport X, Y from window or tab XYZ.
 *
 * @param viewportWidth
 *     Width of viewport.
 * @param viewportHeight
 *     Height of viewport.
 * @param viewportXOut
 *     X viewport coordinate.
 * @param viewportYOut
 *     Y viewport coordinate.
 */
void
AnnotationCoordinate::getViewportXY(const float viewportWidth,
                                    const float viewportHeight,
                                    float& viewportXOut,
                                    float& viewportYOut) const
{
    viewportXOut = (m_xyz[0] / 100.0) * viewportWidth;
    viewportYOut = (m_xyz[1] / 100.0) * viewportHeight;
}

/**
 * @return Surface space structure
 */
StructureEnum::Enum
AnnotationCoordinate::getSurfaceSpaceStructure() const
{
    return m_surfaceSpaceStructure;
}

/**
 * @return Surface space number of nodes
 */
int32_t
AnnotationCoordinate::getSurfaceSpaceNumberOfNodes() const
{
    return m_surfaceSpaceNumberOfNodes;
}

/**
 * @return Surface space node index
 */
int32_t
AnnotationCoordinate::getSurfaceSpaceNodeIndex() const
{
    return m_surfaceSpaceNodeIndex;
}

/**
 * @return Surface space offset length
 */
float
AnnotationCoordinate::getSurfaceSpaceOffsetLength() const
{
    return m_surfaceOffsetLength;
}

/**
 * @return Surface space offset vector type
 */
AnnotationSurfaceOffsetVectorTypeEnum::Enum
AnnotationCoordinate::getSurfaceSpaceOffsetVectorType() const
{
    return m_surfaceOffsetVectorType;
}

/**
 * @return Surface space text offset angle
 */
float
AnnotationCoordinate::getSurfaceTextOffsetPolarAngle() const
{
    return m_surfaceTextOffsetPolarAngle;
}

/**
 * @return Surface space text offset radius
 */
float
AnnotationCoordinate::getSurfaceTextOffsetPolarRadius() const
{
    return m_surfaceTextOffsetPolarRadius;
}

/**
 * Set the surface space structure
 * @param structure
 *    New strucrture
 */
void
AnnotationCoordinate::setSurfaceSpaceStructure(const StructureEnum::Enum structure)
{
    if (m_surfaceSpaceStructure != structure) {
        m_surfaceSpaceStructure = structure;
        setModified();
    }
}

/**
 * Set the surface space number of nodes
 * @param numberOfNodes
 *    New value
 */
void
AnnotationCoordinate::setSurfaceSpaceNumberOfNodes(const int32_t numberOfNodes)
{
    if (m_surfaceSpaceNumberOfNodes != numberOfNodes) {
        m_surfaceSpaceNumberOfNodes = numberOfNodes;
        setModified();
    }
}

/**
 * Set the surface space node index
 * @param nodeIndex
 *    New value
 */
void
AnnotationCoordinate::setSurfaceSpaceNodeIndex(const int32_t nodeIndex)
{
    m_surfaceSpaceNodeIndex = nodeIndex;
}

/**
 * Set the surface space offset length
 * @param offsetLength
 *    New value
 */
void
AnnotationCoordinate::setSurfaceSpaceOffsetLength(const float offsetLength)
{
    if (m_surfaceOffsetLength != offsetLength) {
        m_surfaceOffsetLength = offsetLength;
        setModified();
    }
}

/**
 * Set the surface space offset type
 * @param offsetVectorType
 *    New value
 */
void
AnnotationCoordinate::setSurfaceSpaceOffsetVectorType(const AnnotationSurfaceOffsetVectorTypeEnum::Enum offsetVectorType)
{
    if (m_surfaceOffsetVectorType != offsetVectorType) {
        m_surfaceOffsetVectorType = offsetVectorType;
        setModified();
    }
}

/**
 * Set the surface space offset angle
 * @param angle
 *    New value
 */
void
AnnotationCoordinate::setSurfaceTextOffsetPolarAngle(const float angle)
{
    m_surfaceTextOffsetPolarAngle = angle;
}

/**
 * Set the surface space offset radius
 * @param radius
 *    New value
 */
void
AnnotationCoordinate::setSurfaceTextOffsetPolarRadius(const float radius)
{
    if (m_surfaceTextOffsetPolarRadius != radius) {
        m_surfaceTextOffsetPolarRadius = radius;
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
 * @return The surface structure.
 */
StructureEnum::Enum
AnnotationCoordinate::getSurfaceStructure() const
{
    return m_surfaceSpaceStructure;
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
 * Set the media file name and pixel IJK
 */
void
AnnotationCoordinate::setMediaFileNameAndPixelSpace(const AString& mediaFileName,
                                                    const float xyz[3])
{
    setMediaFileName(mediaFileName);
    for (int32_t i = 0; i < 3; i++) {
        if (xyz[i] != m_xyz[i]) {
            m_xyz[i] = xyz[i];
            setModified();
        }
    }
}

/**
 * @return The histology space key
 */
const HistologySpaceKey&
AnnotationCoordinate::getHistologySpaceKey() const
{
    return m_histologySpaceKey;
}

/**
 * Get the histology space info
 * @param histologySpaceKeyOut
 *    Key for histology space
 * @param xyzOut
 *    Histology coordinate XYZ
 */
void
AnnotationCoordinate::getHistologySpace(HistologySpaceKey& histologySpaceKeyOut,
                                        float xyzOut[3]) const
{
    histologySpaceKeyOut = m_histologySpaceKey;
    xyzOut[0] = m_xyz[0];
    xyzOut[1] = m_xyz[1];
    xyzOut[2] = m_xyz[2];

}

/**
 * Set the histology space key
 * @param histologySpaceKey
 *    Key for histology space
 */
void
AnnotationCoordinate::setHistologySpaceKey(const HistologySpaceKey& histologySpaceKey)
{
    if (histologySpaceKey != m_histologySpaceKey) {
        m_histologySpaceKey = histologySpaceKey;
        setModified();
    }
}


/**
 * Set the histology space info
 * @param histologySpaceKey
 *    Key for histology space
 * @param xyz
 *    Histology coordinate XYZ
 */
void
AnnotationCoordinate::setHistologySpace(const HistologySpaceKey& histologySpaceKey,
                                        const float xyz[3])
{
    setHistologySpaceKey(histologySpaceKey);
    setXYZ(xyz);
}

/**
 * @return Media file name for
 */
AString
AnnotationCoordinate::getMediaFileName() const
{
    return m_mediaFileName;
}

/**
 * Set the media file name
 * @param mediaFileName
 *    Name of media file.
 */
void
AnnotationCoordinate::setMediaFileName(const AString& mediaFileName)
{
    if (mediaFileName != m_mediaFileName) {
        m_mediaFileName = mediaFileName;
        setModified();
    }
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
 * @return A descritption of the coordinates for the given space
 * @param space
 *    The coordinate space
 */
AString
AnnotationCoordinate::toStringForCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum space) const
{
    AString s(AnnotationCoordinateSpaceEnum::toGuiName(space) + " ");
    
    switch (space) {
        case AnnotationCoordinateSpaceEnum::CHART:
            s.append(AString::fromNumbers(m_xyz, 3, " "));
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            s.append("Histology Space Key: "
                     + m_histologySpaceKey.toString()
                     + " "
                     + AString::fromNumbers(m_xyz, 3, " "));
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            s.append("Media File Name: "
                     + m_mediaFileName
                     + " "
                     + AString::fromNumbers(m_xyz, 3, " "));
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            s.append(AString::fromNumbers(m_xyz, 3, " "));
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            s.append(AString::fromNumbers(m_xyz, 3, " "));
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
        {
            StructureEnum::Enum structure(StructureEnum::INVALID);
            int32_t surfaceVertexCount(-1);
            int32_t vertexIndex(-1);
            getSurfaceSpace(structure,
                                       surfaceVertexCount,
                                       vertexIndex);
            s.append("Structure: " + StructureEnum::toGuiName(m_surfaceSpaceStructure));
            s.append(" Surface Num Vertices: " + AString::number(m_surfaceSpaceNumberOfNodes));
            s.append(" Surface Vertex: " + AString::number(m_surfaceSpaceNodeIndex));
            s.append(" Surface Offset Vector Type: " + AnnotationSurfaceOffsetVectorTypeEnum::toName(m_surfaceOffsetVectorType));
            s.append(" Surface Offset Length: " + AString::number(m_surfaceOffsetLength));
            s.append(" Surface Text Offset Polar Angle: " + AString::number(m_surfaceTextOffsetPolarAngle));
            s.append(" Surface Text Offset Polar Radius: " + AString::number(m_surfaceTextOffsetPolarRadius));
        }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            s.append(AString::fromNumbers(m_xyz, 3, " "));
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            s.append("Invalid");
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            s.append(AString::fromNumbers(m_xyz, 3, " "));
            break;
    }
    
    return s;
}


/**
 * Set the user default for the surface offset vector type.
 *
 * @param surfaceOffsetVectorType
 *     new default value.
 */
void
AnnotationCoordinate::setUserDefautlSurfaceOffsetVectorType(const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType)
{
    s_userDefaultSurfaceOffsetVectorType = surfaceOffsetVectorType;
}

/**
 * Set the user default for the surface offset length.
 * 
 * @param surfaceOffsetLength
 *    New default value.
 */
void
AnnotationCoordinate::setUserDefaultSurfaceOffsetLength(const float surfaceOffsetLength)
{
    s_userDefaultSurfaceOffsetLength = surfaceOffsetLength;
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

