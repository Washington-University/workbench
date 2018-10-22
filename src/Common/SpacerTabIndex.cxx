
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

#define __SPACER_TAB_INDEX_DECLARE__
#include "SpacerTabIndex.h"
#undef __SPACER_TAB_INDEX_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::SpacerTabIndex 
 * \brief Index of a spacer tab in a tile tabs configuration
 * \ingroup Scenes
 */

/**
 * Constructor of invalid element.
 */
SpacerTabIndex::SpacerTabIndex()
: CaretObject(),
m_windowIndex(-1),
m_rowIndex(-1),
m_columnIndex(-1)
{
}

/**
 * Constructor with indices
 */
SpacerTabIndex::SpacerTabIndex(const int32_t windowIndex,
                               const int32_t rowIndex,
                               const int32_t columnIndex)
: CaretObject(),
m_windowIndex(windowIndex),
m_rowIndex(rowIndex),
m_columnIndex(columnIndex)
{
}

/**
 * Destructor.
 */
SpacerTabIndex::~SpacerTabIndex()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SpacerTabIndex::SpacerTabIndex(const SpacerTabIndex& obj)
: CaretObject(obj)
{
    this->copyHelperSpacerTabIndex(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SpacerTabIndex&
SpacerTabIndex::operator=(const SpacerTabIndex& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperSpacerTabIndex(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SpacerTabIndex::copyHelperSpacerTabIndex(const SpacerTabIndex& obj)
{
    m_windowIndex = obj.m_windowIndex;
    m_rowIndex    = obj.m_rowIndex;
    m_columnIndex = obj.m_columnIndex;
}

/**
 * Inequality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return
 *    True if this instance and 'obj' instance are considered NOT equal.
 */
bool
SpacerTabIndex::operator!=(const SpacerTabIndex& obj) const
{
    return ( ! (*this == obj));
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
SpacerTabIndex::operator==(const SpacerTabIndex& obj) const
{
    if (this == &obj) {
        return true;    
    }

    if ((m_windowIndex == obj.m_windowIndex)
        && (m_rowIndex == obj.m_rowIndex)
        && (m_columnIndex == obj.m_columnIndex)) {
        return true;
    }
    
    return false;    
}

/**
 * Comparison operator.
 *
 * @param rhs
 *     Other instance for comparison
 * @return True if 'this' is less than 'rhs', else false.
 */
bool
SpacerTabIndex::operator<(const SpacerTabIndex& rhs) const
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

/**
 * @return True if the index is valid, else false.
 */
bool
SpacerTabIndex::isValid() const
{
    if ((m_windowIndex >= 0)
        && (m_rowIndex >= 0)
        && (m_columnIndex >= 0)) {
        return true;
    }
    
    return false;
}


/**
 * @return The window index
 */
int32_t
SpacerTabIndex::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The row index
 */
int32_t
SpacerTabIndex::getRowIndex() const
{
    return m_rowIndex;
}

/**
 * @return The column index
 */
int32_t
SpacerTabIndex::getColumnIndex() const
{
    return m_columnIndex;
}

/**
 * Reset to invalid indices.
 */
void
SpacerTabIndex::reset()
{
    m_windowIndex = -1;
    m_rowIndex    = -1;
    m_columnIndex = -1;
}


/**
 * @return Format for XML attribute (window index, row index,
 * and column index separated by commas).  
 * NOTE: THESE INDICES START AT ZERO
 */
AString
SpacerTabIndex::getXmlAttributeText() const
{
    AString s("%1,%2,%3");
    s = s.arg(m_windowIndex).arg(m_rowIndex).arg(m_columnIndex);
    return s;
}

/**
 * Set from XML text in the form (w,r,c) where 'w' is the
 * window index, 'r' is the row index, and 'c' is the 
 * column index.
 * NOTE: THESE INDICES START AT ZERO
 */
void
SpacerTabIndex::setFromXmlAttributeText(const AString& text)
{
    reset();
    
    if ( ! text.isEmpty()) {
        std::vector<int32_t> indices;
        AString::toNumbers(text, indices);
        if (indices.size() >= 3) {
            m_windowIndex = indices[0];
            m_rowIndex    = indices[1];
            m_columnIndex = indices[2];
        }
    }
}

/**
 * @return Row and column in text form (eg: "Row=2, Column=3")
 * for use in the GUI and viewed by user.
 * NOTE: THESE INDICES START AT ONE
 */
AString
SpacerTabIndex::getRowColumnGuiText() const
{
    AString s("Row=%1, Column=%2");
    s = s.arg(m_rowIndex+1).arg(m_columnIndex+1);
    return s;
}

/**
 * @return Window, row, and column in text form (eg: "Window=1, Row=2, Column=3")
 * for use in the GUI and viewed by user.
 * NOTE: THESE INDICES START AT ONE
 */
AString
SpacerTabIndex::getWindowRowColumnGuiText() const
{
    AString s("Window=%1, ");
    s = s.arg(m_windowIndex+1);
    s.append(getRowColumnGuiText());
    
    return s;
}



/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 * NOTE: THESE INDICES START AT ZERO
 */
AString 
SpacerTabIndex::toString() const
{
    AString s("SpacerTabIndex: windowIndex=%1, rowIndex=%2, columnIndex=%3");
    s = s.arg(m_windowIndex).arg(m_rowIndex).arg(m_columnIndex);

    return s;
}

