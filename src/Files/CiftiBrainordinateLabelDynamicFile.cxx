
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_DECLARE__
#include "CiftiBrainordinateLabelDynamicFile.h"
#undef __CIFTI_BRAINORDINATE_LABEL_DYNAMIC_FILE_DECLARE__

#include "CaretAssert.h"
#include "CiftiFile.h"
#include "CaretLogger.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GroupAndNameHierarchyModel.h"
#include "LabelDrawingProperties.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateLabelDynamicFile 
 * \brief Dynamic Label File created by thresholding scalar data
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiBrainordinateLabelDynamicFile::CiftiBrainordinateLabelDynamicFile(CaretMappableDataFile* parentMappableDataFile)
: CiftiBrainordinateLabelFile(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC),
m_parentMappableDataFile(parentMappableDataFile)
{
    CaretAssert(m_parentMappableDataFile);
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
}

/**
 * Create a label dynamic threshold file for the given file
 *
 * @param ciftiMapFile
 *     The CIFTI map file.
 * @param errorMessageOut
 *     Output with description of error.
 * @retrurn
 *     Pointer to file created or NULL if there was an error.
 */
CiftiBrainordinateLabelDynamicFile*
CiftiBrainordinateLabelDynamicFile::newInstance(CiftiMappableDataFile* ciftiMapFile,
                                                AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    CiftiBrainordinateLabelDynamicFile* labelFile = NULL;
    
    const bool supportedFlag = ciftiMapFile->isLabelDynamicThresholdFileSupported();
    
    if (supportedFlag) {
        CaretAssert(ciftiMapFile);
        
        CiftiBrainModelsMap brainModelsMap;
        int32_t numberOfMaps = -1;
        const bool validFlag = getBrainModelsAndMapsFromFile(ciftiMapFile,
                                                             brainModelsMap,
                                                             numberOfMaps,
                                                             errorMessageOut);
        if (validFlag) {
            CiftiFile* ciftiFile = NULL;
            
            try {
                /*
                 * Create the XML.
                 */
                CiftiXML myXML;
                myXML.setNumberOfDimensions(2);
                
                /*
                 * Add labels or scalars to XML.
                 */
                CiftiLabelsMap labelsMap;
                labelsMap.setLength(numberOfMaps);
                myXML.setMap(CiftiXML::ALONG_ROW,
                             labelsMap);
                
                /*
                 * Add brainordinates to the XML.
                 */
                myXML.setMap(CiftiXML::ALONG_COLUMN,
                             brainModelsMap);
                
                /*
                 * Add XML to the CIFTI file.
                 */
                ciftiFile = new CiftiFile();
                ciftiFile->setCiftiXML(myXML);
                
                /*
                 * Create the filename with a label extension
                 */
                FileInformation fileInfo(ciftiMapFile->getFileName());
                AString filePath, fileNameNoExt, fileExt;
                fileInfo.getFileComponents(filePath, fileNameNoExt, fileExt);
                const AString newFileName = FileInformation::assembleFileComponents(filePath,
                                                                                    fileNameNoExt,
                                                                                    DataFileTypeEnum::toFileExtension(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC));
                
                /*
                 * Add the CiftiFile to the Cifti Mappable File
                 */
                labelFile = new CiftiBrainordinateLabelDynamicFile(ciftiMapFile);
                labelFile->m_ciftiFile.grabNew(ciftiFile);
                labelFile->setFileName(newFileName);
                labelFile->initializeAfterReading(newFileName);
                {
                    const int32_t numMaps = labelFile->getNumberOfMaps();
                    for (int32_t i = 0; i < numMaps; i++) {
                        labelFile->getMapLabelTable(i)->setLabel(LABEL_KEY_OUTLINE, "Outline", 1.0, 1.0, 1.0, 1.0);
                        labelFile->setMapName(i,
                                              ("Outline: " + ciftiMapFile->getMapName(i)));
                    }
                }
                
                LabelDrawingProperties* labelProps = labelFile->getLabelDrawingProperties();
                labelProps->setDrawingType(LabelDrawingTypeEnum::DRAW_OUTLINE_COLOR);
                labelProps->setOutlineColor(CaretColorEnum::WHITE);
                labelProps->setDrawMedialWallFilled(false);
                
                labelFile->m_classNameHierarchy->update(labelFile,
                                                        true);
                labelFile->m_forceUpdateOfGroupAndNameHierarchy = false;
                labelFile->m_classNameHierarchy->setAllSelected(true);
                
                CaretLogFiner("CLASS/NAME Table for : "
                              + labelFile->getFileNameNoPath()
                              + "\n"
                              + labelFile->m_classNameHierarchy->toString());
                
                labelFile->validateKeysAndLabels();
                
                labelFile->setModified();

                CaretLogFine("Created CIFTI label dynamic file for "
                             + ciftiMapFile->getFileName());
            }
            catch (const DataFileException& de) {
                if (ciftiFile != NULL) {
                    delete ciftiFile;
                }
                if (labelFile != NULL) {
                    delete labelFile;
                }
                errorMessageOut = ("CIFTI label dynamic creation failed for "
                                   + ciftiMapFile->getFileName()
                                   + ": "
                                   + de.whatString());
            }
        }
    }
    else {
        errorMessageOut = ("Label dynamic threshold file not supported for "
                           + ciftiMapFile->getFileNameNoPath());
        
    }
    return labelFile;
}

/**
 * Destructor.
 */
CiftiBrainordinateLabelDynamicFile::~CiftiBrainordinateLabelDynamicFile()
{
}

/**
 * @return True if the given file type is supported for dynamic labeling.
 *
 * @param dataFileType
 *     Type of data file.
 */
bool
CiftiBrainordinateLabelDynamicFile::isFileTypeSupported(const DataFileTypeEnum::Enum dataFileType)
{
    bool supportedFlag = false;

    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            supportedFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            supportedFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            supportedFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            supportedFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            supportedFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
    }
    
    return supportedFlag;
}

/**
 * Find the brain models mapping and the number of maps for the given data file.
 *
 * @param mapFile
 *     The map file.
 * @param brainModelsMapOut
 *     Output with brain models map.
 * @param numberOfMapsOut
 *     Number of maps.
 * @param errorMessageOut
 *     Error message.
 * @return True if brain models map and ar valid.
 */
bool
CiftiBrainordinateLabelDynamicFile::getBrainModelsAndMapsFromFile(CaretMappableDataFile* mapFile,
                                                                  CiftiBrainModelsMap& brainModelsMapOut,
                                                                  int32_t& numberOfMapsOut,
                                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    brainModelsMapOut.clear();
    numberOfMapsOut = -1;
    
    CaretAssert(mapFile);
    if ( ! isFileTypeSupported(mapFile->getDataFileType())) {
        errorMessageOut = ("Label dynamic threshold file not supported for "
                           + mapFile->getFileNameNoPath());
        return false;
    }
    
    bool validFlag = false;
    CiftiMappableDataFile* ciftiFile = dynamic_cast<CiftiMappableDataFile*>(mapFile);
    
    const DataFileTypeEnum::Enum dataFileType = mapFile->getDataFileType();
    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
        {
            CaretAssert(ciftiFile);
            const CiftiXML& ciftiXML = ciftiFile->getCiftiXML();
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::BRAIN_MODELS);
            brainModelsMapOut = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
            numberOfMapsOut = 1;
            validFlag = true;
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
        {
            CaretAssert(ciftiFile);
            const CiftiXML& ciftiXML = ciftiFile->getCiftiXML();
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);
            brainModelsMapOut = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
            
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES);
            numberOfMapsOut = 1; /* always one map for dense dynamic */
            validFlag = true;
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
        {
            CaretAssert(ciftiFile);
            const CiftiXML& ciftiXML = ciftiFile->getCiftiXML();
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);
            brainModelsMapOut = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
            
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS);
            numberOfMapsOut = ciftiXML.getScalarsMap(CiftiXML::ALONG_ROW).getLength();
            validFlag = true;
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        {
            CaretAssert(ciftiFile);
            const CiftiXML& ciftiXML = ciftiFile->getCiftiXML();
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);
            brainModelsMapOut = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
            
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES);
            numberOfMapsOut = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW).getLength();
            validFlag = true;
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
        {
            CaretAssert(ciftiFile);
            const CiftiXML& ciftiXML = ciftiFile->getCiftiXML();
            CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::BRAIN_MODELS);
            brainModelsMapOut = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
            numberOfMapsOut = 1;
            validFlag = true;
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
    }
    
    if (validFlag) {
        if (brainModelsMapOut.getLength() <= 0) {
            errorMessageOut.append("Failed to find brain models (length==0).  ");
        }
        if (numberOfMapsOut == 0) {
            errorMessageOut.append("File contains zero maps.  ");
        }
        else if (numberOfMapsOut < 0) {
            errorMessageOut.append("Failed to find number of maps.  ");
        }
        
        if ( ! errorMessageOut.isEmpty()) {
            errorMessageOut.insert(0, (mapFile->getFileNameNoPath() + "  "));
            validFlag = false;
        }
    }
    else {
        if (errorMessageOut.isEmpty()) {
            errorMessageOut = ("PROGRAM ERROR: Failed to search for brain models and number of maps for "
                               + mapFile->getFileNameNoPath());
            CaretAssertMessage(0, errorMessageOut);
        }
    }
    
    return validFlag;
}

/**
 * @return The parent mappable data file (const method)
 */
const CaretMappableDataFile*
CiftiBrainordinateLabelDynamicFile::getParentMappableDataFile() const
{
    return m_parentMappableDataFile;
}

/**
 * @return The parent mappable data file.
 */
CaretMappableDataFile*
CiftiBrainordinateLabelDynamicFile::getParentMappableDataFile()
{
    return m_parentMappableDataFile;
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Dense files do NOT support writing.
 */
bool
CiftiBrainordinateLabelDynamicFile::supportsWriting() const
{
    return false;
}


