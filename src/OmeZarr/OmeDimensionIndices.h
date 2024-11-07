#ifndef __OME_DIMENSION_INDICES_H__
#define __OME_DIMENSION_INDICES_H__

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

    class OmeDimensionIndices : public CaretObject {
        
    public:
        OmeDimensionIndices();
        
        OmeDimensionIndices(const int64_t xDimensionIndex,
                            const int64_t yDimensionIndex,
                            const int64_t zDimensionIndex,
                            const int64_t timeDimensionIndex,
                            const int64_t channelDimensionIndex);
        
        virtual ~OmeDimensionIndices();
        
        OmeDimensionIndices(const OmeDimensionIndices& obj);

        OmeDimensionIndices& operator=(const OmeDimensionIndices& obj);

        /**
         * @return Index for the X-dimension
         */
        inline int64_t getIndexForX() const
        {
            return m_xDimensionIndex;
        }
        
        /**
         * @return Index for the Y-dimension
         */
        inline int64_t getIndexForY() const
        {
            return m_yDimensionIndex;
        }
        
        /**
         * @return Index for the Z-dimension
         */
        inline int64_t getIndexForZ() const
        {
            return m_zDimensionIndex;
        }
        
        /**
         * @return Index for the time dimension
         */
        inline int64_t getIndexForTime() const
        {
            return m_timeDimensionIndex;
        }
        
        /**
         * @return Index for the channel dimension
         */
        inline int64_t getIndexForChannel() const
        {
            return m_channelDimensionIndex;
        }

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperOmeDimensionIndices(const OmeDimensionIndices& obj);

        int64_t m_xDimensionIndex = -1;
        
        int64_t m_yDimensionIndex = -1;
        
        int64_t m_zDimensionIndex = -1;
        
        int64_t m_timeDimensionIndex = -1;
        
        int64_t m_channelDimensionIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_DIMENSION_INDICES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_DIMENSION_INDICES_DECLARE__

} // namespace
#endif  //__OME_DIMENSION_INDICES_H__
