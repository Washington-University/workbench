
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#define __CHART_DATA_SOURCE_DECLARE__
#include "ChartDataSource.h"
#undef __CHART_DATA_SOURCE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;



/**
 * \class caret::ChartDataSource
 * \brief Contains source of data that is displayed in a chart.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartDataSource::ChartDataSource()
: CaretObject(),
SceneableInterface()
{
    initializeMembersChartDataSource();
}

/**
 * Destructor.
 */
ChartDataSource::~ChartDataSource()
{
    delete m_sceneAssistant;
}

/**
 * Initialize members of a new instance.
 */
void
ChartDataSource::initializeMembersChartDataSource()
{
    m_dataSourceMode = ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID;
    m_nodeIndex = -1;
    m_voxelXYZ[0] = 0.0;
    m_voxelXYZ[1] = 0.0;
    m_voxelXYZ[2] = 0.0;
    m_fileRowIndex = -1;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<ChartDataSourceModeEnum, ChartDataSourceModeEnum::Enum>("m_dataSourceMode",
                                                                                  &m_dataSourceMode);
    m_sceneAssistant->add("m_nodeIndex",
                          &m_nodeIndex);
    m_sceneAssistant->add("m_surfaceNumberOfNodes",
                          &m_surfaceNumberOfNodes);
    m_sceneAssistant->add("m_surfaceStructureName",
                          &m_surfaceStructureName);
    m_sceneAssistant->addArray("m_voxelXYZ",
                               m_voxelXYZ, 3, -1);
    m_sceneAssistant->add("m_fileRowIndex",
                          &m_fileRowIndex);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartDataSource::ChartDataSource(const ChartDataSource& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    initializeMembersChartDataSource();
    
    this->copyHelperChartDataSource(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
ChartDataSource&
ChartDataSource::operator=(const ChartDataSource& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartDataSource(obj);
    }
    return *this;
}

/**
 * Equality operator.
 *
 * @param rhs
 *     Other item for comparison.
 */
bool
ChartDataSource::operator==(const ChartDataSource& rhs) const
{
    if (this == &rhs) {
        return true;
    }
    
    if (m_chartableFileName != rhs.m_chartableFileName) {
        return false;
    }
    
    switch (m_dataSourceMode) {
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
            if (m_fileRowIndex == rhs.m_fileRowIndex) {
                return true;
            }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
            if ((m_surfaceStructureName == rhs.m_surfaceStructureName)
                && (m_surfaceNumberOfNodes == rhs.m_surfaceNumberOfNodes)
                && (m_nodeIndex == rhs.m_nodeIndex)) {
                return true;
            }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
            if ((m_surfaceStructureName == rhs.m_surfaceStructureName)
                && (m_surfaceNumberOfNodes == rhs.m_surfaceNumberOfNodes)
                && (m_nodeIndicesAverage == rhs.m_nodeIndicesAverage)) {
                return true;
            }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
            if ((m_voxelXYZ[0] == rhs.m_voxelXYZ[0])
                && (m_voxelXYZ[1] == rhs.m_voxelXYZ[1])
                && (m_voxelXYZ[2] == rhs.m_voxelXYZ[2])) {
                return true;
            }
            break;
    }
    
    return false;
}

/**
 * Equality operator.
 *
 * @param rhs
 *     Other item for comparison.
 */
bool
ChartDataSource::operator<(const ChartDataSource& rhs) const
{
    if (this == &rhs) {
        return false;
    }
    
    if (m_chartableFileName != rhs.m_chartableFileName) {
        return false;
    }
    
    switch (m_dataSourceMode) {
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
            return  (m_fileRowIndex < rhs.m_fileRowIndex);
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
            if (m_surfaceStructureName < rhs.m_surfaceStructureName) {
                return true;
            }
            if (m_surfaceNumberOfNodes < rhs.m_surfaceNumberOfNodes) {
                return true;
            }
            if (m_nodeIndex < rhs.m_nodeIndex) {
                return true;
            }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
            if (m_surfaceStructureName < rhs.m_surfaceStructureName) {
                return true;
            }
            if (m_surfaceNumberOfNodes < rhs.m_surfaceNumberOfNodes) {
                return true;
            }
            if (m_nodeIndicesAverage.size() < rhs.m_nodeIndicesAverage.size()) {
                return true;
            }
            else if (m_nodeIndicesAverage.size() > rhs.m_nodeIndicesAverage.size()) {
                return false;
            }
            else if (m_nodeIndicesAverage.size() == rhs.m_nodeIndicesAverage.size()) {
                std::vector<int32_t> copyMe = m_nodeIndicesAverage;
                std::sort(copyMe.begin(), copyMe.end());
                std::vector<int32_t> copyRhs = rhs.m_nodeIndicesAverage;
                std::sort(copyRhs.begin(), copyRhs.end());
                
                const int32_t numItems = static_cast<int32_t>(copyMe.size());
                for (int32_t i = 0; i < numItems; i++) {
                    CaretAssertVectorIndex(copyMe, i);
                    CaretAssertVectorIndex(copyRhs, i);
                    if (copyMe[i] < copyRhs[i]) {
                        return true;
                    }
                    else if (copyMe[i] > copyRhs[i]) {
                        return false;
                    }
                }
            }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
            if (m_voxelXYZ[0] < rhs.m_voxelXYZ[0]) {
                return true;
            }
            if (m_voxelXYZ[1] < rhs.m_voxelXYZ[1]) {
                return true;
            }
            if (m_voxelXYZ[2] < rhs.m_voxelXYZ[2]) {
                return true;
            }
            break;
    }
    
    return false;
}

/**
 * Copy the given data source to me.
 * 
 * @param copyFrom
 *     Chart data source that is copied to me.
 */
void
ChartDataSource::copy(const ChartDataSource* copyFrom)
{
    CaretAssert(copyFrom);
    copyHelperChartDataSource(*copyFrom);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
ChartDataSource::copyHelperChartDataSource(const ChartDataSource& obj)
{
    m_dataSourceMode       = obj.m_dataSourceMode;
    m_chartableFileName    = obj.m_chartableFileName;
    m_nodeIndex            = obj.m_nodeIndex;
    m_surfaceNumberOfNodes = obj.m_surfaceNumberOfNodes;
    m_surfaceStructureName = obj.m_surfaceStructureName;
    m_nodeIndicesAverage   = obj.m_nodeIndicesAverage;
    m_voxelXYZ[0]          = obj.m_voxelXYZ[0];
    m_voxelXYZ[1]          = obj.m_voxelXYZ[1];
    m_voxelXYZ[2]          = obj.m_voxelXYZ[2];
    m_fileRowIndex         = obj.m_fileRowIndex;
}

/**
 * @return Name of the chartable file.
 */
AString
ChartDataSource::getChartableFileName() const
{
    return m_chartableFileName;
}


/**
 * Setup for a surface node source.
 *
 * @param chartableFileName
 *    Name of the chartable file.
 * @param surfaceStructureName
 *    Name of surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the surface node.
 */
void
ChartDataSource::setSurfaceNode(const AString& chartableFileName,
                                const AString& surfaceStructureName,
                                const int32_t surfaceNumberOfNodes,
                                const int32_t nodeIndex)
{
    CaretAssert(nodeIndex >= 0);
    
    m_dataSourceMode = ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX;
    m_chartableFileName = chartableFileName;
    m_surfaceNumberOfNodes = surfaceNumberOfNodes;
    m_surfaceStructureName = surfaceStructureName;
    m_nodeIndex = nodeIndex;
}

/**
 * Is the given node the source of the data?
 *
 * @param surfaceStructureName
 *    Name of surface structure.
 * @param nodeIndex
 *    Index of the surface node.
 * @return 
 *    True if node is source of data, else false.
 */
bool
ChartDataSource::isSurfaceNodeSourceOfData(const AString& surfaceStructureName,
                                     const int32_t nodeIndex) const
{
    if (m_dataSourceMode == ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX) {
        if (m_nodeIndex == nodeIndex) {
            if (m_surfaceStructureName == surfaceStructureName) {
                return true;
            }
        }
    }

    return false;
}


/**
 * @return Mode indicating source of the data.
 */
ChartDataSourceModeEnum::Enum
ChartDataSource::getDataSourceMode() const
{
    return m_dataSourceMode;
}

/**
 * Get the surface node data source.
 *
 * @param surfaceStructureName
 *    Name of surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the surface node.
 */
void
ChartDataSource::getSurfaceNode(AString& surfaceStructureName,
                                int32_t& surfaceNumberOfNodes,
                                int32_t& nodeIndex) const
{
    surfaceStructureName = m_surfaceStructureName;
    surfaceNumberOfNodes = m_surfaceNumberOfNodes;
    nodeIndex     = m_nodeIndex;
}

/**
 * Get the surface node average data source.
 *
 * @param surfaceStructureName
 *    Name of surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndices
 *    Indices of the surface node.
 */
void
ChartDataSource::getSurfaceNodeAverage(AString& surfaceStructureName,
                                       int32_t& surfaceNumberOfNodes,
                                       std::vector<int32_t>& nodeIndices) const
{
    surfaceStructureName = m_surfaceStructureName;
    surfaceNumberOfNodes = m_surfaceNumberOfNodes;    
    nodeIndices   = m_nodeIndicesAverage;
}

/**
 * Get the surface node average data source.
 *
 * @param chartableFileName
 *    Name of the chartable file.
 * @param surfaceStructureName
 *    Name of surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndices
 *    Indices of the surface node.
 */
void
ChartDataSource::setSurfaceNodeAverage(const AString& chartableFileName,
                                       const AString& surfaceStructureName,
                           const int32_t surfaceNumberOfNodes,
                           const std::vector<int32_t>& nodeIndices)
{
    m_dataSourceMode = ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE;
    m_chartableFileName = chartableFileName;
    m_surfaceStructureName = surfaceStructureName;
    m_surfaceNumberOfNodes = surfaceNumberOfNodes;
    m_nodeIndicesAverage   = nodeIndices;
}


/**
 * Get the volume voxel data source.
 *
 * @param ijk
 *    Indices of the voxel.
 */
void
ChartDataSource::getVolumeVoxel(float xyz[3]) const
{
    xyz[0] = m_voxelXYZ[0];
    xyz[1] = m_voxelXYZ[1];
    xyz[2] = m_voxelXYZ[2];
}

/**
 * Set the volume voxel data source.
 *
 * @param chartableFileName
 *    Name of the chartable file.
 * @param ijk
 *    Indices of the voxel.
 */
void
ChartDataSource::setVolumeVoxel(const AString& chartableFileName,
                                const float xyz[3])
{
    m_dataSourceMode = ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK;
    m_chartableFileName = chartableFileName;
    m_voxelXYZ[0] = xyz[0];
    m_voxelXYZ[1] = xyz[1];
    m_voxelXYZ[2] = xyz[2];
}

/**
 * Get the file row data source.
 *
 * @param chartableFileName
 *    Name of the file.
 * @param fileRowIndex
 *    Index of the row.
 */
void
ChartDataSource::getFileRow(AString& chartableFileName,
                            int32_t& fileRowIndex) const
{
    chartableFileName  = m_chartableFileName;
    fileRowIndex = m_fileRowIndex;
}

/**
 * Set the file row data source.
 *
 * @param chartableFileName
 *    Name of the file.
 * @param fileRowIndex
 *    Index of the row.
 */
void
ChartDataSource::setFileRow(const AString& chartableFileName,
                            const int32_t fileRowIndex)
{
    m_dataSourceMode = ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW;
    m_chartableFileName  = chartableFileName;
    m_fileRowIndex = fileRowIndex;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ChartDataSource::getDescription() const
{
    AString s;
    switch (m_dataSourceMode) {
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
            s += (" Row "
                  + AString::number(m_fileRowIndex + 1));
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
            s += (m_surfaceStructureName
                  + ":  Vertex "
                  + AString::number(m_nodeIndex));
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
            s += (m_surfaceStructureName
                  + ":  Average of "
                  + AString::number(m_nodeIndicesAverage.size())
                  + " Vertices");
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
            s += ("Voxel XYZ ("
                  + AString::fromNumbers(m_voxelXYZ, 3, ",")
                  + ")");
            break;
    }
    
    if ( ! s.isEmpty()) {
        FileInformation fileInfo(m_chartableFileName);
        s += (" from "
              + fileInfo.getFileName());
    }
    
    return s;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ChartDataSource::toString() const
{
    AString s = "ChartDataSource";
    return s;
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
ChartDataSource::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartDataSource",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    sceneClass->addPathName("m_chartableFileName",
                            m_chartableFileName);
    
    switch (m_dataSourceMode) {
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
        {
            const int32_t numNodes = static_cast<int32_t>(m_nodeIndicesAverage.size());
            if (numNodes > 0) {
                sceneClass->addIntegerArray("m_nodeIndicesAverage",
                                            &m_nodeIndicesAverage[0],
                                            numNodes);
            }
        }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
            break;
    }
    
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
ChartDataSource::restoreFromScene(const SceneAttributes* sceneAttributes,
                                               const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    m_chartableFileName = sceneClass->getPathNameValue("m_chartableFileName");
    
    switch (m_dataSourceMode) {
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
        {
            const SceneClassArray* nodeArray = sceneClass->getClassArray("m_nodeIndicesAverage");
            if (nodeArray != NULL) {
                const int32_t numNodes = nodeArray->getNumberOfArrayElements();
                if (numNodes > 0) {
                    m_nodeIndicesAverage.resize(numNodes);
                    sceneClass->getIntegerArrayValue("m_nodeIndicesAverage",
                                                     &m_nodeIndicesAverage[0],
                                                     numNodes);
                }
            }
        }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
            break;
    }
    
}


