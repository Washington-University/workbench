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

#include <memory>

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

        int32_t getUserDigitsRightOfDecimal() const;
        
        void setUserDigitsRightOfDecimal(const int32_t digitsRightOfDecimal);
        
        ChartTwoAxisScaleRangeModeEnum::Enum getScaleRangeMode() const;
        
        void setScaleRangeMode(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode);
        
        ChartAxisUnitsEnum::Enum getUnits() const;
        
        void setUnits(ChartAxisUnitsEnum::Enum units);
        
        NumericFormatModeEnum::Enum getUserNumericFormat() const;
        
        void setUserNumericFormat(const NumericFormatModeEnum::Enum numericFormat);
        
        bool isAutoSubdivisionsEnabled() const;
        
        void setAutoSubdivisionsEnabled(const bool autoSubdivisionsEnabled);
        
        int32_t getUserNumberOfSubdivisions() const;
        
        void setUserNumberOfSubdivisions(const int32_t numberOfSubdivisions);
        
        bool isVisible() const;
        
        void setVisible(const bool visible);

        bool isShowTickmarks() const;

        void setShowTickmarks(const bool showTickmarks);
        
        bool getScaleValuesAndOffsets(const float dataBoundsIn[4],
                                   const float axisLengthInPixels,
                                   float& minimumOut,
                                   float& maximumOut,
                                   std::vector<float>& scaleValuesOffsetInPixelsOut,
                                   std::vector<AString>& scaleValuesOut) const;
        
        AString getAxisTitle() const;
        
        void setAxisTitle(const AString& axisTitle);
        
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

        bool getAutoRangeMinimumAndMaximum(const float dataBounds[4],
                                           float& minimumOut,
                                           float& maximumOut,
                                           float& stepValueOut,
                                           int32_t& digitsRightOfDecimalOut) const;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        const ChartAxisLocationEnum::Enum m_axisLocation;
        
        mutable float m_userScaleMinimumValue = -100.0;
        
        mutable float m_userScaleMaximumValue = 100.0;
        
        float m_axisLabelsStepValue = 1.0;
        
        int32_t m_userDigitsRightOfDecimal = 1;
        
        ChartTwoAxisScaleRangeModeEnum::Enum m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_AUTO;
        
        ChartAxisUnitsEnum::Enum m_units = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
        
        NumericFormatModeEnum::Enum m_userNumericFormat = NumericFormatModeEnum::AUTO;
        
        bool m_autoSubdivisionsEnabled = true;

        int32_t m_userNumberOfSubdivisions = 2;
        
        AString m_axisTitle;

        bool m_visible = false;

        bool m_showTickmarks = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_CARTESIAN_AXIS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_CARTESIAN_AXIS_DECLARE__

} // namespace
#endif  //__CHART_TWO_CARTESIAN_AXIS_H__
