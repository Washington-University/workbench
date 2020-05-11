
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PALETTE_GROUP_DECLARE__
#include "PaletteGroup.h"
#undef __PALETTE_GROUP_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
using namespace caret;


    
/**
 * \class caret::PaletteGroup 
 * \brief Contains a group of palettes
 * \ingroup Palette
 */

/**
 * Constructor creates an empty, editable palette group.
 *
 * @param groupType
 * Type of group containing palettes
 */
PaletteGroup::PaletteGroup(const GroupType groupType)
: CaretObject(),
m_groupType(groupType)
{
    switch (groupType) {
        case GroupType::DATA_FILE:
            CaretLogSevere("Palettes in data file not supported yet");
            return;
            break;
        case GroupType::STANDARD:
            m_groupName = "Standard";
            break;
        case GroupType::USER_CUSTOM:
            m_groupName = "User Custom";
            break;
    }
}

/**
 * Destructor.
 */
PaletteGroup::~PaletteGroup()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PaletteGroup::toString() const
{
    return "PaletteGroup";
}

/**
 * @return The group type
 */
PaletteGroup::GroupType
PaletteGroup::getGroupType() const
{
    return m_groupType;
}

/**
 * @return Name of group
 */
AString
PaletteGroup::getGroupName() const
{
    return m_groupName;
}

/**
 * @return True if this container is editable
 */
bool
PaletteGroup::isEditable() const
{
    return m_editableFlag;
}

/**
 * Set the editable status of this container.  Subclasses that are not editable
 * use this method to disable the editable status.
 *
 * @param status
 *  New editable status
 */
void
PaletteGroup::setEditable(const bool status)
{
    m_editableFlag = status;
}

/**
 * Add a palette.  Palette must contain a name not used by an existing palette in this group.
 * @param palette
 *     Palette to add
 * @param errorMessageOut
 *     Output with error message if failure to add palette
 * @return True if palette was added, else false.
 */
bool
PaletteGroup::addPalette(const PaletteNew& palette,
                         AString& errorMessageOut)
{
    if ( ! m_editableFlag) {
        errorMessageOut = ("Adding palettes to " + m_groupName + " group not allowed");
        return false;
    }
    
    const AString paletteName = palette.getName();
    if (paletteName.trimmed().isEmpty()) {
        errorMessageOut = "Palette must have a name";
        return false;
    }
    
    /*
     * Try to insert palette.
     * Second element in pair is true if successful
     * and false if a palette with same name is in container.
     */
    std::pair<ContainerIterator, bool> result = m_palettes.insert(palette);
    if (result.second) {
        return true;
    }
    
    errorMessageOut = ("Cannot add palette with name "
                       + paletteName
                       + ".  Palette with name exists.");
    return false;
}

/**
 * Replace a palette.  Palette must contain a name that matches name of an existing palette
 * @param palette
 *     Palette used as a replacement
 * @param errorMessageOut
 *     Output with error message if failure to replace palette
 * @return True if palette was replaced, else false.
 */
bool
PaletteGroup::replacePalette(const PaletteNew& palette,
                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! m_editableFlag) {
        errorMessageOut = ("Replacing palettes in " + m_groupName + " group not allowed");
        return false;
    }

    const AString paletteName = palette.getName();
    if (paletteName.trimmed().isEmpty()) {
        errorMessageOut = "Palette must have a name";
        return false;
    }
    
    auto findIter = m_palettes.find(palette);
    if (findIter != m_palettes.end()) {
        /*
         * Must remove before inserting
         */
        m_palettes.erase(findIter);
        
        std::pair<ContainerIterator, bool> result = m_palettes.insert(palette);
        if ( ! result.second) {
            const AString msg("Failed to insert new palette when replacing");
            CaretLogSevere(msg);
            CaretAssertMessage(0, msg);
        }
    }
    else {
        errorMessageOut = ("No palette with name "
                           + paletteName
                           + "was found for replacing.");
        return false;
    }
    
    return true;
}

/**
 * Rename a palette.  Palette must contain a name that matches name of an existing palette
 * @param paletteName
 *     Name of palette that is renamed
 * @param newPaletteName
 *     New name for palette
 * @param errorMessageOut
 *     Output with error message if failure to rename palette
 * @return True if palette was rename, else false.
 */
bool
PaletteGroup::renamePalette(const AString& paletteName,
                            const AString& newPaletteName,
                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (getPaletteWithName(newPaletteName) != NULL) {
        errorMessageOut = ("Palette with name "
                           + newPaletteName
                           + " exists.  Palette names must be unique.");
        return false;
    }

    for (ContainerIterator iter = m_palettes.begin();
         iter != m_palettes.end();
         iter++) {
        if (iter->getName() == paletteName) {
            PaletteNew palette = *iter;
            palette.setName(newPaletteName);
            
            m_palettes.erase(iter);
            
            return addPalette(palette,
                              errorMessageOut);
        }
    }
    
    /*
     * Should not get here
     */
    errorMessageOut = "Failed find palette for renaming..";
    return false;
}

/**
 * Remove a palette.  Palette must contain a name that matches name of an existing palette
 * @param paletteName
 *     Name of palette that is removed
 * @param errorMessageOut
 *     Output with error message if failure to remove palette
 * @return True if palette was removed, else false.
 */bool
PaletteGroup::removePalette(const AString& paletteName,
                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! m_editableFlag) {
        errorMessageOut = ("Removing palettes in " + m_groupName + " group not allowed");
        return false;
    }
    
    for (auto iter = m_palettes.begin();
         iter != m_palettes.end();
         iter++) {
        if (iter->getName() == paletteName) {
            m_palettes.erase(iter);
            return true;
        }
    }
    
    errorMessageOut = ("No palette with name "
                           + paletteName
                           + "was found for removing.");
    return false;
}


/**
 * Get the palettes
 * @param palettesOut
 *    Output containing all palettes in this group
 */
void
PaletteGroup::getPalettes(std::vector<PaletteNew>& palettesOut) const
{
    palettesOut.clear();
    for (auto p : m_palettes) {
        palettesOut.push_back(p);
    }
}

/**
 * Get pointer to palette with the selected name
 * @param paletteName
 *    Name of palette
 * @return Pointer to palette or NULL if no palette with name exits
 */
const PaletteNew*
PaletteGroup::getPaletteWithName(const AString& paletteName)
{
    /* Need reference to palettes to a pointer can be obtained */
    for (auto& p : m_palettes) {
        if (p.getName() == paletteName) {
            return &p;
            break;
        }
    }
    
    return NULL;
}

