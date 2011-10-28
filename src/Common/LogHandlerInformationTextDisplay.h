#ifndef __LOG_HANDLER_INFORMATION_TEXT_DISPLAY__H_
#define __LOG_HANDLER_INFORMATION_TEXT_DISPLAY__H_

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


#include "LogHandler.h"

namespace caret {

    class LogRecord;
    
    /**
     * \brief Logs messages to the text information GUI
     *
     * Logs message to the information text display in the
     * Browser window's Toolbox.
     */
    class LogHandlerInformationTextDisplay : public LogHandler {
        
    public:
        LogHandlerInformationTextDisplay();
        
        virtual ~LogHandlerInformationTextDisplay();
        
        virtual void close();
        
        virtual void flush();
        
        virtual void publish(const LogRecord& logRecord);
        
    private:
        LogHandlerInformationTextDisplay(const LogHandlerInformationTextDisplay&);

        LogHandlerInformationTextDisplay& operator=(const LogHandlerInformationTextDisplay&);
        
    public:
        virtual AString toString() const;
        
    private:
    };
    
#ifdef __LOG_HANDLER_INFORMATION_TEXT_DISPLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LOG_HANDLER_INFORMATION_TEXT_DISPLAY_DECLARE__

} // namespace
#endif  //__LOG_HANDLER_INFORMATION_TEXT_DISPLAY__H_
