#ifndef __OXFORD_SPARSE_THREE_FILE_H__
#define __OXFORD_SPARSE_THREE_FILE_H__

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

#include <cmath>
#include <cstdio>
#include <vector>
#include "stdint.h"
#include "AString.h"
#include "CaretAssert.h"
#include "DataFileException.h"

namespace caret {
    
    struct Fibers
    {
        uint32_t count;
        float fiberFractions[3];
        float distance;
        void zero();
    };
    
    class OxfordSparseThreeFile
    {
        static Fibers decodeFibers(const uint64_t& coded);//takes a uint because right shift on signed is implementation dependent
        static int myclamp(const int& input);
        static int64_t encodeFibers(const Fibers& orig);
        FILE* m_valueFile;//we read the other two into memory, so we only need one handle
        int64_t m_dims[2];
        std::vector<uint64_t> m_indexArray, m_scratchRow;
        OxfordSparseThreeFile();
        OxfordSparseThreeFile(const OxfordSparseThreeFile& rhs);
    public:
        OxfordSparseThreeFile(const AString& dimFileName, const AString& indexFileName, const AString& valueFileName);
        void getRow(int64_t* rowOut, int64_t index);
        void getFibersRow(Fibers* rowOut, int64_t index);
        ~OxfordSparseThreeFile();
    };
    
}

#endif //__OXFORD_SPARSE_THREE_FILE_H__
