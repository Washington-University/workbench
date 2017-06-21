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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "DataFileException.h"
#include "DataFileTypeEnum.h"
#include "GiftiFile.h"
#include "MapFileDataSelector.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "NiftiEnums.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"

#include <limits>

using namespace caret;

/**
 * Constructor.
 */
MetricFile::MetricFile()
: GiftiTypeFile(DataFileTypeEnum::METRIC)
{
    this->initializeMembersMetricFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
MetricFile::MetricFile(const MetricFile& sf)
: GiftiTypeFile(sf),
ChartableLineSeriesBrainordinateInterface()
{
    this->copyHelperMetricFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the MetricFile parameter.
 */
MetricFile& 
MetricFile::operator=(const MetricFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperMetricFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
MetricFile::~MetricFile()
{
    this->columnDataPointers.clear();
}

void MetricFile::writeFile(const AString& filename)
{
    if (!(filename.endsWith(".func.gii") || filename.endsWith(".shape.gii")))
    {
        CaretLogWarning("metric file '" + filename + "' should be saved ending in .func.gii or .shape.gii, see wb_command -gifti-help");
    }
    caret::GiftiTypeFile::writeFile(filename);
}

/**
 * Clear the surface file.
 */
void 
MetricFile::clear()
{
    GiftiTypeFile::clear();
    this->columnDataPointers.clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
MetricFile::validateDataArraysAfterReading()
{
    this->columnDataPointers.clear();

    this->initializeMembersMetricFile();
        
    this->verifyDataArraysHaveSameNumberOfRows(0, 0);

    bool isLabelData = false;
    
    const int32_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    for (int32_t i = 0; i < numberOfDataArrays; i++) {
        GiftiDataArray* gda = this->giftiFile->getDataArray(i);
        if (gda->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
            if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_LABEL) {
                isLabelData = true;
            }
            gda->convertToDataType(NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32);
        }
        int numDims = gda->getNumberOfDimensions();
        std::vector<int64_t> dims = gda->getDimensions();
        if (numDims == 1 || (numDims == 2 && dims[1] == 1))
        {
            this->columnDataPointers.push_back(gda->getDataPointerFloat());
        } else {
            if (numDims != 2)
            {
                throw DataFileException(getFileName(),
                                        "Invalid number of dimensions in metric file: " + AString::number(numDims));
            }
            if (numberOfDataArrays != 1)
            {
                throw DataFileException(getFileName(),
                                        "Two dimensional data arrays are not allowed in metric files with multiple data arrays");
            }
            std::vector<int64_t> newdims = dims;
            newdims[1] = 1;
            GiftiFile* newFile = new GiftiFile();//convert to multiple 1-d arrays on the fly
            *(newFile->getMetaData()) = *(giftiFile->getMetaData());
            int32_t indices[2], newindices[2] = {0, 0};
            for (indices[1] = 0; indices[1] < dims[1]; ++indices[1])
            {
                GiftiDataArray* tempArray = new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL,
                                                            NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32,
                                                            newdims,
                                                            GiftiEncodingEnum::GZIP_BASE64_BINARY);
                for (indices[0] = 0; indices[0] < dims[0]; ++indices[0])
                {
                    newindices[0] = indices[0];
                    tempArray->setDataFloat32(newindices, gda->getDataFloat32(indices));
                }
                newFile->addDataArray(tempArray);
                newFile->setDataArrayName(indices[1], "#" + AString::number(indices[1] + 1));
                columnDataPointers.push_back(tempArray->getDataPointerFloat());
            }
            delete giftiFile;//delete old 2D file
            giftiFile = newFile;//drop new 1D file in
        }
    }
    
    if (isLabelData) {
        CaretLogWarning("Metric File: "
                        + this->getFileName()
                        + " contains data array with NIFTI_INTENT_LABEL !!!");
    }
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
MetricFile::getNumberOfNodes() const
{
    int32_t numNodes = 0;
    int32_t numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays > 0) {
        numNodes = this->giftiFile->getDataArray(0)->getNumberOfRows();
    }
    return numNodes;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
MetricFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}

/**
 * Initialize members of this class.
 */
void 
MetricFile::initializeMembersMetricFile()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
MetricFile::copyHelperMetricFile(const MetricFile& mf)
{
    this->validateDataArraysAfterReading();

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = mf.m_chartingEnabledForTab[i];
    }
}

/**
 * Get value for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Value at the given node and column indices.
 */
float 
MetricFile::getValue(const int32_t nodeIndex,
                     const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), 
                       "Node Index out of range.");
    
    return this->columnDataPointers[columnIndex][nodeIndex];
}

/**
 * set label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * param value
 *     Value inserted at the given node and column indices.
 */
void 
MetricFile::setValue(const int32_t nodeIndex,
                     const int32_t columnIndex,
                     const float value)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), "Node Index out of range.");
    
    this->columnDataPointers[columnIndex][nodeIndex] = value;
    setModified();
}

const float* 
MetricFile::getValuePointerForColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    return this->columnDataPointers[columnIndex];
}

void MetricFile::setNumberOfNodesAndColumns(int32_t nodes, int32_t columns)
{
    giftiFile->clearAndKeepMetadata();
    columnDataPointers.clear();
    std::vector<int64_t> dimensions;
    dimensions.push_back(nodes);
    for (int32_t i = 0; i < columns; ++i)
    {
        giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL, NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, dimensions, GiftiEncodingEnum::GZIP_BASE64_BINARY));
        columnDataPointers.push_back(giftiFile->getDataArray(i)->getDataPointerFloat());
    }
    setModified();
}

/**
 * Add map(s) to this GIFTI file.
 * @param numberOfNodes
 *     Number of nodes.  If file is not empty, this value must
 *     match the number of nodes that are in the file.
 * @param numberOfMaps
 *     Number of maps to add.
 */
void 
MetricFile::addMaps(const int32_t numberOfNodes,
                       const int32_t numberOfMaps)
{
    if (numberOfNodes <= 0) {
        throw DataFileException(getFileName(),
                                "When adding maps the number of nodes must be greater than zero");
    }
    
    if (this->getNumberOfNodes() > 0) {
        if (numberOfNodes != this->getNumberOfNodes()) {
            throw DataFileException(getFileName(),
                                    "When adding maps the requested number of nodes is "
                                    + AString::number(numberOfNodes)
                                    + " but the file contains "
                                    + AString::number(this->getNumberOfNodes())
                                    + " nodes.");
        }
    }
    
    if (numberOfMaps <= 0) {
        throw DataFileException(getFileName(),
                                "When adding maps, the number of maps must be greater than zero.");
    }
    
    if ((this->getNumberOfNodes() > 0) 
        && (this->getNumberOfMaps() > 0)) {
        std::vector<int64_t> dimensions;
        dimensions.push_back(numberOfNodes);
        
        for (int32_t i = 0; i < numberOfMaps; ++i)
        {
            this->giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL, 
                                                             NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, 
                                                             dimensions, 
                                                             GiftiEncodingEnum::GZIP_BASE64_BINARY));
            const int32_t mapIndex = giftiFile->getNumberOfDataArrays() - 1;
            this->columnDataPointers.push_back(giftiFile->getDataArray(mapIndex)->getDataPointerFloat());
        }
    }
    else {
        this->setNumberOfNodesAndColumns(numberOfNodes, 
                                         numberOfMaps);
    }
    
    this->setModified();
}

void MetricFile::setValuesForColumn(const int32_t columnIndex, const float* valuesIn)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    float* myColumn = columnDataPointers[columnIndex];
    int numNodes = (int)getNumberOfNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        myColumn[i] = valuesIn[i];
    }
    setModified();
}

void MetricFile::initializeColumn(const int32_t columnIndex, const float& value)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    float* myColumn = columnDataPointers[columnIndex];
    int numNodes = (int)getNumberOfNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        myColumn[i] = value;
    }
    setModified();
}

/**
 * Get the minimum and maximum values from ALL maps in this file.
 * Note that not all files (due to size of file) are able to provide
 * the minimum and maximum values from the file.  The return value
 * indicates success/failure.  If the failure (false) is returned
 * the returned values are likely +/- the maximum float values.
 *
 * @param dataRangeMinimumOut
 *    Minimum data value found.
 * @param dataRangeMaximumOut
 *    Maximum data value found.
 * @return
 *    True if the values are valid, else false.
 */
bool
MetricFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    const int32_t numberOfMaps = getNumberOfMaps();

    if (numberOfMaps > 0) {
        dataRangeMaximumOut = -std::numeric_limits<float>::max();
        dataRangeMinimumOut = std::numeric_limits<float>::max();
        
        for (int32_t i = 0; i < numberOfMaps; ++i) {
            GiftiDataArray* gda = this->giftiFile->getDataArray(i);
            float mapMin, mapMax;
            gda->getMinMaxValuesFloat(mapMin,
                                      mapMax);
            
            if (mapMin < dataRangeMinimumOut) {
                dataRangeMinimumOut = mapMin;
            }
            if (mapMax > dataRangeMaximumOut) {
                dataRangeMaximumOut = mapMax;
            }
            
        }
    }
    else {
        dataRangeMaximumOut = std::numeric_limits<float>::max();
        dataRangeMinimumOut = -dataRangeMaximumOut;
    }
    
    return true;
}

/**
 * @return Is charting enabled for this file?
 */
bool
MetricFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
MetricFile::isLineSeriesChartingSupported() const
{
    if (getNumberOfMaps() > 1) {
        return true;
    }
    
    return false;
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
MetricFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
MetricFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
}

/**
 * Load charting data for the surface with the given structure and node index.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndex
 *     Index of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
MetricFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                          const int32_t nodeIndex)
{
    ChartDataCartesian* chartData = NULL;

    if (getStructure() == structure) {
        try {
            const int32_t numMaps = getNumberOfMaps();
            
            std::vector<float> data;
            for (int64_t iMap = 0; iMap < numMaps; iMap++) {
                data.push_back(getValue(nodeIndex,
                                        iMap));
            }
            
            chartData = helpCreateCartesianChartData(data);
            
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setSurfaceNode(getFileName(),
                                       StructureEnum::toName(structure),
                                       getNumberOfNodes(),
                                       nodeIndex);
        }
        catch (const DataFileException& dfe) {
            if (chartData != NULL) {
                delete chartData;
                chartData = NULL;
            }
            
            throw dfe;
        }
    }

    return chartData;
}

/**
 * Load average charting data for the surface with the given structure and node indices.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndices
 *     Indices of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
MetricFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                                  const std::vector<int32_t>& nodeIndices)
{
    ChartDataCartesian* chartData = NULL;
    
    if (getStructure() == structure) {
        ChartDataCartesian* chartData = NULL;
        
        try {
            const int32_t numberOfNodeIndices = static_cast<int32_t>(nodeIndices.size());
            const int32_t numberOfMaps  = getNumberOfMaps();
            
            if ((numberOfNodeIndices > 0)
                && (numberOfMaps > 0)) {
                
                std::vector<double> dataSum(numberOfMaps, 0.0);
                
                for (int32_t iMap = 0; iMap < numberOfMaps; iMap++) {
                    CaretAssertVectorIndex(dataSum, iMap);
                    
                    for (int32_t iNode = 0; iNode < numberOfNodeIndices; iNode++) {
                        const int32_t nodeIndex = nodeIndices[iNode];
                        dataSum[iMap] += getValue(nodeIndex,
                                                  iMap);
                    }
                }
                
                std::vector<float> data;
                for (int32_t iMap = 0; iMap < numberOfMaps; iMap++) {
                    CaretAssertVectorIndex(dataSum, iMap);
                    
                    const float mapAverageValue = dataSum[iMap] / numberOfNodeIndices;
                    data.push_back(mapAverageValue);
                }
                
                chartData = helpCreateCartesianChartData(data);
                ChartDataSource* dataSource = chartData->getChartDataSource();
                dataSource->setSurfaceNodeAverage(getFileName(),
                                                  StructureEnum::toName(structure),
                                                  numberOfNodeIndices,
                                                  nodeIndices);
            }
        }
        catch (const DataFileException& dfe) {
            if (chartData != NULL) {
                delete chartData;
                chartData = NULL;
            }
            
            throw dfe;
        }
        
        return chartData;
    }
    
    return chartData;
}

/**
 * Load charting data for the voxel enclosing the given coordinate.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
MetricFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float * /*xyz[3]*/)
{
    ChartDataCartesian* chartData = NULL; //helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
}

/**
 * Get data from the file as requested in the given map file data selector.
 *
 * @param mapFileDataSelector
 *     Specifies selection of data.
 * @param dataOut
 *     Output with data.  Will be empty if data does not support the map file data selector.
 */
void
MetricFile::getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                  std::vector<float>& dataOut) const
{
    dataOut.clear();
    
    switch (mapFileDataSelector.getDataSelectionType()) {
        case MapFileDataSelector::DataSelectionType::INVALID:
            break;
        case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
            break;
        case MapFileDataSelector::DataSelectionType::ROW_DATA:
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
            try {
                StructureEnum::Enum structure = StructureEnum::INVALID;
                int32_t surfaceNumberOfVertices = -1;
                int32_t vertexIndex = -1;
                mapFileDataSelector.getSurfaceVertex(structure,
                                                     surfaceNumberOfVertices,
                                                     vertexIndex);
                
                if ((structure == getStructure())
                     && (surfaceNumberOfVertices == getNumberOfNodes())) {
                    const int32_t numMaps = getNumberOfMaps();
                    
                    for (int64_t iMap = 0; iMap < numMaps; iMap++) {
                        dataOut.push_back(getValue(vertexIndex,
                                                   iMap));
                    }
                }
            }
            catch (const DataFileException& dfe) {
                CaretLogWarning("Exeception: "
                                + dfe.whatString());
                dataOut.clear();
            }
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
        {
            try {
                StructureEnum::Enum structure = StructureEnum::INVALID;
                int32_t surfaceNumberOfVertices = -1;
                std::vector<int32_t> vertexIndices;
                mapFileDataSelector.getSurfaceVertexAverage(structure,
                                                            surfaceNumberOfVertices,
                                                            vertexIndices);
                
                if ((structure == getStructure())
                     && (surfaceNumberOfVertices == getNumberOfNodes())) {
                         const int32_t numberOfVertexIndices = static_cast<int32_t>(vertexIndices.size());
                         const int32_t numberOfMaps  = getNumberOfMaps();
                         
                         if ((numberOfVertexIndices > 0)
                             && (numberOfMaps > 0)) {
                             
                             std::vector<double> dataSum(numberOfMaps, 0.0);
                             
                             for (int32_t iMap = 0; iMap < numberOfMaps; iMap++) {
                                 CaretAssertVectorIndex(dataSum, iMap);
                                 
                                 for (int32_t iNode = 0; iNode < numberOfVertexIndices; iNode++) {
                                     const int32_t nodeIndex = vertexIndices[iNode];
                                     dataSum[iMap] += getValue(nodeIndex,
                                                               iMap);
                                 }
                             }
                             
                             dataOut.resize(numberOfMaps);
                             for (int32_t iMap = 0; iMap < numberOfMaps; iMap++) {
                                 CaretAssertVectorIndex(dataSum, iMap);
                                 
                                 const float mapAverageValue = dataSum[iMap] / numberOfVertexIndices;
                                 dataOut[iMap] = mapAverageValue;
                             }
                         }
                }
            }
            catch (const DataFileException& dfe) {
                CaretLogWarning("Exeception: "
                                + dfe.whatString());
                dataOut.clear();
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
            break;
    }
}


/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
MetricFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                  SceneClass* sceneClass)
{
    GiftiTypeFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    sceneClass->addBooleanArray("m_chartingEnabledForTab",
                                m_chartingEnabledForTab,
                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
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
MetricFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                       const SceneClass* sceneClass)
{
    GiftiTypeFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    const ScenePrimitiveArray* tabArray = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
    if (tabArray != NULL) {
        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
                                         m_chartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else {
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_chartingEnabledForTab[i] = false;
        }
    }
}




