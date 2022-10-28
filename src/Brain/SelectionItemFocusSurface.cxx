
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

#define __SELECTION_ITEM_FOCUS_SURFACE_DECLARE__
#include "SelectionItemFocusSurface.h"
#undef __SELECTION_ITEM_FOCUS_SURFACE_DECLARE__

#include "FociFile.h"
#include "Focus.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class SelectionItemFocusSurface 
 * \brief Contains information about the selected focus.
 */
/**
 * Constructor.
 */
SelectionItemFocusSurface::SelectionItemFocusSurface()
: SelectionItem(SelectionItemDataTypeEnum::FOCUS_SURFACE)
{
    this->surface = NULL;
    this->focus = NULL;
    this->fociFile = NULL;
    this->focusIndex = -1;
    this->focusProjectionIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemFocusSurface::~SelectionItemFocusSurface()
{
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemFocusSurface::reset()
{
    SelectionItem::reset();
    this->surface = NULL;
    this->focus = NULL;
    this->fociFile = NULL;
    this->focusIndex = -1;
    this->focusProjectionIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemFocusSurface::isValid() const
{
    return (this->focus != NULL);
}

/**
 * @return Surface on which focus was drawn.
 */
const Surface*
SelectionItemFocusSurface::getSurface() const
{
    return this->surface;
}

/**
 * @return Surface on which focus was drawn.
 */
Surface*
SelectionItemFocusSurface::getSurface()
{
    return this->surface;
}

/**
 * Set the surface on which focus was drawn.
 * @param surface
 *    New value for surface.
 */
void 
SelectionItemFocusSurface::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * @return The focus that was selected.
 */
const Focus* 
SelectionItemFocusSurface::getFocus() const
{
    return this->focus;
}

/**
 * @return The focus that was selected.
 */
Focus* 
SelectionItemFocusSurface::getFocus()
{
    return this->focus;
}

/**
 * Set the focus that was selected.
 * @param focus
 *    New value for focus.
 */
void 
SelectionItemFocusSurface::setFocus(Focus* focus)
{
    this->focus = focus;
}

/**
 * @return The focus file containing focus that was selected.
 */
const FociFile* 
SelectionItemFocusSurface::getFociFile() const
{
    return this->fociFile;
}

/**
 * @return The focus file containing focus that was selected.
 */
FociFile* 
SelectionItemFocusSurface::getFociFile()
{
    return this->fociFile;
}

/**
 * Set the focus file containing focus that was selected.
 * @param fociFile
 *    New value for focus file.
 */
void 
SelectionItemFocusSurface::setFociFile(FociFile* fociFile)
{
    this->fociFile = fociFile;
}

/**
 * return Index of selected focus.
 */
int32_t 
SelectionItemFocusSurface::getFocusIndex() const
{
    return this->focusIndex;
}

/**
 * Set index of selected focus.
 * @param focusIndex
 *    New value for focus index.
 */
void 
SelectionItemFocusSurface::setFocusIndex(const int32_t focusIndex)
{
    this->focusIndex = focusIndex;
}

/**
 * return Projection Index of selected focus.
 */
int32_t 
SelectionItemFocusSurface::getFocusProjectionIndex() const
{
    return this->focusProjectionIndex;
}

/**
 * Set projection index of selected focus.
 * @param focusProjectionIndex
 *    New value for focus index.
 */
void 
SelectionItemFocusSurface::setFocusProjectionIndex(const int32_t focusProjectionIndex)
{
    this->focusProjectionIndex = focusProjectionIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemFocusSurface::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Foci File: " + ((fociFile != NULL) ? fociFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Focus: " + ((focus != NULL) ? focus->getName() : "INVALID") + "\n");
    text += ("Focus Index: " + AString::number(focusIndex) + "\n");
    text += ("Focus Projection Index: " + AString::number(focusProjectionIndex) + "\n");
    return text;
}
