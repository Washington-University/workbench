#ifndef __SELECTION_ITEM_CZI_IMAGE__H_
#define __SELECTION_ITEM_CZI_IMAGE__H_

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

#include "PixelIndex.h"
#include "SelectionItem.h"

namespace caret {

    class CziImageFile;
    
    class SelectionItemCziImage : public SelectionItem {
        
    public:
        SelectionItemCziImage();
        
        virtual ~SelectionItemCziImage();
        
        virtual bool isValid() const;
        
        CziImageFile* getCziImageFile();
        
        const CziImageFile* getCziImageFile() const;

        void setCziImageFile(CziImageFile* imageFile);
        
        PixelIndex getPixelIndex() const;
        
        void setPixelIndex(const PixelIndex& pixelIndex);
        
        int32_t getTabIndex() const;
        
        void setTabIndex(const int32_t tabIndex);
        
        int32_t getPixelI() const;
        
        int32_t getPixelJ() const;
        
        void setPixelI(const int32_t i);
        
        void setPixelJ(const int32_t j);
        
        void getPixelRGBA(uint8_t pixelRGBAOut[4]) const;
        
        void setPixelRGBA(const uint8_t pixelRGBA[4]);
        
        virtual void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemCziImage(const SelectionItemCziImage&);

        SelectionItemCziImage& operator=(const SelectionItemCziImage&);

        CziImageFile* m_imageFile;
        
        PixelIndex m_pixelIndex;
        
        int32_t m_tabIndex = -1;
        
        int32_t m_pixelI;
        
        int32_t m_pixelJ;
        
        uint8_t m_pixelRGBA[4];
        
    };
    
#ifdef __SELECTION_ITEM_CZI_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CZI_IMAGE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_CZI_IMAGE__H_
