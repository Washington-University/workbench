#ifndef __SURFACE_MONTAGE_VIEWPORT_H__
#define __SURFACE_MONTAGE_VIEWPORT_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
