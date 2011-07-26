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
#include <string>

namespace caret {

/**
 * GIFTI Endian Types.
 */
class GiftiEndian {

public:
    /**  GIFTI Endian Types.
 */
    enum Enum {
        /** Data is in Big Endian byte order   */
        ENDIAN_BIG,
        /** Data is in Little Endian byte order   */
        ENDIAN_LITTLE
    };


    ~GiftiEndian();

    Enum getEnum() const;

    int32_t getIntegerCode() const;

    std::string getName() const;

    std::string getGiftiName() const;
    
private:
    GiftiEndian(const Enum e, const int32_t integerCode, const std::string& name, const std::string& giftiName);

    static std::vector<GiftiEndian> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    std::string name;
    
    std::string giftiName;

public:
    static std::string toString(Enum e, bool* isValidOut);

    static Enum fromString(const std::string& s, bool* isValidOut);

    static const GiftiEndian* findData(const Enum e);

};

#ifdef __GIFTIENDIAN_DECLARE__
std::vector<GiftiEndian> GiftiEndian::enumData;
bool GiftiEndian::initializedFlag = false;
#endif // __GIFTIENDIAN_DECLARE__

} // namespace

#endif // __GIFTIENDIAN_H__
