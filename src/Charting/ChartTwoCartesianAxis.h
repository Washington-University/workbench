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
#include "ChartTwoAxisScaleRangeModeEnum.h"
#include "ChartTwoNumericSubdivisionsModeEnum.h"
#include "CaretUnitsTypeEnum.h"
#include "NumericFormatModeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class ChartTwoOverlaySetInterface;
    class SceneClassAssistant;

    class ChartTwoCartesianAxis : public CaretObject, public SceneableInterface {
        
    public:
        ChartTwoCartesianAxis(const ChartTwoOverlaySetInterface* parentChartOverlaySetInterface,
                              const ChartAxisLocationEnum::Enum axisLocation);
        
        virtual ~ChartTwoCartesianAxis();
        
        ChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj);

        ChartTwoCartesianAxis& operator=(const ChartTwoCartesianAxis& obj);
        
        void reset();
        
        ChartAxisLocationEnum::Enum getAxisLocation() const;
        
        bool isDisplayedByUser() const;
        
        void setDisplayedByUser(const bool displayed);
        
        void getDataRange(float& rangeMinimumOut,
                          float& rangeMaximumOut) const;
        
        float getSceneUserScaleMinimumValue() const;
        
        float getSceneUserScaleMaximumValue() const;
        
        int32_t getUserDigitsRightOfDecimal() const;
        
        void setUserDigitsRightOfDecimal(const int32_t digitsRightOfDecimal);
        
        ChartTwoAxisScaleRangeModeEnum::Enum getSceneScaleRangeMode() const;
        
        CaretUnitsTypeEnum::Enum getUnits() const;
        
        void setUnits(CaretUnitsTypeEnum::Enum units);
        
        NumericFormatModeEnum::Enum getUserNumericFormat() const;
        
        void setUserNumericFormat(const NumericFormatModeEnum::Enum numericFormat);
        
        ChartTwoNumericSubdivisionsModeEnum::Enum getNumericSubdivsionsMode() const;
        
        void setNumericSubdivsionsMode(const ChartTwoNumericSubdivisionsModeEnum::Enum numericSubdivsionsMode);
        
        int32_t getUserNumberOfSubdivisions() const;
        
        void setUserNumberOfSubdivisions(const int32_t numberOfSubdivisions);
        
        bool isShowTickmarks() const;

        void setShowTickmarks(const bool showTickmarks);
        
        bool isShowLabel() const;
        
        void setShowLabel(const bool showLabel);
        
        float getLabelTextSize() const;
        
        void setLabelTextSize(const float labelTextSize);
        
        float getNumericsTextSize() const;
        
        void setNumericsTextSize(const float numericsTextSize);
        
        bool isNumericsTextDisplayed() const;
        
        void setNumericsTextDisplayed(const bool numericsTextDisplayed);
        
        bool isNumericsTextRotated() const;
        
        void setNumericsTextRotated(const bool numericsTextRotated);
        
        float getPaddingSize() const;
        
        void setPaddingSize(const float paddingSize);
        
        int32_t getLabelOverlayIndex(const int32_t maximumNumberOfOverlays) const;
        
        void setLabelOverlayIndex(const int32_t labelOverlayIndex);
        
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

        const ChartTwoOverlaySetInterface* m_parentChartOverlaySetInterface;
        
        const ChartAxisLocationEnum::Enum m_axisLocation;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        mutable float m_userScaleMinimumValue = -100.0f;
        
        mutable float m_userScaleMaximumValue = 100.0f;
        
        float m_axisLabelsStepValue = 1.0f;
        
        int32_t m_userDigitsRightOfDecimal = 1;
        
        mutable int32_t m_titleOverlayIndex = 0;
        
        ChartTwoAxisScaleRangeModeEnum::Enum m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::AUTO;
        
        CaretUnitsTypeEnum::Enum m_units = CaretUnitsTypeEnum::NONE;
        
        NumericFormatModeEnum::Enum m_userNumericFormat = NumericFormatModeEnum::AUTO;
        
        /** numeric subdivisions mode*/
        ChartTwoNumericSubdivisionsModeEnum::Enum m_numericSubdivsionsMode = ChartTwoNumericSubdivisionsModeEnum::AUTO;

        int32_t m_userNumberOfSubdivisions = 2;
        
        /** size of label text*/
        float m_labelTextSize = 2.5f;
        
        /** size of numerics text*/
        float m_numericsTextSize = 2.5f;
        
        /** display numeric text in scale*/
        bool m_numericsTextDisplayed = true;
        
        /** rotate numeric text*/
        bool m_numericsTextRotated = false;

        /** size of padding*/
        float m_paddingSize = 0.0f;
        
        bool m_showTickmarks = true;
        
        /** show axis label*/
        bool m_showLabel = true;
        
        /** user display the axis*/
        bool m_displayedByUser = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_CARTESIAN_AXIS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_CARTESIAN_AXIS_DECLARE__

} // namespace
#endif  //__CHART_TWO_CARTESIAN_AXIS_H__
