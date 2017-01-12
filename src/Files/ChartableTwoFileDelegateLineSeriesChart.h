#ifndef __CHARTABLE_TWO_FILE_DELEGATE_LINE_SERIES_CHART_H__
#define __CHARTABLE_TWO_FILE_DELEGATE_LINE_SERIES_CHART_H__

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


#include "ChartableTwoFileDelegateBaseChart.h"
#include "ChartTwoLineSeriesContentTypeEnum.h"


namespace caret {

    class ChartableTwoFileDelegateLineSeriesChart : public ChartableTwoFileDelegateBaseChart {
        
    public:
        ChartableTwoFileDelegateLineSeriesChart(const ChartTwoLineSeriesContentTypeEnum::Enum lineSeriesContentType,
                                                CaretMappableDataFile* parentCaretMappableDataFile);
        
        virtual ~ChartableTwoFileDelegateLineSeriesChart();
        
        ChartTwoLineSeriesContentTypeEnum::Enum getLineSeriesContentType() const;
        

        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        ChartableTwoFileDelegateLineSeriesChart(const ChartableTwoFileDelegateLineSeriesChart&);

        ChartableTwoFileDelegateLineSeriesChart& operator=(const ChartableTwoFileDelegateLineSeriesChart&);
        
        SceneClassAssistant* m_sceneAssistant;

        const ChartTwoLineSeriesContentTypeEnum::Enum m_lineSeriesContentType;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_FILE_DELEGATE_LINE_SERIES_CHART_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_FILE_DELEGATE_LINE_SERIES_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_DELEGATE_LINE_SERIES_CHART_H__
