#ifndef __SELECTION_ITEM_FOCUS_VOLUME__H_
#define __SELECTION_ITEM_FOCUS_VOLUME__H_

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

    class Focus;
    class FociFile;
    class VolumeMappableInterface;
    
    class SelectionItemFocusVolume : public SelectionItem {
        
    public:
        SelectionItemFocusVolume();
        
        virtual ~SelectionItemFocusVolume();
        
        virtual bool isValid() const;
        
        VolumeMappableInterface* getVolumeFile();
        
        const VolumeMappableInterface* getVolumeFile() const;

        Focus* getFocus();
        
        const Focus* getFocus() const;
        
        void setFocus(Focus* focus);
        
        FociFile* getFociFile();
        
        const FociFile* getFociFile() const;
        
        void setFociFile(FociFile* fociFile);
        
        void setVolumeFile(VolumeMappableInterface* volumeFile);
        
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
        VolumeMappableInterface* volumeFile;
        int32_t focusIndex;
        int32_t focusProjectionIndex;
    };
    
#ifdef __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_FOCUS_VOLUME_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_FOCUS_VOLUME__H_
