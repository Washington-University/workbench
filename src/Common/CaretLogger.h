#ifndef __CARET_LOGGER__H_
#define __CARET_LOGGER__H_

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

#include "CaretFunctionName.h"
#include "CaretObject.h"

#include "Logger.h"

namespace caret {

    /**
     * \brief The Caret Logger.
     *
     * This is the Caret Logger.  Its single instance is created by
     * the LogManager.
     *
     * Do not use this class.  Instead use the macro in this file
     * to send log messages at various levels.  The macros are 
     * designed so that the message is not created unless the 
     * specific level (or below) is enabled.
     */
    class CaretLogger {
    public:
        /** 
         * Set the Caret logger
         * @param logger The logger's value.
         */
        static void setLogger(Logger* logger) { CaretLogger::logger = logger; }
        
        /**
         * Get the Caret Logger.
         * @return The Caret logger 
         */
        inline static Logger* getLogger() { return CaretLogger::logger; }
        
        /**
         * @return True if the CaretLogger is valid and can be used, else false.
         */
        static bool isValid() { return (CaretLogger::logger != NULL); }
        
    private:
        CaretLogger() { }
        ~CaretLogger() { }
        CaretLogger(const CaretLogger&);
        CaretLogger& operator=(const CaretLogger&);

        /** The caret logger.  It is created by the LogManager. */
        static Logger* logger;
    };
    
#ifdef __CARET_LOGGER_DEFINE__
    Logger* CaretLogger::logger = NULL;
#endif //  __CARET_LOGGER_DEFINE__
    
} // namespace

/**
 * \def CaretLogSevere
 *
 * Log a message at the SEVERE level.
 * Severe items typically prevent normal program execution.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogSevere(TEXT) \
((caret::CaretLogger::getLogger()->isSevere())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::SEVERE, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogWarning
 *
 * Log a message at the WARNING level.
 * Warning messages indicate potential problems.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogWarning(TEXT) \
((caret::CaretLogger::getLogger()->isWarning())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::WARNING, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogInfo
 *
 * Log a message at the INFO level.
 * Informational messages that may be helpful to end users.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogInfo(TEXT) \
((caret::CaretLogger::getLogger()->isInfo())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::INFO, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogConfig
 *
 * Log a message at the CONFIG level. 
 * Configuration messages typically involve versions of 
 * libraries, operating system, etc, and may help with
 * issues on specific configurations.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogConfig(TEXT) \
((caret::CaretLogger::getLogger()->isConfig())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::CONFIG, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
: (void)0)


/**
 * \def CaretLogFine
 *
 * Log a message at the FINE level.
 * Fine messages are for developers such as minor, recoverable failures.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogFine(TEXT) \
    ((caret::CaretLogger::getLogger()->isFine())  \
    ? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINE, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
    : (void)0)

/**
 * \def CaretLogFiner
 *
 * Log a message at the FINER level.
 * Finer messages for for developers to provide detailed tracing messages.
 * Typically events and exceptions are logged at the this level.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogFiner(TEXT) \
((caret::CaretLogger::getLogger()->isFiner())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINER, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogFinest
 *
 * Log a message at the FINEST level.
 * Finest messages are for developers to provide highly detailed tracing messages.
 * @param TEXT 
 *    Text that is logged.
 */
#define CaretLogFinest(TEXT) \
((caret::CaretLogger::getLogger()->isFinest())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINEST, __CARET_FUNCTION_NAME__, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogEntering
 *
 * Log a message indicating the function that one is in.
 * This is typically used when entering the function.
 * This message is logged at the FINER level.
 */
#define CaretLogEntering() \
((caret::CaretLogger::getLogger()->isFiner())  \
? caret::CaretLogger::getLogger()->entering(__CARET_FUNCTION_NAME__, __FILE__, __LINE__) \
: (void)0)

/**
 * \def CaretLogExiting
 *
 * Log a message indicating the function that one is in.
 * This is typically used when entering the function.
 * This message is logged at the FINER level.
 */
#define CaretLogExiting() \
((caret::CaretLogger::getLogger()->isFiner())  \
? caret::CaretLogger::getLogger()->exiting(__CARET_FUNCTION_NAME__, __FILE__, __LINE__) \
: (void)0)

/**
 * \def CaretLogThrowing
 *
 * Log a message at the FINER level for an exception class that is
 * derived from CaretException.
 * @param CARET_EXCEPTION 
 *    CaretException that is logged.
 */
#define CaretLogThrowing(CARET_EXCEPTION) \
((caret::CaretLogger::getLogger()->isFiner())  \
? caret::CaretLogger::getLogger()->throwingCaretException(__CARET_FUNCTION_NAME__, __FILE__, __LINE__, CARET_EXCEPTION) \
: (void)0)



#endif  //__CARET_LOGGER__H_
