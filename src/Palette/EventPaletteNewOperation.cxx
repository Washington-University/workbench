
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

#define __EVENT_PALETTE_NEW_OPERATION_DECLARE__
#include "EventPaletteNewOperation.h"
#undef __EVENT_PALETTE_NEW_OPERATION_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventTypeEnum.h"
#include "PaletteNew.h"

using namespace caret;


    
/**
 * \class caret::EventPaletteNewOperation 
 * \brief Event for various palette operations
 * \ingroup Palette
 */

/**
 * Constructor.
 * @param operation
 *    The operation for this event
 */
EventPaletteNewOperation::EventPaletteNewOperation(const Operation operation)
: Event(EventTypeEnum::EVENT_PALETTE_NEW_OPERATION),
m_operation(operation)
{
    
}

/**
 * Destructor.
 */
EventPaletteNewOperation::~EventPaletteNewOperation()
{
}

/**
 * @return The operation for this event
 */
EventPaletteNewOperation::Operation
EventPaletteNewOperation::getOperation() const
{
    return m_operation;
}

/**
 * @return Pointer to palette with the given name or NULL if not found.
 * Failure to find palette with name is not an error.
 */
const PaletteNew*
EventPaletteNewOperation::getPaletteWithName(const AString& name)
{
    EventPaletteNewOperation event(Operation::GET_PALETTE_WITH_NAME);
    event.m_paletteName = name;
    EventManager::get()->sendEvent(event.getPointer());
    
    const PaletteNew* paletteOut(NULL);
    if ( ! event.m_palettes.empty()) {
        CaretAssertVectorIndex(event.m_palettes, 0);
        paletteOut = event.m_palettes[0];
    }
    return paletteOut;
}

/**
 * @return Pointers to the user's palettes.  No palettes is valid if none have been created by user.
 */
FunctionResultValue<std::vector<const PaletteNew*>>
EventPaletteNewOperation::getUserPalettes()
{
    EventPaletteNewOperation event(Operation::GET_USER_PALETTES);
    EventManager::get()->sendEvent(event.getPointer());
    
    return FunctionResultValue<std::vector<const PaletteNew*>>(event.m_palettes,
                                                         "",
                                                         true);
}

/**
 * Add the given palette to the user's palettes.
 * Ownership is take of palette.
 */
FunctionResult
EventPaletteNewOperation::addPalette(PaletteNew* palette)
{
    EventPaletteNewOperation event(Operation::ADD_PALETTE);
    std::vector<const PaletteNew*> palettes { palette };
    event.setPalettes(palettes);
    EventManager::get()->sendEvent(event.getPointer());
    
    return FunctionResult(event.getErrorMessage(),
                          event.getErrorMessage().isEmpty());
}

/**
 * @return A palette read from the given file
 * @param filenamne
 *    Name of file
 */
FunctionResultValue<const PaletteNew*>
EventPaletteNewOperation::readPalette(const AString& filename)
{
    EventPaletteNewOperation event(Operation::READ_PALETTE);
    event.m_filename = filename;
    EventManager::get()->sendEvent(event.getPointer());
    
    AString errorMessage;
    const PaletteNew* paletteOut(NULL);
    if (event.m_palettes.empty()) {
        errorMessage = event.getErrorMessage();
        if (errorMessage.isEmpty()) {
            errorMessage = ("Failed to read palette.");
        }
    }
    else {
        CaretAssertVectorIndex(event.m_palettes, 0);
        paletteOut = event.m_palettes[0];
    }
    return FunctionResultValue<const PaletteNew*>(paletteOut,
                                                  errorMessage,
                                                  errorMessage.isEmpty());
}


/**
 * @return A new palette with the given name and number of control points
 * @param name
 *    Name of palette
 * @param numberOfPositiveControlPoints
 *    Number of positive control points
 * @param numberOfNegativeControlPoints
 *    Number of negative control points
 */
FunctionResultValue<const PaletteNew*>
EventPaletteNewOperation::createNewPalette(const AString& name,
                                        const int32_t numberOfPositiveControlPoints,
                                        const int32_t numberOfNegativeControlPoints)
{
    EventPaletteNewOperation event(Operation::NEW_PALETTE);
    event.setNewPaletteInfo(name,
                            numberOfPositiveControlPoints,
                            numberOfNegativeControlPoints);
    EventManager::get()->sendEvent(event.getPointer());

    AString errorMessage;
    const PaletteNew* paletteOut(NULL);
    if (event.m_palettes.empty()) {
        errorMessage = event.getErrorMessage();
        if (errorMessage.isEmpty()) {
            errorMessage = ("Failed to create a new palette.");
        }
    }
    else {
        CaretAssertVectorIndex(event.m_palettes, 0);
        paletteOut = event.m_palettes[0];
    }
    return FunctionResultValue<const PaletteNew*>(paletteOut,
                                                 errorMessage,
                                                 errorMessage.isEmpty());
}

/**
 * @return A new palette that is a copy of the given palette
 * @param paletteBase
 *    Palette that is copied
 * @param newPaletteName
 *    Name for new palette
 */
FunctionResultValue<const PaletteNew*>
EventPaletteNewOperation::copyPalette(const PaletteBase* paletteBase,
                                      const AString& newPaletteName)
{
    CaretAssert(paletteBase);

    PaletteNew* paletteOut(NULL);
    AString errorMessage;
    
    if (paletteBase != NULL) {
        const PaletteNew* paletteNew(paletteBase->castToPaletteNew());
        const Palette* palette(paletteBase->castToPalette());
        if (paletteNew != NULL) {
            PaletteNew* pn(new PaletteNew(*paletteNew));
            pn->setName(newPaletteName);
            paletteOut = pn;
        }
        else if (palette != NULL) {
            AString notes;
            PaletteNew* pn(PaletteNew::createFromPalette(palette, notes));
            pn->setName(newPaletteName);
            paletteOut = pn;
        }
        
        if (paletteOut != NULL) {
            FunctionResult result(EventPaletteNewOperation::addPalette(paletteOut));
            errorMessage = result.getErrorMessage();
        }
    }
    else {
        errorMessage = "Palette for copying is NULL";
    }
    
    return FunctionResultValue<const PaletteNew*>(paletteOut,
                                                  errorMessage,
                                                  errorMessage.isEmpty());
}

/**
 * Delete the given palette
 * @param palette
 *    Palette to delete
 * @return
 *    Function result with sucess/failure
 */
FunctionResult
EventPaletteNewOperation::deletePalette(const PaletteNew* palette)
{
    EventPaletteNewOperation event(Operation::DELETE_PALETTE);
    std::vector<const PaletteNew*> palettes { const_cast<PaletteNew*>(palette) };
    event.setPalettes(palettes);
    EventManager::get()->sendEvent(event.getPointer());
    
    if (event.isError()) {
        return FunctionResult::error(event.getErrorMessage());
    }
    return FunctionResult::ok();
}

/**
 * Rename the given palette
 * @param palette
 *    Palette to rename
 * @param newName
 *    New name for palette
 * @return
 *    Function result with sucess/failure
 */
FunctionResult
EventPaletteNewOperation::renamePalette(const PaletteNew* palette,
                                     const AString& newName)
{
    EventPaletteNewOperation event(Operation::RENAME_PALETTE);
    event.m_paletteName = newName;
    std::vector<const PaletteNew*> palettes { const_cast<PaletteNew*>(palette) };
    event.setPalettes(palettes);
    EventManager::get()->sendEvent(event.getPointer());
    
    if (event.isError()) {
        return FunctionResult::error(event.getErrorMessage());
    }
    return FunctionResult::ok();
}

/**
 * Send a notification that the palettes have changed
 */
void
EventPaletteNewOperation::sendPalettesChangedNotification()
{
    EventPaletteNewOperation event(Operation::PALETTES_CHANGED_NOTIFICATION);
    EventManager::get()->sendEvent(event.getPointer());
}


/**
 * Sets the palettes for an operation
 * @param palettes
 *   Palettes for the operation
 */
void
EventPaletteNewOperation::setPalettes(std::vector<const PaletteNew*>& palettes)
{
    m_palettes = palettes;
}

/**
 * Set info for a new palette
 * @param name
 *    Name of palette
 * @param numberOfPositiveControlPoints
 *    Number of positive control points
 * @param numberOfNegativeControlPoints
 *    Number of negative control points
 */
void
EventPaletteNewOperation::setNewPaletteInfo(const AString& name,
                                         const int32_t numberOfPositiveControlPoints,
                                         const int32_t numberOfNegativeControlPoints)
{
    m_paletteName = name;
    m_numberOfNegativeControlPoints = numberOfNegativeControlPoints;
    m_numberOfPositiveControlPoints = numberOfPositiveControlPoints;
}

/**
 * Get info for a new palette
 * @param nameOut
 *    Name of palette
 * @param numberOfPositiveControlPointsOut
 *    Number of positive control points
 * @param numberOfNegativeControlPointsOut
 *    Number of negative control points
 */
void
EventPaletteNewOperation::getNewPaletteInfo(AString& nameOut,
                                         int32_t& numberOfPositiveControlPointsOut,
                                         int32_t& numberOfNegativeControlPointsOut)
{
    nameOut = m_paletteName;
    numberOfPositiveControlPointsOut = m_numberOfPositiveControlPoints;
    numberOfNegativeControlPointsOut = m_numberOfNegativeControlPoints;
}

/**
 * Update a palette
 * @param palette
 *    Palette that is updated
 * @param positiveMapping
 *    Updated positive mapping
 * @param negativeMapping
 *    Updated negative mapping
 * @param zeroMapping
 *    Updated zero mapping
 */
FunctionResult
EventPaletteNewOperation::updatePalette(const PaletteNew* palette,
                                     const std::vector<PaletteNew::ScalarColor>& positiveMapping,
                                     const std::vector<PaletteNew::ScalarColor>& negativeMapping,
                                     const PaletteNew::ScalarColor& zeroMapping)
{
    EventPaletteNewOperation event(Operation::UPDATE_PALETTE);
    std::vector<const PaletteNew*> palettes { const_cast<PaletteNew*>(palette) };
    event.setPalettes(palettes);
    event.m_positiveMapping = positiveMapping;
    event.m_negativeMapping = negativeMapping;
    event.m_zeroMapping     = zeroMapping;
    EventManager::get()->sendEvent(event.getPointer());
    
    if (event.isError()) {
        return FunctionResult::error(event.getErrorMessage());
    }
    return FunctionResult::ok();
}

/**
 * Get the mappings for palette update=
 * @param positiveMapping
 *    Updated positive mapping
 * @param negativeMapping
 *    Updated negative mapping
 * @param zeroMapping
 *    Updated zero mapping
 */
void
EventPaletteNewOperation::getUpdateMappings(std::vector<PaletteNew::ScalarColor>& positiveMappingOut,
                                         std::vector<PaletteNew::ScalarColor>& negativeMappingOut,
                                         PaletteNew::ScalarColor& zeroMappingOut)
{
    positiveMappingOut = m_positiveMapping;
    negativeMappingOut = m_negativeMapping;
    zeroMappingOut     = m_zeroMapping;
}

