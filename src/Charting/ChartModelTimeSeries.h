#ifndef __CHART_MODEL_TIME_SERIES_H__
#define __CHART_MODEL_TIME_SERIES_H__

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


#include "ChartModelCartesian.h"



namespace caret {

    class ChartModelTimeSeries : public ChartModelCartesian {
        
    public:
        ChartModelTimeSeries();
        
        virtual ~ChartModelTimeSeries();
        
        ChartModelTimeSeries(const ChartModelTimeSeries& obj);

        ChartModelTimeSeries& operator=(const ChartModelTimeSeries& obj);
        

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperChartModelTimeSeries(const ChartModelTimeSeries& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_MODEL_TIME_SERIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_MODEL_TIME_SERIES_DECLARE__

} // namespace
#endif  //__CHART_MODEL_TIME_SERIES_H__
