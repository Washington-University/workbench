#ifndef __IDENTIFICATION_FORMATTED_TEXT_GENERATOR__H_
#define __IDENTIFICATION_FORMATTED_TEXT_GENERATOR__H_

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

#include <memory>
#include <set>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "EventCaretMappableDataFilesAndMapsInDisplayedOverlays.h"

namespace caret {

    class Brain;
    class BrowserTabContent;
    class CaretDataFile;
    class CaretMappableDataFile;
    class ChartDataSource;
    class DataToolTipsManager;
    class Focus;
    class HistologySlicesFile;
    class HtmlTableBuilder;
    class IdentificationFilter;
    class IdentificationManager;
    class IdentificationStringBuilder;
    class MapFileDataSelector;
    class MediaFile;
    class Overlay;
    class OverlaySet;
    class SelectionItemBorderSurface;
    class SelectionItemChartDataSeries;
    class SelectionItemChartFrequencySeries;
    class SelectionItemChartMatrix;
    class SelectionItemChartTwoHistogram;
    class SelectionItemChartTwoLineLayer;
    class SelectionItemChartTwoLineLayerVerticalNearest;
    class SelectionItemChartTwoLineSeries;
    class SelectionItemChartTwoMatrix;
    class SelectionItemCiftiConnectivityMatrixRowColumn;
    class SelectionItemChartTimeSeries;
    class SelectionItemFocusSurface;
    class SelectionItemFocus;
    class SelectionItemHistologyCoordinate;
    class SelectionItemMediaLogicalCoordinate;
    class SelectionItemMediaPlaneCoordinate;
    class SelectionItemSurfaceNode;
    class SelectionItemVoxel;
    class SelectionManager;
    class Surface;
    
    class IdentificationFormattedTextGenerator : public CaretObject {
        
    public:
        IdentificationFormattedTextGenerator();
        
        virtual ~IdentificationFormattedTextGenerator();
        
        AString createIdentificationText(const SelectionManager* idManager,
                                         const Brain* brain,
                                         const int32_t tabIndex) const;
        
        AString createToolTipText(const Brain* brain,
                                  const BrowserTabContent* browserTab,
                                  const SelectionManager* selectionManager,
                                  const DataToolTipsManager* dataToolTipsManager) const;
        
    private:
        IdentificationFormattedTextGenerator(const IdentificationFormattedTextGenerator&);

        IdentificationFormattedTextGenerator& operator=(const IdentificationFormattedTextGenerator&);
        
    public:
        virtual AString toString() const;
        
    private:
        class MapFileAndMapIndices {
        public:
            MapFileAndMapIndices(CaretDataFile* mapFile);
            
            void addMapIndex(const int32_t mapIndex);
            
            void addMapIndices(const std::vector<int32_t> mapIndices);
            
            void addMapIndices(const std::set<int32_t> mapIndices);
            
            CaretDataFile* m_mapFile;
            
            std::set<int32_t> m_mapIndices;
        };
        
        void getFilesForIdentification(const IdentificationFilter* filter,
                                       const int32_t tabIndex,
                                       std::vector<MapFileAndMapIndices>& mapFilesAndIndicesOut,
                                       std::vector<MapFileAndMapIndices>& chartFilesAndIndicesOut,
                                       std::vector<MapFileAndMapIndices>& histologyFilesAndIndicesOut,
                                       std::vector<MapFileAndMapIndices>& mediaFilesAndIndicesOut) const;
        
        void generateSurfaceToolTip(const Brain* brain,
                                    const IdentificationManager* idManager,
                                    const BrowserTabContent* browserTab,
                                    const SelectionManager* selectionManager,
                                    const DataToolTipsManager* dataToolTipsManager,
                                    IdentificationStringBuilder& idText) const;
        
        void generateVolumeToolTip(const IdentificationManager* idManager,
                                   const BrowserTabContent* browserTab,
                                   const SelectionManager* selectionManager,
                                   const DataToolTipsManager* dataToolTipsManager,
                                   IdentificationStringBuilder& idText) const;
        
        void generateChartToolTip(const SelectionManager* selectionManager,
                                  const DataToolTipsManager* dataToolTipsManager,
                                  IdentificationStringBuilder& idText) const;
        
        void generateHistologyPlaneCoordinateToolTip(const IdentificationManager* idManager,
                                                     const SelectionManager* selectionManager,
                                                     const DataToolTipsManager* dataToolTipsManager,
                                                     IdentificationStringBuilder& idText) const;
        
        void generateMediaLogicalCoordinateToolTip(const SelectionManager* selectionManager,
                                                   const DataToolTipsManager* dataToolTipsManager,
                                                   IdentificationStringBuilder& idText) const;

        void generateMediaPlaneCoordinateToolTip(const SelectionManager* selectionManager,
                                                   const DataToolTipsManager* dataToolTipsManager,
                                                   IdentificationStringBuilder& idText) const;
        void generateSurfaceBorderIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                                    IdentificationStringBuilder& idText,
                                                    const SelectionItemBorderSurface* idSurfaceBorder,
                                                    const bool toolTipFlag) const;
        
        void generateSurfaceFocusIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                                  const SelectionItemFocusSurface* idSurfaceFocus,
                                                  const bool toolTipFlag) const;
        
        void generateFocusIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                            IdentificationStringBuilder& idText,
                                            const SelectionItemFocus* idFocus,
                                            const bool toolTipFlag) const;
        
        void generateFocusIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                            IdentificationStringBuilder& idText,
                                            const Focus* focus,
                                            const int32_t focusIndex,
                                            const int32_t projectionIndex,
                                            const bool toolTipFlag) const;
        
        void generateSurfaceVertexIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                               const Brain* brain,
                                               const SelectionItemSurfaceNode* idSurfaceNode) const;
        
        void generateSurfaceDataIdentificationText(HtmlTableBuilder& labelHtmlTableBuilder,
                                                   HtmlTableBuilder& scalarHtmlTableBuilder,
                                                   CaretMappableDataFile* mapFile,
                                                   const std::set<int32_t>& mapIndices,
                                                   const Brain* brain,
                                                   const SelectionItemSurfaceNode* idSurfaceNode) const;

        void generateHistologyPlaneCoordinateIdentificationText(const IdentificationManager* idManager,
                                                                HtmlTableBuilder& htmlTableBuilder,
                                                            IdentificationStringBuilder& idText,
                                                            const SelectionItemHistologyCoordinate* idHistology) const;
        
        void generateMediaLogicalCoordinateIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                             IdentificationStringBuilder& idText,
                                             const MediaFile* mediaFile,
                                             const std::set<int32_t>& frameIndices,
                                             const SelectionItemMediaLogicalCoordinate* idMedia) const;
        
        void generateMediaPlaneCoordinateIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                            IdentificationStringBuilder& idText,
                                                            const MediaFile* mediaFile,
                                                            const std::set<int32_t>& frameIndices,
                                                            const SelectionItemMediaPlaneCoordinate* idMedia) const;

        void generateVolumeVoxelIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                              const Brain* brain,
                                              const SelectionItemVoxel* idVolumeVoxel) const;

        void generateVolumeDataIdentificationText(HtmlTableBuilder& labelHtmlTableBuilder,
                                                  HtmlTableBuilder& scalarHtmlTableBuilder,
                                                  CaretMappableDataFile* mapFile,
                                                  const std::set<int32_t>& mapIndices,
                                                  const Brain* brain,
                                                  const SelectionItemVoxel* idVolumeVoxel) const;

        void generateChartDataSeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                       const SelectionItemChartDataSeries* idChartDataSeries) const;
        
        void generateChartFrequencySeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                       const SelectionItemChartFrequencySeries* idChartFrequencySeries) const;
        
        void generateChartMatrixIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                       const SelectionItemChartMatrix* idChartMatrix) const;

        void generateChartTwoHistogramIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                         IdentificationStringBuilder& idText,
                                                         const SelectionItemChartTwoHistogram* idChartTwoHistogram,
                                                         CaretMappableDataFile* mapFile,
                                                         const std::set<int32_t>& mapIndices,
                                                         const bool toolTipFlag) const;
        
        void generateChartTwoLineLayerNearestIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                          IdentificationStringBuilder& idText,
                                                          const SelectionItemChartTwoLineLayerVerticalNearest* idChartTwoLineLayer,
                                                          CaretMappableDataFile* mapFile,
                                                          const std::set<int32_t>& mapIndices,
                                                          const bool toolTipFlag) const;
        
        void generateChartTwoLineLayerIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                         IdentificationStringBuilder& idText,
                                                         const SelectionItemChartTwoLineLayer* idChartTwoLineLayer,
                                                         CaretMappableDataFile* mapFile,
                                                         const std::set<int32_t>& mapIndices,
                                                         const bool toolTipFlag) const;
        
        void generateChartTwoLineSeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                          IdentificationStringBuilder& idText,
                                                          const SelectionItemChartTwoLineSeries* idChartTwoLineSeries,
                                                          CaretMappableDataFile* mapFile,
                                                          const std::set<int32_t>& mapIndices,
                                                          const bool toolTipFlag) const;
        
        void generateChartTwoMatrixIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                      IdentificationStringBuilder& idText,
                                                      const SelectionItemChartTwoMatrix* idChartTwoMatrix,
                                                      CaretMappableDataFile* mapFile,
                                                      const std::set<int32_t>& mapIndices,
                                                      const bool toolTipFlag) const;

        void generateCiftiConnectivityMatrixIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                               const SelectionItemCiftiConnectivityMatrixRowColumn* idCiftiConnMatrix) const;
        
        void generateChartTimeSeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                       const SelectionItemChartTimeSeries* idChartTimeSeries) const;
        
        void generateChartDataSourceText(HtmlTableBuilder& htmlTableBuilder,
                                         const AString& typeOfChartText,
                                         const ChartDataSource* chartDataSource) const;
        
        void generateMapFileSelectorText(HtmlTableBuilder& htmlTableBuilder,
                                         const MapFileDataSelector* mapFileDataSelector) const;
        
        Overlay* getTopEnabledOverlay(OverlaySet* overlaySet) const;

        std::unique_ptr<HtmlTableBuilder> createHtmlTableBuilder(const int32_t numberOfColumns) const;
        
        bool isParcelAndScalarTypeFile(const DataFileTypeEnum::Enum dataFileType) const;

        void addIfColumnTwoNotEmpty(HtmlTableBuilder& htmlTableBuilder,
                                    const AString& columnOne,
                                    const AString& columnTwo) const;
        
        AString getTextDistanceToMostRecentIdentificationSymbol(const IdentificationManager* idManager,
                                                                const float selectionXYZ[3]) const;
        
        AString xyToText(const float xy[2],
                         const int32_t precisionDigits = -1) const;
        
        AString xyzToText(const float xyz[3],
                          const int32_t precisionDigits = -1) const;
        
        AString dataValueToText(const float value,
                                const int32_t precisionDigits = -1) const;
        
        const AString m_noDataText;
        
        static const int32_t s_dataValueDigitsRightOfDecimal;
        friend class DataToolTipsManager;
    };
    
#ifdef __IDENTIFICATION_FORMATTED_TEXT_GENERATOR_DECLARE__
    const int32_t IdentificationFormattedTextGenerator::s_dataValueDigitsRightOfDecimal = 4;
#endif // __IDENTIFICATION_SIMPLE_TEXT_GENERATOR_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_FORMATTED_TEXT_GENERATOR__H_
