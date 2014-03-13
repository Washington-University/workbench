#ifndef __LOG_RECORD__H_
#define __LOG_RECORD__H_

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


#include "CaretObject.h"
#include "LogLevelEnum.h"

namespace caret {

    
    /**
     * \brief  Log record.
     *
     * A record for a single logging message
     * This class emulates the Java calss java.util.logging.
     */
    class LogRecord : public CaretObject {
        
    public:
        LogRecord(const LogLevelEnum::Enum level,
                  const AString& methodName,
                  const AString& filename,
                  const int32_t lineNumber,
                  const AString& text);
        
        virtual ~LogRecord();
        
        LogLevelEnum::Enum getLevel() const { return level; }
        
        /**
         * @return Text message.
         */
        AString getText() const { return text; }
        
        /**
         * @return Method name that logged message.
         */
        AString getMethodName() const { return methodName; }
        
        /**
         * @return Filename in which message was logged.
         */
        AString getFilename() const { return filename; }
        
        /**
         * @return Line number at which message was logged.
         */
        int32_t getLineNumber() const { return lineNumber; }
        
    private:
        LogRecord(const LogRecord&);

        LogRecord& operator=(const LogRecord&);
        
    public:
        virtual AString toString() const;
        
    private:
        LogLevelEnum::Enum level;
        
        AString text;
        
        AString methodName;
        
        AString filename;
        
        int32_t lineNumber;
    };
    
#ifdef __LOG_RECORD_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LOG_RECORD_DECLARE__

} // namespace
#endif  //__LOG_RECORD__H_
