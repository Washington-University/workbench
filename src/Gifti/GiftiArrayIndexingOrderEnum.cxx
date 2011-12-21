/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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


#define __GIFTIARRAYINDEXINGORDER_DECLARE__
#include "GiftiArrayIndexingOrderEnum.h"
#undef __GIFTIARRAYINDEXINGORDER_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
GiftiArrayIndexingOrderEnum::GiftiArrayIndexingOrderEnum(
                   const Enum e,
                   const AString& name,
                   const AString& giftiName)
{
    this->e = e;
    this->name = name;
    this->giftiName = giftiName;
}

/**
 * Destructor.
 */
GiftiArrayIndexingOrderEnum::~GiftiArrayIndexingOrderEnum()
{
}

void
GiftiArrayIndexingOrderEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(GiftiArrayIndexingOrderEnum(COLUMN_MAJOR_ORDER, "COLUMN_MAJOR_ORDER", "ColumnMajorOrder"));
    enumData.push_back(GiftiArrayIndexingOrderEnum(ROW_MAJOR_ORDER, "ROW_MAJOR_ORDER", "RowMajorOrder"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GiftiArrayIndexingOrderEnum*
GiftiArrayIndexingOrderEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GiftiArrayIndexingOrderEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
GiftiArrayIndexingOrderEnum::toName(Enum e) {
    initialize();
    
    const GiftiArrayIndexingOrderEnum* gaio = findData(e);
    return gaio->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
GiftiArrayIndexingOrderEnum::Enum 
GiftiArrayIndexingOrderEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ROW_MAJOR_ORDER;
    
    for (std::vector<GiftiArrayIndexingOrderEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiArrayIndexingOrderEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("name \"" + s + " \"failed to match enumerated value for type GiftiArrayIndexingOrderEnum"));
    }
    return e;
}

/**
 * Get a string representation of the enumerated type.
 * @param e 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
GiftiArrayIndexingOrderEnum::toGiftiName(Enum e) {
    initialize();
    
    const GiftiArrayIndexingOrderEnum* gaio = findData(e);
    return gaio->giftiName;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
GiftiArrayIndexingOrderEnum::Enum 
GiftiArrayIndexingOrderEnum::fromGiftiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ROW_MAJOR_ORDER;
    
    for (std::vector<GiftiArrayIndexingOrderEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiArrayIndexingOrderEnum& d = *iter;
        if (d.giftiName == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("giftiName \"" + s + " \"failed to match enumerated value for type GiftiArrayIndexingOrderEnum"));
    }
    return e;
}
