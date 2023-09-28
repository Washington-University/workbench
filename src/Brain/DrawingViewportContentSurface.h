#ifndef __DRAWING_VIEWPORT_CONTENT_SURFACE__
#define __DRAWING_VIEWPORT_CONTENT_SURFACE__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

namespace caret {

    class DrawingViewportContentSurface : public CaretObject {
        
    public:
        DrawingViewportContentSurface();
        
        DrawingViewportContentSurface(const int32_t numberOfRows,
                                          const int32_t numberOfColumns,
                                          const int32_t rowIndex,
                                          const int32_t columnIndex);

        virtual ~DrawingViewportContentSurface();
        
        DrawingViewportContentSurface(const DrawingViewportContentSurface& obj);

        DrawingViewportContentSurface& operator=(const DrawingViewportContentSurface& obj);
        
        int32_t getNumberOfRows() const;

        int32_t getNumberOfColumns() const;
        
        int32_t getRowIndex() const;
        
        int32_t getColumnIndex() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperDrawingViewportContentSurface(const DrawingViewportContentSurface& obj);

        int32_t m_numberOfRows = -1;
        
        int32_t m_numberOfColumns = -1;
        
        int32_t m_rowIndex = -1;
        
        int32_t m_columnIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DRAWING_VIEWPORT_CONTENT_SURFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DRAWING_VIEWPORT_CONTENT_SURFACE_DECLARE__

} // namespace
#endif  //__DRAWING_VIEWPORT_CONTENT_SURFACE__
