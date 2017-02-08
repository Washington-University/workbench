#ifndef __CHART_TWO_DATA_HISTOGRAM_H__
#define __CHART_TWO_DATA_HISTOGRAM_H__

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

#include "CaretColorEnum.h"
#include "ChartAxisUnitsEnum.h"
#include "ChartTwoData.h"

namespace caret {

    class ChartPoint;
    class Histogram;
    
    class ChartTwoDataHistogram : public ChartTwoData {
        
    public:
        ChartTwoDataHistogram();
        
        virtual ~ChartTwoDataHistogram();
        
        virtual ChartTwoData* clone() const;
        
        const std::vector<float>& getHistogramValues() const;

        const Histogram* getHistogram() const;
        
        void setHistogram(Histogram* histogram);
        
        bool getBounds(float boundsOut[4]) const;
        
        CaretColorEnum::Enum getColor() const;
        
        void setColor(const CaretColorEnum::Enum color);
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
        
    private:
        ChartTwoDataHistogram(const ChartTwoDataHistogram& obj);
        
        ChartTwoDataHistogram& operator=(const ChartTwoDataHistogram& obj);
        
        void copyHelperChartTwoDataHistogram(const ChartTwoDataHistogram& obj);

        void initializeMembersChartTwoDataHistogram();
        
        Histogram* m_histogram;
        
        mutable float m_bounds[4];
        
        mutable bool m_boundsValid;
        
        CaretColorEnum::Enum m_color;
        
        static int32_t caretColorIndex;
        
        std::vector<float> m_emptyData;
        
        SceneClassAssistant* m_sceneAssistant;
        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_DATA_HISTOGRAM_DECLARE__
    int32_t ChartTwoDataHistogram::caretColorIndex = 0;
#endif // __CHART_TWO_DATA_HISTOGRAM_DECLARE__

} // namespace
#endif  //__CHART_TWO_DATA_HISTOGRAM_H__
