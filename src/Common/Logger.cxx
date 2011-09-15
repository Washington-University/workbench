
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

#define __LOGGER_DECLARE__
#include "Logger.h"
#undef __LOGGER_DECLARE__

#include "CaretAssert.h"
#include "LogHandler.h"
#include "LogManager.h"
#include "LogRecord.h"

using namespace caret;


/**
 * Constructor.
 */
Logger::Logger(const AString& name)
: CaretObject()
{
    this->name = name;
    this->setLevel(LogLevelEnum::ALL);
}

/**
 * Destructor.
 */
Logger::~Logger()
{
    for (std::vector<LogHandler*>::iterator iter = this->logHandlers.begin();
         iter != this->logHandlers.end();
         iter++) {
        LogHandler* lh = *iter;
        delete lh;
    }
}

/**
 * Find or create a logger with the specified name.
 * If a logger exists with the given name, it is returned.
 * Otherwise, a new Logger is created and returned.
 * Never delete the returned Logger as it will be
 * deleted when the LogManager is deleted.
 */
Logger* 
Logger::getLogger(const AString& name)
{
    CaretAssertMessage((name.isEmpty() == false), "Logger name must not be empty string.");
    
    Logger* existingLogger = LogManager::getLogManager()->getLogger(name);
    if (existingLogger != NULL) {
        return existingLogger;
    }
    
    Logger* logger = new Logger(name);
    bool exists = LogManager::getLogManager()->addLogger(logger);
    CaretAssertMessage(exists, "Trying to add logger and logger with name exists.");
    return logger;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Logger::toString() const
{
    return ("Logger " + this->name);
}

/**
 * Log a message.  Level MAY NOT be ALL or OFF.
 *
 * @param logLevel
 *    Logging level for message.
 * @param filename
 *    Name of file that originated the message.
 * @param lineNumber
 *    Line number of message.
 * @param text
 *    Text description.
 *    
 */
void 
Logger::log(const LogLevelEnum::Enum logLevel,
                const AString& filename,
                const int32_t lineNumber,
                const AString& text)
{
    switch (logLevel) {
        case LogLevelEnum::OFF:
        case LogLevelEnum::ALL:
            CaretAssertMessage(0, "Cannot log record with level = OFF or ALL");
            break;
        case LogLevelEnum::FINEST:
            if (this->finestLoggingEnabled == false) return;
            break;
        case LogLevelEnum::FINER:
            if (this->finerLoggingEnabled == false) return;
            break;
        case LogLevelEnum::FINE:
            if (this->fineLoggingEnabled == false) return;
            break;
        case LogLevelEnum::INFO:
            if (this->infoLoggingEnabled == false) return;
            break;
        case LogLevelEnum::WARNING:
            if (this->warningLoggingEnabled == false) return;
            break;
        case LogLevelEnum::SEVERE:
            if (this->severeLoggingEnabled == false) return;
            break;
    }
    const LogRecord logRecord(logLevel,
                              filename,
                              lineNumber,
                              text);
    
    /*
     * Send to all of the handlers.
     */
    for (std::vector<LogHandler*>::iterator iter = this->logHandlers.begin();
         iter != this->logHandlers.end();
         iter++) {
        LogHandler* lh = *iter;
        lh->publish(logRecord);
    }
}

/**
 * Get the current logging level.
 *
 * @return Current logging level.
 */
LogLevelEnum::Enum 
Logger::getLevel() const
{
    return this->level;
}

/**
 * Set the logging level.
 *
 * @param level
 *    New level for logging.
 */
void 
Logger::setLevel(const LogLevelEnum::Enum level)
{
    this->level = level;
    
    this->severeLoggingEnabled  = false;
    this->warningLoggingEnabled = false;
    this->infoLoggingEnabled    = false;
    this->fineLoggingEnabled    = false;
    this->finerLoggingEnabled   = false;
    this->finestLoggingEnabled  = false;
    
    /*
     * Notice that levels are arranged from
     * from LOWEST to HIGHEST and that most 
     * have not break statements.  Thus
     * 'falling through' the 'case' statements
     * sets the higher levels of logging.
     */
    switch (this->level) {
        case LogLevelEnum::OFF:
            break;
        case LogLevelEnum::ALL:
        case LogLevelEnum::FINEST:
            this->finestLoggingEnabled = true;
        case LogLevelEnum::FINER:
            this->finerLoggingEnabled = true;
        case LogLevelEnum::FINE:
            this->fineLoggingEnabled = true;
        case LogLevelEnum::INFO:
            this->infoLoggingEnabled = true;
        case LogLevelEnum::WARNING:
            this->warningLoggingEnabled = true;
        case LogLevelEnum::SEVERE:
            this->severeLoggingEnabled = true;
            break;
    }
}

/**
 * Add a log handler to a logger.  This object
 * will take care deleting the handlers that
 * it uses.
 * 
 * @param logHandler
 *    Handler that is added.
 */
void 
Logger::addLogHandler(LogHandler* logHandler)
{
    this->logHandlers.push_back(logHandler);
}

