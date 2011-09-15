#ifndef __CARET_LOGGER__H_
#define __CARET_LOGGER__H_

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
 * @param text 
 *    Text that is logged.
 */
#define CaretLogSevere(TEXT) \
((caret::CaretLogger::getLogger()->isSevere())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::SEVERE, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogWarning
 *
 * Log a message at the WARNING level.
 * @param text 
 *    Text that is logged.
 */
#define CaretLogWarning(TEXT) \
((caret::CaretLogger::getLogger()->isWarning())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::WARNING, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogInfo
 *
 * Log a message at the INFO level.
 * @param text 
 *    Text that is logged.
 */
#define CaretLogInfo(TEXT) \
((caret::CaretLogger::getLogger()->isInfo())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::INFO, __FILE__, __LINE__, (TEXT)) \
: (void)0)


/**
 * \def CaretLogFine
 *
 * Log a message at the FINE level.
 * @param text 
 *    Text that is logged.
 */
#define CaretLogFine(TEXT) \
    ((caret::CaretLogger::getLogger()->isFine())  \
    ? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINE, __FILE__, __LINE__, (TEXT)) \
    : (void)0)

/**
 * \def CaretLogFiner
 *
 * Log a message at the FINER level.
 * @param text 
 *    Text that is logged.
 */
#define CaretLogFiner(TEXT) \
((caret::CaretLogger::getLogger()->isFiner())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINER, __FILE__, __LINE__, (TEXT)) \
: (void)0)

/**
 * \def CaretLogFinest
 *
 * Log a message at the FINEST level.
 * @param text 
 *    Text that is logged.
 */
#define CaretLogFinest(TEXT) \
((caret::CaretLogger::getLogger()->isFinest())  \
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINEST, __FILE__, __LINE__, (TEXT)) \
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
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINER, __FILE__, __LINE__, (AString("Entering function: ").append(__func__))) \
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
? caret::CaretLogger::getLogger()->log(caret::LogLevelEnum::FINER, __FILE__, __LINE__, (AString("Exiting function: ").append(__func__))) \
: (void)0)



#endif  //__CARET_LOGGER__H_
