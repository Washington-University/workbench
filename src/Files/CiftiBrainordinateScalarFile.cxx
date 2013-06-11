
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CIFTI_BRAINORDINATE_SCALAR_FILE_DECLARE__
#include "CiftiBrainordinateScalarFile.h"
#undef __CIFTI_BRAINORDINATE_SCALAR_FILE_DECLARE__

#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"

#include "ChartTypeEnum.h"
#include "CiftiFile.h"
#include "CiftiInterface.h"
#include "CiftiXML.h"
#include "SceneClass.h"
#include "TimeLine.h"

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
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                        CiftiMappableDataFile::FILE_READ_DATA_ALL,
                        CIFTI_INDEX_TYPE_SCALARS,
                        CIFTI_INDEX_TYPE_BRAIN_MODELS,
                        CiftiMappableDataFile::DATA_ACCESS_WITH_COLUMN_METHODS,
                        CiftiMappableDataFile::DATA_ACCESS_WITH_ROW_METHODS)
{
    m_chartingEnabled = false;
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
 * @param ciftiMatrixFile
 *    Cifti connectivity matrix file.
 * @param errorMessageOut
 *    Will describe problem if there is an error.
 * @return
 *    Pointer to the newly created Cifti Scalar File.  If there is an error,
 *    NULL will be returned and errorMessageOut will describe the problem.
 */
CiftiBrainordinateScalarFile*
CiftiBrainordinateScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(const CiftiMappableConnectivityMatrixDataFile* ciftiMatrixFile,
                                                                              AString& errorMessageOut)
{
    errorMessageOut.clear();
 
    const CiftiConnectivityMatrixDenseFile* denseFile =
       dynamic_cast<const CiftiConnectivityMatrixDenseFile*>(ciftiMatrixFile);
    if (denseFile == NULL) {
        errorMessageOut = "Only Cifti Dense Matrix Files are supported for conversion to Cifti Scalar Files.";
        return NULL;
    }
    
    const CiftiInterface* ciftiMatrixInterface = ciftiMatrixFile->m_ciftiInterface;
    
    if (ciftiMatrixFile->getNumberOfMaps() <= 0) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (No Maps).";
        return NULL;
    }
    
    std::vector<float> data;
    ciftiMatrixFile->getMapData(0, data);
    if (data.empty()) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (mapData empty).";
        return NULL;
    }
    
    CiftiBrainordinateScalarFile* scalarFile = NULL;
    
    try {
        CiftiFile* ciftiFile = new CiftiFile();
        
        const CiftiXML& ciftiMatrixXML = ciftiMatrixInterface->getCiftiXML();
        
        /*
         * Copy XML from matrix file
         * and update to be a scalar file.
         */
        CiftiXML ciftiScalarXML = ciftiMatrixXML;
        ciftiScalarXML.copyMapping(CiftiXML::ALONG_COLUMN,
                                   ciftiMatrixXML,
                                   CiftiXML::ALONG_ROW);
        ciftiScalarXML.resetRowsToScalars(1);
        AString mapName = ciftiMatrixFile->getMapName(0);
        ciftiScalarXML.setMapNameForRowIndex(0, mapName);
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

        /*
         * Create name of scalar file
         */
        AString newFileName = ciftiMatrixFile->getFileNameNoExtension();
        newFileName.append("_");
        newFileName.append(ciftiMatrixFile->getRowLoadedText());
        newFileName.append(".");
        newFileName.append(DataFileTypeEnum::toFileExtension(scalarFile->getDataFileType()));
        
        /*
         * Add the CiftiFile to the Scalar file
         */
        scalarFile->initializeFromCiftiInterface(ciftiFile, newFileName);
        scalarFile->setModified();
        
        return scalarFile;
    }
    catch (const CiftiFileException& ce) {
        if (scalarFile != NULL) {
            delete scalarFile;
        }
        errorMessageOut = ce.whatString();
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
CiftiBrainordinateScalarFile::isChartingEnabled() const
{
    return m_chartingEnabled;
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiBrainordinateScalarFile::isChartingSupported() const
{
    if (getNumberOfMaps() > 1) {
        return true;
    }
    
    return false;
}


ChartTypeEnum::Enum CiftiBrainordinateScalarFile::getDefaultChartType() const
{
    return ChartTypeEnum::PLOT2D;
}

void CiftiBrainordinateScalarFile::getSupportedChartTypes(std::vector<ChartTypeEnum::Enum> &list) const
{
    list.clear();
    list.push_back(ChartTypeEnum::PLOT2D);    
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiBrainordinateScalarFile::setChartingEnabled(const bool enabled)
{
    m_chartingEnabled = enabled;
}

/**
 * Load the average of chart data for a group of surface nodes.
 * Note: This method will return a chart even if charting for
 * this file is disabled.
 *
 * @param structure
 *     The surface's structure
 * @param nodeIndices
 *     Indices of nodes whose chart data is averaged
 * @param timeLineOut
 *     Output charting data.
 * @return
 *     True if chart data is valid, else false.
 */
bool
CiftiBrainordinateScalarFile::loadAverageChartForSurfaceNodes(const StructureEnum::Enum structure,
                                                                  const std::vector<int32_t>& nodeIndices,
                                                                  TimeLine& timeLineOut) throw (DataFileException)
{
    std::vector<double> dataAverage;
    double numValidNodes = 0.0;
    
    std::vector<float> data;
    
    const int64_t numNodeIndices = static_cast<int64_t>(nodeIndices.size());
    for (int64_t iNode = 0; iNode < numNodeIndices; iNode++) {
        if (getSeriesDataForSurfaceNode(structure,
                                        nodeIndices[iNode],
                                        data)) {
            const int64_t numData = static_cast<int64_t>(data.size());
            if (numData > 0) {
                if (dataAverage.empty()) {
                    dataAverage.resize(numData,
                                       0.0);
                }
                
                for (int64_t iData = 0; iData < numData; iData++) {
                    dataAverage[iData] += data[iData];
                    numValidNodes += 1.0;
                }
            }
        }
    }
    
    if (numValidNodes > 0.0) {
        const int64_t numData = static_cast<int64_t>(dataAverage.size());
        for (int64_t i = 0; i < numData; i++) {
            dataAverage[i] /= numValidNodes;
        }
        
        timeLineOut.x.resize(numData);
        timeLineOut.y.resize(numData);
        for (int64_t i = 0; i < numData; i++) {
            timeLineOut.x[i] = i;
            timeLineOut.y[i] = data[i];
        }
        timeLineOut.nodeid = 0;
        timeLineOut.type = AVERAGE;
        timeLineOut.id = this;
        timeLineOut.filename = getFileName();
        
        float start, step;
        getMapIntervalStartAndStep(start, step);
        timeLineOut.timeStep = step;
        
        return true;
    }
    
    return false;
}

/**
 * Load chart data for a surface node
 * Note: This method will return a chart even if charting for
 * this file is disabled.
 *
 * @param structure
 *     The surface's structure
 * @param nodeIndex
 *     Index of node
 * @param timeLineOut
 *     Output charting data.
 * @return
 *     True if chart data is valid, else false.
 */
bool
CiftiBrainordinateScalarFile::loadChartForSurfaceNode(const StructureEnum::Enum structure,
                                                          const int32_t nodeIndex,
                                                          TimeLine& timeLineOut) throw (DataFileException)
{
    std::vector<float> data;
    if (getSeriesDataForSurfaceNode(structure,
                                    nodeIndex,
                                    data)) {
        const int64_t numData = static_cast<int64_t>(data.size());
        timeLineOut.nodeid = nodeIndex;
        timeLineOut.x.resize(numData);
        timeLineOut.y.resize(numData);
        for (int64_t i = 0; i < numData; i++) {
            timeLineOut.x[i] = i;
            timeLineOut.y[i] = data[i];
        }
        timeLineOut.id = this;
        timeLineOut.filename = getFileName();
        
        float start, step;
        getMapIntervalStartAndStep(start, step);
        timeLineOut.timeStep = step;
        
        return true;
    }
    
    return false;
}

/**
 * Load chart data for a voxel
 * Note: This method will return a chart even if charting for
 * this file is disabled.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @param timeLineOut
 *     Output charting data.
 * @return
 *     True if chart data is valid, else false.
 */
bool
CiftiBrainordinateScalarFile::loadChartForVoxelAtCoordinate(const float xyz[3],
                                                                TimeLine& timeLineOut) throw (DataFileException)
{
    std::vector<float> data;
    if (getSeriesDataForVoxelAtCoordinate(xyz, data)) {
        const int64_t numData = static_cast<int64_t>(data.size());
        timeLineOut.x.resize(numData);
        timeLineOut.y.resize(numData);
        for (int64_t i = 0; i < numData; i++) {
            timeLineOut.x[i] = i;
            timeLineOut.y[i] = data[i];
        }
        timeLineOut.id = this;
        timeLineOut.filename = getFileName();
        
        timeLineOut.label = "Voxel XYZ:[" + AString::fromNumbers(xyz,3,AString(", ")) + "]";
        
        float start, step;
        getMapIntervalStartAndStep(start, step);
        timeLineOut.timeStep = step;
        
        return true;
    }
    
    return false;
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 */
CaretMappableDataFile*
CiftiBrainordinateScalarFile::getCaretMappableDataFile()
{
    return dynamic_cast<CaretMappableDataFile*>(this);
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 */
const CaretMappableDataFile*
CiftiBrainordinateScalarFile::getCaretMappableDataFile() const
{
    return dynamic_cast<const CaretMappableDataFile*>(this);
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
    
    sceneClass->addBoolean("m_chartingEnabled",
                           m_chartingEnabled);
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
    
    m_chartingEnabled = sceneClass->getBooleanValue("m_chartingEnabled",
                                                    false);
}


