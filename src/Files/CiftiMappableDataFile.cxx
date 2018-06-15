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

#include <set>

#define __CIFTI_MAPPABLE_DATA_FILE_DECLARE__
#include "CiftiMappableDataFile.h"
#undef __CIFTI_MAPPABLE_DATA_FILE_DECLARE__

#include "BackgroundAndForegroundColors.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartDataCartesian.h"
#include "CiftiBrainordinateLabelDynamicFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "CaretTemporaryFile.h"
#include "CiftiXML.h"
#include "DataFileContentInformation.h"
#include "EventManager.h"
#include "EventCaretPreferencesGet.h"
#include "EventSurfaceColoringInvalidate.h"
#include "FastStatistics.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GroupAndNameHierarchyModel.h"
#include "Histogram.h"
#include "MapFileDataSelector.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "SparseVolumeIndexer.h"

using namespace caret;


    
/**
 * \class caret::CiftiMappableDataFile 
 * \brief Abstract class for CIFTI files that are mapped to surfaces and volumes
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param dataFileType
 *    Type of data file.
 */
CiftiMappableDataFile::CiftiMappableDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretMappableDataFile(dataFileType)
{
    m_ciftiFile.grabNew(NULL);
    m_labelDynamicThresholdFile.reset();
    m_labelDynamicThresholdFileCreationFailedFlag = false;
    m_voxelIndicesToOffset.grabNew(NULL);
    m_classNameHierarchy.grabNew(NULL);
    m_fileDataReadingType = FILE_READ_DATA_ALL;
    
    switch (CIFTI_FILE_ROW_COLUMN_INDEX_BASE_FOR_GUI) {
        case 0:
        case 1:
            break;
        default:
            CaretAssertMessage(0, "CIFTI_FILE_ROW_COLUMN_INDEX_BASE_FOR_GUI must be zero or one!");
            break;
    }
    
    m_containsSurfaceData = false;
    m_containsVolumeData = false;
    m_mappingTimeStart = 0.0f;
    m_mappingTimeStep  = 1.0f;
    m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
    
    m_dataReadingAccessMethod      = DATA_ACCESS_METHOD_INVALID;
    m_dataMappingAccessMethod      = DATA_ACCESS_METHOD_INVALID;
    m_colorMappingMethod           = COLOR_MAPPING_METHOD_INVALID;
    m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_INVALID;
    m_fileMapDataType              = FILE_MAP_DATA_TYPE_INVALID;
    
    m_dataMappingDirectionForCiftiXML = S_CIFTI_XML_ALONG_INVALID;
    m_dataReadingDirectionForCiftiXML = S_CIFTI_XML_ALONG_INVALID;
    
    m_fileFastStatistics.grabNew(NULL);
    m_fileHistogram.grabNew(NULL);
    m_fileHistorgramLimitedValues.grabNew(NULL);
    
    /*
     * Note: The first palette normalization mode is assumed to
     * be the default mode.
     */
    m_paletteNormalizationModesSupported.clear();
    
    /* 
     * Note: Force an update of the class and name hierarchy 
     */
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    switch (dataFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MATRIX;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MATRIX;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            m_dataReadingAccessMethod = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod      = COLOR_MAPPING_METHOD_LABEL_TABLE;
            m_fileMapDataType         = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
            m_dataReadingAccessMethod = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod      = COLOR_MAPPING_METHOD_LABEL_TABLE;
            m_fileMapDataType         = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            m_dataReadingAccessMethod       = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_dataMappingAccessMethod       = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_colorMappingMethod            = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource     = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_fileMapDataType               = FILE_MAP_DATA_TYPE_MATRIX;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MATRIX;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MATRIX;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_LABEL_TABLE;
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW;
            m_dataMappingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            m_dataReadingAccessMethod      = DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN;
            m_dataMappingAccessMethod      = DATA_ACCESS_NONE;
            m_colorMappingMethod           = COLOR_MAPPING_METHOD_PALETTE;
            m_paletteColorMappingSource    = PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE;
            m_paletteNormalizationModesSupported.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            m_fileMapDataType              = FILE_MAP_DATA_TYPE_MULTI_MAP;
            break;
        case DataFileTypeEnum::ANNOTATION:
        case DataFileTypeEnum::BORDER:
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
        case DataFileTypeEnum::FOCI:
        case DataFileTypeEnum::IMAGE:
        case DataFileTypeEnum::LABEL:
        case DataFileTypeEnum::METRIC:
        case DataFileTypeEnum::PALETTE:
        case DataFileTypeEnum::RGBA:
        case DataFileTypeEnum::SCENE:
        case DataFileTypeEnum::SPECIFICATION:
        case DataFileTypeEnum::SURFACE:
        case DataFileTypeEnum::UNKNOWN:
        case DataFileTypeEnum::VOLUME:
            CaretAssertMessage(0, (DataFileTypeEnum::toGuiName(dataFileType)
                                   + " is not a CIFTI Mappable Data File."));
            break;
    }

    /*
     * Data from matrix files is read as needed since
     * Dense can be very large
     */
    switch (m_fileMapDataType) {
        case FILE_MAP_DATA_TYPE_INVALID:
            break;
        case FILE_MAP_DATA_TYPE_MATRIX:
            m_fileDataReadingType = FILE_READ_DATA_AS_NEEDED;
            break;
        case FILE_MAP_DATA_TYPE_MULTI_MAP:
            m_fileDataReadingType = FILE_READ_DATA_ALL;
            break;
    }
    
    CaretAssert(m_dataReadingAccessMethod != DATA_ACCESS_METHOD_INVALID);
    CaretAssert(m_dataMappingAccessMethod != DATA_ACCESS_METHOD_INVALID);
    CaretAssert(m_colorMappingMethod      != COLOR_MAPPING_METHOD_INVALID);
    CaretAssert(m_fileMapDataType         != FILE_MAP_DATA_TYPE_INVALID);
    if (m_colorMappingMethod != COLOR_MAPPING_METHOD_LABEL_TABLE) {
        CaretAssert(m_paletteColorMappingSource != PALETTE_COLOR_MAPPING_SOURCE_INVALID);
    }

    setupCiftiReadingMappingDirection();
    
    m_classNameHierarchy.grabNew(new GroupAndNameHierarchyModel());
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}

/**
 * Destructor.
 */
CiftiMappableDataFile::~CiftiMappableDataFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    clearPrivate();
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
CiftiMappableDataFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* colorInvalidateEvent
            = dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(colorInvalidateEvent);
        colorInvalidateEvent->setEventProcessed();
        
        invalidateColoringInAllMaps();
    }
}

/**
 * Create a new instance of a CIFTI file from the given CIFTI data file type
 * for the given surface structure and number of nodes.  NOT all CIFTI file 
 * types are supported.  
 *
 * @param ciftiFileType
 *    Data file type for the returned CIFTI file.
 * @param structure
 *    The surface structure.
 * @param numberOfNodes
 *    Number of nodes in the surface.
 * @param errorMessageOut
 *    Will describe problem if there was an error such as an unsupported
 *    CIFTI file type.
 * @param
 *    Pointer to the newly created CIFTI file.  If there is an
 *    error, NULL will be returned and errorMessageOut will describe the
 *    problem.  User will need to 'dynamic_cast' the returned pointer to
 *    the class corresponding to the CIFTI file type.
 */
CiftiMappableDataFile*
CiftiMappableDataFile::newInstanceForCiftiFileTypeAndSurface(const DataFileTypeEnum::Enum ciftiFileType,
                                                             const StructureEnum::Enum structure,
                                                             const int32_t numberOfNodes,
                                                             AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    CiftiFile* ciftiFile = NULL;
    CiftiMappableDataFile* ciftiMappableFile = NULL;
    
    try {
        bool hasLabelsFlag  = false;
        bool hasScalarsFlag = false;
        
        /*
         * Create the appropriate file type.
         */
        switch (ciftiFileType) {
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                ciftiMappableFile = new CiftiBrainordinateLabelFile();
                hasLabelsFlag = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                ciftiMappableFile = new CiftiBrainordinateScalarFile();
                hasScalarsFlag = true;
                break;
            default:
                errorMessageOut = ("Creation of "
                                   + DataFileTypeEnum::toGuiName(ciftiFileType)
                                   + " is not supported.");
                CaretAssertMessage(0, errorMessageOut);
                return NULL;
                break;
        }
        
        /*
         * Create the XML.
         */
        CiftiXML myXML;
        myXML.setNumberOfDimensions(2);
        
        /*
         * Add labels or scalars to XML.
         */
        if (hasLabelsFlag) {
            CiftiLabelsMap labelsMap;
            labelsMap.setLength(1);
            myXML.setMap(CiftiXML::ALONG_ROW,
                         labelsMap);
        }
        else if (hasScalarsFlag) {
            CiftiScalarsMap scalarsMap;
            scalarsMap.setLength(1);
            myXML.setMap(CiftiXML::ALONG_ROW,
                         scalarsMap);            
        }
        else {
            CaretAssert(0);
        }

        /*
         * Add brainordinates to the XML.
         */
        CiftiBrainModelsMap brainModelsMap;
        brainModelsMap.addSurfaceModel(numberOfNodes,
                                       structure);
        myXML.setMap(CiftiXML::ALONG_COLUMN,
                     brainModelsMap);

        /*
         * Add XML to the CIFTI file.
         */
        ciftiFile = new CiftiFile();
        ciftiFile->setCiftiXML(myXML);
        
        /*
         * Fill with zeros
         */
        std::vector<float> zeroData(numberOfNodes,
                                    0.0);
        ciftiFile->setColumn(&zeroData[0],
                             0);
        
        /*
         * Add the CiftiFile to the Cifti Mappable File
         */
        const AString defaultFileName = ciftiMappableFile->getFileName();
        ciftiMappableFile->m_ciftiFile.grabNew(ciftiFile);
        ciftiMappableFile->setFileName(defaultFileName);
        ciftiMappableFile->initializeAfterReading(defaultFileName);
        ciftiMappableFile->setModified();
        
        return ciftiMappableFile;
    }
    catch (const DataFileException& de) {
        errorMessageOut = de.whatString();
    }
    
    if (ciftiMappableFile != NULL) {
        delete ciftiMappableFile;
        ciftiMappableFile = NULL;
    }
    if (ciftiFile != NULL) {
        delete ciftiFile;
        ciftiFile = NULL;
    }
    
    return NULL;
}

/**
 * @return This file's label dynamic threshold file (may be NULL is not supported by this file type).
 */
CaretMappableDataFile*
CiftiMappableDataFile::getLabelDynamicThresholdFile()
{
    return m_labelDynamicThresholdFile.get();
}

/**
 * @return This file's label dynamic threshold file (may be NULL is not supported by this file type) const method.
 */
const CaretMappableDataFile*
CiftiMappableDataFile::getLabelDynamicThresholdFile() const
{
    return m_labelDynamicThresholdFile.get();
}

/**
 * @return True if this file type supports the label dynamic threshold file.
 */
bool
CiftiMappableDataFile::isLabelDynamicThresholdFileSupported() const
{
    if (m_labelDynamicThresholdFileCreationFailedFlag) {
        /*
         * Creation failed, so don't allow it
         */
        return false;
    }
    
    const bool supportedFlag = CiftiBrainordinateLabelDynamicFile::isFileTypeSupported(getDataFileType());
    
    return supportedFlag;
}

/**
 * Set the enabled status of label dynamic threshold file for the given map index.
 *
 * @param index
 *     Index of the map.
 */
void
CiftiMappableDataFile::setMapLabelDynamicThresholdFileEnabled(const int32_t mapIndex,
                                                              const bool enabled)
{
    if (m_labelDynamicThresholdFileCreationFailedFlag) {
        CaretMappableDataFile::setMapLabelDynamicThresholdFileEnabled(mapIndex,
                                                                      false);
        return;
    }
    
    CaretMappableDataFile::setMapLabelDynamicThresholdFileEnabled(mapIndex,
                                                                  enabled);
    
    
    if (isMapLabelDynamicThresholdFileEnabled(mapIndex)) {
        /*
         * Lazily initialize the file
         */
        if ( ! m_labelDynamicThresholdFile) {
            createLabelDynamicThresholdFile();
            if (m_labelDynamicThresholdFileCreationFailedFlag) {
                CaretMappableDataFile::setMapLabelDynamicThresholdFileEnabled(mapIndex,
                                                                              false);
            }
        }
    }
}

/**
 * Create the Label Dynamic Threshold File
 */
void
CiftiMappableDataFile::createLabelDynamicThresholdFile()
{
    AString errorMessage;
    CiftiBrainordinateLabelDynamicFile* labelFile = CiftiBrainordinateLabelDynamicFile::newInstance(this,
                                                                                                    errorMessage);
    if (labelFile != NULL) {
        m_labelDynamicThresholdFile.reset(labelFile);
    }
    else {
        m_labelDynamicThresholdFileCreationFailedFlag = true;
        CaretLogWarning(errorMessage);
    }
}

/**
 * Clear the contents of the file.
 */
void
CiftiMappableDataFile::clear()
{
    CaretMappableDataFile::clear();
    clearPrivate();
}

/**
 * Clear the contents of the file.
 * Note that "clear()" is virtual and cannot be called from destructor.
 */
void
CiftiMappableDataFile::clearPrivate()
{
    /*
     * Do not clear this items as they are setup in the constructor.
     * m_dataReadingAccessMethod
     * m_dataMappingAccessMethod
     * m_colorMappingMethod
     * m_fileMapDataType
     */
    
    m_ciftiFile.grabNew(NULL);
    m_labelDynamicThresholdFile.reset();
    m_labelDynamicThresholdFileCreationFailedFlag = false;
    
    resetDataLoadingMembers();
    
    m_containsSurfaceData = false;
    m_containsVolumeData = false;

    m_mappingTimeStart = 0.0f;
    m_mappingTimeStep  = 1.0f;
    m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
    
    m_brainordinateMapping.reset();
    m_brainordinateMappingCachedFlag = false;
}

/**
 * Reset data loading members.
 * Also used when parcel row/column loading is changed.
 */
void
CiftiMappableDataFile::resetDataLoadingMembers()
{
    const int64_t num = static_cast<int64_t>(m_mapContent.size());
    for (int64_t i = 0; i < num; i++) {
        delete m_mapContent[i];
    }
    m_mapContent.clear();
    m_classNameHierarchy->clear();
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * @return Is this file empty?
 */
bool
CiftiMappableDataFile::isEmpty() const
{
    if (getNumberOfMaps() > 0) {
        return false;
    }
    return true;
}

/**
 * Set preference for reading.  Reading all data from a "matrix" type file
 * is not supported and if requested, it will be ignored.
 *
 * @param prefer
 *    When true, only header is read and no data is read.
 *    When false, both header and all data is read.
 */
void
CiftiMappableDataFile::setPreferOnDiskReading(const bool& prefer)
{
    if (prefer) {
        m_fileDataReadingType = FILE_READ_DATA_AS_NEEDED;
    }
    else {
        switch (m_fileMapDataType) {
            case FILE_MAP_DATA_TYPE_INVALID:
                break;
            case FILE_MAP_DATA_TYPE_MATRIX:
                CaretLogSevere("CIFTI Matrix files do not support reading of all data.");
                break;
            case FILE_MAP_DATA_TYPE_MULTI_MAP:
                m_fileDataReadingType = FILE_READ_DATA_ALL;
                break;
        }
    }
}

/**
 * @return structure file maps to.
 */
StructureEnum::Enum
CiftiMappableDataFile::getStructure() const
{
    /*
     * CIFTI files apply to all structures.
     */
    return StructureEnum::ALL;
}

/**
 * Set the structure to which file maps.
 * @param structure
 *    New structure to which file maps.
 */
void
CiftiMappableDataFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* CIFTI files may apply to all structures */
}

/**
 * Is this file able to map to the given structure?  Some data files, such
 * as CIFTI files, are able to map to multiple surface structure.  The default
 * implementation of this method simply compares the given structure to
 * getStructure() and returns true if they are the same value, else false.
 *
 * @param structure
 *   Structure for testing mappability status.
 * @return True if this file is able to map to the given structure, else false.
 */
bool
CiftiMappableDataFile::isMappableToSurfaceStructure(const StructureEnum::Enum structure) const
{
    /*
     * Validate number of nodes are correct
     */
    int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
    if (numCiftiNodes > 0) {
        return true;
    }
    
    return false;
}

/**
 * @return Metadata for the file.
 */
GiftiMetaData*
CiftiMappableDataFile::getFileMetaData()
{
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    return ciftiXML.getFileMetaData();
}

/**
 * @return Metadata for the file.
 */
const GiftiMetaData*
CiftiMappableDataFile:: getFileMetaData() const
{
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    return ciftiXML.getFileMetaData();
}

/**
 * Read the file.
 *
 * @param ciftiMapFileName
 *    Name of the file to read.
 * @throw
 *    DataFileException if there is an error reading the file.
 */
void
CiftiMappableDataFile::readFile(const AString& ciftiMapFileName)
{
    clear();

    try {
        /*
         * Is the file on the network (name begins with http, ftp, etc.).
         */
        if (DataFile::isFileOnNetwork(ciftiMapFileName)) {
            /*
             * Data in Xnat does not end with a valid file extension
             * but ends with HTTP search parameters.  Thus, if the
             * filename does not have a valid extension, assume that
             * the data is in Xnat.
             */
            bool isValidFileExtension = false;
            DataFileTypeEnum::fromFileExtension(ciftiMapFileName,
                                                &isValidFileExtension);
            
            if (isValidFileExtension) {
                switch (m_fileMapDataType) {
                    case FILE_MAP_DATA_TYPE_INVALID:
                        break;
                    case FILE_MAP_DATA_TYPE_MATRIX:
                        throw DataFileException(ciftiMapFileName
                                                + " of type "
                                                + DataFileTypeEnum::toGuiName(getDataFileType())
                                                + " cannot be read over the network.  The file must be"
                                                " accessed by reading individual rows and/or columns"
                                                " and this cannot be performed over a network.");
                        break;
                    case FILE_MAP_DATA_TYPE_MULTI_MAP:
                        break;
                }
                
                CaretTemporaryFile tempFile;
                tempFile.readFile(ciftiMapFileName);
                m_ciftiFile.grabNew(new CiftiFile());
                m_ciftiFile->openFile(tempFile.getFileName());
                m_ciftiFile->convertToInMemory();
            }
            else {
                m_ciftiFile.grabNew(new CiftiFile());
                AString username = "";
                AString password = "";
                AString filenameToOpen = "";
                
                /*
                 * Username and password may be embedded in URL, so extract them.
                 */
                FileInformation fileInfo(ciftiMapFileName);
                fileInfo.getRemoteUrlUsernameAndPassword(filenameToOpen,
                                                         username,
                                                         password);
                
                /*
                 * Always override with a password entered by the user.
                 */
                if (CaretDataFile::getFileReadingUsername().isEmpty() == false) {
                    username = CaretDataFile::getFileReadingUsername();
                    password = CaretDataFile::getFileReadingPassword();
                }
                
                m_ciftiFile->openURL(filenameToOpen,
                                     username,
                                     password);
            }
        }
        else {
            m_ciftiFile.grabNew(new CiftiFile());
            switch (m_fileMapDataType) {
                case FILE_MAP_DATA_TYPE_INVALID:
                    break;
                case FILE_MAP_DATA_TYPE_MATRIX:
                    m_ciftiFile->openFile(ciftiMapFileName);
                    break;
                case FILE_MAP_DATA_TYPE_MULTI_MAP:
                    m_ciftiFile->openFile(ciftiMapFileName);
                    
                    switch (m_fileDataReadingType) {
                        case FILE_READ_DATA_ALL:
                            m_ciftiFile->convertToInMemory();
                            break;
                        case FILE_READ_DATA_AS_NEEDED:
                            break;
                    }
                    break;
            }
        }
        
        if (m_ciftiFile != NULL) {
            setFileName(ciftiMapFileName); // need by charting delegate
            initializeAfterReading(ciftiMapFileName);
        }
    }
    catch (DataFileException& e) {
        clear();
        throw e;
    }
    catch (CaretException& e) {
        clear();
        throw DataFileException(ciftiMapFileName,
                                e.whatString());
    }
    
    updateAfterFileDataChanges();
    
    setFileName(ciftiMapFileName);
    clearModified();
}

/**
 * Validate the mapping types for each dimension.
 *
 * @param filename
 *     Name of file.
 */
void
CiftiMappableDataFile::validateMappingTypes(const AString& filename)
{
    CaretAssert(m_ciftiFile);
    
    CiftiMappingType::MappingType expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
    CiftiMappingType::MappingType expectedAlongRowMapType    = CiftiMappingType::BRAIN_MODELS;
    
    const DataFileTypeEnum::Enum dataFileType = getDataFileType();
    switch (dataFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::BRAIN_MODELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::SERIES;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::LABELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::LABELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::PARCELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::SCALARS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            expectedAlongColumnMapType = CiftiMappingType::BRAIN_MODELS;
            expectedAlongRowMapType = CiftiMappingType::SERIES;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            expectedAlongColumnMapType = CiftiMappingType::PARCELS;
            expectedAlongRowMapType = CiftiMappingType::PARCELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            expectedAlongColumnMapType = CiftiMappingType::PARCELS;
            expectedAlongRowMapType = CiftiMappingType::BRAIN_MODELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            expectedAlongColumnMapType = CiftiMappingType::PARCELS;
            expectedAlongRowMapType = CiftiMappingType::LABELS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            expectedAlongColumnMapType = CiftiMappingType::PARCELS;
            expectedAlongRowMapType = CiftiMappingType::SCALARS;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            expectedAlongColumnMapType = CiftiMappingType::PARCELS;
            expectedAlongRowMapType = CiftiMappingType::SERIES;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            expectedAlongColumnMapType = CiftiMappingType::SCALARS;
            expectedAlongRowMapType = CiftiMappingType::SERIES;
            break;
        case DataFileTypeEnum::ANNOTATION:
        case DataFileTypeEnum::BORDER:
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
        case DataFileTypeEnum::FOCI:
        case DataFileTypeEnum::IMAGE:
        case DataFileTypeEnum::LABEL:
        case DataFileTypeEnum::METRIC:
        case DataFileTypeEnum::PALETTE:
        case DataFileTypeEnum::RGBA:
        case DataFileTypeEnum::SCENE:
        case DataFileTypeEnum::SPECIFICATION:
        case DataFileTypeEnum::SURFACE:
        case DataFileTypeEnum::UNKNOWN:
        case DataFileTypeEnum::VOLUME:
            throw DataFileException(filename,
                                    DataFileTypeEnum::toGuiName(dataFileType)
                                   + " is not a CIFTI Mappable Data File.");
            break;
    }
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    const CiftiMappingType::MappingType alongColumnMapType = ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN);
    const CiftiMappingType::MappingType alongRowMapType    = ciftiXML.getMappingType(CiftiXML::ALONG_ROW);
    
    AString errorMessage;
    if (alongColumnMapType != expectedAlongColumnMapType) {
        errorMessage.appendWithNewLine("Along column mapping type is "
                                       + CiftiMappableDataFile::mappingTypeToName(alongColumnMapType)
                                       + " but should be "
                                       + CiftiMappableDataFile::mappingTypeToName(expectedAlongColumnMapType)
                                       + " for file type \""
                                       + DataFileTypeEnum::toGuiName(dataFileType)
                                       + "\"");
    }
    if (alongRowMapType != expectedAlongRowMapType) {
        errorMessage.appendWithNewLine("Along row mapping type is "
                                       + CiftiMappableDataFile::mappingTypeToName(alongRowMapType)
                                       + " but should be "
                                       + CiftiMappableDataFile::mappingTypeToName(expectedAlongRowMapType)
                                       + " for file type \""
                                       + DataFileTypeEnum::toGuiName(dataFileType)
                                       + "\"");
    }
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(filename,
                                errorMessage);
    }
}

/**
 * Setup the CIFTI mapping and reading directions.
 */
void
CiftiMappableDataFile::setupCiftiReadingMappingDirection()
{
    switch (m_dataMappingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            m_dataMappingDirectionForCiftiXML = CiftiXML::ALONG_COLUMN;
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            m_dataMappingDirectionForCiftiXML = CiftiXML::ALONG_ROW;
            break;
    }
    
    switch (m_dataReadingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            m_dataReadingDirectionForCiftiXML = CiftiXML::ALONG_COLUMN;
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            m_dataReadingDirectionForCiftiXML = CiftiXML::ALONG_ROW;
            break;
    }
}


/**
 * Initialize the CIFTI file.
 *
 * @param filename
 *     Name of file.
 */
void
CiftiMappableDataFile::initializeAfterReading(const AString& filename)
{
    CaretAssert(m_ciftiFile);
    
    setupCiftiReadingMappingDirection();
    
    validateMappingTypes(filename);
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    if (m_dataMappingDirectionForCiftiXML != S_CIFTI_XML_ALONG_INVALID) {
        switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
            case CiftiMappingType::BRAIN_MODELS:
            {
                const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataMappingDirectionForCiftiXML);
                if (! map.getSurfaceStructureList().empty()) {
                    m_containsSurfaceData = true;
                }
                if (map.hasVolumeData()) {
                    m_containsVolumeData = true;
                }
            }
                break;
            case CiftiMappingType::LABELS:
                CaretAssertMessage(0, "Mapping type should never be LABELS");
                throw DataFileException(filename,
                                        "Mapping type should never be LABELS");
                break;
            case CiftiMappingType::PARCELS:
            {
                const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
                if (! map.getParcelSurfaceStructures().empty()) {
                    m_containsSurfaceData = true;
                }
                if (map.hasVolumeData()) {
                    m_containsVolumeData = true;
                }
            }
                break;
            case CiftiMappingType::SCALARS:
                CaretAssertMessage(0, "Mapping type should never be SCALARS");
                throw DataFileException(filename,
                                        "Mapping type should never be SCALARS");
                break;
            case CiftiMappingType::SERIES:
                CaretAssertMessage(0, "Mapping type should never be SERIES");
                throw DataFileException(filename,
                                        "Mapping type should never be SERIES");
                break;
        }
    }

    switch (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML)) {
        case CiftiMappingType::BRAIN_MODELS:
            break;
        case CiftiMappingType::LABELS:
            break;
        case CiftiMappingType::PARCELS:
            break;
        case CiftiMappingType::SCALARS:
            break;
        case CiftiMappingType::SERIES:
        {
            const CiftiSeriesMap& map = ciftiXML.getSeriesMap(m_dataReadingDirectionForCiftiXML);
            CiftiSeriesMap::Unit units = map.getUnit();
            switch (units) {
                case CiftiSeriesMap::HERTZ:
                    m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_HZ;
                    break;
                case CiftiSeriesMap::METER:
                    CaretLogWarning("CIFTI Units METER not implemented");
                    break;
                case CiftiSeriesMap::RADIAN:
                    CaretLogWarning("CIFTI Units RADIAN not implemented");
                    break;
                case CiftiSeriesMap::SECOND:
                    m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_SEC;
                    break;
            }
            m_mappingTimeStart = map.getStart();
            m_mappingTimeStep  = map.getStep();
        }
            break;
    }
    
    switch (m_dataMappingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            if (ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS) {
                m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer(ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN)));
            }
            else if (ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::PARCELS) {
                m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer(ciftiXML.getParcelsMap(CiftiXML::ALONG_COLUMN)));
            }
            else {
                CaretAssertMessage(0, "Invalid mapping type for mapping data to brainordinates");
                throw DataFileException(filename,
                                        "Invalid mapping type for mapping data to brainordinates");
            }
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            if (ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::BRAIN_MODELS) {
                m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer(ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW)));
            }
            else if (ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::PARCELS) {
                m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer(ciftiXML.getParcelsMap(CiftiXML::ALONG_ROW)));
            }
            else {
                CaretAssertMessage(0, "Invalid mapping type for mapping data to brainordinates");
                throw DataFileException(filename,
                                        "Invalid mapping type for mapping data to brainordinates");
            }
            break;
    }
    
    /*
     * Special case for scalar data series that does NOT map to brainordinates
     */
    if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
        const CiftiSeriesMap& map = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW);
        CiftiSeriesMap::Unit units = map.getUnit();
        switch (units) {
            case CiftiSeriesMap::HERTZ:
                m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_HZ;
                break;
            case CiftiSeriesMap::METER:
                CaretLogWarning("CIFTI Units METER not implemented");
                break;
            case CiftiSeriesMap::RADIAN:
                CaretLogWarning("CIFTI Units RADIAN not implemented");
                break;
            case CiftiSeriesMap::SECOND:
                m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_SEC;
                break;
        }
        m_mappingTimeStart = map.getStart();
        m_mappingTimeStep  = map.getStep();
    }

    if (m_mappingTimeStep <= 0.0) {
        m_mappingTimeStep = 1.0f;
    }
    
    /*
     * May not have mappings to voxels
     */
    if (m_voxelIndicesToOffset == NULL) {
        m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer());
    }
    
    int32_t numberOfMaps = 0;
    switch (m_fileMapDataType) {
        case FILE_MAP_DATA_TYPE_INVALID:
            break;
        case FILE_MAP_DATA_TYPE_MATRIX:
            numberOfMaps = 1;
            break;
        case FILE_MAP_DATA_TYPE_MULTI_MAP:
            switch (m_dataReadingAccessMethod) {
                case DATA_ACCESS_METHOD_INVALID:
                    CaretAssert(0);
                    break;
                case DATA_ACCESS_NONE:
                    break;
                case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
                    numberOfMaps = m_ciftiFile->getNumberOfColumns();
                    break;
                case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
                    numberOfMaps = m_ciftiFile->getNumberOfRows();
                    break;
            }
            break;
    }
    
    /*
     * Get data for maps.
     */
    for (int32_t i = 0; i < numberOfMaps; i++) {
        MapContent* mc = new MapContent(this,
                                        m_ciftiFile,
                                        m_fileMapDataType,
                                        m_dataReadingDirectionForCiftiXML,
                                        m_dataMappingDirectionForCiftiXML,
                                        i);
        m_mapContent.push_back(mc);
    }
    
    m_classNameHierarchy->update(this,
                                 true);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    m_classNameHierarchy->setAllSelected(true);
    
    m_fileFastStatistics.grabNew(NULL);
    m_fileHistogram.grabNew(NULL);
    m_fileHistorgramLimitedValues.grabNew(NULL);
    
    CaretLogFiner("CLASS/NAME Table for : "
                  + this->getFileNameNoPath()
                  + "\n"
                  + m_classNameHierarchy->toString());
    
    validateKeysAndLabels();
    
    validateAfterFileReading();
}

/**
 * This method is intended for overriding by subclasess so that they
 * can examine and verify the data that was read.  This method is
 * called after successfully reading a file.
 */
void
CiftiMappableDataFile::validateAfterFileReading()
{
    /* nothing - see method comment. */
}

/**
 * Write the file.
 *
 * @param ciftiMapFileName
 *    Name of the file to write.
 * @throw
 *    DataFileException if there is an error writing the file.
 */
void
CiftiMappableDataFile::writeFile(const AString& ciftiMapFileName)
{
    if (m_ciftiFile == NULL) {
        throw DataFileException(ciftiMapFileName
                                + " cannot be written because no file is loaded");
    }
    
    if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE) {
        throw DataFileException(ciftiMapFileName
                                + " dense connectivity files cannot be written to files due to their large sizes.");
    }
    
    m_ciftiFile->writeFile(ciftiMapFileName);
    setFileName(ciftiMapFileName);
    clearModified();
}

/**
 * @return Is the data mappable to a surface?
 */
bool
CiftiMappableDataFile::isSurfaceMappable() const
{
    return m_containsSurfaceData;
}


/**
 * @return Is the data mappable to a volume?
 */
bool
CiftiMappableDataFile::isVolumeMappable() const
{
    return m_containsVolumeData;
}

/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t
CiftiMappableDataFile::getNumberOfMaps() const
{
    return m_mapContent.size();
}

/**
 * @return True if the file has map attributes (name and metadata).
 * For files that do not have map attributes, they should override
 * this method and return false.  If not overriden, this method
 * returns true.
 *
 * Some files (such as CIFTI Connectivity Matrix Files and CIFTI
 * Data-Series Files) do not have Map Attributes and thus there
 * is no map name nor map metadata and options to edit these
 * attributes should not be presented to the user.
 *
 * These CIFTI files do contain palette color mapping but it is
 * associated with the file.  To simplify palette color mapping editing
 * these file will return the file's palette color mapping for any
 * calls to getMapPaletteColorMapping().
 */
bool
CiftiMappableDataFile::hasMapAttributes() const
{
    switch (m_fileMapDataType) {
        case FILE_MAP_DATA_TYPE_INVALID:
            break;
        case FILE_MAP_DATA_TYPE_MATRIX:
            break;
        case FILE_MAP_DATA_TYPE_MULTI_MAP:
            return true;
            break;
    }
    
    return false;
}

/**
 * Get the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString
CiftiMappableDataFile::getMapName(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->getName();
}

/**
 * Set the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mapName
 *    New name for the map.
 */
void
CiftiMappableDataFile::setMapName(const int32_t mapIndex,
                                   const AString& mapName)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    m_mapContent[mapIndex]->setName(mapName);
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map (const value).
 */
const GiftiMetaData*
CiftiMappableDataFile::getMapMetaData(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_metadata;
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map.
 */
GiftiMetaData*
CiftiMappableDataFile::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_metadata;
}

/**
 * Get the unique ID (UUID) for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    String containing UUID for the map.
 */
AString
CiftiMappableDataFile::getMapUniqueID(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    const GiftiMetaData* md = getMapMetaData(mapIndex);
    const AString uniqueID = md->getUniqueID();
    return uniqueID;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool
CiftiMappableDataFile::isMappedWithPalette() const
{
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            return true;
            break;
    }
    
    return false;
}

/**
 * @return True if file is mapped with a palette and one
 * palette is used for all maps.
 */
bool
CiftiMappableDataFile::isOnePaletteUsedForAllMaps() const
{
    bool onePaletteFlag = false;
    
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            switch (m_paletteColorMappingSource) {
                case PALETTE_COLOR_MAPPING_SOURCE_INVALID:
                    break;
                case PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE:
                    onePaletteFlag = true;
                    break;
                case PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP:
                    onePaletteFlag = false;
                    break;
            }
            break;
    }
    
    return onePaletteFlag;
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
CiftiMappableDataFile::getMapData(const int32_t mapIndex,
                                  std::vector<float>& dataOut) const
{
    CaretAssertVectorIndex(m_mapContent,
        mapIndex);
    
    CaretAssert(m_ciftiFile);
    CaretAssert(mapIndex >= 0);
    
    switch (m_dataReadingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            CaretAssert(mapIndex < m_ciftiFile->getNumberOfColumns());
            dataOut.resize(m_ciftiFile->getNumberOfRows());
            m_ciftiFile->getColumn(&dataOut[0],
                                   mapIndex);
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            CaretAssert(mapIndex < m_ciftiFile->getNumberOfRows());
            dataOut.resize(m_ciftiFile->getNumberOfColumns());
            m_ciftiFile->getRow(&dataOut[0],
                                mapIndex);
            break;
    }
}

/**
 * Set the data for the given map index.
 *
 * @param mapIndex
 *     Index of the map.
 * @param dataOut
 *     A vector that contains the data for the map.
 */
void
CiftiMappableDataFile::setMapData(const int32_t mapIndex,
                        const std::vector<float>& data)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    CaretAssert(m_ciftiFile);
    CaretAssert(mapIndex >= 0);
    
    switch (m_dataReadingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            CaretAssert(mapIndex < m_ciftiFile->getNumberOfColumns());
            m_ciftiFile->setColumn(&data[0],
                                   mapIndex);
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            CaretAssert(mapIndex < m_ciftiFile->getNumberOfRows());
            m_ciftiFile->setRow(&data[0],
                                mapIndex);
            break;
    }
    
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    m_mapContent[mapIndex]->updateForChangeInMapData();
}

/**
 * Update after a change in map data.
 *
 * @param mapIndex
 *    Index of map whose data has changed.
 */
void
CiftiMappableDataFile::updateForChangeInMapDataWithMapIndex(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    m_mapContent[mapIndex]->updateForChangeInMapData();
}


/**
 * Invalidate coloring in all maps
 */
void
CiftiMappableDataFile::invalidateColoringInAllMaps()
{
    const int64_t numMaps = static_cast<int64_t>(getNumberOfMaps());
    for (int64_t i = 0; i < numMaps; i++) {
        CaretAssertVectorIndex(m_mapContent, i);
        m_mapContent[i]->m_rgbaValid = false;
        
        /* invalidates histograms */
        updateForChangeInMapDataWithMapIndex(i);
    }
    
    /*
     * Force recreation of matrix so that it receives updates to coloring
     * and in particular, matrix grid outline coloring
     */
    m_matrixGraphicsPrimitive.reset();
    m_matrixGraphicsOutlinePrimitive.reset();
    invalidateHistogramChartColoring();
}

/**
 * Get all data within the file.
 *
 * @param data
 *    Filled with data and will contain (number-of-rows * number-of-columns)
 *    of data.
 */
void
CiftiMappableDataFile::getFileData(std::vector<float>& data) const
{
    switch (m_fileMapDataType) {
        case FILE_MAP_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case FILE_MAP_DATA_TYPE_MATRIX:
            break;
        case FILE_MAP_DATA_TYPE_MULTI_MAP:
            break;
    }
    
    CaretAssert(m_ciftiFile);
    const int64_t numRows = m_ciftiFile->getNumberOfRows();
    const int64_t numCols = m_ciftiFile->getNumberOfColumns();
    const int64_t dataSize = numRows * numCols;

    data.resize(dataSize);
    
    for (int64_t iRow = 0; iRow < numRows; iRow++) {
        m_ciftiFile->getRow(&data[iRow * numCols],
                            iRow);
    }
}

/**
 * Get the RGBA mapped version of the file's data matrix.
 *
 * @param rgba
 *    RGBA for file's matrix content.
 */
void
CiftiMappableDataFile::getMatrixRGBA(std::vector<float> &rgba)
{
    std::vector<float> data;
    getFileData(data);
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    PaletteColorMapping *paletteColorMapping = ciftiXML.getFilePalette();
    CaretAssert(paletteColorMapping);
    
    CaretPointer<FastStatistics> fastStatistics(new FastStatistics());
    if ( ! data.empty()) {
        
        fastStatistics->update(&data[0],
                               data.size());
        
        rgba.resize(data.size() * 4);
        NodeAndVoxelColoring::colorScalarsWithPalette(fastStatistics,
                                                      paletteColorMapping,
                                                      &data[0],
                                                      paletteColorMapping,
                                                      &data[0],
                                                      data.size(),
                                                      &rgba[0]);
    }
    else {
        std::fill(rgba.begin(),
                  rgba.end(),
                  0.0);
    }
}

/**
 * @return The graphics primitive containing the matrix representation of the file.
 * All cells are of dimension 1.0 x 1.0
 *
 * @param matrixViewMode
 *     The matrix visualization mode (upper/lower).
 * @param gridMode
 *     The grid mode (filled or outline)
 */
GraphicsPrimitiveV3fC4f*
CiftiMappableDataFile::getMatrixChartingGraphicsPrimitive(const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode,
                                                          const MatrixGridMode gridMode) const
{
    EventCaretPreferencesGet preferencesEvent;
    EventManager::get()->sendEvent(preferencesEvent.getPointer());
    CaretPreferences* caretPreferences = preferencesEvent.getCaretPreferences();
    uint8_t gridByteRGBA[4] = { 0, 0, 0, 0 };
    
    GraphicsPrimitiveV3fC4f* matrixPrimitive = NULL;
    switch (gridMode) {
        case MatrixGridMode::FILLED:
            matrixPrimitive = m_matrixGraphicsPrimitive.get();
            break;
        case MatrixGridMode::OUTLINE:
            caretPreferences->getBackgroundAndForegroundColors()->getColorChartMatrixGridLines(gridByteRGBA);
            matrixPrimitive = m_matrixGraphicsOutlinePrimitive.get();
            
            if ((gridByteRGBA[0] != m_previousMatrixGridRGBA[0])
                || (gridByteRGBA[1] != m_previousMatrixGridRGBA[1])
                || (gridByteRGBA[2] != m_previousMatrixGridRGBA[2])
                || (gridByteRGBA[3] != m_previousMatrixGridRGBA[3])) {
                matrixPrimitive = NULL;
                m_previousMatrixGridRGBA[0] = gridByteRGBA[0];
                m_previousMatrixGridRGBA[1] = gridByteRGBA[1];
                m_previousMatrixGridRGBA[2] = gridByteRGBA[2];
                m_previousMatrixGridRGBA[3] = gridByteRGBA[3];
            }
            break;
    }
    
    if (matrixPrimitive == NULL) {
        int32_t numberOfRows = 0;
        int32_t numberOfColumns = 0;
        std::vector<float> matrixRGBA;
        if (getMatrixForChartingRGBA(numberOfRows, numberOfColumns, matrixRGBA)) {
            const int32_t numberOfCells = numberOfRows * numberOfColumns;
            if (numberOfCells > 0) {
                switch (gridMode) {
                    case MatrixGridMode::FILLED:
                        matrixPrimitive = GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES);
                        matrixPrimitive->reserveForNumberOfVertices(numberOfCells * 6);  // 2 triangles per cell, 3 vertices per triangle
                        break;
                    case MatrixGridMode::OUTLINE:
                        /* Lines are used around each cell to simplify upper/lower triangular options */
                        matrixPrimitive = GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_LINES);
                        matrixPrimitive->reserveForNumberOfVertices(numberOfCells * 8);  // 4 lines per cell, 2 vertices per line
                        break;
                }
                matrixPrimitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_MANY_TIMES);
                
                /*
                 * RGBA for grid outline
                 */
                float cellOutlineRGBA[4] = { 1.0, 0.0, 0.0, 1.0 };
                if (caretPreferences != NULL) {
                    cellOutlineRGBA[0] = static_cast<float>(gridByteRGBA[0]) / 255.0f;
                    cellOutlineRGBA[1] = static_cast<float>(gridByteRGBA[1]) / 255.0f;
                    cellOutlineRGBA[2] = static_cast<float>(gridByteRGBA[2]) / 255.0f;
                    cellOutlineRGBA[3] = 1.0;
                }
                
                /*
                 * Alpha zero for cells that are "not drawn"
                 */
                const float cellNotDrawRGBA[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                
                /*
                 * NOTE: All matrix cells receive coloring, event those that are
                 * not displayed due to the triangular view selection.
                 * The reason is that it greatly simplifies identification as
                 * one can derive the row and column from the primitive index.
                 * Using triangles also simplifies identification.  Lastly, since
                 * OpenGL buffers are used, drawing is very fast.
                 */
                int32_t rgbaOffset = 0;
                const float cellHeight = 1.0;
                const float cellWidth = 1.0;
                float cellY = (numberOfRows - 1) * cellHeight;
                for (int32_t rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
                    float cellX = 0;
                    for (int32_t columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
                        CaretAssertVectorIndex(matrixRGBA, rgbaOffset+3);
                        const float* rgba = &matrixRGBA[rgbaOffset];
                        rgbaOffset += 4;
                        
                        bool drawCellFlag = true;
                        if (matrixViewMode != ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL) {
                            if (numberOfRows == numberOfColumns) {
                                drawCellFlag = false;
                                switch (matrixViewMode) {
                                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                                        break;
                                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                                        if (rowIndex != columnIndex) {
                                            drawCellFlag = true;
                                        }
                                        break;
                                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                                        if (rowIndex > columnIndex) {
                                            drawCellFlag = true;
                                        }
                                        break;
                                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                                        if (rowIndex < columnIndex) {
                                            drawCellFlag = true;
                                        }
                                        break;
                                }
                            }
                            else {
                                drawCellFlag = true;
                                
                                /*
                                 * Diagonals for non-square matrices not allowed
                                 */
                                const bool allowNonSquareMatrixDiagonalsFlag = false;
                                if (allowNonSquareMatrixDiagonalsFlag) {
                                    drawCellFlag = false;
                                    const float slope = static_cast<float>(numberOfRows) / static_cast<float>(numberOfColumns);
                                    const int32_t diagonalRow = static_cast<int32_t>(slope * columnIndex);
                                    
                                    switch (matrixViewMode) {
                                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                                            drawCellFlag = true;
                                            break;
                                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                                            if (rowIndex != diagonalRow) {
                                                drawCellFlag = true;
                                            }
                                            break;
                                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                                            if (rowIndex > diagonalRow) {
                                                drawCellFlag = true;
                                            }
                                            break;
                                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                                            if (rowIndex < diagonalRow) {
                                                drawCellFlag = true;
                                            }
                                            break;
                                    }
                                }
                            }
                        }
                        
                        switch (gridMode) {
                            case MatrixGridMode::FILLED:
                            {
                                const float* cellRGBA = (drawCellFlag ? rgba : cellNotDrawRGBA);
                                matrixPrimitive->addVertex(cellX, cellY + cellHeight, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX, cellY, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX + cellWidth, cellY, 0.0, cellRGBA);
                                
                                matrixPrimitive->addVertex(cellX, cellY + cellHeight, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX + cellWidth, cellY, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, cellRGBA);
                            }
                                break;
                            case MatrixGridMode::OUTLINE:
                            {
                                const float* cellRGBA = (drawCellFlag ? cellOutlineRGBA : cellNotDrawRGBA);
                                matrixPrimitive->addVertex(cellX, cellY, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX + cellWidth, cellY, 0.0, cellRGBA);
                                
                                matrixPrimitive->addVertex(cellX + cellWidth, cellY, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, cellRGBA);

                                matrixPrimitive->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX, cellY + cellHeight, 0.0, cellRGBA);
                                
                                matrixPrimitive->addVertex(cellX, cellY + cellHeight, 0.0, cellRGBA);
                                matrixPrimitive->addVertex(cellX, cellY, 0.0, cellRGBA);
                            }
                                break;
                        }
                        
                        cellX += cellWidth;
                    }
                    
                    cellY -= cellHeight;
                }
            }
        }
    }
    
    switch (gridMode) {
        case MatrixGridMode::FILLED:
            if (matrixPrimitive != m_matrixGraphicsPrimitive.get()) {
                m_matrixGraphicsPrimitive.reset(matrixPrimitive);
            }
            break;
        case MatrixGridMode::OUTLINE:
            if (matrixPrimitive != m_matrixGraphicsOutlinePrimitive.get()) {
                m_matrixGraphicsOutlinePrimitive.reset(matrixPrimitive);
            }
            break;
    }
    
    return matrixPrimitive;
}


/**
 * Get the matrix RGBA coloring for this matrix data creator.
 *
 * @param numberOfRowsOut
 *    Number of rows in the coloring matrix.
 * @param numberOfColumnsOut
 *    Number of rows in the coloring matrix.
 * @param rgbaOut
 *    RGBA coloring output with number of elements
 *    (numberOfRowsOut * numberOfColumnsOut * 4).
 * @return
 *    True if data output data is valid, else false.
 */
bool
CiftiMappableDataFile::getMatrixForChartingRGBA(int32_t& numberOfRowsOut,
                                             int32_t& numberOfColumnsOut,
                                             std::vector<float>& rgbaOut) const
{
    bool useMapFileHelperFlag = false;
    bool useMatrixFileHelperFlag = false;
    
    std::vector<int32_t> parcelReorderedRowIndices;
    
    switch (getDataFileType()) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
        {
            useMatrixFileHelperFlag    = true;
            
            const CiftiConnectivityMatrixParcelFile* parcelConnFile = dynamic_cast<const CiftiConnectivityMatrixParcelFile*>(this);
            CaretAssert(parcelConnFile);
            if (parcelConnFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelConnFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                              parcelLabelReorderingFile,
                                                                              parcelLabelFileMapIndex,
                                                                              reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelConnFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                        parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
        {
            useMapFileHelperFlag = true;
            
            const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(this);
            CaretAssert(parcelLabelFile);
            if (parcelLabelFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelLabelFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                               parcelLabelReorderingFile,
                                                                               parcelLabelFileMapIndex,
                                                                               reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelLabelFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                         parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
        {
            useMapFileHelperFlag = true;
            
            const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(this);
            CaretAssert(parcelScalarFile);
            if (parcelScalarFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelScalarFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                                parcelLabelReorderingFile,
                                                                                parcelLabelFileMapIndex,
                                                                                reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelScalarFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                          parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
        {
            useMapFileHelperFlag = true;
            
            const CiftiParcelSeriesFile* parcelSeriesFile = dynamic_cast<const CiftiParcelSeriesFile*>(this);
            CaretAssert(parcelSeriesFile);
            if (parcelSeriesFile != NULL) {
                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
                int32_t parcelLabelFileMapIndex = -1;
                bool reorderingEnabledFlag = false;
                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
                parcelSeriesFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                                                parcelLabelReorderingFile,
                                                                                parcelLabelFileMapIndex,
                                                                                reorderingEnabledFlag);
                
                if (reorderingEnabledFlag) {
                    const CiftiParcelReordering* parcelReordering = parcelSeriesFile->getParcelReordering(parcelLabelReorderingFile,
                                                                                                          parcelLabelFileMapIndex);
                    if (parcelReordering != NULL) {
                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            useMatrixFileHelperFlag = true;
            break;
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
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
        case DataFileTypeEnum::UNKNOWN:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
    }
    
    if (( ! useMapFileHelperFlag)
        && ( ! useMatrixFileHelperFlag)) {
        CaretAssertMessage(0, "Trying to get matrix from a file that does not support matrix display");
        return false;
    }
    
    bool validDataFlag = false;
    if (useMapFileHelperFlag) {
        validDataFlag = helpMapFileLoadChartDataMatrixRGBA(numberOfRowsOut,
                                                                         numberOfColumnsOut,
                                                                         parcelReorderedRowIndices,
                                                                         rgbaOut);
    }
    else if (useMatrixFileHelperFlag) {
        validDataFlag = helpMatrixFileLoadChartDataMatrixRGBA(numberOfRowsOut,
                                                                            numberOfColumnsOut,
                                                                            parcelReorderedRowIndices,
                                                                            rgbaOut);
    }
    
    return validDataFlag;
}

/**
 * Get the dimensions of the file (rows and columns).
 *
 * @param dim
 *     Contains dimensions upon exit.
 */
void
CiftiMappableDataFile::getMapDimensions(std::vector<int64_t> &dim) const
{
    CaretAssert(m_ciftiFile);
    dim.clear();
    dim.push_back(m_ciftiFile->getNumberOfColumns());
    dim.push_back(m_ciftiFile->getNumberOfRows());
}


/**
 * Get statistics describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Fast statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const FastStatistics*
CiftiMappableDataFile::getMapFastStatistics(const int32_t mapIndex)
{
    FastStatistics* fastStatsOut = NULL;
    
    if (isMappedWithPalette()) {
            CaretAssertVectorIndex(m_mapContent,
                                   mapIndex);
            
            if ( ! m_mapContent[mapIndex]->isFastStatisticsValid()) {
                std::vector<float> data;
                getMapData(mapIndex,
                           data);
                m_mapContent[mapIndex]->updateFastStatistics(data);
            }
            
            fastStatsOut =  m_mapContent[mapIndex]->m_fastStatistics;
    }
                
    return fastStatsOut;
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiMappableDataFile::getMapHistogram(const int32_t mapIndex)
{
    Histogram* histogramOut = NULL;
    
    if (isMappedWithPalette()) {
        CaretAssertVectorIndex(m_mapContent,
                               mapIndex);
        int32_t numberOfBuckets = 0;
        switch (getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                numberOfBuckets = getFileHistogramNumberOfBuckets();
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
                break;
        }
        
        if ( ! m_mapContent[mapIndex]->isHistogramValid(numberOfBuckets)) {
            std::vector<float> data;
            getMapData(mapIndex,
                       data);
            m_mapContent[mapIndex]->updateHistogram(numberOfBuckets,
                                                    data);
        }
        
        histogramOut = m_mapContent[mapIndex]->m_histogram;
    }

    return histogramOut;
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette at the given index for
 * data within the given ranges.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiMappableDataFile::getMapHistogram(const int32_t mapIndex,
                                        const float mostPositiveValueInclusive,
                                        const float leastPositiveValueInclusive,
                                        const float leastNegativeValueInclusive,
                                        const float mostNegativeValueInclusive,
                                        const bool includeZeroValues)
{
    Histogram* histogramOut = NULL;

    if (isMappedWithPalette()) {
        CaretAssertVectorIndex(m_mapContent,
                               mapIndex);
        int32_t numberOfBuckets = 0;
        switch (getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                numberOfBuckets = getFileHistogramNumberOfBuckets();
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
                break;
        }
        if ( ! m_mapContent[mapIndex]->isHistogramLimitedValuesValid(numberOfBuckets,
                                                                     mostPositiveValueInclusive,
                                                                     leastPositiveValueInclusive,
                                                                     leastNegativeValueInclusive,
                                                                     mostNegativeValueInclusive,
                                                                     includeZeroValues)) {
            std::vector<float> data;
            getMapData(mapIndex,
                       data);
            m_mapContent[mapIndex]->updateHistogramLimitedValues(numberOfBuckets,
                                                                 data,
                                                                 mostPositiveValueInclusive,
                                                                 leastPositiveValueInclusive,
                                                                 leastNegativeValueInclusive,
                                                                 mostNegativeValueInclusive,
                                                                 includeZeroValues);
        }
        
        histogramOut = m_mapContent[mapIndex]->m_histogramLimitedValues;
    }
    
    return histogramOut;    
}

/**
 * @return The estimated size of data after it is uncompressed
 * and loaded into RAM.  A negative value indicates that the
 * file size cannot be computed.
 */
int64_t
CiftiMappableDataFile::getDataSizeUncompressedInBytes() const
{
    int64_t dataSize = 0;
    
    if (m_ciftiFile != NULL) {
        dataSize = (m_ciftiFile->getNumberOfColumns()
                    * m_ciftiFile->getNumberOfRows()
                    * sizeof(float));
    }
    
    return dataSize;
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette for all data within the file.
 *
 * @return
 *    Fast statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const FastStatistics*
CiftiMappableDataFile::getFileFastStatistics()
{
    if (m_fileFastStatistics == NULL) {
        std::vector<float> fileData;
        getFileData(fileData);
        if ( ! fileData.empty()) {
            m_fileFastStatistics.grabNew(new FastStatistics());
            m_fileFastStatistics->update(&fileData[0],
                                         fileData.size());
        }
    }
    
    return m_fileFastStatistics;
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette for all data within
 * the file.
 *
 * @return
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiMappableDataFile::getFileHistogram()
{
    bool updateHistogramFlag = false;
    const int32_t numberOfBuckets = getFileHistogramNumberOfBuckets();
    if (m_fileHistogram != NULL) {
        if (numberOfBuckets != m_fileHistogramNumberOfBuckets) {
            updateHistogramFlag = true;
        }
    }
    else {
        updateHistogramFlag = true;
    }
    if (updateHistogramFlag) {
        std::vector<float> fileData;
        getFileData(fileData);
        
        if ( ! fileData.empty()) {
            if (m_fileHistogram == NULL) {
                m_fileHistogram.grabNew(new Histogram(numberOfBuckets));
            }
            m_fileHistogram->update(numberOfBuckets,
                                    &fileData[0],
                                    fileData.size());
            m_fileHistogramNumberOfBuckets = numberOfBuckets;
        }
    }
    return m_fileHistogram;
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette for all data in the file
 * within the given range of values.
 *
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiMappableDataFile::getFileHistogram(const float mostPositiveValueInclusive,
                                           const float leastPositiveValueInclusive,
                                           const float leastNegativeValueInclusive,
                                           const float mostNegativeValueInclusive,
                                           const bool includeZeroValues)
{
    bool updateHistogramFlag = false;
    const int32_t numberOfBuckets = getFileHistogramNumberOfBuckets();
    if (m_fileHistorgramLimitedValues != NULL) {
        if ((numberOfBuckets != m_fileHistogramLimitedValuesNumberOfBuckets)
            ||(mostPositiveValueInclusive != m_fileHistogramLimitedValuesMostPositiveValueInclusive)
            || (leastPositiveValueInclusive != m_fileHistogramLimitedValuesLeastPositiveValueInclusive)
            || (leastNegativeValueInclusive != m_fileHistogramLimitedValuesLeastNegativeValueInclusive)
            || (mostNegativeValueInclusive != m_fileHistogramLimitedValuesMostNegativeValueInclusive)
            || (includeZeroValues != m_fileHistogramLimitedValuesIncludeZeroValues)) {
            updateHistogramFlag = true;
        }
    }
    else {
        updateHistogramFlag = true;
    }
    
    if (updateHistogramFlag) {
        std::vector<float> fileData;
        getFileData(fileData);
        if ( ! fileData.empty()) {
            if (m_fileHistorgramLimitedValues == NULL) {
                m_fileHistorgramLimitedValues.grabNew(new Histogram());
            }
            m_fileHistorgramLimitedValues->update(numberOfBuckets,
                                                  &fileData[0],
                                                  fileData.size(),
                                                  mostPositiveValueInclusive,
                                                  leastPositiveValueInclusive,
                                                  leastNegativeValueInclusive,
                                                  mostNegativeValueInclusive,
                                                  includeZeroValues);
            
            m_fileHistogramLimitedValuesNumberOfBuckets             = numberOfBuckets;
            m_fileHistogramLimitedValuesMostPositiveValueInclusive  = mostPositiveValueInclusive;
            m_fileHistogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
            m_fileHistogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
            m_fileHistogramLimitedValuesMostNegativeValueInclusive  = mostNegativeValueInclusive;
            m_fileHistogramLimitedValuesIncludeZeroValues           = includeZeroValues;
        }
    }
    
    return m_fileHistorgramLimitedValues;
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (will be NULL for data
 *    not mapped using a palette).
 */
PaletteColorMapping*
CiftiMappableDataFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    CaretAssert(m_ciftiFile);
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    PaletteColorMapping* pcm = NULL;
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            switch (m_paletteColorMappingSource) {
                case PALETTE_COLOR_MAPPING_SOURCE_INVALID:
                    break;
                case PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE:
                    pcm = ciftiXML.getFilePalette();
                    break;
                case PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP:
                    CaretAssertVectorIndex(m_mapContent,
                                           mapIndex);
                    pcm = m_mapContent[mapIndex]->m_paletteColorMapping;
                    break;
            }
            break;
    }
    
    return pcm;
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (constant) (will be NULL for data
 *    not mapped using a palette).
 */
const PaletteColorMapping*
CiftiMappableDataFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    CaretAssert(m_ciftiFile);
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    PaletteColorMapping* pcm = NULL;
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            switch (m_paletteColorMappingSource) {
                case PALETTE_COLOR_MAPPING_SOURCE_INVALID:
                    break;
                case PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE:
                    pcm = ciftiXML.getFilePalette();
                    break;
                case PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP:
                    CaretAssertVectorIndex(m_mapContent,
                                           mapIndex);
                    pcm = m_mapContent[mapIndex]->m_paletteColorMapping;
                    break;
            }
            break;
    }
    
    return pcm;
}

/**
 * Get the CIFTI parcels map used for brainordinate mapping.
 *
 * @return
 *     Pointer to the map's Cifti Parcels Map or NULL if the file is not
 *     mapped using parcels.
 */
const CiftiParcelsMap*
CiftiMappableDataFile::getCiftiParcelsMapForBrainordinateMapping() const
{
    CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
    
    return getCiftiParcelsMapForDirection(m_dataMappingDirectionForCiftiXML);
}

/**
 * Get the CIFTI parcels map used for data loading.
 *
 * @return
 *     Pointer to the map's Cifti Parcels Map or NULL if the file is not
 *     loaded using parcels.
 */
const CiftiParcelsMap*
CiftiMappableDataFile::getCiftiParcelsMapForLoading() const
{
    CaretAssert((m_dataReadingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataReadingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
    
    return getCiftiParcelsMapForDirection(m_dataReadingDirectionForCiftiXML);
}

/**
 * Get the CIFTI parcels for the given direction.
 *
 * @param direction
 *     Direction of mapping.  MUST BE one of CiftiXML::ALONG_ROW or
 *     CiftiXML::ALONG_COLUMN.
 * @return
 *     Pointer to the map's Cifti Parcels Map or NULL if the file is not
 *     mapped using parcels or NULL if direction is invalid.
 */
const CiftiParcelsMap*
CiftiMappableDataFile::getCiftiParcelsMapForDirection(const int direction) const
{
    if (m_ciftiFile != NULL) {
        if ((direction != CiftiXML::ALONG_ROW)
            && (direction != CiftiXML::ALONG_COLUMN)) {
            CaretAssert(0);
            return NULL;
        }
        
        const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
        const CiftiMappingType* mapping = ciftiXML.getMap(direction);
        if (mapping->getType() == CiftiMappingType::PARCELS) {
            const CiftiParcelsMap* cpm = dynamic_cast<const CiftiParcelsMap*>(mapping);
            CaretAssert(cpm);
            return cpm;
        }
    }
    
    return NULL;
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool
CiftiMappableDataFile::isMappedWithLabelTable() const
{
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            return true;
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            break;
    }
    
    return false;
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (will be NULL for data
 *    not mapped using a label table).
 */
GiftiLabelTable*
CiftiMappableDataFile::getMapLabelTable(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            return m_mapContent[mapIndex]->m_labelTable;
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            break;
    }
    
    return NULL;
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (constant) (will be NULL for data
 *    not mapped using a label table).
 */
const GiftiLabelTable*
CiftiMappableDataFile::getMapLabelTable(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    switch (m_colorMappingMethod) {
        case COLOR_MAPPING_METHOD_INVALID:
            break;
        case COLOR_MAPPING_METHOD_LABEL_TABLE:
            return m_mapContent[mapIndex]->m_labelTable;
            break;
        case COLOR_MAPPING_METHOD_PALETTE:
            break;
    }
    
    return NULL;
}

/**
 * Get the palette normalization modes that are supported by the file.
 *
 * @param modesSupportedOut
 *     Palette normalization modes supported by a file.  Will be
 *     empty for files that are not mapped with a palette.  If there
 *     is more than one suppported mode, the first mode in the
 *     vector is assumed to be the default mode.
 */
void
CiftiMappableDataFile::getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const
{
    modesSupportedOut = m_paletteNormalizationModesSupported;
}

/**
 * Update coloring for all maps.
 *
 * Note: Overridden since Data-Series files have one palette that is
 * applied to ALL maps.  For data-series, just invalidate the coloring
 * for all maps (data points).
 */
void
CiftiMappableDataFile::updateScalarColoringForAllMaps()
{
    /*
     * Just need to invalidate coloring.
     * Updating coloring for all maps would take time.
     * Coloring update is triggered by code that colors nodes/voxels
     * when drawing.
     */
    invalidateColoringInAllMaps();
}

/**
 * Update scalar coloring for a map.
 *
 * Note that some CIFTI files can be slow to color due to the need to
 * retrieve data for the map.  Use isMapColoringValid() to avoid 
 * unnecessary calls to isMapColoringValid.
 *
 * @param mapIndex
 *    Index of map.
 */
void
CiftiMappableDataFile::updateScalarColoringForMap(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    std::vector<float> data;
    getMapData(mapIndex,
               data);

    m_mapContent[mapIndex]->m_rgbaValid = false;
    if (isMappedWithPalette()) {
        
        FastStatistics* statistics = NULL;
        switch (getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                statistics = const_cast<FastStatistics*>(getFileFastStatistics());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                statistics = const_cast<FastStatistics*>(getMapFastStatistics(mapIndex));
                break;
        }
        
        m_mapContent[mapIndex]->updateColoring(data,
                                               statistics);
    }
    else if (isMappedWithLabelTable()) {
        if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC) {
            CiftiBrainordinateLabelDynamicFile* dynLabelFile = dynamic_cast<CiftiBrainordinateLabelDynamicFile*>(this);
            CaretAssert(dynLabelFile);
            CiftiMappableDataFile* ciftiParentFile = dynamic_cast<CiftiMappableDataFile*>(dynLabelFile->getParentMappableDataFile());
            CaretAssert(ciftiParentFile);
            ciftiParentFile->updateScalarColoringForMap(mapIndex);
            
            const int32_t mapCount = m_mapContent[mapIndex]->m_dataCount;
            CaretAssertVectorIndex(m_mapContent, mapIndex);
            CaretAssertVectorIndex(ciftiParentFile->m_mapContent, mapIndex);
            CaretAssert(mapCount == ciftiParentFile->m_mapContent[mapIndex]->m_dataCount);
            
            std::vector<float> mapData(mapCount);
            for (int32_t i = 0; i < mapCount; i++) {
                const int32_t alphaIndex = i*4 + 3;
                CaretAssertVectorIndex(ciftiParentFile->m_mapContent[mapIndex]->m_rgba, alphaIndex);
                CaretAssertVectorIndex(mapData, i);
                mapData[i] = 0.0;
                if (ciftiParentFile->m_mapContent[mapIndex]->m_rgba[alphaIndex] > 0) {
                    mapData[i] = 1.0;
                }
            }
            
            setMapData(mapIndex, mapData);
        }
        
        m_mapContent[mapIndex]->updateColoring(data,
                                               NULL);
    }
    else {
        CaretAssert(0);
    }
    
    /*
     * Force recreation of matrix so that it receives updates to coloring.
     */
    
    invalidateHistogramChartColoring();
    m_matrixGraphicsPrimitive.reset();
    m_matrixGraphicsOutlinePrimitive.reset();
}

/**
 * Note that some CIFTI files can be slow to color due to the need to
 * retrieve data for the map.  This method can be used to avoid calls
 * to updateScalarColoringForMap.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    True if the coloring for the given map index is valid.
 */
bool
CiftiMappableDataFile::isMapColoringValid(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_rgbaValid;
}

/**
 * Get the node ins the parcel of the given index.
 * @param parcelNodes
 *     Output containing the node indices.
 * @param structure
 *     Structure for which the node indices are requested.
 * @param selectionIndex
 *     Index of the parcel.
 * @return true if parcel is valid, else false.
 */
bool
CiftiMappableDataFile::getParcelNodesElementForSelectedParcel(std::set<int64_t> &parcelNodesOut,
                                                                 const StructureEnum::Enum &structure,
                                                                 const int64_t &selectionIndex) const
{
    if (m_ciftiFile->getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::PARCELS) return false;
    const std::vector<CiftiParcelsMap::Parcel>& parcels = m_ciftiFile->getCiftiXML().getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
    
    if(selectionIndex >= 0 && selectionIndex < (int64_t)parcels.size())
    {
        const CiftiParcelsMap::Parcel& parcelOut = parcels[selectionIndex];
        std::map<StructureEnum::Enum, std::set<int64_t> >::const_iterator findStruct = parcelOut.m_surfaceNodes.find(structure);
        if (findStruct != parcelOut.m_surfaceNodes.end())
        {
            parcelNodesOut = findStruct->second;
            return true;
        }
    }
    return false;
}

/**
 * Get the dimensions of the volume.
 *
 * @param dimOut1
 *     First dimension (i) out.
 * @param dimOut2
 *     Second dimension (j) out.
 * @param dimOut3
 *     Third dimension (k) out.
 * @param dimTimeOut
 *     Time dimensions out (number of maps)
 * @param numComponentsOut
 *     Number of components per voxel.
 */
void
CiftiMappableDataFile::getDimensions(int64_t& dimOut1,
                           int64_t& dimOut2,
                           int64_t& dimOut3,
                           int64_t& dimTimeOut,
                           int64_t& numComponentsOut) const
{
    CaretAssert(m_ciftiFile);
    
    dimOut1 = 0;
    dimOut2 = 0;
    dimOut3 = 0;
    dimTimeOut = 0;
    numComponentsOut = 0;
    
    if (m_dataMappingDirectionForCiftiXML != S_CIFTI_XML_ALONG_INVALID) {
    switch (m_ciftiFile->getCiftiXML().getMappingType(m_dataMappingDirectionForCiftiXML))
    {
        case CiftiMappingType::BRAIN_MODELS:
        {
            const CiftiBrainModelsMap& myDenseMap = m_ciftiFile->getCiftiXML().getBrainModelsMap(m_dataMappingDirectionForCiftiXML);
            if (!myDenseMap.hasVolumeData()) return;
            const VolumeSpace& mySpace = myDenseMap.getVolumeSpace();
            const int64_t* dims = mySpace.getDims();
            dimOut1 = dims[0];
            dimOut2 = dims[1];
            dimOut3 = dims[2];
            dimTimeOut = 1;//???
            numComponentsOut = 1;
            break;
        }
        case CiftiMappingType::PARCELS:
        {
            const CiftiParcelsMap& myParcelMap = m_ciftiFile->getCiftiXML().getParcelsMap(m_dataMappingDirectionForCiftiXML);
            if (!myParcelMap.hasVolumeData()) return;
            const VolumeSpace& mySpace = myParcelMap.getVolumeSpace();
            const int64_t* dims = mySpace.getDims();
            dimOut1 = dims[0];
            dimOut2 = dims[1];
            dimOut3 = dims[2];
            dimTimeOut = 1;//???
            numComponentsOut = 1;
            break;
        }
        default://nothing else has volume dimensions
            break;
    }
    }
}

/**
 * Get the dimensions of the volume.
 *
 * @param dimsOut
 *     Will contain 5 elements: (0) X-dimension, (1) Y-dimension
 * (2) Z-dimension, (3) time, (4) components.
 */
void
CiftiMappableDataFile::getDimensions(std::vector<int64_t>& dimsOut) const
{
    dimsOut.resize(5);
    
    int64_t dimI, dimJ, dimK, dimTime, dimComp;
    getDimensions(dimI,
                  dimJ,
                  dimK,
                  dimTime,
                  dimComp);
    
    dimsOut[0] = dimI;
    dimsOut[1] = dimJ;
    dimsOut[2] = dimK;
    dimsOut[3] = dimTime;
    dimsOut[4] = dimComp;
}

/**
 * @return The number of componenents per voxel in the volume data.
 */
const int64_t&
CiftiMappableDataFile::getNumberOfComponents() const
{
    int64_t dimI, dimJ, dimK, dimTime;
    
    static int64_t dimComp = 0;
    getDimensions(dimI,
                  dimJ,
                  dimK,
                  dimTime,
                  dimComp);
    return dimComp;
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param coordOut1
 *     Output first (x) coordinate.
 * @param coordOut2
 *     Output first (y) coordinate.
 * @param coordOut3
 *     Output first (z) coordinate.
 */
void
CiftiMappableDataFile::indexToSpace(const float& indexIn1,
                          const float& indexIn2,
                          const float& indexIn3,
                          float& coordOut1,
                          float& coordOut2,
                          float& coordOut3) const
{
    CaretAssert(m_voxelIndicesToOffset);
    m_voxelIndicesToOffset->indicesToCoordinate(indexIn1,
                                                indexIn2,
                                                indexIn3,
                                                coordOut1,
                                                coordOut2,
                                                coordOut3);
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param coordOut
 *     Output XYZ coordinates.
 */
void
CiftiMappableDataFile::indexToSpace(const float& indexIn1,
                          const float& indexIn2,
                          const float& indexIn3,
                          float* coordOut) const
{
    CaretAssert(m_voxelIndicesToOffset);
    m_voxelIndicesToOffset->indicesToCoordinate(indexIn1,
                                                indexIn2,
                                                indexIn3,
                                                coordOut[0],
                                                coordOut[1],
                                                coordOut[2]);
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn
 *     IJK indices
 * @param coordOut
 *     Output XYZ coordinates.
 */
void
CiftiMappableDataFile::indexToSpace(const int64_t* indexIn,
                          float* coordOut) const
{
    CaretAssert(m_voxelIndicesToOffset);
    m_voxelIndicesToOffset->indicesToCoordinate(indexIn[0],
                                                indexIn[1],
                                                indexIn[2],
                                                coordOut[0],
                                                coordOut[1],
                                                coordOut[2]);
}

/**
 * Convert a coordinate to indices.  Note that output indices
 * MAY NOT BE WITHIN THE VALID VOXEL DIMENSIONS.
 *
 * @param coordIn1
 *     First (x) input coordinate.
 * @param coordIn2
 *     Second (y) input coordinate.
 * @param coordIn3
 *     Third (z) input coordinate.
 * @param indexOut1
 *     First output index (i).
 * @param indexOut2
 *     First output index (j).
 * @param indexOut3
 *     First output index (k).
 */
void
CiftiMappableDataFile::enclosingVoxel(const float& coordIn1,
                                      const float& coordIn2,
                                      const float& coordIn3,
                                      int64_t& indexOut1,
                                      int64_t& indexOut2,
                                      int64_t& indexOut3) const
{
    CaretAssert(m_voxelIndicesToOffset);
    m_voxelIndicesToOffset->coordinateToIndices(coordIn1,
                                                coordIn2,
                                                coordIn3,
                                                indexOut1,
                                                indexOut2,
                                                indexOut3);
}

/**
 * Determine in the given voxel indices are valid (within the volume dimensions).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param coordOut1
 *     Output first (x) coordinate.
 * @param brickIndex
 *     Time/map index (default 0).
 * @param component
 *     Voxel component (default 0).
 */
bool
CiftiMappableDataFile::indexValid(const int64_t& indexIn1,
                        const int64_t& indexIn2,
                        const int64_t& indexIn3,
                        const int64_t /*brickIndex*/,
                        const int64_t /*component*/) const
{
    std::vector<int64_t> volumeDimensions;
    getDimensions(volumeDimensions);
    CaretAssertVectorIndex(volumeDimensions, 2);
    if ((indexIn1 >= 0)
        && (indexIn1 < volumeDimensions[0])
        && (indexIn2 >= 0)
        && (indexIn2 < volumeDimensions[1])
        && (indexIn3 >= 0)
        && (indexIn3 < volumeDimensions[2])) {
        return true;
    }
    
    return false;
}

const VolumeSpace& CiftiMappableDataFile::getVolumeSpace() const
{
    CaretAssert(m_voxelIndicesToOffset);//because this is where the other space functions get their volume space from, just roll with it for now
    return m_voxelIndicesToOffset->getVolumeSpace();
}

/**
 * Get a bounding box for the voxel coordinate ranges.
 *
 * @param boundingBoxOut
 *    The output bounding box.
 */
void
CiftiMappableDataFile::getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const
{
    CaretAssert(m_voxelIndicesToOffset);
    
    boundingBoxOut.resetForUpdate();
    
    std::vector<int64_t> volumeDimensions(5, 0);
    getDimensions(volumeDimensions);
    CaretAssertVectorIndex(volumeDimensions, 2);

    if (m_voxelIndicesToOffset->isValid()) {
        float xyz[3];
        indexToSpace(0,
                     0,
                     0,
                     xyz);
        boundingBoxOut.update(xyz);
        
        indexToSpace(volumeDimensions[0] - 1,
                     volumeDimensions[1] - 1,
                     volumeDimensions[2] - 1,
                     xyz);
        
        boundingBoxOut.update(xyz);
    }
    else {
        boundingBoxOut.resetZeros();
    }
}

/**
 * Get the voxel colors for a slice in the map.
 *
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    The slice plane.
 * @param sliceIndex
 *    Index of the slice.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *    Output containing the rgba values (must have been allocated
 *    by caller to sufficient count of elements in the slice).
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
CiftiMappableDataFile::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                      const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                      const int64_t sliceIndex,
                                                      const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      uint8_t* rgbaOut) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    CaretAssertMessage((sliceIndex >= 0),
                       "Slice index is invalid.");
    if (sliceIndex < 0) {
        return 0;
    }
    
    if (isMapColoringValid(mapIndex) == false) {
        CiftiMappableDataFile* nonConstThis = const_cast<CiftiMappableDataFile*>(this);
        nonConstThis->updateScalarColoringForMap(mapIndex);
    }
    
    int64_t dimI, dimJ, dimK, dimTime, dimComp;
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    int64_t voxelCount = 0;
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            voxelCount = dimI * dimJ;
            CaretAssert((sliceIndex < dimK));
            if (sliceIndex >= dimK) {
                return 0;
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            voxelCount = dimI * dimK;
            CaretAssert((sliceIndex < dimJ));
            if (sliceIndex >= dimJ) {
                return 0;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            voxelCount = dimJ * dimK;
            CaretAssert((sliceIndex < dimI));
            if (sliceIndex >= dimI) {
                return 0;
            }
            break;
    }
    
    if (voxelCount <= 0) {
        return 0;
    }
    const int64_t componentCount = voxelCount * 4;
    
    /*
     * Clear the slice rgba coloring.
     */
    for (int64_t i = 0; i < componentCount; i++) {
        rgbaOut[i] = 0;
    }
    
    const int64_t mapRgbaCount = m_mapContent[mapIndex]->m_rgba.size();
    
    
    /*
     * RGBA size will be zero if no data has been loaded for a CIFTI
     * matrix type file (user clicking brainordinate).
     */
    if (mapRgbaCount <= 0) {
        return 0;
    }
    
    const uint8_t* mapRGBA = &m_mapContent[mapIndex]->m_rgba[0];
    
    CaretAssert(m_voxelIndicesToOffset);

    /*
     * Data values are only needed when a label volume
     * is being drawn so that we can determine if the
     * label is displayed.
     */
    std::vector<float> dataValues;
    const GiftiLabelTable* labelTable = (isMappedWithLabelTable()
                                         ? getMapLabelTable(mapIndex)
                                         : NULL);
    if (isMappedWithLabelTable()) {
        CaretAssert(labelTable);
        getMapData(mapIndex,
                   dataValues);
    }
    
    int64_t validVoxelCount = 0;
    
    /*
     * Set the rgba components for the slice.
     */
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            for (int64_t j = 0; j < dimJ; j++) {
                for (int64_t i = 0; i < dimI; i++) {
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(i,
                                                                                     j,
                                                                                     sliceIndex);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        const int64_t rgbaOffset = ((j * dimI) + i) * 4;
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                }
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            for (int64_t k = 0; k < dimK; k++) {
                for (int64_t i = 0; i < dimI; i++) {
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(i,
                                                                                           sliceIndex,
                                                                                           k);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        const int64_t rgbaOffset = ((k * dimI) + i) * 4;
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                }
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            for (int64_t k = 0; k < dimK; k++) {
                for (int64_t j = 0; j < dimJ; j++) {
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(sliceIndex,
                                                                                           j,
                                                                                           k);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        const int64_t rgbaOffset = ((k * dimJ) + j) * 4;
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                }
            }
            break;
    }
    
    return validVoxelCount;
}

/**
 * Get voxel coloring for a set of voxels.
 *
 * @param mapIndex
 *     Index of map.
 * @param firstVoxelIJK
 *    IJK Indices of first voxel
 * @param rowStepIJK
 *    IJK Step for moving to next row.
 * @param columnStepIJK
 *    IJK Step for moving to next column.
 * @param numberOfRows
 *    Number of rows.
 * @param numberOfColumns
 *    Number of columns.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *    RGBA color components out.
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
CiftiMappableDataFile::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                      const int64_t firstVoxelIJK[3],
                                                      const int64_t rowStepIJK[3],
                                                      const int64_t columnStepIJK[3],
                                                      const int64_t numberOfRows,
                                                      const int64_t numberOfColumns,
                                                      const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      uint8_t* rgbaOut) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    if (isMapColoringValid(mapIndex) == false) {
        CiftiMappableDataFile* nonConstThis = const_cast<CiftiMappableDataFile*>(this);
        nonConstThis->updateScalarColoringForMap(mapIndex);
    }
    
    const int64_t mapRgbaCount = m_mapContent[mapIndex]->m_rgba.size();
    
    
    /*
     * RGBA size will be zero if no data has been loaded for a CIFTI
     * matrix type file (user clicking brainordinate).
     */
    if (mapRgbaCount <= 0) {
        return 0;
    }
    
    const uint8_t* mapRGBA = &m_mapContent[mapIndex]->m_rgba[0];
    
    CaretAssert(m_voxelIndicesToOffset);
    
    /*
     * Data values are only needed when a label volume
     * is being drawn so that we can determine if the
     * label is displayed.
     */
    std::vector<float> dataValues;
    const GiftiLabelTable* labelTable = (isMappedWithLabelTable()
                                         ? getMapLabelTable(mapIndex)
                                         : NULL);
    if (isMappedWithLabelTable()) {
        CaretAssert(labelTable);
        getMapData(mapIndex,
                   dataValues);
    }
    
    int64_t rowIJK[3] = { firstVoxelIJK[0], firstVoxelIJK[1], firstVoxelIJK[2] };
    uint8_t rgba[4] = { 0, 0, 0, 0 };
    int64_t rgbaOutIndex4 = 0;
    
    int64_t validVoxelCount = 0;
    
    for (int32_t iRow = 0; iRow < numberOfRows; iRow++) {
        int64_t ijk[3] = { rowIJK[0], rowIJK[1], rowIJK[2] };
        
        for (int32_t iCol = 0; iCol < numberOfColumns; iCol++) {
            rgba[3] = 0;
            
            const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(ijk[0], ijk[1], ijk[2]);
            if (dataOffset >= 0) {
                const int64_t dataOffset4 = dataOffset * 4;
                CaretAssert(dataOffset4 < mapRgbaCount);
                
                rgba[0] = mapRGBA[dataOffset4];
                rgba[1] = mapRGBA[dataOffset4+1];
                rgba[2] = mapRGBA[dataOffset4+2];
                
                /*
                 * A negative value for alpha indicates "do not draw".
                 * Since unsigned bytes do not have negative values,
                 * change the value to zero (which indicates "transparent").
                 */
                float alpha = mapRGBA[dataOffset4+3];
                if (alpha < 0.0) {
                    alpha = 0.0;
                }
                
                if (alpha > 0.0) {
                    if (labelTable != NULL) {
                        /*
                         * For label data, verify that the label is displayed.
                         * If NOT displayed, zero out the alpha value to
                         * prevent display of the data.
                         */
                        CaretAssertVectorIndex(dataValues, dataOffset);
                        const int32_t dataValue = dataValues[dataOffset];
                        const GiftiLabel* label = labelTable->getLabel(dataValue);
                        if (label != NULL) {
                            const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                            CaretAssert(item);
                            if (item->isSelected(displayGroup, tabIndex) == false) {
                                alpha = 0.0;
                            }
                        }
                    }
                    
                }
                
                if (alpha > 0.0) {
                    ++validVoxelCount;
                }
                rgba[3] = (alpha * 255.0);
            }
            
            rgbaOut[rgbaOutIndex4]   = rgba[0];
            rgbaOut[rgbaOutIndex4+1] = rgba[1];
            rgbaOut[rgbaOutIndex4+2] = rgba[2];
            rgbaOut[rgbaOutIndex4+3] = rgba[3];
            rgbaOutIndex4 += 4;
            
            if (rgba[3] > 0) {
                validVoxelCount++;
            }
            
            ijk[0] += columnStepIJK[0];
            ijk[1] += columnStepIJK[1];
            ijk[2] += columnStepIJK[2];
        }
        
        rowIJK[0] += rowStepIJK[0];
        rowIJK[1] += rowStepIJK[1];
        rowIJK[2] += rowStepIJK[2];
    }
    
    return validVoxelCount;
}

/**
 * Get the voxel colors for a sub slice in the map.
 *
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    The slice plane.
 * @param sliceIndex
 *    Index of the slice.
 * @param firstCornerVoxelIndex
 *    Indices of voxel for first corner of sub-slice (inclusive).
 * @param lastCornerVoxelIndex
 *    Indices of voxel for last corner of sub-slice (inclusive).
 * @param voxelCountIJK
 *    Voxel counts for each axis.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *    Output containing the rgba values (must have been allocated
 *    by caller to sufficient count of elements in the slice).
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
CiftiMappableDataFile::getVoxelColorsForSubSliceInMap(const int32_t mapIndex,
                                                      const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                      const int64_t sliceIndex,
                                                      const int64_t firstCornerVoxelIndex[3],
                                                      const int64_t lastCornerVoxelIndex[3],
                                                      const int64_t voxelCountIJK[3],
                                                      const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      uint8_t* rgbaOut) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    CaretAssertMessage((sliceIndex >= 0),
                       "Slice index is invalid.");
    if (sliceIndex < 0) {
        return 0;
    }
    
    if (isMapColoringValid(mapIndex) == false) {
        CiftiMappableDataFile* nonConstThis = const_cast<CiftiMappableDataFile*>(this);
        nonConstThis->updateScalarColoringForMap(mapIndex);
    }
    
    const int64_t iStart = firstCornerVoxelIndex[0];
    const int64_t jStart = firstCornerVoxelIndex[1];
    const int64_t kStart = firstCornerVoxelIndex[2];
    
    const int64_t iEnd = lastCornerVoxelIndex[0];
    const int64_t jEnd = lastCornerVoxelIndex[1];
    const int64_t kEnd = lastCornerVoxelIndex[2];
    
    const int64_t voxelCountI = voxelCountIJK[0];
    const int64_t voxelCountJ = voxelCountIJK[1];
    const int64_t voxelCountK = voxelCountIJK[2];
    
    int64_t dimI, dimJ, dimK, dimTime, dimComp;
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    int64_t voxelCount = 0;
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            voxelCount = voxelCountI * voxelCountJ;
            CaretAssert((sliceIndex < dimK));
            if (sliceIndex >= dimK) {
                return 0;
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            voxelCount = voxelCountI * voxelCountK;
            CaretAssert((sliceIndex < dimJ));
            if (sliceIndex >= dimJ) {
                return 0;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            voxelCount = voxelCountJ * voxelCountK;
            CaretAssert((sliceIndex < dimI));
            if (sliceIndex >= dimI) {
                return 0;
            }
            break;
    }
    
    if (voxelCount <= 0) {
        return 0;
    }
    const int64_t componentCount = voxelCount * 4;
    
    /*
     * Clear the slice rgba coloring.
     */
    for (int64_t i = 0; i < componentCount; i++) {
        rgbaOut[i] = 0;
    }
    
    const int64_t mapRgbaCount = m_mapContent[mapIndex]->m_rgba.size();
    
    
    /*
     * RGBA size will be zero if no data has been loaded for a CIFTI
     * matrix type file (user clicking brainordinate).
     */
    if (mapRgbaCount <= 0) {
        return 0;
    }
    
    const uint8_t* mapRGBA = &m_mapContent[mapIndex]->m_rgba[0];
    
    CaretAssert(m_voxelIndicesToOffset);
    
    /*
     * Data values are only needed when a label volume 
     * is being drawn so that we can determine if the
     * label is displayed.
     */
    std::vector<float> dataValues;
    const GiftiLabelTable* labelTable = (isMappedWithLabelTable()
                                         ? getMapLabelTable(mapIndex)
                                         : NULL);
    if (isMappedWithLabelTable()) {
        CaretAssert(labelTable);
        getMapData(mapIndex,
                   dataValues);
    }
    
    /*
     * Note that step indices may be positive or negative
     */
    const int64_t kStep = ((kEnd < kStart) ? -1 : 1);
    const int64_t jStep = ((jEnd < jStart) ? -1 : 1);
    const int64_t iStep = ((iEnd < iStart) ? -1 : 1);
    
    int64_t validVoxelCount = 0;
    
    /*
     * Set the rgba components for the slice.
     */
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        {
            int64_t rgbaOffset = 0;
            
            int64_t j = jStart;
            bool jLoopFlag = true;
            while (jLoopFlag) {
                
                int64_t i = iStart;
                bool iLoopFlag = true;
                while (iLoopFlag) {
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(i,
                                                                                           j,
                                                                                           sliceIndex);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                    
                    if (i == iEnd) {
                        iLoopFlag = false;
                    }
                    else {
                        i += iStep;
                    }
                    
                    rgbaOffset += 4;
                }
                
                if (j == jEnd) {
                    jLoopFlag = false;
                }
                else {
                    j += jStep;
                }
            }

        }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
        {
            int64_t rgbaOffset = 0;
            
            int64_t k = kStart;
            bool kLoopFlag = true;
            while (kLoopFlag) {
                
                int64_t i = iStart;
                bool iLoopFlag = true;
                while (iLoopFlag) {
            
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(i,
                                                                                           sliceIndex,
                                                                                           k);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                    
                    
                    if (i == iEnd) {
                        iLoopFlag = false;
                    }
                    else {
                        i += iStep;
                    }
                    
                    rgbaOffset += 4;
                }
                
                if (k == kEnd) {
                    kLoopFlag = false;
                }
                else {
                    k += kStep;
                }
            }
        }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
        {
            int64_t rgbaOffset = 0;
            
            int64_t k = kStart;
            bool kLoopFlag = true;
            while (kLoopFlag) {
                
                int64_t j = jStart;
                bool jLoopFlag = true;
                while (jLoopFlag) {
                    
                    
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(sliceIndex,
                                                                                           j,
                                                                                           k);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                    
                    if (j == jEnd) {
                        jLoopFlag = false;
                    }
                    else {
                        j += jStep;
                    }
                    
                    rgbaOffset += 4;
                }
                
                if (k == kEnd) {
                    kLoopFlag = false;
                }
                else {
                    k += kStep;
                }
            }
        }
            for (int64_t k = kStart; k <= kEnd; k++) {
                for (int64_t j = jStart; j < jEnd; j++) {
                    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(sliceIndex,
                                                                                           j,
                                                                                           k);
                    if (dataOffset >= 0) {
                        const int64_t dataOffset4 = dataOffset * 4;
                        CaretAssert(dataOffset4 < mapRgbaCount);
                        
                        const int64_t rgbaOffset = (((k - kStart) * voxelCountJ) + (j - jStart)) * 4;
                        CaretAssert(rgbaOffset < componentCount);
                        rgbaOut[rgbaOffset]   = mapRGBA[dataOffset4];
                        rgbaOut[rgbaOffset+1] = mapRGBA[dataOffset4+1];
                        rgbaOut[rgbaOffset+2] = mapRGBA[dataOffset4+2];
                        /*
                         * A negative value for alpha indicates "do not draw".
                         * Since unsigned bytes do not have negative values,
                         * change the value to zero (which indicates "transparent").
                         */
                        float alpha = mapRGBA[dataOffset4+3];
                        if (alpha < 0.0) {
                            alpha = 0.0;
                        }
                        
                        if (alpha > 0.0) {
                            if (labelTable != NULL) {
                                /*
                                 * For label data, verify that the label is displayed.
                                 * If NOT displayed, zero out the alpha value to
                                 * prevent display of the data.
                                 */
                                CaretAssertVectorIndex(dataValues, dataOffset);
                                const int32_t dataValue = dataValues[dataOffset];
                                const GiftiLabel* label = labelTable->getLabel(dataValue);
                                if (label != NULL) {
                                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                                    CaretAssert(item);
                                    if (item->isSelected(displayGroup, tabIndex) == false) {
                                        alpha = 0.0;
                                    }
                                }
                            }
                            
                        }
                        
                        if (alpha > 0.0) {
                            ++validVoxelCount;
                        }
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                }
            }
            break;
    }
    
    return validVoxelCount;
}

/**
 * Get the voxel coloring for the voxel at the given indices.
 * This method is for label data.  Accessing the actual voxel values is
 * needed for coloring labels.  But, one can only access the entire set
 * of values for a map.  Since this method is typically called many times
 * when coloring slices in ALL view, get the map data value before calling
 * this and then pass them in.
 *
 * This will work for non-label data.
 *
 * @param dataForMap
 *     Data for the map.
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param mapIndex
 *     Time/map index.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *     Output containing RGBA values for voxel at the given indices.
 */
void
CiftiMappableDataFile::getVoxelColorInMapForLabelData(const std::vector<float>& dataForMap,
                                                      const int64_t indexIn1,
                                                      const int64_t indexIn2,
                                                      const int64_t indexIn3,
                                                      const int64_t mapIndex,
                                                      const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      uint8_t rgbaOut[4]) const
{
    getVoxelColorInMap(indexIn1,
                       indexIn2,
                       indexIn3,
                       mapIndex,
                       displayGroup,
                       tabIndex,
                       rgbaOut);
    

    if (isMappedWithLabelTable()) {
        if (rgbaOut[3] > 0.0) {
            const GiftiLabelTable* labelTable = getMapLabelTable(mapIndex);
            CaretAssert(labelTable);
            
            const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(indexIn1,
                                                                                   indexIn2,
                                                                                   indexIn3);
            if (dataOffset >= 0) {
                /*
                 * If the label is NOT selected for the given display
                 * group and tab, inhibit its display by setting the
                 * alpha component to zero.
                 */
                CaretAssertVectorIndex(dataForMap, dataOffset);
                const int32_t labelKey = dataForMap[dataOffset];
                const GiftiLabel* label = labelTable->getLabel(labelKey);
                if (label != NULL) {
                    const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                    if (item != NULL) {
                        if (item->isSelected(displayGroup, tabIndex) == false) {
                            rgbaOut[3] = 0.0;
                        }
                    }
                }
            }
        }
    }
}

/**
 * Get the voxel coloring for the voxel at the given indices.
 *
 * @see getVoxelColorInMapForLabelData
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param mapIndex
 *     Time/map index.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *     Output containing RGBA values for voxel at the given indices.
 */
void
CiftiMappableDataFile::getVoxelColorInMap(const int64_t indexIn1,
                                          const int64_t indexIn2,
                                          const int64_t indexIn3,
                                          const int64_t mapIndex,
                                          const DisplayGroupEnum::Enum /*displayGroup*/,
                                          const int32_t /*tabIndex*/,
                                          uint8_t rgbaOut[4]) const
{
    rgbaOut[0] = 0;
    rgbaOut[1] = 0;
    rgbaOut[2] = 0;
    rgbaOut[3] = 0;
    
    if ( ! isMapColoringValid(mapIndex)) {
        CiftiMappableDataFile* nonConstThis = const_cast<CiftiMappableDataFile*>(this);
        nonConstThis->updateScalarColoringForMap(mapIndex);
    }
    
    CaretAssert(m_voxelIndicesToOffset);
    
    const int64_t mapRgbaCount = m_mapContent[mapIndex]->m_rgba.size();
    if (mapRgbaCount <= 0) {
        return;
    }
    
    const uint8_t* mapRGBA = &m_mapContent[mapIndex]->m_rgba[0];
    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(indexIn1,
                                                                           indexIn2,
                                                                           indexIn3);
    if (dataOffset >= 0) {
        const int64_t dataOffset4 = dataOffset * 4;
        CaretAssert(dataOffset4 < mapRgbaCount);
        
        rgbaOut[0] = mapRGBA[dataOffset4];
        rgbaOut[1] = mapRGBA[dataOffset4+1];
        rgbaOut[2] = mapRGBA[dataOffset4+2];
        rgbaOut[3] = mapRGBA[dataOffset4+3];
    }
}

/**
 * Get the brainordinate from the given row.
 *
 * @param rowIndex
 *     Index of the row.
 * @param surfaceStructureOut
 *     Will contain structure of surface if row is a surface node.
 * @param surfaceNodeIndexOut
 *     Will contain index of surface node if row is a surface node.
 * @param surfaceNumberOfNodesOut
 *     Will contain surfaces number of nodes if row is a surface node.
 * @param surfaceNodeValidOut
 *     Will be true upon exit if the row corresponded to a surface node.
 * @param voxelIJKOut
 *     Will contain the voxel's IJK indices if row is a surface node.
 * @param voxelXYZOut
 *     Will contain the voxel's XYZ coordinate if row is a surface node.
 * @param voxelValidOut
 *     Will be true upon exit if the row corresponded to a surface node.
 * @throw DataFileException
 *     If the rows are not for brainordinates or the row index is invalid.
 */
void
CiftiMappableDataFile::getBrainordinateFromRowIndex(const int64_t rowIndex,
                                                    StructureEnum::Enum& surfaceStructureOut,
                                                    int32_t& surfaceNodeIndexOut,
                                                    int32_t& surfaceNumberOfNodesOut,
                                                    bool& surfaceNodeValidOut,
                                                    int64_t voxelIJKOut[3],
                                                    float voxelXYZOut[3],
                                                    bool& voxelValidOut) const
{
    surfaceNodeValidOut = false;
    voxelValidOut       = false;
    
    if (m_ciftiFile == NULL) {
        return;
    }
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    if (ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) {
        throw DataFileException(getFileName(),
                                "File does not have brainordinate data for rows.");
        return;
    }
    
    const CiftiBrainModelsMap& brainMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    
    if ((rowIndex < 0)
        || (rowIndex >= m_ciftiFile->getNumberOfRows())) {
        throw DataFileException(getFileName(),
                                "Row index "
                                + AString::number(rowIndex)
                                + " is out of range [0, "
                                + AString::number(m_ciftiFile->getNumberOfRows() - 1)
                                + "]");
    }
    
    const CiftiBrainModelsMap::IndexInfo indexInfo = brainMap.getInfoForIndex(rowIndex);
    
    switch (indexInfo.m_type) {
        case CiftiBrainModelsMap::SURFACE:
            surfaceStructureOut     = indexInfo.m_structure;
            surfaceNodeIndexOut     = indexInfo.m_surfaceNode;
            surfaceNumberOfNodesOut = brainMap.getSurfaceNumberOfNodes(surfaceStructureOut);
            surfaceNodeValidOut     = true;
            break;
        case CiftiBrainModelsMap::VOXELS:
            voxelIJKOut[0] = indexInfo.m_ijk[0];
            voxelIJKOut[1] = indexInfo.m_ijk[1];
            voxelIJKOut[2] = indexInfo.m_ijk[2];
            indexToSpace(voxelIJKOut,
                         voxelXYZOut);
            voxelValidOut = true;
            break;
    }
}

/**
 * Get the unique label keys in the given map.
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Keys used by the map.
 */
std::vector<int32_t>
CiftiMappableDataFile::getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);

    std::vector<float> data;
    getMapData(mapIndex,
               data);
    std::set<int32_t> uniqueKeys;
    const int64_t numItems = static_cast<int64_t>(data.size());
    if (numItems > 0) {
        const float* dataPtr = &data[0];
        for (int64_t i = 0; i < numItems; i++) {
            const int32_t key = static_cast<int32_t>(dataPtr[i]);
            uniqueKeys.insert(key);
        }
    }

    std::vector<int32_t> keyVector;
    keyVector.insert(keyVector.end(),
                     uniqueKeys.begin(),
                     uniqueKeys.end());
    return keyVector;
}

/**
 * @return The class and name hierarchy.
 */
GroupAndNameHierarchyModel*
CiftiMappableDataFile::getGroupAndNameHierarchyModel()
{
    CaretAssert(m_classNameHierarchy);

    m_classNameHierarchy->update(this,
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * @return The class and name hierarchy.
 */
const GroupAndNameHierarchyModel*
CiftiMappableDataFile::getGroupAndNameHierarchyModel() const
{
    CaretAssert(m_classNameHierarchy);
    
    m_classNameHierarchy->update(const_cast<CiftiMappableDataFile*>(this),
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * Validate keys and labels in the file.
 */
void
CiftiMappableDataFile::validateKeysAndLabels() const
{
    /*
     * Skip if not label file
     */
    if (isMappedWithLabelTable() == false) {
        return;
    }
    
    /*
     * Skip if logging is not fine or less.
     */
    if (CaretLogger::getLogger()->isFine() == false) {
        return;
    }
    
    AString messages;
    
    /*
     * Find the label keys that are in the data
     */
    std::set<int32_t> dataKeys;
    const int32_t numMaps  = getNumberOfMaps();
    for (int32_t jMap = 0; jMap < numMaps; jMap++) {
        AString mapMessage;
        
        std::vector<float> data;
        getMapData(jMap,
                   data);
        const int64_t numItems = static_cast<int64_t>(data.size());
        for (int32_t i = 0; i < numItems; i++) {
            const int32_t key = static_cast<int32_t>(data[i]);
            dataKeys.insert(key);
        }
        
        /*
         * Find any keys that are not in the label table
         */
        const GiftiLabelTable* labelTable = getMapLabelTable(jMap);
        std::set<int32_t> missingLabelKeys;
        for (std::set<int32_t>::iterator dataKeyIter = dataKeys.begin();
             dataKeyIter != dataKeys.end();
             dataKeyIter++) {
            const int32_t dataKey = *dataKeyIter;
            
            const GiftiLabel* label = labelTable->getLabel(dataKey);
            if (label == NULL) {
                missingLabelKeys.insert(dataKey);
            }
        }
        
        if (missingLabelKeys.empty() == false) {
            for (std::set<int32_t>::iterator missingKeyIter = missingLabelKeys.begin();
                 missingKeyIter != missingLabelKeys.end();
                 missingKeyIter++) {
                const int32_t missingKey = *missingKeyIter;
                
                mapMessage.appendWithNewLine("        Missing Label for Key: "
                                             + AString::number(missingKey));
            }
        }
        
        /*
         * Find any label table names that are not used
         */
        std::map<int32_t, AString> labelTableKeysAndNames;
        labelTable->getKeysAndNames(labelTableKeysAndNames);
        for (std::map<int32_t, AString>::const_iterator ltIter = labelTableKeysAndNames.begin();
             ltIter != labelTableKeysAndNames.end();
             ltIter++) {
            const int32_t ltKey = ltIter->first;
            if (std::find(dataKeys.begin(),
                          dataKeys.end(),
                          ltKey) == dataKeys.end()) {
                mapMessage.appendWithNewLine("        Label Not Used Key="
                                             + AString::number(ltKey)
                                             + ": "
                                             + ltIter->second);
            }
        }
        
        if (mapMessage.isEmpty() == false) {
            mapMessage = ("    Map: "
                          + getMapName(jMap)
                          + ":\n"
                          + mapMessage
                          + "\n"
                          + labelTable->toFormattedString("        "));
            messages += mapMessage;
        }
    }
    
    
    AString msg = ("File: "
                   + getFileName()
                   + "\n"
                   + messages);
    CaretLogFine(msg);
}

/**
 * Get connectivity value for a surface's node.  When the data is mapped
 * to parcels, the numerical value will not be valid.  
 *
 * @param mapIndex
 *     Index of the map.
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node
 * @param numberOfNodes
 *     Number of nodes in the surface.
 * @param numericalValueOut
 *     Numerical value out.
 * @param numericalValueOutValid
 *     Output that indicates the numerical value output is valid.
 *     For label data, this value will be the lable key.
 * @param textValueOut
 *     Text containing node' value will always be valid if the method 
 *     returns true.  For parcel data, this will contain the name of the
 *     parcel.  For label data, this will contain the name of the label.
 *     For numerical data, this will contain the text representation
 *     of the numerical value.
 * @return
 *    True if the text value is valid.  The numerical value may or may not
 *    also be valid.
 */
bool
CiftiMappableDataFile::getMapSurfaceNodeValue(const int32_t mapIndex,
                                              const StructureEnum::Enum structure,
                                              const int nodeIndex,
                                              const int32_t numberOfNodes,
                                              float& numericalValueOut,
                                              bool& numericalValueOutValid,
                                              AString& textValueOut) const
{
    numericalValueOut = 0.0;
    numericalValueOutValid = false;
    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);

    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
    
    switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
        case CiftiMappingType::BRAIN_MODELS:
        {
            const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
                const int64_t dataIndex = map.getIndexForNode(nodeIndex,
                                                              structure);
                if (dataIndex >= 0) {
                    std::vector<float> mapData;
                    getMapData(mapIndex, mapData);
                    
                    if (dataIndex < static_cast<int64_t>(mapData.size())) {
                        numericalValueOut = mapData[dataIndex];
                        numericalValueOutValid = true;
                        
                        if (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML) == CiftiMappingType::LABELS) {
                            const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                            const int32_t labelKey = static_cast<int32_t>(numericalValueOut);
                            const GiftiLabel* gl = glt->getLabel(labelKey);
                            if (gl != NULL) {
                                textValueOut += gl->getName();
                            }
                            else {
                                textValueOut += ("InvalidLabelKey="
                                                 + AString::number(labelKey));
                            }
                        }
                        else {
                            textValueOut = AString::number(numericalValueOut, 'f');
                        }
                        return true;
                    }
                }
            }
        }
            break;
        case CiftiMappingType::LABELS:
            CaretAssertMessage(0, "Mapping type should never be LABELS");
            break;
        case CiftiMappingType::PARCELS:
        {
            int64_t parcelIndex = -1;
            const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
                const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
                parcelIndex = map.getIndexForNode(nodeIndex,
                                                  structure);
                if ((parcelIndex >= 0)
                    && (parcelIndex < static_cast<int64_t>(parcels.size()))) {
                    textValueOut = parcels[parcelIndex].m_name;
                    
                    std::vector<float> mapData;
                    getMapData(mapIndex, mapData);
                }
            }
            
            if (parcelIndex >= 0) {
                int64_t itemIndex = -1;
                switch (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML)) {
                    case CiftiMappingType::BRAIN_MODELS:
                    {
                        const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataReadingDirectionForCiftiXML);
                        if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
                            itemIndex = map.getIndexForNode(nodeIndex,
                                                            structure);
                        }
                    }
                        break;
                    case CiftiMappingType::LABELS:
                        break;
                    case CiftiMappingType::PARCELS:
                        itemIndex = mapIndex;
                        break;
                    case CiftiMappingType::SCALARS:
                        itemIndex = mapIndex;
                        break;
                    case CiftiMappingType::SERIES:
                        itemIndex = mapIndex;
                        break;
                }
                if (itemIndex >= 0) {
                    const int64_t numRows = m_ciftiFile->getNumberOfRows();
                    const int64_t numCols = m_ciftiFile->getNumberOfColumns();
                    
                    switch (m_dataReadingDirectionForCiftiXML) {
                        case CiftiXML::ALONG_COLUMN:
                        {
                            std::vector<float> data;
                            data.resize(numRows);
                            CaretAssert(parcelIndex < numCols);
                            m_ciftiFile->getColumn(&data[0], parcelIndex);
                            CaretAssertVectorIndex(data, itemIndex);
                            textValueOut += (" " + AString::number(data[itemIndex]));
                        }
                            break;
                        case CiftiXML::ALONG_ROW:
                        {
                            std::vector<float> data;
                            data.resize(numCols);
                            CaretAssert(parcelIndex < numRows);
                            m_ciftiFile->getRow(&data[0], parcelIndex);
                            CaretAssertVectorIndex(data, itemIndex);
                            textValueOut += (" " + AString::number(data[itemIndex]));
                        }
                            break;
                    }
                
                }
            }
        }
            return true;
            break;
        case CiftiMappingType::SCALARS:
            CaretAssertMessage(0, "Mapping type should never be SCALARS");
            break;
        case CiftiMappingType::SERIES:
            CaretAssertMessage(0, "Mapping type should never be SERIES");
            break;
    }
    
    return false;
}

/**
 * Get connectivity value for a surface's node.  When the data is mapped
 * to parcels, the numerical value will not be valid.
 *
 * @param mapIndices
 *     Index of the map.
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node
 * @param numberOfNodes
 *     Number of nodes in the surface.
 * @param numericalValuesOut
 *     Numerical values out for all map indices
 * @param numericalValuesOutValid
 *     Output that indicates the numerical value output is valid.
 *     For label data, this value will be the lable key.
 * @param textValueOut
 *     Text containing node' value will always be valid if the method
 *     returns true.  For parcel data, this will contain the name of the
 *     parcel.  For label data, this will contain the name of the label.
 *     For numerical data, this will contain the text representation
 *     of the numerical value.
 * @return
 *    True if the text value is valid.  The numerical values may or may not
 *    also be valid.
 */
bool
CiftiMappableDataFile::getMapSurfaceNodeValues(const std::vector<int32_t>& mapIndices,
                                               const StructureEnum::Enum structure,
                                               const int nodeIndex,
                                               const int32_t numberOfNodes,
                                               std::vector<float>& numericalValuesOut,
                                               std::vector<bool>& numericalValuesOutValid,
                                               AString& textValueOut) const
{
    numericalValuesOut.clear();
    numericalValuesOutValid.clear();
    textValueOut.clear();
    
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
    
    switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
        case CiftiMappingType::BRAIN_MODELS:
        {
            const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
                const int64_t dataIndex = map.getIndexForNode(nodeIndex,
                                                              structure);
                if (dataIndex >= 0) {
                    for (std::vector<int32_t>::const_iterator mapIter = mapIndices.begin();
                         mapIter != mapIndices.end();
                         mapIter++) {
                        const int32_t mapIndex = *mapIter;
                        
                        std::vector<float> mapData;
                        CaretAssertVectorIndex(m_mapContent, mapIndex);
                        getMapData(mapIndex, mapData);
                        
                        if (dataIndex < static_cast<int64_t>(mapData.size())) {
                            const float value = mapData[dataIndex];
                            numericalValuesOut.push_back(mapData[dataIndex]);
                            numericalValuesOutValid.push_back(true);
                            
                            if (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML) == CiftiMappingType::LABELS) {
                                const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                                const int32_t labelKey = static_cast<int32_t>(value);
                                const GiftiLabel* gl = glt->getLabel(labelKey);
                                if (gl != NULL) {
                                    textValueOut += (" " + gl->getName());
                                }
                                else {
                                    textValueOut += (" InvalidLabelKey="
                                                     + AString::number(labelKey));
                                }
                            }
                            else {
                                textValueOut += (" " + AString::number(value, 'f'));
                            }
                        }
                    }
                }
            }
        }
            break;
        case CiftiMappingType::LABELS:
            CaretAssertMessage(0, "Mapping type should never be LABELS");
            break;
        case CiftiMappingType::PARCELS:
        {
            int64_t parcelIndex = -1;
            const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
                const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
                parcelIndex = map.getIndexForNode(nodeIndex,
                                                  structure);
                if ((parcelIndex >= 0)
                    && (parcelIndex < static_cast<int64_t>(parcels.size()))) {
                    textValueOut = parcels[parcelIndex].m_name;
                }
            }
            
            for (std::vector<int32_t>::const_iterator mapIter = mapIndices.begin();
                 mapIter != mapIndices.end();
                 mapIter++) {
                const int32_t mapIndex = *mapIter;
                
                if (parcelIndex >= 0) {
                    int64_t itemIndex = -1;
                    switch (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML)) {
                        case CiftiMappingType::BRAIN_MODELS:
                        {
                            const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataReadingDirectionForCiftiXML);
                            if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
                                itemIndex = map.getIndexForNode(nodeIndex,
                                                                structure);
                            }
                        }
                            break;
                        case CiftiMappingType::LABELS:
                            break;
                        case CiftiMappingType::PARCELS:
                            itemIndex = mapIndex;
                            break;
                        case CiftiMappingType::SCALARS:
                            itemIndex = mapIndex;
                            break;
                        case CiftiMappingType::SERIES:
                            itemIndex = mapIndex;
                            break;
                    }
                    if (itemIndex >= 0) {
                        const int64_t numRows = m_ciftiFile->getNumberOfRows();
                        const int64_t numCols = m_ciftiFile->getNumberOfColumns();
                        
                        switch (m_dataReadingDirectionForCiftiXML) {
                            case CiftiXML::ALONG_COLUMN:
                            {
                                std::vector<float> data;
                                data.resize(numRows);
                                CaretAssert(parcelIndex < numCols);
                                m_ciftiFile->getColumn(&data[0], parcelIndex);
                                CaretAssertVectorIndex(data, itemIndex);
                                textValueOut += (" " + AString::number(data[itemIndex]));
                            }
                                break;
                            case CiftiXML::ALONG_ROW:
                            {
                                std::vector<float> data;
                                data.resize(numCols);
                                CaretAssert(parcelIndex < numRows);
                                m_ciftiFile->getRow(&data[0], parcelIndex);
                                CaretAssertVectorIndex(data, itemIndex);
                                textValueOut += (" " + AString::number(data[itemIndex]));
                            }
                                break;
                        }
                        
                    }
                }
            }
        }
            break;
        case CiftiMappingType::SCALARS:
            CaretAssertMessage(0, "Mapping type should never be SCALARS");
            break;
        case CiftiMappingType::SERIES:
            CaretAssertMessage(0, "Mapping type should never be SERIES");
            break;
    }

    if (textValueOut.isEmpty()) {
        return false;
    }
    
    /*
     * Output text is valid
     */
    return true;
}

/**
 * Get Parcel Label File value for a surface's node.  When the data is mapped
 * to parcels, the numerical value will not be valid.
 *
 * @param mapIndex
 *     Index of the map.
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node
 * @param numberOfNodes
 *     Number of nodes in the surface.
 * @param numericalValueOut
 *     Numerical value out.
 * @param numericalValueOutValid
 *     Output that indicates the numerical value output is valid.
 *     For label data, this value will be the lable key.
 * @param textValueOut
 *     Text containing node' value will always be valid if the method
 *     returns true.  For parcel data, this will contain the name of the
 *     parcel.  For label data, this will contain the name of the label.
 *     For numerical data, this will contain the text representation
 *     of the numerical value.
 * @return
 *    True if the text value is valid.  The numerical value may or may not
 *    also be valid.
 */
bool
CiftiMappableDataFile::getParcelLabelMapSurfaceNodeValue(const int32_t mapIndex,
                                              const StructureEnum::Enum structure,
                                              const int nodeIndex,
                                              const int32_t numberOfNodes,
                                              float& numericalValueOut,
                                              bool& numericalValueOutValid,
                                              AString& textValueOut) const
{
    numericalValueOut = 0.0;
    numericalValueOutValid = false;
    textValueOut = "";
    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    AString areaName = "";
    
    CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));

    const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
    if (map.getSurfaceNumberOfNodes(structure) == numberOfNodes) {
        const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
        const int64_t parcelIndex = map.getIndexForNode(nodeIndex,
                                                        structure);
        if ((parcelIndex >= 0)
            && (parcelIndex < static_cast<int64_t>(parcels.size()))) {
            areaName = parcels[parcelIndex].m_name;

            const AString networkName = getMapName(mapIndex);
            
            textValueOut = ("Area: "
                            + areaName
                            + ", Network: "
                            + networkName);
            return true;
        }
    }
    

    return false;
}

/**
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param structure
 *    Structure of the surface.
 * @param nodeIndex
 *    Index of the node.
 * @param numberOfNodes
 *    Number of nodes in the surface.
 * @param textOut
 *    Output containing identification information.
 */
bool
CiftiMappableDataFile::getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                              const StructureEnum::Enum structure,
                                                              const int nodeIndex,
                                                              const int32_t numberOfNodes,
                                                              AString& textOut) const
{
    CaretAssert(m_ciftiFile);
    if (mapIndices.empty()) {
        return false;
    }

    bool useMapData = false;
    bool useParcelLabelMapData = false;
    bool useSeriesData = false;
    switch (getDataFileType()) {
        case DataFileTypeEnum::ANNOTATION:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::BORDER:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            useSeriesData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC:
            useSeriesData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            useSeriesData = true;
           break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            useSeriesData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            useParcelLabelMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            useMapData = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::FOCI:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::IMAGE:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::LABEL:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::METRIC:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::PALETTE:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::RGBA:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::SCENE:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::SPECIFICATION:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::SURFACE:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::UNKNOWN:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::VOLUME:
            CaretAssert(0);
            break;
    }
    const int32_t numberOfMapIndices = static_cast<int32_t>(mapIndices.size());
    
    textOut = "";
    
    bool validID = false;
    
    if (useMapData) {
        std::vector<float> numericalValues;
        std::vector<bool>  numericalValuesValid;
        AString textValue;
        if (getMapSurfaceNodeValues(mapIndices,
                                    structure,
                                    nodeIndex,
                                    numberOfNodes,
                                    numericalValues,
                                    numericalValuesValid,
                                    textValue)) {
            textOut += textValue;
            textOut += " ";
            validID = true;
        }
    }
    else if (useSeriesData) {
        /*
         * Use series data which contains values for node from all maps.
         */
        std::vector<float> seriesData;
        if (getSeriesDataForSurfaceNode(structure,
                                        nodeIndex,
                                        seriesData)) {
            for (int32_t i = 0; i < numberOfMapIndices; i++) {
                const int32_t mapIndex = mapIndices[i];
                CaretAssertVectorIndex(seriesData, mapIndex);

                const float value = seriesData[mapIndex];
                if (isMappedWithLabelTable()) {
                    const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                    const int32_t labelKey = static_cast<int32_t>(value);
                    const GiftiLabel* gl = glt->getLabel(labelKey);
                    if (gl != NULL) {
                        textOut += gl->getName();
                    }
                    else {
                        textOut += ("InvalidLabelKey="
                                    + AString::number(value));
                    }
                    validID = true;
                }
                else if (isMappedWithPalette()) {
                    textOut += AString::number(value);
                    validID = true;
                }
                else {
                    CaretAssert(0);
                }
                
                textOut += " ";
            }
        }
    }
    else if (useParcelLabelMapData) {
        for (int32_t i = 0; i < numberOfMapIndices; i++) {
            const int32_t mapIndex = mapIndices[i];
            
            float numericalValue;
            AString textValue;
            bool numericalValueValid;
            if (getParcelLabelMapSurfaceNodeValue(mapIndex,
                                                  structure,
                                                  nodeIndex,
                                                  numberOfNodes,
                                                  numericalValue,
                                                  numericalValueValid,
                                                  textValue)) {
                textOut += textValue;
                textOut += " ";
                validID = true;
            }
        }
    }
    
    return validID;
}

/**
 * Get the series data (one data value from each map) for a surface node.
 *
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node.
 * @param seriesDataOut
 *     Series data for given node.
 * @return
 *     True if output data is valid, else false.
 */
bool
CiftiMappableDataFile::getSeriesDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t nodeIndex,
                                         std::vector<float>& seriesDataOut) const
{
    CaretAssert(m_ciftiFile);

    bool valid = false;
    
    switch (m_dataMappingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            seriesDataOut.resize(m_ciftiFile->getNumberOfRows());
            valid = m_ciftiFile->getColumnFromNode(&seriesDataOut[0],
                                                nodeIndex,
                                                structure);
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            seriesDataOut.resize(m_ciftiFile->getNumberOfColumns());
            valid = m_ciftiFile->getRowFromNode(&seriesDataOut[0],
                                                nodeIndex,
                                                structure);
            break;
    }

    return valid;
}

/**
 * Get the series data (oone data value for each map) for a voxel at the
 * given coordinate.
 *
 * @param xyz
 *     Coordinate of the voxel.
 * @param seriesDataOut
 *     Series data for the given voxel.
 * @return
 *     True if output data is valid, else false.
 */
bool
CiftiMappableDataFile::getSeriesDataForVoxelAtCoordinate(const float xyz[3],
                                                         std::vector<float>& seriesDataOut) const
{
    CaretAssert(m_ciftiFile);
    
    bool valid = false;
    
    switch (m_dataMappingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            seriesDataOut.resize(m_ciftiFile->getNumberOfRows());
            valid = m_ciftiFile->getColumnFromVoxelCoordinate(&seriesDataOut[0],
                                                              xyz);
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            seriesDataOut.resize(m_ciftiFile->getNumberOfColumns());
            valid = m_ciftiFile->getRowFromVoxelCoordinate(&seriesDataOut[0],
                                                           xyz);
            break;
    }
    
    return valid;
}

/**
 * Get the node coloring for the surface.
 * @param mapIndex
 *    Index of the map.
 * @param structure
 *    Surface structure nodes are colored.
 * @param surfaceRGBAOut
 *    Filled with RGBA coloring for the surface's nodes.
 *    Contains numberOfNodes * 4 elements.
 * @param dataValuesOut
 *    Data values for the nodes (elements are valid when the alpha value in
 *    the RGBA colors is valid (greater than zero).
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @return
 *    True if coloring is valid, else false.
 */
bool
CiftiMappableDataFile::getMapSurfaceNodeColoring(const int32_t mapIndex,
                                                  const StructureEnum::Enum structure,
                                                  float* surfaceRGBAOut,
                                                  float* dataValuesOut,
                                                  const int32_t surfaceNumberOfNodes)
{
    CaretAssert(m_ciftiFile);    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    const int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
    if (numCiftiNodes != surfaceNumberOfNodes) {
        return false;
    }
    
    std::vector<float> mapData;
    getMapData(mapIndex,
               mapData);
    
    /*
     * Map data may be empty for connectivity matrix files with no rows loaded.
     */
    if (mapData.empty()) {
        return false;
    }
    
    std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMap;
    switch (m_dataMappingAccessMethod) {
        case DATA_ACCESS_METHOD_INVALID:
            CaretAssert(0);
            break;
        case DATA_ACCESS_NONE:
            break;
        case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            m_ciftiFile->getSurfaceMapForRows(surfaceMap,
                                              structure);
            break;
        case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            m_ciftiFile->getSurfaceMapForColumns(surfaceMap,
                                              structure);
            break;
    }
    
    const MapContent* mc = m_mapContent[mapIndex];
    
    /*
     * May need to update map coloring
     */
    if ( ! mc->m_rgbaValid) {
        updateScalarColoringForMap(mapIndex);
    }
    
    std::vector<int64_t> dataIndicesForNodes;

    bool validColorsFlag = false;
    if (getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                        surfaceNumberOfNodes,
                                                        dataIndicesForNodes)) {
        for (int64_t iNode = 0; iNode < surfaceNumberOfNodes; iNode++) {
            CaretAssertVectorIndex(dataIndicesForNodes,
                                   iNode);
            
            const int64_t dataIndex = dataIndicesForNodes[iNode];
            
            const int64_t node4 = iNode * 4;
            CaretAssertArrayIndex(surfaceRGBA, (surfaceNumberOfNodes * 4), node4);
            
            if (dataIndex >= 0) {
                CaretAssert(dataIndex < mc->m_dataCount);
                
                const int64_t data4 = dataIndex * 4;
                CaretAssertArrayIndex(this->dataRGBA, (mc->m_dataCount * 4), data4);
                
                surfaceRGBAOut[node4]   = mc->m_rgba[data4] / 255.0;
                surfaceRGBAOut[node4+1] = mc->m_rgba[data4+1] / 255.0;
                surfaceRGBAOut[node4+2] = mc->m_rgba[data4+2] / 255.0;
                surfaceRGBAOut[node4+3] = mc->m_rgba[data4+3] / 255.0;
                
                dataValuesOut[iNode] = mapData[dataIndex];
                
                validColorsFlag = true;
            }
            else {
                surfaceRGBAOut[node4]   =  0.0;
                surfaceRGBAOut[node4+1] =  0.0;
                surfaceRGBAOut[node4+2] =  0.0;
                surfaceRGBAOut[node4+3] = -1.0;
                
                dataValuesOut[iNode] = 0.0;
            }
        }
    }

   
    
    
    return validColorsFlag;
}

/**
 * Get the data indices corresponding to all nodes in the given surface.
 * 
 * @param structure
 *     Surface's structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in the surface.
 * @param dataIndicesForNodes
 *     Will containg "surfaceNumberOfNodes" element where the values are
 *     indices into the CIFTI data.
 * @return True if valid, else false.
 */
bool
CiftiMappableDataFile::getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                                          const int64_t surfaceNumberOfNodes,
                                                                          std::vector<int64_t>& dataIndicesForNodes) const
{
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
    
    switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
        case CiftiMappingType::BRAIN_MODELS:
        {
            const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.getSurfaceNumberOfNodes(structure) == surfaceNumberOfNodes) {
                const std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMap = map.getSurfaceMap(structure);
                
                dataIndicesForNodes.resize(surfaceNumberOfNodes);
                std::fill(dataIndicesForNodes.begin(),
                          dataIndicesForNodes.end(),
                          -1);
                for (std::vector<CiftiBrainModelsMap::SurfaceMap>::const_iterator iter = surfaceMap.begin();
                     iter != surfaceMap.end();
                     iter++) {
                    const CiftiBrainModelsMap::SurfaceMap& nodeMap = *iter;
                    CaretAssertVectorIndex(dataIndicesForNodes,
                                           nodeMap.m_surfaceNode);
                    dataIndicesForNodes[nodeMap.m_surfaceNode] = nodeMap.m_ciftiIndex;
                }
                
                return true;
            }
        }
            break;
        case CiftiMappingType::LABELS:
            CaretAssertMessage(0, "Mapping type should never be LABELS");
            break;
        case CiftiMappingType::PARCELS:
        {
            const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.getSurfaceNumberOfNodes(structure) == surfaceNumberOfNodes) {
                dataIndicesForNodes.resize(surfaceNumberOfNodes);
                for (int64_t i = 0; i < surfaceNumberOfNodes; i++) {
                    dataIndicesForNodes[i] = map.getIndexForNode(i,
                                                                 structure);
                }
                
                return true;
            }
        }
            break;
        case CiftiMappingType::SCALARS:
            CaretAssertMessage(0, "Mapping type should never be SCALARS");
            break;
        case CiftiMappingType::SERIES:
            CaretAssertMessage(0, "Mapping type should never be SERIES");
            break;
    }
    
    return false;
}

/**
 * Get connectivity value for a voxel.  When the data is mapped
 * to parcels, the numerical value will not be valid.
 *
 * @param mapIndex
 *     Index of the map.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param numericalValueOut
 *     Numerical value out.
 * @param numericalValueOutValid
 *     Output that indicates the numerical value output is valid.
 *     For label data, this value will be the label key.
 * @param textValueOut
 *     Text containing node' value will always be valid if the method
 *     returns true.  For parcel data, this will contain the name of the
 *     parcel.  For label data, this will contain the name of the label.
 *     For numerical data, this will contain the text representation
 *     of the numerical value.
 * @return
 *    True if the text value is valid.  The numerical value may or may not
 *    also be valid.
 */
bool
CiftiMappableDataFile::getMapVolumeVoxelValue(const int32_t mapIndex,
                                              const float xyz[3],
                                              int64_t ijkOut[3],
                                              float& numericalValueOut,
                                              bool& numericalValueOutValid,
                                              AString& textValueOut) const
{
    textValueOut = "";
    numericalValueOutValid = false;
    
    CaretAssert(m_ciftiFile);
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    int64_t ijk[3];
    enclosingVoxel(xyz[0],
                   xyz[1],
                   xyz[2],
                   ijk[0],
                   ijk[1],
                   ijk[2]);
    if (indexValid(ijk[0],
                   ijk[1],
                   ijk[2])) {
        /*
         * Only set the IJK if the index is valid.
         */
        ijkOut[0] = ijk[0];
        ijkOut[1] = ijk[1];
        ijkOut[2] = ijk[2];
        const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(ijk[0],
                                                                               ijk[1],
                                                                               ijk[2]);
        if (dataOffset >= 0) {
            CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                        || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
            
            const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
            
            const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(this);
            if (parcelLabelFile != NULL) {
                const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
                const int64_t parcelMapIndex = map.getIndexForVoxel(ijk);
                const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
                AString areaName = "";
                if ((parcelMapIndex >= 0)
                    && (parcelMapIndex < static_cast<int64_t>(parcels.size()))) {
                    CaretAssertVectorIndex(parcels,
                                           parcelMapIndex);
                    areaName = parcels[parcelMapIndex].m_name;
                }
                
                const AString networkName = getMapName(mapIndex);
                
                textValueOut = ("Area: "
                                + areaName
                                + ", Network: "
                                + networkName);
                return true;
            
            }
            else {
                switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
                    case CiftiMappingType::BRAIN_MODELS:
                    {
                        /*
                         * Note: For a Dense connectivity file, it may not have
                         * data loaded since data is loaded upon demand.
                         */
                        std::vector<float> mapData;
                        getMapData(mapIndex,
                                   mapData);
                        if ( ! mapData.empty()) {
                            CaretAssertVectorIndex(mapData,
                                                   dataOffset);
                            numericalValueOut = mapData[dataOffset];
                            
                            if (isMappedWithLabelTable()) {
                                textValueOut = "Invalid Label Index";
                                
                                const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                                const int32_t labelKey = static_cast<int32_t>(numericalValueOut);
                                const GiftiLabel* gl = glt->getLabel(labelKey);
                                if (gl != NULL) {
                                    textValueOut = gl->getName();
                                }
                                else {
                                    textValueOut += ("InvalidLabelKey="
                                                     + AString::number(labelKey));
                                }
                                numericalValueOutValid = true;
                            }
                            else if (isMappedWithPalette()) {
                                numericalValueOutValid = true;
                                textValueOut = AString::number(numericalValueOut);
                            }
                            else {
                                CaretAssert(0);
                            }
                            
                            return true;
                        }
                    }
                        break;
                    case CiftiMappingType::LABELS:
                        CaretAssertMessage(0, "Mapping type should never be LABELS");
                        break;
                    case CiftiMappingType::PARCELS:
                    {
                        const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
                        const int64_t parcelMapIndex = map.getIndexForVoxel(ijk);
                        const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
                        if ((parcelMapIndex >= 0)
                            && (parcelMapIndex < static_cast<int64_t>(parcels.size()))) {
                            CaretAssertVectorIndex(parcels,
                                                   parcelMapIndex);
                            textValueOut = parcels[parcelMapIndex].m_name;
                        }
                        
                        if (parcelMapIndex >= 0) {
                            int64_t itemIndex = -1;
                            switch (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML)) {
                                case CiftiMappingType::BRAIN_MODELS:
                                {
                                    const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataReadingDirectionForCiftiXML);
                                    itemIndex = map.getIndexForVoxel(ijk);
                                }
                                    break;
                                case CiftiMappingType::LABELS:
                                    break;
                                case CiftiMappingType::PARCELS:
                                    break;
                                case CiftiMappingType::SCALARS:
                                    itemIndex = mapIndex;
                                    break;
                                case CiftiMappingType::SERIES:
                                    itemIndex = mapIndex;
                                    break;
                            }
                            if (itemIndex >= 0) {
                                const int64_t numRows = m_ciftiFile->getNumberOfRows();
                                const int64_t numCols = m_ciftiFile->getNumberOfColumns();
                                
                                switch (m_dataReadingDirectionForCiftiXML) {
                                    case CiftiXML::ALONG_COLUMN:
                                    {
                                        std::vector<float> data;
                                        data.resize(numRows);
                                        CaretAssert(parcelMapIndex < numCols);
                                        m_ciftiFile->getColumn(&data[0], parcelMapIndex);
                                        CaretAssertVectorIndex(data, itemIndex);
                                        textValueOut += (" " + AString::number(data[itemIndex]));
                                    }
                                        break;
                                    case CiftiXML::ALONG_ROW:
                                    {
                                        std::vector<float> data;
                                        data.resize(numCols);
                                        CaretAssert(parcelMapIndex < numRows);
                                        m_ciftiFile->getRow(&data[0], parcelMapIndex);
                                        CaretAssertVectorIndex(data, itemIndex);
                                        textValueOut += (" " + AString::number(data[itemIndex]));
                                    }
                                        break;
                                }
                            }
                        }
                    }
                        return true;
                        break;
                    case CiftiMappingType::SCALARS:
                        CaretAssertMessage(0, "Mapping type should never be SCALARS");
                        break;
                    case CiftiMappingType::SERIES:
                        CaretAssertMessage(0, "Mapping type should never be SERIES");
                        break;
                }
            }
        }
    }
            
    return false;
}

/**
 * Get connectivity value for a voxel.  When the data is mapped
 * to parcels, the numerical value will not be valid.
 *
 * @param mapIndices
 *     Indices of the maps.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param numericalValuesOut
 *     Numerical values out.
 * @param numericalValuesOutValid
 *     Output that indicates the numerical values output is valid.
 *     For label data, this value will be the label key.
 * @param textValueOut
 *     Text containing node' value will always be valid if the method
 *     returns true.  For parcel data, this will contain the name of the
 *     parcel.  For label data, this will contain the name of the label.
 *     For numerical data, this will contain the text representation
 *     of the numerical value.
 * @return
 *    True if the text value is valid.  The numerical value may or may not
 *    also be valid.
 */
bool
CiftiMappableDataFile::getMapVolumeVoxelValues(const std::vector<int32_t> mapIndices,
                                               const float xyz[3],
                                               int64_t ijkOut[3],
                                               std::vector<float>& numericalValuesOut,
                                               std::vector<bool>& numericalValuesOutValid,
                                               AString& textValueOut) const
{
    textValueOut = "";
    numericalValuesOut.clear();
    numericalValuesOutValid.clear();
    
    if (mapIndices.empty()) {
        return false;
    }
    
    CaretAssert(m_ciftiFile);
    
    int64_t ijk[3];
    enclosingVoxel(xyz[0],
                   xyz[1],
                   xyz[2],
                   ijk[0],
                   ijk[1],
                   ijk[2]);
    if (indexValid(ijk[0],
                   ijk[1],
                   ijk[2])) {
        /*
         * Only set the IJK if the index is valid.
         */
        ijkOut[0] = ijk[0];
        ijkOut[1] = ijk[1];
        ijkOut[2] = ijk[2];
        const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(ijk[0],
                                                                               ijk[1],
                                                                               ijk[2]);
        if (dataOffset >= 0) {
            CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                        || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
            
            const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
            
            const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(this);
            if (parcelLabelFile != NULL) {
                const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
                const int64_t parcelMapIndex = map.getIndexForVoxel(ijk);
                const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
                AString areaName = "";
                if ((parcelMapIndex >= 0)
                    && (parcelMapIndex < static_cast<int64_t>(parcels.size()))) {
                    CaretAssertVectorIndex(parcels,
                                           parcelMapIndex);
                    areaName = parcels[parcelMapIndex].m_name;
                }
                
                textValueOut = ("Area: "
                                + areaName
                                + ", Network(s): ");
                
                for (std::vector<int32_t>::const_iterator mapIter = mapIndices.begin();
                     mapIter != mapIndices.end();
                     mapIter++) {
                    textValueOut += (getMapName(*mapIter)
                                     + " ");
                }
                return true;
            }
            else {
                switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
                    case CiftiMappingType::BRAIN_MODELS:
                    {
                        for (std::vector<int32_t>::const_iterator mapIter = mapIndices.begin();
                             mapIter != mapIndices.end();
                             mapIter++) {
                            const int32_t mapIndex = *mapIter;
                            /*
                             * Note: For a Dense connectivity file, it may not have
                             * data loaded since data is loaded upon demand.
                             */
                            std::vector<float> mapData;
                            getMapData(mapIndex,
                                       mapData);
                            if ( ! mapData.empty()) {
                                CaretAssertVectorIndex(mapData,
                                                       dataOffset);
                                const float value = mapData[dataOffset];
                                
                                if (isMappedWithLabelTable()) {
                                    textValueOut = "Invalid Label Index";
                                    
                                    const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                                    const int32_t labelKey = static_cast<int32_t>(value);
                                    const GiftiLabel* gl = glt->getLabel(labelKey);
                                    if (gl != NULL) {
                                        textValueOut = (gl->getName());
                                    }
                                    else {
                                        textValueOut += ("InvalidLabelKey="
                                                         + AString::number(labelKey)
                                                         + " ");
                                    }
                                    numericalValuesOut.push_back(value);
                                    numericalValuesOutValid.push_back(false); // NOT VALID !
                                }
                                else if (isMappedWithPalette()) {
                                    numericalValuesOut.push_back(value);
                                    numericalValuesOutValid.push_back(true);
                                    textValueOut = (AString::number(value)
                                                    + " ");
                                }
                                else {
                                    CaretAssert(0);
                                }
                                
                                return true;
                            }
                        }
                    }
                        break;
                    case CiftiMappingType::LABELS:
                        CaretAssertMessage(0, "Mapping type should never be LABELS");
                        break;
                    case CiftiMappingType::PARCELS:
                    {
                        const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
                        const int64_t parcelMapIndex = map.getIndexForVoxel(ijk);
                        const std::vector<CiftiParcelsMap::Parcel>& parcels = map.getParcels();
                        if ((parcelMapIndex >= 0)
                            && (parcelMapIndex < static_cast<int64_t>(parcels.size()))) {
                            CaretAssertVectorIndex(parcels,
                                                   parcelMapIndex);
                            textValueOut = parcels[parcelMapIndex].m_name;
                        }
                        
                        for (std::vector<int32_t>::const_iterator mapIter = mapIndices.begin();
                             mapIter != mapIndices.end();
                             mapIter++) {
                            const int32_t mapIndex = *mapIter;
                            if (parcelMapIndex >= 0) {
                                int64_t itemIndex = -1;
                                switch (ciftiXML.getMappingType(m_dataReadingDirectionForCiftiXML)) {
                                    case CiftiMappingType::BRAIN_MODELS:
                                    {
                                        const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataReadingDirectionForCiftiXML);
                                        itemIndex = map.getIndexForVoxel(ijk);
                                    }
                                        break;
                                    case CiftiMappingType::LABELS:
                                        break;
                                    case CiftiMappingType::PARCELS:
                                        break;
                                    case CiftiMappingType::SCALARS:
                                        itemIndex = mapIndex;
                                        break;
                                    case CiftiMappingType::SERIES:
                                        itemIndex = mapIndex;
                                        break;
                                }
                                if (itemIndex >= 0) {
                                    const int64_t numRows = m_ciftiFile->getNumberOfRows();
                                    const int64_t numCols = m_ciftiFile->getNumberOfColumns();
                                    
                                    switch (m_dataReadingDirectionForCiftiXML) {
                                        case CiftiXML::ALONG_COLUMN:
                                        {
                                            std::vector<float> data;
                                            data.resize(numRows);
                                            CaretAssert(parcelMapIndex < numCols);
                                            m_ciftiFile->getColumn(&data[0], parcelMapIndex);
                                            CaretAssertVectorIndex(data, itemIndex);
                                            textValueOut += (" " + AString::number(data[itemIndex]));
                                        }
                                            break;
                                        case CiftiXML::ALONG_ROW:
                                        {
                                            std::vector<float> data;
                                            data.resize(numCols);
                                            CaretAssert(parcelMapIndex < numRows);
                                            m_ciftiFile->getRow(&data[0], parcelMapIndex);
                                            CaretAssertVectorIndex(data, itemIndex);
                                            textValueOut += (" " + AString::number(data[itemIndex]));
                                        }
                                            break;
                                    }
                                }
                            }
                        }
                    }
                        return true;
                        break;
                    case CiftiMappingType::SCALARS:
                        CaretAssertMessage(0, "Mapping type should never be SCALARS");
                        break;
                    case CiftiMappingType::SERIES:
                        CaretAssertMessage(0, "Mapping type should never be SERIES");
                        break;
                }
            }
        }
    }
    
    return false;
}


/**
 * Get the value of the voxel containing the given coordinate.
 *
 * @param coordinateIn
 *    The 3D coordinate
 * @param validOut
 *    If not NULL, will indicate if the coordinate (and hence the
 *    returned value) is valid.
 * @param mapIndex
 *    Index of map.
 * @param component
 *    Voxel component.
 * @return
 *    Value of voxel containing the given coordinate.
 */
float
CiftiMappableDataFile::getVoxelValue(const float* coordinateIn,
                                   bool* validOut,
                                   const int64_t mapIndex,
                                   const int64_t component) const
{
    return getVoxelValue(coordinateIn[0],
                         coordinateIn[1],
                         coordinateIn[2],
                         validOut,
                         mapIndex,
                         component);
}

/**
 * Get the value of the voxel containing the given coordinate.
 *
 * @param coordinateX
 *    The X coordinate
 * @param coordinateY
 *    The Y coordinate
 * @param coordinateZ
 *    The Z coordinate
 * @param validOut
 *    If not NULL, will indicate if the coordinate (and hence the
 *    returned value) is valid.
 * @param mapIndex
 *    Index of map.
 * @param component
 *    Voxel component.
 * @return
 *    Value of voxel containing the given coordinate.
 */
float
CiftiMappableDataFile::getVoxelValue(const float coordinateX,
                                   const float coordinateY,
                                   const float coordinateZ,
                                   bool* validOut,
                                   const int64_t mapIndex,
                                   const int64_t component) const
{
    if (validOut != NULL) {
        *validOut = false;
    }
    
    CaretAssert(m_ciftiFile);
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    int64_t voxelI, voxelJ, voxelK;
    enclosingVoxel(coordinateX,
                   coordinateY,
                   coordinateZ,
                   voxelI,
                   voxelJ,
                   voxelK);
    if (indexValid(voxelI,
                   voxelJ,
                   voxelK,
                   mapIndex,
                   component)) {
        const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(voxelI,
                                                                               voxelJ,
                                                                               voxelK);
        if (dataOffset >= 0) {
            std::vector<float> mapData;
            getMapData(mapIndex,
                       mapData);
            CaretAssertVectorIndex(mapData,
                                   dataOffset);
            const float value = mapData[dataOffset];
            if (validOut != NULL) {
                *validOut = true;
            }
            return value;
        }
    }
    
    return 0.0;
}

/**
 * @param coordinate
 *    Coordinate for which enclosing voxel is located.
 * @param mapIndex
 *    Index of the map.
 * @return 
 *    Offset in map data for enclosing voxel or NULL if not within a voxel.
 */
int64_t
CiftiMappableDataFile::getMapDataOffsetForVoxelAtCoordinate(const float coordinate[3],
                                             const int32_t mapIndex) const
{
    int64_t dataOffset = -1;
    
    CaretAssert(m_ciftiFile);
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    int64_t voxelI, voxelJ, voxelK;
    enclosingVoxel(coordinate[0],
                   coordinate[1],
                   coordinate[2],
                   voxelI,
                   voxelJ,
                   voxelK);
    if (indexValid(voxelI,
                   voxelJ,
                   voxelK,
                   mapIndex,
                   0)) {
        dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(voxelI,
                                                                 voxelJ,
                                                                 voxelK);
    }
    
    return dataOffset;    
}

/**
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param textOut
 *    Output containing identification information.
 */
bool
CiftiMappableDataFile::getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                           const float xyz[3],
                                                           int64_t ijkOut[3],
                                                           AString& textOut) const
{
    CaretAssert(m_ciftiFile);
    
    const int32_t numberOfMapIndices = static_cast<int32_t>(mapIndices.size());
    if (numberOfMapIndices <= 0) {
        return false;
    }
    
    textOut = "";
    
    std::vector<float> numericalValues;
    std::vector<bool>  numericalValuesValid;
    AString textValue;
    if (getMapVolumeVoxelValues(mapIndices,
                                xyz,
                                ijkOut,
                                numericalValues,
                                numericalValuesValid,
                                textValue)) {
        textOut = textValue;
    }
    
    if (textOut.isEmpty() == false) {
        return true;
    }
    
    return false;
}

/**
 * Set the status to unmodified.
 */
void
CiftiMappableDataFile::clearModified()
{
    CaretMappableDataFile::clearModified();
    
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    ciftiXML.getFileMetaData()->clearModified();
    
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; i++) {
        m_mapContent[i]->clearModified();
    }
}

/**
 * @return True if the file is modified in any way EXCEPT for
 * the palette color mapping.  Also see isModified().
 */
bool
CiftiMappableDataFile::isModifiedExcludingPaletteColorMapping() const
{
    if (CaretMappableDataFile::isModifiedExcludingPaletteColorMapping()) {
        return true;
    }
    
    CaretAssert(m_ciftiFile);
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    if (ciftiXML.getFileMetaData()->isModified()) {
        return true;
    }
    
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; i++) {
        if (m_mapContent[i]->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return The units for the 'interval' between two consecutive maps.
 */
NiftiTimeUnitsEnum::Enum
CiftiMappableDataFile::getMapIntervalUnits() const
{
    return m_mappingTimeUnits;
}

/**
 * Get the units value for the first map and the
 * quantity of units between consecutive maps.  If the
 * units for the maps is unknown, value of one (1) are
 * returned for both output values.
 *
 * @param firstMapUnitsValueOut
 *     Output containing units value for first map.
 * @param mapIntervalStepValueOut
 *     Output containing number of units between consecutive maps.
 */
void
CiftiMappableDataFile::getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                                  float& mapIntervalStepValueOut) const
{
    firstMapUnitsValueOut   = m_mappingTimeStart;
    mapIntervalStepValueOut = m_mappingTimeStep;
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
CiftiMappableDataFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    CaretAssert(m_ciftiFile);
    
    /*
     * Dense is very large but at this time is [-1, 1]
     */
    if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE) {
        dataRangeMaximumOut = 1.0;
        dataRangeMinimumOut = -1;
        return true;
    }
    else {
        if (m_ciftiFile->getDataRangeFromAllMaps(dataRangeMinimumOut,
                                                      dataRangeMaximumOut)) {
            return true;
        }
    }
    
    /*
     * Default
     */
    dataRangeMaximumOut = std::numeric_limits<float>::max();
    dataRangeMinimumOut = -dataRangeMaximumOut;
    
    return false;
}

/**
 * Get the number of nodes for the structure for mapping data.
 *
 * @param structure
 *     Structure for which number of nodes is requested.
 * @return
 *     Number of nodes corresponding to structure.  If no matching structure
 *     is found, a negative value is returned.
 */
int32_t
CiftiMappableDataFile::getMappingSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    int32_t numCiftiNodes = -1;
    
    if (m_dataMappingAccessMethod == DATA_ACCESS_NONE) {
        return numCiftiNodes;
    }
    
    CaretAssert((m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_ROW)
                || (m_dataMappingDirectionForCiftiXML == CiftiXML::ALONG_COLUMN));
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    switch (ciftiXML.getMappingType(m_dataMappingDirectionForCiftiXML)) {
        case CiftiMappingType::BRAIN_MODELS:
        {
            const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.hasSurfaceData(structure)) {
                numCiftiNodes = map.getSurfaceNumberOfNodes(structure);
            }
        }
            break;
        case CiftiMappingType::LABELS:
            CaretAssertMessage(0, "Mapping type should never be LABELS");
            break;
        case CiftiMappingType::PARCELS:
        {
            const CiftiParcelsMap& map = ciftiXML.getParcelsMap(m_dataMappingDirectionForCiftiXML);
            if (map.hasSurfaceData(structure)) {
                numCiftiNodes = map.getSurfaceNumberOfNodes(structure);
            }
        }
            break;
        case CiftiMappingType::SCALARS:
            CaretAssertMessage(0, "Mapping type should never be SCALARS");
            break;
        case CiftiMappingType::SERIES:
            CaretAssertMessage(0, "Mapping type should never be SERIES");
            break;
    }
    
    return numCiftiNodes;
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
CiftiMappableDataFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                           SceneClass* sceneClass)
{
    CaretMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    if (isMappedWithLabelTable()) {
        sceneClass->addClass(m_classNameHierarchy->saveToScene(sceneAttributes,
                                                               "m_classNameHierarchy"));
    }
    
    if (m_labelDynamicThresholdFile) {
        sceneClass->addClass(m_labelDynamicThresholdFile->saveToScene(sceneAttributes,
                                                                      "m_labelDynamicThresholdFile"));
    }
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
CiftiMappableDataFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    CaretMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    if (isMappedWithLabelTable()) {
        m_classNameHierarchy->restoreFromScene(sceneAttributes,
                                               sceneClass->getClass("m_classNameHierarchy"));
    }
    
    m_labelDynamicThresholdFile.reset();
    const SceneClass* dynLabelFileSceneClass = sceneClass->getClass("m_labelDynamicThresholdFile");
    if (dynLabelFileSceneClass != NULL) {
        createLabelDynamicThresholdFile();
        if (m_labelDynamicThresholdFile) {
            m_labelDynamicThresholdFile->restoreFromScene(sceneAttributes,
                                                          dynLabelFileSceneClass);
        }
    }
    
    /*
     * When a scene is created and there is a modified palette, the user may choose
     * to save the modified palette to the scene so that the file does not need to
     * be saved with a changed palette.  When scenes are loaded and a file in the
     * scene is already in memory, the file is NOT reloaded to save time.  However,
     * since the palette may be saved to the scene, the coloring will needd to be
     * updated.  If this is not done, the coloring of the file's data prior to 
     * loading the scene remains and may be incorrect.
     */
    invalidateColoringInAllMaps();
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CiftiMappableDataFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretAssert(m_ciftiFile);
    
    CaretMappableDataFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Number of Rows", m_ciftiFile->getNumberOfRows());
    dataFileInformation.addNameAndValue("Number of Columns", m_ciftiFile->getNumberOfColumns());
    
    int64_t dimI, dimJ, dimK, dimTime, dimNumComp;
    getDimensions(dimI,
                  dimJ,
                  dimK,
                  dimTime,
                  dimNumComp);
    
    dataFileInformation.addNameAndValue("Volume Dim[0]", dimI);
    dataFileInformation.addNameAndValue("Volume Dim[1]", dimJ);
    dataFileInformation.addNameAndValue("Volume Dim[2]", dimK);
    
    AString paletteType;
    switch (m_paletteColorMappingSource) {
        case PALETTE_COLOR_MAPPING_SOURCE_INVALID:
            paletteType = "None";
            break;
        case PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE:
            paletteType = "File (One for all maps)";
            break;
        case PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP:
            paletteType = "Map (Unique for each map)";
            break;
    }
    dataFileInformation.addNameAndValue("Palette Type",
                                        paletteType);
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    CiftiMappableDataFile::addCiftiXmlToDataFileContentInformation(dataFileInformation,
                                                                   ciftiXML);
}

/**
 * Get a text name for a CIFTI mapping type.
 *
 * @param mappingType
 *    The CIFTI mapping type.
 * @return 
 *    String containing text name.
 */
AString
CiftiMappableDataFile::mappingTypeToName(const CiftiMappingType::MappingType mappingType)
{
    AString mapTypeName;
    switch (mappingType) {
        case CiftiMappingType::BRAIN_MODELS:
            mapTypeName = "BRAIN_MODELS";
            break;
        case CiftiMappingType::LABELS:
            mapTypeName = "LABELS";
            break;
        case CiftiMappingType::PARCELS:
            mapTypeName = "PARCELS";
            break;
        case CiftiMappingType::SCALARS:
            mapTypeName = "SCALARS";
            break;
        case CiftiMappingType::SERIES:
            mapTypeName = "SERIES";
            break;
    }
    
    return mapTypeName;
}


/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 * @param ciftiXML
 *    The CIFTI XML.
 * @param numberOfCiftiDimensions
 *    Number of Dimensions of the CIFTI File.
 */
void
CiftiMappableDataFile::addCiftiXmlToDataFileContentInformation(DataFileContentInformation& dataFileInformation,
                                                               const CiftiXML& ciftiXML)
{
    std::vector<int64_t> dims = ciftiXML.getDimensions();
    const int32_t numDims = static_cast<int32_t>(dims.size());
    
    for (int32_t i = 0; i < numDims; i++) {
        dataFileInformation.addNameAndValue(("CIFTI Dim[" + AString::number(i) + "]"),
                                            dims[i]);
    }
    
    const bool showLabelMappingsFlag = dataFileInformation.isOptionFlag(DataFileContentInformation::OPTION_SHOW_CIFTI_LABEL_MAPPING);
    
    for (int32_t alongType = 0; alongType < numDims; alongType++) {
        AString alongName;
        CiftiMappingType::MappingType mapType = CiftiMappingType::BRAIN_MODELS;
        
        switch (alongType) {
            case CiftiXML::ALONG_ROW:
                alongName = "ALONG_ROW";
                mapType = ciftiXML.getMappingType(CiftiXML::ALONG_ROW);
                break;
            case CiftiXML::ALONG_COLUMN:
                alongName = "ALONG_COLUMN";
                mapType = ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN);
                break;
            case CiftiXML::ALONG_STACK:
                alongName = "ALONG_STACK";
                mapType = ciftiXML.getMappingType(CiftiXML::ALONG_STACK);
                break;
        }
        
        AString mapInfoString;
        if ( ! alongName.isEmpty()) {
            AString mapTypeName;
            switch (mapType) {
                case CiftiMappingType::BRAIN_MODELS:
                    mapTypeName = "BRAIN_MODELS";
                    break;
                case CiftiMappingType::LABELS:
                    mapTypeName = "LABELS";
                    break;
                case CiftiMappingType::PARCELS:
                    mapTypeName = "PARCELS";
                    break;
                case CiftiMappingType::SCALARS:
                    mapTypeName = "SCALARS";
                    break;
                case CiftiMappingType::SERIES:
                    mapTypeName = "SERIES";
                    break;
            }
            dataFileInformation.addNameAndValue((alongName
                                                 + " map type"),
                                                mapTypeName);
            
            switch (mapType) {
                case CiftiMappingType::BRAIN_MODELS:
                {
                    const CiftiBrainModelsMap& bmm = ciftiXML.getBrainModelsMap(alongType);
                    
                    dataFileInformation.addNameAndValue("    Has Volume Data",
                                                        bmm.hasVolumeData());
                    
                    if (bmm.hasVolumeData()) {
                        const VolumeSpace volumeSpace = bmm.getVolumeSpace();
                        const int64_t* dims = volumeSpace.getDims();
                        dataFileInformation.addNameAndValue("    Volume Dims",
                                                            AString::fromNumbers(dims, 3, ","));
                        const std::vector<std::vector<float> >& sform = volumeSpace.getSform();
                        dataFileInformation.addNameAndValue("    Volume Space",
                                                            AString::fromNumbers(sform[0].data(), 4, ",") + ";" +
                                                            AString::fromNumbers(sform[1].data(), 4, ",") + ";" +
                                                            AString::fromNumbers(sform[2].data(), 4, ","));
                    }
                    
                    std::vector<CiftiBrainModelsMap::ModelInfo> modelInfo = bmm.getModelInfo();//allows us to visit the models in the order they are in the file
                    for (int i = 0; i < (int)modelInfo.size(); ++i)
                    {
                        if (modelInfo[i].m_type == CiftiBrainModelsMap::SURFACE)
                        {
                            dataFileInformation.addNameAndValue(("    "
                                                                + StructureEnum::toGuiName(modelInfo[i].m_structure)),
                                                                (AString::number(modelInfo[i].m_indexCount) + " out of " + AString::number(bmm.getSurfaceNumberOfNodes(modelInfo[i].m_structure))
                                                                + " vertices"));
                        } else {
                            CaretAssert(modelInfo[i].m_type == CiftiBrainModelsMap::VOXELS);
                            dataFileInformation.addNameAndValue(("    "
                                                                + StructureEnum::toGuiName(modelInfo[i].m_structure)),
                                                                (AString::number(modelInfo[i].m_indexCount)
                                                                + " voxels"));
                        }
                    }
                }
                    break;
                case CiftiMappingType::LABELS:
                    if (showLabelMappingsFlag) {
                        const CiftiLabelsMap& clm = ciftiXML.getLabelsMap(alongType);
                        const int32_t numItems = clm.getLength();
                        for (int32_t i = 0; i < numItems; i++) {
                            dataFileInformation.addText("    Index="
                                                        + AString::number(i)
                                                        + " Name="
                                                        + clm.getMapName(i)
                                                        + "\n"
                                                        + clm.getMapLabelTable(i)->toFormattedString("        ")
                                                        + "\n");
                        }
                    }
                    break;
                case CiftiMappingType::PARCELS:
                {
                    const CiftiParcelsMap& cpm = ciftiXML.getParcelsMap(alongType);
                    
                    dataFileInformation.addNameAndValue("    Has Volume Data",
                                                        cpm.hasVolumeData());
                    
                    if (cpm.hasVolumeData()) {
                        const VolumeSpace volumeSpace = cpm.getVolumeSpace();
                        const int64_t* dims = volumeSpace.getDims();
                        dataFileInformation.addNameAndValue("    Volume Dims",
                                                            AString::fromNumbers(dims, 3, ","));
                    }
                    
                    
                    const std::vector<StructureEnum::Enum> surfaceStructures = cpm.getParcelSurfaceStructures();
                    for (std::vector<StructureEnum::Enum>::const_iterator surfaceIter = surfaceStructures.begin();
                         surfaceIter != surfaceStructures.end();
                         surfaceIter++) {
                        const StructureEnum::Enum structure = *surfaceIter;
                        dataFileInformation.addNameAndValue(("    "
                                                             + StructureEnum::toGuiName(structure)),
                                                            (AString::number(cpm.getSurfaceNumberOfNodes(structure))
                                                             + " vertices"));
                    }
                    

                    const std::vector<CiftiParcelsMap::Parcel>& parcels = cpm.getParcels();
                    for (std::vector<CiftiParcelsMap::Parcel>::const_iterator parcelIter = parcels.begin();
                         parcelIter != parcels.end();
                         parcelIter++) {
                        const CiftiParcelsMap::Parcel parcel = *parcelIter;
                        dataFileInformation.addNameAndValue("    Parcel " + AString::number(parcelIter - parcels.begin() + 1), parcel.m_name);
                        
                        for (std::map<StructureEnum::Enum, std::set<int64_t> >::const_iterator surfIter = parcel.m_surfaceNodes.begin();
                             surfIter != parcel.m_surfaceNodes.end();
                             surfIter++) {
                            const StructureEnum::Enum structure  = surfIter->first;
                            const std::set<int64_t>& nodeIndices = surfIter->second;
                            dataFileInformation.addNameAndValue("        " +
                                                                StructureEnum::toGuiName(structure),
                                                                AString::number(nodeIndices.size()) + " vertices");
                            
                        }
                        if (parcel.m_voxelIndices.size() != 0) 
                        {
                            dataFileInformation.addNameAndValue("    ", AString::number(parcel.m_voxelIndices.size()) + " voxels");
                        }
                    }
                }
                    break;
                case CiftiMappingType::SCALARS:
                    break;
                case CiftiMappingType::SERIES:
                {
                    const CiftiSeriesMap& csm = ciftiXML.getSeriesMap(alongType);
                    dataFileInformation.addNameAndValue("    Start", csm.getStart());
                    dataFileInformation.addNameAndValue("    Step",  csm.getStep());
                    QString unitsName = "Unknown";
                    switch (csm.getUnit()) {
                        case CiftiSeriesMap::HERTZ:
                            unitsName = "Hertz";
                            break;
                        case CiftiSeriesMap::METER:
                            unitsName = "Meters";
                            break;
                        case CiftiSeriesMap::RADIAN:
                            unitsName = "Radians";
                            break;
                        case CiftiSeriesMap::SECOND:
                            unitsName = "Seconds";
                            break;
                    }
                    dataFileInformation.addNameAndValue("    Units", unitsName);
                }
                    break;
            }
        }
    }    
}

/**
 * Get information about the content of a generic CIFTI file that is
 * not a Workbench supported CIFTI file type.
 *
 * @param filename
 *     Name of the file.
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CiftiMappableDataFile::getDataFileContentInformationForGenericCiftiFile(const AString& filename,
                                                                        DataFileContentInformation& dataFileInformation)
{
    CiftiFile ciftiFile(filename);
    const CiftiXML& ciftiXML = ciftiFile.getCiftiXML();
    
    std::vector<int64_t> dims = ciftiXML.getDimensions();
    const int32_t numDims = static_cast<int32_t>(dims.size());
    int64_t dataSizeInBytes = 1;
    for (int32_t i = 0; i < numDims; i++) {
        dataSizeInBytes *= dims[i];
    }
    dataSizeInBytes *= sizeof(float);

    dataFileInformation.addNameAndValue("Name", filename);
    dataFileInformation.addNameAndValue("Type", AString("Connectivity Unknown (Could be Unsupported CIFTI File)"));
    dataFileInformation.addNameAndValue("Data Size", FileInformation::fileSizeToStandardUnits(dataSizeInBytes));
    
    dataFileInformation.setOptionFlag(DataFileContentInformation::OPTION_SHOW_CIFTI_LABEL_MAPPING,
                                      true);
    CiftiMappableDataFile::addCiftiXmlToDataFileContentInformation(dataFileInformation,
                                                                   ciftiXML);
}



/**
 * Help load charting data for the surface with the given structure and node average.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndices
 *     Indices of nodes for averaging.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will return true.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiMappableDataFile::helpLoadChartDataForSurfaceNodeAverage(const StructureEnum::Enum structure,
                                                              const std::vector<int32_t>& nodeIndices)
{
    ChartDataCartesian* chartData = NULL;
    
    try {
        const int32_t numberOfNodes = static_cast<int32_t>(nodeIndices.size());
        if (numberOfNodes > 0) {
            std::vector<double> dataSum;
            int32_t dataSumSize = 0;
            int32_t dataAverageCount = 0;
            
            std::vector<float> data;
            bool firstNodeFlag = true;
            
            for (int32_t i = 0; i < numberOfNodes; i++) {
                if (getSeriesDataForSurfaceNode(structure,
                                                nodeIndices[i],
                                                data)) {
                    if (firstNodeFlag) {
                        firstNodeFlag = false;
                        
                        dataSumSize = static_cast<int32_t>(data.size());
                        if (dataSumSize > 0) {
                            dataSum.resize(dataSumSize,
                                           0.0);
                        }
                    }
                    
                    CaretAssert(dataSumSize == static_cast<int32_t>(data.size()));
                    
                    for (int32_t j = 0; j < dataSumSize; j++) {
                        dataSum[j] += data[j];
                    }
                    dataAverageCount++;
                }
            }
            
            if ((dataAverageCount > 0)
                && (dataSumSize > 0)) {
                std::vector<float> dataAverage(dataSumSize);
                for (int32_t k = 0; k < dataSumSize; k++) {
                    dataAverage[k] = dataSum[k] / dataAverageCount;
                }
                chartData = helpCreateCartesianChartData(dataAverage);
            }
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

/**
 * Help load charting data for the voxel at the given coordinate.
 *
 * @param xyz
 *     The voxel coordinate.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will return true.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiMappableDataFile::helpLoadChartDataForVoxelAtCoordinate(const float xyz[3])
{
    ChartDataCartesian* chartData = NULL;
    
    try {
        std::vector<float> data;
        if (getSeriesDataForVoxelAtCoordinate(xyz, data)) {
            chartData = helpCreateCartesianChartData(data);
            
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

/**
 * Help load charting data for the surface with the given structure and node index.
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
CiftiMappableDataFile::helpLoadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                       const int32_t nodeIndex)
{
    ChartDataCartesian* chartData = NULL;
    
    try {
        std::vector<float> data;
        if (getSeriesDataForSurfaceNode(structure,
                                        nodeIndex,
                                        data)) {
            
            chartData = helpCreateCartesianChartData(data);
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

bool CiftiMappableDataFile::getMapDataForSurface(const int32_t mapIndex, const StructureEnum::Enum structure, std::vector<float>& surfaceMapData, std::vector<float>* roiData) const
{
    surfaceMapData.clear();//empty data is secondary hint at failure
    CaretAssert(m_ciftiFile);
    CaretAssertVectorIndex(m_mapContent, mapIndex);

    const int32_t surfaceNumNodes = getMappingSurfaceNumberOfNodes(structure);
    if (surfaceNumNodes < 1) return false;

    std::vector<float> mapData;
    getMapData(mapIndex, mapData);
    /*
     * Map data may be empty for connectivity matrix files with no rows loaded.
     */
    if (mapData.empty())
    {
        return false;
    }
    std::vector<int64_t> dataIndicesForNodes;
    if (!getSurfaceDataIndicesForMappingToBrainordinates(structure, surfaceNumNodes, dataIndicesForNodes))
    {
        return false;//currently should never happen, this currently works for parcellated files
    }
    CaretAssert((int)dataIndicesForNodes.size() == surfaceNumNodes);
    
    surfaceMapData.resize(surfaceNumNodes, 0.0f);
    if (roiData != NULL)
    {
        roiData->clear();//make sure all values get initialized before setting the roi nodes
        roiData->resize(surfaceNumNodes, 0.0f);
    }
    for (int32_t iNode = 0; iNode < surfaceNumNodes; iNode++) {
        CaretAssertVectorIndex(dataIndicesForNodes,
                                iNode);
        
        const int64_t dataIndex = dataIndicesForNodes[iNode];
        if (dataIndex >= 0) {
            surfaceMapData[iNode] = mapData[dataIndex];
            if (roiData != NULL)
            {
                (*roiData)[iNode] = 1.0f;
            }
        }
    }
    return true;
}

/**
 * Set the map data for the given structure.
 *
 * @param mapIndex
 *    Index of the map.
 * @param structure
 *    The surface structure.
 * @param surfaceMapData
 *    Data for surface map that must contain same number of elements as
 *    in the brain models map for the surface.
 * @throw
 *    DataFileException if there is an error.
 *
 */
void
CiftiMappableDataFile::setMapDataForSurface(const int32_t mapIndex,
                                                   const StructureEnum::Enum structure,
                                                   const std::vector<float> surfaceMapData)
{
    CaretAssert(m_ciftiFile);
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    
    const int32_t surfaceNumberOfNodes = static_cast<int32_t>(surfaceMapData.size());
    const int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
    if (numCiftiNodes != surfaceNumberOfNodes) {
        return;
    }
    
    std::vector<float> mapData;
    getMapData(mapIndex,
               mapData);
    
    /*
     * Map data may be empty for connectivity matrix files with no rows loaded.
     */
    if (mapData.empty()) {
        return;
    }
    
    std::vector<int64_t> dataIndicesForNodes;
    if (getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                                   surfaceNumberOfNodes,
                                                        dataIndicesForNodes)) {
        
        for (int32_t iNode = 0; iNode < surfaceNumberOfNodes; iNode++) {
            CaretAssertVectorIndex(dataIndicesForNodes,
                                   iNode);
            
            const int64_t dataIndex = dataIndicesForNodes[iNode];
            if (dataIndex >= 0) {
                mapData[dataIndex] = surfaceMapData[iNode];
            }
        }
        
        setMapData(mapIndex,
                   mapData);
    }
}

/**
 * Get the matrix dimensions.
 *
 * @param numberOfRowsOut
 *    Output number of rows in rgba matrix.
 * @param numberOfColumnsOut
 *    Output number of Columns in rgba matrix.
 */
void
CiftiMappableDataFile::helpMapFileGetMatrixDimensions(int32_t& numberOfRowsOut,
                                                      int32_t& numberOfColumnsOut) const
{
    CaretAssert(m_ciftiFile);
    
    /*
     * Dimensions of matrix.
     */
    numberOfRowsOut    = m_ciftiFile->getNumberOfRows();
    numberOfColumnsOut = m_ciftiFile->getNumberOfColumns();
}


/**
 * Help load matrix chart data and order in the given row indices
 * for a file with multi-mapped file that uses a unqiue palette
 * or label table for each column (row) in the file.
 *
 * @param numberOfRowsOut
 *    Output number of rows in rgba matrix.
 * @param numberOfColumnsOut
 *    Output number of Columns in rgba matrix.
 * @param rowIndicesIn
 *    Indices of rows inserted into matrix.
 * @param rgbaOut
 *    RGBA matrix (number of elements is rows * columns * 4).
 * @return
 *    True if output data is valid, else false.
 */
bool
CiftiMappableDataFile::helpMapFileLoadChartDataMatrixRGBA(int32_t& numberOfRowsOut,
                                                          int32_t& numberOfColumnsOut,
                                                          const std::vector<int32_t>& rowIndicesIn,
                                                          std::vector<float>& rgbaOut) const
{
    CaretAssert(m_ciftiFile);

    /*
     * Dimensions of matrix.
     */
    numberOfRowsOut    = m_ciftiFile->getNumberOfRows();
    numberOfColumnsOut = m_ciftiFile->getNumberOfColumns();
    const int32_t numberOfData = numberOfRowsOut * numberOfColumnsOut;
    if (numberOfData <= 0) {
        return false;
    }

    const bool useLabelTableFlag = isMappedWithLabelTable();
    const bool usePaletteFlag    = isMappedWithPalette();
    
    if (( ! useLabelTableFlag)
        && ( ! usePaletteFlag)) {
        CaretAssert(0);
        return false;
    }
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    const AString badMapTypeMessage("Matrix charts supports only maps in columns at this time for LABEL, SCALAR, and SERIES data");
    if (useLabelTableFlag) {
        if (ciftiXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS) {
            CaretAssertMessage(0, badMapTypeMessage);
            CaretLogSevere(badMapTypeMessage);
            return false;
        }
    }

    if (usePaletteFlag) {
        if (ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS) {
            /* ok */
        }
        else if (ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES) {
            /* ok */
        }
        else {
            CaretAssertMessage(0, badMapTypeMessage);
            CaretLogSevere(badMapTypeMessage);
            return false;
        }
    }
    
    std::vector<int32_t> rowIndices = rowIndicesIn;
    if (rowIndices.empty()) {
        rowIndices.resize(numberOfRowsOut);
        for (int32_t i = 0; i < numberOfRowsOut; i++) {
            rowIndices[i] = i;
        }
    }
    else {
        if (static_cast<int32_t>(rowIndices.size()) != numberOfRowsOut) {
            const AString msg = AString("rowIndices size=%1 is different than "
                                        "number of rows in the matrix=%2.").arg(rowIndices.size()).arg(numberOfRowsOut);
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
            return false;
        }
    }
    
    /*
     * Set up Fast Stats for files that use all data for
     * statistics and color mapping.
     */
    CiftiMappableDataFile* nonConstMapFile = const_cast<CiftiMappableDataFile*>(this);
    
    /*
     * Allocate rgba output
     */
    const int32_t numberOfRgba = numberOfData * 4;
    rgbaOut.resize(numberOfRgba);

    /*
     * Get each column, color it using its label table, and then
     * add the column's coloring into the output coloring.
     */
    std::vector<float> columnData(numberOfRowsOut);
    std::vector<float> columnRGBA(numberOfRowsOut * 4);
    for (int32_t iCol = 0; iCol < numberOfColumnsOut; iCol++) {
        CaretAssertVectorIndex(m_mapContent, iCol);
        m_ciftiFile->getColumn(&columnData[0],
                               iCol);
        if (useLabelTableFlag) {
            const GiftiLabelTable* labelTable = getMapLabelTable(iCol);
            NodeAndVoxelColoring::colorIndicesWithLabelTable(labelTable,
                                                             &columnData[0],
                                                             numberOfRowsOut,
                                                             &columnRGBA[0]);
        }
        else if (usePaletteFlag) {
            const PaletteColorMapping* pcm = getMapPaletteColorMapping(iCol);
            CaretAssert(pcm);
            
            NodeAndVoxelColoring::colorScalarsWithPalette(nonConstMapFile->getFileFastStatistics(),
                                                          pcm,
                                                          &columnData[0],
                                                          pcm,
                                                          &columnData[0],
                                                          numberOfRowsOut,
                                                          &columnRGBA[0]);
        }
        else {
            CaretAssert(0);
        }

        for (int32_t iRow = 0; iRow < numberOfRowsOut; iRow++) {
            const int32_t rgbaOffset = (((iRow * numberOfColumnsOut)
                                        + iCol) * 4);
            CaretAssertVectorIndex(rgbaOut, rgbaOffset + 3);
            const int32_t columnRgbaOffset = (iRow * 4);
            CaretAssertVectorIndex(columnRGBA, columnRgbaOffset + 3);
            rgbaOut[rgbaOffset] = columnRGBA[columnRgbaOffset];
            rgbaOut[rgbaOffset+1] = columnRGBA[columnRgbaOffset+1];
            rgbaOut[rgbaOffset+2] = columnRGBA[columnRgbaOffset+2];
            rgbaOut[rgbaOffset+3] = columnRGBA[columnRgbaOffset+3];
        }
    }
        
    return true;
}

/**
 * Help load matrix chart data and order in the given row indices
 * for a connectivity matrix file where one palette is used
 * for all data in the file.
 *
 * @param numberOfRowsOut
 *    Output number of rows in rgba matrix.
 * @param numberOfColumnsOut
 *    Output number of Columns in rgba matrix.
 * @param rowIndicesIn
 *    Indices of rows inserted into matrix.
 * @param rgbaOut
 *    RGBA matrix (number of elements is rows * columns * 4).
 * @return
 *    True if output data is valid, else false.
 */
bool
CiftiMappableDataFile::helpMatrixFileLoadChartDataMatrixRGBA(int32_t& numberOfRowsOut,
                                                             int32_t& numberOfColumnsOut,
                                                             const std::vector<int32_t>& rowIndicesIn,
                                                             std::vector<float>& rgbaOut) const
{
    CaretAssert(m_ciftiFile);
    
    /*
     * Dimensions of matrix.
     */
    numberOfRowsOut    = m_ciftiFile->getNumberOfRows();
    numberOfColumnsOut = m_ciftiFile->getNumberOfColumns();
    const int32_t numberOfData = numberOfRowsOut * numberOfColumnsOut;
    if (numberOfData <= 0) {
        return false;
    }
    
    std::vector<int32_t> rowIndices = rowIndicesIn;
    if (rowIndices.empty()) {
        rowIndices.resize(numberOfRowsOut);
        for (int32_t i = 0; i < numberOfRowsOut; i++) {
            rowIndices[i] = i;
        }
    }
    else {
        if (static_cast<int32_t>(rowIndices.size()) != numberOfRowsOut) {
            const AString msg = AString("rowIndices size=%1 is different than "
                                        "number of rows in the matrix=%2.").arg(rowIndices.size()).arg(numberOfRowsOut);
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
            return false;
        }
    }
    
    /*
     * Get the data.
     */
    std::vector<float> data(numberOfData);
    for (int32_t iRow = 0; iRow < numberOfRowsOut; iRow++) {
        CaretAssertVectorIndex(rowIndices, iRow);
        const int32_t rowIndex = rowIndices[iRow];
        const int32_t rowOffset = rowIndex * numberOfColumnsOut;
        CaretAssertVectorIndex(data, rowOffset + numberOfColumnsOut - 1);
        m_ciftiFile->getRow(&data[rowOffset],
                            iRow);
    }
    
    /*
     * Get palette for color mapping.
     */
    if (isMappedWithPalette()) {
        const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
        const PaletteColorMapping* pcm = ciftiXML.getFilePalette();
        CaretAssert(pcm);
        
        /*
         * Set up Fast Stats for files that use all data for
         * statistics and color mapping.
         * Map "0" will return the file fast statistics
         */
        CiftiMappableDataFile* nonConstMapFile = const_cast<CiftiMappableDataFile*>(this);
        const FastStatistics* fileFastStats = nonConstMapFile->getFileFastStatistics();

        /*
         * Color the data.
         */
        const int32_t numRGBA = numberOfData * 4;
        rgbaOut.resize(numRGBA);
        NodeAndVoxelColoring::colorScalarsWithPalette(fileFastStats,
                                                      pcm,
                                                      &data[0],
                                                      pcm,
                                                      &data[0],
                                                      numberOfData,
                                                      &rgbaOut[0]);
        
        return true;
    }
    else {
        CaretAssertMessage(0, "Only palette mapped files supported at this time.");
    }
    
    return false;
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
CiftiMappableDataFile::getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                          std::vector<float>& dataOut) const
{
    dataOut.clear();
    
    switch (mapFileDataSelector.getDataSelectionType()) {
        case MapFileDataSelector::DataSelectionType::INVALID:
            break;
        case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
        {
            const bool loadColumnDataFlag = false;
            if (loadColumnDataFlag) {
                const DataFileTypeEnum::Enum dataFileType = getDataFileType();
                if (dataFileType == DataFileTypeEnum::UNKNOWN) {
                    CaretMappableDataFile* mapFile = NULL;
                    AString mapFileName;
                    int32_t columnIndex = -1;
                    mapFileDataSelector.getColumnIndex(mapFile,
                                                       mapFileName,
                                                       columnIndex);
                    bool loadDataFlag = false;
                    if (mapFile != NULL) {
                        if (mapFile == dynamic_cast<const CaretMappableDataFile*>(this)) {
                            loadDataFlag = true;
                        }
                    }
                    else if (mapFileName.endsWith(getFileNameNoPath())) {
                        loadDataFlag = true;
                    }
                    if (loadDataFlag) {
                        if ((columnIndex >= 0)
                            && (columnIndex < m_ciftiFile->getNumberOfColumns())) {
                            const int32_t numberOfElementsInColumn = m_ciftiFile->getNumberOfRows();
                            if (numberOfElementsInColumn > 0) {
                                dataOut.resize(numberOfElementsInColumn);
                                m_ciftiFile->getColumn(&dataOut[0],
                                                       columnIndex);
                            }
                        }
                    }
                }
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::ROW_DATA:
        {
            const DataFileTypeEnum::Enum dataFileType = getDataFileType();
            if (dataFileType == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                CaretMappableDataFile* mapFile = NULL;
                AString mapFileName;
                int32_t rowIndex = -1;
                mapFileDataSelector.getRowIndex(mapFile,
                                                mapFileName,
                                                rowIndex);
                
                bool loadDataFlag = false;
                if (mapFile != NULL) {
                    if (mapFile == dynamic_cast<const CaretMappableDataFile*>(this)) {
                        loadDataFlag = true;
                    }
                }
                else if (mapFileName.endsWith(getFileNameNoPath())) {
                    loadDataFlag = true;
                }
                if (loadDataFlag) {
                    if ((rowIndex >= 0)
                        && (rowIndex < m_ciftiFile->getNumberOfRows())) {
                        const int32_t numberOfElementsInRow = m_ciftiFile->getNumberOfColumns();
                        if (numberOfElementsInRow > 0) {
                            dataOut.resize(numberOfElementsInRow);
                            m_ciftiFile->getRow(&dataOut[0],
                                                rowIndex);
                        }
                    }
                }
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
            try {
                StructureEnum::Enum structure = StructureEnum::INVALID;
                int32_t numberOfVertices = -1;
                int32_t vertexIndex = -1;
                mapFileDataSelector.getSurfaceVertex(structure,
                                                     numberOfVertices,
                                                     vertexIndex);
                
                if (getMappingSurfaceNumberOfNodes(structure) == numberOfVertices) {
                    if ( ! getSeriesDataForSurfaceNode(structure,
                                                    vertexIndex,
                                                    dataOut)) {
                        dataOut.clear();
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
                
                if (getMappingSurfaceNumberOfNodes(structure) == surfaceNumberOfVertices) {
                    const int32_t numberOfVertices = static_cast<int32_t>(vertexIndices.size());
                    if (numberOfVertices > 0) {
                        std::vector<double> dataSum;
                        int32_t dataSumSize = 0;
                        int32_t dataAverageCount = 0;
                        
                        std::vector<float> data;
                        bool firstNodeFlag = true;
                        
                        for (int32_t i = 0; i < numberOfVertices; i++) {
                            if (getSeriesDataForSurfaceNode(structure,
                                                            vertexIndices[i],
                                                            data)) {
                                if (firstNodeFlag) {
                                    firstNodeFlag = false;
                                    
                                    dataSumSize = static_cast<int32_t>(data.size());
                                    if (dataSumSize > 0) {
                                        dataSum.resize(dataSumSize,
                                                       0.0);
                                    }
                                }
                                
                                CaretAssert(dataSumSize == static_cast<int32_t>(data.size()));
                                
                                for (int32_t j = 0; j < dataSumSize; j++) {
                                    dataSum[j] += data[j];
                                }
                                dataAverageCount++;
                            }
                        }
                        
                        if ((dataAverageCount > 0)
                            && (dataSumSize > 0)) {
                            dataOut.resize(dataSumSize);
                            for (int32_t k = 0; k < dataSumSize; k++) {
                                dataOut[k] = dataSum[k] / dataAverageCount;
                            }
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
        {
            float xyz[3];
            mapFileDataSelector.getVolumeVoxelXYZ(xyz);
            getSeriesDataForVoxelAtCoordinate(xyz,
                                              dataOut);
        }
            break;
    }
}

/**
 * @return Pointer to mapping of data to brainordinates.
 *         Will be NULL if data does not map to brainordinates.
 */
const CiftiBrainModelsMap*
CiftiMappableDataFile::getBrainordinateMapping() const
{
    if ( ! m_brainordinateMappingCachedFlag) {
        m_brainordinateMappingCachedFlag = true;
        
        switch (m_dataMappingAccessMethod) {
            case DATA_ACCESS_METHOD_INVALID:
                CaretAssert(0);
                break;
            case DATA_ACCESS_NONE:
                break;
            case DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN:
            case DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW:
            {
                const CiftiXML& myXML = m_ciftiFile->getCiftiXML();
                if (myXML.getMappingType(m_dataMappingDirectionForCiftiXML) == CiftiMappingType::BRAIN_MODELS) {
                    /*
                     * Cache a COPY of the CiftiBrainModelsMap to avoid calling CiftiFile::getCiftiXML() many times
                     */
                    m_brainordinateMapping.reset(new CiftiBrainModelsMap(myXML.getBrainModelsMap(m_dataMappingDirectionForCiftiXML)));
                }
            }
                break;
        }
    }
    
    return m_brainordinateMapping.get();
}

/**
 * Are all brainordinates in this file also in the given file?
 * That is, the brainordinates are equal to or a subset of the brainordinates
 * in the given file.
 *
 * @param mapFile
 *     The given map file.
 * @return
 *     Match status.
 */
CaretMappableDataFile::BrainordinateMappingMatch
CiftiMappableDataFile::getBrainordinateMappingMatch(const CaretMappableDataFile* mapFile) const
{
    CaretAssert(mapFile);
    if (this == mapFile) {
        return BrainordinateMappingMatch::EQUAL;
    }
    
    const CiftiMappableDataFile* otherCiftiFile = dynamic_cast<const CiftiMappableDataFile*>(mapFile);
    if (otherCiftiFile == NULL) {
        return BrainordinateMappingMatch::NO;
    }
    
    const CiftiBrainModelsMap* myBrainMap = getBrainordinateMapping();
    if (myBrainMap != NULL) {
        const CiftiBrainModelsMap* otherBrainMap = otherCiftiFile->getBrainordinateMapping();
        if (otherBrainMap != NULL) {
            switch (myBrainMap->testMatch(*otherBrainMap)) {
                case CiftiBrainModelsMap::MatchResult::EQUAL:
                    return BrainordinateMappingMatch::EQUAL;
                    break;
                case CiftiBrainModelsMap::MatchResult::NO:
                    return BrainordinateMappingMatch::NO;
                    break;
                case CiftiBrainModelsMap::MatchResult::SUBSET:
                    return BrainordinateMappingMatch::SUBSET;
                    break;
            }
        }
    }
    
    return BrainordinateMappingMatch::NO;
}



///* ========================================================================== */

/**
 * Constructor.
 *
 * @param ciftiMappableDataFile
 *    CIFTI file containing this map
 * @param ciftiFile
 *    The CIFTI data file
 * @param fileMapDataType
 *    Type of CIFTI file (matrix or multi-map).
 * @param readingDirectionForCiftiXML
 *    Reading direction for CIFTI XML access
 * @param mappingDirectionForCiftiXML
 *    Mapping direction for CIFTI XML access
 * @param mapIndex
 *    Index of this map.
 */
CiftiMappableDataFile::MapContent::MapContent(CiftiMappableDataFile* ciftiMappableDataFile,
                                              CiftiFile* ciftiFile,
                                                 const FileMapDataType fileMapDataType,
                                                 const int32_t readingDirectionForCiftiXML,
                                                 const int32_t mappingDirectionForCiftiXML,
                                                 const int32_t mapIndex)
: CaretObjectTracksModification(),
m_ciftiMappableDataFile(ciftiMappableDataFile),
m_ciftiFile(ciftiFile),
m_fileMapDataType(fileMapDataType),
m_readingDirectionForCiftiXML(readingDirectionForCiftiXML),
m_mappingDirectionForCiftiXML(mappingDirectionForCiftiXML),
m_mapIndex(mapIndex)
{
    CaretAssert(ciftiFile);
    CaretAssert(mapIndex >= 0);
    
    m_fastStatistics.grabNew(NULL);
    m_histogram.grabNew(NULL);
    m_histogramLimitedValues.grabNew(NULL);
    
    m_metadata = NULL;
    m_paletteColorMapping = NULL;
    m_labelTable = NULL;
    
    m_dataCount = 0;
    m_rgbaValid = false; 
    m_dataIsMappedWithLabelTable = false;
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    
    switch (m_mappingDirectionForCiftiXML) {
        case CiftiXML::ALONG_COLUMN:
            m_dataCount = ciftiFile->getNumberOfRows();
            break;
        case CiftiXML::ALONG_ROW:
            m_dataCount = ciftiFile->getNumberOfColumns();
            break;
        case CiftiXML::ALONG_STACK:
            break;
    }
    
    switch (m_fileMapDataType) {
        case FILE_MAP_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case FILE_MAP_DATA_TYPE_MATRIX:
            m_metadata = ciftiXML.getFileMetaData();
            m_paletteColorMapping = ciftiXML.getFilePalette();
            break;
        case FILE_MAP_DATA_TYPE_MULTI_MAP:
            switch (ciftiXML.getMappingType(m_readingDirectionForCiftiXML)) {
                case CiftiMappingType::BRAIN_MODELS:
                {
                    CaretAssert(0);
                }
                    break;
                case CiftiMappingType::LABELS:
                {
                    const CiftiLabelsMap& map = ciftiXML.getLabelsMap(m_readingDirectionForCiftiXML);
                    m_dataIsMappedWithLabelTable = true;
                    m_metadata = map.getMapMetadata(mapIndex);
                    CaretAssert(m_metadata);
                    m_labelTable = map.getMapLabelTable(mapIndex);
                    CaretAssert(m_labelTable);
                    
                    m_mapName = map.getMapName(m_mapIndex);
                }
                    break;
                case CiftiMappingType::PARCELS:
                {
                    CaretAssert(0);
                }
                    break;
                case CiftiMappingType::SCALARS:
                {
                    const CiftiScalarsMap& map = ciftiXML.getScalarsMap(m_readingDirectionForCiftiXML);
                    m_metadata = map.getMapMetadata(mapIndex);
                    CaretAssert(m_metadata);
                    m_paletteColorMapping = map.getMapPalette(mapIndex);
                    CaretAssert(m_paletteColorMapping);
                    
                    m_mapName = map.getMapName(m_mapIndex);
                }
                    break;
                case CiftiMappingType::SERIES:
                {
                    /*
                     * Series Data has no map metadata but still need valid metadata instance
                     */
                    m_metadataForMapsWithNoMetaData.grabNew(new GiftiMetaData());
                    m_metadata = m_metadataForMapsWithNoMetaData;

                    /*
                     * Series data usings the file's palette
                     */
                    m_paletteColorMapping = ciftiXML.getFilePalette();
                    CaretAssert(m_paletteColorMapping);

                    /*
                     * Data series do not have map names but the map name is
                     * a function of units and map index.
                     */
                    const CiftiSeriesMap& map = ciftiXML.getSeriesMap(m_readingDirectionForCiftiXML);
                    AString unitsSuffix;
                    switch (map.getUnit()) {
                        case CiftiSeriesMap::HERTZ:
                            unitsSuffix = " hertz";
                            break;
                        case CiftiSeriesMap::METER:
                            unitsSuffix = " meters";
                            break;
                        case CiftiSeriesMap::RADIAN:
                            unitsSuffix = " radians";
                            break;
                        case CiftiSeriesMap::SECOND:
                            unitsSuffix = " seconds";
                            break;
                            
                    }
                    
                    if (unitsSuffix.isEmpty()) {
                        m_mapName = ("Map Index: "
                                   + AString::number(m_mapIndex + 1));
                    }
                    else {
                        const float value = (map.getStart()
                                             + (m_mapIndex * map.getStep()));
                        m_mapName = (AString::number(value)
                                   + unitsSuffix);
                    }
                }
                break;
            }
            break;
    }
}

/**
 * Destructor.
 */
CiftiMappableDataFile::MapContent::~MapContent()
{
    /**
     * Do not delete these as they point to data in CIFTI XML:
     *   m_labelTable
     *   m_paletteColorMapping
     *   m_metadata;
     */
}

/**
 * Clear the modification status of this map.
 */
void
CiftiMappableDataFile::MapContent::clearModified()
{
    CaretObjectTracksModification::clearModified();
    
    if (m_labelTable != NULL) {
        m_labelTable->clearModified();
    }
    
    m_metadata->clearModified();
    
    if (m_paletteColorMapping != NULL) {
        m_paletteColorMapping->clearModified();
    }
}

/**
 * @return Modification status.
 *
 * DOES NOT include modification status of palette.
 */
bool
CiftiMappableDataFile::MapContent::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    if (m_labelTable != NULL) {
        if (m_labelTable->isModified()) {
            return true;
        }
    }

    if (m_metadata->isModified()) {
        return true;
    }

    /* DO NOT include palette color mapping status ! */
    
    return false;
}

/**
 * @return Name of the map.
 */
AString
CiftiMappableDataFile::MapContent::getName() const
{
    return m_mapName;
}

/**
 * Set the name of the map.
 * 
 * @param name
 *     New name for map.
 */
void
CiftiMappableDataFile::MapContent::setName(const AString& name)
{
    if (name == m_mapName) {
        return;
    }
    
    m_mapName = name;
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    switch (ciftiXML.getMappingType(m_readingDirectionForCiftiXML)) {
        case CiftiMappingType::BRAIN_MODELS:
        {
            CaretAssert(0);
        }
            break;
        case CiftiMappingType::LABELS:
        {
            const CiftiLabelsMap& map = ciftiXML.getLabelsMap(m_readingDirectionForCiftiXML);
            map.setMapName(m_mapIndex,
                           m_mapName);
            setModified();
        }
            break;
        case CiftiMappingType::PARCELS:
        {
            CaretAssert(0);
        }
            break;
        case CiftiMappingType::SCALARS:
        {
            const CiftiScalarsMap& map = ciftiXML.getScalarsMap(m_readingDirectionForCiftiXML);
            map.setMapName(m_mapIndex,
                           m_mapName);
            setModified();
        }
            break;
        case CiftiMappingType::SERIES:
            /* 
             * Data series do not have map names but the map name is
             * a function of units and map index.  
             */
            break;
    }
}


/**
 * Invalidate the coloring (usually due to palette or data changes).
 */
void
CiftiMappableDataFile::MapContent::updateForChangeInMapData()
{
    m_fastStatistics.grabNew(NULL);
    m_histogram.grabNew(NULL);
    m_histogramLimitedValues.grabNew(NULL);    
    m_rgbaValid = false;
}

/**
 * @return True if fast statistics is valid, else false.
 */
bool
CiftiMappableDataFile::MapContent::isFastStatisticsValid() const
{
    return (m_fastStatistics != NULL);
}

/**
 * Update the Fast Statistics but only when needed.
 *
 * @param data
 *     Data for fast statistics.
 */
void
CiftiMappableDataFile::MapContent::updateFastStatistics(const std::vector<float>& data)
{
    if (data.empty()) {
        m_fastStatistics.grabNew(NULL);
    }
    else {
        if (m_fastStatistics == NULL) {
            m_fastStatistics.grabNew(new FastStatistics());
            m_fastStatistics->update(&data[0],
                                     data.size());
        }
    }
}

/**
 * @param numberOfBuckets,
 *    Number of buckets in the histogram.
 * @return True if histogram is valid, else false.
 */
bool
CiftiMappableDataFile::MapContent::isHistogramValid(const int32_t numberOfBuckets) const
{
    return ((m_histogram != NULL)
            && (m_histogramNumberOfBuckets == numberOfBuckets));
}

/**
 * Update the Histogram but only when needed.
 *
 * @param numberOfBuckets,
 *    Number of buckets in the histogram.
 * @param data
 *     Data for histogram.
 */
void
CiftiMappableDataFile::MapContent::updateHistogram(const int32_t numberOfBuckets,
                                                   const std::vector<float>& data)
{
    if (data.empty()) {
        m_histogram.grabNew(NULL);
    }
    else {
        CaretAssert(m_paletteColorMapping);
        if (m_histogram == NULL) {
            m_histogram.grabNew(new Histogram(numberOfBuckets));
        }
        m_histogram->update(numberOfBuckets,
                            &data[0],
                            data.size());
        m_histogramNumberOfBuckets = numberOfBuckets;
    }
}

/**
 * Is limited values histogram valid?  Is is valid when it exists and
 * the limited value parameters have not changed.
 *
 * @param numberOfBuckets,
 *    Number of buckets in the histogram.
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 *
 * @return true if valid, else false.
 */
bool
CiftiMappableDataFile::MapContent::isHistogramLimitedValuesValid(const int32_t numberOfBuckets,
                                                                 const float mostPositiveValueInclusive,
                                                                 const float leastPositiveValueInclusive,
                                                                 const float leastNegativeValueInclusive,
                                                                 const float mostNegativeValueInclusive,
                                                                 const bool includeZeroValues) const
{
    if (m_histogramLimitedValues == NULL) {
        return false;
    }
    else if ((numberOfBuckets != m_histogramLimitedValuesNumberOfBuckets)
             || (mostPositiveValueInclusive != m_histogramLimitedValuesMostPositiveValueInclusive)
             || (leastPositiveValueInclusive != m_histogramLimitedValuesLeastPositiveValueInclusive)
             || (leastNegativeValueInclusive != m_histogramLimitedValuesLeastNegativeValueInclusive)
             || (mostNegativeValueInclusive != m_histogramLimitedValuesMostNegativeValueInclusive)
             || (includeZeroValues != m_histogramLimitedValuesIncludeZeroValues)) {
        return false;
    }

    return true;
}

/**
 * Update the Histogram for limited values.
 *
 * @param data
 *     Data for histogram.
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 */
void
CiftiMappableDataFile::MapContent::updateHistogramLimitedValues(const int32_t numberOfBuckets,
                                                                const std::vector<float>& data,
                                                                const float mostPositiveValueInclusive,
                                                                const float leastPositiveValueInclusive,
                                                                const float leastNegativeValueInclusive,
                                                                const float mostNegativeValueInclusive,
                                                                const bool includeZeroValues)
{
    if (data.empty()) {
        m_histogramLimitedValues.grabNew(NULL);
    }
    else {
        if (m_histogramLimitedValues == NULL) {
            m_histogramLimitedValues.grabNew(new Histogram());
        }
        m_histogramLimitedValues->update(numberOfBuckets,
                                         &data[0],
                                         data.size(),
                                         mostPositiveValueInclusive,
                                         leastPositiveValueInclusive,
                                         leastNegativeValueInclusive,
                                         mostNegativeValueInclusive,
                                         includeZeroValues);
        m_histogramLimitedValuesNumberOfBuckets = numberOfBuckets;
        m_histogramLimitedValuesMostPositiveValueInclusive = mostPositiveValueInclusive;
        m_histogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
        m_histogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
        m_histogramLimitedValuesMostNegativeValueInclusive = mostNegativeValueInclusive;
        m_histogramLimitedValuesIncludeZeroValues = includeZeroValues;
    }
}

/**
 * Get the thresholding data from the given thresholding file in a vector that
 * has the same brainordinate mapping as this file.
 *
 * @param threshMapFile
 *     The thresholding file.
 * @param threshMapIndex
 *     The map index in the thresholding file.
 * @param thresholdDataOut
 *     Output containing data for thresholding a map in this file.
 */
bool
CiftiMappableDataFile::MapContent::getThresholdData(const CaretMappableDataFile* threshMapFile,
                                                    const int32_t threshMapIndex,
                                                    std::vector<float>& thresholdDataOut)
{
    CaretAssert(threshMapFile);
    CaretAssert(threshMapIndex >= 0);
    const CiftiMappableDataFile* thresholdCiftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(threshMapFile);
    CaretAssert(thresholdCiftiMapFile);
    
    thresholdDataOut.resize(m_dataCount);
    
    switch (m_ciftiMappableDataFile->getBrainordinateMappingMatch(threshMapFile)) {
        case BrainordinateMappingMatch::EQUAL:
            thresholdCiftiMapFile->getMapData(threshMapIndex,
                                              thresholdDataOut);
            break;
        case BrainordinateMappingMatch::NO:
            CaretAssert(0); /* should never happen */
            break;
        case BrainordinateMappingMatch::SUBSET:
        {
            /*
             * Since this file is a "subset" of the other file, will need to
             * data using the structures in each file.
             */
            std::vector<float> thresholdingFileMapData;
            thresholdCiftiMapFile->getMapData(threshMapIndex, thresholdingFileMapData);
            
            const CiftiBrainModelsMap* threshBrainMap = thresholdCiftiMapFile->getBrainordinateMapping();
            CaretAssert(threshBrainMap);
            const CiftiBrainModelsMap* dataBrainMap = m_ciftiMappableDataFile->getBrainordinateMapping();
            CaretAssert(dataBrainMap);
            
            const std::vector<CiftiBrainModelsMap::ModelInfo>& dataModelsMap   = dataBrainMap->getModelInfo();
            const std::vector<CiftiBrainModelsMap::ModelInfo>& threshModelsMap = threshBrainMap->getModelInfo();
            for (const auto& dataModelsInfo : dataModelsMap) {
                const StructureEnum::Enum structure = dataModelsInfo.m_structure;
                for (const auto& threshModelsInfo : threshModelsMap) {
                    if (structure == threshModelsInfo.m_structure) {
                        CaretAssert(dataModelsInfo.m_indexCount == threshModelsInfo.m_indexCount);
                        CaretAssertVectorIndex(thresholdingFileMapData, (threshModelsInfo.m_indexStart + threshModelsInfo.m_indexCount) - 1);
                        CaretAssertVectorIndex(thresholdDataOut, (dataModelsInfo.m_indexStart + dataModelsInfo.m_indexCount) - 1);
                        std::copy_n(&thresholdingFileMapData[threshModelsInfo.m_indexStart],
                                    threshModelsInfo.m_indexCount,
                                    &thresholdDataOut[dataModelsInfo.m_indexStart]);
                        break;
                    }
                }
            }
        }
            break;
    }
    
    return true;
}

/**
 * Update coloring for this map.  If the paletteFile is NOT NULL,
 * color using a palette; otherwise, color with label table.
 *
 * @param data
 *    Data contained in the map.
 * @param fastStatistics
 *    Fast statistics used for palette coloring.  While map content contains
 *    a fast statistics member, it is calculated on the data within the map.
 *    However, some files need the statistics calculated on the entire file
 *    so that a particular data value from one map is colored exactly the
 *    same as the particular data value in another map (the user may have
 *    a min/max coloring selected and the two maps may a have different
 *    min/max values).
 */
void
CiftiMappableDataFile::MapContent::updateColoring(const std::vector<float>& data,
                                                  const FastStatistics* fastStatistics)
{
    if (data.empty()) {
        return;
    }
    
    if (m_rgbaValid) {
        return;
    }
    
    if (m_dataCount != static_cast<int64_t>(data.size())) {
        m_dataCount = static_cast<int64_t>(data.size());
    }
    const uint64_t rgbaCount = m_dataCount * 4;
    if (m_rgba.size() != rgbaCount) {
        m_rgba.resize(rgbaCount, 0);
        
    }
    
    if (m_dataIsMappedWithLabelTable) {
        NodeAndVoxelColoring::colorIndicesWithLabelTable(m_labelTable,
                                                         &data[0],
                                                         data.size(),
                                                         &m_rgba[0]);
    }
    else {
        CaretAssert(m_paletteColorMapping);
        if (fastStatistics != NULL) {
            bool useThreshMapFileFlag = false;
            switch (m_paletteColorMapping->getThresholdType()) {
                case PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE:
                    useThreshMapFileFlag = true;
                    break;
                case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                    break;
                case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                    break;
                case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
                    break;
                case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
                    break;
            }
            
            if (useThreshMapFileFlag) {
                CaretAssert(m_ciftiMappableDataFile);
                const CaretMappableDataFileAndMapSelectionModel* threshFileModel = m_ciftiMappableDataFile->getMapThresholdFileSelectionModel(m_mapIndex);
                CaretAssert(threshFileModel);
                const CaretMappableDataFile* threshMapFile = threshFileModel->getSelectedFile();
                if (threshMapFile != NULL) {
                    const int32_t threshMapIndex = threshFileModel->getSelectedMapIndex();
                    std::vector<float> thresholdData;
                    getThresholdData(threshMapFile,
                                     threshMapIndex,
                                     thresholdData);
                    CaretAssert(threshMapFile);
                    CaretAssert(threshMapIndex >= 0);
                    PaletteColorMapping* thresholdPaletteColorMapping = const_cast<PaletteColorMapping*>(threshMapFile->getMapPaletteColorMapping(threshMapIndex));
                    CaretAssert(thresholdPaletteColorMapping);
                    CaretAssert(data.size() == thresholdData.size());
                    NodeAndVoxelColoring::colorScalarsWithPalette(fastStatistics,
                                                                  m_paletteColorMapping,
                                                                  &data[0],
                                                                  thresholdPaletteColorMapping,
                                                                  &thresholdData[0],
                                                                  m_dataCount,
                                                                  &m_rgba[0]);
                }
                else {
                    NodeAndVoxelColoring::colorScalarsWithPalette(fastStatistics,
                                                                  m_paletteColorMapping,
                                                                  &data[0],
                                                                  m_paletteColorMapping,
                                                                  &data[0],
                                                                  m_dataCount,
                                                                  &m_rgba[0]);
                }
            }
            else {
                NodeAndVoxelColoring::colorScalarsWithPalette(fastStatistics,
                                                              m_paletteColorMapping,
                                                              &data[0],
                                                              m_paletteColorMapping,
                                                              &data[0],
                                                              m_dataCount,
                                                              &m_rgba[0]);
            }
        }
        else {
            std::fill(m_rgba.begin(),
                      m_rgba.end(),
                      0);
        }
    }
    
    m_rgbaValid = true;
}

bool CiftiMappableDataFile::hasCiftiXML() const
{
    return true;
}

const CiftiXML CiftiMappableDataFile::getCiftiXML() const
{
    if (m_ciftiFile != NULL)
    {
        return m_ciftiFile->getCiftiXML();
    }
    return CiftiXML();//this is why the function doesn't return a reference: must return something even when it doesn't have a CiftiXML allocated - could make it a pointer and return NULL
}
