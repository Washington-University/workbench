#ifndef __SURFACE_MONTAGE_VIEWPORT_H__
#define __SURFACE_MONTAGE_VIEWPORT_H__

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


#include "CaretObject.h"
#include "ProjectionViewTypeEnum.h"
#include "StructureEnum.h"

namespace caret {

    class Surface;
    class SurfaceMontageViewport : public CaretObject {
        
    public:
//        SurfaceMontageViewport();
        
        SurfaceMontageViewport(Surface* surface,
                               const ProjectionViewTypeEnum::Enum projectionViewType);
        
        virtual ~SurfaceMontageViewport();
        
        /**
         * @return The surface in the viewport.
         */
        Surface* getSurface() const { return m_surface; }
        
        ProjectionViewTypeEnum::Enum getProjectionViewType() const { return m_projectionViewType; }
        
        bool isInside(const int32_t x,
                      const int32_t y) const;
        
        /**
         * @return Row of this item (0 is top)
         */
        int32_t getRow() const;
        
        /**
         * @return Column of this item (0 is left)
         */
        int32_t getColumn() const;
        
        void setRowAndColumn(const int32_t row,
                             const int32_t column);
        
        /**
         * @return X-coordinate in viewport.
         */
        int32_t getX() const;
        
        /**
         * @return Y-Coordinate in viewport.
         */
        int32_t getY() const;
        
        /**
         * @return Width of viewport.
         */
        int32_t getWidth() const;
        
        /**
         * @return Height of viewport.
         */
        int32_t getHeight() const;
        
        void getViewport(int32_t viewportOut[4]) const;
        
        void setViewport(const int32_t viewport[4]);
        
        static void getNumberOfRowsAndColumns(const std::vector<SurfaceMontageViewport*>& montageViewports,
                                              int32_t& numberOfRowsOut,
                                              int32_t& numberOfColumnsOut);
        
        static void getNumberOfRowsAndColumns(const std::vector<const SurfaceMontageViewport*>& montageViewports,
                                              int32_t& numberOfRowsOut,
                                              int32_t& numberOfColumnsOut);
    private:
        int32_t m_row;
        
        int32_t m_column;
        
        int32_t m_viewport[4];
        
        Surface* m_surface;
        
        ProjectionViewTypeEnum::Enum m_projectionViewType;

        StructureEnum::Enum m_structure;
        
        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SURFACE_MONTAGE_VIEWPORT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_VIEWPORT_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_VIEWPORT_H__
