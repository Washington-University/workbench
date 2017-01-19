#ifndef __CHARTABLE_MATRIX_SERIES_INTERFACE_H__
#define __CHARTABLE_MATRIX_SERIES_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "ChartableMatrixInterface.h"
#include "MapYokingGroupEnum.h"


namespace caret {

    class ChartableMatrixSeriesInterface : public ChartableMatrixInterface {
        
    public:
        ChartableMatrixSeriesInterface() { }
        
        virtual ~ChartableMatrixSeriesInterface() { }
        
        virtual MapYokingGroupEnum::Enum getMatrixRowColumnMapYokingGroup(const int32_t tabIndex) const = 0;
        
        virtual void setMatrixRowColumnMapYokingGroup(const int32_t tabIndex,
                                    const MapYokingGroupEnum::Enum yokingType) = 0;
        
        virtual int32_t getSelectedMapIndex(const int32_t tabIndex) const = 0;
        
        virtual void setSelectedMapIndex(const int32_t tabIndex,
                                         const int32_t mapIndex) = 0;
        
        // ADD_NEW_METHODS_HERE

    private:
        ChartableMatrixSeriesInterface(const ChartableMatrixSeriesInterface&);

        ChartableMatrixSeriesInterface& operator=(const ChartableMatrixSeriesInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_MATRIX_SERIES_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_MATRIX_SERIES_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_MATRIX_SERIES_INTERFACE_H__
