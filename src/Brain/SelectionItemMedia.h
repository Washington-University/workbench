#ifndef __SELECTION_ITEM_MEDIA_H_
#define __SELECTION_ITEM_MEDIA_H_

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

#include <array>

#include "PixelIndex.h"
#include "PixelLogicalIndex.h"
#include "SelectionItem.h"

namespace caret {

    class MediaFile;
    
    class SelectionItemMedia : public SelectionItem {
        
    public:
        SelectionItemMedia();
        
        virtual ~SelectionItemMedia();
        
        virtual bool isValid() const;
        
        MediaFile* getMediaFile();
        
        const MediaFile* getMediaFile() const;

        void setMediaFile(MediaFile* mediaFile);
        
        PixelLogicalIndex getPixelLogicalIndex() const;
        
        void setPixelLogicalIndex(const PixelLogicalIndex& pixelLogicalIndex);
        
        int32_t getOverlayIndex() const;
        
        int32_t getTabIndex() const;
        
        bool getStereotaxicXYZ(std::array<float, 3>& stereotaxicXYZOut);
        
        void setOverlayIndex(const int32_t tabIndex);
        
        void setTabIndex(const int32_t tabIndex);
        
        bool getPixelRGBA(uint8_t pixelRGBAOut[4]) const;
        
        void setPixelRGBA(const uint8_t pixelRGBA[4]);

        bool isIncludePixelRGBA() const;
        
        void setIncludePixelRGBA(const bool status);
        
        virtual void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemMedia(const SelectionItemMedia&);

        SelectionItemMedia& operator=(const SelectionItemMedia&);

        void resetPrivate();
        
        MediaFile* m_mediaFile;
        
        PixelLogicalIndex m_pixelLogicalIndex;
        
        int32_t m_tabIndex = -1;
        
        int32_t m_overlayIndex = -1;
        
        uint8_t m_pixelRGBA[4];
        
        bool m_pixelRGBAValidFlag = false;
        
        bool m_includePixelRGBAFlag = false;
    };
    
#ifdef __SELECTION_ITEM_MEDIA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_MEDIA_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_MEDIA_H_
