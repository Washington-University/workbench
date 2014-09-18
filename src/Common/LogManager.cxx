
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

#define __LOG_MANAGER_DECLARE__
#include "CaretLogger.h"
#include "LogManager.h"
#undef __LOG_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "Logger.h"
#include "LogHandlerStandardError.h"

using namespace caret;


/**
 * Constructor.
 */
LogManager::LogManager()
: CaretObject()
{
}

/**
 * Destructor.
 */
LogManager::~LogManager()
{
    /*
     * Delete all of the loggers.
     */
    for (std::vector<Logger*>::iterator iter = this->loggers.begin();
         iter != this->loggers.end();
         iter++) {
        delete (*iter);
    }
}

/**
 * Add a named logger.  This does nothing an returns false
 * if a logger with the same name is already registered.
 * The Logger factory methods call this method to register
 * newly created loggers.  The LogManager will 'delete'
 * any registed loggers when the LogManager is deleted.
 * 
 * @param logger
 *    Logger that is added.
 * @return 
 *    true if logger was successfully registered, false if
 *    a logger with the same name already exists.
 */
bool 
LogManager::addLogger(Logger* logger)
{
    CaretAssert(logger);
    
    Logger* existingLogger = this->getLogger(logger->getName());
    if (existingLogger != NULL) {
        return false;
    }
    
    this->loggers.push_back(logger);
    
    return true;
}

/**
 * Find a named logger.
 * 
 * @param name
 *   Name of the logger.
 * @return
 *   Matching logger or NULL if none found.
 */
Logger* 
LogManager::getLogger(const AString& name)
{
    /*
     * Find logger with matching name.
     */
    for (std::vector<Logger*>::iterator iter = this->loggers.begin();
         iter != this->loggers.end();
         iter++) {
        Logger* logger = *iter;
        if (logger->getName() == name) {
            return logger;
        }
    }
    
    return NULL;
}

/**
 * Return the global LogManager object.
 * 
 * @return The global LogManager object.
 */
LogManager* 
LogManager::getLogManager()
{
    return LogManager::singletonLogManager;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
LogManager::toString() const
{
    return "LogManager";
}

/**
 * Create the log manager.
 * This must be called one AND ONLY one time.
 */
void 
LogManager::createLogManager()
{
    CaretAssertMessage((LogManager::singletonLogManager == NULL), 
                       "Log manager has already been created.");
    
    LogManager::singletonLogManager = new LogManager();
    
    Logger* caretLoggerInstance = Logger::getLogger("CaretLogger");
    caretLoggerInstance->setLevel(LogLevelEnum::CONFIG);
    //caretLoggerInstance->setLevel(LogLevelEnum::FINEST);
    caretLoggerInstance->addLogHandler(new LogHandlerStandardError());
    CaretLogger::setLogger(caretLoggerInstance);
}

/**
 * Delete the log manager.
 * This may only be called one time after log manager is created.
 */
void 
LogManager::deleteLogManager()
{
    CaretAssertMessage((LogManager::singletonLogManager != NULL), 
                       "Log manager does not exist, cannot delete it.");
    
    delete LogManager::singletonLogManager;
    LogManager::singletonLogManager = NULL;
}



