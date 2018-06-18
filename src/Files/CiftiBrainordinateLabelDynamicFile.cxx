
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
        
        int32_t brainModelsDirection = -1;
        int32_t mapsDirection        = -1;
        switch (ciftiMapFile->m_dataMappingDirectionForCiftiXML) {
            case CiftiXML::ALONG_COLUMN:
                brainModelsDirection = CiftiXML::ALONG_COLUMN;
                mapsDirection        = CiftiXML::ALONG_ROW;
                break;
            case CiftiXML::ALONG_ROW:
                brainModelsDirection = CiftiXML::ALONG_ROW;
                mapsDirection        = CiftiXML::ALONG_COLUMN;
                break;
        }
        
        const CiftiXML& parentXML = ciftiMapFile->getCiftiXML();
        CiftiBrainModelsMap brainModelsMap;
        if (CiftiMappingType::MappingType::BRAIN_MODELS)
        switch (brainModelsDirection) {
            case CiftiXML::ALONG_ROW:
            case CiftiXML::ALONG_COLUMN:
                if (parentXML.getMappingType(brainModelsDirection) == CiftiMappingType::BRAIN_MODELS) {
                    brainModelsMap = parentXML.getBrainModelsMap(brainModelsDirection);
                }
                break;
            default:
                break;
        }
        
        int32_t numberOfMaps = -1;
        switch (mapsDirection) {
            case CiftiXML::ALONG_COLUMN:
            case CiftiXML::ALONG_ROW:
                switch (parentXML.getMappingType(mapsDirection)) {
                    case CiftiMappingType::BRAIN_MODELS:
                        if (ciftiMapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE) {
                            numberOfMaps = 1;
                        }
                        break;
                    case CiftiMappingType::LABELS:
                        break;
                    case CiftiMappingType::PARCELS:
                        if (ciftiMapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE) {
                            numberOfMaps = 1;
                        }
                        break;
                    case CiftiMappingType::SCALARS:
                    {
                        CiftiScalarsMap scalarsMap = parentXML.getScalarsMap(mapsDirection);
                        numberOfMaps = scalarsMap.getLength();
                    }
                        break;
                    case CiftiMappingType::SERIES:
                    {
                        CiftiSeriesMap seriesMap = parentXML.getSeriesMap(mapsDirection);
                        numberOfMaps = seriesMap.getLength();
                    }
                        break;
                }
                break;
        }
        
        if ((brainModelsMap.getLength() > 0)
            && (numberOfMaps > 0)) {
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
                labelFile->setModified();
                
                std::cout << "Created CIFTI label dynamic file for " << ciftiMapFile->getFileName() << std::endl;
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
        else {
            errorMessageOut = ("CIFTI file is NOT label dynamic compatible: "
                               + ciftiMapFile->getFileName()
                               + "  ");
            if (brainModelsMap.getLength() <= 0) {
                errorMessageOut.append("Unable to find Brain Models.  ");
            }
            if (numberOfMaps <= 0) {
                errorMessageOut.append("Unable to find number of maps or file is empty.  ");
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

