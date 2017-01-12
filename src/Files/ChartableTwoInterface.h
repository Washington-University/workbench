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
#include "ChartableTwoFileDelegateHistogramChart.h"
#include "ChartableTwoFileDelegateLineSeriesChart.h"
#include "ChartableTwoFileDelegateMatrixChart.h"
#include "ChartTwoLineSeriesContentTypeEnum.h"
#include "ChartTwoMatrixContentTypeEnum.h"

namespace caret {

    class CaretMappableDataFile;
    class ChartTwoDataCartesianHistory;
    class CiftiMappableDataFile;
    
    class ChartableTwoInterface {
        
    public:
        ChartableTwoInterface();
        
        virtual ~ChartableTwoInterface();
        
        virtual CaretMappableDataFile* getAsCaretMappableDataFile() = 0;
        
        virtual const CaretMappableDataFile* getAsCaretMappableDataFile() const = 0;
        
        virtual ChartableTwoFileDelegateHistogramChart* getHistogramChartDelegate() = 0;
        
        virtual const ChartableTwoFileDelegateHistogramChart* getHistogramChartDelegate() const = 0;
        
        virtual ChartableTwoFileDelegateLineSeriesChart* getLineSeriesChartDelegate() = 0;
        
        virtual const ChartableTwoFileDelegateLineSeriesChart* getLineSeriesChartDelegate() const = 0;
        
        virtual ChartableTwoFileDelegateMatrixChart* getMatrixChartDelegate() = 0;
        
        virtual const ChartableTwoFileDelegateMatrixChart* getMatrixChartDelegate() const = 0;
        
        bool isChartingSupported() const;
        
        bool isChartingSupportedForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const;
        
        bool isChartingSupportedForChartCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const;
        
        void getSupportedChartDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        void getSupportedChartCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const;
        
        bool getChartCompoundDataTypeForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
                                                      ChartTwoCompoundDataType& chartCompoundDataTypeOut) const;
        
        
        /* IMPLEMENT ABOVE NON-VIRTUAL METHODS USING THE VIRTUAL METHODS */
        
        /* METHODS NO LONGER NEEDED ??? */
        
//        CiftiMappableDataFile* getAsCiftiMappableDataFile();
//        
//        const CiftiMappableDataFile* getAsCiftiMappableDataFile() const;
//        
//        
//        ChartTwoHistogramContentTypeEnum::Enum getHistogramContentType() const;
//        
//        ChartTwoLineSeriesContentTypeEnum::Enum getLineSeriesContentType() const;
//        
//        ChartTwoMatrixContentTypeEnum::Enum getMatrixContentType() const;
//        
//        void getTwoMatrixDimensions(int32_t& numberOfRowsOut,
//                                    int32_t& numberOfColumnsOut) const;
//        
//        bool getTwoMatrixDataRGBA(int32_t& numberOfRowsOut,
//                                  int32_t& numberOfColumnsOut,
//                                  std::vector<float>& rgbaOut) const;
        
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
        //virtual ChartTwoDataCartesianHistory* getLineSeriesChartingHistory() = 0;
        
        // ADD_NEW_METHODS_HERE

    private:
        ChartableTwoInterface(const ChartableTwoInterface&);

        ChartableTwoInterface& operator=(const ChartableTwoInterface&);
        
//        const ChartTwoHistogramContentTypeEnum::Enum m_histogramContentType;
//        
//        const ChartTwoLineSeriesContentTypeEnum::Enum m_lineSeriesContentType;
//
//        const ChartTwoMatrixContentTypeEnum::Enum m_matrixContentType;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_INTERFACE_H__
