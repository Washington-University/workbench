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
#include "GiftiArrayIndexingOrder.h"
#undef __GIFTIARRAYINDEXINGORDER_DECLARE__


using namespace caret;

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
GiftiArrayIndexingOrder::GiftiArrayIndexingOrder(
                   const Enum e,
                   const std::string& name,
                   const std::string& giftiName)
{
    this->e = e;
    this->name = name;
    this->giftiName = giftiName;
}

/**
 * Destructor.
 */
GiftiArrayIndexingOrder::~GiftiArrayIndexingOrder()
{
}

void
GiftiArrayIndexingOrder::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(GiftiArrayIndexingOrder(COLUMN_MAJOR_ORDER, "COLUMN_MAJOR_ORDER", "ColumnMajorOrder"));
    enumData.push_back(GiftiArrayIndexingOrder(ROW_MAJOR_ORDER, "ROW_MAJOR_ORDER", "RowMajorOrder"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GiftiArrayIndexingOrder*
GiftiArrayIndexingOrder::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GiftiArrayIndexingOrder* d = &enumData[i];
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
std::string 
GiftiArrayIndexingOrder::toName(Enum e) {
    initialize();
    
    const GiftiArrayIndexingOrder* gaio = findData(e);
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
GiftiArrayIndexingOrder::Enum 
GiftiArrayIndexingOrder::fromName(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<GiftiArrayIndexingOrder>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiArrayIndexingOrder& d = *iter;
        if (d.name == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
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
std::string 
GiftiArrayIndexingOrder::toGiftiName(Enum e) {
    initialize();
    
    const GiftiArrayIndexingOrder* gaio = findData(e);
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
GiftiArrayIndexingOrder::Enum 
GiftiArrayIndexingOrder::fromGiftiName(const std::string& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<GiftiArrayIndexingOrder>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiArrayIndexingOrder& d = *iter;
        if (d.giftiName == s) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}
