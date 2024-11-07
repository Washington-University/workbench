#ifndef __ZARR_COMPRESSOR_TYPE_ENUM_H__
#define __ZARR_COMPRESSOR_TYPE_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

class ZarrCompressorTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** No Compressor */
        NO_COMPRESSOR,
        /** Blosc */
        BLOSC,
        /** Bytes */
        BYTES,
        /** Crc32c */
        CRC32C,
        /** Gzip */
        GZIP
    };


    ~ZarrCompressorTypeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toEncodingName(Enum enumValue);
    
    static Enum fromEncodingName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllEncodingNames(std::vector<AString>& allEncodingNames, const bool isSorted);

private:
    ZarrCompressorTypeEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& encodingName);

    static const ZarrCompressorTypeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<ZarrCompressorTypeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** The encoding name  */
    AString encodingName;
};

#ifdef __ZARR_COMPRESSOR_TYPE_ENUM_DECLARE__
std::vector<ZarrCompressorTypeEnum> ZarrCompressorTypeEnum::enumData;
bool ZarrCompressorTypeEnum::initializedFlag = false;
int32_t ZarrCompressorTypeEnum::integerCodeCounter = 0; 
#endif // __ZARR_COMPRESSOR_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__ZARR_COMPRESSOR_TYPE_ENUM_H__
