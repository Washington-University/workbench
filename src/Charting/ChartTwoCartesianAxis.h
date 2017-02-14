#ifndef __CHART_TWO_CARTESIAN_AXIS_H__
#define __CHART_TWO_CARTESIAN_AXIS_H__

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


#include "CaretObject.h"
#include "ChartAxisLocationEnum.h"
#include "ChartAxisUnitsEnum.h"
#include "ChartTwoAxisScaleRangeModeEnum.h"
#include "HistogramDrawingInfo.h"
#include "NumericFormatModeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ChartTwoCartesianAxis : public CaretObject, public SceneableInterface {
        
    public:
        ChartTwoCartesianAxis(const ChartAxisLocationEnum::Enum axisLocation);
        
        virtual ~ChartTwoCartesianAxis();
        
        ChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj);

        ChartTwoCartesianAxis& operator=(const ChartTwoCartesianAxis& obj);
        
        ChartAxisLocationEnum::Enum getAxisLocation() const;
        
        float getUserScaleMinimumValue() const;
        
        void setUserScaleMinimumValue(const float value);
        
        float getUserScaleMaximumValue() const;
        
        void setUserScaleMaximumValue(const float value);
        
        float getMinimumValue() const;
        
        float getMaximumValue() const;

        ChartTwoAxisScaleRangeModeEnum::Enum getScaleRangeMode() const;
        
        void setScaleRangeMode(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode);
        
        ChartAxisUnitsEnum::Enum getUnits() const;
        
        void setUnits(ChartAxisUnitsEnum::Enum units);
        
        NumericFormatModeEnum::Enum getNumericFormat() const;
        
        void setNumericFormat(const NumericFormatModeEnum::Enum numericFormat);
        
        int32_t getNumberOfSubdivisions() const;
        
        void setNumberOfSubdivisions(const int32_t numberOfSubdivisions);
        
        bool isVisible() const;
        
        void setVisible(const bool visible);

        bool isShowTickmarks() const;

        void setShowTickmarks(const bool showTickmarks);
        
        void getLabelsAndPositions(const float dataBounds[4],
                                   const float axisLengthInPixels,
                                   const float fontSizeInPixels,
                                   std::vector<float>& labelOffsetInPixelsOut,
                                   std::vector<AString>& labelTextOut);

        AString getLabelText() const;
        
        void setLabelText(const AString& labelText);
        
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
        void copyHelperChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj);

        virtual void updateForAutoRangeScale(const float dataBounds[4]);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        const ChartAxisLocationEnum::Enum m_axisLocation;
        
        float m_userScaleMinimumValue = -1.0;
        
        float m_userScaleMaximumValue = 1.0;
        
        float m_minimumValue = 0.0;
        
        float m_maximumValue = 100.0;
        
        float m_axisLabelsMinimumValue = 0.0;
        
        float m_axisLabelsMaximumValue = 100.0;
        
        float m_axisLabelsStepValue = 1.0;
        
        int32_t m_axisDigitsRightOfDecimal = 1;
        
        ChartTwoAxisScaleRangeModeEnum::Enum m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_AUTO;
        
        ChartAxisUnitsEnum::Enum m_units = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
        
        NumericFormatModeEnum::Enum m_numericFormat = NumericFormatModeEnum::AUTO;
        
        int32_t m_numberOfSubdivisions = 2;
        
        AString m_labelText;

        bool m_visible = false;

        bool m_showTickmarks = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_CARTESIAN_AXIS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_CARTESIAN_AXIS_DECLARE__

} // namespace
#endif  //__CHART_TWO_CARTESIAN_AXIS_H__
