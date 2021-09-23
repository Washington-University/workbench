#ifndef __CHART_TWO_OVERLAY_H__
#define __CHART_TWO_OVERLAY_H__

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

#include <array>
#include <limits>
#include <memory>
#include <set>

#include "CardinalDirectionEnum.h"
#include "CaretColor.h"
#include "CaretObject.h"
#include "ChartAxisLocationEnum.h"
#include "ChartTwoCompoundDataType.h"
#include "ChartTwoMatrixTriangularViewingModeEnum.h"
#include "ChartTwoOverlayActiveModeEnum.h"
#include "EventListenerInterface.h"
#include "GraphicsLineMeanDeviationSettings.h"
#include "MapYokingGroupEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationColorBar;
    class BoundingBox;
    class CaretMappableDataFile;
    class ChartTwoDataCartesian;
    class ChartTwoOverlaySet;
    class Matrix4x4;
    class PlainTextStringBuilder;
    class SceneClassAssistant;

    class ChartTwoOverlay : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        enum class SelectedIndexType {
            INVALID,
            MAP,
            ROW,
            COLUMN
        };
        
        ChartTwoOverlay(ChartTwoOverlaySet* parentChartTwoOverlaySet,
                        const ChartTwoDataTypeEnum::Enum chartDataType,
                        const int32_t tabIndex,
                        const int32_t overlayIndex);
        
        virtual ~ChartTwoOverlay();
        
        std::weak_ptr<ChartTwoOverlay> getWeakPointerToSelf();
        
        ChartTwoDataTypeEnum::Enum getChartTwoDataType() const;
        
        const ChartTwoCompoundDataType* getChartTwoCompoundDataType() const;
        
        void setChartTwoCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType);
        
        int32_t getTabIndex() const;
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        bool isLineSeriesLoadingEnabled() const;
        
        void setLineSeriesLoadingEnabled(const bool lineSeriesLoadingEnabled);
        
        void copyData(const ChartTwoOverlay* overlay);
        
        void swapData(ChartTwoOverlay* overlay);
        
        bool isHistorySupported() const;
        
        bool isMapYokingSupported() const;
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        AnnotationColorBar* getColorBar();
        
        const AnnotationColorBar* getColorBar() const;
        
        ChartTwoMatrixTriangularViewingModeEnum::Enum getMatrixTriangularViewingMode() const;
        
        void setMatrixTriangularViewingMode(const ChartTwoMatrixTriangularViewingModeEnum::Enum mode);
        
        bool isMatrixTriangularViewingModeSupported() const;
        
        float getMatrixOpacity() const;
        
        void setMatrixOpacity(const float opacity);
        
        ChartAxisLocationEnum::Enum getSceneCartesianVerticalAxisLocation() const;
        
        bool isCartesianVerticalAxisLocationSupported() const;
        
        ChartAxisLocationEnum::Enum getCartesianHorizontalAxisLocation() const;
        
        void setCartesianHorizontalAxisLocation(const ChartAxisLocationEnum::Enum cartesianHorizontalAxisLocation);
        
        bool isCartesianHorizontalAxisLocationSupported() const;
        
        CaretMappableDataFile* getSelectedMapFile() const;
        
        void getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                              CaretMappableDataFile* &selectedMapFileOut,
                              std::vector<AString>& selectedFileMapNamesOut,
                              SelectedIndexType& selectedIndexTypeOut,
                              int32_t& selectedIndexOut) const;
        
        void getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                              CaretMappableDataFile* &selectedMapFileOut,
                              SelectedIndexType& selectedIndexTypeOut,
                              int32_t& selectedIndexOut) const;
        
        void getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                              SelectedIndexType& selectedIndexTypeOut,
                              int32_t& selectedIndexOut) const;
        
        void setSelectionData(CaretMappableDataFile* selectedMapFile,
                              const int32_t selectedMapIndex);
        
        bool isAllMapsSupported() const;
        
        bool isAllMapsSelected() const;
        
        void setAllMapsSelected(const bool status);
        
        bool getBounds(BoundingBox& boundingBoxOut) const;
        
        ChartTwoDataCartesian* getLineLayerChartDisplayedCartesianData();
        
        const ChartTwoDataCartesian* getLineLayerChartDisplayedCartesianData() const;

        ChartTwoDataCartesian* getLineLayerChartMapFileCartesianData();
        
        const ChartTwoDataCartesian* getLineLayerChartMapFileCartesianData() const;
        
        CaretColor getLineLayerColor() const;
        
        void setLineLayerColor(const CaretColor& color);
        
        float getLineLayerLineWidth() const;
        
        void setLineLayerLineWidth(const float lineWidth);
        
        int32_t getSelectedLineChartNumberOfPoints() const;
        
        int32_t getSelectedLineChartPointIndex() const;
        
        void setSelectedLineChartPointIndex(const int32_t pointIndex);
        
        void incrementSelectedLineChartPointIndex(const int32_t incrementValue);

        ChartTwoOverlayActiveModeEnum::Enum getLineChartActiveMode() const;
        
        void setLineChartActiveMode(const ChartTwoOverlayActiveModeEnum::Enum lineChartActiveMode);
        
        bool getSelectedLineChartPointXYZ(std::array<float, 3>& xyzOut) const;
        
        CardinalDirectionEnum::Enum getSelectedLineChartTextOffset() const;
        
        void setSelectedLineChartTextOffset(const CardinalDirectionEnum::Enum offset);
        
        bool isLineChartNewMeanEnabled() const;
        
        void setLineChartNewMeanEnabled(const bool enabled);
        
        float getLineChartNewMeanValue() const;
        
        void setLineChartNewMeanValue(const float value);
        
        bool isLineChartNewDeviationEnabled() const;
        
        void setLineChartNewDeviationEnabled(const bool enabled);
        
        bool isLineChartNormalizationAbsoluteValueEnabled() const;
        
        void setLineChartNormalizationAbsoluteValueEnabled(const bool enabled);
        
        float getLineChartNewDeviationValue() const;
        
        void setLineChartNewDeviationValue(const float value);
        
        bool isLineChartAddToMeanEnabled();
        
        void setLineChartAddToMeanEnabled(const bool enabled);
        
        float getLineChartAddToMeanValue() const;
        
        void setLineChartAddToMeanValue(const float value);
        
        bool isLineChartMultiplyDeviationEnabled() const;
        
        void setLineChartMultiplyDeviationEnabled(const bool enabled);
        
        float getLineChartMultiplyDeviationValue() const;
        
        void setLineChartMultiplyDeviationValue(const float value);
        
        virtual void receiveEvent(Event* event);

        // ADD_NEW_METHODS_HERE
        
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
        ChartTwoOverlay(const ChartTwoOverlay&);

        ChartTwoOverlay& operator=(const ChartTwoOverlay&);
        
        void setWeakPointerToSelf(std::weak_ptr<ChartTwoOverlay> weakPointerToSelf);
        
        void getSelectionDataPrivate(std::vector<CaretMappableDataFile*>& mapFilesOut,
                                     CaretMappableDataFile* &selectedMapFileOut,
                                     std::vector<AString>* selectedFileMapNamesOut,
                                     SelectedIndexType& selectedIndexTypeOut,
                                     int32_t& selectedIndexOut) const;
        
        bool isMapYokingSupportedPrivate(const CaretMappableDataFile* mapFile) const;
        
        void validateCartesianVerticalAxisLocation() const;
        
        void validateCartesianHorizontalAxisLocation() const;
        
        void validateSelectedLineChartPointIndex() const;
        
        static CaretColorEnum::Enum generateDefaultColor();
        
        /** Parent chart overlay set (only used by first overlay in the set */
        ChartTwoOverlaySet* m_parentChartTwoOverlaySet;
        
        /** Enumerated Type of charts allowed in this overlay */
        const ChartTwoDataTypeEnum::Enum m_chartDataType;
        
        const int32_t m_tabIndex;
        
        /** Index of this overlay (DO NOT COPY)*/
        const int32_t m_overlayIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        /** Current 'compound chart type' of charts allowed in this overlay DO NOT COPY */
        mutable ChartTwoCompoundDataType m_chartCompoundDataType;
        
        /** Name of overlay (DO NOT COPY)*/
        AString m_name;
        
        /** enabled status */
        mutable bool m_enabled = true;
        
        /** map yoking group */
        mutable MapYokingGroupEnum::Enum m_mapYokingGroup;

        /** The color bar displayed in the graphics window */
        std::unique_ptr<AnnotationColorBar> m_colorBar;
        
        /** selected mappable file */
        mutable CaretMappableDataFile* m_selectedMapFile = NULL;
        
        /** histogram selected map index */
        mutable int32_t m_selectedHistogramMapIndex = -1;
        
        mutable int32_t m_selectedLineLayerMapIndex = -1;
        
        bool m_allHistogramMapsSelectedFlag = false;
        
        mutable ChartTwoMatrixTriangularViewingModeEnum::Enum m_matrixTriangularViewingMode;
        
        float m_matrixOpacity = 1.0;
        
        /** Location of vertical cartesian axis*/
        mutable ChartAxisLocationEnum::Enum m_cartesianVerticalAxisLocation;
        
        /** Location of horizontal cartesian axis*/
        mutable ChartAxisLocationEnum::Enum m_cartesianHorizontalAxisLocation;
        
        CaretColor m_lineLayerColor;
        
        float m_lineLayerLineWidth;
        
        mutable int32_t m_selectedLineChartPointIndex = 0;
        
        ChartTwoOverlayActiveModeEnum::Enum m_lineChartActiveMode = ChartTwoOverlayActiveModeEnum::OFF;
        
        CardinalDirectionEnum::Enum m_selectedLineChartTextOffset = CardinalDirectionEnum::AUTO;
        
        mutable std::unique_ptr<ChartTwoDataCartesian> m_lineChartNormalizedCartesianData;
        
        GraphicsLineMeanDeviationSettings m_lineChartSettings;
        
        GraphicsLineMeanDeviationSettings m_previousLineChartSettings;

        /** A weak pointer to 'self' so that can be stored to safely test instance is valid and can be accessed */
        std::weak_ptr<ChartTwoOverlay> m_weakPointerToSelf;
        
        /** prevents excessing warnings when NaNs and/or Inf found in normalization */
        std::set<std::pair<CaretMappableDataFile*, int32_t>> m_normalizedMapFilesWithNanInf;
        
        static int32_t s_defaultColorIndexGenerator;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ChartTwoOverlaySet;
    };
    
#ifdef __CHART_TWO_OVERLAY_DECLARE__
    int32_t ChartTwoOverlay::s_defaultColorIndexGenerator = 0;
#endif // __CHART_TWO_OVERLAY_DECLARE__

} // namespace
#endif  //__CHART_TWO_OVERLAY_H__
