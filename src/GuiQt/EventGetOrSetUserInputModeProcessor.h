#ifndef __EVENT_GET_OR_SET_USER_INPUT_MODE_PROCESSOR_H__
#define __EVENT_GET_OR_SET_USER_INPUT_MODE_PROCESSOR_H__

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


#include "Event.h"
#include "UserInputModeAbstract.h"

namespace caret {

    /// Event for getting or setting the user input mode processor
    class EventGetOrSetUserInputModeProcessor : public Event {
        
    public:
        EventGetOrSetUserInputModeProcessor(const int32_t windowIndex,
                                            const UserInputModeEnum::Enum userInputMode);
        
        EventGetOrSetUserInputModeProcessor(const int32_t windowIndex);
        
        virtual ~EventGetOrSetUserInputModeProcessor();
        
        bool isGetUserInputMode() const;
        
        bool isSetUserInputMode() const;
        
        int32_t getWindowIndex() const;

        UserInputModeEnum::Enum getUserInputMode() const;

        void setUserInputProcessor(UserInputModeAbstract* userInputProcessor);
        
        UserInputModeAbstract* getUserInputProcessor();
        
    private:
        enum MODE_GET_OR_SET {
            GET,
            SET
        };
        
        EventGetOrSetUserInputModeProcessor(const EventGetOrSetUserInputModeProcessor&);
        
        EventGetOrSetUserInputModeProcessor& operator=(const EventGetOrSetUserInputModeProcessor&);
        
        /** Is set when GETTING input mode */
        UserInputModeAbstract* userInputProcessor;
        
        /** Requested input mode for SETTING and set when GETTING*/
        UserInputModeEnum::Enum userInputMode;
        
        /** index of window for update */
        int32_t windowIndex;
        
        /** getting or setting */
        MODE_GET_OR_SET modeGetOrSet;
    };

} // namespace

#endif // __EVENT_GET_OR_SET_USER_INPUT_MODE_PROCESSOR_H__
