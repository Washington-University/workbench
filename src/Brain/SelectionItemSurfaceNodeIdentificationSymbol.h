#ifndef __SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL__H_
#define __SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL__H_

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
#include "StructureEnum.h"

namespace caret {

    class Surface;
    
    class SelectionItemSurfaceNodeIdentificationSymbol : public SelectionItem {
        
    public:
        SelectionItemSurfaceNodeIdentificationSymbol();
        
        virtual ~SelectionItemSurfaceNodeIdentificationSymbol();
        
        virtual bool isValid() const;
        
        void setSurface(Surface* surface);
        
        int32_t getNodeNumber() const;
        
        int32_t getSurfaceNumberOfNodes() const;
        
        StructureEnum::Enum getStructure() const;
        
        void set(const StructureEnum::Enum structure,
                 const int32_t surfaceNumberOfNodes,
                 const int32_t nodeNumber);
        
        virtual void reset();
        
        virtual AString toString() const;
    private:
        SelectionItemSurfaceNodeIdentificationSymbol(const SelectionItemSurfaceNodeIdentificationSymbol&);

        SelectionItemSurfaceNodeIdentificationSymbol& operator=(const SelectionItemSurfaceNodeIdentificationSymbol&);
        
    public:
    private:
        void setSurfaceNumberOfNodes(const int32_t numberOfNodes);
        
        void setNodeNumber(const int32_t nodeNumber);
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        Surface* surface;
        
        int32_t nodeNumber;
        
        int32_t surfaceNumberOfNodes;
        
        StructureEnum::Enum structure;
    };
    
#ifdef __SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL__H_
