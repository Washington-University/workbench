#ifndef __LOG_MANAGER__H_
#define __LOG_MANAGER__H_

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

    class Logger;
    
    /**
     * \brief The logging manager.
     *
     * There is one global LogManager that keeps track of all
     * Loggers.  The LogManager must be initialized prior to
     * creating any Loggers.  Whn the LogManager is deleted
     * all loggers are also deleted.
     * This class emulates Java's java.util.logging.LogManager.
     */
    class LogManager : public CaretObject {
        
    public:        
        static void createLogManager();
        
        static void deleteLogManager();
        
        bool addLogger(Logger* logger);
        
        Logger* getLogger(const AString& name);
        
        static LogManager* getLogManager();
        
    private:
        LogManager();
        
        virtual ~LogManager();
        
        LogManager(const LogManager&);

        LogManager& operator=(const LogManager&);
        
        /** All loggers */
        std::vector<Logger*> loggers;

        /** Global log manager */
        static LogManager* singletonLogManager;
        
    public:
        virtual AString toString() const;
        
    private:
    };
    
#ifdef __LOG_MANAGER_DECLARE__
    LogManager* LogManager::singletonLogManager = NULL;
#endif // __LOG_MANAGER_DECLARE__

} // namespace
#endif  //__LOG_MANAGER__H_
