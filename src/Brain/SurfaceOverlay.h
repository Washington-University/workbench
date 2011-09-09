#ifndef __SURFACE_OVERLAY__H_
#define __SURFACE_OVERLAY__H_

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

#include "SurfaceOverlayDataTypeEnum.h"

namespace caret {

    
    /// Contains settings for a single surface overlay
    class SurfaceOverlay : public CaretObject {
        
    public:
        SurfaceOverlay();
        
        virtual ~SurfaceOverlay();
        
        SurfaceOverlayDataTypeEnum::Enum getSelectedType() const;
        
        void setSelectedType(SurfaceOverlayDataTypeEnum::Enum selectedType);
        
        float getOpacity() const;
        
        void setOpacity(const float opacity);
        
        AString getName() const;
        
        void setOverlayNumber(const int32_t overlayIndex);
        
    private:
        SurfaceOverlay(const SurfaceOverlay&);

        SurfaceOverlay& operator=(const SurfaceOverlay&);
        
        
    public:
        virtual AString toString() const;
        
        /** Name of overlay */
        AString name;
        
        /** Type of selected overlay */
        SurfaceOverlayDataTypeEnum::Enum selectedType;
        
        /** Index of this overlay */
        int32_t overlayIndex;
        
        /** opacity for overlay */
        float opacity;
        
        /** UUID of selected data */
        AString selectedDataUUID;
        
    private:
    };
    
#ifdef __SURFACE_OVERLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_OVERLAY_DECLARE__

} // namespace
#endif  //__SURFACE_OVERLAY__H_
