/*LICENSE_START*/
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <algorithm>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "CiftiXnat.h"
#include "DescriptiveStatistics.h"
#include "ElapsedTimer.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "ConnectivityLoaderFile.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * Constructor.
 */
ConnectivityLoaderFile::ConnectivityLoaderFile()
: CaretMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE)
{
    this->ciftiDiskFile = NULL;
    this->ciftiXnatFile = NULL;
    this->ciftiInterface = NULL;
    this->descriptiveStatistics = NULL;
    this->paletteColorMapping = NULL;
    this->labelTable = NULL;
    this->metadata = NULL;
    this->data = NULL;
    this->dataRGBA = NULL;
    this->loaderType = LOADER_TYPE_INVALID;
    this->connectivityVolumeFile = NULL;
    this->mapToType = MAP_TO_TYPE_INVALID;
    this->timeSeriesGraphEnabled = false;
    this->selectedTimePoint = 0.0;
    this->dataLoadingEnabled = true;
    this->uniqueID = SystemUtilities::createUniqueID();
    
    this->setFileName("");
}

/**
 * Destructor.
 */
ConnectivityLoaderFile::~ConnectivityLoaderFile()
{
    this->clearData();
}

/**
 * Clear all data in the file.
 */
void 
ConnectivityLoaderFile::clearData()
{
    if (this->ciftiDiskFile != NULL) {
        delete this->ciftiDiskFile;
        this->ciftiDiskFile = NULL;
    }
    if (this->ciftiXnatFile != NULL) {
        delete this->ciftiXnatFile;
        this->ciftiXnatFile = NULL;
    }
    if (this->descriptiveStatistics != NULL) {
        delete this->descriptiveStatistics;
        this->descriptiveStatistics = NULL;
    }
    if (this->paletteColorMapping != NULL) {
        delete this->paletteColorMapping;
        this->paletteColorMapping = NULL;
    }
    if (this->labelTable != NULL) {
        delete this->labelTable;
        this->labelTable = NULL;
    }
    if (this->metadata != NULL) {
        delete this->metadata;
        this->metadata = NULL;
    }
    if (this->connectivityVolumeFile != NULL) {
        delete this->connectivityVolumeFile;
        this->connectivityVolumeFile = NULL;
    }
    this->selectedTimePoint = 0.0;
    this->ciftiInterface = NULL; // pointer to disk or network file so do not delete
    this->loaderType = LOADER_TYPE_INVALID;
    this->mapToType = MAP_TO_TYPE_INVALID;
    this->allocateData(0);

    this->dataLoadingEnabled = true;
}

/**
 * Clear the contents of this file.
 */
void 
ConnectivityLoaderFile::clear()
{
    CaretMappableDataFile::clear();
    this->reset();
}

/**
 * Reset this file.
 * Clear all data and initialize needed data.
 */
void
ConnectivityLoaderFile::reset()
{
    this->clearData();
    this->descriptiveStatistics = new DescriptiveStatistics();
    
    this->paletteColorMapping = new PaletteColorMapping();
    
    this->labelTable = new GiftiLabelTable();
    
    this->metadata = new GiftiMetaData();    
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
ConnectivityLoaderFile::isEmpty() const
{
    return this->getFileName().isEmpty();
}

/**
 * @return Is loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 */
bool 
ConnectivityLoaderFile::isDataLoadingEnabled() const
{
    return this->dataLoadingEnabled;
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
ConnectivityLoaderFile::setDataLoadingEnabled(const bool dataLoadingEnabled)
{
    this->dataLoadingEnabled = dataLoadingEnabled;
}

/**
 * Setup the loader for a file.
 * @param filepath
 *    Path of file from which data is obtained.
 * @param connectivityFileType
 *    Type of data.
 * @param username
 *    Username for logging in.
 * @param password
 *    Password for logging in.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
ConnectivityLoaderFile::setup(const AString& path,
                              const DataFileTypeEnum::Enum connectivityFileType,
                              const AString& username,
                              const AString& password) throw (DataFileException)
{
    this->clear();
    
    switch (connectivityFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            this->loaderType = LOADER_TYPE_DENSE;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            this->loaderType = LOADER_TYPE_DENSE_TIME_SERIES;
            break;
        default:
            throw DataFileException("Unsupported connectivity type " 
                                    + DataFileTypeEnum::toName(connectivityFileType));
            break;
    }
    
    try {
        if (path.startsWith("http")) {
            this->ciftiXnatFile = new CiftiXnat();
            if (username.isEmpty() == false) {
                this->ciftiXnatFile->setAuthentication(path, username, password);
            }
            this->ciftiXnatFile->openURL(path);
            this->ciftiInterface = this->ciftiXnatFile;
        }
        else {
            this->ciftiDiskFile = new CiftiFile();
            if(this->loaderType == LOADER_TYPE_DENSE_TIME_SERIES)
            {
                this->ciftiDiskFile->openFile(path, IN_MEMORY);
            }
            else this->ciftiDiskFile->openFile(path, ON_DISK);

            this->ciftiInterface = this->ciftiDiskFile;
        } 
        this->setFileName(path);
        this->setDataFileType(connectivityFileType);

        this->paletteColorMapping->setSelectedPaletteName(Palette::ROY_BIG_BL_PALETTE_NAME);
        this->paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
        this->paletteColorMapping->setInterpolatePaletteFlag(true);
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}

/**
 * Setup the loader.
 * @param url
 *    URL of file from which data is obtained.
 * @param connectivityFileType
 *    Type of data.
 * @param username
 *    Username for logging in.
 * @param password
 *    Password for logging in.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
ConnectivityLoaderFile::setupNetworkFile(const AString& url,
                                         const DataFileTypeEnum::Enum connectivityFileType,
                                         const AString& username,
                                         const AString& password) throw (DataFileException)
{
    this->clear();
    if (url.startsWith("http") == false) {
        throw DataFileException("For network files, name must begin with \"http\"");
    }
    this->setup(url, connectivityFileType, username, password);
}

/**
 * Setup the loader for a local file.
 * @param filename
 *    Name of file from which data is obtained.
 * @param connectivityFileType
 *    Type of data.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
ConnectivityLoaderFile::setupLocalFile(const AString& filename,
                                       const DataFileTypeEnum::Enum connectivityFileType) throw (DataFileException)
{
    this->setup(filename, connectivityFileType, "", "");
}

/**
 * Read the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
ConnectivityLoaderFile::readFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Reading of ConnectivityLoaderFile not supported, use setup()");
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
ConnectivityLoaderFile::writeFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Writing of ConnectivityLoaderFile not supported.");
}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
ConnectivityLoaderFile::toString() const
{
    return "ConnectivityLoaderFile";
}

StructureEnum::Enum 
ConnectivityLoaderFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure.
 * @param structure 
 *    New value for file's structure.
 */
void 
ConnectivityLoaderFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* do nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
ConnectivityLoaderFile::getFileMetaData()
{
    return this->metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
ConnectivityLoaderFile::getFileMetaData() const
{
    return this->metadata;
}

/**
 * @return The palette color mapping for a data column.
 */
PaletteColorMapping* 
ConnectivityLoaderFile::getPaletteColorMapping(const int32_t /*columnIndex*/)
{
    /*
     * Use one palette color mapping for all
     */
    return this->paletteColorMapping;
}

/**
 * @return Is the data mappable to a surface?
 */
bool 
ConnectivityLoaderFile::isSurfaceMappable() const
{
    return true;
}

/**
 * @return Is the data mappable to a volume?
 */
bool 
ConnectivityLoaderFile::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.  
 * Note: Caret5 used the term 'columns'.
 */
int32_t 
ConnectivityLoaderFile::getNumberOfMaps() const
{
    int32_t numMaps = 0;
    
    if (this->ciftiInterface != NULL) {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;
            case LOADER_TYPE_DENSE:
                numMaps = 1;
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
                numMaps = 1;
                break;
        }
    }
    
    return numMaps;
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
ConnectivityLoaderFile::getMapName(const int32_t /*mapIndex*/) const
{
    AString name = "Invalid";
    
    if (this->ciftiInterface != NULL) {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;
            case LOADER_TYPE_DENSE:
                name = "Dense Data";
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
                name = "Dense Time Series";
                break;
        }
    }
    
    return name;
}

/**
 * Find the index of the map that uses the given name.
 * 
 * @param mapName
 *    Name of the desired map.
 * @return
 *    Index of the map using the given name.  If there is more
 *    than one map with the given name, this method is likely
 *    to return the index of the first map with the name.
 */
int32_t 
ConnectivityLoaderFile::getMapIndexFromName(const AString& /*mapName*/)
{
    return 0;
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
ConnectivityLoaderFile::setMapName(const int32_t /*mapIndex*/,
                                   const AString& /*mapName*/)
{
    
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
ConnectivityLoaderFile::getMapUniqueID(const int32_t /*mapIndex*/) const
{
    return this->uniqueID;
}

/**
 * Find the index of the map that uses the given unique ID (UUID).
 * 
 * @param uniqueID
 *    Unique ID (UUID) of the desired map.
 * @return
 *    Index of the map using the given UUID.
 */
int32_t 
ConnectivityLoaderFile::getMapIndexFromUniqueID(const AString& /*uniqueID*/) const
{
    return 0;
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
ConnectivityLoaderFile::getMapMetaData(const int32_t /*mapIndex*/) const
{
    /*
     * One metadata for all
     */
    return this->metadata;
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
ConnectivityLoaderFile::getMapMetaData(const int32_t /*mapIndex*/)
{
    /*
     * One metadata for all
     */
    return this->metadata;
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
ConnectivityLoaderFile::getMapStatistics(const int32_t /*mapIndex*/)
{
    this->descriptiveStatistics->update(this->data, 
                                        this->numberOfDataElements);
    return this->descriptiveStatistics;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool 
ConnectivityLoaderFile::isMappedWithPalette() const
{
    return true;
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
ConnectivityLoaderFile::getMapPaletteColorMapping(const int32_t /*mapIndex*/)
{
    /*
     * One palette mapping for all
     */
    return this->paletteColorMapping;    
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
ConnectivityLoaderFile::getMapPaletteColorMapping(const int32_t /*mapIndex*/) const
{
    /*
     * One palette mapping for all
     */
    return this->paletteColorMapping;    
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool 
ConnectivityLoaderFile::isMappedWithLabelTable() const
{
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
ConnectivityLoaderFile::getMapLabelTable(const int32_t /*mapIndex*/)
{
    /*
     * Use file's label table since GIFTI uses one
     * label table for all data arrays.
     */
    return this->labelTable;
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
ConnectivityLoaderFile::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    /*
     * Use file's label table since GIFTI uses one
     * label table for all data arrays.
     */
    return this->labelTable;
}

/**
 * @return Is this loading dense connectivity data?
 */
bool 
ConnectivityLoaderFile::isDense() const
{
    return (this->loaderType == LOADER_TYPE_DENSE);
}

/**
 * @return Is this loading dense time series connectivity data?
 */
bool 
ConnectivityLoaderFile::isDenseTimeSeries() const
{
    return (this->loaderType == LOADER_TYPE_DENSE_TIME_SERIES);
}

/**
 * @return Name describing loader content.
 */
AString 
ConnectivityLoaderFile::getCiftiTypeName() const
{
    switch (this->loaderType) {
        case LOADER_TYPE_INVALID:
            break;
        case LOADER_TYPE_DENSE:
            return "Dense";
            break;
        case LOADER_TYPE_DENSE_TIME_SERIES:
            return "Dense Time";
            break;
    }
    return "";
}

/**
 * Allocate data.
 * @param numberOfDataElements
 *    Number of elements in the data.
 */
void 
ConnectivityLoaderFile::allocateData(const int32_t numberOfDataElements)
{
    if (numberOfDataElements != this->numberOfDataElements) {
        if (data != NULL) {
            delete [] data;
            this->data = NULL;
        }
        if (dataRGBA != NULL) {
            delete [] this->dataRGBA;
            this->dataRGBA = NULL;
        }
        
        this->numberOfDataElements = numberOfDataElements;
        
        if (numberOfDataElements > 0) {
            this->data = new float[this->numberOfDataElements];
            this->dataRGBA = new float[this->numberOfDataElements * 4];
        }
    }    
}

/**
 * Zero out the data such as when loading data fails.
 */
void 
ConnectivityLoaderFile::zeroizeData()
{
    std::fill(this->data, 
              this->data + this->numberOfDataElements,
              0.0);   
    this->mapToType = MAP_TO_TYPE_INVALID;
}

/**
 * Load connectivity data for the data at the specified time.
 * @param seconds
 *    Time of data in seconds.
 */
void 
ConnectivityLoaderFile::loadTimePointAtTime(const float seconds) throw (DataFileException)
{
    if (this->ciftiInterface == NULL) {
        throw DataFileException("Connectivity Loader has not been initialized");
    }
    
    /*
     * Allow loading of data disable?
     */
    if (this->dataLoadingEnabled == false) {
        return;
    }
    
    this->zeroizeData();
    this->selectedTimePoint = seconds;//TSC: update seconds any time you change data in any way, otherwise it may ignore a request while all you have is zeros from error

    try {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;
            case LOADER_TYPE_DENSE:
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
            {
                const int32_t num = this->ciftiInterface->getNumberOfRows();
                this->allocateData(num);
                
                if (this->ciftiInterface->getColumnFromTimepoint(this->data, seconds)) {
                    this->mapToType = MAP_TO_TYPE_BRAINORDINATES;
                }
                else {
                    CaretLogSevere("FAILED to read column for seconds " + AString::number(seconds));
                }
            }
                break;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}

/**
 * Load connectivity data for the surface's node.  
 *
 * For a dense connectivity file, the data loaded is
 * the connectivity from the node to other brainordinates.
 * For a dense time series file, the data loaded is the
 * time-series for this node.
 *
 * @param surfaceFile
 *    Surface file used for structure.
 * @param nodeIndex
 *    Index of node number.
 */
void 
ConnectivityLoaderFile::loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                               const int32_t nodeIndex) throw (DataFileException)
{
    if (this->ciftiInterface == NULL) {
        throw DataFileException("Connectivity Loader has not been initialized");
    }
        
    /*
     * Allow loading of data disable?
     */
    if (this->dataLoadingEnabled == false) {
        return;
    }
    
    try {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;
            case LOADER_TYPE_DENSE:
            {
                this->zeroizeData();

                const int32_t num = this->ciftiInterface->getNumberOfColumns();
                this->allocateData(num);
                
                if (this->ciftiInterface->hasRowSurfaceData(structure)) {
                    if (this->ciftiInterface->getRowFromNode(this->data,
                                                             nodeIndex,
                                                             structure)) {
                        CaretLogFine("Read row for node " + AString::number(nodeIndex));
                        this->mapToType = MAP_TO_TYPE_BRAINORDINATES;
                    }
                    else {
                        CaretLogFine("FAILED to read row for node " + AString::number(nodeIndex));
                    }
                }
            }
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
            {
                /*this->zeroizeData();

                const int32_t num = this->ciftiInterface->getNumberOfColumns();
                this->allocateData(num);
                
                if (this->ciftiInterface->hasRowSurfaceData(structure)) {
                    if (this->ciftiInterface->getRowFromNode(this->data,
                                                             nodeIndex,
                                                             structure)) {
                        CaretLogFine("Read row for node " + AString::number(nodeIndex));
                        this->mapToType = MAP_TO_TYPE_TIMEPOINTS;
                        if(this->timeSeriesGraphEnabled)
                        {
                            tl.x.clear();
                            tl.y.clear();
                            for(int64_t i = 0;i<num;i++)
                            {
                                tl.x.push_back(i);
                                tl.y.push_back(this->data[i]);
                            }
                            double point[3] = {0.0,0.0,0.0};
                            this->tl.nodeid = nodeIndex;
                        }
                    }
                    else {
                        CaretLogFine("FAILED to read row for node " + AString::number(nodeIndex));
                    }
                }*/
            }
            break;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * For a dense connectivity file, the data loaded is
 * the connectivity from the voxel to other brainordinates.
 * For a dense time series file, the data loaded is the
 * time-series for this voxel.
 *
 * @param xyz
 *    Coordinate of voxel.
 */
void 
ConnectivityLoaderFile::loadDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    if (this->ciftiInterface == NULL) {
        throw DataFileException("Connectivity Loader has not been initialized");
    }
    
    /*
     * Allow loading of data disable?
     */
    if (this->dataLoadingEnabled == false) {
        return;
    }
    
    try {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;
            case LOADER_TYPE_DENSE:
            {
                this->zeroizeData();
                
                const int32_t num = this->ciftiInterface->getNumberOfColumns();
                this->allocateData(num);
                
                if (this->ciftiInterface->hasRowVolumeData()) {
                    if (this->ciftiInterface->getRowFromVoxelCoordinate(this->data, xyz)) {
                        CaretLogFine("Read row for voxel " + AString::fromNumbers(xyz, 3, ","));
                        this->mapToType = MAP_TO_TYPE_BRAINORDINATES;
                    }
                    else {
                        CaretLogFine("FAILED to read row for voxel " + AString::fromNumbers(xyz, 3, ","));
                    }
                }
            }
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
            {
                /*this->zeroizeData();

                const int32_t num = this->ciftiInterface->getNumberOfColumns();
                this->allocateData(num);
                
                if (this->ciftiInterface->hasRowVolumeData()) {
                    if (this->ciftiInterface->getRowFromVoxelCoordinate(this->data,xyz))                                                             
                    {
                        CaretLogFine("Read row for node " + AString::fromNumbers(xyz, 3, ","));
                        this->mapToType = MAP_TO_TYPE_TIMEPOINTS;
                        if(this->timeSeriesGraphEnabled)
                        {
                            tl.x.clear();
                            tl.y.clear();
                            for(int64_t i = 0;i<num;i++)
                            {
                                tl.x.push_back(i);
                                tl.y.push_back(this->data[i]);
                            }
                            double point[3] = {0.0,0.0,0.0};
                            //this->tl.nodeid = nodeIndex;

                        }
                    }
                    else {
                        CaretLogFine("FAILED to read row for node " + AString::fromNumbers(xyz, 3, ","));
                    }
                }*/
            }
            break;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}

/**
 * @return Number of elements in the data that was loaded.
 */
int32_t 
ConnectivityLoaderFile::getNumberOfDataElements() const
{
    return this->numberOfDataElements;
}

/**
 * @return Pointer to data that was loaded which contains
 * "getNumberOfDataElements()" elements.
 */
float* 
ConnectivityLoaderFile::getData()
{
    return this->data;
}

/**
 * @return Pointer to RGBA coloring for data that was loaded which contains
 * "getNumberOfDataElements() * 4" elements.
 */
float* 
ConnectivityLoaderFile::getDataRGBA()
{
    return this->dataRGBA;
}

/**
 * Get connectivity value for a voxel at the given coordinate.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param valueOut
 *     Output containing the node's value, value only if true returned.
 * @return
 *    true if a value was available for the voxel, else false.
 */
bool 
ConnectivityLoaderFile::getVolumeVoxelValue(const float xyz[3],
                                            int64_t ijkOut[3],
                                            float &valueOut) const
{
    if (this->numberOfDataElements <= 0) {
        return false;
    }
    
    if (this->connectivityVolumeFile != NULL) {
        int64_t vfIJK[3];
        this->connectivityVolumeFile->closestVoxel(xyz, 
                         vfIJK);
        
        if (this->connectivityVolumeFile->indexValid(vfIJK[0], vfIJK[1], vfIJK[2])) {
            bool validMapToType = false;
            switch (this->mapToType) {
                case MAP_TO_TYPE_INVALID:
                    validMapToType = false;
                    break;
                case MAP_TO_TYPE_BRAINORDINATES:
                    validMapToType = true;
                    break;
                case MAP_TO_TYPE_TIMEPOINTS:
                    validMapToType = false;
                    break;
            }
            if (validMapToType == false) {
                return false;
            }
            
            bool useColumnsFlag = false;
            bool useRowsFlag = false;
            switch (this->loaderType) {
                case LOADER_TYPE_INVALID:
                    break;
                case LOADER_TYPE_DENSE:
                    useColumnsFlag = true;
                    break;
                case LOADER_TYPE_DENSE_TIME_SERIES:
                    useRowsFlag = true;
                    break;
            }
            
            std::vector<CiftiVolumeMap> volumeMap;
            if (useColumnsFlag
                && this->ciftiInterface->hasColumnVolumeData()) {
                this->ciftiInterface->getVolumeMapForColumns(volumeMap);
            }
            if (useRowsFlag
                && this->ciftiInterface->hasRowVolumeData()) {
                this->ciftiInterface->getVolumeMapForRows(volumeMap);
            }
            
            const int64_t numMaps = static_cast<int64_t>(volumeMap.size());
            for (int64_t i = 0; i < numMaps; i++) {
                if (volumeMap[i].m_ijk[0] == vfIJK[0]
                    && volumeMap[i].m_ijk[1] == vfIJK[1]
                    && volumeMap[i].m_ijk[2] == vfIJK[2]) {
                    CaretAssertArrayIndex(this->data, this->numberOfDataElements, volumeMap[i].m_ciftiIndex);
                    valueOut = this->data[volumeMap[i].m_ciftiIndex];
                    ijkOut[0] = vfIJK[0];
                    ijkOut[1] = vfIJK[1];
                    ijkOut[2] = vfIJK[2];
                    return true;
                }
            }
        }

    }
    
    return false;
}

/**
 * Get connectivity value for a surface's node.
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node
 * @param numberOfNodes
 *     Number of nodes in the surface.
 * @param valueOut
 *     Output containing the node's value, value only if true returned.
 * @return
 *    true if a value was available for the node, else false.
 */
bool 
ConnectivityLoaderFile::getSurfaceNodeValue(const StructureEnum::Enum structure,
                                            const int nodeIndex,
                                            const int32_t numberOfNodes,
                                            float& valueOut) const
{
    if (this->numberOfDataElements <= 0) {
        return false;
    }
    
    bool validMapToType = false;
    switch (this->mapToType) {
        case MAP_TO_TYPE_INVALID:
            validMapToType = false;
            break;
        case MAP_TO_TYPE_BRAINORDINATES:
            validMapToType = true;
            break;
        case MAP_TO_TYPE_TIMEPOINTS:
            validMapToType = false;
            break;
    }
    if (validMapToType == false) {
        return false;
    }
    
    bool useColumnsFlag = false;
    bool useRowsFlag = false;
    switch (this->loaderType) {
        case LOADER_TYPE_INVALID:
            break;
        case LOADER_TYPE_DENSE:
            useColumnsFlag = true;
            break;
        case LOADER_TYPE_DENSE_TIME_SERIES:
            useRowsFlag = true;
            break;
    }
    
    std::vector<CiftiSurfaceMap> nodeMap;
    if (useColumnsFlag  
        && this->ciftiInterface->hasColumnSurfaceData(structure)) {
        const int numCiftiNodes = this->ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
        if (numberOfNodes != numCiftiNodes) {
            return false;
        }
        this->ciftiInterface->getSurfaceMapForColumns(nodeMap, structure);
    }
    if (useRowsFlag 
        && this->ciftiInterface->hasRowSurfaceData(structure)) {
        const int numCiftiNodes = this->ciftiInterface->getRowSurfaceNumberOfNodes(structure);
        if (numberOfNodes != numCiftiNodes) {
            return false;
        }
        this->ciftiInterface->getSurfaceMapForRows(nodeMap, structure);
    }
    
    if (nodeMap.empty() == false) {
        const int64_t numNodeMaps = static_cast<int32_t>(nodeMap.size());
        for (int i = 0; i < numNodeMaps; i++) {
            if (nodeMap[i].m_surfaceNode == nodeIndex) {
                CaretAssertArrayIndex(this->data, this->numberOfDataElements, nodeMap[i].m_ciftiIndex);
                valueOut = this->data[nodeMap[i].m_ciftiIndex];
                return true;
            }
        }
    }
    
    return false;
}

/**
 * Get the node coloring for the surface.
 * @param surface
 *    Surface whose nodes are colored.
 * @param nodeRGBA
 *    Filled with RGBA coloring for the surface's nodes. 
 *    Contains numberOfNodes * 4 elements.
 * @param numberOfNodes
 *    Number of nodes in the surface.
 * @return 
 *    True if coloring is valid, else false.
 */
bool 
ConnectivityLoaderFile::getSurfaceNodeColoring(const StructureEnum::Enum structure,
                                               float* nodeRGBA,
                                               const int32_t numberOfNodes)
{
    if (this->numberOfDataElements <= 0) {
        return false;
    }
    
    bool validMapToType = false;
    switch (this->mapToType) {
        case MAP_TO_TYPE_INVALID:
            validMapToType = false;
            break;
        case MAP_TO_TYPE_BRAINORDINATES:
            validMapToType = true;
            break;
        case MAP_TO_TYPE_TIMEPOINTS:
            validMapToType = false;
            break;
    }
    if (validMapToType == false) {
        return false;
    }
    
    bool useColumnsFlag = false;
    bool useRowsFlag = false;
    switch (this->loaderType) {
        case LOADER_TYPE_INVALID:
            break;
        case LOADER_TYPE_DENSE:
            useColumnsFlag = true;
            break;
        case LOADER_TYPE_DENSE_TIME_SERIES:
            useRowsFlag = true;
            break;
    }
    
    std::vector<CiftiSurfaceMap> nodeMap;
    if (useColumnsFlag  
        && this->ciftiInterface->hasColumnSurfaceData(structure)) {
        const int numCiftiNodes = this->ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
        if (numberOfNodes != numCiftiNodes) {
            CaretLogWarning("CIFTI file "
                            + this->getFileNameNoPath()
                            + " has "
                            + AString::number(numCiftiNodes)
                            + " but surface with structure "
                            + StructureEnum::toGuiName(structure)
                            + " contains "
                            + AString::number(numberOfNodes));
            return false;
        }
        this->ciftiInterface->getSurfaceMapForColumns(nodeMap, structure);
    }
    if (useRowsFlag  
        && this->ciftiInterface->hasRowSurfaceData(structure)) {
        const int numCiftiNodes = this->ciftiInterface->getRowSurfaceNumberOfNodes(structure);
        if (numberOfNodes != numCiftiNodes) {
            CaretLogWarning("CIFTI file "
                            + this->getFileNameNoPath()
                            + " has "
                            + AString::number(numCiftiNodes)
                            + " but surface with structure "
                            + StructureEnum::toGuiName(structure)
                            + " contains "
                            + AString::number(numberOfNodes));
            return false;
        }
        this->ciftiInterface->getSurfaceMapForRows(nodeMap, structure);
    }
    
    if (nodeMap.empty() == false) {
        std::fill(nodeRGBA, (nodeRGBA + (numberOfNodes * 4)), 0.0);
        const int64_t numNodeMaps = static_cast<int32_t>(nodeMap.size());
        for (int i = 0; i < numNodeMaps; i++) {
            const int64_t node4 = nodeMap[i].m_surfaceNode * 4;
            const int64_t cifti4 = nodeMap[i].m_ciftiIndex * 4;
            CaretAssertArrayIndex(nodeRGBA, (numberOfNodes * 4), node4);
            CaretAssertArrayIndex(this->dataRGBA, (this->numberOfDataElements * 4), cifti4);
            nodeRGBA[node4]   = this->dataRGBA[cifti4];
            nodeRGBA[node4+1] = this->dataRGBA[cifti4+1];
            nodeRGBA[node4+2] = this->dataRGBA[cifti4+2];
            nodeRGBA[node4+3] = this->dataRGBA[cifti4+3];
        }
        return true;
    }
    
    return false;
}

/**
 * @return A volume file that contains connectivity values
 * from the last loaded data. Will be NULL if not valid.
 */
VolumeFile* 
ConnectivityLoaderFile::getConnectivityVolumeFile()
{
    if (this->numberOfDataElements <= 0) {
        return NULL;
    }
    
    if (this->ciftiInterface == NULL) {
        return NULL;
    }

    bool validMapToType = false;
    switch (this->mapToType) {
        case MAP_TO_TYPE_INVALID:
            validMapToType = false;
            break;
        case MAP_TO_TYPE_BRAINORDINATES:
            validMapToType = true;
            break;
        case MAP_TO_TYPE_TIMEPOINTS:
            validMapToType = false;
            break;
    }
    if (validMapToType == false) {
        return NULL;
    }

    VolumeFile::OrientTypes orientation[3];
    int64_t dimensions[3];
    float origin[3];
    float spacing[3];
    if (this->ciftiInterface->getVolumeAttributesForPlumb(orientation, 
                                                          dimensions, 
                                                          origin, 
                                                          spacing) == false) {
        return NULL;
    }
    
    if (dimensions[0] <= 0) {
        return NULL;
    }
    
    bool createVolumeFlag = false;
    if (this->connectivityVolumeFile == NULL) {
        createVolumeFlag = true;
    }
    else {
        int64_t dimI, dimJ, dimK, dimTime, numComp;
        this->connectivityVolumeFile->getDimensions(dimI, dimJ, dimK, dimTime, numComp);
        if ((dimI != dimensions[0])
            || (dimJ != dimensions[1])
            || (dimK != dimensions[2])) {
            createVolumeFlag = true;
        }
        else {
            const int64_t zero = 0;
            const int64_t one  = 1;
            float x0, y0, z0, x1, y1, z1;
            this->connectivityVolumeFile->indexToSpace(zero, zero, zero, x0, y0, z0);
            this->connectivityVolumeFile->indexToSpace(one, one, one, x1, y1, z1);
            const float dx = x1 - x0;
            const float dy = y1 - y0;
            const float dz = z1 - z0;
            
            if ((x0 != origin[0])
                || (y0 != origin[1])
                || (z0 != origin[2])) {
                createVolumeFlag = true;
            }
            else if ((dx != spacing[0])
                     || (dy != spacing[1])
                     || (dz != spacing[2])) {
                createVolumeFlag = true;
            }
        }        
    }
    
    if (createVolumeFlag) {
        if (this->connectivityVolumeFile != NULL) {
            delete this->connectivityVolumeFile;
        }
        
        vector<int64_t> dimensionsNew;
        dimensionsNew.push_back(dimensions[0]);
        dimensionsNew.push_back(dimensions[1]);
        dimensionsNew.push_back(dimensions[2]);
        
        std::vector<float> row1;
        row1.push_back(spacing[0]);
        row1.push_back(0.0);
        row1.push_back(0.0);
        row1.push_back(origin[0]);

        std::vector<float> row2;
        row2.push_back(0.0);
        row2.push_back(spacing[1]);
        row2.push_back(0.0);
        row2.push_back(origin[1]);

        std::vector<float> row3;
        row3.push_back(0.0);
        row3.push_back(0.0);
        row3.push_back(spacing[2]);
        row3.push_back(origin[2]);

        vector<vector<float> > indexToSpace;
        indexToSpace.push_back(row1);
        indexToSpace.push_back(row2);
        indexToSpace.push_back(row3);
        
        int64_t numComponents = 1;
        
        this->connectivityVolumeFile = new VolumeFile(dimensionsNew, 
                                              indexToSpace, 
                                              numComponents);
    }
    
    bool useColumnsFlag = false;
    bool useRowsFlag = false;
    switch (this->loaderType) {
        case LOADER_TYPE_INVALID:
            break;
        case LOADER_TYPE_DENSE:
            useColumnsFlag = true;
            break;
        case LOADER_TYPE_DENSE_TIME_SERIES:
            useRowsFlag = true;
            break;
    }
    
    std::vector<CiftiVolumeMap> volumeMaps;
    if (useColumnsFlag
        && this->ciftiInterface->hasColumnVolumeData()) {
        this->ciftiInterface->getVolumeMapForColumns(volumeMaps);
    }
    if (useRowsFlag
        && this->ciftiInterface->hasRowVolumeData()) {
        this->ciftiInterface->getVolumeMapForRows(volumeMaps);
    }
    
    
    if (volumeMaps.empty() == false) {
        this->connectivityVolumeFile->setValueAllVoxels(0.0);
        
        for (std::vector<CiftiVolumeMap>::const_iterator iter = volumeMaps.begin();
             iter != volumeMaps.end();
             iter++) {
            const CiftiVolumeMap& vm = *iter;
            
            CaretAssertArrayIndex(this->data, this->numberOfDataElements, vm.m_ciftiIndex);
            this->connectivityVolumeFile->setValue(this->data[vm.m_ciftiIndex], vm.m_ijk);
        }
        
        return this->connectivityVolumeFile;
    }
    
    return NULL;
}

/**

 * @return
 *   Is the time series graph enabled?
 */
bool 
ConnectivityLoaderFile::isTimeSeriesGraphEnabled() const
{
    return this->timeSeriesGraphEnabled;
}

/**
 * Set time-series graph enabled.
 * @param showGraph
 *   New value for time-series graph.
 */
void 
ConnectivityLoaderFile::setTimeSeriesGraphEnabled(const bool showGraph)
{
    this->timeSeriesGraphEnabled = showGraph;
}




 /* Get the number of time points
 * @return
 *    int64_t timePoints
 */
int64_t 
ConnectivityLoaderFile::getNumberOfTimePoints()
{
    if(this->isDenseTimeSeries()) {
        if (this->ciftiInterface != NULL) {
            return this->ciftiInterface->getNumberOfColumns();
        }
    }
    return -1;
}

/**
 * @return The time step.
 */
float
ConnectivityLoaderFile::getTimeStep() const
{
    if (this->isDenseTimeSeries()) {
        if (this->ciftiInterface != NULL) {
            float timeStep = 0.0;
            if (this->ciftiInterface->getColumnTimestep(timeStep)) {
                return timeStep;
            }
        }
    }
    
    return 0.0;
}

/**
 * @return Get the selected time point.
 */ 
float 
ConnectivityLoaderFile::getSelectedTimePoint() const
{
    return this->selectedTimePoint;
}

/**
  * create TimeLine data for Time Course Dialog
  */
void
ConnectivityLoaderFile::getTimeLine(TimeLine &tlOut)
{
    this->tl.id = (void *)this;
    this->tl.filename = this->getFileName();
    tlOut = this->tl;
}

void 
ConnectivityLoaderFile::loadTimeLineForSurfaceNode(const StructureEnum::Enum structure,
                          const int32_t nodeIndex) throw (DataFileException)
{
    if (this->ciftiInterface == NULL) {
        throw DataFileException("Connectivity Loader has not been initialized");
    }
        
    /*
     * Allow loading of data disable?
     */
    if (this->dataLoadingEnabled == false) {
        return;
    }
    
    try {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;     
            case LOADER_TYPE_DENSE:
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
            {
                const int32_t num = this->ciftiInterface->getNumberOfColumns();                
                float *data = new float [num];
                
                if (this->ciftiInterface->hasRowSurfaceData(structure)) {
                    if (this->ciftiInterface->getRowFromNode(data,
                                                             nodeIndex,
                                                             structure)) {
                        CaretLogFine("Read row for node " + AString::number(nodeIndex));                        
                        if(this->timeSeriesGraphEnabled)
                        {
                            tl.x.clear();
                            tl.y.clear();
                            this->tl.x.reserve(num);
                            this->tl.y.reserve(num);
                            for(int64_t i = 0;i<num;i++)
                            {
                                tl.x.push_back(i);
                                tl.y.push_back(data[i]);
                            }
                            //double point[3] = {0.0,0.0,0.0};
                            this->tl.nodeid = nodeIndex;
                        }
                    }
                    else {
                        CaretLogFine("FAILED to read row for node " + AString::number(nodeIndex));
                    }
                }
                delete [] data;
            }
            break;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }

}

void ConnectivityLoaderFile::loadTimeLineForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    if (this->ciftiInterface == NULL) {
        throw DataFileException("Connectivity Loader has not been initialized");
    }
    
    /*
     * Allow loading of data disable?
     */
    if (this->dataLoadingEnabled == false) {
        return;
    }
    
    try {
        switch (this->loaderType) {
            case LOADER_TYPE_INVALID:
                break;
            case LOADER_TYPE_DENSE:
                break;
            case LOADER_TYPE_DENSE_TIME_SERIES:
            {
                const int32_t num = this->ciftiInterface->getNumberOfColumns();                
                float *data = new float [num];
                if (this->ciftiInterface->hasRowVolumeData()) {
                    if (this->ciftiInterface->getRowFromVoxelCoordinate(data,xyz))                                                             
                    {
                        CaretLogFine("Read row for node " + AString::fromNumbers(xyz, 3, ","));
                        //this->mapToType = MAP_TO_TYPE_TIMEPOINTS;
                        if(this->timeSeriesGraphEnabled)
                        {
                            tl.x.clear();
                            tl.y.clear();
                            this->tl.x.reserve(num);
                            this->tl.y.reserve(num);
                            for(int64_t i = 0;i<num;i++)
                            {
                                tl.x.push_back(i);
                                tl.y.push_back(data[i]);
                            }
                            //double point[3] = {0.0,0.0,0.0};
                            //this->tl.nodeid = nodeIndex;

                        }
                    }
                    else {
                        CaretLogFine("FAILED to read row for node " + AString::fromNumbers(xyz, 3, ","));
                    }
                }
                delete [] data;
            }
            break;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}


