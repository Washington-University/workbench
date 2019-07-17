
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __METRIC_DYNAMIC_CONNECTIVITY_FILE_DECLARE__
#include "MetricDynamicConnectivityFile.h"
#undef __METRIC_DYNAMIC_CONNECTIVITY_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ConnectivityCorrelation.h"
#include "ConnectivityDataLoaded.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "SceneClassAssistant.h"
using namespace caret;


    
/**
 * \class caret::MetricDynamicConnectivityFile 
 * \brief Dynamic connectivity from metric file
 * \ingroup Files
 */

/**
 * Constructor.
 * @param parentMetricFile
 *     The parent metric file.
 */
MetricDynamicConnectivityFile::MetricDynamicConnectivityFile(MetricFile* parentMetricFile)
: MetricFile(DataFileTypeEnum::METRIC_DYNAMIC),
m_parentMetricFile(parentMetricFile)
{
    CaretAssert(m_parentMetricFile);
    
    m_connectivityDataLoaded.reset(new ConnectivityDataLoaded());
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_dataLoadingEnabledFlag",
                          &m_dataLoadingEnabledFlag);
    m_sceneAssistant->add("m_enabledAsLayer",
                          &m_enabledAsLayer);
    m_sceneAssistant->add("m_connectivityDataLoaded",
                          "ConnectivityDataLoaded",
                          m_connectivityDataLoaded.get());
}

/**
 * Destructor.
 */
MetricDynamicConnectivityFile::~MetricDynamicConnectivityFile()
{
}

/**
 * Clear the file.
 */
void
MetricDynamicConnectivityFile::clear()
{
    MetricFile::clear();
    clearPrivateData();
}

/**
 * Clear the file.
 */
void
MetricDynamicConnectivityFile::clearPrivateData()
{
    m_numberOfVertices = 0;
    m_validDataFlag = false;
    m_enabledAsLayer = false;
    m_connectivityCorrelation.reset();
    m_connectivityDataLoaded->reset();
}

/**
 * @return Pointer to the information about last loaded connectivity data.
 */
const ConnectivityDataLoaded*
MetricDynamicConnectivityFile::getConnectivityDataLoaded() const
{
    return m_connectivityDataLoaded.get();
}

/**
 * @return True if enabled as a layer.
 */
bool
MetricDynamicConnectivityFile::isEnabledAsLayer() const
{
    return m_enabledAsLayer;
}

/**
 * Set enabled as a layer.
 *
 * @param True if enabled as a layer.
 */
void
MetricDynamicConnectivityFile::setEnabledAsLayer(const bool enabled)
{
    m_enabledAsLayer = enabled;
}

/**
 * @return True if data loading enabled.
 */
bool
MetricDynamicConnectivityFile::isDataLoadingEnabled() const
{
    return m_dataLoadingEnabledFlag;
}

/**
 * Set data loading enabled.
 *
 * @param True if data loading enabled.
 */
void
MetricDynamicConnectivityFile::setDataLoadingEnabled(const bool enabled)
{
    m_dataLoadingEnabledFlag = enabled;
}

/**
 * Initialize the file using information from parent volume file
 */
void
MetricDynamicConnectivityFile::initializeFile()
{
    clearPrivateData();
    
    CaretAssert(m_parentMetricFile);
    m_numberOfVertices = m_parentMetricFile->getNumberOfNodes();
    const int32_t numberOfMaps = 1;
    setNumberOfNodesAndColumns(m_numberOfVertices,
                               numberOfMaps);
    setStructure(m_parentMetricFile->getStructure());
    
    CaretAssert(getNumberOfNodes() == m_numberOfVertices);
    CaretAssert(getNumberOfMaps() == numberOfMaps);
    
    AString path, nameNoExt, ext;
    FileInformation fileInfo(m_parentMetricFile->getFileName());
    fileInfo.getFileComponents(path, nameNoExt, ext);
    setFileName(FileInformation::assembleFileComponents(path,
                                                        nameNoExt,
                                                        DataFileTypeEnum::toFileExtension(DataFileTypeEnum::METRIC_DYNAMIC)));
    clearVertexValues();
    clearModified();
    
    m_validDataFlag = true;
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Dense files do NOT support writing.
 */
bool
MetricDynamicConnectivityFile::supportsWriting() const
{
    return false;
}

/**
 * @return The parent volume file
 */
MetricFile*
MetricDynamicConnectivityFile::getParentMetricFile()
{
    return const_cast<MetricFile*>(m_parentMetricFile);
}

/**
 * @return The parent metric file (const method)
 */
const MetricFile*
MetricDynamicConnectivityFile::getParentMetricFile() const
{
    return m_parentMetricFile;
}

/**
 * @return True if the data is valid
 */
bool
MetricDynamicConnectivityFile::isDataValid() const
{
    return m_validDataFlag;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
MetricDynamicConnectivityFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    MetricFile::addToDataFileContentInformation(dataFileInformation);
}

/**
 * Read the file with the given name.
 *
 * @param filename
 *     Name of file
 * @throws DataFileException
 *     If error occurs
 */
void
MetricDynamicConnectivityFile::readFile(const AString& /*filename*/)
{
    throw DataFileException("Read of Metric Dynamic Connectivity File is not allowed");
}

/**
 * Read the file with the given name.
 *
 * @param filename
 *     Name of file
 * @throws DataFileException
 *     If error occurs
 */
void
MetricDynamicConnectivityFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("Writing of Metric Dynamic Connectivity File is not allowed");
}

/**
 * Clear voxels in this volume
 */
void
MetricDynamicConnectivityFile::clearVertexValues()
{
    CaretAssert(getNumberOfMaps() == 1);
    const int32_t mapIndex(0);
    const float value(0.0f);
    initializeColumn(mapIndex,
                     value);
    m_dataLoadedName = "";
}

/**
 * Load connectivity data for the surface's node.
 *
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndex
 *    Index of node number.
 * @return
 *    True if data was loaded, else false.
 */
bool
MetricDynamicConnectivityFile::loadDataForSurfaceNode(const int32_t surfaceNumberOfNodes,
                                                      const StructureEnum::Enum structure,
                                                      const int32_t nodeIndex)
{
    bool validFlag(false);
    
    if ( ! isDataValid()) {
        return validFlag;
    }
    if ( ! m_dataLoadingEnabledFlag) {
        return validFlag;
    }
    if (getStructure() != structure) {
        return validFlag;
    }
    if (getNumberOfNodes() != surfaceNumberOfNodes) {
        return validFlag;
    }
    
    clearVertexValues();
    m_connectivityDataLoaded->reset();
    
    std::vector<float> data;
    if (getConnectivityForVertexIndex(nodeIndex, data)) {
        CaretAssert(m_numberOfVertices == static_cast<int64_t>(data.size()));
        float* dataPointer = const_cast<float*>(getValuePointerForColumn(0));
        CaretAssert(dataPointer);
        std::copy(data.begin(),
                  data.end(),
                  dataPointer);
        
        validFlag = true;
        
        m_connectivityDataLoaded->setSurfaceNodeLoading(getStructure(),
                                                        getNumberOfNodes(),
                                                        nodeIndex,
                                                        -1, -
                                                        1);
    }
    
    const int32_t mapIndex(0);
    const AString mapName("Vertex_Index_"
                           + AString::number(nodeIndex)
                           + "_Structure_"
                           + StructureEnum::toGuiName(structure));
    setMapName(mapIndex,
               mapName);
    m_dataLoadedName = mapName;
    
    updateAfterFileDataChanges();
    updateScalarColoringForMap(0);
   
    return validFlag;
}



/**
 * Load connectivity data for the surface's nodes and then average the data.
 *
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndices
 *    Indices of nodes.
 * @return
 *    True if data was loaded, else false.
 */
bool
MetricDynamicConnectivityFile::loadAverageDataForSurfaceNodes(const int32_t surfaceNumberOfNodes,
                                                              const StructureEnum::Enum structure,
                                                              const std::vector<int32_t>& nodeIndices)
{
    bool validFlag(false);
    
    if ( ! isDataValid()) {
        return validFlag;
    }
    if ( ! m_dataLoadingEnabledFlag) {
        return validFlag;
    }
    if (getStructure() != structure) {
        return validFlag;
    }
    if (getNumberOfNodes() != surfaceNumberOfNodes) {
        return validFlag;
    }
    if (nodeIndices.size() < 2) {
        return validFlag;
    }
    
    ConnectivityCorrelation* connCorrelation = getConnectivityCorrelation();
    if (connCorrelation == NULL) {
        return validFlag;
    }
    
    clearVertexValues();
    m_connectivityDataLoaded->reset();

    float* dataPointer = const_cast<float*>(getValuePointerForColumn(0));
    CaretAssert(dataPointer);

    std::vector<int64_t> nodeIndices64(nodeIndices.begin(), nodeIndices.end());
    std::vector<float> data(getNumberOfNodes());
    connCorrelation->getCorrelationForBrainordinateROI(nodeIndices64,
                                                       data);
    const int64_t numData = static_cast<int64_t>(data.size());
    validFlag = (numData == getNumberOfNodes());
    if (validFlag) {
        for (int64_t i = 0; i < numData; i++) {
            dataPointer[i] = data[i];
        }
    }
    
    
    m_connectivityDataLoaded->setSurfaceAverageNodeLoading(getStructure(),
                                                           getNumberOfNodes(),
                                                           nodeIndices);

    const AString mapName("Average_Vertex_Count_"
                          + AString::number(static_cast<int32_t>(nodeIndices.size())));
    m_dataLoadedName = mapName;
    const int32_t mapIndex(0);
    setMapName(mapIndex,
               mapName);
    
    updateAfterFileDataChanges();
    updateScalarColoringForMap(0);

    return validFlag;
}

/**
 * Get the connectivity for the given vertex index.
 * If the vertex index is invalid, zeros are loaded into all voxels.
 *
 * @param vertexIndex
 *     The vertex index.
 * @param vertexDataOut
 *     Output containing vertex data
 * @return
 *     True if data was loaded.
 */
bool
MetricDynamicConnectivityFile::getConnectivityForVertexIndex(const int32_t vertexIndex,
                                                             std::vector<float>& vertexDataOut)
{
    bool validFlag(false);
    if ((vertexIndex >= 0)
        && (vertexIndex < getNumberOfNodes())) {
        ConnectivityCorrelation* connCorrelation = getConnectivityCorrelation();
        
        if (connCorrelation) {
            connCorrelation->getCorrelationForBrainordinate(vertexIndex,
                                                            vertexDataOut);
            CaretAssert(m_numberOfVertices == static_cast<int64_t>(vertexDataOut.size()));
            validFlag = true;
        }
    }

    if ( ! validFlag) {
        vertexDataOut.resize(m_numberOfVertices);
        std::fill(vertexDataOut.begin(), vertexDataOut.end(),
                  0.0f);
    }
    
    updateAfterFileDataChanges();
    invalidateHistogramChartColoring();
    
    return validFlag;
}

/**
 * @return Pointer to connectivity correlation or NULL if not valid
 */
ConnectivityCorrelation*
MetricDynamicConnectivityFile::getConnectivityCorrelation()
{
    if ( ! m_connectivityCorrelationFailedFlag) {
        if (m_connectivityCorrelation == NULL) {
            /*
             * Need data and timepoint count from parent file
             */
            CaretAssert(m_parentMetricFile);
            const int32_t numberOfTimePoints = m_parentMetricFile->getNumberOfMaps();
            CaretAssert(numberOfTimePoints >= 2);
            std::vector<const float*> timePointData;
            for (int32_t i = 0; i < numberOfTimePoints; i++) {
                const float* dataPtr = m_parentMetricFile->getValuePointerForColumn(i);
                CaretAssert(dataPtr);
                timePointData.push_back(dataPtr);
            }
            const int64_t brainordinateStride(1);
            AString errorMessage;
            ConnectivityCorrelation* cc = ConnectivityCorrelation::newInstanceTimePoints(timePointData,
                                                                                         m_numberOfVertices,
                                                                                         brainordinateStride,
                                                                                         errorMessage);
            if (cc != NULL) {
                m_connectivityCorrelation.reset(cc);
            }
            else {
                m_connectivityCorrelationFailedFlag = true;
                CaretLogSevere("Failed to create connectvity correlation for "
                               + m_parentMetricFile->getFileNameNoPath());
            }
        }
    }
    
    return m_connectivityCorrelation.get();
}

/**
 * @return A metric file using the loaded data (will return NULL if there is an error).
 *
 * @param directoryName
 *     Directory for file
 * @param errorMessageOut
 *     Contains error information
 */
MetricFile*
MetricDynamicConnectivityFile::newMetricFileFromLoadedData(const AString& directoryName,
                                                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    bool validDataFlag(false);
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_COLUMN:
            break;
        case ConnectivityDataLoaded::MODE_NONE:
            break;
        case ConnectivityDataLoaded::MODE_ROW:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
            validDataFlag = true;
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE_AVERAGE:
            validDataFlag = true;
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
            break;
    }
    
    if ( ! validDataFlag) {
        errorMessageOut = "No metric connectivity data is loaded";
        return NULL;
    }
    
    MetricFile* mf(NULL);
    
    try {
        const int32_t numVertices = getNumberOfNodes();
        mf = new MetricFile();
        mf->setStructure(getStructure());
        mf->setNumberOfNodesAndColumns(numVertices,
                                       1);
        mf->setValuesForColumn(0, this->getValuePointerForColumn(0));
        
        /*
         * May need to convert a remote path to a local path
         */
        FileInformation fileNameInfo(getFileName());
        const AString metricFileName = fileNameInfo.getAsLocalAbsoluteFilePath(directoryName,
                                                                               mf->getDataFileType());

        /*
         * Create name of metric file data loaded  information
         */
        FileInformation metricFileInfo(metricFileName);
        AString thePath, theName, theExtension;
        metricFileInfo.getFileComponents(thePath,
                                         theName,
                                         theExtension);
        theName.append("_" + m_dataLoadedName);
        AString newFileName = FileInformation::assembleFileComponents(thePath,
                                                                      theName,
                                                                      theExtension);
        mf->setFileName(newFileName);
        mf->setMapName(0, m_dataLoadedName);

        /*
         * Need to copy color palette since it may be the default
         */
        PaletteColorMapping* metricPalette = mf->getMapPaletteColorMapping(0);
        CaretAssert(metricPalette);
        const PaletteColorMapping* myPalette = getMapPaletteColorMapping(0);
        CaretAssert(myPalette);
        metricPalette->copy(*myPalette,
                             true);

        mf->updateAfterFileDataChanges();
        mf->updateScalarColoringForMap(0);
        mf->setModified();
    }
    catch (const DataFileException& dfe) {
        errorMessageOut = dfe.whatString();
        if (mf != NULL) {
            delete mf;
            mf = NULL;
        }
    }
    
    return mf;
}


/**
 * Save data to the scene.
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
MetricDynamicConnectivityFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                   SceneClass* sceneClass)
{
    MetricFile::saveFileDataToScene(sceneAttributes,
                                    sceneClass);
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
MetricDynamicConnectivityFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                        const SceneClass* sceneClass)
{
    m_connectivityDataLoaded->reset();
    
    MetricFile::restoreFileDataFromScene(sceneAttributes,
                                         sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_COLUMN:
            break;
        case ConnectivityDataLoaded::MODE_NONE:
            break;
        case ConnectivityDataLoaded::MODE_ROW:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfVertices(-1);
            int32_t vertexIndex(-1);
            int64_t rowIndex(-1);
            int64_t columnIndex(-1);
            m_connectivityDataLoaded->getSurfaceNodeLoading(structure,
                                                            surfaceNumberOfVertices,
                                                            vertexIndex,
                                                            rowIndex,
                                                            columnIndex);
            if (vertexIndex >= 0) {
                loadDataForSurfaceNode(surfaceNumberOfVertices,
                                       structure,
                                       vertexIndex);
            }
        }
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE_AVERAGE:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfVertices(-1);
            std::vector<int32_t> vertexIndices;
            m_connectivityDataLoaded->getSurfaceAverageNodeLoading(structure,
                                                                   surfaceNumberOfVertices,
                                                                   vertexIndices);
            if ( ! vertexIndices.empty()) {
                loadAverageDataForSurfaceNodes(surfaceNumberOfVertices,
                                               structure,
                                               vertexIndices);
            }
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
            break;
    }
}

