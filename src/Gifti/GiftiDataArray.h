
#ifndef __GIFTI_DATA_ARRAY_H__
#define __GIFTI_DATA_ARRAY_H__

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

#include <map>
#include <ostream>
#include <AString.h>
#include <vector>

#include <stdint.h>

#include "CaretObject.h"
#include "CaretPointer.h"
#include "DescriptiveStatistics.h"
#include "FastStatistics.h"
#include "GiftiArrayIndexingOrderEnum.h"
#include "GiftiEncodingEnum.h"
#include "GiftiEndianEnum.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "Histogram.h"
#include "Matrix4x4.h"
#include "NiftiEnums.h"
#include "TracksModificationInterface.h"



namespace caret {
    
    class GiftiFile;
    class GiftiException;
    class PaletteColorMapping;
    
    /// class GiftiDataArray.
    class GiftiDataArray : public CaretObject, TracksModificationInterface {
        
    public:            
        // constructor
        GiftiDataArray(const NiftiIntentEnum::Enum intentIn,
                       const NiftiDataTypeEnum::Enum dataTypeIn,
                       const std::vector<int64_t>& dimensionsIn,
                       const GiftiEncodingEnum::Enum encodingIn = GiftiEncodingEnum::ASCII);
        
        // constructor used when reading data
        GiftiDataArray(const NiftiIntentEnum::Enum intentIn);
        
        // copy constructor
        GiftiDataArray(const GiftiDataArray& nda);
        
        // assignment operator
        GiftiDataArray& operator=(const GiftiDataArray& nda);
        
        // destructor
        virtual ~GiftiDataArray();
        
        // add rows (increase 1st dimension)
        void addRows(const int32_t numRowsToAdd);
        
        // delete rows
        void deleteRows(const std::vector<int32_t>& rowsToDelete);
        
        // convert all data arrays to data type
        void convertToDataType(const NiftiDataTypeEnum::Enum newDataType) ;
        
        // set the dimensions 
        void setDimensions(const std::vector<int64_t> dimensionsIn);
        
        // reset column
        virtual void clear();
        
        /// get the number of dimensions
        int32_t getNumberOfDimensions() const { return dimensions.size(); }
        
        /// get the dimensions
        std::vector<int64_t> getDimensions() const { return dimensions; }
        
        /// current size of the data (in bytes)
        int64_t getDataSizeInBytes() const { return data.size(); }
        
        /// get a dimension
        int32_t getDimension(const int32_t dimIndex) const { return dimensions[dimIndex]; }
        
        // get the number of rows (1st dimension)
        int64_t getNumberOfRows() const;
        
        // get number of components per node (2nd dimension)
        int64_t getNumberOfComponents() const;
        
        // get the total number of elements
        int64_t getTotalNumberOfElements() const;
        
        // get data offset 
        //int64_t getDataOffset(const int64_t nodeNum, const int64_t componentNum) const;//TSC: implementation was wrong, commenting out for now
        
        // read a data array from text
        void readFromText(const AString text,
                          const GiftiEndianEnum::Enum dataEndianForReading,
                          const GiftiArrayIndexingOrderEnum::Enum arraySubscriptingOrderForReading,
                          const NiftiDataTypeEnum::Enum dataTypeForReading,
                          const std::vector<int64_t>& dimensionsForReading,
                          const GiftiEncodingEnum::Enum encodingForReading,
                          const AString& externalFileNameForReading,
                          const int64_t externalFileOffsetForReading,
                          const bool isReadOnlyMetaData);
        
        // write the data as XML
        void writeAsXML(std::ostream& stream, 
                        std::ostream* externalBinaryOutputStream,
                        GiftiEncodingEnum::Enum encodingForWriting);
        
        /// get endian
        GiftiEndianEnum::Enum getEndian() const { return endian; }
        
        // set endian
        void setEndian(const GiftiEndianEnum::Enum e) { endian = e; }
        
        
        /// get the system's endian
        static GiftiEndianEnum::Enum getSystemEndian();
        
        
        // get external file information
        void getExternalFileInformation(AString& nameOut,
                                        int64_t& offsetOut) const;
        
        // set external file information
        void setExternalFileInformation(const AString& nameIn,
                                        const int64_t offsetIn);
        
        /// get the metadata
        GiftiMetaData* getMetaData() { return &metaData; }
        
        /// get the metadata (const method)
        const GiftiMetaData* getMetaData() const { return &metaData; }
        
        /// set the metadata
        void setMetaData(const GiftiMetaData* gmd) { metaData = *gmd; setModified(); }
        
        /// get the number of matrices
        int32_t getNumberOfMatrices() const { return matrices.size(); }
        
        /// add a matrix
        void addMatrix(const Matrix4x4& gm) { matrices.push_back(gm); }
        
        /// get a matrix
        Matrix4x4* getMatrix(const int32_t indx) { return &matrices[indx]; }
        
        /// remove all matrices
        void removeAllMatrices();
        
        /// remove a matrix
        void removeMatrix(const int32_t indx);
        
        /// get the matrix (const method)
        const Matrix4x4* getMatrix(const int32_t indx) const { return &matrices[indx]; }
        
        /// get the non-written metadata for values not saved to file
        //GiftiMetaData* getNonWrittenMetaData() { return &nonWrittenMetaData; }
        
        /// get the non-written metadata for values not save to file (const method)
        //const GiftiMetaData* getNonWrittenMetaData() const { return &nonWrittenMetaData; }
        
        /// get the data type
        NiftiDataTypeEnum::Enum getDataType() const { return dataType; }
        
        /// set the data type
        void setDataType(const NiftiDataTypeEnum::Enum dt) { dataType = dt; setModified(); }
        
        /// get the encoding
        GiftiEncodingEnum::Enum getEncoding() const { return encoding; }
        
        /// set the encoding
        void setEncoding(const GiftiEncodingEnum::Enum e) { encoding = e; setModified(); }
        
        /// get the data intent
        NiftiIntentEnum::Enum getIntent() const { return intent; }
        
        /// set the data intent
        void setIntent(const NiftiIntentEnum::Enum cat) { intent = cat; setModified(); }
        
        /// valid intent name
        static bool intentNameValid(const AString& intentNameIn);
        
        /// get array subscripting order
        GiftiArrayIndexingOrderEnum::Enum getArraySubscriptingOrder() const { return arraySubscriptingOrder; }
        
        /// set array subscripting order
        void setArraySubscriptingOrder(const GiftiArrayIndexingOrderEnum::Enum aso) { arraySubscriptingOrder = aso; }
        
        /// get pointer for floating point data (valid only if data type is FLOAT)
        float* getDataPointerFloat() { return dataPointerFloat; }
        
        /// get pointer for floating point data (const method) (valid only if data type is FLOAT)
        const float* getDataPointerFloat() const { return dataPointerFloat; }
        
        /// get pointer for integer data (valid only if data type is INT)
        int32_t* getDataPointerInt() { return dataPointerInt; }
        
        /// get pointer for integer data (const method) (valid only if data type is INT)
        const int32_t* getDataPointerInt() const { return dataPointerInt; }
        
        /// get pointer for unsigned byte data (valid only if data type is UBYTE)
        uint8_t* getDataPointerUByte() { return dataPointerUByte; }
        
        /// get pointer for unsigned byte data (const method) (valid only if data type is UBYTE)
        const uint8_t* getDataPointerUByte() const { return dataPointerUByte; }
        
        // set all elements of array to zero
        void zeroize();
        
        // get minimum and maximum values (valid for int data only)
        void getMinMaxValues(int& minValue, int& maxValue) const;
        
        void getMinMaxValuesFloat(float& minValue,
                                  float& maxValue) const;
        
        // remap integer values that are indices to a table
        //void remapIntValues(const std::vector<int32_t>& remappingTable);
        
        void transferLabelIndices(const std::map<int32_t,int32_t>& indexConverter);
        
        // get the data type appropriate for the intent (returns true if valid intent)
        static bool getDataTypeAppropriateForIntent(const NiftiIntentEnum::Enum intentIn,
                                                    NiftiDataTypeEnum::Enum& dataTypeOut);
        
        // get an offset for indices into data (dimensionality of indices must be same as data)
        int64_t getDataOffset(const int32_t indices[]) const;
        
        // get a float value (data type must be float and dimensionality of indices must be same as data)
        float getDataFloat32(const int32_t indices[]) const;
        
        // get a float value pointer (data type must be float and dimensionality of indices must be same as data)
        const float* getDataFloat32Pointer(const int32_t indices[]) const;
        
        // get an int value (data type must be int and dimensionality of indices must be same as data)
        int32_t getDataInt32(const int32_t indices[]) const;
        
        // get an int value pointer(data type must be int and dimensionality of indices must be same as data)
        const int32_t* getDataInt32Pointer(const int32_t indices[]) const;
        
        // get a byte value (data type must be unsigned char and dimensionality of indices must be same as data)
        uint8_t getDataUInt8(const int32_t indices[]) const;
        
        // get a byte value pointer (data type must be unsigned char and dimensionality of indices must be same as data)
        const uint8_t* getDataUInt8Pointer(const int32_t indices[]) const;
        
        // set a float value (data type must be float and dimensionality of indices must be same as data)
        void setDataFloat32(const int32_t indices[], const float dataValue) const;
        
        // set an int value (data type must be int and dimensionality of indices must be same as data)
        void setDataInt32(const int32_t indices[], const int32_t dataValue) const;
        
        // set a byte value (data type must be unsigned char and dimensionality of indices must be same as data)
        void setDataUInt8(const int32_t indices[], const uint8_t dataValue) const;
        
        void setModified();
        
        void clearModified();
        
        bool isModified() const;
        
        virtual AString toString() const;
        
        PaletteColorMapping* getPaletteColorMapping();
        
        const PaletteColorMapping* getPaletteColorMapping() const;
        
        const DescriptiveStatistics* getDescriptiveStatistics() const;
        
        const FastStatistics* getFastStatistics() const;
        
        const Histogram* getHistogram(const int32_t numberOfBuckets) const;
        
        const DescriptiveStatistics* getDescriptiveStatistics(const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues) const;
        
        const Histogram* getHistogram(const int32_t numberOfBuckets,
                                      const float mostPositiveValueInclusive,
                                        const float leastPositiveValueInclusive,
                                        const float leastNegativeValueInclusive,
                                        const float mostNegativeValueInclusive,
                                        const bool includeZeroValues) const;
        
    protected:
        
        //validate the array
        void validateArrayAfterReading();
        
        // allocate data for this column
        virtual void allocateData();
        
        // the copy helper (used by copy constructor and assignment operator)
        void copyHelperGiftiDataArray(const GiftiDataArray& nda);
        
        // update the data pointers
        void updateDataPointers();
        
        // byte swap the data (data read is different endian than this system)
        void byteSwapData(const GiftiEndianEnum::Enum newEndian);
        
        /// convert array indexing order of data
        void convertArrayIndexingOrder();
        
        /// the data
        std::vector<uint8_t> data;
        
        /// size of one data type element
        uint32_t dataTypeSize;
        
        /// pointer for floating point data
        float* dataPointerFloat;
        
        /// pointer for integer data
        int32_t* dataPointerInt;
        
        /// pointer for unsigned byte data
        uint8_t* dataPointerUByte;
        
        /// the matrix (typically only used by coordinates)
        std::vector<Matrix4x4> matrices;
        
        /// the metadata
        GiftiMetaData metaData;
        
        /// the metadata not written to file (mainly for file specific data array meta data)
        GiftiMetaData nonWrittenMetaData;
        
        /// dimensions of the data
        std::vector<int64_t> dimensions;
        
        /// data type
        NiftiDataTypeEnum::Enum dataType;
        
        /// encoding of data
        GiftiEncodingEnum::Enum encoding;
        
        // endian of data
        GiftiEndianEnum::Enum endian;
        
        /// intent name
        NiftiIntentEnum::Enum intent;
        
        /// array subscripting order
        GiftiArrayIndexingOrderEnum::Enum arraySubscriptingOrder;
        
        /// external file name
        AString externalFileName;
        
        /// external file offset
        int64_t externalFileOffset;
        
        /// the palette color mapping
        mutable PaletteColorMapping* paletteColorMapping;
        
        /// minimum float value
        mutable float minValueFloat;
        
        /// maximum float value
        mutable float maxValueFloat;
        
        /// min/max float values valid (child class must set this false when an array value is changed)
        mutable bool minMaxFloatValuesValid;
        
        /// minimum int value
        mutable int32_t minValueInt;
        
        /// maximum int value
        mutable int32_t maxValueInt;
        
        /// min/max int values valid (child class must set this false when an array value is changed)
        mutable bool minMaxIntValuesValid;
        
        mutable float negMaxPct;
        mutable float negMinPct;
        mutable float posMinPct;
        mutable float posMaxPct;
        mutable float negMaxPctValue;
        mutable float negMinPctValue;
        mutable float posMinPctValue;
        mutable float posMaxPctValue;      
        
        /// min/max percentage values valid
        mutable bool minMaxPercentageValuesValid;
        
        /// statistics about data (DO NOT COPY)
        mutable DescriptiveStatistics* descriptiveStatistics;
        
        mutable CaretPointer<FastStatistics> m_fastStatistics;
        
        mutable CaretPointer<Histogram> m_histogram;
        mutable int32_t m_histogramNumberOfBuckets = 100;
        
        mutable CaretPointer<Histogram> m_histogramLimitedValues;
        mutable int32_t m_histogramLimitedValuesNumberOfBuckets = 100;
        mutable float m_histogramLimitedValuesMostPositiveValueInclusive;
        mutable float m_histogramLimitedValuesLeastPositiveValueInclusive;
        mutable float m_histogramLimitedValuesLeastNegativeValueInclusive;
        mutable float m_histogramLimitedValuesMostNegativeValueInclusive;
        mutable bool m_histogramLimitedValuesIncludeZeroValues;
        
        /// statistics about data (DO NOT COPY)
        mutable DescriptiveStatistics* descriptiveStatisticsLimitedValues;
        
        
        bool modifiedFlag; // DO NOT COPY
        // ***** BE SURE TO UPDATE copyHelper() if elements are added ******
        
        /// allow NodeDataFile access to protected elements
        friend class GiftiFile;
    };
    
} // namespace

#endif // __GIFTI_DATA_ARRAY_H__
