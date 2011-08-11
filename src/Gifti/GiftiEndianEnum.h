#ifndef __GIFTIENDIAN_H__
#define __GIFTIENDIAN_H__

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


#include "GiftiException.h"

#include <stdint.h>
#include <vector>
#include <AString.h>

namespace caret {

/**
 * GIFTI Endian Types.
 */
class GiftiEndianEnum {

public:
    /**  GIFTI Endian Types.
 */
    enum Enum {
        /** Data is in Big Endian byte order   */
        ENDIAN_BIG,
        /** Data is in Little Endian byte order   */
        ENDIAN_LITTLE
    };


    ~GiftiEndianEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGiftiName(Enum e);
    
    static Enum fromGiftiName(const AString& s, bool* isValidOut);

private:
    GiftiEndianEnum(const Enum e, const int32_t integerCode, const AString& name, const AString& giftiName);

    static const GiftiEndianEnum* findData(const Enum e);

    static std::vector<GiftiEndianEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;
    
    AString giftiName;


};

#ifdef __GIFTIENDIAN_DECLARE__
std::vector<GiftiEndianEnum> GiftiEndianEnum::enumData;
bool GiftiEndianEnum::initializedFlag = false;
#endif // __GIFTIENDIAN_DECLARE__

} // namespace

#endif // __GIFTIENDIAN_H__
