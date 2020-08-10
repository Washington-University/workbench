#ifndef __CHART_TWO_OVERLAY_SET_H__
#define __CHART_TWO_OVERLAY_SET_H__

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
#include <memory>

#include "BrainConstants.h"
#include "CaretObject.h"
#include "ChartAxisLocationEnum.h"
#include "ChartTwoAxisOrientationTypeEnum.h"
#include "ChartTwoDataTypeEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationPercentSizeText;
    class CaretMappableDataFile;
    class ChartTwoCartesianOrientedAxes;
    class ChartTwoCartesianAxis;
    class ChartTwoOverlay;
    class ChartTwoTitle;
    class PlainTextStringBuilder;
    class SceneClassAssistant;

    class ChartTwoOverlaySet : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        ChartTwoOverlaySet(const ChartTwoDataTypeEnum::Enum chartDataType,
                        const AString& name,
                        const int32_t tabIndex);
        
        virtual ~ChartTwoOverlaySet();
        
        void copyOverlaySet(const ChartTwoOverlaySet* overlaySet);
        
        void copyCartesianAxes(const ChartTwoOverlaySet* overlaySet);
        
        ChartTwoDataTypeEnum::Enum getChartTwoDataType() const;

        ChartTwoOverlay* getPrimaryOverlay();
        
        const ChartTwoOverlay* getPrimaryOverlay() const;
        
        ChartTwoOverlay* getOverlay(const int32_t overlayNumber);
        
        const ChartTwoOverlay* getOverlay(const int32_t overlayNumber) const;
        
        std::weak_ptr<ChartTwoOverlay> getOverlayWeakPointer(const int32_t overlayNumber);
        
        std::vector<ChartTwoOverlay*> getDisplayedOverlays() const;
        
        std::vector<ChartTwoOverlay*> getEnabledOverlays() const;
        
        void addDisplayedOverlay();
        
        int32_t getNumberOfDisplayedOverlays() const;
        
        ChartTwoOverlay* getDisplayedOverlayContainingDataFile(const CaretMappableDataFile* mapFile);
        
        void getDisplayedChartAxes(std::vector<ChartTwoCartesianAxis*>& axesOut) const;
        
        ChartTwoCartesianOrientedAxes* getHorizontalAxes();
        
        const ChartTwoCartesianOrientedAxes* getHorizontalAxes() const;
        
        ChartTwoCartesianOrientedAxes* getVerticalAxes();
        
        const ChartTwoCartesianOrientedAxes* getVerticalAxes() const;
        
        void applyMouseTranslation(const int32_t viewport[4],
                                   const float mouseDX,
                                   const float mouseDY);
        
        void applyMouseScaling(const int32_t viewport[4],
                               const float mouseDY);
        
        AString getAxisLabel(const ChartTwoCartesianAxis* axis) const;
        
        void setAxisLabel(const ChartTwoCartesianAxis* axis,
                          const AString& label);
        
        bool isAxesSupportedByChartDataType() const;
        
        bool getDataRangeForAxis(const ChartAxisLocationEnum::Enum chartAxisLocation,
                                 float& minimumValueOut,
                                 float& maximumValueOut) const;
        
        bool getDataRangeForAxisOrientation(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientationType,
                                            float& minimumValueOut,
                                            float& maximumValueOut) const;

        ChartTwoTitle* getChartTitle();
        
        const ChartTwoTitle* getChartTitle() const;
        
        void insertOverlayAbove(const int32_t overlayIndex);
        
        void insertOverlayBelow(const int32_t overlayIndex);
        
        void removeDisplayedOverlay(const int32_t overlayIndex);
        
        void moveDisplayedOverlayUp(const int32_t overlayIndex);
        
        void moveDisplayedOverlayDown(const int32_t overlayIndex);
        
        void initializeOverlays();
        
        void resetOverlayYokingToOff();

        float getAxisLineThickness() const;
        
        void setAxisLineThickness(const float axisLineThickness);
        
        void incrementOverlayActiveLineChartPoint(const int32_t incrementValue);
        
        void selectOverlayActiveLineChart(ChartTwoOverlay* chartTwoOverlay,
                                          const int32_t lineSegmentPointIndex);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        virtual void receiveEvent(Event* event);

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
        ChartTwoOverlaySet(const ChartTwoOverlaySet&);

        ChartTwoOverlaySet& operator=(const ChartTwoOverlaySet&);
        
        void firstOverlaySelectionChanged();
        
        void assignUnusedColor(ChartTwoOverlay* chartOverlay);

        void updateRangeScaleFromVersionOneScene(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientationType,
                                                 ChartTwoCartesianAxis* leftOrBottomAxis,
                                                 ChartTwoCartesianAxis* rightOrTopAxis);
        
        SceneClassAssistant* m_sceneAssistant;

        std::vector<std::shared_ptr<ChartTwoOverlay>> m_overlays;
        
        const ChartTwoDataTypeEnum::Enum m_chartDataType;
        
        std::unique_ptr<ChartTwoCartesianOrientedAxes> m_horizontalAxes;
        
        std::unique_ptr<ChartTwoCartesianOrientedAxes> m_verticalAxes;
        
        const AString m_name;
        
        const int32_t m_tabIndex;
        
        std::unique_ptr<ChartTwoTitle> m_title;
        
        int32_t m_numberOfDisplayedOverlays;
        
        bool m_inFirstOverlayChangedMethodFlag = false;
        
        /** Thickness of box around chart and tick marks on axes*/
        float m_axisLineThickness = 0.5;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class ChartTwoOverlay;

    };
    
#ifdef __CHART_TWO_OVERLAY_SET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_OVERLAY_SET_DECLARE__

} // namespace
#endif  //__CHART_TWO_OVERLAY_SET_H__
