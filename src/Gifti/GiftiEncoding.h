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
#include <string>

namespace caret {

/**
 * GIFTI Encoding Types.
 */
class GiftiEncoding {

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


    ~GiftiEncoding();

    static std::string toName(Enum e);
    
    static Enum fromName(const std::string& s, bool* isValidOut);
    
    static std::string toGiftiName(Enum e);
    
    static Enum fromGiftiName(const std::string& s, bool* isValidOut);
    
private:
    GiftiEncoding(const Enum e, const int32_t integerCode, const std::string& name, const std::string& giftiName);

    static const GiftiEncoding* findData(const Enum e);

    static std::vector<GiftiEncoding> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    std::string name;

    std::string giftiName;

};

#ifdef __GIFTIENCODING_DECLARE__
std::vector<GiftiEncoding> GiftiEncoding::enumData;
bool GiftiEncoding::initializedFlag = false;
#endif // __GIFTIENCODING_DECLARE__

} // namespace

#endif // __GIFTIENCODING_H__
