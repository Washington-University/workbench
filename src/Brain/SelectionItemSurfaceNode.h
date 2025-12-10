#ifndef __SELECTION_ITEM_SURFACE_NODE__H_
#define __SELECTION_ITEM_SURFACE_NODE__H_

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
        
//        bool isContralateral() const;
//        
//        void setContralateral(const bool status);
//        
        virtual void reset();
        
        virtual AString toString() const;

        virtual AString getBrainordinateInformation() const override;
        
    private:
        void copyHelperSelectionItemSurfaceNode(const SelectionItemSurfaceNode& idItem);
        
    public:
    private:
        Surface* m_surface;
        
        int32_t m_nodeNumber;
        
        bool m_contralateralFlag;
    };
    
#ifdef __SELECTION_ITEM_SURFACE_NODE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_SURFACE_NODE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_SURFACE_NODE__H_
