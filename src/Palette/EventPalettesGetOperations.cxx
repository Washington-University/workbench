
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __EVENT_PALETTES_GET_OPERATIONS_DECLARE__
#include "EventPalettesGetOperations.h"
#undef __EVENT_PALETTES_GET_OPERATIONS_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventTypeEnum.h"
#include "PaletteBase.h"

using namespace caret;


    
/**
 * \class caret::EventPalettesGetOperations 
 * \brief Event to get a palette or all palettes
 * \ingroup Palette
 */

/**
 * Constructor for the given operation
 * @param operation
 *    The palette get operation
 */
EventPalettesGetOperations::EventPalettesGetOperations(const Operation operation)
: Event(EventTypeEnum::EVENT_PALETTES_GET_OPERATION),
m_operation(operation)
{
    
}

/**
 * Destructor.
 */
EventPalettesGetOperations::~EventPalettesGetOperations()
{
}

/**
 * @return The operation
 */
EventPalettesGetOperations::Operation
EventPalettesGetOperations::getOperation() const
{
    return m_operation;
}

/**
 * @return All PaletteNew type palettes
 */
std::vector<const PaletteNew*>
EventPalettesGetOperations::getAllPaletteNewSortedByName()
{
    std::vector<const PaletteBase*> palettes(getAllPalettesSortedByName(false,
                                                                        true));
    std::vector<const PaletteNew*> paletteNews;
    for (const PaletteBase* pb : palettes) {
        const PaletteNew* pn(pb->castToPaletteNew());
        if (pn != NULL) {
            paletteNews.push_back(pn);
        }
    }
    
    return paletteNews;
}

/**
 * @return All palette types sorted by name
 */
std::vector<const PaletteBase*>
EventPalettesGetOperations::getAllPaletteTypesSortedByName()
{
    return getAllPalettesSortedByName(true,
                                      true);
}

/**
 * @return All palettes of the enabled types sorted by name
 * @param includePaletteFlag
 *    Include 'Palette" instances
 * @param includePaletteNewFlag
 *    Include 'PaletteNew" instances
 */
std::vector<const PaletteBase*>
EventPalettesGetOperations::getAllPalettesSortedByName(const bool includePaletteFlag,
                                                       const bool includePaletteNewFlag)
{
    std::vector<const PaletteBase*> palettes;
    std::vector<const PaletteBase*> paletteNews;

    EventPalettesGetOperations event(Operation::GET_ALL_PALETTES);
    EventManager::get()->sendEvent(event.getPointer());
    for (const PaletteBase* p : event.m_palettes) {
        switch (p->getPaletteDesignType()) {
            case PaletteDesignTypeEnum::PALETTE:
                if (includePaletteFlag) {
                    palettes.push_back(p);
                }
                break;
            case PaletteDesignTypeEnum::PALETTE_NEW:
                if (includePaletteNewFlag) {
                    paletteNews.push_back(p);
                }
                break;
        }
    }
    
    PaletteBase::sortByName(palettes);
    PaletteBase::sortByName(paletteNews);

    std::vector<const PaletteBase*> palettesOut;
    palettesOut.insert(palettesOut.end(), paletteNews.begin(), paletteNews.end());
    palettesOut.insert(palettesOut.end(), palettes.begin(), palettes.end());
    
    return palettesOut;
}

/**
 * @return Palettes with the given name or NULL if not found
 * @param name
 *    Name of the palette
 */
const PaletteBase*
EventPalettesGetOperations::getPaletteWithName(const AString& name)
{
    const PaletteBase* paletteOut(NULL);
    
    EventPalettesGetOperations event(Operation::GET_PALETTE_WITH_NAME);
    event.m_paletteName = name;
    EventManager::get()->sendEvent(event.getPointer());
    
    if (event.m_palettes.size() == 1) {
        paletteOut = event.m_palettes[0];
    }
    
    return paletteOut;
}

void
EventPalettesGetOperations::addPalette(const PaletteBase* palette)
{
    m_palettes.push_back(palette);
}

/**
 * @return Name of the palette
 */
AString
EventPalettesGetOperations::getPaletteName() const
{
    return m_paletteName;
}





