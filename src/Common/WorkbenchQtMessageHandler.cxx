
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

#define __WORKBENCH_QT_MESSAGE_HANDLER_DECLARE__
#include "WorkbenchQtMessageHandler.h"
#undef __WORKBENCH_QT_MESSAGE_HANDLER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
using namespace std;

    
/**
 * \class caret::WorkbenchQtMessageHandler
 * \brief Processes messages from Qt with filtering to suppress some messages
 * \ingroup Common
 */

/**
 * Constructor.
 */
WorkbenchQtMessageHandler::WorkbenchQtMessageHandler()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
WorkbenchQtMessageHandler::~WorkbenchQtMessageHandler()
{
}

/**
 * Setup message handle for Qt.
 * @param beeperCallback
 *    Instance of 'Beeper' class with method for issuing 'beep' sound.  
 */

void
WorkbenchQtMessageHandler::setupHandler(Beeper* beeper)
{
    s_beeper = beeper;
    
    qInstallMessageHandler(WorkbenchQtMessageHandler::messageHandlerForQt5);//this handler uses CaretLogger and GuiManager, so we must install it after the logger is available and the application is created
}

/**
 * Setup message handle for Qt 5
 * @param type
 *    Type of message (Debug, info, warning, etc)
 * @param context
 *    Message log context (see Qt Doc for QMessageLogContext)
 * @param msg
 *    Message that is displayed.
 */
void
WorkbenchQtMessageHandler::messageHandlerForQt5(QtMsgType type,
                                                const QMessageLogContext& context,
                                                const QString& msg)
{
    /*
     * Some messages produced by Qt can be safely ignored
     */
    if (WorkbenchQtMessageHandler::isMessageTextSuppressed(msg)) {
        return;
    }
    
    const AString backtrace = SystemUtilities::getBackTrace();
    
    const AString contextInfo = ("   Context Info File ("
                                 + QString(context.file)
                                 + ") Function (" + QString(context.function)
                                 + ") Line (" + QString::number(context.line)
                                 + ") Version (" + QString::number(context.version)
                                 + ") Category (" + QString(context.category)
                                 + ")");
    const AString message = (AString(msg) + "\n"
                             + contextInfo + "\n"
                             + backtrace);
    
    if (CaretLogger::isValid()) {
        bool abortFlag = false;
        bool displayedFlag = false;
        switch (type) {
            case QtDebugMsg:
                CaretLogInfo(message);
                displayedFlag = CaretLogger::getLogger()->isInfo();
                break;
            case QtWarningMsg:
                CaretLogWarning(message);
                displayedFlag = CaretLogger::getLogger()->isWarning();
                break;
            case QtCriticalMsg:
                CaretLogSevere(message);
                displayedFlag = CaretLogger::getLogger()->isSevere();
                break;
            case QtFatalMsg:
                cerr << "Qt Fatal: " << message << endl;
                abortFlag = true;//fatal will cause an abort, so always display it, bypassing logger entirely
                displayedFlag = true;
                break;
#if QT_VERSION >= 0x050500
            case QtInfoMsg:
                CaretLogInfo(message);
                displayedFlag = CaretLogger::getLogger()->isInfo();
                break;
#endif
        }
        
        /*
         * Beep to alert user about an error!!!
         */
        if (displayedFlag && (type != QtDebugMsg))//don't beep for debug
        {
            WorkbenchQtMessageHandler::makeBeepSound();
        }
#ifndef NDEBUG
        if (!displayedFlag)
        {
            cerr << "DEBUG: Qt ";
            switch (type)
            {
                case QtDebugMsg:
                    cerr << "Debug ";
                    break;
                case QtWarningMsg:
                    cerr << "Warning ";
                    break;
                case QtCriticalMsg:
                    cerr << "Critical ";
                    break;
                case QtFatalMsg:
                    cerr << "FATAL (?!?) ";//should never happen
                    break;
#if QT_VERSION >= 0x050500
                case QtInfoMsg:
                    std::cerr << "Info ";
                    break;
#endif
            }
            cerr << "message hidden" << endl;
        }
#endif
        
        if (abortFlag) {
            std::abort();
        }
    }
    else {
        switch (type) {
            case QtDebugMsg:
                std::cerr << "Qt Debug: " << message << std::endl;
                break;
            case QtWarningMsg:
                std::cerr << "Qt Warning: " << message << std::endl;
                break;
            case QtCriticalMsg:
                std::cerr << "Qt Critical: " << message << std::endl;
                break;
            case QtFatalMsg:
                std::cerr << "Qt Fatal: " << message << std::endl;
                std::abort();
                break;
#if QT_VERSION >= 0x050500
            case QtInfoMsg:
                std::cerr << "Qt Info: " << message << std::endl;
                break;
#endif
        }
    }
}

/**
 * Make a beeping sound using the 'Beeper' instance, if available
 */
void
WorkbenchQtMessageHandler::makeBeepSound()
{
    if (s_beeper != NULL) {
        s_beeper->makeBeep();
    }
}

/**
 * @return True if the given message should be suppressed else false.
 * @param msg
 *    Message tested for suppression.
 *
 * Note: some messages produce by Qt are not a problem and can be safely ignored.
 * An example is "ICC" profile issues for PNG images.
 */
bool
WorkbenchQtMessageHandler::isMessageTextSuppressed(const QString& msg)
{
    if ( ! s_suppressedMessagesTextValidFlag) {
        s_suppressedMessagesTextValidFlag = true;
        
        /*
         * Suppress messages that are not a problem, usually from Qt
         */
        s_supressedMessagesText.push_back("QPngHandler: Failed to parse ICC profile");
        s_supressedMessagesText.push_back("fromIccProfile: failed general sanity check");
        s_supressedMessagesText.push_back("Unsupported ICC profile class");
        s_supressedMessagesText.push_back("Populating font family aliases");
    }
    
    for (const auto& smt : s_supressedMessagesText) {
        if (msg.contains(smt)) {
            return true;
        }
    }
    
    return false;
}


