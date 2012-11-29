#ifndef __SELECTION_ITEM_FOCUS_VOLUME__H_
#define __SELECTION_ITEM_FOCUS_VOLUME__H_

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


#include "SelectionItem.h"

namespace caret {

    class Focus;
    class FociFile;
    class VolumeFile;
    
    class SelectionItemFocusVolume : public SelectionItem {
        
    public:
        SelectionItemFocusVolume();
        
        virtual ~SelectionItemFocusVolume();
        
        virtual bool isValid() const;
        
        VolumeFile* getVolumeFile();
        
        const VolumeFile* getVolumeFile() const;

        Focus* getFocus();
        
        const Focus* getFocus() const;
        
        void setFocus(Focus* focus);
        
        FociFile* getFociFile();
        
        const FociFile* getFociFile() const;
        
        void setFociFile(FociFile* fociFile);
        
        void setVolumeFile(VolumeFile* volumeFile);
        
        int32_t getFocusIndex() const;
        
        void setFocusIndex(const int32_t focusIndex);
        
        int32_t getFocusProjectionIndex() const;
        
        void setFocusProjectionIndex(const int32_t focusIndex);
        
        void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemFocusVolume(const SelectionItemFocusVolume&);

        SelectionItemFocusVolume& operator=(const SelectionItemFocusVolume&);
        
        Focus* focus;
        FociFile* fociFile;
        VolumeFile* volumeFile;
        int32_t focusIndex;
        int32_t focusProjectionIndex;
    };
    
#ifdef __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_FOCUS_VOLUME__H_
