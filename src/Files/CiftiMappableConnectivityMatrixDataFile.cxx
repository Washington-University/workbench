
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

#define __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__
#include "CiftiMappableConnectivityMatrixDataFile.h"
#undef __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__

#include "CaretAssert.h"
#include "CiftiFile.h"
#include "CaretLogger.h"
#include "ChartableMatrixParcelInterface.h"
#include "ConnectivityDataLoaded.h"
#include "DataFileException.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiMappableConnectivityMatrixDataFile 
 * \brief Data file for Cifti Connectivity Matrix Files.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiMappableConnectivityMatrixDataFile::CiftiMappableConnectivityMatrixDataFile(const DataFileTypeEnum::Enum dataFileType)
: CiftiMappableDataFile(dataFileType)
{
    m_connectivityDataLoaded = new ConnectivityDataLoaded();
    
    /*
     * This method initializes some members
     */
    clearPrivate();

    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_connectivityDataLoaded",
                          "ConnectivityDataLoaded",
                          m_connectivityDataLoaded);
    m_sceneAssistant->add("m_dataLoadingEnabled",
                           &m_dataLoadingEnabled);
}

/**
 * Destructor.
 */
CiftiMappableConnectivityMatrixDataFile::~CiftiMappableConnectivityMatrixDataFile()
{
    clearPrivate();
    
    delete m_connectivityDataLoaded;
    delete m_sceneAssistant;
}

/**
 * Clear the contents of the file.
 */
void
CiftiMappableConnectivityMatrixDataFile::clear()
{
    CiftiMappableDataFile::clear();
    clearPrivate();
}

/**
 * Clear the contents of the file.
 * Note that "clear()" is virtual and cannot be called from destructor.
 */
void
CiftiMappableConnectivityMatrixDataFile::clearPrivate()
{
    m_loadedRowData.clear();
    m_rowLoadedTextForMapName = "";
    m_rowLoadedText = "";
    m_dataLoadingEnabled = true;
    m_connectivityDataLoaded->reset();
    m_chartLoadingDimension = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
    if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC) {
        m_chartLoadingDimension = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
    }
}

/**
 * @return Pointer to the information about last loaded connectivity data.
 */
const ConnectivityDataLoaded*
CiftiMappableConnectivityMatrixDataFile::getConnectivityDataLoaded() const
{
    return m_connectivityDataLoaded;
}

/**
 * Get the nodes for the parcel for the given structure that corresponds
 * to the last selected row.
 *
 * @param parcelNodesOut
 *    Ouput containing the node indices.
 * @param structure
 *    The surface structure.
 */
bool
CiftiMappableConnectivityMatrixDataFile::getParcelNodesElementForSelectedParcel(std::set<int64_t> &parcelNodesOut,
                                                                                const StructureEnum::Enum &structure) const
{
    bool validFlag = false;
    
    int64_t rowIndex = -1;
    int64_t columnIndex = -1;
    m_connectivityDataLoaded->getRowColumnLoading(rowIndex,
                                                  columnIndex);
    if (rowIndex >= 0) {
        validFlag = CiftiMappableDataFile::getParcelNodesElementForSelectedParcel(parcelNodesOut,
                                                           structure,
                                                           rowIndex);
    }
    
    return validFlag;
}

/**
 * @return Is this file empty?
 */
bool
CiftiMappableConnectivityMatrixDataFile::isEmpty() const
{
    if (CiftiMappableDataFile::isEmpty()) {
        return true;
    }
    
    return false;
}

/**
 * @return Is loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 */
bool
CiftiMappableConnectivityMatrixDataFile::isMapDataLoadingEnabled(const int32_t /*mapIndex*/) const
{
    return m_dataLoadingEnabled;
}

/**
 * Set loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 *
 * @param dataLoadingEnabled
 *   New data loading enabled status.
 */
void
CiftiMappableConnectivityMatrixDataFile::setMapDataLoadingEnabled(const int32_t /*mapIndex*/,
                                                          const bool dataLoadingEnabled)
{
    m_dataLoadingEnabled = dataLoadingEnabled;
}

/**
 * Get the data for the given map index.
 *
 * @param mapIndex
 *     Index of the map.
 * @param dataOut
 *     A vector that will contain the data for the map upon exit.
 */
void
CiftiMappableConnectivityMatrixDataFile::getMapData(const int32_t /*mapIndex*/,
                                  std::vector<float>& dataOut) const
{
    if (!isEnabledAsLayer())
    {//TSC: HACK to make identification show empty string instead of number when dynconn is used, then set to not load, not layer
        dataOut.clear();
    } else {
        dataOut = m_loadedRowData;
    }
}

/**
 * Get the index of a row or column when loading data for a surface node.
 *
 * @param structure
 *    Structure of the surface.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the node.
 * @param rowIndexOut
 *    Index of row corresponding to node or -1 if no row in the
 *    matrix corresponds to the node.
 * @param columnIndexOut
 *    Index of column corresponding to node or -1 if no column in the
 *    matrix corresponds to the node.
 */
void
CiftiMappableConnectivityMatrixDataFile::getRowColumnIndexForNodeWhenLoading(const StructureEnum::Enum structure,
                                         const int64_t surfaceNumberOfNodes,
                                         const int64_t nodeIndex,
                                         int64_t& rowIndexOut,
                                         int64_t& columnIndexOut)
{
    rowIndexOut    = -1;
    columnIndexOut = -1;
    
    if (m_ciftiFile == NULL) {
        return;
    }
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    const int32_t ciftiDirection = getCifitDirectionForLoadingRowOrColumn();
    
    /*
     * Make sure number of nodes matches.
     */
    switch (ciftiXML.getMappingType(ciftiDirection))
    {
        case CiftiMappingType::BRAIN_MODELS:
            if (ciftiXML.getBrainModelsMap(ciftiDirection).getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) return;
            break;
        case CiftiMappingType::PARCELS:
            if (ciftiXML.getParcelsMap(ciftiDirection).getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) return;
            break;
        default:
            return;
    }
    /*
     * Get the mapping type
     */
    const CiftiMappingType::MappingType rowMappingType = ciftiXML.getMappingType(ciftiDirection);
    
    int64_t rowOrColumnIndex = -1;
    
    /*
     * Get the row/column index for the node.
     */
    switch (rowMappingType) {
        case CiftiMappingType::BRAIN_MODELS:
            rowOrColumnIndex = ciftiXML.getBrainModelsMap(ciftiDirection).getIndexForNode(nodeIndex, structure);
            break;
        case CiftiMappingType::PARCELS:
            rowOrColumnIndex = ciftiXML.getParcelsMap(ciftiDirection).getIndexForNode(nodeIndex, structure);
            break;
        case CIFTI_INDEX_TYPE_SCALARS:
            break;
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            break;
        default:
            CaretAssert(0);
            CaretLogSevere("Invalid row mapping type for connectivity file "
                           + DataFileTypeEnum::toName(getDataFileType()));
            break;
    }
    
    switch (ciftiDirection) {
        case CiftiXML::ALONG_COLUMN:
            rowIndexOut = rowOrColumnIndex;
            break;
        case CiftiXML::ALONG_ROW:
            columnIndexOut = rowOrColumnIndex;
            break;
        default:
            CaretAssert(0);
            break;
    }
}

/**
 * Get the indices for rows or columns when loading data for a surface nodes.
 *
 * @param structure
 *    Structure of the surface.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndices
 *    Indices of the node.
 * @param rowIndicesOut
 *    Indices of row corresponding to node or empty if no data for rows.
 * @param columnIndicesOut
 *    Index of column corresponding to node or empty if no data for columns
 */
void
CiftiMappableConnectivityMatrixDataFile::getRowColumnIndicesForNodesWhenLoading(const StructureEnum::Enum structure,
                                            const int64_t surfaceNumberOfNodes,
                                            const std::vector<int32_t>& nodeIndices,
                                            std::vector<int64_t>& rowIndicesOut,
                                            std::vector<int64_t>& columnIndicesOut)
{
    rowIndicesOut.clear();
    columnIndicesOut.clear();
    
    for (std::vector<int32_t>::const_iterator iter = nodeIndices.begin();
         iter != nodeIndices.end();
         iter++) {
        int64_t columnIndex = -1;
        int64_t rowIndex    = -1;
        getRowColumnIndexForNodeWhenLoading(structure,
                                            surfaceNumberOfNodes,
                                            *iter,
                                            rowIndex,
                                            columnIndex);
        if (rowIndex >= 0) {
            rowIndicesOut.push_back(rowIndex);
        }
        else if (columnIndex >= 0) {
            columnIndicesOut.push_back(columnIndex);
        }
    }
}

/**
 * Get the average for or column for the given row/column indices.
 *
 * @param rowIndices
 *     Indices of the row.
 * @param columnIndices
 *     Indices of the column.
 * @param rowAverageOut
 *     Average values for rows.
 * @param columnAverageOut
 *     Average value for columns.
 */
void
CiftiMappableConnectivityMatrixDataFile::getRowColumnAverageForIndices(const std::vector<int64_t>& rowIndices,
                                                                       const std::vector<int64_t>& columnIndices,
                                                                       std::vector<float>& rowAverageOut,
                                                                       std::vector<float>& columnAverageOut)
{
    columnAverageOut.clear();
    rowAverageOut.clear();
    
    int64_t dataLength = 0;
    std::vector<int64_t> indices;
    
    bool doRowsFlag = false;
    if (! rowIndices.empty()) {
        dataLength = m_ciftiFile->getNumberOfColumns();
        indices    = rowIndices;
        doRowsFlag = true;
    }
    else if ( ! columnIndices.empty()) {
        dataLength = m_ciftiFile->getNumberOfRows();
        indices    = columnIndices;
    }
    
    const int64_t numIndices = static_cast<int64_t>(indices.size());
    if (numIndices > 0) {
        std::vector<double> sum(dataLength, 0.0);
        std::vector<float>  data(dataLength);
        
        for (std::vector<int64_t>::const_iterator iter = indices.begin();
             iter != indices.end();
             iter++) {
            if (doRowsFlag) {
                getDataForRow(&data[0], *iter);
            }
            else {
                getDataForColumn(&data[0], *iter);
            }
            
            for (int64_t i = 0; i < dataLength; i++) {
                CaretAssertVectorIndex(sum, i);
                CaretAssertVectorIndex(data, i);
                sum[i] += data[i];
            }
        }

        CaretAssert(dataLength > 0);
        std::vector<float> average(dataLength);
        const float floatNumIndices = numIndices; //dataLength;
        for (int64_t i = 0; i < dataLength; i++) {
            CaretAssertVectorIndex(average, i);
            CaretAssertVectorIndex(sum, i);
            average[i] = sum[i] / floatNumIndices;
        }

        if (doRowsFlag) {
            rowAverageOut = average;
        }
        else {
            columnAverageOut = average;
        }
    }
}

/**
 * @return The CIFTI Direction (ALONG_COLUMN, ALONG_ROW) for loading data.
 */
int32_t
CiftiMappableConnectivityMatrixDataFile::getCifitDirectionForLoadingRowOrColumn()
{
    /*
     * Default to loading by row (ALONG_COLUMN) which is how the files normally load.
     */
    int32_t ciftiDirection = CiftiXML::ALONG_COLUMN;
    
    /**
     * Parcel File is able to load by either and implements the 
     * ChartableMatrixInterface.
     */
    ChartableMatrixParcelInterface* matrixParcelInterface = dynamic_cast<ChartableMatrixParcelInterface*>(this);
    if (matrixParcelInterface != NULL) {
        switch (matrixParcelInterface->getMatrixLoadingDimension()) {
            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                ciftiDirection = CiftiXML::ALONG_ROW;
                break;
            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                ciftiDirection = CiftiXML::ALONG_COLUMN;
                break;
        }
    }
    
    return ciftiDirection;
}

/**
 * Get the index of a row or column when loading data for a voxel at a coordinate.
 * @param xyz
 *    Coordinate of the voxel.
 * @param rowIndexOut
 *    Index of row corresponding to voxel or -1 if no row in the
 *    matrix corresponds to the voxel.
 * @param columnIndexOut
 *    Index of column corresponding to voxel or -1 if no column in the
 *    matrix corresponds to the voxel.
 */
void
CiftiMappableConnectivityMatrixDataFile::getRowColumnIndexForVoxelAtCoordinateWhenLoading(const float xyz[3],
                                                      int64_t& rowIndexOut,
                                                      int64_t& columnIndexOut)
{
    rowIndexOut    = -1;
    columnIndexOut = -1;
    
    if (m_ciftiFile == NULL) {
        return;
    }
    
    int64_t ijk[3];
    enclosingVoxel(xyz[0], xyz[1], xyz[2], ijk[0], ijk[1], ijk[2]);
    return getRowColumnIndexForVoxelIndexWhenLoading(ijk,
                                                     rowIndexOut,
                                                     columnIndexOut);
}

/**
 * Get the index of a row or column when loading data for a voxel index.
 * @param ijk
 *    Indicies of the voxel.
 * @param rowIndexOut
 *    Index of row corresponding to voxel or -1 if no row in the
 *    matrix corresponds to the voxel.
 * @param columnIndexOut
 *    Index of column corresponding to voxel or -1 if no column in the
 *    matrix corresponds to the voxel.
 */
void
CiftiMappableConnectivityMatrixDataFile::getRowColumnIndexForVoxelIndexWhenLoading(const int64_t ijk[3],
                                               int64_t& rowIndexOut,
                                               int64_t& columnIndexOut)
{
    
    rowIndexOut    = -1;
    columnIndexOut = -1;
    
    
    if (m_ciftiFile == NULL) {
        return;
    }
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    const int32_t ciftiDirection = getCifitDirectionForLoadingRowOrColumn();
    
    const CiftiMappingType::MappingType rowMappingType = ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN);
    
    int64_t rowOrColumnIndex = -1;
    
    /*
     * Get the mapping type
     */
    if (indexValid(ijk[0], ijk[1], ijk[2])) {
        switch (rowMappingType) {
            case CIFTI_INDEX_TYPE_BRAIN_MODELS:
                rowOrColumnIndex = ciftiXML.getBrainModelsMap(ciftiDirection).getIndexForVoxel(ijk);
                break;
            case CIFTI_INDEX_TYPE_PARCELS:
                rowOrColumnIndex = ciftiXML.getParcelsMap(ciftiDirection).getIndexForVoxel(ijk);
                break;
            default:
                CaretAssert(0);
                CaretLogSevere("Invalid row mapping type for connectivity file "
                               + DataFileTypeEnum::toName(getDataFileType()));
                break;
        }
    }
    
    switch (ciftiDirection) {
        case CiftiXML::ALONG_COLUMN:
            rowIndexOut = rowOrColumnIndex;
            break;
        case CiftiXML::ALONG_ROW:
            columnIndexOut = rowOrColumnIndex;
            break;
        default:
            CaretAssert(0);
            break;
    }
}

/**
 * Set the loaded row data to zeros.
 */
void
CiftiMappableConnectivityMatrixDataFile::setLoadedRowDataToAllZeros()
{
    if ( ! m_loadedRowData.empty()){
        std::fill(m_loadedRowData.begin(),
                  m_loadedRowData.end(),
                  0.0);
    }
    updateForChangeInMapDataWithMapIndex(0);
    m_connectivityDataLoaded->reset();
    m_rowLoadedText.clear();
    m_rowLoadedTextForMapName.clear();
}

/**
 * Reset the loaded row data to empty.
 * 
 * Resets the loaded row data to zero elements and also 
 * clears loaded data attributes.
 *
 * One usage of this method is when a parcel connectivity file has
 * its loading direction changed between row and parcel loading.
 */
void
CiftiMappableConnectivityMatrixDataFile::resetLoadedRowDataToEmpty()
{
    m_loadedRowData.clear();
    setLoadedRowDataToAllZeros();
}

/**
 * Load raw data for the given column.
 *
 * @param dataOut
 *     Output with data.
 * @param index of the column.
 */
void
CiftiMappableConnectivityMatrixDataFile::getDataForColumn(float* dataOut, const int64_t& index) const
{
    m_ciftiFile->getColumn(dataOut,
                           index);
}

/**
 * Load data for the given row.
 *
 * @param dataOut
 *     Output with data.
 * @param index of the row.
 */
void
CiftiMappableConnectivityMatrixDataFile::getDataForRow(float* dataOut, const int64_t& index) const
{
    m_ciftiFile->getRow(dataOut,
                        index);
}

/**
 * Load PROCESSED data for the given column.
 *
 * Some file types may have special processing for a column.  This method can be
 * overridden for those types of files.
 *
 * @param dataOut
 *     Output with data.
 * @param index of the column.
 */
void
CiftiMappableConnectivityMatrixDataFile::getProcessedDataForColumn(float* dataOut, const int64_t& index) const
{
    m_ciftiFile->getColumn(dataOut,
                           index);
}

/**
 * Load PROCESSED data for the given row.
 *
 * Some file types may have special processing for a row.  This method can be
 * overridden for those types of files.
 *
 * @param dataOut
 *     Output with data.
 * @param index of the row.
 */
void
CiftiMappableConnectivityMatrixDataFile::getProcessedDataForRow(float* dataOut, const int64_t& index) const
{
    m_ciftiFile->getRow(dataOut,
                        index);
}

/**
 * Some file types may perform additional processing of row average data and
 * can override this method.
 *
 * @param rowAverageData
 *     The row average data.
 */
void
CiftiMappableConnectivityMatrixDataFile::processRowAverageData(std::vector<float>& /*rowAverageData*/)
{
    /* This method may be overridden by subclasses */
}


/**
 * Load the given row from the file even if the file is disabled.
 *
 * NOTE: Afterwards, it will be necessary to update this file's color mapping
 * with updateScalarColoringForMap().
 *
 *
 * @param rowIndex
 *    Index of row that is loaded.
 * @throw DataFileException
 *    If an error occurs.
 */
void
CiftiMappableConnectivityMatrixDataFile::loadDataForRowIndex(const int64_t rowIndex)
{
    setLoadedRowDataToAllZeros();
    
    const int64_t dataCount = m_ciftiFile->getNumberOfColumns();
    if (dataCount > 0) {
        if ((rowIndex >= 0)
            && (rowIndex < m_ciftiFile->getNumberOfRows())) {
            m_rowLoadedTextForMapName = ("Row: "
                                         + AString::number(rowIndex));
            
            m_rowLoadedText = ("Row_"
                               + AString::number(rowIndex));
            CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiFile->getNumberOfRows()));
            m_loadedRowData.resize(dataCount);
            
            getProcessedDataForRow(&m_loadedRowData[0],
                                   rowIndex);
            
            CaretLogFine("Read row " + AString::number(rowIndex));
            m_connectivityDataLoaded->setRowColumnLoading(rowIndex,
                                                          -1);
        }
    }
    
    updateForChangeInMapDataWithMapIndex(0);
}

/**
 * Load the given column from the file even if the file is disabled.
 *
 * NOTE: Afterwards, it will be necessary to update this file's color mapping
 * with updateScalarColoringForMap().
 *
 *
 * @param columnIndex
 *    Index of row that is loaded.
 * @throw DataFileException
 *    If an error occurs.
 */
void
CiftiMappableConnectivityMatrixDataFile::loadDataForColumnIndex(const int64_t columnIndex)
{
    setLoadedRowDataToAllZeros();
    
    const int64_t dataCount = m_ciftiFile->getNumberOfRows();
    if (dataCount > 0) {
        if ((columnIndex >= 0)
            && (columnIndex < m_ciftiFile->getNumberOfColumns())) {
            m_rowLoadedTextForMapName = ("Column: "
                                         + AString::number(columnIndex));
            
            m_rowLoadedText = ("Column_"
                               + AString::number(columnIndex));
            CaretAssert((columnIndex >= 0) && (columnIndex < m_ciftiFile->getNumberOfColumns()));
            m_loadedRowData.resize(dataCount);
            
            getProcessedDataForColumn(&m_loadedRowData[0],
                                      columnIndex);
            
            CaretLogFine("Read column " + AString::number(columnIndex));
            m_connectivityDataLoaded->setRowColumnLoading(-1,
                                                          columnIndex);
        }
    }
    
    updateForChangeInMapDataWithMapIndex(0);
}

/**
 * Load connectivity data for the surface's node.
 *
 * @param mapIndex
 *    Index of map.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndex
 *    Index of node number.
 * @param rowIndexOut
 *    Index of row corresponding to node or -1 if no row in the
 *    matrix corresponds to the node.
 * @param columnIndexOut
 *    Index of column corresponding to node or -1 if no column in the
 *    matrix corresponds to the node.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiMappableConnectivityMatrixDataFile::loadMapDataForSurfaceNode(const int32_t /*mapIndex*/,
                                                           const int32_t surfaceNumberOfNodes,
                                                           const StructureEnum::Enum structure,
                                                                   const int32_t nodeIndex,
                                                                   int64_t& rowIndexOut,
                                                                   int64_t& columnIndexOut)
{
    if (!isEnabledAsLayer())
    {
        return;//TSC: HACK to do nothing when dynconn layer is disabled
    }
    
    ElapsedTimer timer;
    timer.start();
    
    rowIndexOut    = -1;
    columnIndexOut = -1;
    
    if (m_ciftiFile == NULL) {
        setLoadedRowDataToAllZeros();
        return;
    }
  
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    /*
     * Zero out here so that data only gets cleared when data
     * is to be loaded.
     */
    setLoadedRowDataToAllZeros();
    
    int64_t rowIndex = -1;
    int64_t columnIndex = -1;
    
    try {
        bool dataWasLoaded = false;
        
        getRowColumnIndexForNodeWhenLoading(structure,
                                            surfaceNumberOfNodes,
                                            nodeIndex,
                                            rowIndex,
                                            columnIndex);
        
        if (rowIndex >= 0) {
            int64_t dataCount = m_ciftiFile->getNumberOfColumns();
            if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC) {
                /* 
                 * Dense dynamic is special case where number of rows equals number of brainordinates.
                 * Number of columns is number of time points
                 */
                dataCount = m_ciftiFile->getNumberOfRows();
            }
            
            if (dataCount > 0) {
                m_rowLoadedTextForMapName = ("Row: "
                                         + AString::number(rowIndex)
                                         + ", Node Index: "
                                         + AString::number(nodeIndex)
                                         + ", Structure: "
                                         + StructureEnum::toName(structure));

                m_rowLoadedText = ("Row_"
                                   + AString::number(rowIndex)
                                   + "_Node_Index_"
                                   + AString::number(nodeIndex)
                                   + "_Structure_"
                                   + StructureEnum::toGuiName(structure));
                CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiFile->getNumberOfRows()));
                m_loadedRowData.resize(dataCount);
                getProcessedDataForRow(&m_loadedRowData[0],
                                    rowIndex);
                
                CaretLogFine("Read row for node " + AString::number(nodeIndex));
                
                m_connectivityDataLoaded->setSurfaceNodeLoading(structure,
                                                                surfaceNumberOfNodes,
                                                                nodeIndex,
                                                                rowIndex,
                                                                -1);
                
                rowIndexOut = rowIndex;
                dataWasLoaded = true;
            }
        }
        else if (columnIndex >= 0) {
            const int64_t dataCount = m_ciftiFile->getNumberOfRows();
            if (dataCount > 0) {
                m_rowLoadedTextForMapName = ("Column: "
                                             + AString::number(columnIndex)
                                             + ", Node Index: "
                                             + AString::number(nodeIndex)
                                             + ", Structure: "
                                             + StructureEnum::toName(structure));
                
                m_rowLoadedText = ("Column_"
                                   + AString::number(columnIndex)
                                   + "_Node_Index_"
                                   + AString::number(nodeIndex)
                                   + "_Structure_"
                                   + StructureEnum::toGuiName(structure));
                CaretAssert((columnIndex >= 0) && (columnIndex < m_ciftiFile->getNumberOfColumns()));
                m_loadedRowData.resize(dataCount);
                getProcessedDataForColumn(&m_loadedRowData[0],
                                          columnIndex);
                
                CaretLogFine("Read column for node " + AString::number(nodeIndex));
                
                m_connectivityDataLoaded->setSurfaceNodeLoading(structure,
                                                                surfaceNumberOfNodes,
                                                                nodeIndex,
                                                                -1,
                                                                columnIndex);
                columnIndexOut = columnIndex;
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read data for node " + AString::number(nodeIndex));
            m_connectivityDataLoaded->reset();
        }
    }
    catch (DataFileException& e) {
        m_connectivityDataLoaded->reset();
        throw e;
    }
    
    updateForChangeInMapDataWithMapIndex(0);

    AString msg = ("Time load data for surface node in "
                   + getFileNameNoPath()
                   + " was "
                   + AString::number(timer.getElapsedTimeSeconds())
                   + " seconds.");
    CaretLogInfo(msg);
}



/**
 * Load connectivity data for the surface's nodes and then average the data.
 *
 * @param mapIndex
 *    Index of map.
 * @param surfaceFile
 *    Surface file used for structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndices
 *    Indices of nodes.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiMappableConnectivityMatrixDataFile::loadMapAverageDataForSurfaceNodes(const int32_t /*mapIndex*/,
                                                                   const int32_t surfaceNumberOfNodes,
                                                                   const StructureEnum::Enum structure,
                                                                   const std::vector<int32_t>& nodeIndices)
{
    if (m_ciftiFile == NULL) {
        setLoadedRowDataToAllZeros();
        return;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    /*
     * Zero out here so that data only gets cleared when data
     * is to be loaded.
     */
    setLoadedRowDataToAllZeros();
    
    
    const int32_t numberOfNodeIndices = static_cast<int32_t>(nodeIndices.size());
    if (numberOfNodeIndices <= 0) {
        return;
    }
    
    std::vector<int64_t> rowIndices, columnIndices;
    getRowColumnIndicesForNodesWhenLoading(structure,
                                           surfaceNumberOfNodes,
                                           nodeIndices,
                                           rowIndices,
                                           columnIndices);
    if (rowIndices.empty()
        && columnIndices.empty()) {
        return;
    }
    
    std::vector<float> rowAverage, columnAverage;
    getRowColumnAverageForIndices(rowIndices,
                                  columnIndices,
                                  rowAverage,
                                  columnAverage);

    /*
     * Update the viewed data
     */
    bool dataWasLoaded = false;
    if ( ! rowAverage.empty()) {
        processRowAverageData(rowAverage);
        m_loadedRowData = rowAverage;
        dataWasLoaded = true;
    }
    else if ( ! columnAverage.empty()) {
        m_loadedRowData = columnAverage;
        dataWasLoaded = true;
    }
    
    if (dataWasLoaded) {
        m_rowLoadedTextForMapName = ("Structure: "
                                     + StructureEnum::toName(structure)
                                     + ", Averaged Vertex Count: "
                                     + AString::number(numberOfNodeIndices));
        m_rowLoadedText =  ("Structure_"
                            + StructureEnum::toGuiName(structure)
                            + "_Averaged_Vertex_Count_"
                            + AString::number(numberOfNodeIndices));
    }
    
    if (dataWasLoaded == false) {
        CaretLogFine("FAILED to read data for node average" + AString::fromNumbers(nodeIndices, ","));
    }

    updateForChangeInMapDataWithMapIndex(0);

    if (dataWasLoaded) {
        m_connectivityDataLoaded->setSurfaceAverageNodeLoading(structure,
                                                                surfaceNumberOfNodes,
                                                                nodeIndices);
    }
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * @param mapIndex
 *    Index of map.
 * @param xyz
 *    Coordinate of voxel.
 * @param rowIndexOut
 *    Index of row corresponding to voxel or -1 if no row in the
 *    matrix corresponds to the voxel.
 * @param columnIndexOut
 *    Index of column corresponding to voxel or -1 if no column in the
 *    matrix corresponds to the voxel.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiMappableConnectivityMatrixDataFile::loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                                         const float xyz[3],
                                                                         int64_t& rowIndexOut,
                                                                         int64_t& columnIndexOut)
{
    rowIndexOut    = -1;
    columnIndexOut = -1;
    
    if (mapIndex != 0) {
        setLoadedRowDataToAllZeros();
        CaretAssertMessage(0, "Map index must be zero.");
        return;
    }
    
    if (m_ciftiFile == NULL) {
        setLoadedRowDataToAllZeros();
        return;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    /*
     * Zero out here so that data only gets cleared when data
     * is to be loaded.
     */
    setLoadedRowDataToAllZeros();
    
    int64_t rowIndex = -1;
    int64_t columnIndex = -1;
    bool dataWasLoaded = false;
    
    getRowColumnIndexForVoxelAtCoordinateWhenLoading(xyz,
                                                     rowIndex,
                                                     columnIndex);
    
    if (rowIndex >= 0) {
        int64_t dataCount = m_ciftiFile->getNumberOfColumns();
        if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC) {
            /*
             * Dense dynamic is special case where number of rows equals number of brainordinates.
             * Number of columns is number of time points
             */
            dataCount = m_ciftiFile->getNumberOfRows();
        }
        if (dataCount > 0) {
            m_loadedRowData.resize(dataCount);
            CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiFile->getNumberOfRows()));
            getProcessedDataForRow(&m_loadedRowData[0],
                                   rowIndex);
            
            m_rowLoadedTextForMapName = ("Row: "
                                        + AString::number(rowIndex)
                                        + ", Voxel XYZ: ("
                                        + AString::fromNumbers(xyz, 3, ",")
                                        + ")");
            
            m_rowLoadedText = ("Row_"
                                + AString::number(rowIndex)
                                + "_Voxel_XYZ_"
                                + AString::fromNumbers(xyz, 3, "_").replace('-', 'm'));
            
            CaretLogFine("Read row for voxel " + AString::fromNumbers(xyz, 3, ","));
            
            rowIndexOut = rowIndex;
            dataWasLoaded = true;
        }
    }
    else if (columnIndex >= 0) {
        const int64_t dataCount = m_ciftiFile->getNumberOfRows();
        if (dataCount > 0) {
            m_loadedRowData.resize(dataCount);
            CaretAssert((columnIndex >= 0) && (columnIndex < m_ciftiFile->getNumberOfColumns()));
            getProcessedDataForColumn(&m_loadedRowData[0],
                                      columnIndex);
            
            m_rowLoadedTextForMapName = ("Column: "
                                         + AString::number(columnIndex)
                                         + ", Voxel XYZ: ("
                                         + AString::fromNumbers(xyz, 3, ",")
                                         + ")");
            
            m_rowLoadedText = ("Column_"
                               + AString::number(columnIndex)
                               + "_Voxel_XYZ_"
                               + AString::fromNumbers(xyz, 3, "_").replace('-', 'm'));
            
            CaretLogFine("Read column for voxel " + AString::fromNumbers(xyz, 3, ","));
            
            columnIndexOut = columnIndex;
            dataWasLoaded = true;
        }
    }
    
    if (dataWasLoaded == false) {
        CaretLogFine("FAILED to read row/column for voxel " + AString::fromNumbers(xyz, 3, ","));
    }

    updateForChangeInMapDataWithMapIndex(0);
    
    m_connectivityDataLoaded->setVolumeXYZLoading(xyz,
                                                  rowIndex,
                                                  columnIndex);
}

/**
 * Get row/column indices for the given voxel indices for the loading of data.
 *
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices of voxels.
 * @param rowIndicesOut
 *    Indices of row corresponding to node or empty if no data for rows.
 * @param columnIndicesOut
 *    Index of column corresponding to node or empty if no data for columns
 */
void
CiftiMappableConnectivityMatrixDataFile::getRowColumnIndicesForVoxelsWhenLoading(const int64_t volumeDimensionIJK[3],
                                                                                 const std::vector<VoxelIJK>& voxelIndices,
                                                                                 std::vector<int64_t>& rowIndicesOut,
                                                                                 std::vector<int64_t>& columnIndicesOut)
{
    rowIndicesOut.clear();
    columnIndicesOut.clear();
    
    /*
     * Match dimensions
     */
    std::vector<int64_t> volumeDimensions;
    getDimensions(volumeDimensions);
    if (volumeDimensions.size() < 3) {
        return;
    }
    if ((volumeDimensions[0] != volumeDimensionIJK[0])
        || (volumeDimensions[1] != volumeDimensionIJK[1])
        || (volumeDimensions[2] != volumeDimensionIJK[2])) {
        return;
    }

    for (std::vector<VoxelIJK>::const_iterator iter = voxelIndices.begin();
         iter != voxelIndices.end();
         iter++) {
        
        const VoxelIJK& voxelIJK = *iter;
        
        int64_t rowIndex;
        int64_t columnIndex;
        getRowColumnIndexForVoxelIndexWhenLoading(voxelIJK.m_ijk,
                                                  rowIndex,
                                                  columnIndex);
        if (rowIndex >= 0) {
            rowIndicesOut.push_back(rowIndex);
        }
        else if (columnIndex >= 0) {
            columnIndicesOut.push_back(columnIndex);
        }
    }
}

/**
 * Load connectivity data for the voxel indices and then average the data.
 *
 * @param mapIndex
 *    Index of map.
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices of voxels.
 * @throw
 *    DataFileException if there is an error.
 */
bool
CiftiMappableConnectivityMatrixDataFile::loadMapAverageDataForVoxelIndices(const int32_t mapIndex,
                                                                           const int64_t volumeDimensionIJK[3],
                                                                           const std::vector<VoxelIJK>& voxelIndices)
{
    
    if (mapIndex != 0) { // eliminates compilation warning when compiled for release
        CaretAssert(mapIndex == 0);
        setLoadedRowDataToAllZeros();
    }
    
    if (m_ciftiFile == NULL) {
        setLoadedRowDataToAllZeros();
        return false;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return false;
    }
    
    /*
     * Zero out here so that data only gets cleared when data
     * is to be loaded.
     */
    setLoadedRowDataToAllZeros();
    
    
    std::vector<int64_t> rowIndices, columnIndices;
    getRowColumnIndicesForVoxelsWhenLoading(volumeDimensionIJK,
                                            voxelIndices,
                                            rowIndices,
                                            columnIndices);
    if (rowIndices.empty()
        && columnIndices.empty()) {
        return false;
    }

    std::vector<float> rowAverage, columnAverage;
    getRowColumnAverageForIndices(rowIndices,
                                  columnIndices,
                                  rowAverage,
                                  columnAverage);
    
    bool dataWasLoadedFlag = false;
    if ( ! rowAverage.empty()) {
        processRowAverageData(rowAverage);
        m_loadedRowData = rowAverage;
        dataWasLoadedFlag = true;
    }
    else if ( ! columnAverage.empty()) {
        m_loadedRowData = columnAverage;
        dataWasLoadedFlag = true;
    }
//    if (userCancelled) {
//        m_loadedRowData.clear();
//        m_loadedRowData.resize(dataCount, 0.0);
//    }
    if (dataWasLoadedFlag) {
//        progressEvent.setProgress(numberOfVoxelIndices - 1,
//                                  "Averaging voxel data");
//        EventManager::get()->sendEvent(progressEvent.getPointer());
        
        const int32_t numberOfVoxelIndices = static_cast<int32_t>(voxelIndices.size());
        m_rowLoadedTextForMapName = ("Averaged Voxel Count: "
                                     + AString::number(numberOfVoxelIndices));
        m_rowLoadedText =  ("Averaged_Voxel_Count_"
                            + AString::number(numberOfVoxelIndices));
        
        m_connectivityDataLoaded->setVolumeAverageVoxelLoading(volumeDimensionIJK,
                                                               voxelIndices);
    }
    
    updateForChangeInMapDataWithMapIndex(0);
    
    return dataWasLoadedFlag;
}

/**
 * @return Text describing row loaded that uses
 * underscores as separators.
 */
AString
CiftiMappableConnectivityMatrixDataFile::getRowLoadedText() const
{
    return m_rowLoadedText;
}


/**
 * Get the name of the map at the given index.  For connectivity matrix
 * files this always returns a description of the last data row that 
 * was loaded.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString
CiftiMappableConnectivityMatrixDataFile::getMapName(const int32_t /*mapIndex*/) const
{
    return m_rowLoadedTextForMapName;
}

AString
CiftiMappableConnectivityMatrixDataFile::getRowName(const int32_t rowIndex) const
{
    const CiftiXML& xml = m_ciftiFile->getCiftiXML();
    if (xml.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::PARCELS) return "";//TSC: this was originally implemented only for parcels, dunno why
    const std::vector<CiftiParcelsMap::Parcel>& plist = xml.getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
    CaretAssertVectorIndex(plist, rowIndex);
    return plist[rowIndex].m_name;
}

AString
CiftiMappableConnectivityMatrixDataFile::getColumnName(const int32_t columnIndex) const
{
    const CiftiXML& xml = m_ciftiFile->getCiftiXML();
    if (xml.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::PARCELS) return "";//ditto
    const std::vector<CiftiParcelsMap::Parcel>& plist = xml.getParcelsMap(CiftiXML::ALONG_ROW).getParcels();
    CaretAssertVectorIndex(plist, columnIndex);
    return plist[columnIndex].m_name;
}

/**
 * @return The matrix loading type (by row/column).
 */
ChartMatrixLoadingDimensionEnum::Enum
CiftiMappableConnectivityMatrixDataFile::getChartMatrixLoadingDimension() const
{
    return m_chartLoadingDimension;
}

/**
 * Set the matrix loading type (by row/column).
 *
 * @param matrixLoadingType
 *    New value for matrix loading type.
 */
void
CiftiMappableConnectivityMatrixDataFile::setChartMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum matrixLoadingType)
{
    m_chartLoadingDimension = matrixLoadingType;
    
    resetDataLoadingMembers();
    
    switch (m_chartLoadingDimension) {
        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            break;
        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            break;
    }
    
    initializeAfterReading(getFileName());
    
    resetLoadedRowDataToEmpty();
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
CiftiMappableConnectivityMatrixDataFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                             SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    sceneClass->addEnumeratedType<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>("m_chartLoadingDimension",
                                                                                                          m_chartLoadingDimension);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveSubClassDataToScene(sceneAttributes,
                            sceneClass);
    
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
CiftiMappableConnectivityMatrixDataFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                                  const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    m_connectivityDataLoaded->reset();
    
    /*
     * The chart loading dimension is restored by the scene assistant but
     * we need to call setChartMatrixLoadingDimension() so that loading
     * by row or column is properly setup.
     */
    m_chartLoadingDimension = sceneClass->getEnumeratedTypeValue<ChartMatrixLoadingDimensionEnum, ChartMatrixLoadingDimensionEnum::Enum>("m_chartLoadingDimension",
                                                                                                                                         ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW);
    setChartMatrixLoadingDimension(m_chartLoadingDimension);

    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
    
    /*
     * Loading of data may be disabled in the scene
     * so temporarily enabled loading and then
     * restore the status.
     */
    const int32_t mapIndex = 0;
    const bool loadingEnabledStatus = isMapDataLoadingEnabled(mapIndex);
    
    setMapDataLoadingEnabled(mapIndex, true);
    
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_NONE:
            setLoadedRowDataToAllZeros();
            break;
        case ConnectivityDataLoaded::MODE_ROW:
        {
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getRowColumnLoading(rowIndex,
                                                          columnIndex);
            loadDataForRowIndex(rowIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_COLUMN:
        {
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getRowColumnLoading(rowIndex,
                                                          columnIndex);
            loadDataForColumnIndex(columnIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
        {
            StructureEnum::Enum structure;
            int32_t surfaceNumberOfNodes;
            int32_t surfaceNodeIndex;
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getSurfaceNodeLoading(structure,
                                                            surfaceNumberOfNodes,
                                                            surfaceNodeIndex,
                                                            rowIndex,
                                                            columnIndex);
            loadMapDataForSurfaceNode(mapIndex,
                                      surfaceNumberOfNodes,
                                      structure,
                                      surfaceNodeIndex,
                                      rowIndex,
                                      columnIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE_AVERAGE:
        {
            StructureEnum::Enum structure;
            int32_t surfaceNumberOfNodes;
            std::vector<int32_t> surfaceNodeIndices;
            m_connectivityDataLoaded->getSurfaceAverageNodeLoading(structure,
                                                                   surfaceNumberOfNodes,
                                                                   surfaceNodeIndices);
            loadMapAverageDataForSurfaceNodes(mapIndex,
                                              surfaceNumberOfNodes,
                                              structure,
                                              surfaceNodeIndices);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
        {
            float volumeXYZ[3];
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getVolumeXYZLoading(volumeXYZ,
                                                          rowIndex,
                                                          columnIndex);
            loadMapDataForVoxelAtCoordinate(mapIndex,
                                            volumeXYZ,
                                            rowIndex,
                                            columnIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
        {
            int64_t volumeDimensionsIJK[3];
            std::vector<VoxelIJK> voxelIndicesIJK;
            m_connectivityDataLoaded->getVolumeAverageVoxelLoading(volumeDimensionsIJK,
                                                                   voxelIndicesIJK);
            loadMapAverageDataForVoxelIndices(mapIndex,
                                              volumeDimensionsIJK,
                                              voxelIndicesIJK);
        }
            break;
    }
    
    setMapDataLoadingEnabled(mapIndex,
                             loadingEnabledStatus);
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
CiftiMappableConnectivityMatrixDataFile::saveSubClassDataToScene(const SceneAttributes* /*sceneAttributes*/,
                              SceneClass* /*sceneClass*/)
{
    /* This method is intended only for subclasses to override */
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
CiftiMappableConnectivityMatrixDataFile::restoreSubClassDataFromScene(const SceneAttributes* /*sceneAttributes*/,
                                   const SceneClass* /*sceneClass*/)
{
    /* This method is intended only for subclasses to override */
}

