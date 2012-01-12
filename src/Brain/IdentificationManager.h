#ifndef __IDENTIFICATION_MANAGER__H_
#define __IDENTIFICATION_MANAGER__H_

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

namespace caret {
    class Brain;
    class BrowserTabContent;
    class IdentificationItem;
    class IdentificationItemBorderSurface;
    class IdentificationItemSurfaceNode;
    class IdentificationItemSurfaceNodeIdentificationSymbol;
    class IdentificationItemSurfaceTriangle;
    class IdentificationItemVoxel;
    class IdentificationTextGenerator;
    class Surface;
    
    class IdentificationManager : public CaretObject {
        
    public:
        IdentificationManager();
        
        virtual ~IdentificationManager();
        
        void reset();
        
        IdentificationItemBorderSurface* getSurfaceBorderIdentification();
        
        const IdentificationItemBorderSurface* getSurfaceBorderIdentification() const;
        
        IdentificationItemSurfaceNode* getSurfaceNodeIdentification();
        
        const IdentificationItemSurfaceNode* getSurfaceNodeIdentification() const;
        
        IdentificationItemSurfaceNodeIdentificationSymbol* getSurfaceNodeIdentificationSymbol();
        
        const IdentificationItemSurfaceNodeIdentificationSymbol* getSurfaceNodeIdentificationSymbol() const;
        
        IdentificationItemSurfaceTriangle* getSurfaceTriangleIdentification();
        
        const IdentificationItemSurfaceTriangle* getSurfaceTriangleIdentification() const;
        
        const IdentificationItemVoxel* getVoxelIdentification() const;
        
        IdentificationItemVoxel* getVoxelIdentification();
        
        AString getIdentificationText(const BrowserTabContent* browserTabContent,
                                      const Brain* brain) const;
        
        void filterSelections();
        
        void clearDistantSelections();
        
        void addAdditionalSurfaceNodeIdentification(Surface* surface,
                                                    const int32_t nodeIndex,
                                                    bool isInterhemisphericIdentification);
        
        int32_t getNumberOfAdditionalSurfaceNodeIdentifications() const;
        
        IdentificationItemSurfaceNode* getAdditionalSurfaceNodeIdentification(const int32_t indx);
        
        const IdentificationItemSurfaceNode* getAdditionalSurfaceNodeIdentification(const int32_t indx) const;
        
        
    private:
        IdentificationManager(const IdentificationManager&);

        IdentificationManager& operator=(const IdentificationManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        IdentificationItem* getMinimumDepthFromMultipleSelections(std::vector<IdentificationItem*> items) const;

        /** ALL items */
        std::vector<IdentificationItem*> allIdentificationItems;
        
        /** Layered items (foci, borders, etc.) */
        std::vector<IdentificationItem*> layeredSelectedItems;
        
        /** Surface items (nodes, triangles) */
        std::vector<IdentificationItem*> surfaceSelectedItems;
        
        /** Volume items */
        std::vector<IdentificationItem*> volumeSelectedItems;
        
        IdentificationItemBorderSurface* surfaceBorderIdentification;
        
        IdentificationItemSurfaceNode* surfaceNodeIdentification;
        
        IdentificationItemSurfaceNodeIdentificationSymbol* surfaceNodeIdentificationSymbol;
        
        IdentificationItemSurfaceTriangle* surfaceTriangleIdentification;
        
        IdentificationTextGenerator* idTextGenerator;
        
        IdentificationItemVoxel* voxelIdentification;
        
        std::vector<IdentificationItemSurfaceNode*> additionalSurfaceNodeIdentifications;
    };
    
#ifdef __IDENTIFICATION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_MANAGER_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_MANAGER__H_
