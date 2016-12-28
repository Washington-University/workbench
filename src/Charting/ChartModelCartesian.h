#ifndef __CHART_MODEL_CARTESIAN_H__
#define __CHART_MODEL_CARTESIAN_H__

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

#include "ChartAxisUnitsEnum.h"
#include "ChartModel.h"


namespace caret {

    class ChartDataCartesian;
    
    class ChartModelCartesian : public ChartModel {
        
    public:
        ChartModelCartesian(const ChartVersionOneDataTypeEnum::Enum chartDataType,
                                               const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                               const ChartAxisUnitsEnum::Enum dataAxisUnitsY);
        
        virtual ~ChartModelCartesian();
        
        ChartModelCartesian(const ChartModelCartesian& obj);

        ChartModelCartesian& operator=(const ChartModelCartesian& obj);
        
        void getBounds(float& boundsMinX,
                       float& boundsMaxX,
                       float& boundsMinY,
                       float& boundsMaxY) const;
        
        virtual bool isAverageChartDisplaySupported() const;
        
        virtual const ChartData* getAverageChartDataForDisplay(const int32_t tabIndex) const;
        
        float getLineWidth() const;
        
        void setLineWidth(const float lineWidth);
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
    private:
        void copyHelperChartModelCartesian(const ChartModelCartesian& obj);

//        void adjustAxisDefaultRange(float& minValue,
//                                    float& maxValue);
        
        SceneClassAssistant* m_sceneAssistant;
        
        mutable ChartDataCartesian* m_averageChartData;
        
        float m_lineWidth;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_MODEL_CARTESIAN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_MODEL_CARTESIAN_DECLARE__

} // namespace
#endif  //__CHART_MODEL_CARTESIAN_H__
