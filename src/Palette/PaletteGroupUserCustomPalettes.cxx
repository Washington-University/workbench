
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

#define __PALETTE_GROUP_USER_CUSTOM_PALETTES_DECLARE__
#include "PaletteGroupUserCustomPalettes.h"
#undef __PALETTE_GROUP_USER_CUSTOM_PALETTES_DECLARE__

#include "CaretAssert.h"
#include "CaretPreferences.h"

using namespace caret;


    
/**
 * \class caret::PaletteGroupUserCustomPalettes 
 * \brief Group for user custom palettes
 * \ingroup Palette
 */

/**
 * Constructor.
 * @param caretPreferences
 *   Pointer to caret preferences
 */
PaletteGroupUserCustomPalettes::PaletteGroupUserCustomPalettes(CaretPreferences* caretPreferences)
: PaletteGroup(PaletteGroup::GroupType::USER_CUSTOM)
{
    CaretAssert(caretPreferences);
    m_caretPreferences = caretPreferences;
}

/**
 * Destructor.
 */
PaletteGroupUserCustomPalettes::~PaletteGroupUserCustomPalettes()
{
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
PaletteGroupUserCustomPalettes::addPaletteImplementation(const PaletteNew& palette,
                                                         AString& errorMessageOut)
{
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
PaletteGroupUserCustomPalettes::replacePaletteImplementation(const PaletteNew& palette,
                                                             AString& errorMessageOut)
{
    return false;
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
PaletteGroupUserCustomPalettes::renamePaletteImplementation(const AString& paletteName,
                                                            const AString& newPaletteName,
                                                            AString& errorMessageOut)
{
    return false;
}

/**
 * Remove a palette.  Palette must contain a name that matches name of an existing palette
 * @param paletteName
 *     Name of palette that is removed
 * @param errorMessageOut
 *     Output with error message if failure to remove palette
 * @return True if palette was removed, else false.
 */
bool
PaletteGroupUserCustomPalettes::removePaletteImplementation(const AString& paletteName,
                                                            AString& errorMessageOut)
{
    return m_caretPreferences->paletteUserCustomRemove(paletteName,
                                                       errorMessageOut);
}

/**
 * Get the palettes
 * @param palettesOut
 *    Output containing all palettes in this group
 */
void
PaletteGroupUserCustomPalettes::getPalettes(std::vector<PaletteNew>& palettesOut) const
{
    palettesOut.clear();
    //    for (auto p : m_palettes) {
    //        palettesOut.push_back(p);
    //    }
}

/**
 * Get pointer to palette with the selected name
 * @param paletteName
 *    Name of palette
 * @return Pointer to palette or NULL if no palette with name exits else false
 */
std::unique_ptr<PaletteNew>
PaletteGroupUserCustomPalettes::getPaletteWithName(const AString& paletteName)
{
    std::unique_ptr<PaletteNew> paletteOut;
    //    /* Need reference to palettes to a pointer can be obtained */
    //    for (auto& p : m_palettes) {
    //        if (p.getName() == paletteName) {
    //            return &p;
    //            break;
    //        }
    //    }
    
    return paletteOut;
}

/**
 * @return True if a palette with the given name exists
 * @param paletteName
 *    Name of palette
 */
bool
PaletteGroupUserCustomPalettes::hasPaletteWithName(const AString& paletteName)
{
    return m_caretPreferences->paletteUserCustomExists(paletteName);
}

