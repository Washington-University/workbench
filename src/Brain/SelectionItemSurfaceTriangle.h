#ifndef __SELECTION_ITEM_SURFACE_TRIANGLE__H_
#define __SELECTION_ITEM_SURFACE_TRIANGLE__H_

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


#include "SelectionItem.h"

namespace caret {

    class Surface;
    
    class SelectionItemSurfaceTriangle : public SelectionItem {
        
    public:
        SelectionItemSurfaceTriangle();
        
        virtual ~SelectionItemSurfaceTriangle();
        
        virtual bool isValid() const;
        
        Surface* getSurface();
        
        void setSurface(Surface* surface);
        
        int32_t getTriangleNumber() const;
        
        void setTriangleNumber(const int32_t triangleNumber);
        
        int32_t getNearestNodeNumber() const;
        
        void setNearestNode(const int32_t nearestNodeNumber);
        
        void getNearestNodeScreenXYZ(double screenXYZ[3]) const;
        
        void setNearestNodeScreenXYZ(const double screenXYZ[3]);
        
        void getNearestNodeModelXYZ(double modelXYZ[3]) const;
        
        void setNearestNodeModelXYZ(const double modelXYZ[3]);
        
        virtual void reset();
        
        virtual AString toString() const;
    private:
        SelectionItemSurfaceTriangle(const SelectionItemSurfaceTriangle&);

        SelectionItemSurfaceTriangle& operator=(const SelectionItemSurfaceTriangle&);
        
    private:
        Surface* surface;
        
        int32_t triangleNumber;
        
        int32_t nearestNodeNumber;
        
        double nearestNodeScreenXYZ[3];
        
        double nearestNodeModelXYZ[3];
        
    };
    
#ifdef __SELECTION_ITEM_SURFACE_TRIANGLE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_SURFACE_TRIANGLE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_SURFACE_TRIANGLE__H_
