
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __EVENT_PALETTE_GET_BY_NAME_DECLARE__
#include "EventPaletteGetByName.h"
#undef __EVENT_PALETTE_GET_BY_NAME_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventPaletteGetByName 
 * \brief Find a palette by name
 * \ingroup Common
 */

/**
 * Constructor.
 */
EventPaletteGetByName::EventPaletteGetByName(const AString& paletteName)
: Event(EventTypeEnum::EVENT_PALETTE_GET_BY_NAME),
m_paletteName(paletteName),
m_palette(NULL)
{
}

/**
 * Destructor.
 */
EventPaletteGetByName::~EventPaletteGetByName()
{
}

/**
 * @return Name of desired palette.
 */
AString
EventPaletteGetByName::getPaletteName() const
{
    return m_paletteName;
}

/**
 * @return Palette that was found (NULL if no matching palette was found).
 */
Palette*
EventPaletteGetByName::getPalette() const
{
    return m_palette;
}

/**
 * Set the palette that matches by name.
 *
 * @param palette
 *    Palette that matches name of desired palette.
 */
void
EventPaletteGetByName::setPalette(Palette* palette)
{
    CaretAssert(palette);
    
    if (m_palette != NULL) {
        CaretLogWarning("There appears to be more than one palette with name \""
                        + m_paletteName
                        + "\"");
    }
    
    m_palette = palette;
}

