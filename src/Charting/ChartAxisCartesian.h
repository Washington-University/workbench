#ifndef __CHART_AXIS_CARTESIAN_H__
#define __CHART_AXIS_CARTESIAN_H__

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


#include "ChartAxis.h"



namespace caret {

    class ChartModelCartesian;
    
    class ChartAxisCartesian : public ChartAxis {
        
    public:
        virtual ~ChartAxisCartesian();
        
        ChartAxisCartesian(const ChartAxisCartesian& obj);

        ChartAxisCartesian& operator=(const ChartAxisCartesian& obj);
        
        virtual ChartAxis* clone() const;
        
        float getMinimumValue() const;
        
        void setMinimumValue(const float minimumValue);
        
        float getMaximumValue() const;
        
        void setMaximumValue(const float maximumValue);
        
        void getLabelsAndPositions(const float axisLengthInPixels,
                                   const float fontSizeInPixels,
                                   std::vector<float>& labelOffsetInPixelsOut,
                                   std::vector<AString>& labelTextOut);
        // ADD_NEW_METHODS_HERE
          
    protected:
        ChartAxisCartesian(const ChartAxisLocationEnum::Enum axisLocation);
        
        virtual void updateForAutoRangeScale();
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperChartAxisCartesian(const ChartAxisCartesian& obj);
        
        void initializeMembersChartAxisCartesian();

        SceneClassAssistant* m_sceneAssistant;

        mutable float m_maximumValue;
        
        mutable float m_minimumValue;
        
        mutable int32_t m_digitsRightOfDecimal;
        
        mutable float m_axisLabelsMinimumValue;
        
        mutable float m_axisLabelsMaximumValue;
        
        mutable float m_axisLabelsStepValue;
        
        // ADD_NEW_MEMBERS_HERE
        
    friend class ChartAxis;

    };
    
#ifdef __CHART_AXIS_CARTESIAN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_AXIS_CARTESIAN_DECLARE__

} // namespace
#endif  //__CHART_AXIS_CARTESIAN_H__
