#ifndef __CARET_SPARSE_FILE_H__
#define __CARET_SPARSE_FILE_H__

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

#include <limits>
#include <vector>
#include <stdint.h>

#include "AString.h"
#include "ByteSwapping.h"
#include "CaretAssert.h"
#include "CaretBinaryFile.h"
#include "CiftiXML.h"
#include "DataFile.h"
#include "DataFileException.h"

namespace caret {
    
    class FileInformation;
    
    struct FiberFractions
    {
        FiberFractions() { clear(); }
        
        explicit FiberFractions(const int& rhs) { CaretAssert(rhs == 0); clear(); (void)rhs; } //hack: support conversion from zero to make getRow() (non-sparse) work out
        bool operator!=(const uint& rhs) const { CaretAssert(rhs == 0); return (totalCount != rhs); } //ditto for writing from non-sparse array
        
        uint32_t totalCount;  // total number of streamline that go through the voxel
        std::vector<float> fiberFractions;  // fraction of totalCount for each fiber  (3 orientation for now, could be zero)
        float distance;   // average distance from seed across all streamlines
        void clear();
    };
    
    class CaretSparseFile /* : public DataFile */
    {
    public:
        enum ValueType
        {
            Fibers = 1,
            Float32 = 2,
            Int32 = 3,
            Float64 = 4,
            Int64 = 5,
        };
#pragma pack(push)
#pragma pack(1)
        struct HeaderV2
        {//public just so writer can use it (could do friend instead...)
            int16_t minorVersion;
            int16_t valueType;
            int64_t dims[2]; //consider 3D+ support?
            int8_t longIndex; //0 or 1 - only affects row encoding, not dims or column offsets (only affects dims[0])
            
            HeaderV2(): dims{0, 0} { minorVersion = -1; valueType = Fibers; longIndex = 1; } //don't leave uninitialized
            void read(CaretBinaryFile& file);
            void write(CaretBinaryFile& file) const;
            int indexSize() { return (longIndex > 0) ? sizeof(int64_t) : sizeof(int32_t); }
            int valueSize()
            {
                switch (valueType)
                {
                    case Float32:
                    case Int32:
                        return sizeof(int32_t);
                    case Fibers:
                    case Float64:
                    case Int64:
                        return sizeof(int64_t);
                    default:
                        CaretAssert(0);
                        throw DataFileException("unknown value type in CaretSparseFile");
                }
            }
        };
#pragma pack(pop)
    private:
        void decodeFibers(const uint64_t& coded, FiberFractions& decoded); //takes a uint because right shift on signed is implementation dependent
        void readFileV1(FileInformation& fileInfo);
        void readFileV2(FileInformation& fileInfo);
        CaretBinaryFile m_file;
        HeaderV2 m_header;
        int64_t m_valuesOffset;
        std::vector<int64_t> m_indexArray, m_scratchIndices;
        //std::vector<uint64_t> m_scratchSparseFibersRow, m_scratchFullFibersRow;
        std::vector<char> m_scratchByteArray;
        CaretSparseFile(const CaretSparseFile& rhs);
        CiftiXML m_xml;
        //bool m_longIndex;
        //ValueType m_valueType;
    public:
        const int64_t* getDimensions() { return m_header.dims; }
        
        CaretSparseFile() { m_valuesOffset = -1; };
        
        virtual void readFile(const AString& filename);
        
        //virtual void writeFile(const AString&) { throw DataFileException("writeFile not implemented for CaretSparseFile"); }
        
        CaretSparseFile(const AString& fileName);
        
        ///get a reference to the XML data
        const CiftiXML& getCiftiXML() const { return m_xml; }
        
        //void getRow(const int64_t& index, int64_t* rowOut);
        
        template <typename V>
        void getRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<V>& valuesOut)
        {
            CaretAssert(index >= 0 && index < m_header.dims[1]);
            if (index < 0 || index >= m_header.dims[1]) throw DataFileException("invalid row index requested in wbsparse");
            int64_t start = m_indexArray[index], end = m_indexArray[index + 1];
            int64_t entriesToRead = (end - start);
            const int indexSize = m_header.indexSize();
            const int entrySize = indexSize + m_header.valueSize();
            int64_t bytesToRead = entriesToRead * entrySize;
            m_scratchByteArray.resize(bytesToRead);
            m_file.seek(m_valuesOffset + start * entrySize);
            m_file.read(m_scratchByteArray.data(), bytesToRead);
            indicesOut.resize(entriesToRead);
            valuesOut.resize(entriesToRead);
            int64_t lastIndex = -1;
            for (int64_t i = 0; i < entriesToRead; ++i)
            {
                indicesOut[i] = convertIndexRead(m_scratchByteArray.data() + i * entrySize);
                valuesOut[i] = convertValueRead<V>(m_scratchByteArray.data() + i * entrySize + indexSize);
                if (indicesOut[i] <= lastIndex || indicesOut[i] >= m_header.dims[0]) throw DataFileException("impossible index value found in file " + m_file.getFilename());
                lastIndex = indicesOut[i];
            }
        }
        
        template <typename V>
        void getRow(const int64_t& index, V* valuesOut)
        {
            std::vector<V> sparseValues; //no good way to keep this allocated between calls...static and omp critical would work, but...
            getRowSparse(index, m_scratchIndices, sparseValues);
            int64_t nextindex = 0;
            for (int64_t indexindex = 0; indexindex < (int64_t)m_scratchIndices.size(); ++indexindex)
            {
                for (; nextindex < m_scratchIndices[indexindex]; ++nextindex)
                {
                    valuesOut[nextindex] = V(0);
                }
                valuesOut[nextindex] = sparseValues[indexindex];
                ++nextindex;
            }
            for (; nextindex < m_header.dims[0]; ++nextindex)
            {
                valuesOut[nextindex] = V(0);
            }
        }

        void getFibersRow(const int64_t& index, FiberFractions* rowOut) { getRow(index, rowOut); } //forward old functions to templated version
        
        void getFibersRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<FiberFractions>& valuesOut) { getRowSparse(index, indicesOut, valuesOut); }
        
        virtual ~CaretSparseFile();
    private:
        int64_t convertIndexRead(char* buffer)
        {
            if (m_header.longIndex > 0)
            {
                int64_t ret = *(int64_t*)buffer;
                if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(ret);
                return ret;
            } else {
                uint32_t ret = *(uint32_t*)buffer;
                if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(ret);
                return ret;
            }
        }
        template <typename V>
        V convertValueRead(char* buffer)
        {
            if (std::numeric_limits<V>::is_integer)//do round to nearest from float to integer
            {
                switch (m_header.valueType)
                {
                    case Float32:
                        return clamp<V, float>(std::floor(convertValueReadRaw<float>(buffer) + 0.5f));
                    case Float64:
                        return clamp<V, double>(std::floor(convertValueReadRaw<double>(buffer) + 0.5));
                    case Int32:
                        return clamp<V, int32_t>(convertValueReadRaw<int32_t>(buffer));
                    case Fibers:
                        CaretAssert(0); //error in debugger if we interpret encoded fibers as a number
                        //fallthrough
                    case Int64:
                        return clamp<V, int64_t>(convertValueReadRaw<int64_t>(buffer));
                    default:
                        CaretAssert(0);
                        throw DataFileException("unknown value type in CaretSparseFile");
                }
            } else {
                switch (m_header.valueType)
                {
                    case Float32:
                        return (V)convertValueReadRaw<float>(buffer);
                    case Float64:
                        return (V)convertValueReadRaw<double>(buffer);
                    case Int32:
                        return (V)convertValueReadRaw<int32_t>(buffer);
                    case Fibers:
                        CaretAssert(0);
                        //fallthrough
                    case Int64:
                        return (V)convertValueReadRaw<int64_t>(buffer);
                    default:
                        CaretAssert(0);
                        throw DataFileException("unknown value type in CaretSparseFile");
                }
            }
        }
        template <typename R>
        R convertValueReadRaw(char* buffer)
        {// consider putting details like this in Common, maybe the templated niftiIO conversions, too...maybe even CaretBinaryFile.h?
            R ret = *(R*)buffer;
            if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(ret);
            return ret;
        }
    public:
        template<typename TO, typename FROM>
        static TO clamp(const FROM& in)
        {
            typedef std::numeric_limits<TO> mylimits;
            if (mylimits::has_infinity && std::isinf(in)) return (TO)in;//in case we use this on float types at some point
            if (mylimits::max() < in) return mylimits::max();
            if (mylimits::lowest() > in) return mylimits::lowest();
            return (TO)in;
        }
    };
    
    //specialize this for fiber fractions, and the rest works generically
    template <>
    inline FiberFractions CaretSparseFile::convertValueRead(char* buffer)
    {
        CaretAssert(m_header.valueType == Fibers);
        if (m_header.valueType != Fibers) throw DataFileException("attempted to convert wbsparse with value type " + AString::number(m_header.valueType) + " to fiber fractions");
        FiberFractions ret;
        decodeFibers(convertValueReadRaw<uint64_t>(buffer), ret);
        return ret;
    }
    
    //REMOVEME: the old implementation as a specialization
    //template<>
    //void CaretSparseFile::getRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<int64_t>& valuesOut);

    class CaretSparseFileWriter
    {
        static void encodeFibers(const FiberFractions& orig, uint64_t& coded);
        static uint32_t myclamp(const int& x);
        CaretBinaryFile m_file;
        CaretSparseFile::HeaderV2 m_header;
        int64_t m_valuesOffset, m_nextRowIndex;//, m_dims[2];
        bool m_finished;
        std::vector<uint64_t> m_lengthArray;//, m_scratchRow;
        //std::vector<int64_t> m_scratchArray, m_scratchSparseRow;
        std::vector<char> m_scratchBytes;
        CaretSparseFileWriter(const CaretSparseFileWriter& rhs);
        CiftiXML m_xml;
    public:
        CaretSparseFileWriter(const AString& fileName, const CiftiXML& xml, const CaretSparseFile::ValueType writingType = CaretSparseFile::Fibers, const int forceVersion = -1);
        
        ~CaretSparseFileWriter();
        
        ///you must write the rows in order, though you can skip empty rows
        template <typename V>
        void writeRow(const int64_t& index, const V* row)
        {//use the sparse function for simplicity
            std::vector<int64_t> indices;
            std::vector<V> values;
            for (int64_t i = 0; i < m_header.dims[0]; ++i)
            {
                if (row[i] != 0)
                {
                    indices.push_back(i);
                    values.push_back(row[i]);
                }
            }
            writeRowSparse(index, indices, values);
        }

        ///you must write the rows in order, though you can skip empty rows
        template <typename V>
        void writeRowSparse(const int64_t& index, const std::vector<int64_t>& indices, const std::vector<V>& values)
        {
            CaretAssert(index < m_header.dims[1]);
            CaretAssert(index >= m_nextRowIndex);
            CaretAssert(indices.size() == values.size());
            CaretAssert(!m_finished);
            if (m_finished) throw DataFileException("row writing attempted on already-finished wbsparse file");
            if (index < m_nextRowIndex) throw DataFileException("row writing attempted out of order on wbsparse file");
            while (m_nextRowIndex < index)
            {
                m_lengthArray[m_nextRowIndex] = 0;
                ++m_nextRowIndex;
            }
            int64_t numNonZero = int64_t(indices.size()); //assume no zeros
            m_lengthArray[index] = numNonZero;
            m_nextRowIndex = index + 1;
            const int indexSize = m_header.indexSize();
            const int entrySize = indexSize + m_header.valueSize();
            m_scratchBytes.resize(numNonZero * entrySize);
            for (int64_t i = 0; i < int64_t(indices.size()); ++i)
            {
                CaretAssert(indices[i] >= 0 && indices[i] < m_header.dims[0]);
                if (m_header.longIndex > 0) //stick this in a convertIndexWrite function?
                {
                    int64_t& temp = *(int64_t*)&m_scratchBytes[i * entrySize];
                    temp = indices[i];
                    if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
                } else {
                    uint32_t& temp = *(uint32_t*)&m_scratchBytes[i * entrySize];
                    temp = indices[i];
                    if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
                }
                convertValueWrite(values[i], m_scratchBytes.data() + i * entrySize + indexSize);//so that it can specialize for FiberFraction type instead of this whole function
            }
            m_file.write(m_scratchBytes.data(), m_scratchBytes.size());
        }
        /*
        ///you must write the rows in order, though you can skip empty rows
        void writeRow(const int64_t& index, const int64_t* row);
        
        ///you must write the rows in order, though you can skip empty rows
        void writeRowSparse(const int64_t& index, const std::vector<int64_t>& indices, const std::vector<int64_t>& values);
        //*/
        ///you must write the rows in order, though you can skip empty rows
        void writeFibersRow(const int64_t& index, const FiberFractions* row) { writeRow(index, row); }
        
        ///you must write the rows in order, though you can skip empty rows
        void writeFibersRowSparse(const int64_t& index, const std::vector<int64_t>& indices, const std::vector<FiberFractions>& values) { writeRowSparse(index, indices, values); }
        
        ///call this if no rows remain to be written
        void finish();
    private:
        template<typename V>
        void convertValueWrite(const V& value, char* buffer)
        {
            switch(m_header.valueType)
            {
                case CaretSparseFile::Float32:
                {
                    float& temp = *(float*)buffer;
                    temp = float(value);
                    if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
                    break;
                }
                case CaretSparseFile::Float64:
                {
                    double& temp = *(double*)buffer;
                    temp = double(value);
                    if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
                    break;
                }
                case CaretSparseFile::Int32:
                {
                    int32_t& temp = *(int32_t*)buffer;
                    temp = CaretSparseFile::clamp<int32_t, V>(value);
                    if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
                    break;
                }
                case CaretSparseFile::Fibers:
                    CaretAssert(0); //error in debugger if we interpret encoded fibers as a number
                    //fallthrough
                case CaretSparseFile::Int64:
                {
                    int64_t& temp = *(int64_t*)buffer;
                    temp = CaretSparseFile::clamp<int64_t, V>(value);
                    if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
                    break;
                }
                default:
                    CaretAssert(0);
                    throw DataFileException("unknown value type in CaretSparseFileWriter");
            }
        }
    };
    
    template<>
    inline void CaretSparseFileWriter::convertValueWrite(const FiberFractions& value, char* buffer)
    {
        CaretAssert(m_header.valueType == CaretSparseFile::Fibers);
        if (m_header.valueType != CaretSparseFile::Fibers) throw DataFileException("attempted to write fiber fractions to wbsparse with type " + AString::number(m_header.valueType));
        uint64_t& temp = *(uint64_t*)buffer;
        encodeFibers(value, temp);
        if (ByteSwapping::isSystemBigEndian()) ByteSwapping::swap(temp);
    }
}

#endif //__CARET_SPARSE_FILE_H__
