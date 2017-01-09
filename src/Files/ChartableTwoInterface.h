#ifndef __CHARTABLE_TWO_INTERFACE_H__
#define __CHARTABLE_TWO_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include "ChartTwoCompoundDataType.h"
#include "ChartTwoDataTypeEnum.h"

namespace caret {

    class CaretMappableDataFile;
    class ChartTwoDataCartesianHistory;
    
    class ChartableTwoInterface {
        
    public:
        ChartableTwoInterface();
        
        virtual ~ChartableTwoInterface();
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual CaretMappableDataFile* getAsCaretMappableDataFile() = 0;
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual const CaretMappableDataFile* getAsCaretMappableDataFile() const = 0;
        
        /**
         * Does this file support any type of charting?
         */
        virtual bool isChartingSupported() const = 0;
        
        /**
         * Test for support of the given chart data type.
         * 
         * @param chartDataType
         *     Type of chart data.
         * @return
         *     True if the chart data type is supported, else false.
         */
        virtual bool isChartingSupportedForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const = 0;
        
        /**
         * Test for support of the given chart compound data type.
         *
         * @param chartCompoundDataType
         *     Type of chart compound data.
         * @return
         *     True if the chart compound data type is supported, else false.
         */
        virtual bool isChartingSupportedForChartCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const = 0;
        
        /**
         * Get chart data types supported by this file.
         *
         * @param chartDataTypesOut
         *     Output containing all chart data types supported by this data file.
         */
        virtual void getSupportedChartDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const = 0;
        
        /**
         * Get chart data types supported by this file.
         *
         * @param chartDataTypesOut
         *     Output containing all chart data types supported by this data file.
         */
        virtual void getSupportedChartCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const = 0;
        
//        /**
//         * @return Is line series charting enabled for this file in the given tab?
//         */
//        virtual bool isLineSeriesChartingEnabled(const int32_t tabIndex) const = 0;
//        
//        /**
//         * Set charting enabled for this file in the given tab
//         *
//         * @param enabled
//         *    New status for charting enabled.
//         */
//        virtual void setLineSeriesChartingEnabled(const int32_t tabIndex,
//                                                  const bool enabled) = 0;
        
        /**
         * @return Charting history if this file charts to lines.
         */
        virtual ChartTwoDataCartesianHistory* getLineSeriesChartingHistory() = 0;
        
        // ADD_NEW_METHODS_HERE

    private:
        ChartableTwoInterface(const ChartableTwoInterface&);

        ChartableTwoInterface& operator=(const ChartableTwoInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_INTERFACE_H__
