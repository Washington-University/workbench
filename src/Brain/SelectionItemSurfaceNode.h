#ifndef __IDENTIFICATION_ITEM_SURFACE_NODE__H_
#define __IDENTIFICATION_ITEM_SURFACE_NODE__H_

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
    
    class SelectionItemSurfaceNode : public SelectionItem {
        
    public:
        SelectionItemSurfaceNode();
        
        virtual ~SelectionItemSurfaceNode();
        
        SelectionItemSurfaceNode(const SelectionItemSurfaceNode&);
        
        SelectionItemSurfaceNode& operator=(const SelectionItemSurfaceNode&);
        
        virtual bool isValid() const;
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        void setSurface(Surface* surface);
        
        int32_t getNodeNumber() const;
        
        void setNodeNumber(const int32_t nodeNumber);
        
        bool isContralateral() const;
        
        void setContralateral(const bool status);
        
        virtual void reset();
        
        virtual AString toString() const;

    private:
        void copyHelperSelectionItemSurfaceNode(const SelectionItemSurfaceNode& idItem);
        
    public:
    private:
        Surface* m_surface;
        
        int32_t m_nodeNumber;
        
        bool m_contralateralFlag;
    };
    
#ifdef __IDENTIFICATION_ITEM_SURFACE_NODE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_ITEM_SURFACE_NODE_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_ITEM_SURFACE_NODE__H_
