#ifndef __WORKBENCH_QT_MESSAGE_HANDLER_H__
#define __WORKBENCH_QT_MESSAGE_HANDLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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



#include <memory>

#include <QtGlobal>

#include "CaretObject.h"

namespace caret {

    class WorkbenchQtMessageHandler : public CaretObject {
        
    public:
        /*
         * Class for issuing a 'beep' sound
         */
        class Beeper {
        public:
            /**
             * Constructor
             */
            Beeper() { }
            
            /**
             * Destructor
             */
            virtual ~Beeper() { }
            
            /**
             * Issue the beep sound (must be overridden to produce a beep sound)
             */
            virtual void makeBeep() { }
        };
        
        WorkbenchQtMessageHandler(const WorkbenchQtMessageHandler&) = delete;

        WorkbenchQtMessageHandler& operator=(const WorkbenchQtMessageHandler&) = delete;
        
        static void setupHandler(Beeper* beeper);
        
        static void makeBeepSound();
        
        // ADD_NEW_METHODS_HERE
        
    private:
        WorkbenchQtMessageHandler();
        
        virtual ~WorkbenchQtMessageHandler();
        
        static void messageHandlerForQt4(QtMsgType type,
                                         const char* msg);
        
        static void messageHandlerForQt5(QtMsgType type,
                                         const QMessageLogContext& context,
                                         const QString& msg);
        
        static bool isMessageTextSuppressed(const QString& msg);
        
        static Beeper* s_beeper;
        
        static std::vector<QString> s_supressedMessagesText;
        
        static bool s_suppressedMessagesTextValidFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_QT_MESSAGE_HANDLER_DECLARE__
    WorkbenchQtMessageHandler::Beeper* WorkbenchQtMessageHandler::s_beeper = NULL;
    std::vector<QString> WorkbenchQtMessageHandler::s_supressedMessagesText;
    bool WorkbenchQtMessageHandler::s_suppressedMessagesTextValidFlag = false;
#endif // __WORKBENCH_QT_MESSAGE_HANDLER_DECLARE__

} // namespace
#endif  //__WORKBENCH_QT_MESSAGE_HANDLER_H__
