#ifndef __OVERLAY__H_
#define __OVERLAY__H_

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
#include "DataFileTypeEnum.h"

namespace caret {
    class BrowserTabContent;
    class CaretMappableDataFile;
    
    class Overlay : public CaretObject {
        
    public:
        Overlay();
        
        virtual ~Overlay();
        
        float getOpacity() const;
        
        void setOpacity(const float opacity);
        
        AString getName() const;
        
        void setOverlayNumber(const int32_t overlayIndex);
        
        virtual AString toString() const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        void copyData(const Overlay* overlay);
        
        void swapData(Overlay* overlay);
        
        void getSelectionData(BrowserTabContent* browserTabContent,
                              DataFileTypeEnum::Enum& mapFileTypeOut,
                              AString& selectedMapNameOut);
        
        void getSelectionData(BrowserTabContent* browserTabContent,
                              std::vector<CaretMappableDataFile*>& mapFilesOut,
                              CaretMappableDataFile* &selectedMapFileOut,
                              AString& selectedMapNameOut,
                              int32_t& selectedMapIndexOut);
        
        void getSelectionData(BrowserTabContent* browserTabContent,
                              CaretMappableDataFile* &selectedMapFileOut,
                              int32_t& selectedMapIndexOut);
        
        void setSelectionData(CaretMappableDataFile* selectedMapFile,
                              const int32_t selectedMapIndex);
        
        bool isPaletteDisplayEnabled() const;
        
        void setPaletteDisplayEnabled(const bool enabled);
        
    private:
        Overlay(const Overlay&);

        Overlay& operator=(const Overlay&);
        
        /** Name of overlay (DO NOT COPY)*/
        AString name;
        
        /** Index of this overlay (DO NOT COPY)*/
        int32_t overlayIndex;
        
        /** opacity for overlay */
        float opacity;
        
        /** enabled status */
        bool enabled;
        
        /** available mappable files */
        std::vector<CaretMappableDataFile*> mapFiles;
        
        /* selected mappable file */
        CaretMappableDataFile* selectedMapFile;
        
        /* selected data file map */
        AString selectedMapName;
        
        /* Display palette in graphics window */
        bool paletteDisplayedFlag;
    };
    
#ifdef __OVERLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OVERLAY_DECLARE__

} // namespace
#endif  //__OVERLAY__H_
