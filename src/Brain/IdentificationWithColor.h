#ifndef __IDENTIFICATION_WITH_COLOR__H_
#define __IDENTIFICATION_WITH_COLOR__H_

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

namespace caret {

    class IdentificationWithColor : public CaretObject {
        
    public:
        IdentificationWithColor();
        
        virtual ~IdentificationWithColor();
        
        void addItem(int8_t rgbOut[3],
                     const int32_t index1,
                     const int32_t index2 = -1,
                     const int32_t index3 = -1);
        
        void getItem(const int8_t rgb[3],
                     int32_t* index1Out,
                     int32_t* index2Out = NULL,
                     int32_t* index3Out = NULL) const;

        void reset(const int32_t estimatedNumberOfItems = -1);
        
        
    private:
        IdentificationWithColor(const IdentificationWithColor&);

        IdentificationWithColor& operator=(const IdentificationWithColor&);
        
        static void encodeIntegerIntoRGB(const int32_t integerValue,
                          int8_t rgbOut[3]);
        
        static int32_t decodeIntegerFromRGB(const int8_t rgb[3]);
                                 
                          
    public:
        virtual AString toString() const;
        
    private:
        class Item {
        public:
            int8_t rgb[3];
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
