
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

#define __WB_MACRO_CUSTOM_OPERATION_BASE_DECLARE__
#include "WbMacroCustomOperationBase.h"
#undef __WB_MACRO_CUSTOM_OPERATION_BASE_DECLARE__

#include <QApplication>

#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfacesGet.h"
#include "EventUserInterfaceUpdate.h"
#include "MovieRecorder.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SystemUtilities.h"
#include "WuQMacroCommand.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationBase 
 * \brief Base class for all macro custom operations
 * \ingroup GuiQt
 */

/**
 * Constructor
 *
 * @param operationTye
 *     Type of custom command operation
 */
WbMacroCustomOperationBase::WbMacroCustomOperationBase(const WbMacroCustomOperationTypeEnum::Enum operationType)
: m_operationType(operationType)
{
    
}


/**
 * Destructor
 */
WbMacroCustomOperationBase::~WbMacroCustomOperationBase()
{
    
}

/**
 * @return The custom operation type
 */
WbMacroCustomOperationTypeEnum::Enum
WbMacroCustomOperationBase::getOperationType() const
{
    return m_operationType;
}

/**
 * @return The error message
 */
QString
WbMacroCustomOperationBase::getErrorMessage() const
{
    return m_errorMessage;
}

bool
WbMacroCustomOperationBase::validateCorrectNumberOfParameters(const WuQMacroCommand* command)
{
    std::unique_ptr<WuQMacroCommand> validCommand(createCommand());
    const int32_t paramCount = command->getNumberOfParameters();
    const int32_t validCount = validCommand->getNumberOfParameters();
    
    if (paramCount < validCount) {
        appendToErrorMessage("Command should contain "
                             + QString::number(validCount)
                             + " parameters but contains "
                             + QString::number(paramCount)
                             + " parameters");
        return false;
    }
    
    return true;
}

/**
 * Append text to the error message.  If the current error message
 * is not empty, a newline is added prior to the text.
 *
 * @param text
 *     Text to append to error message
 */
void
WbMacroCustomOperationBase::appendToErrorMessage(const QString& text)
{
    if ( m_errorMessage.isEmpty()) {
        m_errorMessage.append("\n");
    }
    m_errorMessage.append(text);
}

/**
 * Find the surface with the given name
 *
 * @param name
 *    Name of surface
 * @param errorMessagePrefix
 *    Prefix inserted into error message if an error occurs
 * @return
 *    Pointer to surface or NULL if not found.
 *    If not found getErrorMessage() explains why
 */
Surface*
WbMacroCustomOperationBase::findSurface(const QString& name,
                                        const QString& errorMessagePrefix)
{
    if (name.isEmpty()) {
        appendToErrorMessage(errorMessagePrefix
                             + " name is empty");
        return NULL;
    }
    
    EventSurfacesGet eventSurfaces;
    EventManager::get()->sendEvent(eventSurfaces.getPointer());
    std::vector<Surface*> allSurfaces = eventSurfaces.getSurfaces();
    
    for (auto s : allSurfaces) {
        if (s->getFileName().endsWith(name)) {
            return s;
        }
    }
    
    appendToErrorMessage(errorMessagePrefix
                         + " with name \""
                         + name
                         + "\" not found.");
    return NULL;
    
}

/**
 * Update graphics
 */
void
WbMacroCustomOperationBase::updateGraphics()
{
    /*
     * Passing 'true' indicate do a repaint().  A 'repaint' is performed immediately.
     * Otherwise, the graphics update is scheduled for a later time.
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(true).getPointer());
    
    /*
     * Qt needs time to update stuff
     */
    QApplication::processEvents();
}

/**
 * Update the user-interface
 */
void
WbMacroCustomOperationBase::updateUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Get number of steps and sleep time.  If a movie is being recorded, the number of
 * steps is set so that the command will run for the requested duration using the
 * frame rate of the movie recorder.  If a movie
 * is NOT being recorded, the number of steps out is the default number of steps and
 * the sleep time is set so that the command will run for approximately the duration.
 *
 * @param defaultNumberOfSteps
 *     The default number of steps used when a movie is not being recorded
 * @param durationSeconds
 *     The number of seconds for which the command should run
 * @param numberOfStepsOut
 *     Output with number of steps for the command
 * @param sleepTimeOut
 *     Output with time command should sleep at the end of each iteration
 *     when a movie is not being recorded
 */
void
WbMacroCustomOperationBase::getNumberOfStepsAndSleepTime(const float defaultNumberOfSteps,
                                                         const float durationSeconds,
                                                         float& numberOfStepsOut,
                                                         float& sleepTimeOut)
{
    numberOfStepsOut = defaultNumberOfSteps;
    sleepTimeOut     = 0.0;

    const MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    switch (movieRecorder->getRecordingMode()) {
        case MovieRecorderModeEnum::MANUAL:
            sleepTimeOut = durationSeconds / numberOfStepsOut;
            break;
        case MovieRecorderModeEnum::AUTOMATIC:
            numberOfStepsOut = (durationSeconds
                                * movieRecorder->getFramesRate());
            break;
    }
}

/**
 * Sleep for the given number of seconds at the end of an iteration
 *
 * @param seconds
 *     Seconds to sleep
 */
void
WbMacroCustomOperationBase::sleepForSecondsAtEndOfIteration(const float seconds)
{
    if (seconds > 0.0) {
        SystemUtilities::sleepSeconds(seconds);
    }
}


