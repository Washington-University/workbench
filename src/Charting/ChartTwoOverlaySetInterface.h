#ifndef __CHART_TWO_OVERLAY_SET_INTERFACE_H__
#define __CHART_TWO_OVERLAY_SET_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include "ChartAxisLocationEnum.h"
#include "ChartTwoAxisOrientationTypeEnum.h"

namespace caret {

    class ChartTwoOverlaySetInterface {
        
    public:
        ChartTwoOverlaySetInterface();
        
        virtual ~ChartTwoOverlaySetInterface();
        
        virtual bool getDataRangeForAxis(const ChartAxisLocationEnum::Enum axisLocation,
                                         float& dataMinimum,
                                         float& dataMaximum) const = 0;

        virtual bool getDataRangeForAxisOrientation(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                    float& dataMinimum,
                                                    float& dataMaximum) const = 0;

        // ADD_NEW_METHODS_HERE

    private:
        ChartTwoOverlaySetInterface(const ChartTwoOverlaySetInterface& obj);
        
        ChartTwoOverlaySetInterface& operator=(const ChartTwoOverlaySetInterface& obj);
        
        void copyHelperChartTwoOverlaySetInterface(const ChartTwoOverlaySetInterface& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_OVERLAY_SET_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_OVERLAY_SET_INTERFACE_DECLARE__

} // namespace
#endif  //__CHART_TWO_OVERLAY_SET_INTERFACE_H__
