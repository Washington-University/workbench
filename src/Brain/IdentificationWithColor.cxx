
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

#define __IDENTIFICATION_WITH_COLOR_DECLARE__
#include "IdentificationWithColor.h"
#undef __IDENTIFICATION_WITH_COLOR_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class IdentificationWithColor 
 * \brief Assists with identification of items using colors.
 *
 * To perform identification of items, an identifier
 * is encoded as RGB colors.  The color present at
 * the location of identification is then decoded
 * to recover the identifier.
 *
 * Since there can be many items, this class
 * should be used once for each type of item.
 */

/**
 * Constructor.
 */
IdentificationWithColor::IdentificationWithColor()
: CaretObject()
{
    this->itemCounter = 0;
    this->items.reserve(250000);
}

/**
 * Destructor.
 */
IdentificationWithColor::~IdentificationWithColor()
{
    
}

/**
 * Gets an RGB color that is associated with the
 * specified indices.
 * @param index1
 *    First index of item.
 * @param index2
 *    Optional second index of item.
 * @param index3
 *    Optional third index of item.
 */
void 
IdentificationWithColor::addItem(uint8_t rgbOut[3],
                                 const IdentificationItemDataTypeEnum::Enum dataType,
             const int32_t index1,
             const int32_t index2,
             const int32_t index3)
{    
    if (this->itemCounter >= static_cast<int32_t>(this->items.size())) {
        this->items.push_back(Item());
    }
    IdentificationWithColor::encodeIntegerIntoRGB(this->itemCounter, 
                                                  rgbOut);
    
    Item& item = this->items[this->itemCounter];
    item.dataType = dataType;
    item.rgb[0] = rgbOut[0];
    item.rgb[1] = rgbOut[1];
    item.rgb[2] = rgbOut[2];
    item.index1 = index1;
    item.index2 = index2;
    item.index3 = index3;
    
    itemCounter++;
}

/**
 * Gets indices associated with an RGB color.
 * @param index1Out
 *    Set to first index of item.
 * @param index2
 *    Set to second index of item.  This paramter
 *    may be NULL;
 * @param index3
 *    Set to third index of item.  This paramter
 *    may be NULL;
 */
void 
IdentificationWithColor::getItem(const uint8_t rgb[3],
                                 const IdentificationItemDataTypeEnum::Enum dataType,
             int32_t* index1Out,
             int32_t* index2Out,
             int32_t* index3Out) const
{
    CaretAssert(index1Out);
    
    const int32_t integerValue = IdentificationWithColor::decodeIntegerFromRGB(rgb);
    
    *index1Out = -1;
    if (index2Out != NULL) {
        *index2Out = -1;
    }
    if (index3Out != NULL) {
        *index3Out = -1;
    }

    if ((integerValue < 0) ||
        (integerValue >= this->itemCounter)) {
        return;
    }
    
    const Item& item = this->items[integerValue];
    if (dataType == item.dataType) {
        *index1Out = item.index1;
        if (index2Out != NULL) {
            *index2Out = item.index2;
        }
        if (index3Out != NULL) {
            *index3Out = item.index3;
        }
    }
}

/**
 * Reset for a new round of identification.
 * @param estimatedNumberOfItems
 *   The estimated number of items.  Must be non-negative
 *   and can improve performance if greater than or equal
 *   to the number of items.
 */
void 
IdentificationWithColor::reset(const int32_t estimatedNumberOfItems)
{
    this->itemCounter = 0;
    if (estimatedNumberOfItems > static_cast<int32_t>(this->items.size())) {
        this->items.reserve(estimatedNumberOfItems);
    }
}

/**
 * Encode an integer as RGB values.
 * @param integerValue
 *    Integer value.
 * @param rgbOut
 *    Output RGB.
 */
void 
IdentificationWithColor::encodeIntegerIntoRGB(const int32_t integerValue,
                         uint8_t rgbOut[3])
{
    rgbOut[2] = (uint8_t)(integerValue         & 0xff);
    rgbOut[1] = (uint8_t)((integerValue >> 8)  & 0xff);
    rgbOut[0] = (uint8_t)((integerValue >> 16) & 0xff);    
}

/**
 * Decode an integer from RGB values.
 * @param rgb
 *    RGB value.
 * @return The integer value.
 */
int32_t 
IdentificationWithColor::decodeIntegerFromRGB(const uint8_t rgb[3])
{            
    int32_t r = rgb[0] & 0xff;
    int32_t g = rgb[1] & 0xff;
    int32_t b = rgb[2] & 0xff;
    int32_t colorValue = (r << 16) + (g << 8) + b;
    return colorValue;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationWithColor::toString() const
{
    return "IdentificationWithColor";
}
