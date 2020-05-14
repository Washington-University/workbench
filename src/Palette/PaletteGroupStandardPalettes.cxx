
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

#define __PALETTE_GROUP_STANDARD_PALETTES_DECLARE__
#include "PaletteGroupStandardPalettes.h"
#undef __PALETTE_GROUP_STANDARD_PALETTES_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
    
/**
 * \class caret::PaletteGroupStandardPalettes
 * \brief Palette group that contains the standard palettes (built-in)
 * \ingroup Palette
 */

/**
 * Constructor.
 */
PaletteGroupStandardPalettes::PaletteGroupStandardPalettes()
: PaletteGroup(GroupType::STANDARD)
{
}

/**
 * Destructor.
 */
PaletteGroupStandardPalettes::~PaletteGroupStandardPalettes()
{
}

/**
 * Call after constructor to load palettes.  Uses some virtual methods
 * so cannot call from constructor.
 */
void
PaletteGroupStandardPalettes::loadPalettes()
{
    m_palettes.clear();
    
    /*
     * Some simulated palettes for now
     */
    {
        std::vector<PaletteNew::ScalarColor> posRange;
        posRange.push_back(PaletteNew::ScalarColor(0.0, 0.4, 0.0, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(0.3, 0.7, 0.0, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(1.0, 0.1, 0.0, 0.0));
        
        std::vector<PaletteNew::ScalarColor> negRange;
        negRange.push_back(PaletteNew::ScalarColor(-1.0, 0.0, 0.4, 0.9));
        negRange.push_back(PaletteNew::ScalarColor(-0.6, 0.7, 0.0, 0.7));
        negRange.push_back(PaletteNew::ScalarColor( 0.0, 0.1, 0.0, 0.3));
        
        float zeroRGB[3] = { 0.0, 1.0, 0.0 };
        PaletteNew p(posRange, zeroRGB, negRange);
        p.setName("Psycho");
        addPaletteHelper(p);
    }
    
    {
        std::vector<PaletteNew::ScalarColor> posRange;
        posRange.push_back(PaletteNew::ScalarColor(0.0, 0.4, 0.7, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(0.5, 0.7, 0.7, 0.0));
        posRange.push_back(PaletteNew::ScalarColor(1.0, 0.1, 0.0, 0.0));
        
        std::vector<PaletteNew::ScalarColor> negRange;
        negRange.push_back(PaletteNew::ScalarColor(-1.0, 0.0, 0.5, 0.9));
        negRange.push_back(PaletteNew::ScalarColor(-0.2, 0.7, 0.0, 0.7));
        negRange.push_back(PaletteNew::ScalarColor( 0.0, 0.1, 0.0, 0.3));
        
        float zeroRGB[3] = { 0.5, 1.0, 0.0 };
        PaletteNew p(posRange, zeroRGB, negRange);
        p.setName("Roy");
        addPaletteHelper(p);
    }
}

/**
 * Add the given palette and log any errors
 * @param palette
 * Palette to add
 */
void
PaletteGroupStandardPalettes::addPaletteHelper(const PaletteNew& palette)
{
    setEditable(true);
    
    AString errorMessage;
    const bool result = addPalette(palette,
                                   errorMessage);
    if ( ! result) {
        CaretLogSevere("Creating Standard Palette named \""
                       + palette.getName()
                       + "\": "
                       + errorMessage);
    }
    
    setEditable(false);
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
PaletteGroupStandardPalettes::addPaletteImplementation(const PaletteNew& palette,
                                                         AString& errorMessageOut)
{
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
                       + palette.getName()
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
PaletteGroupStandardPalettes::replacePaletteImplementation(const PaletteNew& palette,
                                                             AString& errorMessageOut)
{
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
            errorMessageOut = msg;
            return false;
        }
    }

    errorMessageOut = ("No palette with name "
                       + palette.getName()
                       + "was found for replacing.");
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
PaletteGroupStandardPalettes::renamePaletteImplementation(const AString& paletteName,
                                                            const AString& newPaletteName,
                                                            AString& errorMessageOut)
{
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
    
    errorMessageOut = ("Palette for renaming with name "
                       + newPaletteName
                       + " does not exist.");
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
PaletteGroupStandardPalettes::removePaletteImplementation(const AString& paletteName,
                                                            AString& errorMessageOut)
{
    for (auto iter = m_palettes.begin();
         iter != m_palettes.end();
         iter++) {
        if (iter->getName() == paletteName) {
            m_palettes.erase(iter);
            return true;
        }
    }
    
    errorMessageOut = ("No palette with name \""
                       + paletteName
                       + "\" was found for removing.");
    return false;
}

/**
 * Get the palettes
 * @param palettesOut
 *    Output containing all palettes in this group
 */
void
PaletteGroupStandardPalettes::getPalettes(std::vector<PaletteNew>& palettesOut) const
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
std::unique_ptr<PaletteNew>
PaletteGroupStandardPalettes::getPaletteWithName(const AString& paletteName)
{
    std::unique_ptr<PaletteNew> paletteOut;
    
    /* Need reference to palettes to a pointer can be obtained */
    for (auto& p : m_palettes) {
        if (p.getName() == paletteName) {
            paletteOut.reset(new PaletteNew(p));
            break;
        }
    }

    return paletteOut;
}

/**
 * @return True if a palette with the given name exists, else false
 * @param paletteName
 *    Name of palette
 */
bool
PaletteGroupStandardPalettes::hasPaletteWithName(const AString& paletteName)
{
    /* Need reference to palettes to a pointer can be obtained */
    for (auto& p : m_palettes) {
        if (p.getName() == paletteName) {
            return true;
            break;
        }
    }
    
    return false;
}
