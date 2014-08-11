
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

#define __CIFTI_BRAINORDINATE_SCALAR_FILE_DECLARE__
#include "CiftiBrainordinateScalarFile.h"
#undef __CIFTI_BRAINORDINATE_SCALAR_FILE_DECLARE__

#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "SceneClass.h"
#include "SceneClassArray.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateScalarFile 
 * \brief CIFTI Brainordinate by Scalar File
 * \ingroup Files
 *
 */

/**
 * Constructor.
 */
CiftiBrainordinateScalarFile::CiftiBrainordinateScalarFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
}

/**
 * Destructor.
 */
CiftiBrainordinateScalarFile::~CiftiBrainordinateScalarFile()
{
    
}

/**
 * Create a Cifti Scalar File using the currently loaded row in a Cifti 
 * connectivity matrix file.
 *
 * @param sourceCiftiMatrixFile
 *    Cifti connectivity matrix file.
 * @param errorMessageOut
 *    Will describe problem if there is an error.
 * @return
 *    Pointer to the newly created Cifti Scalar File.  If there is an error,
 *    NULL will be returned and errorMessageOut will describe the problem.
 */
CiftiBrainordinateScalarFile*
CiftiBrainordinateScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(const CiftiMappableConnectivityMatrixDataFile* sourceCiftiMatrixFile,
                                                                              AString& errorMessageOut)
{
    errorMessageOut.clear();
 
    const CiftiConnectivityMatrixDenseFile* denseFile =
       dynamic_cast<const CiftiConnectivityMatrixDenseFile*>(sourceCiftiMatrixFile);
    if (denseFile == NULL) {
        errorMessageOut = "Only Cifti Dense Matrix Files are supported for conversion to Cifti Scalar Files.";
        return NULL;
    }
    
    const CiftiFile* sourceCiftiFile = sourceCiftiMatrixFile->m_ciftiFile;
    
    if (sourceCiftiMatrixFile->getNumberOfMaps() <= 0) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (No Maps).";
        return NULL;
    }
    
    std::vector<float> data;
    sourceCiftiMatrixFile->getMapData(0, data);
    if (data.empty()) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (mapData empty).";
        return NULL;
    }
    
    CiftiBrainordinateScalarFile* scalarFile = NULL;
    
    try {
        CiftiFile* ciftiFile = new CiftiFile();
        
        /*
         * Copy XML from matrix file
         * and update to be a scalar file.
         */
        const CiftiXML& ciftiMatrixXML = sourceCiftiFile->getCiftiXML();
        CiftiXML ciftiScalarXML = ciftiMatrixXML;
        CiftiBrainModelsMap brainModelsMap = ciftiMatrixXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
        CiftiScalarsMap scalarsMap;
        scalarsMap.setLength(1);
        scalarsMap.setMapName(0,
                              sourceCiftiMatrixFile->getMapName(0));
        ciftiScalarXML.setMap(CiftiXML::ALONG_ROW,
                              scalarsMap);
        ciftiScalarXML.setMap(CiftiXML::ALONG_COLUMN,
                              brainModelsMap);
        ciftiFile->setCiftiXML(ciftiScalarXML);
        
        /*
         * Add data to the file
         */
        ciftiFile->setColumn(&data[0],
                             0);
        
        /*
         * Create a scalar file
         */
        CiftiBrainordinateScalarFile* scalarFile = new CiftiBrainordinateScalarFile();
        scalarFile->m_ciftiFile.grabNew(ciftiFile);

        /*
         * Create name of scalar file
         */
        AString newFileName = sourceCiftiMatrixFile->getFileNameNoExtension();
        newFileName.append("_");
        newFileName.append(sourceCiftiMatrixFile->getRowLoadedText());
        newFileName.append(".");
        newFileName.append(DataFileTypeEnum::toFileExtension(scalarFile->getDataFileType()));
        scalarFile->setFileName(newFileName);
        
        /*
         * Add the CiftiFile to the Scalar file
         */
        scalarFile->initializeAfterReading(newFileName);
        scalarFile->setModified();
        
        return scalarFile;
    }
    catch (const DataFileException& de) {
        if (scalarFile != NULL) {
            delete scalarFile;
        }
        errorMessageOut = de.whatString();
    }
    
    return NULL;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiBrainordinateScalarFile::isBrainordinateChartingEnabled(const int32_t tabIndex) const
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
CiftiBrainordinateScalarFile::isBrainordinateChartingSupported() const
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
CiftiBrainordinateScalarFile::setBrainordinateChartingEnabled(const int32_t tabIndex,
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
CiftiBrainordinateScalarFile::getSupportedBrainordinateChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedBrainordinateChartDataTypes(chartDataTypesOut);
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
 *     the returned pointer will return true.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateScalarFile::loadBrainordinateChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                               const int32_t nodeIndex) throw (DataFileException)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNode(structure,
                                                           nodeIndex);
    return chartData;

//    ChartDataCartesian* chartData = NULL;
//    
//    try {
//        std::vector<float> data;
//        if (getSeriesDataForSurfaceNode(structure,
//                                        nodeIndex,
//                                        data)) {
//            const int64_t numData = static_cast<int64_t>(data.size());
//            
//            chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES,
//                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
//                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
//            for (int64_t i = 0; i < numData; i++) {
//                float xValue = i;
//                chartData->addPoint(xValue,
//                                    data[i]);
//            }
//            
//            const AString description = (getFileNameNoPath()
//                                         + " node "
//                                         + AString::number(nodeIndex));
//            chartData->setDescription(description);
//        }
//    }
//    catch (const DataFileException& dfe) {
//        if (chartData != NULL) {
//            delete chartData;
//            chartData = NULL;
//        }
//        
//        throw dfe;
//    }
//    
//    return chartData;
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
CiftiBrainordinateScalarFile::loadAverageBrainordinateChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                                      const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNodeAverage(structure,
                                                                  nodeIndices);
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
CiftiBrainordinateScalarFile::loadBrainordinateChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    ChartDataCartesian* chartData = helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
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
CiftiBrainordinateScalarFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
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
CiftiBrainordinateScalarFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    const ScenePrimitiveArray* tabArray = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
    if (tabArray != NULL) {
        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
                                         m_chartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else {
        /*
         * Obsolete value when charting was not 'per tab'
         */
        const bool chartingEnabled = sceneClass->getBooleanValue("m_chartingEnabled",
                                                                 false);
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_chartingEnabledForTab[i] = chartingEnabled;
        }
    }
}


