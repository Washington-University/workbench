#ifndef __EVENT_GET_OR_SET_USER_INPUT_MODE_PROCESSOR_H__
#define __EVENT_GET_OR_SET_USER_INPUT_MODE_PROCESSOR_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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


#include "Event.h"
#include "UserInputReceiverInterface.h"

namespace caret {

    /// Event for getting or setting the user input mode processor
    class EventGetOrSetUserInputModeProcessor : public Event {
        
    public:
        EventGetOrSetUserInputModeProcessor(const int32_t windowIndex,
                                            const UserInputReceiverInterface::UserInputMode userInputMode);
        
        EventGetOrSetUserInputModeProcessor(const int32_t windowIndex);
        
        virtual ~EventGetOrSetUserInputModeProcessor();
        
        bool isGetUserInputMode() const;
        
        bool isSetUserInputMode() const;
        
        int32_t getWindowIndex() const;

        UserInputReceiverInterface::UserInputMode getUserInputMode() const;

        void setUserInputProcessor(UserInputReceiverInterface* userInputProcessor);
        
    private:
        enum MODE_GET_OR_SET {
            GET,
            SET
        };
        
        EventGetOrSetUserInputModeProcessor(const EventGetOrSetUserInputModeProcessor&);
        
        EventGetOrSetUserInputModeProcessor& operator=(const EventGetOrSetUserInputModeProcessor&);
        
        /** Is set when GETTING input mode */
        UserInputReceiverInterface* userInputProcessor;
        
        /** Requested input mode for SETTING and set when GETTING*/
        UserInputReceiverInterface::UserInputMode userInputMode;
        
        /** index of window for update */
        int32_t windowIndex;
        
        /** getting or setting */
        MODE_GET_OR_SET modeGetOrSet;
    };

} // namespace

#endif // __EVENT_GET_OR_SET_USER_INPUT_MODE_PROCESSOR_H__
