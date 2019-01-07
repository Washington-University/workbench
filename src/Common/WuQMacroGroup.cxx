
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

#include <QUuid>

#include "CaretAssert.h"
#include "WuQMacro.h"
#include "WuQMacroGroupXmlReader.h"
#include "WuQMacroGroupXmlWriter.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroGroup 
 * \brief Contains a group of macros
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param name
 *     Name of group
 */
WuQMacroGroup::WuQMacroGroup(const QString& name)
: QStandardItemModel(), TracksModificationInterface(),
m_name(name)
{
    m_uniqueIdentifier = QUuid::createUuid().toString();
}

/**
 * Destructor.
 */
WuQMacroGroup::~WuQMacroGroup()
{
    clearPrivate();
}

/**
 * Clear (remove all macros)
 */
void
WuQMacroGroup::clearPrivate()
{
    /* Note: Do not clear unique identifier */
}

/**
 * @return Name of group
 */
QString
WuQMacroGroup::getName() const
{
    return m_name;
}

/**
 * Set name of group
 *
 * @param name
 *     Name of group
 */
void
WuQMacroGroup::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        setModified();
    }
}

/**
 * @return The unique identifier
 */
QString
WuQMacroGroup::getUniqueIdentifier() const
{
    return m_uniqueIdentifier;
}

/**
 * Set unique identifier of macro
 *
 * @param uniqueIdentifier
 *    New unique identifier
 */void
WuQMacroGroup::setUniqueIdentifier(const QString& uniqueIdentifier)
{
    if (uniqueIdentifier.isEmpty()) {
        return;
    }
    if (m_uniqueIdentifier != uniqueIdentifier) {
        m_uniqueIdentifier = uniqueIdentifier;
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
    appendRow(macro);
    setModified();
}

/**
 * Append macros in the given group to this group
 *
 * @param macroGroup
 *     The macro group whose macros are appended to this macro group
 */
void
WuQMacroGroup::appendMacroGroup(const WuQMacroGroup* macroGroup)
{
    const int32_t numMacros = macroGroup->getNumberOfMacros();
    for (int32_t i = 0; i < numMacros; i++) {
        const WuQMacro* macro = macroGroup->getMacroAtIndex(i);
        CaretAssert(macro);
        appendRow(new WuQMacro(*macro));
    }
    setModified();
}

/**
 * @return Number of macros in group
 */
int32_t
WuQMacroGroup::getNumberOfMacros() const
{
    return rowCount();
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
    const int32_t numItems = rowCount();
    for (int32_t i = 0; i < numItems; i++) {
        WuQMacro* macro = getMacroAtIndex(i);
        if (macro->getName() == name) {
            return macro;
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
    const int32_t numItems = rowCount();
    for (int32_t i = 0; i < numItems; i++) {
        const WuQMacro* macro = getMacroAtIndex(i);
        if (macro->getName() == name) {
            return macro;
        }
    }
    
    return NULL;
}

/**
 * Get the macro with the given unique identifier
 *
 * @param uniqueIdentifier
 *     Unique identifier of the macro
 * @return
 *     Pointer to macro with the unique identifier or NULL if not found
 */
WuQMacro*
WuQMacroGroup::getMacroWithUniqueIdentifier(const QString& uniqueIdentifier)
{
    const int32_t numItems = rowCount();
    for (int32_t i = 0; i < numItems; i++) {
        WuQMacro* macro = getMacroAtIndex(i);
        if (macro->getUniqueIdentifier() == uniqueIdentifier) {
            return macro;
        }
    }

    return NULL;
}

/**
 * Get the macro with the given unique identifier (const method)
 *
 * @param uniqueIdentifier
 *     Unique identifier of the macro
 * @return
 *     Pointer to macro with the unique identifier or NULL if not found
 */
const WuQMacro*
WuQMacroGroup::getMacroWithUniqueIdentifier(const QString& uniqueIdentifier) const
{
    const int32_t numItems = rowCount();
    for (int32_t i = 0; i < numItems; i++) {
        const WuQMacro* macro = getMacroAtIndex(i);
        if (macro->getUniqueIdentifier() == uniqueIdentifier) {
            return macro;
        }
    }
    
    return NULL;
}

/**
 * Get the macro with the given short cut key
 *
 * @param name
 *     Name of the macro
 * @return
 *     Pointer to macro with the short cut key or NULL if not found
 */
WuQMacro*
WuQMacroGroup::getMacroWithShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey)
{
    const int32_t numItems = rowCount();
    for (int32_t i = 0; i < numItems; i++) {
        WuQMacro* macro = getMacroAtIndex(i);
        if (macro->getShortCutKey() == shortCutKey) {
            return macro;
        }
    }

    return NULL;
}

/**
 * Get the macro with the given short cut key
 *
 * @param name
 *     Name of the macro
 * @return
 *     Pointer to macro with the short cut key or NULL if not found
 */
const WuQMacro*
WuQMacroGroup::getMacroWithShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey) const
{
    const int32_t numItems = rowCount();
    for (int32_t i = 0; i < numItems; i++) {
        const WuQMacro* macro = getMacroAtIndex(i);
        if (macro->getShortCutKey() == shortCutKey) {
            return macro;
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
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* standardItem = item(index);
    CaretAssert(standardItem);
    WuQMacro* macro = dynamic_cast<WuQMacro*>(standardItem);
    CaretAssert(macro);
    return macro;
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
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* standardItem = item(index);
    CaretAssert(standardItem);
    WuQMacro* macro = dynamic_cast<WuQMacro*>(standardItem);
    CaretAssert(macro);
    return macro;

}

/**
 * @return True if this macro group contains the given macro
 *
 * @param macro
 *     Macro for inclusion testing
 */
bool
WuQMacroGroup::containsMacro(const WuQMacro* macro)
{
    const int32_t count = getNumberOfMacros();
    for (int32_t i = 0; i < count; i++) {
        if (getMacroAtIndex(i) == macro) {
            return true;
        }
    }
    
    return false;
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
    /*
     * Note that 'takeItem' removes the item and the
     * row becomes NULL so also need to remove the row.
     */
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* item = takeItem(index);
    CaretAssert(item);
    removeRow(index);
    delete item;
    
    setModified();
}

/**
 * @return True if this instance is modified
 */
bool
WuQMacroGroup::isModified() const
{
    if (m_modifiedStatusFlag) {
        return true;
    }
    
    
    const int32_t count = getNumberOfMacros();
    for (int32_t i = 0; i < count; i++) {
        if (getMacroAtIndex(i)->isModified()) {
            break;
        }
    }

    return false;
}

/**
 * Clear the modified status
 */
void
WuQMacroGroup::clearModified()
{
    m_modifiedStatusFlag = false;
    
    const int32_t count = getNumberOfMacros();
    for (int32_t i = 0; i < count; i++) {
        getMacroAtIndex(i)->clearModified();
    }
}

/**
 * Set the modification status to modified
 */
void
WuQMacroGroup::setModified()
{
    m_modifiedStatusFlag = true;
}


/**
 * Read from a string containing XML.  If successful,
 * the modified status is cleared.
 *
 * @param xmlString
 *     String containing XML
 * @param errorMessageOut
 *     Contains error information if reading fails
 * @param nonFatalWarningMessageOut
 *     May contain non-fatal warnings when reading is successful
 * @return
 *     True if successful, else false
 */
bool
WuQMacroGroup::readXmlFromStringOld(const QString& xmlString,
                                 QString& errorMessageOut,
                                 QString& nonFatalWarningMessageOut)
{
    errorMessageOut.clear();
    nonFatalWarningMessageOut.clear();
    
    WuQMacroGroupXmlReader reader;
    reader.readFromString(xmlString,
                          this);
    if (reader.hasError()) {
        errorMessageOut = reader.getErrorMessage();
        return false;
    }
    else {
        if (reader.hasWarnings()) {
            nonFatalWarningMessageOut = reader.getWarningMessage();
        }
    }
    clearModified();
    
    return true;
}

/**
 * Write to a string containing XML.  If successful,
 * the modified status is cleared.
 *
 * @param xmlString
 *     String to which XML is written
 * @param errorMessageOut
 *     Contains error information if reading fails
 * @return
 *     True if successful, else false
 */
bool
WuQMacroGroup::writeXmlToString(QString& xmlString,
                                QString& errorMessageOut)
{
    xmlString.clear();
    errorMessageOut.clear();
    
    WuQMacroGroupXmlWriter writer;
    writer.writeToString(this,
                         xmlString);
    clearModified();
    
    return true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WuQMacroGroup::toString() const
{
    QString s("WuQMacroGroup name=" + m_name + "\n");
    const int32_t count = getNumberOfMacros();
    for (int32_t i = 0; i < count; i++) {
        s.append(getMacroAtIndex(i)->toString() + "\n");
    }
    return s;
}

