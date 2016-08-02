#ifndef __IDENTIFICATION_TEXT_GENERATOR__H_
#define __IDENTIFICATION_TEXT_GENERATOR__H_

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
    class SelectionItemBorderSurface;
    class SelectionItemChartDataSeries;
    class SelectionItemChartFrequencySeries;
    class SelectionItemChartMatrix;
    class SelectionItemCiftiConnectivityMatrixRowColumn;
    class SelectionItemChartTimeSeries;
    class SelectionItemFocusSurface;
    class SelectionItemFocusVolume;
    class SelectionItemImage;
    class SelectionItemSurfaceNode;
    class SelectionItemVoxel;
    class SelectionManager;
    class IdentificationStringBuilder;
    
    class IdentificationTextGenerator : public CaretObject {
        
    public:
        IdentificationTextGenerator();
        
        virtual ~IdentificationTextGenerator();
        
        AString createIdentificationText(const SelectionManager* idManager,
                                         const Brain* brain) const;
        
    private:
        IdentificationTextGenerator(const IdentificationTextGenerator&);

        IdentificationTextGenerator& operator=(const IdentificationTextGenerator&);
        
    public:
        virtual AString toString() const;
        
    private:
        void generateSurfaceBorderIdentifcationText(IdentificationStringBuilder& idText,
                                                    const SelectionItemBorderSurface* idSurfaceBorder) const;
        
        void generateSurfaceFociIdentifcationText(IdentificationStringBuilder& idText,
                                                    const SelectionItemFocusSurface* idSurfaceFocus) const;
        
        void generateVolumeFociIdentifcationText(IdentificationStringBuilder& idText,
                                                  const SelectionItemFocusVolume* idVolumeFocus) const;
        
        void generateSurfaceIdentificationText(IdentificationStringBuilder& idText,
                                               const Brain* brain,
                                               const SelectionItemSurfaceNode* idSurfaceNode) const;
        
        void generateImageIdentificationText(IdentificationStringBuilder& idText,
                                             const SelectionItemImage* idImage) const;
        
        void generateVolumeIdentificationText(IdentificationStringBuilder& idText,
                                               const Brain* brain,
                                               const SelectionItemVoxel* idVolumeVoxel) const;
        
        void generateChartDataSeriesIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartDataSeries* idChartDataSeries) const;
        
        void generateChartFrequencySeriesIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartFrequencySeries* idChartFrequencySeries) const;
        
        void generateChartMatrixIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartMatrix* idChartMatrix) const;

        void generateCiftiConnectivityMatrixIdentificationText(IdentificationStringBuilder& idText,
                                                               const SelectionItemCiftiConnectivityMatrixRowColumn* idCiftiConnMatrix) const;
        
        void generateChartTimeSeriesIdentificationText(IdentificationStringBuilder& idText,
                                                       const SelectionItemChartTimeSeries* idChartTimeSeries) const;
        
        void getMapIndicesOfFileUsedInOverlays(const CaretMappableDataFile* caretMappableDataFile,
                                               std::vector<int32_t>& mapIndicesOut) const;
        
        void generateChartDataSourceText(IdentificationStringBuilder& idText,
                                         const AString& typeOfChartText,
                                         const ChartDataSource* chartDataSource) const;
    };
    
#ifdef __IDENTIFICATION_TEXT_GENERATOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_TEXT_GENERATOR_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_TEXT_GENERATOR__H_
