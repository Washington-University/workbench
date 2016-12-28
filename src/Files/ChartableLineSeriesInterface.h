#ifndef __CHARTABLE_LINE_SERIES_INTERFACE_H__
#define __CHARTABLE_LINE_SERIES_INTERFACE_H__

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

#include "ChartVersionOneDataTypeEnum.h"
#include "StructureEnum.h"

namespace caret {

    //class CaretMappableDataFile;
    class ChartDataCartesian;
    class CaretMappableDataFile;
    
    class ChartableLineSeriesInterface {
        
    protected:
        ChartableLineSeriesInterface() { }
        
        virtual ~ChartableLineSeriesInterface() { }
   
    public:

        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual CaretMappableDataFile* getLineSeriesChartCaretMappableDataFile();
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual const CaretMappableDataFile* getLineSeriesChartCaretMappableDataFile() const;
        
        /**
         * @return Is charting enabled for this file in the given tab?
         */
        virtual bool isLineSeriesChartingEnabled(const int32_t tabIndex) const = 0;
        
        /**
         * @return Return true if the file's current state supports
         * charting data, else false.  Typically a brainordinate file
         * is chartable if it contains more than one map.
         */
        virtual bool isLineSeriesChartingSupported() const = 0;
        
        /**
         * Set charting enabled for this file in the given tab
         *
         * @param enabled
         *    New status for charting enabled.
         */
        virtual void setLineSeriesChartingEnabled(const int32_t tabIndex,
                                        const bool enabled) = 0;
        
        /**
         * Get chart data types supported by the file.
         *
         * @param chartDataTypesOut
         *    Chart types supported by this file.
         */
        virtual void getSupportedLineSeriesChartDataTypes(std::vector<ChartVersionOneDataTypeEnum::Enum>& chartDataTypesOut) const = 0;
        
        bool isLineSeriesChartDataTypeSupported(const ChartVersionOneDataTypeEnum::Enum chartDataType) const;

    private:
//        ChartableLineSeriesInterface(const ChartableLineSeriesInterface&);
//
//        ChartableLineSeriesInterface& operator=(const ChartableLineSeriesInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_LINE_SERIES_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_LINE_SERIES_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_LINE_SERIES_INTERFACE_H__
