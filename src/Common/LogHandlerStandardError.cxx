
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

#include <iostream>

#define __LOG_HANDLER_STANDARD_ERROR_DECLARE__
#include "LogHandlerStandardError.h"
#undef __LOG_HANDLER_STANDARD_ERROR_DECLARE__

#include "LogRecord.h"

using namespace caret;


/**
 * Constructor.
 */
LogHandlerStandardError::LogHandlerStandardError()
: LogHandler()
{
    
}

/**
 * Destructor.
 */
LogHandlerStandardError::~LogHandlerStandardError()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
LogHandlerStandardError::toString() const
{
    return "LogHandlerStandardError";
}

/**
 * close the handler and free resources.
 */
void 
LogHandlerStandardError::close()
{
    // nothing to close
}

/**
 * Flush any buffered output.
 */
void 
LogHandlerStandardError::flush()
{
    std::cerr.flush();
}

/**
 * Publish a log record.
 *
 * @param logRecord
 *    Logging record that is sent to standard error.
 */
void 
LogHandlerStandardError::publish(const LogRecord& logRecord)
{
    std::cerr << std::endl;
    std::cerr << "Level: " << LogLevelEnum::toName(logRecord.getLevel()) << std::endl;
    if (logRecord.getMethodName().isEmpty() == false) {
        std::cerr << "Method: " << logRecord.getMethodName() << std::endl;        
    }
    std::cerr << "File:   " << logRecord.getFilename() << std::endl;
    std::cerr << "Line:   " << logRecord.getLineNumber() << std::endl;
    std::cerr << "Text:   " << logRecord.getText() << std::endl;
    std::cerr << std::endl;
}


