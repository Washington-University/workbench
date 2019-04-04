#ifndef __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_H__
#define __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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
    class GraphicsPrimitive;

    class VolumeSurfaceOutlineModelCacheValue : public CaretObject {
        
    public:
        VolumeSurfaceOutlineModelCacheValue();
        
        virtual ~VolumeSurfaceOutlineModelCacheValue();
        
        VolumeSurfaceOutlineModelCacheValue(const VolumeSurfaceOutlineModelCacheValue&) = delete;

        VolumeSurfaceOutlineModelCacheValue& operator=(const VolumeSurfaceOutlineModelCacheValue&) = delete;
        
        void setGraphicsPrimitive(const std::vector<GraphicsPrimitive*>& primitives);
        
        std::vector<GraphicsPrimitive*> getGraphicsPrimitives() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void deletePrimitives();
        
        std::vector<GraphicsPrimitive*> m_contourLinePrimitives;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_H__
