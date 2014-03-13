#ifndef __LOG_HANDLER_STANDARD_ERROR__H_
#define __LOG_HANDLER_STANDARD_ERROR__H_

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


#include "LogHandler.h"

namespace caret {

    class LogRecord;
    
    /**
     * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
     *
     * <REPLACE-WITH-THOROUGH DESCRIPTION>
     */
    class LogHandlerStandardError : public LogHandler {
        
    public:
        LogHandlerStandardError();
        
        virtual ~LogHandlerStandardError();
        
        virtual void close();
        
        virtual void flush();
        
        virtual void publish(const LogRecord& logRecord);
        
    private:
        LogHandlerStandardError(const LogHandlerStandardError&);

        LogHandlerStandardError& operator=(const LogHandlerStandardError&);
        
    public:
        virtual AString toString() const;
        
    private:
    };
    
#ifdef __LOG_HANDLER_STANDARD_ERROR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LOG_HANDLER_STANDARD_ERROR_DECLARE__

} // namespace
#endif  //__LOG_HANDLER_STANDARD_ERROR__H_
