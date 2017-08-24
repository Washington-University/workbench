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
    class SelectionItemChartTwoLineSeries;
    class SelectionItemChartTwoMatrix;
    class SelectionItemCiftiConnectivityMatrixRowColumn;
    class SelectionItemFocusSurface;
    class SelectionItemFocusVolume;
    class SelectionItemImage;
    class SelectionItemImageControlPoint;
    class SelectionItemSurfaceNode;
    class SelectionItemSurfaceNodeIdentificationSymbol;
    class SelectionItemSurfaceTriangle;
    class SelectionItemVoxel;
    class SelectionItemVoxelEditing;
    class SelectionItemVoxelIdentificationSymbol;
    class IdentificationTextGenerator;
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
        
        SelectionItemFocusVolume* getVolumeFocusIdentification();
        
        const SelectionItemFocusVolume* getVolumeFocusIdentification() const;
        
        SelectionItemImage* getImageIdentification();
        
        const SelectionItemImage* getImageIdentification() const;
        
        SelectionItemImageControlPoint* getImageControlPointIdentification();
        
        const SelectionItemImageControlPoint* getImageControlPointIdentification() const;
        
        SelectionItemSurfaceNode* getSurfaceNodeIdentification();
        
        const SelectionItemSurfaceNode* getSurfaceNodeIdentification() const;
        
        SelectionItemSurfaceNodeIdentificationSymbol* getSurfaceNodeIdentificationSymbol();
        
        const SelectionItemSurfaceNodeIdentificationSymbol* getSurfaceNodeIdentificationSymbol() const;
        
        SelectionItemSurfaceTriangle* getSurfaceTriangleIdentification();
        
        const SelectionItemSurfaceTriangle* getSurfaceTriangleIdentification() const;
        
        const SelectionItemVoxel* getVoxelIdentification() const;
        
        SelectionItemVoxel* getVoxelIdentification();
        
        SelectionItemVoxelIdentificationSymbol* getVoxelIdentificationSymbol();
        
        const SelectionItemVoxelIdentificationSymbol* getVoxelIdentificationSymbol() const;
        
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
        
        SelectionItemChartTwoLineSeries* getChartTwoLineSeriesIdentification();
        
        const SelectionItemChartTwoLineSeries* getChartTwoLineSeriesIdentification() const;
        
        SelectionItemChartTwoLabel* getChartTwoLabelIdentification();
        
        const SelectionItemChartTwoLabel* getChartTwoLabelIdentification() const;
        
        SelectionItemChartTwoMatrix* getChartTwoMatrixIdentification();
        
        const SelectionItemChartTwoMatrix* getChartTwoMatrixIdentification() const;
        
        AString getIdentificationText(const Brain* brain) const;
        
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
        
        std::unique_ptr<SelectionItemChartTwoLabel> m_chartTwoLabelIdentification;
        
        std::unique_ptr<SelectionItemChartTwoMatrix>     m_chartTwoMatrixIdentification;

        SelectionItemCiftiConnectivityMatrixRowColumn* m_ciftiConnectivityMatrixRowColumnIdentfication;
        
        SelectionItemFocusSurface* m_surfaceFocusIdentification;
        
        SelectionItemFocusVolume* m_volumeFocusIdentification;
        
        SelectionItemImage* m_imageIdentification;
        
        SelectionItemImageControlPoint* m_imageControlPointIdentification;
        
        SelectionItemSurfaceNode* m_surfaceNodeIdentification;
        
        SelectionItemSurfaceNodeIdentificationSymbol* m_surfaceNodeIdentificationSymbol;
        
        SelectionItemSurfaceTriangle* m_surfaceTriangleIdentification;
        
        IdentificationTextGenerator* m_idTextGenerator;
        
        SelectionItemVoxel* m_voxelIdentification;
        
        SelectionItemVoxelIdentificationSymbol* m_voxelIdentificationSymbol;
        
        SelectionItemVoxelEditing* m_voxelEditingIdentification;
        
        /** Last selected item DOES NOT GET PUT IN m_allSelectionItems */
        SelectionItem* m_lastSelectedItem;
    };
    
#ifdef __SELECTION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_MANAGER_DECLARE__

} // namespace
#endif  //__SELECTION_MANAGER__H_
