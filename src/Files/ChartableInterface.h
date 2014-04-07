#ifndef __CHARTABLE_INTERFACE_H__
#define __CHARTABLE_INTERFACE_H__

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

#include "ChartDataTypeEnum.h"

namespace caret {

    class CaretMappableDataFile;
    
    /**
     * \class caret::ChartableInterface
     * \brief Interface for files that are able to produce charts.
     * \ingroup Files
     */
    class ChartableInterface {
        
    public:
//        ChartableInterface() { }
//        
//        virtual ~ChartableInterface() { }
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual CaretMappableDataFile* getCaretMappableDataFile();
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual const CaretMappableDataFile* getCaretMappableDataFile() const;
        
        /**
         * @return Is charting enabled for this file in the given tab?
         */
        virtual bool isChartingEnabled(const int32_t tabIndex) const = 0;
        
        /**
         * @return Return true if the file's current state supports
         * charting data, else false.  Typically a brainordinate file
         * is chartable if it contains more than one map.
         */
        virtual bool isChartingSupported() const = 0;
        
        /**
         * Set charting enabled for this file in the given tab
         *
         * @param enabled
         *    New status for charting enabled.
         */
        virtual void setChartingEnabled(const int32_t tabIndex,
                                        const bool enabled) = 0;

        /**
         * Get chart data types supported by the file.
         *
         * @param chartDataTypesOut
         *    Chart types supported by this file.
         */
        virtual void getSupportedChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const = 0;
        
        bool isChartDataTypeSupported(const ChartDataTypeEnum::Enum chartDataType) const;
        
    private:
//        ChartableInterface(const ChartableInterface&);
//
//        ChartableInterface& operator=(const ChartableInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_INTERFACE_H__
