#ifndef __GIFTIENCODING_H__
#define __GIFTIENCODING_H__

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

#include <stdint.h>
#include <vector>
#include <AString.h>

namespace caret {

/**
 * GIFTI Encoding Types.
 */
class GiftiEncodingEnum {

public:
    /**  GIFTI Encoding Types.
 */
    enum Enum {
        /** The data is ASCII Text  */
        ASCII,
        /** THe data is binary data that is encoded as text using the Base64 Algorithm   */
        BASE64_BINARY,
        /** The data is binary encoded as Base64 and then compressed using the GZIP algorithm   */
        GZIP_BASE64_BINARY,
        /** The data is stored in a separate, uncompressed, binary data file   */
        EXTERNAL_FILE_BINARY
    };


    ~GiftiEncodingEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGiftiName(Enum e);
    
    static Enum fromGiftiName(const AString& s, bool* isValidOut);
    
private:
    GiftiEncodingEnum(const Enum e, const int32_t integerCode, const AString& name, const AString& giftiName);

    static const GiftiEncodingEnum* findData(const Enum e);

    static std::vector<GiftiEncodingEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;

    AString giftiName;

};

#ifdef __GIFTIENCODING_DECLARE__
std::vector<GiftiEncodingEnum> GiftiEncodingEnum::enumData;
bool GiftiEncodingEnum::initializedFlag = false;
#endif // __GIFTIENCODING_DECLARE__

} // namespace

#endif // __GIFTIENCODING_H__
