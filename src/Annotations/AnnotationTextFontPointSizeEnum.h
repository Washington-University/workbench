#ifndef __ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_H__
#define __ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

class AnnotationTextFontPointSizeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** */
        SIZE10,
        /** */
        SIZE12,
        /** */
        SIZE14,
        /** */
        SIZE16,
        /** */
        SIZE18,
        /** */
        SIZE20,
        /** */
        SIZE24,
        /** */
        SIZE28,
        /** */
        SIZE32,
        /** */
        SIZE36,
        /** */
        SIZE40,
        /** */
        SIZE44,
        /** */
        SIZE48,
        /** */
        SIZE54,
        /** */
        SIZE60,
        /** */
        SIZE66,
        /** */
        SIZE72
    };


    ~AnnotationTextFontPointSizeEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

    static int32_t toSizeNumeric(Enum enumValue);
    
    static int32_t getMinimumSizeNumeric();
    
private:
    AnnotationTextFontPointSizeEnum(const Enum enumValue, 
                           const AString& name,
                           const AString& guiName,
                           const int32_t sizeNumeric);

    static const AnnotationTextFontPointSizeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<AnnotationTextFontPointSizeEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    static int32_t minimumNumericSize;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
    
    /** Numerical size of font */
    int32_t sizeNumeric;
};

#ifdef __ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_DECLARE__
    std::vector<AnnotationTextFontPointSizeEnum> AnnotationTextFontPointSizeEnum::enumData;
    bool AnnotationTextFontPointSizeEnum::initializedFlag = false;
    int32_t AnnotationTextFontPointSizeEnum::integerCodeCounter = 0;
    int32_t AnnotationTextFontPointSizeEnum::minimumNumericSize = -1;
#endif // __ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_FONT_POINT_SIZE_ENUM_H__
