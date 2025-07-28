#ifndef __DATA_SLICE_H__
#define __DATA_SLICE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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



#include <cstdint>

#include "CaretObject.h"



namespace caret {

    class DataSlice : public CaretObject {
        
    public:
        DataSlice(const float* dataPointer,
                  const int64_t dataLength,
                  const int64_t dataStride = 1);
        
        virtual ~DataSlice();
        
        DataSlice(const DataSlice& obj);

        DataSlice& operator=(const DataSlice& obj);
        
        /** @return Pointer to first data element */
        float*  getDataPointer() const { return m_dataPointer; }
        
        /** @return Length of data (number of elements) */
        int64_t getDataLength() const { return m_dataLength; }
        
        /** @return Offset to next element (1 is contiguous data) */
        int64_t getDataStride() const { return m_dataStride; }
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperDataSlice(const DataSlice& obj);

        float*  m_dataPointer;

        int64_t m_dataLength;

        int64_t m_dataStride;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_SLICE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_SLICE_DECLARE__

} // namespace
#endif  //__DATA_SLICE_H__
