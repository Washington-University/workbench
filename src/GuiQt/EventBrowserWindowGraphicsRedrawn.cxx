
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

#define __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_DECLARE__
#include "EventBrowserWindowGraphicsRedrawn.h"
#undef __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_DECLARE__

#include "BrainBrowserWindow.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserWindowGraphicsRedrawn 
 * \brief Event issued when a browser windows graphics are redrawn.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
EventBrowserWindowGraphicsRedrawn::EventBrowserWindowGraphicsRedrawn(BrainBrowserWindow* brainBrowserWindow)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN)
{
    m_brainBrowserWindow = brainBrowserWindow;
    m_brainBrowserWindowIndex = m_brainBrowserWindow->getBrowserWindowIndex();
}

/**
 * Destructor.
 */
EventBrowserWindowGraphicsRedrawn::~EventBrowserWindowGraphicsRedrawn()
{
    
}

/**
 * @return Browser window that was redrawn.
 */
BrainBrowserWindow*
EventBrowserWindowGraphicsRedrawn::getBrowserWindow() const
{
    return m_brainBrowserWindow;
}

/**
 * @return Index of browser window that was redrawn.
 */
int32_t
EventBrowserWindowGraphicsRedrawn::getBrowserWindowIndex() const
{
    return m_brainBrowserWindowIndex;
}

