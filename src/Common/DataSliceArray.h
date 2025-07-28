#ifndef __DATA_SLICE_ARRAY_H__
#define __DATA_SLICE_ARRAY_H__

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



#include <memory>

#include "CaretObject.h"
#include "DataSlice.h"


namespace caret {

    class DataSliceArray : public CaretObject {
        
    public:
        DataSliceArray();
        
        virtual ~DataSliceArray();
        
        DataSliceArray(const DataSliceArray& obj);

        DataSliceArray& operator=(const DataSliceArray& obj);

        void addDataSlice(const float* dataPointer,
                          const int64_t dataLength,
                          const int64_t dataStride = 1);

        void getAllData(std::vector<float>& dataOut) const;
        
        int64_t getLength() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperDataSliceArray(const DataSliceArray& obj);

        std::vector<DataSlice> m_dataSlices;
        
        int64_t m_allDataLength = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_SLICE_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_SLICE_ARRAY_DECLARE__

} // namespace
#endif  //__DATA_SLICE_ARRAY_H__
