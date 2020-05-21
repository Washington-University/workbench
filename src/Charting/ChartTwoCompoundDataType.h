#ifndef __CHART_TWO_COMPOUND_DATA_TYPE_H__
#define __CHART_TWO_COMPOUND_DATA_TYPE_H__

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


#include "CaretObject.h"
#include "CaretUnitsTypeEnum.h"
#include "ChartTwoDataTypeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ChartTwoCompoundDataType : public CaretObject, public SceneableInterface {
        
    public:
        ChartTwoCompoundDataType();
        
        ChartTwoCompoundDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
                                 const int32_t histogramNumberOfBuckets,
                                 const CaretUnitsTypeEnum::Enum lineChartUnitsAxisX,
                                 const int32_t lineChartNumberOfElementsAxisX,
                                 const int32_t matrixNumberOfRows,
                                 const int32_t matrixNumberOfColumns);
        
        static ChartTwoCompoundDataType newInstanceForHistogram(const int32_t histogramNumberOfBuckets);
        
        static ChartTwoCompoundDataType newInstanceForLineLayer(const CaretUnitsTypeEnum::Enum lineChartUnitsAxisX,
                                                                 const int32_t lineChartNumberOfElementsAxisX);
        
        static ChartTwoCompoundDataType newInstanceForLineSeries(const CaretUnitsTypeEnum::Enum lineChartUnitsAxisX,
                                                                 const int32_t lineChartNumberOfElementsAxisX);
        
        static ChartTwoCompoundDataType newInstanceForMatrix(const int32_t matrixNumberOfRows,
                                                             const int32_t matrixNumberOfColumns);
        

        virtual ~ChartTwoCompoundDataType();
        
        ChartTwoCompoundDataType(const ChartTwoCompoundDataType& obj);

        ChartTwoCompoundDataType& operator=(const ChartTwoCompoundDataType& obj);
        
        bool operator==(const ChartTwoCompoundDataType& obj) const;
        
        ChartTwoDataTypeEnum::Enum getChartTwoDataType() const;
        
        int32_t getHistogramNumberOfBuckets() const;
        
        CaretUnitsTypeEnum::Enum getLineChartUnitsAxisX() const;
        
        int32_t getLineChartNumberOfElementsAxisX() const;
        
        int32_t getMatrixNumberOfRows() const;
        
        int32_t getMatrixNumberOfColumns() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperChartTwoCompoundDataType(const ChartTwoCompoundDataType& obj);

        void initializeChartTwoCompoundDataType();
        
        SceneClassAssistant* m_sceneAssistant;

        ChartTwoDataTypeEnum::Enum m_chartDataType;
        
        int32_t m_histogramNumberOfBuckets;
        
        CaretUnitsTypeEnum::Enum m_lineChartUnitsAxisX;
        
        int32_t m_lineChartNumberOfElementsAxisX;
        
        int32_t m_matrixNumberOfRows;
        
        int32_t m_matrixNumberOfColumns;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_COMPOUND_DATA_TYPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_COMPOUND_DATA_TYPE_DECLARE__

} // namespace
#endif  //__CHART_TWO_COMPOUND_DATA_TYPE_H__
