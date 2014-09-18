#ifndef __LOG_LEVEL_ENUM__H_
#define __LOG_LEVEL_ENUM__H_

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

    /**
     * \brief Log level.
     *
     * The log level defines a standard logging levels that are used to 
     * control logging output.  The levels are ordered and enabling 
     * logging at a given level enables logging at all higher levels.
     * This class emulates the Java class java.util.logging.Level.
     */
class LogLevelEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** serious failure */
        SEVERE,
        /** potential problem */
        WARNING,
        /** informational messages */
        INFO,
        /** configuration messages, versions of libraries etc. */
        CONFIG,
        /** Detailed information */
        FINE,
        /** More detailed information */
        FINER,
        /** Very detailed information, all Events are logged at this level */
        FINEST,
        /** Log all records */
        ALL,
        /** */
        OFF
    };


    ~LogLevelEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static AString toHintedName(Enum enumValue);
    
    static Enum fromHintedName(const AString& hintedName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

private:
    LogLevelEnum(const Enum enumValue, 
                 const int32_t integerCode, 
                 const AString& name,
                 const AString& guiName,
                 const AString& hintedName);

    static const LogLevelEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<LogLevelEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
    
    /** A name that emphasizes important levels with all-caps */
    AString hintedName;
};

#ifdef __LOG_LEVEL_ENUM_DECLARE__
std::vector<LogLevelEnum> LogLevelEnum::enumData;
bool LogLevelEnum::initializedFlag = false;
#endif // __LOG_LEVEL_ENUM_DECLARE__

} // namespace
#endif  //__LOG_LEVEL_ENUM__H_
