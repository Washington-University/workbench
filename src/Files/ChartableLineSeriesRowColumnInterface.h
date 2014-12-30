#ifndef __CHARTABLE_LINE_SERIES_ROW_COLUMN_INTERFACE_H__
#define __CHARTABLE_LINE_SERIES_ROW_COLUMN_INTERFACE_H__

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


#include "ChartableLineSeriesInterface.h"



namespace caret {

    class ChartDataCartesian;
    
    /**
     * \class caret::ChartableLineSeriesRowColumnInterface
     * \brief Interface for charts that load a row and/or column of data.
     * \ingroup Files
     */
    class ChartableLineSeriesRowColumnInterface : public ChartableLineSeriesInterface {
        
    public:
        ChartableLineSeriesRowColumnInterface() { }
        
        virtual ~ChartableLineSeriesRowColumnInterface() { };
        
        /**
         * Load charting data for the given column index.
         *
         * @param columnIndex
         *     Index of the column.
         * @return
         *     Pointer to the chart data.  If the data FAILED to load,
         *     the returned pointer will be NULL.  Caller takes ownership
         *     of the pointer and must delete it when no longer needed.
         */
        virtual ChartDataCartesian* loadLineSeriesChartDataForColumn(const int32_t columnIndex) = 0;
        
        /**
         * Load charting data for the given row index.
         *
         * @param rowIndex
         *     Index of the row.
         * @return
         *     Pointer to the chart data.  If the data FAILED to load,
         *     the returned pointer will be NULL.  Caller takes ownership
         *     of the pointer and must delete it when no longer needed.
         */
        virtual ChartDataCartesian* loadLineSeriesChartDataForRow(const int32_t rowIndex) = 0;

        // ADD_NEW_METHODS_HERE

    private:
        ChartableLineSeriesRowColumnInterface(const ChartableLineSeriesRowColumnInterface&);

        ChartableLineSeriesRowColumnInterface& operator=(const ChartableLineSeriesRowColumnInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_LINE_SERIES_ROW_COLUMN_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_LINE_SERIES_ROW_COLUMN_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_LINE_SERIES_ROW_COLUMN_INTERFACE_H__
