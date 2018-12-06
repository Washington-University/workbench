
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

#define __WU_Q_MACRO_DECLARE__
#include "WuQMacro.h"
#undef __WU_Q_MACRO_DECLARE__

#include "CaretAssert.h"
#include "WuQMacroCommand.h"

using namespace caret;


    
/**
 * \class caret::WuQMacro 
 * \brief Contains a sequence of WuQMacroCommand's
 * \ingroup WuQMacro
 *
 */

/**
 * Constructor.
 */
WuQMacro::WuQMacro()
: CaretObjectTracksModification()
{
    
}

/**
 * Destructor.
 */
WuQMacro::~WuQMacro()
{
    clearCommands();
}

/**
 * Clear (remove) all commands in this macro
 */
void
WuQMacro::clearCommands()
{
    for (auto mc : m_macroCommands) {
        delete mc;
    }
    m_macroCommands.clear();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacro::WuQMacro(const WuQMacro& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperWuQMacro(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
WuQMacro&
WuQMacro::operator=(const WuQMacro& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperWuQMacro(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
WuQMacro::copyHelperWuQMacro(const WuQMacro& obj)
{
    clearCommands();
    
    for (auto mc : obj.m_macroCommands) {
        addMacroCommand(new WuQMacroCommand(*mc));
    }

    m_name = obj.m_name;
    m_description = obj.m_description;
    m_functionKey = obj.m_functionKey;
}

void
WuQMacro::addMacroCommand(WuQMacroCommand* macroCommand)
{
    m_macroCommands.push_back(macroCommand);
    setModified();
}

/**
 * @return Name of macro
 */
QString
WuQMacro::getName() const
{
    return m_name;
}

/**
 * Set name of macro
 *
 * @param name
 *    New name
 */
void
WuQMacro::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        setModified();
    }
}

/**
 * @return Description of macro
 */
QString
WuQMacro::getDescription() const
{
    return m_description;
}

/**
 * Set description of macro
 *
 * @param description
 *    New description
 */
void
WuQMacro::setDescription(const QString& description)
{
    if (m_description != description) {
        m_description = description;
        setModified();
    }
}

/**
 * @return The function key
 */
QString
WuQMacro::getFunctionKey() const
{
    return m_functionKey;
}

/**
 * Set the function key
 *
 * @param functionKey
 *    New function key
 */
void
WuQMacro::setFunctionKey(const QString& functionKey)
{
    if (m_functionKey != functionKey) {
        m_functionKey = functionKey;
        setModified();
    }
}

/**
 * @return The number of macro commands in this macro
 */
int32_t
WuQMacro::getNumberOfMacroCommands() const
{
    return m_macroCommands.size();
}

/**
 * @return
 *     The macro command at the given index
 * @param index
 *     Index of the macro command
 */
const WuQMacroCommand*
WuQMacro::getMacroCommandAtIndex(const int32_t index) const
{
    CaretAssertVectorIndex(m_macroCommands, index);
    return m_macroCommands[index];
}

/**
 * @return
 *     The macro command at the given index
 * @param index
 *     Index of the macro command
 */
WuQMacroCommand*
WuQMacro::getMacroCommandAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_macroCommands, index);
    return m_macroCommands[index];
}

/**
 * @return True if this instance is modified
 */
bool
WuQMacro::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    for (const auto mc : m_macroCommands) {
        if (mc->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status
 */
void
WuQMacro::clearModified()
{
    CaretObjectTracksModification::clearModified();
    
    for (auto mc : m_macroCommands) {
        mc->clearModified();
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WuQMacro::toString() const
{
    QString s("WuQMacro\n");
    
    s.append("Name=" + m_name + "\n");
    s.append("Description=" + m_description + "\n");
    s.append("Function Key=" + m_functionKey + "\n");
    
    for (auto mc : m_macroCommands) {
        s.append(mc->toString() + "\n");
    }
    
    return s;
}

