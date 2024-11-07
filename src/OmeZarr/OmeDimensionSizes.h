#ifndef __OME_DIMENSION_SIZES_H__
#define __OME_DIMENSION_SIZES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



namespace caret {

    class OmeDimensionSizes : public CaretObject {
        
    public:
        OmeDimensionSizes();
        
        OmeDimensionSizes(const int64_t x,
                          const int64_t y,
                          const int64_t z,
                          const int64_t time,
                          const int64_t channels);

        virtual ~OmeDimensionSizes();
        
        OmeDimensionSizes(const OmeDimensionSizes& obj);

        OmeDimensionSizes& operator=(const OmeDimensionSizes& obj);
        
        /** @return Number for elements in X-dimension; negative if invalid */
        inline int64_t getSizeX() const { return m_x; }

        /** @return Number for elements in Y-dimension; negative if invalid */
        inline int64_t getSizeY() const { return m_y; }
        
        /** @return Number for elements in Z-dimension; negative if invalid */
        inline int64_t getSizeZ() const { return m_z; }
        
        /** @return Number for elements in TIme-dimension; negative if invalid */
        inline int64_t getSizeTime() const { return m_time; }
        
        /** @return Number for elements in Channels-dimension; negative if invalid */
        inline int64_t getSizeChannels() const { return m_channels; }
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperOmeDimensionSizes(const OmeDimensionSizes& obj);

        int64_t m_x = -1;
        
        int64_t m_y = -1;
        
        int64_t m_z = -1;
        
        int64_t m_time = -1;
        
        int64_t m_channels = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_DIMENSION_SIZES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_DIMENSION_SIZES_DECLARE__

} // namespace
#endif  //__OME_DIMENSION_SIZES_H__
