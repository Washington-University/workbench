
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_SPACER_TAB_GET_DECLARE__
#include "EventSpacerTabGet.h"
#undef __EVENT_SPACER_TAB_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventSpacerTabGet 
 * \brief Event for getting a spacer tag.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param windowIndex
 *     Index of the window
 * @param rowIndex
 *     Index of the row
 * @param columnIndex
 *     Index of the column
 */
EventSpacerTabGet::EventSpacerTabGet(const int32_t windowIndex,
                                     const int32_t rowIndex,
                                     const int32_t columnIndex)
: Event(EventTypeEnum::EVENT_SPACER_TAB_GET),
m_windowIndex(windowIndex),
m_rowIndex(rowIndex),
m_columnIndex(columnIndex),
m_spacerTabContent(NULL)
{
    
}

/**
 * Destructor.
 */
EventSpacerTabGet::~EventSpacerTabGet()
{
}

/**
 * @return The window index.
 */
int32_t
EventSpacerTabGet::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The row index.
 */
int32_t
EventSpacerTabGet::getRowIndex() const
{
    return m_rowIndex;
}

/**
 * @return The column index.
 */
int32_t
EventSpacerTabGet::getColumnIndex() const
{
    return m_columnIndex;
}

/**
 * @return The spacer tab.
 */
SpacerTabContent*
EventSpacerTabGet::getSpacerTabContent()
{
    return m_spacerTabContent;
}

/**
 * Set the spacer tab content.
 *
 * @param spacerTabContent
 *     New value for the spacer tab content.
 */
void
EventSpacerTabGet::setSpacerTabContent(SpacerTabContent* spacerTabContent)
{
    m_spacerTabContent = spacerTabContent;
}


