
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

#define __EVENT_BROWSER_TAB_VALIDATE_DECLARE__
#include "EventBrowserTabValidate.h"
#undef __EVENT_BROWSER_TAB_VALIDATE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabValidate 
 * \brief Verify that a pointer to a browser tab is still valid.
 * \ingroup Brain
 */

/**
 * Constructor.
 *  @param browserTabContent
 *  Browser tab tested for validity
 */
EventBrowserTabValidate::EventBrowserTabValidate(const BrowserTabContent* browserTabContent)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_VALIDATE),
m_browserTabContent(browserTabContent)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabValidate::~EventBrowserTabValidate()
{
}

/**
 * @return Pointer to browser tab that is being validated
 */
const BrowserTabContent*
EventBrowserTabValidate::getBrowserTabContent() const
{
    return m_browserTabContent;
}

/**
 * @return True if browser tab is valid, else false.
 */
bool
EventBrowserTabValidate::isValid() const
{
    return m_validFlag;
}

/**
 * Set the validity of a browser tab
 * @param validFlag
 *    Validity of tab
 */
void
EventBrowserTabValidate::setValid(const bool validFlag)
{
    m_validFlag = validFlag;
}
