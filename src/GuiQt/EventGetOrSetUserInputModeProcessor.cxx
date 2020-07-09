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

#include "EventGetOrSetUserInputModeProcessor.h"

using namespace caret;
/**
 * \class caret::EventGetOrSetUserInputModeProcessor 
 * \brief Event that acts as a getter/setting for user input mode.
 * \ingroup GuiQt
 */

/**
 * Constructor for SETTING the user input mode.
 *
 * @param windowIndex
 *    Index of the window for the user input mode.
 * @param userInputMode
 *    The requested input mode.
 */
EventGetOrSetUserInputModeProcessor::EventGetOrSetUserInputModeProcessor(const int32_t windowIndex,
                                                                         const UserInputModeEnum::Enum userInputMode)
: Event(EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE)
{
    this->userInputProcessor = NULL;
    this->userInputMode = userInputMode;
    this->windowIndex   = windowIndex;
    this->modeGetOrSet  = SET;
}

/**
 * Constructor for GETTING the user input mode.
 *
 * @param windowIndex
 *    Index of the window for the user input mode.
 */
EventGetOrSetUserInputModeProcessor::EventGetOrSetUserInputModeProcessor(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE)
{
    this->userInputProcessor = NULL;
    this->userInputMode = UserInputModeEnum::Enum::INVALID;
    this->windowIndex   = windowIndex;
    this->modeGetOrSet  = GET;
}

/*
 * Destructor.
 */
EventGetOrSetUserInputModeProcessor::~EventGetOrSetUserInputModeProcessor()
{
}

/**
 * @return The window index.
 */
int32_t 
EventGetOrSetUserInputModeProcessor::getWindowIndex() const 
{ 
    return this->windowIndex; 
}

/**
 * @return The requested input mode.
 */
UserInputModeEnum::Enum
EventGetOrSetUserInputModeProcessor::getUserInputMode() const
{
    return this->userInputMode; 
}

/**
 * Set the user input processor which is called when GETTING.
 * @param userInputProcessor
 *    Value of current input processor.
 */
void 
EventGetOrSetUserInputModeProcessor::setUserInputProcessor(UserInputModeAbstract* userInputProcessor)
{
    this->userInputProcessor = userInputProcessor;
    this->userInputMode      = this->userInputProcessor->getUserInputMode();
}

/**
 * @return The user input processor valid only when GETTING.
 */
UserInputModeAbstract* 
EventGetOrSetUserInputModeProcessor::getUserInputProcessor()
{
    return this->userInputProcessor;
}

/**
 * @return true if this event is GETTING the user input mode.
 */
bool 
EventGetOrSetUserInputModeProcessor::isGetUserInputMode() const
{
    return (this->modeGetOrSet == GET);
}

/**
 * @return true if this event is SETTING the user input mode.
 */
bool 
EventGetOrSetUserInputModeProcessor::isSetUserInputMode() const
{
    return (this->modeGetOrSet == SET);
}

