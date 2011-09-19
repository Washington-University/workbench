
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

#define __LOG_RECORD_DECLARE__
#include "LogRecord.h"
#undef __LOG_RECORD_DECLARE__

using namespace caret;


/**
 * Constructor.
 *
 * @param level
 *    Logging level for message.
 * @param methodName
 *    Method that logged the message.
 * @param filename
 *    Name of file that originated the message.
 * @param lineNumber
 *    Line number of message.
 * @param text
 *    Text description.
 */
LogRecord::LogRecord(const LogLevelEnum::Enum level,
                     const AString& methodName,
                     const AString& filename,
                     const int32_t lineNumber,
                     const AString& text)
: CaretObject()
{
    this->level = level;
    this->methodName = methodName;
    this->filename = filename;
    this->lineNumber = lineNumber;
    this->text = text;
}

/**
 * Destructor.
 */
LogRecord::~LogRecord()
{
    
}

/**
 * Get a description of this log record.
 * @return String describing this log record.
 */
AString 
LogRecord::toString() const
{
    AString s = "Level=" + LogLevelEnum::toName(this->level);
    if (this->methodName.isEmpty() == false) {
        s += " Method=" + this->methodName;
    }
    if (this->filename.isEmpty() == false) {
        s += " File=" + this->filename;
    }
    if (this->lineNumber >= 0) {
        s += " Line=" + AString::number(this->lineNumber);
    }
    if (this->text.isEmpty() == false) {
        s += " Text=" + this->text;
    }
    
    return s;
}
