#ifndef __SELECTION_MANAGER__H_
#define __SELECTION_MANAGER__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "CaretObject.h"
#include "EventListenerInterface.h"

namespace caret {
    class Brain;
    class BrowserTabContent;
    class SelectionItem;
    class SelectionItemBorderSurface;
    class SelectionItemFocusSurface;
    class SelectionItemFocusVolume;
    class SelectionItemSurfaceNode;
    class SelectionItemSurfaceNodeIdentificationSymbol;
    class SelectionItemSurfaceTriangle;
    class SelectionItemVoxel;
    class IdentificationTextGenerator;
    class Surface;
    
    class SelectionManager : public CaretObject, public EventListenerInterface {
        
    public:
        SelectionManager();
        
        virtual ~SelectionManager();
        
        void receiveEvent(Event* event);
        
        void reset();
        
        SelectionItemBorderSurface* getSurfaceBorderIdentification();
        
        const SelectionItemBorderSurface* getSurfaceBorderIdentification() const;
        
        SelectionItemFocusSurface* getSurfaceFocusIdentification();
        
        const SelectionItemFocusSurface* getSurfaceFocusIdentification() const;
        
        SelectionItemFocusVolume* getVolumeFocusIdentification();
        
        const SelectionItemFocusVolume* getVolumeFocusIdentification() const;
        
        SelectionItemSurfaceNode* getSurfaceNodeIdentification();
        
        const SelectionItemSurfaceNode* getSurfaceNodeIdentification() const;
        
        SelectionItemSurfaceNodeIdentificationSymbol* getSurfaceNodeIdentificationSymbol();
        
        const SelectionItemSurfaceNodeIdentificationSymbol* getSurfaceNodeIdentificationSymbol() const;
        
        SelectionItemSurfaceTriangle* getSurfaceTriangleIdentification();
        
        const SelectionItemSurfaceTriangle* getSurfaceTriangleIdentification() const;
        
        const SelectionItemVoxel* getVoxelIdentification() const;
        
        SelectionItemVoxel* getVoxelIdentification();
        
        AString getIdentificationText(const BrowserTabContent* browserTabContent,
                                      const Brain* brain) const;
        
        void filterSelections(const bool applySelectionBackgroundFiltering);
        
        void clearDistantSelections();
        
        void clearOtherSelectedItems(SelectionItem* selectedItem);
        
        void addAdditionalSurfaceNodeIdentification(Surface* surface,
                                                    const int32_t nodeIndex,
                                                    bool isContralateralIdentification);
        
        int32_t getNumberOfAdditionalSurfaceNodeIdentifications() const;
        
        SelectionItemSurfaceNode* getAdditionalSurfaceNodeIdentification(const int32_t indx);
        
        const SelectionItemSurfaceNode* getAdditionalSurfaceNodeIdentification(const int32_t indx) const;
        
        const SelectionItem* getLastSelectedItem() const;
        
        void setLastSelectedItem(const SelectionItem* lastItem);
        
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
        
        SelectionItemBorderSurface* m_surfaceBorderIdentification;
        
        SelectionItemFocusSurface* m_surfaceFocusIdentification;
        
        SelectionItemFocusVolume* m_volumeFocusIdentification;
        
        SelectionItemSurfaceNode* m_surfaceNodeIdentification;
        
        SelectionItemSurfaceNodeIdentificationSymbol* m_surfaceNodeIdentificationSymbol;
        
        SelectionItemSurfaceTriangle* m_surfaceTriangleIdentification;
        
        IdentificationTextGenerator* m_idTextGenerator;
        
        SelectionItemVoxel* m_voxelIdentification;
        
        std::vector<SelectionItemSurfaceNode*> m_additionalSurfaceNodeIdentifications;
        
        /** Last selected item DOES NOT GET PUT IN m_allSelectionItems */
        SelectionItem* m_lastSelectedItem;
    };
    
#ifdef __SELECTION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_MANAGER_DECLARE__

} // namespace
#endif  //__SELECTION_MANAGER__H_
