#ifndef __OXFORD_SPARSE_THREE_FILE_H__
#define __OXFORD_SPARSE_THREE_FILE_H__

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

#include "CaretSparseFile.h"
//for the Fibers struct

namespace caret {
    
    class OxfordSparseThreeFile
    {
        static void decodeFibers(const uint64_t& coded, FiberFractions& decoded);//takes a uint because right shift on signed is implementation dependent
        FILE* m_valueFile;//we read the other two into memory, so we only need one handle
        int64_t m_dims[2];
        std::vector<uint64_t> m_indexArray, m_scratchRow;
        std::vector<int64_t> m_scratchArray, m_scratchSparseRow;
        OxfordSparseThreeFile();
        OxfordSparseThreeFile(const OxfordSparseThreeFile& rhs);
    public:
        const int64_t* getDimensions() { return m_dims; }
        OxfordSparseThreeFile(const AString& dimFileName, const AString& indexFileName, const AString& valueFileName);
        void getRow(const int64_t& index, int64_t* rowOut);
        void getRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<int64_t>& valuesOut);
        void getFibersRow(const int64_t& index, FiberFractions* rowOut);
        void getFibersRowSparse(const int64_t& index, std::vector<int64_t>& indicesOut, std::vector<FiberFractions>& valuesOut);
        ~OxfordSparseThreeFile();
    };
    
}

#endif //__OXFORD_SPARSE_THREE_FILE_H__
