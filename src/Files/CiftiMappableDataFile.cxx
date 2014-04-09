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

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiFacade.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiFile.h"
#include "CiftiInterface.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CaretTemporaryFile.h"
#include "CiftiXnat.h"
#include "CiftiXML.h"
#include "DataFileContentInformation.h"
#include "DescriptiveStatistics.h"
#include "EventManager.h"
#include "EventPaletteGetByName.h"
#include "FastStatistics.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GroupAndNameHierarchyModel.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
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
 * @param fileReading
 *    How to read data from the file 
 * @param rowIndexType
 *    Type of CIFTI indexing along the rows (dimension 0) for the file.
 * @param columnIndexType
 *    Type of CIFTI indexing along the columns (dimension 1) for the file.
 * @param brainordinateMappedDataAccess
 *    Location of the brainordinate data
 * @param seriesDataAccess
 *    Location of the series data
 */
CiftiMappableDataFile::CiftiMappableDataFile(const DataFileTypeEnum::Enum dataFileType,
                                             const FileReading fileReading,
                                             const IndicesMapToDataType rowIndexType,
                                             const IndicesMapToDataType columnIndexType,
                                             const DataAccess brainordinateMappedDataAccess,
                                             const DataAccess seriesDataAccess)
: CaretMappableDataFile(dataFileType),
m_fileReading(fileReading),
m_requiredRowIndexType(rowIndexType),
m_requiredColumnIndexType(columnIndexType),
m_brainordinateMappedDataAccess(brainordinateMappedDataAccess),
m_seriesDataAccess(seriesDataAccess)
{
    m_classNameHierarchy.grabNew(new GroupAndNameHierarchyModel());
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    m_ciftiFacade.grabNew(NULL);
    m_ciftiInterface.grabNew(NULL);
    m_metadata.grabNew(new GiftiMetaData());
    m_voxelIndicesToOffset.grabNew(NULL);
    clearPrivate();
}

/**
 * Destructor.
 */
CiftiMappableDataFile::~CiftiMappableDataFile()
{
    clearPrivate();
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
         * Add the CiftiFile to the Cifti Mappable File
         */
        const AString defaultFileName = ciftiMappableFile->getFileName();
        ciftiMappableFile->initializeFromCiftiInterface(ciftiFile,
                                                     defaultFileName);
        ciftiMappableFile->setModified();
        
        return ciftiMappableFile;
    }
    catch (const CiftiFileException& ce) {
        errorMessageOut = ce.whatString();
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
    m_ciftiFacade.grabNew(NULL);
    m_ciftiInterface.grabNew(NULL);
    m_metadata->clear();
    m_voxelIndicesToOffset.grabNew(NULL);

    const int64_t num = static_cast<int64_t>(m_mapContent.size());
    for (int64_t i = 0; i < num; i++) {
        delete m_mapContent[i];
    }
    m_mapContent.clear();
    m_containsVolumeData  = false;
    
    m_volumeDimensions[0] = 0;
    m_volumeDimensions[1] = 0;
    m_volumeDimensions[2] = 0;
    m_volumeDimensions[3] = 0;
    m_volumeDimensions[4] = 0;
    
    m_classNameHierarchy->clear();
    m_forceUpdateOfGroupAndNameHierarchy = true;

    m_niftiHeaderDimensions.clear();
    m_niftiDataType = NiftiDataTypeEnum::NIFTI_TYPE_INVALID;
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
    
//    switch (m_brainordinateMappedDataAccess) {
//        case DATA_ACCESS_INVALID:
//            break;
//        case DATA_ACCESS_WITH_COLUMN_METHODS:
//            numCiftiNodes = m_ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
//            break;
//        case DATA_ACCESS_WITH_ROW_METHODS:
//            numCiftiNodes = m_ciftiInterface->getRowSurfaceNumberOfNodes(structure);
//            break;
//    }
    
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
    return m_metadata;
}

/**
 * @return Metadata for the file.
 */
const GiftiMetaData*
CiftiMappableDataFile:: getFileMetaData() const
{
    return m_metadata;
}

/**
 * Read the file.
 *
 * @param filename
 *    Name of the file to read.
 * @throw
 *    DataFileException if there is an error reading the file.
 */
void
CiftiMappableDataFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();
    
    try {
        
        CiftiInterface* ciftiInterface = NULL;
        
        /*
         * Is the file on the network (name begins with http, ftp, etc.).
         */
        if (DataFile::isFileOnNetwork(filename)) {
            /*
             * Data in Xnat does not end with a valid file extension
             * but ends with HTTP search parameters.  Thus, if the
             * filename does not have a valid extension, assume that
             * the data is in Xnat.
             */
            bool isValidFileExtension = false;
            DataFileTypeEnum::fromName(filename,
                                       &isValidFileExtension);
            
            if (isValidFileExtension) {
                switch (m_fileReading) {
                    case FILE_READ_DATA_ALL:
                        break;
                    case FILE_READ_DATA_AS_NEEDED:
                        throw DataFileException(filename
                                                + " of type "
                                                + DataFileTypeEnum::toGuiName(getDataFileType())
                                                + " cannot be read over the network.  The file must be"
                                                " accessed by reading individual rows and/or columns"
                                                " and this cannot be performed over a network.");
                        break;
                }
                
                CaretTemporaryFile tempFile;
                tempFile.readFile(filename);
                
                CiftiFile* ciftiFile = new CiftiFile();
                ciftiFile->openFile(tempFile.getFileName(),
                                    IN_MEMORY);
                ciftiInterface = ciftiFile;
            }
            else {
                CiftiXnat* ciftiXnat = new CiftiXnat();
                AString username = "";
                AString password = "";
                AString filenameToOpen = "";
                
                /*
                 * Username and password may be embedded in URL, so extract them.
                 */
                FileInformation fileInfo(filename);
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
                
                ciftiXnat->setAuthentication(filenameToOpen,
                                             username,
                                             password);
                ciftiXnat->openURL(filenameToOpen);
                ciftiInterface = ciftiXnat;
            }
        }
        else {
            CiftiFile* ciftiFile = new CiftiFile();
            switch (m_fileReading) {
                case FILE_READ_DATA_ALL:
                    ciftiFile->openFile(filename,
                                        IN_MEMORY);
                    break;
                case FILE_READ_DATA_AS_NEEDED:
                    ciftiFile->openFile(filename,
                                        ON_DISK);
                    break;
            }
            ciftiInterface = ciftiFile;
        }
        
        if (ciftiInterface != NULL) {
            initializeFromCiftiInterface(ciftiInterface,
                                         filename);
        }
    }
    catch (CiftiFileException& e) {
        clear();
        throw DataFileException(e.whatString());
    }
    catch (DataFileException& e) {
        clear();
        throw e;
    }
}
/**
 * Initialize from a CIFTI Interface after either reading a CIFTI file
 * or creating a CIFTI file.
 *
 * @param ciftiInterface
 *     Typically a subclass such as CiftiFile ro CiftiXnat.
 * @param filename
 *     Name of the file.
 */
void
CiftiMappableDataFile::initializeFromCiftiInterface(CiftiInterface* ciftiInterface,
                                                    const AString& filename) throw (DataFileException)
{
    CaretAssert(ciftiInterface);
    
    try {
        /*
         * Need a pointer to the CIFTI XML.
         */
        m_ciftiInterface.grabNew(ciftiInterface);
        CaretAssert(m_ciftiInterface);
        const CiftiXMLOld& ciftiXML = m_ciftiInterface->getCiftiXMLOld();
        
        setFileName(filename);
        
        /*
         * Create the CIFTI facade for simplified access to CIFTI data
         */
        m_ciftiFacade.grabNew(new CiftiFacade(getDataFileType(),
                                              m_ciftiInterface));
        AString errorMessage;
        if (m_ciftiFacade->isValidCiftiFile() == false) {
            errorMessage.appendWithNewLine("Support for "
                                           + DataFileTypeEnum::toName(getDataFileType())
                                           + " needs to be implemented or is invalid type.");
        }
        
        /*
         * Get contents of the matrix.
         */
        IndicesMapToDataType rowIndexTypeInFile = ciftiXML.getMappingType(CiftiXMLOld::ALONG_ROW);
        AString rowIndexTypeName = ciftiIndexTypeToName(rowIndexTypeInFile);
        
        const IndicesMapToDataType columnIndexTypeInFile = ciftiXML.getMappingType(CiftiXMLOld::ALONG_COLUMN);
        AString columnIndexTypeName = ciftiIndexTypeToName(columnIndexTypeInFile);
        
        /*
         * There are some Scalar and data series files that get
         * row index type mixed up
         */
        if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR) {
            if (rowIndexTypeInFile == CIFTI_INDEX_TYPE_TIME_POINTS) {
                CaretLogSevere(getFileNameNoPath()
                                + " has row index type="
                                + rowIndexTypeName
                                + " which is incorrect for this CIFTI Scalar File and has been converted to "
                                + ciftiIndexTypeToName(CIFTI_INDEX_TYPE_SCALARS));
                rowIndexTypeInFile = CIFTI_INDEX_TYPE_SCALARS;
            }
        }
        else if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES) {
            if (rowIndexTypeInFile == CIFTI_INDEX_TYPE_SCALARS) {
                CaretLogSevere(getFileNameNoPath()
                                + " has row index type="
                                + rowIndexTypeName
                                + " which is incorrect for this CIFTI Data Series File and has been converted to "
                                + ciftiIndexTypeToName(CIFTI_INDEX_TYPE_TIME_POINTS));
                rowIndexTypeInFile = CIFTI_INDEX_TYPE_TIME_POINTS;
            }
        }
        
        /*
         * Validate type of data in rows and columns
         */
        if (m_requiredRowIndexType != rowIndexTypeInFile) {
            errorMessage.appendWithNewLine("Row Index Type should be "
                                           + ciftiIndexTypeToName(m_requiredRowIndexType)
                                           + " but is "
                                           + rowIndexTypeName);
        }
        if (m_requiredColumnIndexType != columnIndexTypeInFile) {
            errorMessage.appendWithNewLine("Column Index Type should be "
                                           + ciftiIndexTypeToName(m_requiredColumnIndexType)
                                           + " but is "
                                           + columnIndexTypeName);
        }
        
        
        if (m_ciftiFacade->isBrainordinateDataColoredWithLabelTable()) {
            const int32_t numberOfMaps = m_ciftiFacade->getNumberOfMaps();
            for (int32_t i = 0; i < numberOfMaps; i++) {
                if (m_ciftiFacade->getLabelTableForMapOrSeriesIndex(i) == NULL) {
                    throw DataFileException(getFileNameNoPath()
                                            + " label table is missing.  This CIFTI file probably "
                                            "has invalid row/column mapping type(s). ");
                }
            }
        }
        else if (m_ciftiFacade->isBrainordinateDataColoredWithPalette()) {
            const int32_t numberOfMaps = m_ciftiFacade->getNumberOfMaps();
            for (int32_t i = 0; i < numberOfMaps; i++) {
                if (m_ciftiFacade->getPaletteColorMappingForMapOrSeriesIndex(i) == NULL) {
                    throw DataFileException(getFileNameNoPath()
                                            + " palette color mapping is missing.  This CIFTI file probably "
                                            "has invalid row/column mapping type(s). ");
                }
            }
        }
        else {
            errorMessage.appendWithNewLine("Data is neither color with label table nor palette.");
        }
        
        if (errorMessage.isEmpty() == false) {
            errorMessage = (getFileNameNoPath()
                            + errorMessage);
            clear();
            throw DataFileException(errorMessage);
        }
        
        /*
         * Copy the file metadata into a GiftiMetaData object.
         */
        m_ciftiFacade->getFileMetadata(m_metadata);

        /*
         * Get data for maps.
         */
        const int32_t numberOfMaps = m_ciftiFacade->getNumberOfMaps();
        for (int32_t i = 0; i < numberOfMaps; i++) {
            MapContent* mc = new MapContent(m_ciftiFacade,
                                            i);
            m_mapContent.push_back(mc);
        }
        
        /*
         * Setup voxel mapping
         */
        const std::vector<CiftiBrainModelsMap::VolumeMap>* voxelMapping =
        m_ciftiFacade->getVolumeMapForMappingDataToBrainordinates();
        if (voxelMapping != NULL) {
            m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer(m_ciftiInterface,
                                                                   *voxelMapping));
        }
        else {
            std::vector<CiftiBrainModelsMap::VolumeMap> emptyVoxelMapping;
            m_voxelIndicesToOffset.grabNew(new SparseVolumeIndexer(m_ciftiInterface,
                                                                   emptyVoxelMapping));
        }
        
        
        /*
         * Indicate if volume mappable
         */
        if (m_voxelIndicesToOffset->isValid()) {
            m_containsVolumeData = true;
            
            VolumeSpace::OrientTypes orient[3];
            int64_t dimensions[3];
            float origin[3];
            float spacing[3];
            if (ciftiXML.getVolumeAttributesForPlumb(orient,
                                                     dimensions,
                                                     origin,
                                                     spacing)) {
                m_volumeDimensions[0] = dimensions[0];
                m_volumeDimensions[1] = dimensions[1];
                m_volumeDimensions[2] = dimensions[2];
                m_volumeDimensions[3] = 1;
                m_volumeDimensions[4] = 1;
            }
        }
        
        AString mapNames;
        if (hasMapAttributes()) {
            for (int32_t i = 0; i < getNumberOfMaps(); i++) {
                mapNames.appendWithNewLine("        Map "
                                           + AString::number(i)
                                           + " Name: "
                                           + getMapName(i));
            }
            if (mapNames.isEmpty() == false) {
                mapNames.insert(0, "\n");
            }
        }
        
        /*
         * Map units
         */
        float startValue;
        float stepValue;
        NiftiTimeUnitsEnum::Enum units;
        m_ciftiFacade->getMapIntervalStartStepAndUnits(startValue,
                                                       stepValue,
                                                       units);
        const AString unitString(NiftiTimeUnitsEnum::toName(units)
                                 + ", "
                                 + AString::number(startValue)
                                 + ", "
                                 + AString::number(stepValue));
        
        const int64_t voxelCount = ((voxelMapping != NULL)
                                    ? voxelMapping->size()
                                    : 0);
        
        const AString msg = (getFileNameNoPath()
                             + "\n   " + DataFileTypeEnum::toGuiName(getDataFileType())
                             + "\n   Rows: " + AString::number(m_ciftiFacade->getNumberOfRows())
                             + "\n   Columns: " + AString::number(m_ciftiFacade->getNumberOfColumns())
                             + "\n   RowType: " + rowIndexTypeName
                             + "\n   ColType: " + columnIndexTypeName
                             + "\n   Has Surface Data: " + AString::fromBool(m_ciftiFacade->containsSurfaceDataForMappingToBrainordinates())
                             + "\n   Has Volume Data: " + AString::fromBool(m_containsVolumeData)
                             + "\n   Voxel Count: " + AString::number(voxelCount)
                             + "\n   Volume Dimensions: " + AString::fromNumbers(m_volumeDimensions, 5, ",")
                             + "\n   Number of Maps: " + AString::number(m_mapContent.size())
                             + mapNames
                             + "\n   Map Units, Start, Stop: " + unitString
                             + "\n   Map with Label Table: " + AString::fromBool(m_ciftiFacade->isBrainordinateDataColoredWithLabelTable())
                             + "\n   Map With Palette: " + AString::fromBool(m_ciftiFacade->isBrainordinateDataColoredWithPalette()));
        
        CaretLogFine(msg);
        
        m_niftiHeaderDimensions.clear();
        m_niftiDataType = NiftiDataTypeEnum::NIFTI_TYPE_INVALID;
        
        CiftiFile* ciftiFile = dynamic_cast<CiftiFile*>(ciftiInterface);
        if (ciftiFile != NULL) {
            CiftiHeader ciftiHeader;
            ciftiFile->getHeader(ciftiHeader);
            
            nifti_2_header nifti2Struct;
            ciftiHeader.getHeaderStruct(nifti2Struct);
            for (int32_t i = 0; i < 8; i++) {
                m_niftiHeaderDimensions.push_back(nifti2Struct.dim[i]);
            }
            ciftiHeader.getNiftiDataTypeEnum(m_niftiDataType);
        }
        
        clearModified();
    }
    catch (CiftiFileException& e) {
        clear();
        throw DataFileException(e.whatString());
    }
    catch (DataFileException& e) {
        clear();
        throw e;
    }
    
    m_classNameHierarchy->update(this,
                                 true);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    m_classNameHierarchy->setAllSelected(true);
    
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
CiftiMappableDataFile::validateAfterFileReading() throw (DataFileException)
{
    /* nothing - see method comment. */
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of the file to write.
 * @throw
 *    DataFileException if there is an error writing the file.
 */
void
CiftiMappableDataFile::writeFile(const AString& filename) throw (DataFileException)
{
    try {
        if (m_ciftiInterface == NULL) {
            throw DataFileException(filename
                                    + " cannot be written because no file is loaded");
        }
        CiftiFile* ciftiFile = dynamic_cast<CiftiFile*>(m_ciftiInterface.getPointer());
        if (ciftiFile == NULL) {
            throw DataFileException(filename
                                    + " cannot be written because it was not read from a disk file and was"
                                    + " likely read via the network.");
        }
        
        if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE) {
            throw DataFileException(filename
                                    + " dense connectivity files cannot be written to files due to their large sizes.");
        }
        
        /*
         * Update the file's metadata.
         */
        m_ciftiFacade->setFileMetadata(m_metadata);
        
        /*
         * Update all data in the file.
         */
        const int32_t numMaps = getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            /*
             * Does file have map attributes
             */
            if (hasMapAttributes()) {
                /*
                 * Replace the map's metadata.
                 */
                m_ciftiFacade->setMetadataForMapOrSeriesIndex(i, getMapMetaData(i));
            }
            
        }
        
        ciftiFile->writeFile(filename);
    }
    catch (const CiftiFileException& cfe) {
        throw DataFileException(cfe);
    }
}

/**
 * @return The string name of the CIFTI index type.
 * @param ciftiIndexType
 */
AString
CiftiMappableDataFile::ciftiIndexTypeToName(const IndicesMapToDataType ciftiIndexType)
{
    AString name = "Invalid";
    
    switch (ciftiIndexType) {
        case CIFTI_INDEX_TYPE_BRAIN_MODELS:
            name = "CIFTI_INDEX_TYPE_BRAIN_MODELS";
            break;
        case CIFTI_INDEX_TYPE_FIBERS:
            name = "CIFTI_INDEX_TYPE_FIBERS";
            break;
        case CIFTI_INDEX_TYPE_INVALID:
            name = "CIFTI_INDEX_TYPE_INVALID";
            break;
        case CIFTI_INDEX_TYPE_LABELS:
            name = "CIFTI_INDEX_TYPE_LABELS";
            break;
        case CIFTI_INDEX_TYPE_PARCELS:
            name = "CIFTI_INDEX_TYPE_PARCELS";
            break;
        case CIFTI_INDEX_TYPE_SCALARS:
            name = "CIFTI_INDEX_TYPE_SCALARS";
            break;
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            name = "CIFTI_INDEX_TYPE_TIME_POINTS";
            break;
    }
    
    return name;
}


/**
 * @return Is the data mappable to a surface?
 */
bool
CiftiMappableDataFile::isSurfaceMappable() const
{
    return m_ciftiFacade->containsSurfaceDataForMappingToBrainordinates();
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
    if (m_ciftiFacade != NULL) {
        return m_ciftiFacade->containsMapAttributes();
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
    return m_mapContent[mapIndex]->m_name;
    
//  Using ciftiFacace for name can be slow for series maps since it
//  requires converting numbers to strings
//    return m_ciftiFacade->getNameForMapOrSeriesIndex(mapIndex);
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
    
    /*
     * If map name does not change, then get out
     */
    if (mapName == getMapName(mapIndex)) {
        return;
    }
    
    m_mapContent[mapIndex]->m_name = mapName;
    m_ciftiFacade->setNameForMapOrSeriesIndex(mapIndex,
                                              mapName);
    
    setModified();
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
    return m_ciftiFacade->isBrainordinateDataColoredWithPalette();
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
    m_ciftiFacade->getDataForMapOrSeriesIndex(mapIndex,
        dataOut);
}

void 
CiftiMappableDataFile::getMatrixRGBA(std::vector<float> &rgba,PaletteFile *paletteFile)
{
    CaretAssertVectorIndex(m_mapContent,
        0);

    const int ncols = m_ciftiFacade->getNumberOfColumns();
    const int nrows = m_ciftiFacade->getNumberOfRows();
    std::vector<float> data;
    data.resize(ncols*nrows);
    

    for(int i = 0;i<nrows;i++)
    {
        m_ciftiInterface->getRow(&data[i*ncols],i);
    }
    
    //MapContent* mc = m_mapContent[0];
    //if(!mc->m_rgbaValid) mc->updateColoring(data,paletteFile);
   
    PaletteColorMapping *paletteColorMapping = m_ciftiFacade->getPaletteColorMappingForMapOrSeriesIndex(0);
    FastStatistics *fastStatistics = new FastStatistics();
    {
        CaretAssert(paletteColorMapping);
        const AString paletteName = paletteColorMapping->getSelectedPaletteName();
        const Palette* palette = paletteFile->getPaletteByName(paletteName);
        if (palette != NULL) {
            fastStatistics->update(&data[0],
                data.size());

            rgba.resize(ncols*nrows*4);
            NodeAndVoxelColoring::colorScalarsWithPalette(fastStatistics,
                paletteColorMapping,
                palette,
                &data[0],
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
    delete fastStatistics;
}

void 
CiftiMappableDataFile::getMapDimensions(std::vector<int64_t> &dim) const
{
    dim.push_back(m_ciftiFacade->getNumberOfColumns());
    dim.push_back(m_ciftiFacade->getNumberOfRows());
    
}


/**
 * Get statistics describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const DescriptiveStatistics*
CiftiMappableDataFile::getMapStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    std::vector<float> data;
    getMapData(mapIndex,
               data);
    
    DescriptiveStatistics* ds = m_mapContent[mapIndex]->m_descriptiveStatistics;
    if (data.empty()) {
        ds->update(NULL,
                   0);
    }
    else {
        ds->update(&data[0],
                   data.size());
    }
    return ds;
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
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    std::vector<float> data;
    getMapData(mapIndex,
               data);
    
    FastStatistics* fs = m_mapContent[mapIndex]->m_fastStatistics;
    if (data.empty()) {
        fs->update(NULL,
                   0);
    }
    else {
        fs->update(&data[0],
                   data.size());
    }
    return fs;
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
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);

    std::vector<float> data;
    getMapData(mapIndex,
               data);
    
    Histogram* h = m_mapContent[mapIndex]->m_histogram;
    if (data.empty()) {
        h->update(NULL,
                  0);
    }
    else {
        h->update(&data[0],
                  data.size());
    }
    return h;
}

/**
 * Get statistics describing the distribution of data
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
const DescriptiveStatistics*
CiftiMappableDataFile::getMapStatistics(const int32_t mapIndex,
                                         const float mostPositiveValueInclusive,
                                         const float leastPositiveValueInclusive,
                                         const float leastNegativeValueInclusive,
                                         const float mostNegativeValueInclusive,
                                         const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    std::vector<float> data;
    getMapData(mapIndex,
               data);
    
    
    DescriptiveStatistics* ds = m_mapContent[mapIndex]->m_descriptiveStatistics;
    if (data.empty()) {
        ds->update(NULL,
                   0);
    }
    else {
        ds->update(&data[0],
                   data.size(),
                   mostPositiveValueInclusive,
                   leastPositiveValueInclusive,
                   leastNegativeValueInclusive,
                   mostNegativeValueInclusive,
                   includeZeroValues);
    }
    return ds;
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
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);

    std::vector<float> data;
    getMapData(mapIndex,
               data);
    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    Histogram* h = m_mapContent[mapIndex]->m_histogram;
    if (data.empty()) {
        h->update(NULL,
                  0);
    }
    else {
        h->update(&data[0],
                  data.size(),
                  mostPositiveValueInclusive,
                  leastPositiveValueInclusive,
                  leastNegativeValueInclusive,
                  mostNegativeValueInclusive,
                  includeZeroValues);
    }
    return h;
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
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    if (hasMapAttributes()) {
        return m_mapContent[mapIndex]->m_paletteColorMapping;
    }
    
    CaretAssert(m_ciftiInterface);
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    return ciftiXML.getFilePalette();
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
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    if (hasMapAttributes()) {
        return m_mapContent[mapIndex]->m_paletteColorMapping;
    }
    
    CaretAssert(m_ciftiInterface);
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    return ciftiXML.getFilePalette();
//    CaretAssertVectorIndex(m_mapContent,
//                           mapIndex);
//    
//    if (m_ciftiFacade->isConnectivityMatrixFile()) {
//        CaretAssert(m_ciftiInterface);
//        const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
//        return ciftiXML.getFilePalette();
//    }
//
//    return m_mapContent[mapIndex]->m_paletteColorMapping;
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool
CiftiMappableDataFile::isMappedWithLabelTable() const
{
    return m_ciftiFacade->isBrainordinateDataColoredWithLabelTable();
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
    return m_mapContent[mapIndex]->m_labelTable;
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
    return m_mapContent[mapIndex]->m_labelTable;
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
 * @param paletteFile
 *    Palette file containing palettes.
 */
void
CiftiMappableDataFile::updateScalarColoringForMap(const int32_t mapIndex,
                                                   const PaletteFile* paletteFile)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    std::vector<float> data;
    getMapData(mapIndex,
               data);

    m_mapContent[mapIndex]->m_rgbaValid = false;
    if (m_ciftiFacade->isBrainordinateDataColoredWithLabelTable()) {
        m_mapContent[mapIndex]->updateColoring(data,
                                               paletteFile);
    }
    else if (m_ciftiFacade->isBrainordinateDataColoredWithPalette()) {
        m_mapContent[mapIndex]->updateColoring(data,
                                               paletteFile);
    }
    else {
        CaretAssert(0);
    }
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
    dimOut1 = m_volumeDimensions[0];
    dimOut2 = m_volumeDimensions[1];
    dimOut3 = m_volumeDimensions[2];
    dimTimeOut = m_volumeDimensions[3];
    numComponentsOut = m_volumeDimensions[4];
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
    dimsOut.push_back(m_volumeDimensions[0]);
    dimsOut.push_back(m_volumeDimensions[1]);
    dimsOut.push_back(m_volumeDimensions[2]);
    dimsOut.push_back(m_volumeDimensions[3]);
    dimsOut.push_back(m_volumeDimensions[4]);
}

/**
 * @return The number of componenents per voxel in the volume data.
 */
const int64_t&
CiftiMappableDataFile::getNumberOfComponents() const
{
    return m_volumeDimensions[4];
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
    if ((indexIn1 >= 0)
        && (indexIn1 < m_volumeDimensions[0])
        && (indexIn2 >= 0)
        && (indexIn2 < m_volumeDimensions[1])
        && (indexIn3 >= 0)
        && (indexIn3 < m_volumeDimensions[2])) {
        return true;
    }
    
    return false;
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
    
    if (m_voxelIndicesToOffset->isValid()) {
        float xyz[3];
        indexToSpace(0,
                     0,
                     0,
                     xyz);
        boundingBoxOut.update(xyz);
        
        indexToSpace(m_volumeDimensions[0] - 1,
                     m_volumeDimensions[1] - 1,
                     m_volumeDimensions[2] - 1,
                     xyz);
        
        boundingBoxOut.update(xyz);
    }
}

/**
 * Get the voxel colors for a slice in the map.
 *
 * @param paletteFile
 *    The palette file.
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
 */
void
CiftiMappableDataFile::getVoxelColorsForSliceInMap(const PaletteFile* paletteFile,
                                                   const int32_t mapIndex,
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
        return;
    }
    
    if (isMapColoringValid(mapIndex) == false) {
        CiftiMappableDataFile* nonConstThis = const_cast<CiftiMappableDataFile*>(this);
        nonConstThis->updateScalarColoringForMap(mapIndex,
                                                 paletteFile);
    }
    
    const int64_t dimI = m_volumeDimensions[0];
    const int64_t dimJ = m_volumeDimensions[1];
    const int64_t dimK = m_volumeDimensions[2];
    
    int64_t voxelCount = 0;
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            voxelCount = dimI * dimJ;
            CaretAssert((sliceIndex < dimK));
            if (sliceIndex >= dimK) {
                return;
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            voxelCount = dimI * dimK;
            CaretAssert((sliceIndex < dimJ));
            if (sliceIndex >= dimJ) {
                return;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            voxelCount = dimJ * dimK;
            CaretAssert((sliceIndex < dimI));
            if (sliceIndex >= dimI) {
                return;
            }
            break;
    }
    
    if (voxelCount <= 0) {
        return;
    }
    const int64_t componentCount = voxelCount * 4;
    
    /*
     * Clear the slice rgba coloring.
     */
    for (int64_t i = 0; i < componentCount; i++) {
        rgbaOut[i] = 0;
    }
    
    const int64_t mapRgbaCount = m_mapContent[mapIndex]->m_rgba.size();
    CaretAssert(mapRgbaCount > 0);
    if (mapRgbaCount <= 0) {
        return;
    }
    
    const float* mapRGBA = &m_mapContent[mapIndex]->m_rgba[0];
    
    CaretAssert(m_voxelIndicesToOffset);

    std::vector<float> dataValues;
    getMapData(mapIndex,
               dataValues);
    const GiftiLabelTable* labelTable = (isMappedWithLabelTable()
                                         ? getMapLabelTable(mapIndex)
                                         : NULL);
    if (isMappedWithLabelTable()) {
        CaretAssert(labelTable);
    }
    
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
                        rgbaOut[rgbaOffset]   = (mapRGBA[dataOffset4] * 255.0);
                        rgbaOut[rgbaOffset+1] = (mapRGBA[dataOffset4+1] * 255.0);
                        rgbaOut[rgbaOffset+2] = (mapRGBA[dataOffset4+2] * 255.0);
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
                        rgbaOut[rgbaOffset]   = (mapRGBA[dataOffset4] * 255.0);
                        rgbaOut[rgbaOffset+1] = (mapRGBA[dataOffset4+1] * 255.0);
                        rgbaOut[rgbaOffset+2] = (mapRGBA[dataOffset4+2] * 255.0);
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
                        rgbaOut[rgbaOffset]   = (mapRGBA[dataOffset4] * 255.0);
                        rgbaOut[rgbaOffset+1] = (mapRGBA[dataOffset4+1] * 255.0);
                        rgbaOut[rgbaOffset+2] = (mapRGBA[dataOffset4+2] * 255.0);
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
                        
                        rgbaOut[rgbaOffset+3] = (alpha * 255.0);
                    }
                }
            }
            break;
    }
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
 * @param paletteFile
 *     The palette file.
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
CiftiMappableDataFile::getVoxelColorInMapForLabelData(const PaletteFile* paletteFile,
                                                      const std::vector<float>& dataForMap,
                                                      const int64_t indexIn1,
                                                      const int64_t indexIn2,
                                                      const int64_t indexIn3,
                                                      const int64_t mapIndex,
                                                      const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      uint8_t rgbaOut[4]) const
{
    getVoxelColorInMap(paletteFile,
                       indexIn1,
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
 * @param paletteFile
 *     The palette file.
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
CiftiMappableDataFile::getVoxelColorInMap(const PaletteFile* paletteFile,
                                          const int64_t indexIn1,
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
    
    const int64_t mapRgbaCount = m_mapContent[mapIndex]->m_rgba.size();
    CaretAssert(mapRgbaCount > 0);
    if (mapRgbaCount <= 0) {
        return;
    }
    
    if (isMapColoringValid(mapIndex) == false) {
        CiftiMappableDataFile* nonConstThis = const_cast<CiftiMappableDataFile*>(this);
        nonConstThis->updateScalarColoringForMap(mapIndex,
                                             paletteFile);
    }
    
    CaretAssert(m_voxelIndicesToOffset);
    
    const float* mapRGBA = &m_mapContent[mapIndex]->m_rgba[0];
    const int64_t dataOffset = m_voxelIndicesToOffset->getOffsetForIndices(indexIn1,
                                                                           indexIn2,
                                                                           indexIn3);
    if (dataOffset >= 0) {
        const int64_t dataOffset4 = dataOffset * 4;
        CaretAssert(dataOffset4 < mapRgbaCount);
        
        rgbaOut[0] = (mapRGBA[dataOffset4] * 255.0);
        rgbaOut[1] = (mapRGBA[dataOffset4+1] * 255.0);
        rgbaOut[2] = (mapRGBA[dataOffset4+2] * 255.0);
        
        /*
         * A negative value for alpha indicates "do not draw".
         * Since unsigned bytes do not have negative values,
         * change the value to zero (which indicates "transparent").
         */
        float alpha = mapRGBA[dataOffset4+3];
        if (alpha < 0.0) {
            alpha = 0.0;
        }
        rgbaOut[3] = (alpha * 255.0);
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
 * @return true if the CIFTI interface is valid, else false.
 * A message is logged if the interface is not valid.
 */
bool
CiftiMappableDataFile::isCiftiInterfaceValid() const
{
    if (m_ciftiInterface != NULL) {
        return true;
    }

    CaretLogSevere(getFileNameNoPath()
                   + "\" of type\""
                   + DataFileTypeEnum::toName(getDataFileType())
                   + "\" does not have a file loaded (CIFTI Interface is not valid).");
    return false;
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
    numericalValueOut = false;
    
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);

    const CiftiXMLOld& ciftiXML = m_ciftiInterface->getCiftiXMLOld();
    
    const int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
//    /*
//     * Validate number of nodes are correct
//     */
//    switch (m_brainordinateMappedDataAccess) {
//        case DATA_ACCESS_INVALID:
//            break;
//        case DATA_ACCESS_WITH_COLUMN_METHODS:
//            numCiftiNodes = m_ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
//            break;
//        case DATA_ACCESS_WITH_ROW_METHODS:
//            numCiftiNodes = m_ciftiInterface->getRowSurfaceNumberOfNodes(structure);
//            break;
//    }
    
    if (numCiftiNodes != numberOfNodes) {
        return false;
    }
    
    if (m_ciftiFacade->isMappingDataToBrainordinateParcels()) {
        /*
         * Get content for map.
         */
        int64_t parcelMapIndex = -1;
        std::vector<CiftiParcelElement> parcels;
        switch (m_brainordinateMappedDataAccess) {
            case DATA_ACCESS_INVALID:
                break;
            case DATA_ACCESS_WITH_COLUMN_METHODS:
                ciftiXML.getParcelsForColumns(parcels);
                parcelMapIndex = ciftiXML.getColumnParcelForNode(nodeIndex,
                                                                 structure);
                break;
            case DATA_ACCESS_WITH_ROW_METHODS:
                ciftiXML.getParcelsForRows(parcels);
                parcelMapIndex = ciftiXML.getRowParcelForNode(nodeIndex,
                                                              structure);
                break;
        }
        
        if ((parcelMapIndex >= 0)
            && (parcelMapIndex < static_cast<int64_t>(parcels.size()))) {
            textValueOut = parcels[parcelMapIndex].m_parcelName;
            return true;
        }
    }
    else {
        /*
         * Get content for map.
         */
        int64_t dataIndex = -1;
        std::vector<CiftiSurfaceMap> nodeMap;
        switch (m_brainordinateMappedDataAccess) {
            case DATA_ACCESS_INVALID:
                break;
            case DATA_ACCESS_WITH_COLUMN_METHODS:
                ciftiXML.getSurfaceMapForColumns(nodeMap,
                                                 structure);
                dataIndex = ciftiXML.getColumnIndexForNode(nodeIndex,
                                                           structure);
                break;
            case DATA_ACCESS_WITH_ROW_METHODS:
                ciftiXML.getSurfaceMapForRows(nodeMap,
                                              structure);
                dataIndex = ciftiXML.getRowIndexForNode(nodeIndex,
                                                        structure);
                break;
        }
        
        if (dataIndex >= 0) {
            std::vector<float> mapData;
            getMapData(mapIndex, mapData);
            
            if (dataIndex < static_cast<int64_t>(mapData.size())) {
                numericalValueOut = mapData[dataIndex];
                numericalValueOutValid = true;
                
                if (m_ciftiFacade->isBrainordinateDataColoredWithLabelTable()) {
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

        /*
         * At this time, ciftiXML.getRowIndexForNode() does not return
         * the index for a node in a Parcel Dense File.  
         */
        if (nodeMap.empty() == false) {
            const int64_t numNodeMaps = static_cast<int32_t>(nodeMap.size());
            for (int i = 0; i < numNodeMaps; i++) {
                if (nodeMap[i].m_surfaceNode == nodeIndex) {
                    std::vector<float> mapData;
                    getMapData(mapIndex, mapData);
                    if (mapData.size() == 0)//if the node to load data for is not in the column mapping, we will have no map data
                    {
                        numericalValueOutValid = false;
                        return false;
                    } else {
                        CaretAssertVectorIndex(mapData,
                                            nodeMap[i].m_ciftiIndex);
                        numericalValueOut = mapData[nodeMap[i].m_ciftiIndex];
                        numericalValueOutValid = true;
                        textValueOut = AString::number(numericalValueOut, 'f');
                        return true;
                    }
                }
            }
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
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    if (mapIndices.empty()) {
        return false;
    }
    
    bool useMapData = false;
    bool useSeriesData = false;
    switch (m_ciftiFacade->getCiftiFileType()) {
        case CiftiFacade::CIFTI_INVALID:
            break;
        case CiftiFacade::CIFTI_DENSE:
            useMapData = true;
            break;
        case CiftiFacade::CIFTI_LABEL:
            useSeriesData = true;
            break;
        case CiftiFacade::CIFTI_SCALAR:
            useSeriesData = true;
            break;
        case CiftiFacade::CIFTI_DENSE_PARCEL:
            useMapData = true;
            break;
        case CiftiFacade::CIFTI_DATA_SERIES:
            useSeriesData = true;
            break;
        case CiftiFacade::CIFTI_PARCEL:
            useMapData = true;
            break;
        case CiftiFacade::CIFTI_PARCEL_DENSE:
            useMapData = true;
            break;
        case CiftiFacade::CIFTI_PARCEL_SCALAR:
            useMapData = true;
            break;
        case CiftiFacade::CIFTI_PARCEL_SERIES:
            useMapData = true;
            break;
    }
    
    
    const int32_t numberOfMapIndices = static_cast<int32_t>(mapIndices.size());
    
    textOut = "";
    
    bool validID = false;
    
    if (useMapData) {
        for (int32_t i = 0; i < numberOfMapIndices; i++) {
            const int32_t mapIndex = mapIndices[i];
            
            float numericalValue;
            AString textValue;
            bool numericalValueValid;
            if (getMapSurfaceNodeValue(mapIndex,
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
    else {
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
                if (m_ciftiFacade->isBrainordinateDataColoredWithLabelTable()) {
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
                else if (m_ciftiFacade->isBrainordinateDataColoredWithPalette()) {
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
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    
    const bool valid = m_ciftiFacade->getSeriesDataForSurfaceNode(structure,
                                                                  nodeIndex,
                                                                  seriesDataOut);
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
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    
    const bool valid = m_ciftiFacade->getSeriesDataForVoxelAtCoordinate(xyz,
                                                                        seriesDataOut);
    
    return valid;
}

/**
 * Get the node coloring for the surface.
 * @param surface
 *    Surface whose nodes are colored.
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
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    const int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
//    /*
//     * Validate number of nodes are correct
//     */
//    switch (m_brainordinateMappedDataAccess) {
//        case DATA_ACCESS_INVALID:
//            break;
//        case DATA_ACCESS_WITH_COLUMN_METHODS:
//            numCiftiNodes = m_ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
//            break;
//        case DATA_ACCESS_WITH_ROW_METHODS:
//            numCiftiNodes = m_ciftiInterface->getRowSurfaceNumberOfNodes(structure);
//            break;
//    }
    
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
    
    const MapContent* mc = m_mapContent[mapIndex];
    const std::vector<int64_t>* dataIndicesForNodes =
    m_ciftiFacade->getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                                   surfaceNumberOfNodes);
    if (dataIndicesForNodes == NULL) {
        return false;
    }

    bool validColorsFlag = false;

   
    
    for (int64_t iNode = 0; iNode < surfaceNumberOfNodes; iNode++) {
        CaretAssertVectorIndex((*dataIndicesForNodes),
                               iNode);
        
        const int64_t dataIndex = (*dataIndicesForNodes)[iNode];
        
        const int64_t node4 = iNode * 4;
        CaretAssertArrayIndex(surfaceRGBA, (surfaceNumberOfNodes * 4), node4);
        
        if (dataIndex >= 0) {
            CaretAssert(dataIndex < mc->m_dataCount);
            
            const int64_t data4 = dataIndex * 4;
            CaretAssertArrayIndex(this->dataRGBA, (mc->m_dataCount * 4), data4);
            
            surfaceRGBAOut[node4]   = mc->m_rgba[data4];
            surfaceRGBAOut[node4+1] = mc->m_rgba[data4+1];
            surfaceRGBAOut[node4+2] = mc->m_rgba[data4+2];
            surfaceRGBAOut[node4+3] = mc->m_rgba[data4+3];           
            
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
    
    return validColorsFlag;
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
    
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    
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
            const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
            if (m_ciftiFacade->isMappingDataToBrainordinateParcels()) {
                /*
                 * Get content for map.
                 */
                int64_t parcelMapIndex = -1;
                std::vector<CiftiParcelsMap::Parcel> parcels;
                switch (m_brainordinateMappedDataAccess) {
                    case DATA_ACCESS_INVALID:
                        break;
                    case DATA_ACCESS_WITH_COLUMN_METHODS:
                        parcels = ciftiXML.getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
                        parcelMapIndex = ciftiXML.getParcelsMap(CiftiXML::ALONG_COLUMN).getIndexForVoxel(ijk);
                        break;
                    case DATA_ACCESS_WITH_ROW_METHODS:
                        parcels = ciftiXML.getParcelsMap(CiftiXML::ALONG_ROW).getParcels();
                        parcelMapIndex = ciftiXML.getParcelsMap(CiftiXML::ALONG_ROW).getIndexForVoxel(ijk);
                        break;
                }
                
                if ((parcelMapIndex >= 0)
                    && (parcelMapIndex < static_cast<int64_t>(parcels.size()))) {
                    textValueOut = parcels[parcelMapIndex].m_name;
                    return true;
                }
            }
            else {
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
                    
                    if (m_ciftiFacade->isBrainordinateDataColoredWithLabelTable()) {
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
                    else if (m_ciftiFacade->isBrainordinateDataColoredWithPalette()) {
                        numericalValueOutValid = true;
                        textValueOut = AString::number(numericalValueOut);
                    }
                    else {
                        CaretAssert(0);
                    }
                    
                    return true;
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
    
    if (isCiftiInterfaceValid() == false) {
        return 0.0;
    }
    
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
    
    if (isCiftiInterfaceValid() == false) {
        return dataOffset;
    }
    
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
    const int32_t numberOfMapIndices = static_cast<int32_t>(mapIndices.size());
    if (numberOfMapIndices <= 0) {
        return false;
    }
    
    textOut = "";
    
    for (int32_t i = 0; i < numberOfMapIndices; i++) {
        const int32_t mapIndex = mapIndices[i];
        
        float numericalValue;
        AString textValue;
        bool numericalValueValid;
        if (getMapVolumeVoxelValue(mapIndex,
                                   xyz,
                                   ijkOut,
                                   numericalValue,
                                   numericalValueValid,
                                   textValue)) {
            textOut += textValue;
            textOut += " ";
        }
    }
    
    if (textOut.isEmpty() == false) {
        return true;
    }
    
    return false;
}

/**
 * For the given row index, find its corresponding surface structure and
 * node index.
 *
 * @param rowIndex
 *    The file's row index.
 * @param structureOut
 *    Output structure corresponding to row index.
 * @param nodeIndexOut
 *    Output node index corresponding to row index.
 * @return true if the row corresponds to a surface structure and node index
 *    else false.
 */
bool
CiftiMappableDataFile::getStructureAndNodeIndexFromRowIndex(const int64_t rowIndex,
                                                                              StructureEnum::Enum& structureOut,
                                                                              int64_t& nodeIndexOut) const
{
    CaretAssert(m_ciftiFacade);
    
    const int64_t numberOfRows = m_ciftiFacade->getNumberOfRows();
    if ((rowIndex >= 0)
        && (rowIndex < numberOfRows)) {
        std::vector<StructureEnum::Enum> surfaceStructures;
        std::vector<StructureEnum::Enum> volumeStructures;
        
        const CiftiXMLOld& ciftiXML = m_ciftiInterface->getCiftiXMLOld();
        ciftiXML.getStructureListsForColumns(surfaceStructures,
                                             volumeStructures);
        
        /*
         * Try to find the CIFTI row in the available surface structures.
         */
        for (std::vector<StructureEnum::Enum>::iterator structureIter = surfaceStructures.begin();
             structureIter != surfaceStructures.end();
             structureIter++) {
            const StructureEnum::Enum structure = *structureIter;
            std::vector<CiftiSurfaceMap> surfaceMaps;
            ciftiXML.getSurfaceMapForColumns(surfaceMaps, structure);
            
            /*
             * Search the surface maps for the structure.
             */
            for (std::vector<CiftiSurfaceMap>::iterator surfaceMapIter = surfaceMaps.begin();
                 surfaceMapIter != surfaceMaps.end();
                 surfaceMapIter++) {
                CiftiSurfaceMap& csm = *surfaceMapIter;
                if (csm.m_ciftiIndex == rowIndex) {
                    structureOut = structure;
                    nodeIndexOut = csm.m_surfaceNode;
                    return true;
                }
            }
        }
    }
    
    return false;
}

/**
 * For the given row index, find its corresponding voxel indices and
 * coordinate.
 *
 * @param rowIndex
 *    The file's row index.
 * @param ijkOut
 *    Output voxel indices corresponding to row index.
 * @param xyzOut
 *    Output voxel coordinates corresponding to row index.
 * @return true if the row corresponds to a voxel else false.
 */
bool
CiftiMappableDataFile::getVoxelIndexAndCoordinateFromRowIndex(const int64_t rowIndex,
                                                                                int64_t ijkOut[3],
                                                                                float xyzOut[3]) const
{
    CaretAssert(m_ciftiFacade);
    
    const int64_t numberOfRows = m_ciftiFacade->getNumberOfRows();
    if ((rowIndex >= 0)
        && (rowIndex < numberOfRows)) {
        std::vector<CiftiVolumeMap> volumeMaps;
        
        const CiftiXMLOld& ciftiXML = m_ciftiInterface->getCiftiXMLOld();
        ciftiXML.getVolumeMapForColumns(volumeMaps);
        
        /*
         * Search the surface maps for the structure.
         */
        for (std::vector<CiftiVolumeMap>::iterator volumeMapIter = volumeMaps.begin();
             volumeMapIter != volumeMaps.end();
             volumeMapIter++) {
            CiftiVolumeMap& cvm = *volumeMapIter;
            if (cvm.m_ciftiIndex == rowIndex) {
                ijkOut[0] = cvm.m_ijk[0];
                ijkOut[1] = cvm.m_ijk[1];
                ijkOut[2] = cvm.m_ijk[2];
                
                indexToSpace(ijkOut,
                             xyzOut);
                return true;
            }
        }
        
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
    
    m_metadata->clearModified();
    
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; i++) {
        m_mapContent[i]->clearModifiedStatus();
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
    
    if (m_metadata->isModified()) {
        return true;
    }
    
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; i++) {
        if (m_mapContent[i]->isModifiedStatus()) {
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
    float start, step;
    NiftiTimeUnitsEnum::Enum units;
    m_ciftiFacade->getMapIntervalStartStepAndUnits(start,
                                                   step,
                                                   units);
    return units;
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
    NiftiTimeUnitsEnum::Enum units;
    m_ciftiFacade->getMapIntervalStartStepAndUnits(firstMapUnitsValueOut,
                                                   mapIntervalStepValueOut,
                                                   units);
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
    CaretAssert(m_ciftiInterface);
    
    /*
     * Dense is very large but at this time is [-1, 1]
     */
    if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE) {
        dataRangeMaximumOut = 1.0;
        dataRangeMinimumOut = -1;
        return true;
    }
    else {
        if (m_ciftiInterface->getDataRangeFromAllMaps(dataRangeMinimumOut,
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
    /*
     * Validate number of nodes are correct
     */
    int32_t numCiftiNodes = -1;
    
    switch (m_brainordinateMappedDataAccess) {
        case DATA_ACCESS_INVALID:
            break;
        case DATA_ACCESS_WITH_COLUMN_METHODS:
            if (m_ciftiInterface->hasColumnSurfaceData(structure)) {
                numCiftiNodes = m_ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
            }
            break;
        case DATA_ACCESS_WITH_ROW_METHODS:
            if (m_ciftiInterface->hasRowSurfaceData(structure)) {
                numCiftiNodes = m_ciftiInterface->getRowSurfaceNumberOfNodes(structure);
            }
            break;
    }
    
    return numCiftiNodes;
    
    //    CaretAssert(m_ciftiInterface);
    //
    //    if (m_ciftiInterface->hasRowSurfaceData(structure)) {
    //    }
    //
    //    int32_t numNodes = m_ciftiInterface->getRowSurfaceNumberOfNodes(structure);
    //    if (numNodes < 0) {
    //        numNodes = m_ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
    //    }
    //
    //    return numNodes;
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
    CaretMappableDataFile::addToDataFileContentInformation(dataFileInformation);
    
    int64_t dimI, dimJ, dimK, dimTime, dimNumComp;
    getDimensions(dimI,
                  dimJ,
                  dimK,
                  dimTime,
                  dimNumComp);
    
    dataFileInformation.addNameAndValue("Volume Dim[0]", dimI);
    dataFileInformation.addNameAndValue("Volume Dim[1]", dimJ);
    dataFileInformation.addNameAndValue("Volume Dim[2]", dimK);
    
    if ( ! m_niftiHeaderDimensions.empty()) {
        const int32_t numDims = static_cast<int32_t>(m_niftiHeaderDimensions.size());
        for (int32_t i = 0; i < numDims; i++) {
            dataFileInformation.addNameAndValue(("NIFTI Dim["
                                                 + AString::number(i)
                                                 + "]"),
                                                m_niftiHeaderDimensions[i]);
        }
    }
    
    dataFileInformation.addNameAndValue("NIFTI Data Type",
                                        NiftiDataTypeEnum::toName(m_niftiDataType));
    
    std::vector<StructureEnum::Enum> allStructures;
    StructureEnum::getAllEnums(allStructures);
    
    for (std::vector<StructureEnum::Enum>::iterator iter = allStructures.begin();
         iter != allStructures.end();
         iter++) {
        const int32_t numNodes = getMappingSurfaceNumberOfNodes(*iter);
        if (numNodes > 0) {
            dataFileInformation.addNameAndValue(("Number of Vertices ("
                                                 + StructureEnum::toGuiName(*iter)
                                                 + ")"),
                                                (AString::number(numNodes)
                                                 + "  "));
        }
    }
}

///**
// * Create cartesian chart data from the given data.
// *
// * @param
// *     Data for the Y-axis.
// */
//ChartDataCartesian*
//CiftiMappableDataFile::helpCreateCartesianChartData(const std::vector<float>& data) throw (DataFileException)
//{
//    ChartDataCartesian* chartData = NULL;
//    
//    const int64_t numData = static_cast<int64_t>(data.size());
//    
//    /*
//     * Some files may have time data
//     */
//    bool timeSeriesFlag = false;
//    bool dataSeriesFlag = false;
//    bool mayHaveTimeUnitsFlag = false;
//    switch (getDataFileType()) {
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
//            dataSeriesFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
//            mayHaveTimeUnitsFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
//            dataSeriesFlag = true;
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
//            mayHaveTimeUnitsFlag = true;
//            break;
//        default:
//            const AString msg("Has a new type of connectivity file for chart data loading been added?");
//            CaretAssertMessage(0, msg);
//            CaretLogSevere(msg);
//            throw DataFileException(msg);
//            break;
//    }
//    
//    float convertTimeToSeconds = 1.0;
//    if (mayHaveTimeUnitsFlag) {
//        switch (getMapIntervalUnits()) {
//            case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
//                break;
//            case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
//                timeSeriesFlag = true;
//                convertTimeToSeconds = 1000.0;
//                break;
//            case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
//                break;
//            case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
//                convertTimeToSeconds = 1.0;
//                timeSeriesFlag = true;
//                break;
//            case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
//                dataSeriesFlag = true;
//                break;
//            case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
//                convertTimeToSeconds = 1000000.0;
//                timeSeriesFlag = true;
//                break;
//        }
//        
//        if (timeSeriesFlag) {
//            dataSeriesFlag = false;
//        }
//    }
//    
//    if (dataSeriesFlag) {
//        chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES,
//                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
//                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
//    }
//    else if (timeSeriesFlag) {
//        chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES,
//                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS,
//                                           ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
//    }
//    else {
//        const AString msg = "New type of units for data series flag, needs updating for charting";
//        CaretAssertMessage(0, msg);
//        throw DataFileException(msg);
//    }
//    
//    if (chartData != NULL) {
//        float timeStart = 0.0;
//        float timeStep  = 1.0;
//        if (timeSeriesFlag) {
//            getMapIntervalStartAndStep(timeStart,
//                                       timeStep);
//            timeStart *= convertTimeToSeconds;
//            timeStep  *= convertTimeToSeconds;
//            chartData->setTimeStartInSecondsAxisX(timeStart);
//            chartData->setTimeStepInSecondsAxisX(timeStep);
//        }
//        
//        for (int64_t i = 0; i < numData; i++) {
//            float xValue = i;
//            
//            if (timeSeriesFlag) {
//                xValue = timeStart + (i * timeStep);
//            }
//            
//            chartData->addPoint(xValue,
//                                data[i]);
//        }
//    }
//    
//    return chartData;
//}

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
                                                              const std::vector<int32_t>& nodeIndices) throw (DataFileException)
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
CiftiMappableDataFile::helpLoadChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
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
                                                       const int32_t nodeIndex) throw (DataFileException)
{
    ChartDataCartesian* chartData = NULL;
    
    try {
        std::vector<float> data;
        if (getSeriesDataForSurfaceNode(structure,
                                        nodeIndex,
                                        data)) {
            
            chartData = helpCreateCartesianChartData(data);
            //            const int64_t numData = static_cast<int64_t>(data.size());
            //
            //
            //            /*
            //             * Some files may have time data
            //             */
            //            bool timeSeriesFlag = false;
            //            bool dataSeriesFlag = false;
            //            bool mayHaveTimeUnitsFlag = false;
            //            switch (getDataFileType()) {
            //                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            //                    dataSeriesFlag = true;
            //                    break;
            //                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            //                    mayHaveTimeUnitsFlag = true;
            //                    break;
            //                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            //                    dataSeriesFlag = true;
            //                    break;
            //                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            //                    mayHaveTimeUnitsFlag = true;
            //                    break;
            //                default:
            //                    const AString msg("Has a new type of connectivity file for chart data loading been added?");
            //                    CaretAssertMessage(0, msg);
            //                    CaretLogSevere(msg);
            //                    throw DataFileException(msg);
            //                    break;
            //            }
            //
            //            float convertTimeToSeconds = 1.0;
            //            if (mayHaveTimeUnitsFlag) {
            //                switch (getMapIntervalUnits()) {
            //                    case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
            //                        break;
            //                    case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
            //                        timeSeriesFlag = true;
            //                        convertTimeToSeconds = 1000.0;
            //                        break;
            //                    case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
            //                        break;
            //                    case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
            //                        convertTimeToSeconds = 1.0;
            //                        timeSeriesFlag = true;
            //                        break;
            //                    case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
            //                        dataSeriesFlag = true;
            //                        break;
            //                    case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
            //                        convertTimeToSeconds = 1000000.0;
            //                        timeSeriesFlag = true;
            //                        break;
            //                }
            //
            //                if (timeSeriesFlag) {
            //                    dataSeriesFlag = false;
            //                }
            //            }
            //
            //            if (dataSeriesFlag) {
            //                chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES,
            //                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
            //                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            //            }
            //            else if (timeSeriesFlag) {
            //                chartData = new ChartDataCartesian(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES,
            //                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS,
            //                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            //            }
            //            else {
            //                const AString msg = "New type of units for data series flag, needs updating for charting";
            //                CaretAssertMessage(0, msg);
            //                throw DataFileException(msg);
            //            }
            //
            //            if (chartData != NULL) {
            //                float timeStart = 0.0;
            //                float timeStep  = 1.0;
            //                if (timeSeriesFlag) {
            //                    getMapIntervalStartAndStep(timeStart,
            //                                               timeStep);
            //                    timeStart *= convertTimeToSeconds;
            //                    timeStep  *= convertTimeToSeconds;
            //                    chartData->setTimeStartInSecondsAxisX(timeStart);
            //                    chartData->setTimeStepInSecondsAxisX(timeStep);
            //                }
            //
            //                for (int64_t i = 0; i < numData; i++) {
            //                    float xValue = i;
            //
            //                    if (timeSeriesFlag) {
            //                        xValue = timeStart + (i * timeStep);
            //                    }
            //
            //                    chartData->addPoint(xValue,
            //                                        data[i]);
            //                }
            //                
            //                const AString description = (getFileNameNoPath()
            //                                             + " node "
            //                                             + AString::number(nodeIndex));
            //                chartData->setDescription(description);
            //            }
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
                                                   const std::vector<float> surfaceMapData) throw (DataFileException)
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    
    
    if (isCiftiInterfaceValid() == false) {
        return;
    }
    
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    const int32_t surfaceNumberOfNodes = static_cast<int32_t>(surfaceMapData.size());
    const int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
    if (numCiftiNodes != surfaceNumberOfNodes) {
        return;
    }
    
    std::vector<float> mapData;
    m_ciftiFacade->getDataForMapOrSeriesIndex(mapIndex,
                                              mapData);
    
    /*
     * Map data may be empty for connectivity matrix files with no rows loaded.
     */
    if (mapData.empty()) {
        return;
    }
    
    const std::vector<int64_t>* dataIndicesForNodes =
    m_ciftiFacade->getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                                   surfaceNumberOfNodes);
    if (dataIndicesForNodes == NULL) {
        return;
    }
    
    for (int32_t iNode = 0; iNode < surfaceNumberOfNodes; iNode++) {
        CaretAssertVectorIndex((*dataIndicesForNodes),
                               iNode);
        
        const int64_t dataIndex = (*dataIndicesForNodes)[iNode];
        if (dataIndex >= 0) {
            mapData[dataIndex] = surfaceMapData[iNode];
        }
    }
    
    const bool dataUpdateValid = m_ciftiFacade->setDataForMapOrSeriesIndex(mapIndex,
                                                                       mapData);

    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    m_mapContent[mapIndex]->invalidateColoring();
    
    if ( ! dataUpdateValid) {
        throw DataFileException("Writing of data failed.  Is this file remote (on the Web)?");
    }
}


/**
 * Help load matrix chart data.
 *
 * @param mapIndex
 *    Index of map for which chart data is requested.
 * @param numberOfRowsOut
 *    Output number of rows in rgba matrix.
 * @param numberOfColumnsOut
 *    Output number of Columns in rgba matrix.
 * @param rgbaOut
 *    RGBA matrix (number of elements is rows * columns * 4).
 * @return
 *    True if output data is valid, else false.
 */
bool
CiftiMappableDataFile::helpLoadChartDataMatrixForMap(const int32_t mapIndex,
                                                     int32_t& numberOfRowsOut,
                                                     int32_t& numberOfColumnsOut,
                                                     std::vector<float>& rgbaOut) const
{
    if (getDataFileType() != DataFileTypeEnum::CONNECTIVITY_PARCEL) {
        CaretAssertMessage(0,
                           "Unsupported file type for chart matrix: "
                           + DataFileTypeEnum::toGuiName(getDataFileType()));
        return false;
    }
    
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    MapContent* mc = m_mapContent[mapIndex];
    
    /*
     * Dimensions of matrix.
     */
    numberOfRowsOut    = m_ciftiInterface->getNumberOfRows();
    numberOfColumnsOut = m_ciftiInterface->getNumberOfColumns();
    const int32_t numberOfData = numberOfRowsOut * numberOfColumnsOut;
    if (numberOfData <= 0) {
        return false;
    }
    
    /*
     * Get the data.
     */
    std::vector<float> data(numberOfData);
    for (int32_t iRow = 0; iRow < numberOfRowsOut; iRow++) {
        const int32_t rowOffset = iRow * numberOfColumnsOut;
        CaretAssertVectorIndex(data, rowOffset + numberOfColumnsOut - 1);
        m_ciftiInterface->getRow(&data[rowOffset],
                                 iRow);
    }

    /*
     * Get palette for color mapping.
     */
    const AString paletteName = mc->m_paletteColorMapping->getSelectedPaletteName();
    if (paletteName.isEmpty()) {
        CaretLogSevere("No palette name for coloring matrix chart data.");
        return false;
    }
    EventPaletteGetByName eventPaletteGetName(paletteName);
    EventManager::get()->sendEvent(eventPaletteGetName.getPointer());
    const Palette* palette = eventPaletteGetName.getPalette();
    if (palette == NULL) {
        CaretLogSevere("No palette named "
                       + paletteName
                       + " found for coloring matrix chart data.");
        return false;
    }
    
    /*
     * Color the data.
     */
    const int32_t numRGBA = numberOfData * 4;
    rgbaOut.resize(numRGBA);
    FastStatistics fastStatistics;
    fastStatistics.update(&data[0],
                          numberOfData);
    NodeAndVoxelColoring::colorScalarsWithPalette(&fastStatistics,
                                                  mc->m_paletteColorMapping,
                                                  palette,
                                                  &data[0],
                                                  &data[0],
                                                  numberOfData,
                                                  &rgbaOut[0]);
    
    return true;
}

/* ========================================================================== */

/**
 * Constructor.
 *
 * @param ciftiFacade
 *    CIFTI Facade that hides complexities of CIFTI interface.
 * @param mapIndex
 *    Index of this map.
 */
CiftiMappableDataFile::MapContent::MapContent(CiftiFacade* ciftiFacade,
                                              const int32_t mapIndex)
:
m_dataCount(ciftiFacade->getMapDataCount()),
m_paletteColorMapping(NULL),
m_labelTable(NULL),
m_rgbaValid(false)
{
    m_fastStatistics.grabNew(new FastStatistics());
    m_histogram.grabNew(new Histogram());
    m_metadata.grabNew(new GiftiMetaData());
    
    m_name = "";
    if (ciftiFacade->containsMapAttributes()) {
        ciftiFacade->getMetadataForMapOrSeriesIndex(mapIndex,
                                                    m_metadata);
        m_name = ciftiFacade->getNameForMapOrSeriesIndex(mapIndex);
    }
    
    
    m_paletteColorMapping = ciftiFacade->getPaletteColorMappingForMapOrSeriesIndex(mapIndex);
    
    m_labelTable = ciftiFacade->getLabelTableForMapOrSeriesIndex(mapIndex);
    
    m_dataIsMappedWithLabelTable = ciftiFacade->isBrainordinateDataColoredWithLabelTable();
    
    /*
     * Resize RGBA.  Values are filled in updateColoring()
     */
    m_rgba.resize(m_dataCount * 4, 0.0);
}

/**
 * Destructor.
 */
CiftiMappableDataFile::MapContent::~MapContent()
{
    /**
     * Do not delete these as they point to data in CIFTI XML:
     *   m_labelTable
     *   m_paletteSettings
     */
}

/**
 * Clear the modfied status of items in the map.
 */
void
CiftiMappableDataFile::MapContent::clearModifiedStatus()
{
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
CiftiMappableDataFile::MapContent::isModifiedStatus()
{
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
 * Invalidate the coloring (usually due to palette or data changes).
 */
void
CiftiMappableDataFile::MapContent::invalidateColoring()
{
    m_rgbaValid = false;
}

/**
 * Update coloring for this map.  If the paletteFile is NOT NULL,
 * color using a palette; otherwise, color with label table.
 *
 * @param data
 *    Data contained in the map.
 * @param paletteFile
 *    File containing the palettes.
 */
void
CiftiMappableDataFile::MapContent::updateColoring(const std::vector<float>& data,
                                                  const PaletteFile* paletteFile)
{
    if (data.empty()) {
        return;
    }
    
    if (m_rgbaValid) {
        return;
    }
    
    CaretAssert(m_dataCount == static_cast<int32_t>(data.size()));
    
    if (m_dataIsMappedWithLabelTable) {
        NodeAndVoxelColoring::colorIndicesWithLabelTable(m_labelTable,
                                                         &data[0],
                                                         data.size(),
                                                         &m_rgba[0]);
    }
    else if (paletteFile != NULL) {
        CaretAssert(m_paletteColorMapping);
        const AString paletteName = m_paletteColorMapping->getSelectedPaletteName();
        const Palette* palette = paletteFile->getPaletteByName(paletteName);
        if (palette != NULL) {
            m_fastStatistics->update(&data[0],
                                     data.size());
            NodeAndVoxelColoring::colorScalarsWithPalette(m_fastStatistics,
                                                          m_paletteColorMapping,
                                                          palette,
                                                          &data[0],
                                                          &data[0],
                                                          m_dataCount,
                                                          &m_rgba[0]);
        }
        else {
            std::fill(m_rgba.begin(),
                      m_rgba.end(),
                      0.0);
        }
    }
    else {
        const AString msg("NULL palette for coloring scalar data.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    
    m_rgbaValid = true;
    
    CaretLogFine("Connectivity Data Average/Min/Max: "
                 + QString::number(m_fastStatistics->getMean())
                 + " "
                 + QString::number(m_fastStatistics->getMostNegativeValue())
                 + " "
                 + QString::number(m_fastStatistics->getMostPositiveValue()));
}



