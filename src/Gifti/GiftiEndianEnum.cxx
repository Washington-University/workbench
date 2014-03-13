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


#define __GIFTIENDIAN_DECLARE__
#include "GiftiEndianEnum.h"
#undef __GIFTIENDIAN_DECLARE__

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
GiftiEndianEnum::GiftiEndianEnum(
                   const Enum e,
                   const int32_t integerCode,
                   const AString& name,
                   const AString& giftiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->giftiName = giftiName;
}

/**
 * Destructor.
 */
GiftiEndianEnum::~GiftiEndianEnum()
{
}

void
GiftiEndianEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(GiftiEndianEnum(ENDIAN_BIG, 0, "ENDIAN_BIG", "BigEndian"));
    enumData.push_back(GiftiEndianEnum(ENDIAN_LITTLE, 1, "ENDIAN_LITTLE", "LittleEndian"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GiftiEndianEnum*
GiftiEndianEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GiftiEndianEnum* d = &enumData[i];
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
GiftiEndianEnum::toName(Enum e) {
    initialize();
    
    const GiftiEndianEnum* gaio = findData(e);
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
GiftiEndianEnum::Enum 
GiftiEndianEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ENDIAN_LITTLE;
    
    for (std::vector<GiftiEndianEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiEndianEnum& d = *iter;
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
        CaretAssertMessage(0, AString("name \"" + s + " \"failed to match enumerated value for type GiftiEndianEnum"));
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
GiftiEndianEnum::toGiftiName(Enum e) {
    initialize();
    
    const GiftiEndianEnum* gaio = findData(e);
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
GiftiEndianEnum::Enum 
GiftiEndianEnum::fromGiftiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ENDIAN_LITTLE;
    
    for (std::vector<GiftiEndianEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiEndianEnum& d = *iter;
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
        CaretAssertMessage(0, AString("giftiName \"" + s + " \"failed to match enumerated value for type GiftiEndianEnum"));
    }
    return e;
}
