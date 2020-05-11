
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
 * Destructor.
 */
PaletteGroupStandardPalettes::~PaletteGroupStandardPalettes()
{
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
