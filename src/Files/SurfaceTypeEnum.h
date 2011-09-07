#ifndef __SURFACE_TYPE_ENUM__H_
#define __SURFACE_TYPE_ENUM__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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
#include <QString>

namespace caret {

/**
 * Enumerated type for 
 */
class SurfaceTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** UNKNOWN */
        SURFACE_TYPE_UNKNOWN,
        /** Anatomical */
        SURFACE_TYPE_ANATOMICAL,
        /** Inflated */
        SURFACE_TYPE_INFLATED,
        /** Very Inflated */
        SURFACE_TYPE_VERY_INFLATED,
        /** Flat */
        SURFACE_TYPE_FLAT
    };


    ~SurfaceTypeEnum();

    static QString toName(Enum e);
    
    static Enum fromName(const QString& s, bool* isValidOut);
    
    static QString toGuiName(Enum e);
    
    static Enum fromGuiName(const QString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    SurfaceTypeEnum(const Enum e, 
                 const int32_t integerCode, 
                 const QString& name,
                 const QString& guiName);

    static const SurfaceTypeEnum* findData(const Enum e);

    static std::vector<SurfaceTypeEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    QString name;
    
    QString guiName;
};

#ifdef __SURFACE_TYPE_ENUM_DECLARE__
std::vector<SurfaceTypeEnum> SurfaceTypeEnum::enumData;
bool SurfaceTypeEnum::initializedFlag = false;
#endif // __SURFACE_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__SURFACE_TYPE_ENUM__H_
