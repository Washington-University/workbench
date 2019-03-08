
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WU_Q_MACRO_EXECUTOR_MONITOR_DECLARE__
#include "WuQMacroExecutorMonitor.h"
#undef __WU_Q_MACRO_EXECUTOR_MONITOR_DECLARE__

#include <iostream>

#include <QApplication>
#include <QMutexLocker>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQMacroExecutorMonitor 
 * \brief Allows interuption to pause or stop a macro
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent object
 */
WuQMacroExecutorMonitor::WuQMacroExecutorMonitor(QObject* parent)
: QObject(parent)
{
    
}

/**
 * Destructor.
 */
WuQMacroExecutorMonitor::~WuQMacroExecutorMonitor()
{
}

/**
 * @return Error message indicating macro stopped by
 * user.  Use this method for error message so that it
 * is consistent.
 */
QString
WuQMacroExecutorMonitor::getStoppedByUserMessage() const
{
    return "Execution stopped by user";
}

/**
 * @return The mode
 */
WuQMacroExecutorMonitor::Mode
WuQMacroExecutorMonitor::getMode() const
{
    QMutexLocker locker(&m_modeMutex);
    return m_mode;
}

/**
 * Set the mode
 *
 * @param mode
 *     New mode
 */
void
WuQMacroExecutorMonitor::setMode(const Mode mode)
{
    QMutexLocker locker(&m_modeMutex);
    m_mode = mode;
    switch (m_mode) {
        case Mode::PAUSE:
            break;
        case Mode::RUN:
            break;
        case Mode::STOP:
            break;
    }
}

/**
 * @return True if execution should stop and the command
 * should cleanup and return
 */
bool
WuQMacroExecutorMonitor::testForStop() const
{
    bool stopFlag(false);
    switch (getMode()) {
        case Mode::PAUSE:
            stopFlag = doPause();
            break;
        case Mode::RUN:
            break;
        case Mode::STOP:
            stopFlag = true;
            break;
    }
    return stopFlag;
}

/**
 * Pause execution and return when mode changes to run or stop.
 * @return True if execution should stop and the command
 * should cleanup and return.
 */
bool
WuQMacroExecutorMonitor::doPause() const
{
    bool stopFlag(false);
    
    bool waitFlag(true);
    while (waitFlag) {
        QApplication::processEvents();
        
        switch (getMode()) {
            case Mode::PAUSE:
                break;
            case Mode::RUN:
                stopFlag = false;
                waitFlag = false;
                break;
            case Mode::STOP:
                stopFlag = true;
                waitFlag = false;
                break;
        }
    }
    
    return stopFlag;
}


