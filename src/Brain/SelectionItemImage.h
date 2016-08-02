#ifndef __SELECTION_ITEM_IMAGE__H_
#define __SELECTION_ITEM_IMAGE__H_

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

    class ImageFile;
    
    class SelectionItemImage : public SelectionItem {
        
    public:
        SelectionItemImage();
        
        virtual ~SelectionItemImage();
        
        virtual bool isValid() const;
        
        ImageFile* getImageFile();
        
        const ImageFile* getImageFile() const;

        void setImageFile(ImageFile* imageFile);
        
        int32_t getPixelI() const;
        
        int32_t getPixelJ() const;
        
        void setPixelI(const int32_t i);
        
        void setPixelJ(const int32_t j);
        
        virtual void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemImage(const SelectionItemImage&);

        SelectionItemImage& operator=(const SelectionItemImage&);

        ImageFile* m_imageFile;
        
        int32_t m_pixelI;
        
        int32_t m_pixelJ;
    };
    
#ifdef __SELECTION_ITEM_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_IMAGE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_IMAGE__H_
