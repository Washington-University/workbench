
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

#define __WU_Q_MACRO_GROUP_DECLARE__
#include "WuQMacroGroup.h"
#undef __WU_Q_MACRO_GROUP_DECLARE__

#include "CaretAssert.h"
#include "WuQMacro.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroGroup 
 * \brief Contains a group of macros
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param groupName
 *     Name of group
 */
WuQMacroGroup::WuQMacroGroup(const QString& groupName)
: CaretObjectTracksModification(),
m_groupName(groupName)
{
    
}

/**
 * Destructor.
 */
WuQMacroGroup::~WuQMacroGroup()
{
    for (auto m : m_macros) {
        delete m;
    }
    m_macros.clear();
}

/**
 * @return Name of group
 */
QString
WuQMacroGroup::getGroupName() const
{
    return m_groupName;
}

/**
 * Set name of group
 *
 * @param groupName
 *     Name of group
 */
void
WuQMacroGroup::setGroupName(const QString& groupName)
{
    if (m_groupName != groupName) {
        m_groupName = groupName;
        setModified();
    }
}

/**
 * Add a macro
 *  
 * @param macro
 *     Macro added to this group.  This group will take ownership of the macro.
 */
void
WuQMacroGroup::addMacro(WuQMacro* macro)
{
    m_macros.push_back(macro);
    setModified();
}

/**
 * @return Number of macros in group
 */
int32_t
WuQMacroGroup::getNumberOfMacros() const
{
    return m_macros.size();
}

/**
 * Get the macro with the given name
 *
 * @param name
 *     Name of the macro
 * @return
 *     Pointer to macro with the name or NULL if not found
 */
WuQMacro*
WuQMacroGroup::getMacroByName(const QString& name)
{
    for (auto m : m_macros) {
        if (m->getName() == name) {
            return m;
        }
    }
    return NULL;
}

/**
 * Get the macro with the given name
 *
 * @param name
 *     Name of the macro
 * @return
 *     Pointer to macro with the name or NULL if not found
 */
const WuQMacro*
WuQMacroGroup::getMacroByName(const QString& name) const
{
    for (auto m : m_macros) {
        if (m->getName() == name) {
            return m;
        }
    }
    return NULL;
}

/**
 * @return
 *    Macro at the given index
 * @param index
 *    Index of the macro
 */
WuQMacro*
WuQMacroGroup::getMacroAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_macros, index);
    return m_macros[index];
}

/**
 * @return
 *    Macro at the given index
 * @param index
 *    Index of the macro
 */
const WuQMacro*
WuQMacroGroup::getMacroAtIndex(const int32_t index) const
{
    CaretAssertVectorIndex(m_macros, index);
    return m_macros[index];
}

/**
 * Delete the given macro from this group
 *
 * @param macro
 *     Macro for deletion
 */
void
WuQMacroGroup::deleteMacro(const WuQMacro* macro)
{
    const int32_t count = getNumberOfMacros();
    for (int32_t i = 0; i < count; i++) {
        if (getMacroAtIndex(i) == macro) {
            deleteMacroAtIndex(i);
            break;
        }
    }
}

/**
 * Delete the macro at the given index
 *
 * @param index
 *     Index of macro for deletion
 */
void
WuQMacroGroup::deleteMacroAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_macros, index);
    WuQMacro* macro = m_macros[index];
    m_macros.erase(m_macros.begin() + index);
    delete macro;
    
    setModified();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WuQMacroGroup::toString() const
{
    QString s("WuQMacroGroup name=" + m_groupName + "\n");
    for (auto m : m_macros) {
        s.append(m->toString() + "\n");
    }
    return s;
}

