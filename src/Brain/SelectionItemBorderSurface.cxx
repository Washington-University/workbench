
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

#define __SELECTION_ITEM_BORDER_SURFACE_DECLARE__
#include "SelectionItemBorderSurface.h"
#undef __SELECTION_ITEM_BORDER_SURFACE_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class SelectionItemBorderSurface 
 * \brief Contains information about the selected border.
 */
/**
 * Constructor.
 */
SelectionItemBorderSurface::SelectionItemBorderSurface()
: SelectionItem(SelectionItemDataTypeEnum::BORDER_SURFACE)
{
    this->surface = NULL;
    this->border = NULL;
    this->borderFile = NULL;
    this->borderIndex = -1;
    this->borderPointIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemBorderSurface::~SelectionItemBorderSurface()
{
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemBorderSurface::reset()
{
    SelectionItem::reset();
    this->surface = NULL;
    this->border = NULL;
    this->borderFile = NULL;
    this->borderIndex = -1;
    this->borderPointIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemBorderSurface::isValid() const
{
    return (this->border != NULL);
}

/**
 * @return Surface on which border was drawn.
 */
const Surface* 
SelectionItemBorderSurface::getSurface() const
{
    return this->surface;
}

/**
 * @return Surface on which border was drawn.
 */
Surface* 
SelectionItemBorderSurface::getSurface()
{
    return this->surface;
}

/**
 * Set the surface on which border was drawn.
 * @param surface
 *    New value for surface.
 */
void 
SelectionItemBorderSurface::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * @return The border that was selected.
 */
const Border* 
SelectionItemBorderSurface::getBorder() const
{
    return this->border;
}

/**
 * @return The border that was selected.
 */
Border* 
SelectionItemBorderSurface::getBorder()
{
    return this->border;
}

/**
 * Set the border that was selected.
 * @param border
 *    New value for border.
 */
void 
SelectionItemBorderSurface::setBorder(Border* border)
{
    this->border = border;
}

/**
 * @return The border file containing border that was selected.
 */
const BorderFile* 
SelectionItemBorderSurface::getBorderFile() const
{
    return this->borderFile;
}

/**
 * @return The border file containing border that was selected.
 */
BorderFile* 
SelectionItemBorderSurface::getBorderFile()
{
    return this->borderFile;
}

/**
 * Set the border file containing border that was selected.
 * @param borderFile
 *    New value for border file.
 */
void 
SelectionItemBorderSurface::setBorderFile(BorderFile* borderFile)
{
    this->borderFile = borderFile;
}

/**
 * return Index of selected border.
 */
int32_t 
SelectionItemBorderSurface::getBorderIndex() const
{
    return this->borderIndex;
}

/**
 * Set index of selected border.
 * @param borderIndex
 *    New value for border index.
 */
void 
SelectionItemBorderSurface::setBorderIndex(const int32_t borderIndex)
{
    this->borderIndex = borderIndex;
}

/**
 * return Index of selected border.
 */
int32_t 
SelectionItemBorderSurface::getBorderPointIndex() const
{
    return this->borderPointIndex;
}

/**
 * Set index of selected border.
 * @param borderIndex
 *    New value for border index.
 */
void 
SelectionItemBorderSurface::setBorderPointIndex(const int32_t borderPointIndex)
{
    this->borderPointIndex = borderPointIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemBorderSurface::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Border File: " + ((borderFile != NULL) ? borderFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Border: " + ((border != NULL) ? border->getName() : "INVALID") + "\n");
    text += ("Border Index: " + AString::number(borderIndex) + "\n");
    text += ("Border Point Index: " + AString::number(borderPointIndex) + "\n");
    return text;
}

