#ifndef __CARET_SPARSE_TRAJECTORY_FILE_H__
#define __CARET_SPARSE_TRAJECTORY_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
/*LICENSE_END*/

#include <cstdio>
#include <vector>
#include "stdint.h"
#include "AString.h"
#include "DataFile.h"
#include "CiftiXML.h"

namespace caret {
    
    struct FiberFractions
    {
        uint32_t totalCount;
        std::vector<float> fiberFractions;
        float distance;
        void zero();
    };
    
    class CaretSparseFile : public DataFile
    {
        static void decodeFibers(const uint64_t& coded, FiberFractions& decoded);//takes a uint because right shift on signed is implementation dependent
        FILE* m_file;
        int64_t m_dims[2], m_valuesOffset;
        std::vector<uint64_t> m_indexArray, m_scratchRow;
        std::vector<int64_t> m_scratchArray;
        CaretSparseFile(const CaretSparseFile& rhs);
        CiftiXML m_xml;
    public:
        const int64_t* getDimensions() { return m_dims; }

        CaretSparseFile();
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString&) throw (DataFileException) { throw DataFileException("writeFile not implemented for CaretSparseFile"); }
        
        CaretSparseFile(const AString& fileName);
        
        ///get a reference to the XML data
        const CiftiXML& getCiftiXML() const { return m_xml; }
        
        void getRow(int64_t* rowOut, const int64_t& index);
        
        void getFibersRow(FiberFractions* rowOut, const int64_t& index);
        
        ~CaretSparseFile();
    };
    
    class CaretSparseFileWriter
    {
        static void encodeFibers(const FiberFractions& orig, uint64_t& coded);
        static uint32_t myclamp(const int& x);
        FILE* m_file;
        int64_t m_dims[2], m_valuesOffset, m_nextRowIndex;
        bool m_finished;
        std::vector<uint64_t> m_lengthArray, m_scratchRow;
        std::vector<int64_t> m_scratchArray;
        CaretSparseFileWriter(const CaretSparseFileWriter& rhs);
        CiftiXML m_xml;
    public:
        CaretSparseFileWriter(const AString& fileName, const int64_t dimensions[2], const CiftiXML& xml);
        
        ~CaretSparseFileWriter();
        
        ///you must write the rows in order, though you can skip empty rows
        void writeRow(const int64_t* row, const int64_t& index);
        
        ///you must write the rows in order, though you can skip empty rows
        void writeFibersRow(const FiberFractions* row, const int64_t& index);
        
        ///call this if no rows remain to be written
        void finish();
    };
    
}

#endif //__CARET_SPARSE_TRAJECTORY_FILE_H__