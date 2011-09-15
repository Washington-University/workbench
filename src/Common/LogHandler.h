#ifndef __LOG_HANDLER__H_
#define __LOG_HANDLER__H_

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


#include "CaretObject.h"

namespace caret {

    class LogRecord;
    
    /**
     * \brief Processes log record sent by a Logger.
     *
     * A Handler takes log record messages and 
     * does something with them.  This is an abstract
     * class and must be subclassed.
     *
     * This class emulates Java's java.util.logging.Handler.
     */
    class LogHandler : public CaretObject {
        
    public:
        /// close the handler and free resources
        virtual void close() = 0;
        
        /// flush any buffered output
        virtual void flush() = 0;
        
        /// Publish a log record
        virtual void publish(const LogRecord& logRecord) = 0;
        
        virtual ~LogHandler();
        
    protected:
        LogHandler();
        
    private:
        LogHandler(const LogHandler&);

        LogHandler& operator=(const LogHandler&);
        
    public:
        virtual AString toString() const;
        
    private:
    };
    
#ifdef __LOG_HANDLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LOG_HANDLER_DECLARE__

} // namespace
#endif  //__LOG_HANDLER__H_
