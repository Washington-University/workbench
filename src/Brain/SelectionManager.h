#ifndef __SELECTION_MANAGER__H_
#define __SELECTION_MANAGER__H_

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

#include "CaretObject.h"
#include "EventListenerInterface.h"

namespace caret {
    class Annotation;
    class Brain;
    class BrowserTabContent;
    class SelectionItemAnnotation;
    class SelectionItem;
    class SelectionItemBorderSurface;
    class SelectionItemChartDataSeries;
    class SelectionItemChartFrequencySeries;
    class SelectionItemChartMatrix;
    class SelectionItemChartTimeSeries;
    class SelectionItemChartTwoHistogram;
    class SelectionItemChartTwoLabel;
    class SelectionItemChartTwoLineLayer;
    class SelectionItemChartTwoLineLayerVerticalNearest;
    class SelectionItemChartTwoLineSeries;
    class SelectionItemChartTwoMatrix;
    class SelectionItemCiftiConnectivityMatrixRowColumn;
    class SelectionItemFocusSurface;
    class SelectionItemFocus;
    class SelectionItemHistologyCoordinate;
    class SelectionItemHistologyStereotaxicCoordinate;
    class SelectionItemImageControlPoint;
    class SelectionItemMediaLogicalCoordinate;
    class SelectionItemMediaPlaneCoordinate;
    class SelectionItemSurfaceNode;
    class SelectionItemSurfaceTriangle;
    class SelectionItemUniversalIdentificationSymbol;
    class SelectionItemVolumeMprCrosshair;
    class SelectionItemVoxel;
    class SelectionItemVoxelEditing;
    class IdentificationFormattedTextGenerator;
    class IdentificationSimpleTextGenerator;
    class Surface;
    
    class SelectionManager : public CaretObject, public EventListenerInterface {
        
    public:
        SelectionManager();
        
        virtual ~SelectionManager();
        
        void receiveEvent(Event* event);
        
        void reset();

        SelectionItemAnnotation* getAnnotationIdentification();
        
        const SelectionItemAnnotation* getAnnotationIdentification() const;
        
        SelectionItemBorderSurface* getSurfaceBorderIdentification();
        
        const SelectionItemBorderSurface* getSurfaceBorderIdentification() const;
        
        SelectionItemFocusSurface* getSurfaceFocusIdentification();
        
        const SelectionItemFocusSurface* getSurfaceFocusIdentification() const;
        
        SelectionItemFocus* getFocusIdentification();
        
        const SelectionItemFocus* getFocusIdentification() const;
        
        SelectionItemHistologyCoordinate* getHistologyPlaneCoordinateIdentification();
        
        const SelectionItemHistologyCoordinate* getHistologyPlaneCoordinateIdentification() const;
        
        SelectionItemHistologyStereotaxicCoordinate* getHistologyStereotaxicCoordinateIdentification();
        
        const SelectionItemHistologyStereotaxicCoordinate* getHistologyStereotaxicCoordinateIdentification() const;
        
        SelectionItemImageControlPoint* getImageControlPointIdentification();
        
        const SelectionItemImageControlPoint* getImageControlPointIdentification() const;
        
        SelectionItemMediaLogicalCoordinate* getMediaLogicalCoordinateIdentification();
        
        const SelectionItemMediaLogicalCoordinate* getMediaLogicalCoordinateIdentification() const;
        
        SelectionItemMediaPlaneCoordinate* getMediaPlaneCoordinateIdentification();
        
        const SelectionItemMediaPlaneCoordinate* getMediaPlaneCoordinateIdentification() const;
        
        SelectionItemSurfaceNode* getSurfaceNodeIdentification();
        
        const SelectionItemSurfaceNode* getSurfaceNodeIdentification() const;
        
        SelectionItemSurfaceTriangle* getSurfaceTriangleIdentification();
        
        const SelectionItemSurfaceTriangle* getSurfaceTriangleIdentification() const;
        
        const SelectionItemVoxel* getVoxelIdentification() const;
        
        SelectionItemVoxel* getVoxelIdentification();
        
        SelectionItemUniversalIdentificationSymbol* getUniversalIdentificationSymbol();

        const SelectionItemUniversalIdentificationSymbol* getUniversalIdentificationSymbol() const;
        
        SelectionItemVolumeMprCrosshair* getVolumeMprCrosshairIdentification();
        
        const SelectionItemVolumeMprCrosshair* getVolumeMprCrosshairIdentification() const;
        
        SelectionItemVoxelEditing* getVoxelEditingIdentification();
        
        const SelectionItemVoxelEditing* getVoxelEditingIdentification() const;
        
        SelectionItemChartDataSeries* getChartDataSeriesIdentification();
        
        const SelectionItemChartDataSeries* getChartDataSeriesIdentification() const;
        
        SelectionItemChartFrequencySeries* getChartFrequencySeriesIdentification();
        
        const SelectionItemChartFrequencySeries* getChartFrequencySeriesIdentification() const;
        
        SelectionItemChartMatrix* getChartMatrixIdentification();
        
        const SelectionItemChartMatrix* getChartMatrixIdentification() const;
        
        SelectionItemChartTimeSeries* getChartTimeSeriesIdentification();
        
        const SelectionItemChartTimeSeries* getChartTimeSeriesIdentification() const;
        
        SelectionItemCiftiConnectivityMatrixRowColumn* getCiftiConnectivityMatrixRowColumnIdentification();
        
        const SelectionItemCiftiConnectivityMatrixRowColumn* getCiftiConnectivityMatrixRowColumnIdentification() const;
        
        SelectionItemChartTwoHistogram* getChartTwoHistogramIdentification();
        
        const SelectionItemChartTwoHistogram* getChartTwoHistogramIdentification() const;
        
        SelectionItemChartTwoLineLayer* getChartTwoLineLayerIdentification();
        
        const SelectionItemChartTwoLineLayer* getChartTwoLineLayerIdentification() const;
        
        SelectionItemChartTwoLineLayerVerticalNearest* getChartTwoLineLayerVerticalNearestIdentification();
        
        const SelectionItemChartTwoLineLayerVerticalNearest* getChartTwoLineLayerVerticalNearestIdentification() const;
        
        SelectionItemChartTwoLineSeries* getChartTwoLineSeriesIdentification();
        
        const SelectionItemChartTwoLineSeries* getChartTwoLineSeriesIdentification() const;
        
        SelectionItemChartTwoLabel* getChartTwoLabelIdentification();
        
        const SelectionItemChartTwoLabel* getChartTwoLabelIdentification() const;
        
        SelectionItemChartTwoMatrix* getChartTwoMatrixIdentification();
        
        const SelectionItemChartTwoMatrix* getChartTwoMatrixIdentification() const;
        
        AString getSimpleIdentificationText(const Brain* brain) const;
        
        AString getFormattedIdentificationText(const Brain* brain,
                                               const int32_t tabIndex) const;
        
        void filterSelections(const bool applySelectionBackgroundFiltering);
        
        void clearDistantSelections();
        
        void clearOtherSelectedItems(SelectionItem* selectedItem);
        
        const SelectionItem* getLastSelectedItem() const;
        
        void setLastSelectedItem(const SelectionItem* lastItem);
        
        void setAllSelectionsEnabled(const bool status);
        
    private:
        SelectionManager(const SelectionManager&);

        SelectionManager& operator=(const SelectionManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        SelectionItem* getMinimumDepthFromMultipleSelections(std::vector<SelectionItem*> items) const;

        /** ALL items */
        std::vector<SelectionItem*> m_allSelectionItems;
        
        /** Layered items (foci, borders, etc.) */
        std::vector<SelectionItem*> m_layeredSelectedItems;
        
        /** Surface items (nodes, triangles) */
        std::vector<SelectionItem*> m_surfaceSelectedItems;
        
        /** Volume items */
        std::vector<SelectionItem*> m_volumeSelectedItems;
        
        SelectionItemAnnotation* m_annotationIdentification;
        
        SelectionItemBorderSurface* m_surfaceBorderIdentification;
        
        SelectionItemChartDataSeries* m_chartDataSeriesIdentification;
        
        SelectionItemChartFrequencySeries* m_chartDataFrequencyIdentification;
        
        SelectionItemChartMatrix* m_chartMatrixIdentification;
        
        SelectionItemChartTimeSeries* m_chartTimeSeriesIdentification;
        
        std::unique_ptr<SelectionItemChartTwoHistogram>  m_chartTwoHistogramIdentification;
        
        std::unique_ptr<SelectionItemChartTwoLineSeries> m_chartTwoLineSeriesIdentification;
        
        std::unique_ptr<SelectionItemChartTwoLineLayer> m_chartTwoLineLayerIdentification;
        
        std::unique_ptr<SelectionItemChartTwoLineLayerVerticalNearest> m_chartTwoLineLayerVerticalNearestIdentification;
        
        std::unique_ptr<SelectionItemChartTwoLabel> m_chartTwoLabelIdentification;
        
        std::unique_ptr<SelectionItemChartTwoMatrix>     m_chartTwoMatrixIdentification;

        SelectionItemCiftiConnectivityMatrixRowColumn* m_ciftiConnectivityMatrixRowColumnIdentfication;
        
        SelectionItemFocusSurface* m_surfaceFocusIdentification;
        
        SelectionItemFocus* m_focusIdentification;
        
        SelectionItemImageControlPoint* m_imageControlPointIdentification;
        
        std::unique_ptr<SelectionItemHistologyCoordinate> m_histologyPlaneCoordinateIdentification;
        
        std::unique_ptr<SelectionItemHistologyStereotaxicCoordinate> m_histologyStereotaxicCoordinateIdentification;
        
        std::unique_ptr<SelectionItemMediaLogicalCoordinate> m_mediaLogicalCoordinateIdentification;
        
        std::unique_ptr<SelectionItemMediaPlaneCoordinate> m_mediaPlaneCoordinateIdentification;
        
        SelectionItemSurfaceNode* m_surfaceNodeIdentification;
        
        SelectionItemSurfaceTriangle* m_surfaceTriangleIdentification;
        
        IdentificationSimpleTextGenerator* m_idTextGenerator;
        
        std::unique_ptr<IdentificationFormattedTextGenerator> m_idFormattedTextGenerator;
        
        std::unique_ptr<SelectionItemUniversalIdentificationSymbol> m_universalIdentificationSymbol;
        
        SelectionItemVoxel* m_voxelIdentification;
        
        SelectionItemVoxelEditing* m_voxelEditingIdentification;
        
        std::unique_ptr<SelectionItemVolumeMprCrosshair> m_volumeMprCrosshairIdentification;
        
        /** Last selected item DOES NOT GET PUT IN m_allSelectionItems */
        SelectionItem* m_lastSelectedItem;
    };
    
#ifdef __SELECTION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_MANAGER_DECLARE__

} // namespace
#endif  //__SELECTION_MANAGER__H_
