#ifndef __IDENTIFICATION_SIMPLE_TEXT_GENERATOR__H_
#define __IDENTIFICATION_SIMPLE_TEXT_GENERATOR__H_

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


#include "CaretObject.h"

namespace caret {

    class Brain;
    class BrowserTabContent;
    class CaretMappableDataFile;
    class ChartDataSource;
    class DataToolTipsManager;
    class MapFileDataSelector;
    class Overlay;
    class OverlaySet;
    class SelectionItemBorderSurface;
    class SelectionItemChartDataSeries;
    class SelectionItemChartFrequencySeries;
    class SelectionItemChartMatrix;
    class SelectionItemChartTwoHistogram;
    class SelectionItemChartTwoLineLayerVerticalNearest;
    class SelectionItemChartTwoLineSeries;
    class SelectionItemChartTwoMatrix;
    class SelectionItemCiftiConnectivityMatrixRowColumn;
    class SelectionItemChartTimeSeries;
    class SelectionItemFocusSurface;
    class SelectionItemFocus;
    class SelectionItemMediaLogicalCoordinate;
    class SelectionItemMediaPlaneCoordinate;
    class SelectionItemSurfaceNode;
    class SelectionItemVoxel;
    class SelectionManager;
    class IdentificationStringBuilder;
    
    class IdentificationSimpleTextGenerator : public CaretObject {
        
    public:
        IdentificationSimpleTextGenerator();
        
        virtual ~IdentificationSimpleTextGenerator();
        
        AString createIdentificationText(const SelectionManager* idManager,
                                         const Brain* brain) const;
        
        AString createToolTipText(const Brain* brain,
                                  const BrowserTabContent* browserTab,
                                  const SelectionManager* selectionManager,
                                  const DataToolTipsManager* dataToolTipsManager) const;
        
    private:
        IdentificationSimpleTextGenerator(const IdentificationSimpleTextGenerator&);

        IdentificationSimpleTextGenerator& operator=(const IdentificationSimpleTextGenerator&);
        
    public:
        virtual AString toString() const;
        
    private:
        void generateSurfaceToolTip(const Brain* brain,
                                    const BrowserTabContent* browserTab,
                                    const SelectionManager* selectionManager,
                                    const DataToolTipsManager* dataToolTipsManager,
                                    IdentificationStringBuilder& idText) const;
        
        void generateVolumeToolTip(const BrowserTabContent* browserTab,
                                   const SelectionManager* selectionManager,
                                   const DataToolTipsManager* dataToolTipsManager,
                                   IdentificationStringBuilder& idText) const;
        
        void generateChartToolTip(const SelectionManager* selectionManager,
                                  const DataToolTipsManager* dataToolTipsManager,
                                  IdentificationStringBuilder& idText) const;
        
        void generateSurfaceBorderIdentifcationText(IdentificationStringBuilder& idText,
                                                    const SelectionItemBorderSurface* idSurfaceBorder,
                                                    const bool toolTipFlag) const;
        
        void generateSurfaceFociIdentifcationText(IdentificationStringBuilder& idText,
                                                  const SelectionItemFocusSurface* idSurfaceFocus,
                                                  const bool toolTipFlag) const;
        
        void generateFociIdentifcationText(IdentificationStringBuilder& idText,
                                                  const SelectionItemFocus* idFocus) const;
        
        void generateSurfaceIdentificationText(IdentificationStringBuilder& idText,
                                               const Brain* brain,
                                               const SelectionItemSurfaceNode* idSurfaceNode) const;
        
        void generateMediaLogicalCoordinateIdentificationText(IdentificationStringBuilder& idText,
                                             const SelectionItemMediaLogicalCoordinate* idImage) const;
        
        void generateMediaPlaneCoordinateIdentificationText(IdentificationStringBuilder& idText,
                                             const SelectionItemMediaPlaneCoordinate* idImage) const;
        
        void generateVolumeIdentificationText(IdentificationStringBuilder& idText,
                                               const Brain* brain,
                                               const SelectionItemVoxel* idVolumeVoxel) const;
        
        void generateChartDataSeriesIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartDataSeries* idChartDataSeries) const;
        
        void generateChartFrequencySeriesIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartFrequencySeries* idChartFrequencySeries) const;
        
        void generateChartMatrixIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartMatrix* idChartMatrix) const;

        void generateChartTwoHistogramIdentificationText(IdentificationStringBuilder& idText,
                                                         const SelectionItemChartTwoHistogram* idChartTwoHistogram) const;
        
        void generateChartTwoLineLayerIdentificationText(IdentificationStringBuilder& idText,
                                                         const SelectionItemChartTwoLineLayerVerticalNearest* idChartTwoLineLayer) const;
        
        void generateChartTwoLineSeriesIdentificationText(IdentificationStringBuilder& idText,
                                                          const SelectionItemChartTwoLineSeries* idChartTwoLineSeries) const;
        
        void generateChartTwoMatrixIdentificationText(IdentificationStringBuilder& idText,
                                                      const SelectionItemChartTwoMatrix* idChartTwoMatrix) const;

        void generateCiftiConnectivityMatrixIdentificationText(IdentificationStringBuilder& idText,
                                                               const SelectionItemCiftiConnectivityMatrixRowColumn* idCiftiConnMatrix) const;
        
        void generateChartTimeSeriesIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartTimeSeries* idChartTimeSeries) const;
        
        void getMapIndicesOfFileUsedInOverlays(const CaretMappableDataFile* caretMappableDataFile,
                                               std::vector<int32_t>& mapIndicesOut) const;
        
        void generateChartDataSourceText(IdentificationStringBuilder& idText,
                                         const AString& typeOfChartText,
                                         const ChartDataSource* chartDataSource) const;
        
        void generateMapFileSelectorText(IdentificationStringBuilder& idText,
                                         const MapFileDataSelector* mapFileDataSelector) const;
        
        Overlay* getTopEnabledOverlay(OverlaySet* overlaySet) const;

        friend class DataToolTipsManager;
    };
    
#ifdef __IDENTIFICATION_SIMPLE_TEXT_GENERATOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_SIMPLE_TEXT_GENERATOR_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_SIMPLE_TEXT_GENERATOR__H_
