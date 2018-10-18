
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

#define __SPACER_TAB_CONTENT_DECLARE__
#include "SpacerTabContent.h"
#undef __SPACER_TAB_CONTENT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SpacerTabContent 
 * \brief Content of a tile tabs 'spacer'
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param windowIndex
 *     Index of the window.
 * @param rowIndex
 *     Index of the row.
 * @param columnIndex
 *     Index of the column.
 */
SpacerTabContent::SpacerTabContent(const int32_t windowIndex,
                                   const int32_t rowIndex,
                                   const int32_t columnIndex)
: TabContentBase(),
m_key(windowIndex,
      rowIndex,
      columnIndex)
{
}

/**
 * Destructor.
 */
SpacerTabContent::~SpacerTabContent()
{
}

/**
 * @return Name of the tab.
 */
AString
SpacerTabContent::getTabName() const
{
    AString s("Spacer "
              + getTabNamePrefix());
    
    return s;
}

/**
 * @return Prefix name of the tab.
 */
AString
SpacerTabContent::getTabNamePrefix() const
{
    AString s("%1, %2");
    s = s.arg(m_key.m_rowIndex).arg(m_key.m_columnIndex);
    
    return s;
}

/**
 * @return The Window index
 */
int32_t
SpacerTabContent::getWindowIndex() const
{
    return m_key.m_windowIndex;
}

/**
 * @return The row index
 */
int32_t
SpacerTabContent::getRowIndex() const
{
    return m_key.m_rowIndex;
}

/**
 * @return The column index
 */
int32_t
SpacerTabContent::getColumnIndex() const
{
    return m_key.m_columnIndex;
}

/**
 * Constructor.
 *
 * @param windowIndex
 *     Index of the window.
 * @param rowIndex
 *     Index of the row.
 * @param columnIndex
 *     Index of the column.
 */
SpacerTabContent::MapKey::MapKey(const int32_t windowIndex,
                                 const int32_t rowIndex,
                                 const int32_t columnIndex)
: m_windowIndex(windowIndex),
m_rowIndex(rowIndex),
m_columnIndex(columnIndex)
{
    CaretAssert(m_windowIndex >= 0);
    CaretAssert(m_rowIndex >= 0);
    CaretAssert(m_columnIndex >= 0);
}

/**
 * Comparison operator.
 * 
 * @return True if 'this' is less than 'rhs', else false.
 */
bool
SpacerTabContent::MapKey::operator<(const MapKey& rhs) const
{
    if (m_windowIndex != rhs.m_windowIndex) {
        return (m_windowIndex < rhs.m_windowIndex);
    }
    if (m_rowIndex != rhs.m_rowIndex) {
        return (m_rowIndex < rhs.m_rowIndex);
    }
    if (m_columnIndex != rhs.m_columnIndex) {
        return (m_columnIndex < rhs.m_columnIndex);
    }
    
    return false;
}
