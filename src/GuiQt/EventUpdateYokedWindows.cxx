
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __EVENT_UPDATE_YOKED_WINDOWS_DECLARE__
#include "EventUpdateYokedWindows.h"
#undef __EVENT_UPDATE_YOKED_WINDOWS_DECLARE__

#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventUpdateYokedWindows 
 * \brief Updates windows that are yoked.
 * \ingroup GuiQt
 */

/**
 * Constructor for a window.  This given window will NOT be updated.
 *
 * @param browserWindowIndexThatIssuedEvent
 *     Index of browser window that issued event.
 * @param yokingGroup
 *     Yoking group that is selected.
 */
EventUpdateYokedWindows::EventUpdateYokedWindows(const int32_t browserWindowIndexThatIssuedEvent,
                                                 const YokingGroupEnum::Enum yokingGroup)
: Event(EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS),
m_browserWindowIndexThatIssuedEvent(browserWindowIndexThatIssuedEvent),
m_yokingGroup(yokingGroup)
{
}

/**
 * Constructor for updating all windows.
 *
 * @param browserWindowIndexThatIssuedEvent
 *     Index of browser window that issued event.
 * @param yokingGroup
 *     Yoking group that is selected.
 */
EventUpdateYokedWindows::EventUpdateYokedWindows(const YokingGroupEnum::Enum yokingGroup)
: Event(EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS),
m_browserWindowIndexThatIssuedEvent(-1),
m_yokingGroup(yokingGroup)
{
}

/**
 * Destructor.
 */
EventUpdateYokedWindows::~EventUpdateYokedWindows()
{
    
}

/**
 * @return Index of browser window that issued the event.
 */
int32_t
EventUpdateYokedWindows::getBrowserWindowIndexThatIssuedEvent() const
{
    return m_browserWindowIndexThatIssuedEvent;
}

/**
 * @return The yoking group that should be updated.
 */
YokingGroupEnum::Enum
EventUpdateYokedWindows::getYokingGroup() const
{
    return m_yokingGroup;
}
