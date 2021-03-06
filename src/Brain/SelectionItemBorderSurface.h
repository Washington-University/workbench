#ifndef __SELECTION_ITEM_BORDER_SURFACE__H_
#define __SELECTION_ITEM_BORDER_SURFACE__H_

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

    class Border;
    class BorderFile;
    class Surface;
    
    class SelectionItemBorderSurface : public SelectionItem {
        
    public:
        SelectionItemBorderSurface();
        
        virtual ~SelectionItemBorderSurface();
        
        virtual bool isValid() const;
        
        Surface* getSurface();
        
        const Surface* getSurface() const;

        Border* getBorder();
        
        const Border* getBorder() const;
        
        void setBorder(Border* border);
        
        BorderFile* getBorderFile();
        
        const BorderFile* getBorderFile() const;
        
        void setBorderFile(BorderFile* borderFile);
        
        void setSurface(Surface* surface);
        
        int32_t getBorderIndex() const;
        
        void setBorderIndex(const int32_t borderIndex);
        
        int32_t getBorderPointIndex() const;
        
        void setBorderPointIndex(const int32_t borderPointIndex);
        
        void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemBorderSurface(const SelectionItemBorderSurface&);

        SelectionItemBorderSurface& operator=(const SelectionItemBorderSurface&);
        
        Border* border;
        BorderFile* borderFile;
        Surface* surface; 
        int32_t borderIndex;
        int32_t borderPointIndex;
    };
    
#ifdef __SELECTION_ITEM_BORDER_SURFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_BORDER_SURFACE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_BORDER_SURFACE__H_
