#ifndef __CHART_TWO_CARTESIAN_ORIENTATED_AXES_H__
#define __CHART_TWO_CARTESIAN_ORIENTATED_AXES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include "ChartTwoAxisOrientationTypeEnum.h"
#include "ChartTwoAxisScaleRangeModeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class ChartTwoCartesianAxis;
    class ChartTwoOverlaySetInterface;
    class SceneClassAssistant;

    class ChartTwoCartesianOrientedAxes : public CaretObject, public SceneableInterface {
        
    public:
        ChartTwoCartesianOrientedAxes(const ChartTwoOverlaySetInterface* parentChartOverlaySetInterface,
                                const ChartTwoAxisOrientationTypeEnum::Enum orientationType);
        
        virtual ~ChartTwoCartesianOrientedAxes();
        
        ChartTwoCartesianOrientedAxes(const ChartTwoCartesianOrientedAxes& obj);

        ChartTwoCartesianOrientedAxes& operator=(const ChartTwoCartesianOrientedAxes& obj);

        ChartTwoAxisOrientationTypeEnum::Enum getOrientationType();

        ChartTwoAxisScaleRangeModeEnum::Enum getScaleRangeMode() const;
        
        void initializeScaleRangeMode(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode);
        
        void setScaleRangeModeFromGUI(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode);
        
        void getDataRange(float& minimumValueOut,
                          float& maximumValueOut) const;
        
        void getUserScaleMinimumMaximumValues(float& minimumOut,
                                              float& maximumOut) const;
        
        float getUserScaleMinimumValue() const;
        
        void setUserScaleMinimumValueFromGUI(const float value);
        
        float getUserScaleMaximumValue() const;
        
        void setUserScaleMaximumValueFromGUI(const float value);
        
        void resetUserScaleRange();
        
        void setRangeModeAndUserScaleFromVersionOneScene(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode,
                                                         const float userScaleMinimumValue,
                                                         const float userScaleMaximumValue);
        
        void copyAxes(const ChartTwoCartesianOrientedAxes* axes);
        
        ChartTwoCartesianAxis* getLeftOrBottomAxis();
        
        const ChartTwoCartesianAxis* getLeftOrBottomAxis() const;
        
        ChartTwoCartesianAxis* getRightOrTopAxis();

        const ChartTwoCartesianAxis* getRightOrTopAxis() const;
        
        bool getScaleValuesAndOffsets(const ChartTwoCartesianAxis* cartesianAxis,
                                      const float minimumDataValue,
                                      const float maximumDataValue,
                                      const float axisLength,
                                      float& minimumOut,
                                      float& maximumOut,
                                      std::vector<float>& scaleValuesOffsetInPixelsOut,
                                      std::vector<AString>& scaleValuesOut) const;
        
        bool isTransformationEnabled() const;
        
        void setTransformationEnabled(const bool enabled);
        
        void applyMouseTranslation(const int32_t viewport[4],
                                   const float mouseDX,
                                   const float mouseDY);
        
        void applyMouseScaling(const int32_t viewport[4],
                               const float mouseXY,
                               const float mouseDY);
        
        float getPercentageOfDataRange(const float percentage) const;
        
        void reset();
        
        float getAxesCoordinateFromViewportCoordinate(const int32_t viewportWidth,
                                                      const int32_t viewportHeight,
                                                      const float viewportValue) const;
        
        float getDataPercentageFromPercentageOfViewport(const int32_t viewportWidth,
                                                        const int32_t viewportHeight,
                                                        const float viewportValue) const;
        
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
        void copyHelperChartTwoCartesianOrientedAxes(const ChartTwoCartesianOrientedAxes& obj);

        bool getAutoRangeMinimumAndMaximum(const float minimumValue,
                                           const float maximumValue,
                                           float& minimumOut,
                                           float& maximumOut,
                                           float& stepValueOut,
                                           int32_t& digitsRightOfDecimalOut) const;
        
        void updateRangeModeAfterMinimumOrMaximumChanged();
        
        void updateMinMaxValuesForYoking() const;
        
        const ChartTwoOverlaySetInterface* m_parentChartOverlaySetInterface;
        
        const ChartTwoAxisOrientationTypeEnum::Enum m_orientationType;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        ChartTwoAxisScaleRangeModeEnum::Enum m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::AUTO;
        
        mutable float m_userScaleMinimumValue = -100.0f;
        
        mutable float m_userScaleMaximumValue = 100.0f;
        
        std::unique_ptr<ChartTwoCartesianAxis> m_leftOrBottomAxis;
        
        std::unique_ptr<ChartTwoCartesianAxis> m_rightOrTopAxis;
        
        bool m_transformationEnabled = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_CARTESIAN_ORIENTATED_AXES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_CARTESIAN_ORIENTATED_AXES_DECLARE__

} // namespace
#endif  //__CHART_TWO_CARTESIAN_ORIENTATED_AXES_H__
