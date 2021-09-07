#ifndef __IDENTIFICATION_WITH_COLOR__H_
#define __IDENTIFICATION_WITH_COLOR__H_

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


#include "CaretObject.h"
#include "SelectionItemDataTypeEnum.h"

namespace caret {

    class IdentificationWithColor : public CaretObject {
        
    public:
        IdentificationWithColor();
        
        virtual ~IdentificationWithColor();
        
        void addItem(uint8_t rgbOut[4],
                     const SelectionItemDataTypeEnum::Enum dataType,
                     const int32_t index1,
                     const int32_t index2 = -1,
                     const int32_t index3 = -1);
        
        void getItem(const uint8_t rgb[4],
                     const SelectionItemDataTypeEnum::Enum dataType,
                     int32_t* index1Out,
                     int32_t* index2Out = NULL,
                     int32_t* index3Out = NULL) const;

        void getItemAnyType(const uint8_t rgb[4],
                            SelectionItemDataTypeEnum::Enum& dataTypeOut,
                            int32_t* index1Out,
                            int32_t* index2Out = NULL,
                            int32_t* index3Out = NULL) const;

        void reset(const int32_t estimatedNumberOfItems = -1);
        
        
    private:
        IdentificationWithColor(const IdentificationWithColor&);

        IdentificationWithColor& operator=(const IdentificationWithColor&);
        
        static void encodeIntegerIntoRGB(const int32_t integerValue,
                          uint8_t rgbOut[3]);
        
        static int32_t decodeIntegerFromRGB(const uint8_t rgb[3]);
                                 
                          
    public:
        virtual AString toString() const;
        
    private:
        class Item {
        public:
            SelectionItemDataTypeEnum::Enum dataType;
            uint8_t rgb[3];
            int32_t index1;
            int32_t index2;
            int32_t index3;
            
        };
        
        std::vector<Item> items;
        
        int32_t itemCounter;
    };
    
#ifdef __IDENTIFICATION_WITH_COLOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_WITH_COLOR_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_WITH_COLOR__H_
