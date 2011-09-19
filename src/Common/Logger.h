#ifndef __LOGGER_H__
#define __LOGGER_H__

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
#include "CaretException.h"
#include "LogLevelEnum.h"

namespace caret {

    class LogHandler;
    
    /*!
     * \brief Logs messages.
     *
     * Logger logs messages that are sent to registered handlers.
     * Each logger has an associated level.  Only messages with
     * a level equal to or 'above' the level are forwarded to 
     * handlers.  If the message is to be forwarded, a LogRecord
     * is created and then forwarded to the registered handlers.
     * This class emulates the Java class java.util.logging.
     */
    class Logger : public CaretObject {
        
    public:        
        virtual ~Logger();
        
        static Logger* getLogger(const AString& name);
        
        void log(const LogLevelEnum::Enum logLevel,
                 const AString& methodName,
                 const AString& filename,
                 const int32_t lineNumber,
                 const AString& text);
        
        void entering(const AString& methodName,
                      const AString& filename,
                      const int32_t lineNumber);
        
        void exiting(const AString& methodName,
                     const AString& filename,
                     const int32_t lineNumber);
        
        void throwingCaretException(const AString& methodName,
                                    const AString& filename,
                                    const int32_t lineNumber,
                                    CaretException& caretException);
                      
        LogLevelEnum::Enum getLevel() const;
        
        void setLevel(const LogLevelEnum::Enum level);
        
        void addLogHandler(LogHandler* logHandler);
        
        /** @return Is severe logging enabled? */
        inline bool isSevere() const { return this->severeLoggingEnabled; }
        
        /** @return Is warning logging enabled? */
        inline bool isWarning() const { return this->warningLoggingEnabled; }
        
        /** @return Is info logging enabled? */
        inline bool isInfo() const { return this->infoLoggingEnabled; }
        
        /** @return Is config logging enabled? */
        inline bool isConfig() const { return this->configLoggingEnabled; }
        
        /** @return Is fine logging enabled? */
        inline bool isFine() const { return this->fineLoggingEnabled; }
        
        /** @return Is finer logging enabled? */
        inline bool isFiner() const { return this->finerLoggingEnabled; }
        
        /** @return Is finest logging enabled? */
        inline bool isFinest() const { return this->finestLoggingEnabled; }
        
        /** @return Name of this logger. */
        AString getName() const { return this->name; }
        
    private:
        Logger(const AString& name);
        
        Logger(const Logger&);

        Logger& operator=(const Logger&);
        
        AString name;
        
        bool severeLoggingEnabled;
        
        bool warningLoggingEnabled;
        
        bool infoLoggingEnabled;
        
        bool configLoggingEnabled;
        
        bool fineLoggingEnabled;
        
        bool finerLoggingEnabled;
        
        bool finestLoggingEnabled;
        
        LogLevelEnum::Enum level;
        
        std::vector<LogHandler*> logHandlers;
        
    public:
        virtual AString toString() const;
        
    private:
    };
    
#ifdef __LOGGER_DECLARE__
#endif // __LOGGER_DECLARE__

} // namespace



#endif  //__LOGGER_H__
