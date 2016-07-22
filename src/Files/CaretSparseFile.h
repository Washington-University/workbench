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

#include <vector>
#include "stdint.h"

#include "AString.h"
#include "CaretBinaryFile.h"
#include "CiftiXML.h"
#include "DataFile.h"
#include "DataFileException.h"

namespace caret {
    
    struct FiberFractions
    {
        uint32_t totalCount;  // total number of streamline that go through the voxel
        std::vector<float> fiberFractions;  // fraction of totalCount for each fiber  (3 orientation for now, could be zero)
        float distance;   // average distance from seed across all streamlines
        void zero();
    };
    
    class CaretSparseFile /* : public DataFile */
    {
        static void decodeFibers(const uint64_t& coded, FiberFractions& decoded);//takes a uint because right shift on signed is implementation dependent
        CaretBinaryFile m_file;
        int64_t m_dims[2], m_valuesOffset;
        std::vector<uint64_t> m_indexArray, m_scratchRow;
        std::vector<int64_t> m_scratchArray, m_scratchSparseRow;
        CaretSparseFile(const CaretSparseFile& rhs);
        CiftiXML m_xml;
    public:
        const int64_t* getDimensions() { return m_dims; }

        CaretSparseFile() {};
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString&) { throw DataFileException("writeFile not implemented for CaretSparseFile"); }
        
        CaretSparseFile(const AString& fileName);
        
        ///get a reference to the XML data
        const CiftiXML& getCiftiXML() const { return m_xml; }
        
        void getRow(const int64_t& index, int64_t* rowOut);
        
        void getRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<int64_t>& valuesOut);

        void getFibersRow(const int64_t& index, FiberFractions* rowOut);
        
        void getFibersRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<FiberFractions>& valuesOut);

        virtual ~CaretSparseFile();
    };
    
    class CaretSparseFileWriter
    {
        static void encodeFibers(const FiberFractions& orig, uint64_t& coded);
        static uint32_t myclamp(const int& x);
        CaretBinaryFile m_file;
        int64_t m_dims[2], m_valuesOffset, m_nextRowIndex;
        bool m_finished;
        std::vector<uint64_t> m_lengthArray, m_scratchRow;
        std::vector<int64_t> m_scratchArray, m_scratchSparseRow;
        CaretSparseFileWriter(const CaretSparseFileWriter& rhs);
        CiftiXML m_xml;
    public:
        CaretSparseFileWriter(const AString& fileName, const CiftiXML& xml);
        
        ~CaretSparseFileWriter();
        
        ///you must write the rows in order, though you can skip empty rows
        void writeRow(const int64_t& index, const int64_t* row);
        
        ///you must write the rows in order, though you can skip empty rows
        void writeRowSparse(const int64_t& index, const std::vector<int64_t>& indices, const std::vector<int64_t>& values);
        
        ///you must write the rows in order, though you can skip empty rows
        void writeFibersRow(const int64_t& index, const FiberFractions* row);
        
        ///you must write the rows in order, though you can skip empty rows
        void writeFibersRowSparse(const int64_t& index, const std::vector<int64_t>& indices, const std::vector<FiberFractions>& values);
        
        ///call this if no rows remain to be written
        void finish();
    };
    
}

#endif //__CARET_SPARSE_FILE_H__
