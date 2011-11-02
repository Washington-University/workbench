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

#include "CaretLogger.h"
#include "DescriptiveStatistics.h"
#include "ElapsedTimer.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "ConnectivityLoaderFile.h"
#include "PaletteColorMapping.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 */
ConnectivityLoaderFile::ConnectivityLoaderFile()
: CaretMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE)
{
    this->descriptiveStatistics = new DescriptiveStatistics();
    
    this->paletteColorMapping = new PaletteColorMapping();
    
    this->labelTable = new GiftiLabelTable();
    
    this->metadata = new GiftiMetaData();
}

/**
 * Destructor.
 */
ConnectivityLoaderFile::~ConnectivityLoaderFile()
{
    delete this->descriptiveStatistics;
    delete this->paletteColorMapping;
    delete this->labelTable;
    delete this->metadata;
}

/**
 * Clear the contents of this file.
 */
void 
ConnectivityLoaderFile::clear()
{
    CaretMappableDataFile::clear();
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
 * Setup the loader.
 * @param filename
 *    Name of file from which data is obtained.
 * @param connectivityFileType
 *    Type of data.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
ConnectivityLoaderFile::setup(const AString& filename,
                              const DataFileTypeEnum::Enum connectivityFileType) throw (DataFileException)
{
    this->clear();
    
    /*
     * Make sure type is valid
     */
    std::vector<DataFileTypeEnum::Enum> connectivityDataTypes;
    DataFileTypeEnum::getAllConnectivityEnums(connectivityDataTypes);
    if (std::find(connectivityDataTypes.begin(),
                  connectivityDataTypes.end(),
                  connectivityFileType) == connectivityDataTypes.end()) {
        const AString msg = "Unacceptable connectivity file type: "
            + DataFileTypeEnum::toName(connectivityFileType);
        throw DataFileException(msg);
    }
    
    this->setFileName(filename);
    this->setDataFileType(connectivityFileType);
    
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
ConnectivityLoaderFile::readFile(const AString& filename) throw (DataFileException)
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
ConnectivityLoaderFile::writeFile(const AString& filename) throw (DataFileException)
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
ConnectivityLoaderFile::setStructure(const StructureEnum::Enum structure)
{
    /* do nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
ConnectivityLoaderFile::getFileMetaData()
{
    return NULL;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
ConnectivityLoaderFile::getFileMetaData() const
{
    return NULL;
}

/**
 * @return The palette color mapping for a data column.
 */
PaletteColorMapping* 
ConnectivityLoaderFile::getPaletteColorMapping(const int32_t columnIndex)
{
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
    return -1;
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
ConnectivityLoaderFile::getMapName(const int32_t mapIndex) const
{
    return "Map " + AString::number(mapIndex + 1);
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
ConnectivityLoaderFile::getMapIndexFromName(const AString& mapName)
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
ConnectivityLoaderFile::setMapName(const int32_t mapIndex,
                        const AString& mapName)
{
    
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
ConnectivityLoaderFile::getMapMetaData(const int32_t mapIndex) const
{
    return NULL;
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
ConnectivityLoaderFile::getMapMetaData(const int32_t mapIndex)
{
    return NULL;
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
ConnectivityLoaderFile::getMapStatistics(const int32_t mapIndex)
{
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
ConnectivityLoaderFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
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
ConnectivityLoaderFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
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
    return (this->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE);
}

/**
 * @return Is this loading dense time series connectivity data?
 */
bool 
ConnectivityLoaderFile::isDenseTimeSeries() const
{
    return (this->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
}

/**
 * @return Name describing loader content.
 */
AString 
ConnectivityLoaderFile::getCiftiTypeName() const
{
    if (this->isEmpty() == false) {
        if (this->isDense()) {
            return "Dense";
        }
        else if (this->isDenseTimeSeries()) {
            return "Dense Time";
        }
        else {
            return "Unknown Type";
        }
    }
    else {
        return "";
    }
}

/**
 * Load connectivity data for the surface's node.
 * @param surfaceFile
 *    Surface file used for structure.
 * @param nodeIndex
 *    Index of node number.
 */
void 
ConnectivityLoaderFile::loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                            const int32_t nodeIndex) throw (DataFileException)
{
    std::cout << "Connectivity Load Surface: "
    << surfaceFile->getFileName()
    << " Node: "
    << nodeIndex
    << std::endl;
}

/**
 * Load data for a voxel at the given coordinate.
 * @param xyz
 *    Coordinate of voxel.
 */
void 
ConnectivityLoaderFile::loadDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    std::cout << "Connectivity Load Voxel: "
    << AString::fromNumbers(xyz, 3, ", ")
    << std::endl;
}


