
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __EVENT_DISPLAY_PROPERTIES_LABELS_DECLARE__
#include "EventDisplayPropertiesLabels.h"
#undef __EVENT_DISPLAY_PROPERTIES_LABELS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDisplayPropertiesLabels 
 * \brief Event to either get DisplayPropertiesLabels or to be notified if there has been a change
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param mode
 *    The mode of the event
 */
EventDisplayPropertiesLabels::EventDisplayPropertiesLabels(const Mode mode)
: Event(EventTypeEnum::EVENT_DISPLAY_PROPERTIES_LABELS),
m_mode(mode)
{
    
}

/**
 * Destructor.
 */
EventDisplayPropertiesLabels::~EventDisplayPropertiesLabels()
{
}

/**
 * @return the mode of the event
 */
EventDisplayPropertiesLabels::Mode
EventDisplayPropertiesLabels::getMode() const
{
    return m_mode;
}

/**
 * @return The label display properties
 */
const DisplayPropertiesLabels*
EventDisplayPropertiesLabels::getDisplayPropertiesLabels() const
{
    CaretAssertMessage(m_displayPropertiesLabels,
                       "m_displayPropertiesLabels should have been set");
    return m_displayPropertiesLabels;
}

/**
 * Set the label display properties
 * @param dpl
 *    The label display properties
 */
void
EventDisplayPropertiesLabels::setDisplayPropertiesLabels(const DisplayPropertiesLabels* dpl)
{
    CaretAssert(dpl);
    m_displayPropertiesLabels = dpl;
}
