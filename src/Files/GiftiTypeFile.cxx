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

#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "FastStatistics.h"
#include "GiftiDataArray.h"
#include "GiftiFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GiftiTypeFile.h"
#include "GiftiMetaDataXmlElements.h"
#include "Histogram.h"
#include "LabelFile.h"
#include "MapFileDataSelector.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "PaletteColorMappingSaxReader.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 */
GiftiTypeFile::GiftiTypeFile(const DataFileTypeEnum::Enum dataFileType)
: CaretMappableDataFile(dataFileType)
{
    this->initializeMembersGiftiTypeFile();   
}

/**
 * Destructor.
 */
GiftiTypeFile::~GiftiTypeFile()
{
    if (this->giftiFile != NULL) {
        delete this->giftiFile;
        this->giftiFile = NULL;
    }
}

/**
 * Copy Constructor.
 *
 * @param gtf
 *    File that is copied.
 */
GiftiTypeFile::GiftiTypeFile(const GiftiTypeFile& gtf)
: CaretMappableDataFile(gtf)
{
    this->giftiFile = new GiftiFile(*gtf.giftiFile);//NOTE: while CONSTRUCTING, this has virtual type GiftiTypeFile*, NOT MetricFile*, or whatever
}//so, validateDataArraysAfterReading will ABORT due to pure virtual

/**
 * Assignment operator.
 *
 * @param gtf
 *     File whose contents are copied to this file.
 */
GiftiTypeFile& 
GiftiTypeFile::operator=(const GiftiTypeFile& gtf)
{
    if (this != &gtf) {
        CaretMappableDataFile::operator=(gtf);
        this->copyHelperGiftiTypeFile(gtf);
    }
    return *this;
}

/**
 * Clear the contents of this file.
 */
void 
GiftiTypeFile::clear()
{
    DataFile::clear();
    this->giftiFile->clear(); 
}

/**
 * Clear modified status.
 */
void 
GiftiTypeFile::clearModified()
{
    CaretDataFile::clearModified();
    this->giftiFile->clearModified();
}

/**
 * @return True if any of the maps in this file contain a
 * color mapping that possesses a modified status.
 */
bool
GiftiTypeFile::isModifiedExcludingPaletteColorMapping() const
{
    if (CaretMappableDataFile::isModifiedExcludingPaletteColorMapping()) {
        return true;
    }
    
    if (this->giftiFile->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
GiftiTypeFile::isEmpty() const
{
    return this->giftiFile->isEmpty();
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
GiftiTypeFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    this->setFileName(filename);
    this->giftiFile->readFile(filename);
    this->validateDataArraysAfterReading();
    updateAfterFileDataChanges();
    this->clearModified();
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
GiftiTypeFile::writeFile(const AString& filename)
{
    checkFileWritability(filename);
    this->giftiFile->writeFile(filename);
    this->clearModified();
}
/**
 * Helps with file copying.
 * 
 * @param gtf
 *    File that is copied.
 */
void 
GiftiTypeFile::copyHelperGiftiTypeFile(const GiftiTypeFile& gtf)
{
    if (this->giftiFile != NULL) {
        delete this->giftiFile;
    }
    this->giftiFile = new GiftiFile(*gtf.giftiFile);
    this->validateDataArraysAfterReading();
}

/**
 * Initialize members of this class.
 */
void 
GiftiTypeFile::initializeMembersGiftiTypeFile()
{
    this->giftiFile = new GiftiFile();
}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
GiftiTypeFile::toString() const
{
    return this->giftiFile->toString();
}

StructureEnum::Enum 
GiftiTypeFile::getStructure() const
{
    AString structurePrimaryName;
    
    /*
     * Surface contains anatomical structure in pointset array.
     */
    const SurfaceFile* surfaceFile = dynamic_cast<const SurfaceFile*>(this);
    if (surfaceFile != NULL) {
        const GiftiDataArray* gda = this->giftiFile->getDataArrayWithIntent(NiftiIntentEnum::NIFTI_INTENT_POINTSET);
        const GiftiMetaData* metadata = gda->getMetaData();
        structurePrimaryName = metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY);
    }
    else {
        const GiftiMetaData* metadata = this->giftiFile->getMetaData();
        structurePrimaryName = metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY);
    }
    
    bool isValid = false;
    StructureEnum::Enum structure = StructureEnum::fromGuiName(structurePrimaryName, &isValid);
    return structure;
}

/**
 * Set the structure.
 * @param structure 
 *    New value for file's structure.
 */
void 
GiftiTypeFile::setStructure(const StructureEnum::Enum structure)
{
    const AString structureName = StructureEnum::toGuiName(structure);
    /*
     * Surface contains anatomical structure in pointset array.
     */
    SurfaceFile* surfaceFile = dynamic_cast<SurfaceFile*>(this);
    if (surfaceFile != NULL) {
        GiftiDataArray* gda = this->giftiFile->getDataArrayWithIntent(NiftiIntentEnum::NIFTI_INTENT_POINTSET);
        GiftiMetaData* metadata = gda->getMetaData();
        metadata->set(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY,
                      structureName);
    }
    else {
        GiftiMetaData* metadata = this->giftiFile->getMetaData();
        metadata->set(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY,
                      structureName);
    }
    
}

/**
 * Add map(s) to this GIFTI file.
 * @param numberOfNodes
 *     Number of nodes.  If file is not empty, this value must
 *     match the number of nodes that are in the file.
 * @param numberOfMaps
 *     Number of maps to add.
 */
void 
GiftiTypeFile::addMaps(const int32_t /*numberOfNodes*/,
                       const int32_t /*numberOfMaps*/)
{
    throw DataFileException(getFileName(),
                            "This file does not support adding additional maps");
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
GiftiTypeFile::getFileMetaData()
{
    return this->giftiFile->getMetaData();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
GiftiTypeFile::getFileMetaData() const
{
    return this->giftiFile->getMetaData();
}

/**
 * Verify that all of the data arrays have the same number of rows.
 * @throws DataFileException
 *    If there are data arrays that have a different number of rows.
 */
void 
GiftiTypeFile::verifyDataArraysHaveSameNumberOfRows(const int32_t minimumSecondDimension,
                                                    const int32_t maximumSecondDimension) const
{
    const int32_t numberOfArrays = this->giftiFile->getNumberOfDataArrays();
    if (numberOfArrays > 1) {
        /*
         * Verify all arrays contain the same number of rows.
         */
        int64_t numberOfRows = this->giftiFile->getDataArray(0)->getNumberOfRows();        
        for (int32_t i = 1; i < numberOfArrays; i++) {
            const int32_t arrayNumberOfRows = this->giftiFile->getDataArray(i)->getNumberOfRows();
            if (numberOfRows != arrayNumberOfRows) {
                AString message = "All data arrays (columns) in the file must have the same number of rows.";
                message += "  The first array (column) contains " + AString::number(numberOfRows) + " rows.";
                message += "  Array " + AString::number(i + 1) + " contains " + AString::number(arrayNumberOfRows) + " rows.";
                DataFileException e(getFileName(),
                                    message);
                CaretLogThrowing(e);
                throw e;                                     
            }
        }
        
        /*
         * Verify that second dimensions is within valid range.
         */
        for (int32_t i = 0; i < numberOfArrays; i++) {
            const GiftiDataArray* gda = this->giftiFile->getDataArray(i);
            const int32_t numberOfDimensions = gda->getNumberOfDimensions();
            if (numberOfDimensions > 2) {
                DataFileException e(getFileName(),
                                    "Data array "
                                    + AString::number(i + 1)
                                    + " contains "
                                    + AString::number(numberOfDimensions)
                                    + " dimensions.  Two is the maximum allowed.");
                CaretLogThrowing(e);
                throw e;
            }
            
            int32_t secondDimension = 0;
            if (numberOfDimensions > 1) {
                secondDimension = gda->getDimension(1);
                if (secondDimension == 1) {
                    secondDimension = 0;
                }
            }
            
            if ((secondDimension < minimumSecondDimension) 
                || (secondDimension > maximumSecondDimension)) {
                DataFileException e(getFileName(),
                                    "Data array "
                                    + AString::number(i + 1)
                                    + " second dimension is "
                                    + AString::number(numberOfDimensions)
                                    + ".  Minimum allowed is "
                                    + AString::number(minimumSecondDimension)
                                    + ".  Maximum allowed is "
                                    + AString::number(maximumSecondDimension));
                CaretLogThrowing(e);
                throw e;
            }
        }
    }
}
   
/**
 * Get the name of a file column.
 * @param columnIndex
 *    Index of column.
 * @return
 *    Name of column.
 */
AString 
GiftiTypeFile::getColumnName(const int columnIndex) const
{
    return this->giftiFile->getDataArrayName(columnIndex);
}

/**
 * Find the first column with the given column name.
 * @param columnName
 *     Name of column.
 * @return
 *     Index of column with name or negative if no match.
 */
int32_t 
GiftiTypeFile::getColumnIndexFromColumnName(const AString& columnName) const
{
    return this->giftiFile->getDataArrayWithNameIndex(columnName);
}

/**
 * Set the name of a column.
 * @param columnIndex
 *    Index of column.
 * @param columnName
 *    New name for column.
 */
void 
GiftiTypeFile::setColumnName(const int32_t columnIndex,
                             const AString& columnName)
{
    this->giftiFile->setDataArrayName(columnIndex, columnName);
}

/**
 * @return The palette color mapping for a data column.
 */
PaletteColorMapping* 
GiftiTypeFile::getPaletteColorMapping(const int32_t columnIndex)
{
    GiftiDataArray* gda = this->giftiFile->getDataArray(columnIndex);
    return gda->getPaletteColorMapping();
}

/**
 * @return The palette color mapping for a data column.
 */
const PaletteColorMapping* 
GiftiTypeFile::getPaletteColorMapping(const int32_t columnIndex) const
{
    const GiftiDataArray* gda = this->giftiFile->getDataArray(columnIndex);
    return gda->getPaletteColorMapping();
}

/**
 * @return Is the data mappable to a surface?
 */
bool 
GiftiTypeFile::isSurfaceMappable() const
{
    return true;
}

/**
 * @return Is the data mappable to a volume?
 */
bool 
GiftiTypeFile::isVolumeMappable() const
{
    return false;
}

/**
 * @return The number of maps in the file.  
 * Note: Caret5 used the term 'columns'.
 */
int32_t 
GiftiTypeFile::getNumberOfMaps() const
{
    return this->giftiFile->getNumberOfDataArrays();
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
GiftiTypeFile::getMapName(const int32_t mapIndex) const
{
    return this->giftiFile->getDataArrayName(mapIndex);
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
GiftiTypeFile::setMapName(const int32_t mapIndex,
                        const AString& mapName)
{
    this->giftiFile->setDataArrayName(mapIndex, mapName);
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
GiftiTypeFile::getMapMetaData(const int32_t mapIndex) const
{
    return this->giftiFile->getDataArray(mapIndex)->getMetaData();
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
GiftiTypeFile::getMapMetaData(const int32_t mapIndex)
{
    return this->giftiFile->getDataArray(mapIndex)->getMetaData();
}

const FastStatistics* GiftiTypeFile::getMapFastStatistics(const int32_t mapIndex)
{
    const GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getFastStatistics();
}

const Histogram* GiftiTypeFile::getMapHistogram(const int32_t mapIndex)
{
    int32_t numberOfBuckets = 0;
    switch (getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            numberOfBuckets = getFileHistogramNumberOfBuckets();
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
            break;
    }
    
    const GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getHistogram(numberOfBuckets);
}

const Histogram* GiftiTypeFile::getMapHistogram(const int32_t mapIndex,
                                                const float mostPositiveValueInclusive,
                                                const float leastPositiveValueInclusive,
                                                const float leastNegativeValueInclusive,
                                                const float mostNegativeValueInclusive,
                                                const bool includeZeroValues)
{
    int32_t numberOfBuckets = 0;
    switch (getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            numberOfBuckets = getFileHistogramNumberOfBuckets();
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
            break;
    }
    const GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getHistogram(numberOfBuckets,
                             mostPositiveValueInclusive,
                             leastPositiveValueInclusive,
                             leastNegativeValueInclusive,
                             mostNegativeValueInclusive,
                             includeZeroValues);
}

/**
 * @return The estimated size of data after it is uncompressed
 * and loaded into RAM.  A negative value indicates that the
 * file size cannot be computed.
 */
int64_t
GiftiTypeFile::getDataSizeUncompressedInBytes() const
{
    const int32_t numDataArrays = getNumberOfMaps();
    
    int64_t dataSizeInBytes = 0;
    
    for (int32_t iMap = 0; iMap < numDataArrays; iMap++) {
        const GiftiDataArray* gda = this->giftiFile->getDataArray(iMap);
        dataSizeInBytes += gda->getDataSizeInBytes();
    }
    
    return dataSizeInBytes;
}

/**
 * Get all data for a file that contains floats.  If the file is very
 * large this method may take a large amount of time!
 *
 * @param dataOut
 *    Output with all data for a float file.  Empty if no data in file
 *    or data is not float.
 */
void
GiftiTypeFile::getFileDataFloat(std::vector<float>& dataOut) const
{
    int64_t dataSize = 0;
    
    /*
     * Get the size of the data
     */
    const int64_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    for (int64_t i = 0; i < numberOfDataArrays; i++) {
        const GiftiDataArray* gda = this->giftiFile->getDataArray(i);
        if (gda->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
            dataSize += gda->getTotalNumberOfElements();
        }
        else {
            dataOut.clear();
            return;
        }
    }
    
    if (dataSize <= 0) {
        dataOut.clear();
        return;
    }
    
    dataOut.resize(dataSize);
    int64_t dataOffset = 0;
    
    /*
     * Copy the data.
     */
    for (int64_t i = 0; i < numberOfDataArrays; i++) {
        const GiftiDataArray* gda = this->giftiFile->getDataArray(i);
        const int64_t arraySize = gda->getTotalNumberOfElements();
        const float* arrayPointer = gda->getDataPointerFloat();
        
        for (int64_t j = 0; j < arraySize; j++) {
            CaretAssertVectorIndex(dataOut, dataOffset);
            dataOut[dataOffset] = arrayPointer[j];
            ++dataOffset;
        }
    }
    
    CaretAssert(dataOffset == static_cast<int64_t>(dataOut.size()));
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
GiftiTypeFile::getFileFastStatistics()
{
    if (m_fileFastStatistics == NULL) {
        std::vector<float> fileData;
        getFileDataFloat(fileData);
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
GiftiTypeFile::getFileHistogram()
{
    const int32_t numBuckets = getFileHistogramNumberOfBuckets();
    bool updateHistogramFlag = false;
    if (m_fileHistogram != NULL) {
        if (numBuckets != m_histogramNumberOfBuckets) {
            updateHistogramFlag = true;
        }
    }
    else {
        updateHistogramFlag = true;
        
    }
    
    if (updateHistogramFlag) {
        std::vector<float> fileData;
        getFileDataFloat(fileData);
        if ( ! fileData.empty()) {
            if (m_fileHistogram == NULL) {
                m_fileHistogram.grabNew(new Histogram(numBuckets));
            }
            m_fileHistogram->update(numBuckets,
                                    &fileData[0],
                                    fileData.size());
            m_histogramNumberOfBuckets = numBuckets;
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
GiftiTypeFile::getFileHistogram(const float mostPositiveValueInclusive,
                                           const float leastPositiveValueInclusive,
                                           const float leastNegativeValueInclusive,
                                           const float mostNegativeValueInclusive,
                                           const bool includeZeroValues)
{
    const int32_t numberOfBuckets = getFileHistogramNumberOfBuckets();
    bool updateHistogramFlag = false;
    if (m_fileHistorgramLimitedValues != NULL) {
        if ((numberOfBuckets != m_fileHistogramLimitedValuesNumberOfBuckets)
            || (mostPositiveValueInclusive != m_fileHistogramLimitedValuesMostPositiveValueInclusive)
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
        getFileDataFloat(fileData);
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
            
            m_fileHistogramLimitedValuesNumberOfBuckets = numberOfBuckets;
            m_fileHistogramLimitedValuesMostPositiveValueInclusive  = mostPositiveValueInclusive;
            m_fileHistogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
            m_fileHistogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
            m_fileHistogramLimitedValuesMostNegativeValueInclusive  = mostNegativeValueInclusive;
            m_fileHistogramLimitedValuesIncludeZeroValues           = includeZeroValues;
        }
    }
    
    return m_fileHistorgramLimitedValues;
//    bool updateHistogramFlag = false;
//    if (m_fileHistorgramLimitedValues != NULL) {
//        if ((mostPositiveValueInclusive != m_fileHistogramLimitedValuesMostPositiveValueInclusive)
//            || (leastPositiveValueInclusive != m_fileHistogramLimitedValuesLeastPositiveValueInclusive)
//            || (leastNegativeValueInclusive != m_fileHistogramLimitedValuesLeastNegativeValueInclusive)
//            || (mostNegativeValueInclusive != m_fileHistogramLimitedValuesMostNegativeValueInclusive)
//            || (includeZeroValues != m_fileHistogramLimitedValuesIncludeZeroValues)) {
//            updateHistogramFlag = true;
//        }
//    }
//    else {
//        updateHistogramFlag = true;
//    }
//    
//    if (updateHistogramFlag) {
//        std::vector<float> fileData;
//        getFileDataFloat(fileData);
//        if ( ! fileData.empty()) {
//            if (m_fileHistorgramLimitedValues == NULL) {
//                m_fileHistorgramLimitedValues.grabNew(new Histogram());
//            }
//            m_fileHistorgramLimitedValues->update(getFileHistogramNumberOfBuckets(),
//                                                  &fileData[0],
//                                                  fileData.size(),
//                                                  mostPositiveValueInclusive,
//                                                  leastPositiveValueInclusive,
//                                                  leastNegativeValueInclusive,
//                                                  mostNegativeValueInclusive,
//                                                  includeZeroValues);
//            
//            m_fileHistogramLimitedValuesMostPositiveValueInclusive  = mostPositiveValueInclusive;
//            m_fileHistogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
//            m_fileHistogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
//            m_fileHistogramLimitedValuesMostNegativeValueInclusive  = mostNegativeValueInclusive;
//            m_fileHistogramLimitedValuesIncludeZeroValues           = includeZeroValues;
//        }
//    }
//    
//    return m_fileHistorgramLimitedValues;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool 
GiftiTypeFile::isMappedWithPalette() const
{
    bool paletteFlag(false);
    
    switch (getDataFileType()) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
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
            paletteFlag = true;
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            paletteFlag = true;
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
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }
    
    return paletteFlag;
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
GiftiTypeFile::getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const
{
    modesSupportedOut.clear();
    
    switch (getDataFileType()) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
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
            modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
            modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
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
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }
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
GiftiTypeFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getPaletteColorMapping();    
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
GiftiTypeFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    const GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getPaletteColorMapping();
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool 
GiftiTypeFile::isMappedWithLabelTable() const
{
    if (this->getDataFileType() == DataFileTypeEnum::LABEL) {
        return true;
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
GiftiTypeFile::getMapLabelTable(const int32_t /*mapIndex*/)
{
    /*
     * Use file's label table since GIFTI uses one
     * label table for all data arrays.
     */
    return this->giftiFile->getLabelTable();
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
GiftiTypeFile::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    /*
     * Use file's label table since GIFTI uses one
     * label table for all data arrays.
     */
    return this->giftiFile->getLabelTable();
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
GiftiTypeFile::getMapUniqueID(const int32_t mapIndex) const
{
    const GiftiMetaData* md = this->giftiFile->getDataArray(mapIndex)->getMetaData();
    return md->getUniqueID();    
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
GiftiTypeFile::getMapIndexFromUniqueID(const AString& uniqueID) const
{
    const int32_t numberOfArrays = this->giftiFile->getNumberOfDataArrays();
    for (int32_t i = 0; i < numberOfArrays; i++) {
        if (this->getMapUniqueID(i) == uniqueID) {
            return i;
        }
    }
    
    return -1;
}

/**
 * Update coloring for a map.
 *
 * @param mapIndex
 *    Index of map.
 * @param paletteFile
 *    Palette file containing palettes.
 */
void
GiftiTypeFile::updateScalarColoringForMap(const int32_t mapIndex)
{
    invalidateHistogramChartColoring();
    if ((mapIndex >= 0)
        && (mapIndex < getNumberOfMaps())) {
        this->giftiFile->getDataArray(mapIndex)->invalidateHistograms();
    }
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
GiftiTypeFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretMappableDataFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Number of Vertices",
                                        getNumberOfNodes());
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
GiftiTypeFile::getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                  std::vector<float>& dataOut) const
{
    dataOut.clear();
    
    switch (mapFileDataSelector.getDataSelectionType()) {
        case MapFileDataSelector::DataSelectionType::INVALID:
        case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
        case MapFileDataSelector::DataSelectionType::ROW_DATA:
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t numberOfNodes(-1);
            int32_t nodeIndex(-1);
            mapFileDataSelector.getSurfaceVertex(structure, numberOfNodes, nodeIndex);
            if ((getStructure() == structure)
                && (getNumberOfNodes() == numberOfNodes)) {
                const int32_t numberOfMaps = getNumberOfMaps();
                if (isMappedWithLabelTable()) {
                    const LabelFile* lf = dynamic_cast<const LabelFile*>(this);
                    CaretAssert(lf);
                    for (int32_t mapIndex = 0; mapIndex < numberOfMaps; mapIndex++) {
                        const int32_t key = lf->getLabelKey(nodeIndex,
                                                            mapIndex);
                        dataOut.push_back(key);
                    }
                }
                if (isMappedWithPalette()) {
                    const MetricFile* mf = dynamic_cast<const MetricFile*>(this);
                    CaretAssert(mf);
                    for (int32_t mapIndex = 0; mapIndex < numberOfMaps; mapIndex++) {
                        const float value = mf->getValue(nodeIndex,
                                                         mapIndex);
                        dataOut.push_back(value);
                    }
                }
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t numberOfNodes(-1);
            std::vector<int32_t> nodeIndices;
            mapFileDataSelector.getSurfaceVertexAverage(structure, numberOfNodes, nodeIndices);
            if ((getStructure() == structure)
                && (getNumberOfNodes() == numberOfNodes)) {
                if (isMappedWithPalette()) {
                    const MetricFile* mf = dynamic_cast<const MetricFile*>(this);
                    CaretAssert(mf);
                    const int32_t numberOfNodeIndices = static_cast<int32_t>(nodeIndices.size());
                    if (numberOfNodeIndices > 0) {
                        const int32_t numberOfMaps = getNumberOfMaps();
                        if (numberOfMaps > 0) {
                            for (int32_t iNode = 0; iNode < numberOfNodeIndices; iNode++) {
                                CaretAssertVectorIndex(nodeIndices, iNode);
                                const int32_t nodeIndex = nodeIndices[iNode];
                                
                                float sum(0.0f);
                                for (int32_t mapIndex = 0; mapIndex < numberOfMaps; mapIndex++) {
                                    const float value = mf->getValue(nodeIndex,
                                                                     mapIndex);
                                    sum += value;
                                }
                                
                                const float value = sum / numberOfMaps;
                                dataOut.push_back(value);
                            }
                        }

                    }
                }
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
            break;
    }
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
GiftiTypeFile::getBrainordinateMappingMatch(const CaretMappableDataFile* mapFile) const
{
    bool giftiFlag = false;
    CaretAssert(mapFile);
    switch (mapFile->getDataFileType()) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
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
            giftiFlag = true;
            break;
        case DataFileTypeEnum::METRIC:
            giftiFlag = true;
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            giftiFlag = true;
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            giftiFlag = true;
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            giftiFlag = true;
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }
    
    if (giftiFlag) {
        const GiftiTypeFile* gtf = dynamic_cast<const GiftiTypeFile*>(mapFile);
        CaretAssert(gtf);
        if ((this->getNumberOfNodes() == gtf->getNumberOfNodes())
            && (this->getStructure() == gtf->getStructure())) {
            return BrainordinateMappingMatch::EQUAL;
        }
    }
    
    return BrainordinateMappingMatch::NO;
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
GiftiTypeFile::getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                           const StructureEnum::Enum structure,
                                                           const int nodeIndex,
                                                           const int32_t numberOfNodes,
                                                           AString& textOut) const
{
    textOut.clear();
    
    if ((getStructure() == structure)
        && (getNumberOfNodes() == numberOfNodes)) {
        switch (getDataFileType()) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
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
            {
                const LabelFile* lf = dynamic_cast<const LabelFile*>(this);
                CaretAssert(lf);
                const GiftiLabelTable* labelTable = lf->getLabelTable();
                AString valuesText;
                for (const auto mapIndex : mapIndices) {
                    const int32_t key = lf->getLabelKey(nodeIndex,
                                                        mapIndex);
                    if (key >= 0) {
                        if ( ! valuesText.isEmpty()) {
                            valuesText.append(", ");
                        }
                        valuesText.append(labelTable->getLabel(key)->getName());
                    }
                }
                
                if ( ! valuesText.isEmpty()) {
                    textOut = valuesText;
                }
            }
                break;
            case DataFileTypeEnum::METRIC:
            case DataFileTypeEnum::METRIC_DYNAMIC: // subclass of METRIC
            {
                const MetricFile* mf = dynamic_cast<const MetricFile*>(this);
                CaretAssert(mf);
                AString valuesText;
                for (const auto mapIndex : mapIndices) {
                    const float value = mf->getValue(nodeIndex,
                                                     mapIndex);
                    if ( ! valuesText.isEmpty()) {
                        valuesText.append(", ");
                    }
                    valuesText.append(AString::number(value, 'f', 3));
                }
                
                if ( ! valuesText.isEmpty()) {
                    textOut = valuesText;
                }
            }
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
            case DataFileTypeEnum::VOLUME_DYNAMIC:
                break;
        }
    }
    
    return ( ! textOut.isEmpty());
}


