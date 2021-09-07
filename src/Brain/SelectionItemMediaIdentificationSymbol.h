#ifndef __SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_H__
#define __SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

    class SelectionItemMediaIdentificationSymbol : public SelectionItem {
        
    public:
        SelectionItemMediaIdentificationSymbol();
        
        virtual ~SelectionItemMediaIdentificationSymbol();
        
        SelectionItemMediaIdentificationSymbol(const SelectionItemMediaIdentificationSymbol& obj);

        SelectionItemMediaIdentificationSymbol& operator=(const SelectionItemMediaIdentificationSymbol& obj);
        
        void getPixelXYZ(float xyzOut[3]) const;
        
        void setPixelXYZ(const float xyz[3]);
        
        virtual bool isValid() const;
        
        virtual void reset();
        
        virtual AString toString() const;
        

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSelectionItemMediaIdentificationSymbol(const SelectionItemMediaIdentificationSymbol& obj);

        void resetPrivate();
        
        float m_pixelXYZ[3];
        
        bool m_pixelValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_MEDIA_IDENTIFICATION_SYMBOL_H__
