#ifndef __OVERLAY_SET__H_
#define __OVERLAY_SET__H_

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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "Overlay.h"

namespace caret {

    class SurfaceOverlaySet;
    
    class OverlaySet : public CaretObject {
        
    public:
        static SurfaceOverlaySet* newInstanceSurfaceOverlaySet();
        
        virtual ~OverlaySet();
        
        void addDisplayedOverlay();
        
        int32_t getNumberOfDisplayedOverlays() const;
        
        void removeDisplayedOverlay(const int32_t overlayIndex);
        
        void moveDisplayedOverlayUp(const int32_t overlayIndex);
        
        void moveDisplayedOverlayDown(const int32_t overlayIndex);

    protected:
        enum OverlaySetType {
            OVERLAY_SET_SURFACE,
            OVERLAY_SET_VOLUME
        };
        
        OverlaySet(const OverlaySetType overlaySetType);
        
        Overlay* getPrimaryOverlayGeneric();
        
        Overlay* getUnderlayGeneric();
        
        Overlay* getOverlayGeneric(const int32_t overlayNumber);
        
        const Overlay* getOverlayGeneric(const int32_t overlayNumber) const;
        
    public:
        virtual AString toString() const;
        
    private:
        OverlaySet(const OverlaySet&);
        
        OverlaySet& operator=(const OverlaySet&);
        
        OverlaySetType overlaySetType;
        
        Overlay* overlays;
        
        int32_t numberOfDisplayedOverlays;
    };
    
#ifdef __OVERLAY_SET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OVERLAY_SET_DECLARE__

} // namespace
#endif  //__OVERLAY_SET__H_
