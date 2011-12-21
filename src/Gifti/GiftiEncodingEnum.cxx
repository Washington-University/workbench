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


#define __GIFTIENCODING_DECLARE__
#include "GiftiEncodingEnum.h"
#undef __GIFTIENCODING_DECLARE__

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
GiftiEncodingEnum::GiftiEncodingEnum(
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
GiftiEncodingEnum::~GiftiEncodingEnum()
{
}

void
GiftiEncodingEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(GiftiEncodingEnum(ASCII, -1, "ASCII", "ASCII"));
    enumData.push_back(GiftiEncodingEnum(BASE64_BINARY, -1, "BASE64_BINARY", "Base64Binary"));
    enumData.push_back(GiftiEncodingEnum(GZIP_BASE64_BINARY, -1, "GZIP_BASE64_BINARY", "GZipBase64Binary"));
    enumData.push_back(GiftiEncodingEnum(EXTERNAL_FILE_BINARY, -1, "EXTERNAL_FILE_BINARY", "ExternalFileBinary"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GiftiEncodingEnum*
GiftiEncodingEnum::findData(const Enum e)
{
    initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GiftiEncodingEnum* d = &enumData[i];
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
GiftiEncodingEnum::toName(Enum e) {
    initialize();
    
    const GiftiEncodingEnum* gaio = findData(e);
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
GiftiEncodingEnum::Enum 
GiftiEncodingEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ASCII;
    
    for (std::vector<GiftiEncodingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiEncodingEnum& d = *iter;
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
        CaretAssertMessage(0, AString("name \"" + s + " \"failed to match enumerated value for type GiftiEncodingEnum"));
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
GiftiEncodingEnum::toGiftiName(Enum e) {
    initialize();
    
    const GiftiEncodingEnum* gaio = findData(e);
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
GiftiEncodingEnum::Enum 
GiftiEncodingEnum::fromGiftiName(const AString& s, bool* isValidOut)
{
    initialize();
    
    bool validFlag = false;
    Enum e = ASCII;
    
    for (std::vector<GiftiEncodingEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GiftiEncodingEnum& d = *iter;
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
        CaretAssertMessage(0, AString("giftiName \"" + s + " \"failed to match enumerated value for type GiftiEncodingEnum"));
    }
    return e;
}
