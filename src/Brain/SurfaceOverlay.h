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

#include "Overlay.h"

#include "SurfaceOverlayDataTypeEnum.h"

namespace caret {

    class BrowserTabContent;
    class GiftiTypeFile;
    
    /// Contains settings for an overlay
    class SurfaceOverlay : public Overlay {
        
    public:
        SurfaceOverlay();
        
        virtual ~SurfaceOverlay();
        
        virtual AString toString() const;
        
        void getSelectionData(BrowserTabContent* browserTabContent,
                              SurfaceOverlayDataTypeEnum::Enum& selectedDataTypeOut,
                              AString& selectedColumnNameOut);
        
        void getSelectionData(BrowserTabContent* browserTabContent,
                              std::vector<GiftiTypeFile*>& dataFilesOut,
                              GiftiTypeFile* &selectedFileOut,
                              AString& selectedColumnNameOut,
                              int32_t& selectedColumnIndexOut);
        
        void setSelectionData(GiftiTypeFile* selectedDataFile,
                              const int32_t selectedColumnIndex);

    protected:
        virtual void copyOverlayData(const Overlay* overlay);
        
    private:
        SurfaceOverlay(const SurfaceOverlay&);

        SurfaceOverlay& operator=(const SurfaceOverlay&);
        
        /** available files */
        std::vector<GiftiTypeFile*> dataFiles;
        
        /* selected data file */
        GiftiTypeFile* selectedDataFile;
        
        /* selected data file column */
        AString selectedColumnName;
    };
    
#ifdef __SURFACE_OVERLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_OVERLAY_DECLARE__

} // namespace
#endif  //__SURFACE_OVERLAY__H_
