#ifndef __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_H__
#define __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include <map>

#include "ChartableTwoFileBaseChart.h"
#include "ChartTwoHistogramContentTypeEnum.h"


namespace caret {

    class ChartTwoDataHistogram;
    
    class ChartableTwoFileHistogramChart : public ChartableTwoFileBaseChart {
        
    public:
        ChartableTwoFileHistogramChart(const ChartTwoHistogramContentTypeEnum::Enum histogramContentType,
                                               CaretMappableDataFile* parentCaretMappableDataFile);
        
        virtual ~ChartableTwoFileHistogramChart();
        
        ChartTwoHistogramContentTypeEnum::Enum getHistogramContentType() const;
        
        virtual bool isValid() const override;
        
        virtual bool isEmpty() const override;
        
        const ChartTwoDataHistogram* getMapHistogramChart(const int32_t mapIndex) const;
        
        // ADD_NEW_METHODS_HERE
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) override;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) override;

    private:
        ChartableTwoFileHistogramChart(const ChartableTwoFileHistogramChart&);

        ChartableTwoFileHistogramChart& operator=(const ChartableTwoFileHistogramChart&);
        
        SceneClassAssistant* m_sceneAssistant;

        const ChartTwoHistogramContentTypeEnum::Enum m_histogramContentType;
        
        mutable std::map<int32_t, ChartTwoDataHistogram*> m_indexHistogramsMap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_HISTOGRAM_CHART_H__
