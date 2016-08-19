
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

#include <iostream>

#define __LOG_HANDLER_STANDARD_ERROR_DECLARE__
#include "LogHandlerStandardError.h"
#undef __LOG_HANDLER_STANDARD_ERROR_DECLARE__

#include "FileInformation.h"
#include "LogRecord.h"

using namespace caret;
using namespace std;

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
    cerr << endl;
    cerr << LogLevelEnum::toHintedName(logRecord.getLevel()) << ": " << logRecord.getText().toLocal8Bit().constData() << endl;
#ifndef NDEBUG
    if (logRecord.getMethodName().isEmpty() == false)
    {//in debug, also give method name and source location
        cerr << "Method: " << logRecord.getMethodName() << endl;
    }
    cerr << "Location: " << logRecord.getFilename() << ":" << AString::number(logRecord.getLineNumber()) << endl;
#else
    if (LogLevelEnum::toIntegerCode(logRecord.getLevel()) >= LogLevelEnum::toIntegerCode(LogLevelEnum::SEVERE))//in release, give method and simplified source location only for severe or worse
    {
        if (logRecord.getMethodName().isEmpty() == false)
        {
            cerr << "Method: " << logRecord.getMethodName() << endl;
        }
        cerr << "Location: " << FileInformation(logRecord.getFilename()).getFileName() << ":" << AString::number(logRecord.getLineNumber()) << endl;
    }
#endif
    cerr << endl;
}


