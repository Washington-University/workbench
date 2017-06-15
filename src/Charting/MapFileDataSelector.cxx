
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __MAP_FILE_DATA_SELECTOR_DECLARE__
#include "MapFileDataSelector.h"
#undef __MAP_FILE_DATA_SELECTOR_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::MapFileDataSelector 
 * \brief Identifies data for selection in a map file
 * \ingroup Charting
 */

/**
 * Constructor.
 */
MapFileDataSelector::MapFileDataSelector()
: CaretObject()
{
    reset();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_surfaceNumberOfVertices",
                          &m_surfaceNumberOfVertices);
    m_sceneAssistant->add<StructureEnum, StructureEnum::Enum>("m_surfaceStructure",
                                                              &m_surfaceStructure);
    m_sceneAssistant->add("m_surfaceVertexIndex",
                          &m_surfaceVertexIndex);
    m_sceneAssistant->addArray("m_voxelXYZ", m_voxelXYZ, 3, 0.0f);
}

/**
 * Destructor.
 */
MapFileDataSelector::~MapFileDataSelector()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
MapFileDataSelector::MapFileDataSelector(const MapFileDataSelector& obj)
: CaretObject(obj)
{
    this->copyHelperMapFileDataSelector(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
MapFileDataSelector&
MapFileDataSelector::operator=(const MapFileDataSelector& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperMapFileDataSelector(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
MapFileDataSelector::copyHelperMapFileDataSelector(const MapFileDataSelector& obj)
{
    m_dataSelectionType           = obj.m_dataSelectionType;
    m_surfaceNumberOfVertices     = obj.m_surfaceNumberOfVertices;
    m_surfaceStructure            = obj.m_surfaceStructure;
    m_surfaceVertexAverageIndices = obj.m_surfaceVertexAverageIndices;
    m_surfaceVertexIndex          = obj.m_surfaceVertexIndex;
    m_voxelXYZ[0] = obj.m_voxelXYZ[0];
    m_voxelXYZ[1] = obj.m_voxelXYZ[1];
    m_voxelXYZ[2] = obj.m_voxelXYZ[2];
}

/**
 * @return The data selection type.
 */
MapFileDataSelector::DataSelectionType
MapFileDataSelector::getDataSelectionType() const
{
    return m_dataSelectionType;
}

/**
 * Get data for a surface vertex.
 *
 * @param surfaceStructure
 *     The surface structure.
 * @param surfaceNumberOfVertices
 *     Number of vertices in the surface.
 * @param surfaceVertexIndex
 *     Index of the vertex.
 */
void
MapFileDataSelector::getSurfaceVertex(StructureEnum::Enum& surfaceStructure,
                                                  int32_t& surfaceNumberOfVertices,
                                                  int32_t& surfaceVertexIndex) const
{
    surfaceStructure        = m_surfaceStructure;
    surfaceNumberOfVertices = m_surfaceNumberOfVertices;
    surfaceVertexIndex      = m_surfaceVertexIndex;
}

/**
 * Set data for a surface vertex.
 *
 * @param surfaceStructure
 *     The surface structure.
 * @param surfaceNumberOfVertices
 *     Number of vertices in the surface.
 * @param surfaceVertexIndex
 *     Index of the vertex.
 */
void
MapFileDataSelector::setSurfaceVertex(const StructureEnum::Enum& surfaceStructure,
                                                  const int32_t surfaceNumberOfVertices,
                                                  const int32_t surfaceVertexIndex)
{
    reset();
    m_dataSelectionType       = DataSelectionType::SURFACE_VERTEX;
    m_surfaceStructure        = surfaceStructure;
    m_surfaceNumberOfVertices = surfaceNumberOfVertices;
    m_surfaceVertexIndex      = surfaceVertexIndex;
}

/**
 * Get data for a surface vertex average.
 *
 * @param surfaceStructure
 *     The surface structure.
 * @param surfaceNumberOfVertices
 *     Number of vertices in the surface.
 * @param surfaceVertexAverageIndices
 *     Indices of the vertex.
 */
void
MapFileDataSelector::getSurfaceVertexAverage(StructureEnum::Enum& surfaceStructure,
                                                         int32_t& surfaceNumberOfVertices,
                                                         std::vector<int32_t>& surfaceVertexAverageIndices) const
{
    surfaceStructure        = m_surfaceStructure;
    surfaceNumberOfVertices = m_surfaceNumberOfVertices;
    surfaceVertexAverageIndices = m_surfaceVertexAverageIndices;
}

/**
 * Set data for a surface vertex average.
 *
 * @param surfaceStructure
 *     The surface structure.
 * @param surfaceNumberOfVertices
 *     Number of vertices in the surface.
 * @param surfaceVertexAverageIndices
 *     Indices of the vertex.
 */
void
MapFileDataSelector::setSurfaceVertexAverage(const StructureEnum::Enum& surfaceStructure,
                                                         const int32_t surfaceNumberOfVertices,
                                                         const std::vector<int32_t>& surfaceVertexAverageIndices)
{
    reset();
    m_dataSelectionType           = DataSelectionType::SURFACE_VERTICES_AVERAGE;
    m_surfaceStructure            = surfaceStructure;
    m_surfaceNumberOfVertices     = surfaceNumberOfVertices;
    m_surfaceVertexAverageIndices = surfaceVertexAverageIndices;
}

/**
 * Get the stereotaxic coordinate of the volume voxel.
 *
 * @param voxelXYZ
 *    The volume voxel XYZ.
 */
void
MapFileDataSelector::getVolumeVoxelXYZ(float voxelXYZ[3]) const
{
    voxelXYZ[0] = m_voxelXYZ[0];
    voxelXYZ[1] = m_voxelXYZ[1];
    voxelXYZ[2] = m_voxelXYZ[2];
}

/**
 * Set the stereotaxic coordinate of the volume voxel.
 *
 * @param voxelXYZ
 *    The volume voxel XYZ.
 */
void
MapFileDataSelector::setVolumeVoxelXYZ(const float voxelXYZ[3])
{
    reset();
    m_dataSelectionType = DataSelectionType::VOLUME_XYZ;
    m_voxelXYZ[0] = voxelXYZ[0];
    m_voxelXYZ[1] = voxelXYZ[1];
    m_voxelXYZ[2] = voxelXYZ[2];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
MapFileDataSelector::toString() const
{
    AString s;
    
    switch (m_dataSelectionType) {
        case DataSelectionType::INVALID:
            break;
        case DataSelectionType::SURFACE_VERTEX:
            s = ("Vertex "
                 + StructureEnum::toGuiName(m_surfaceStructure)
                 + " "
                 + AString::number(m_surfaceVertexIndex + 1));
            break;
        case DataSelectionType::SURFACE_VERTICES_AVERAGE:
            s = ("Vertex Average"
                 + StructureEnum::toGuiName(m_surfaceStructure)
                 + " count="
                 + AString::number(m_surfaceVertexAverageIndices.size()));
            break;
        case DataSelectionType::VOLUME_XYZ:
            s = ("Voxel "
                 + AString::fromNumbers(m_voxelXYZ, 3, ","));
            break;
    }
    
    return s;
}

/**
 * Reset to an invalid state.
 */
void
MapFileDataSelector::reset()
{
    m_dataSelectionType       = DataSelectionType::SURFACE_VERTEX;
    m_surfaceStructure        = StructureEnum::INVALID;
    m_surfaceNumberOfVertices = -1;
    m_surfaceVertexIndex      = -1;
    m_surfaceVertexAverageIndices.clear();
    m_voxelXYZ[0] = 0.0f;
    m_voxelXYZ[1] = 0.0f;
    m_voxelXYZ[2] = 0.0f;
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
MapFileDataSelector::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "MapFileDataSelector",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    AString dataSelectionTypeName;
    switch (m_dataSelectionType) {
        case DataSelectionType::INVALID:
            dataSelectionTypeName = "INVALID";
            break;
        case DataSelectionType::SURFACE_VERTEX:
            dataSelectionTypeName = "SURFACE_VERTEX";
            break;
        case DataSelectionType::SURFACE_VERTICES_AVERAGE:
            dataSelectionTypeName = "SURFACE_VERTICES_AVERAGE";
            break;
        case DataSelectionType::VOLUME_XYZ:
            dataSelectionTypeName = "VOLUME_XYZ";
            break;
    }
    CaretAssert( ! dataSelectionTypeName.isEmpty());
    
    sceneClass->addString("m_dataSelectionType",
                          dataSelectionTypeName);

    const int32_t numVertices = static_cast<int32_t>(m_surfaceVertexAverageIndices.size());
    if (numVertices > 0) {
        sceneClass->addIntegerArray("m_surfaceVertexAverageIndices",
                                    &m_surfaceVertexAverageIndices[0],
                                    numVertices);
    }
    
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
MapFileDataSelector::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    m_dataSelectionType = DataSelectionType::INVALID;
    const AString dataSelectionTypeName = sceneClass->getStringValue("m_dataSelectionType",
                                                                     "INVALID");
    if (dataSelectionTypeName == "INVALID") {
        m_dataSelectionType = DataSelectionType::INVALID;
    }
    else if (dataSelectionTypeName == "SURFACE_VERTEX") {
        m_dataSelectionType = DataSelectionType::SURFACE_VERTEX;
    }
    else if (dataSelectionTypeName == "SURFACE_VERTICES_AVERAGE") {
        m_dataSelectionType = DataSelectionType::SURFACE_VERTICES_AVERAGE;
    }
    else if (dataSelectionTypeName == "VOLUME_XYZ") {
        m_dataSelectionType = DataSelectionType::VOLUME_XYZ;
    }
    else {
        CaretAssertMessage(0, ("Invalid data selection type name \""
                               + dataSelectionTypeName
                               + "\""));
    }
    
    const SceneClassArray* vertexArray = sceneClass->getClassArray("m_surfaceVertexAverageIndices");
    if (vertexArray != NULL) {
        const int32_t numVertices = vertexArray->getNumberOfArrayElements();
        if (numVertices > 0) {
            m_surfaceVertexAverageIndices.resize(numVertices);
            sceneClass->getIntegerArrayValue("m_surfaceVertexAverageIndices",
                                             &m_surfaceVertexAverageIndices[0],
                                             numVertices);
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

