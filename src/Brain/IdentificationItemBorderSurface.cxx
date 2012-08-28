
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __IDENTIFICATION_ITEM_BORDER_SURFACE_DECLARE__
#include "IdentificationItemBorderSurface.h"
#undef __IDENTIFICATION_ITEM_BORDER_SURFACE_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class IdentificationItemBorderSurface 
 * \brief Contains information about the identified border.
 */
/**
 * Constructor.
 */
IdentificationItemBorderSurface::IdentificationItemBorderSurface()
: IdentificationItem(IdentificationItemDataTypeEnum::BORDER_SURFACE)
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
IdentificationItemBorderSurface::~IdentificationItemBorderSurface()
{
}

/**
 * Reset this selection item. 
 */
void 
IdentificationItemBorderSurface::reset()
{
    IdentificationItem::reset();
    this->surface = NULL;
    this->border = NULL;
    this->borderFile = NULL;
    this->borderIndex = -1;
    this->borderPointIndex = -1;
}

/**
 * @return Is this identified item valid?
 */
bool 
IdentificationItemBorderSurface::isValid() const
{
    return (this->border != NULL);
}

/**
 * @return Surface on which border was drawn.
 */
const Surface* 
IdentificationItemBorderSurface::getSurface() const
{
    return this->surface;
}

/**
 * @return Surface on which border was drawn.
 */
Surface* 
IdentificationItemBorderSurface::getSurface()
{
    return this->surface;
}

/**
 * Set the surface on which border was drawn.
 * @param surface
 *    New value for surface.
 */
void 
IdentificationItemBorderSurface::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * @return The border that was identified.
 */
const Border* 
IdentificationItemBorderSurface::getBorder() const
{
    return this->border;
}

/**
 * @return The border that was identified.
 */
Border* 
IdentificationItemBorderSurface::getBorder()
{
    return this->border;
}

/**
 * Set the border that was identified.
 * @param border
 *    New value for border.
 */
void 
IdentificationItemBorderSurface::setBorder(Border* border)
{
    this->border = border;
}

/**
 * @return The border file containing border that was identified.
 */
const BorderFile* 
IdentificationItemBorderSurface::getBorderFile() const
{
    return this->borderFile;
}

/**
 * @return The border file containing border that was identified.
 */
BorderFile* 
IdentificationItemBorderSurface::getBorderFile()
{
    return this->borderFile;
}

/**
 * Set the border file containing border that was identified.
 * @param borderFile
 *    New value for border file.
 */
void 
IdentificationItemBorderSurface::setBorderFile(BorderFile* borderFile)
{
    this->borderFile = borderFile;
}

/**
 * return Index of selected border.
 */
int32_t 
IdentificationItemBorderSurface::getBorderIndex() const
{
    return this->borderIndex;
}

/**
 * Set index of selected border.
 * @param borderIndex
 *    New value for border index.
 */
void 
IdentificationItemBorderSurface::setBorderIndex(const int32_t borderIndex)
{
    this->borderIndex = borderIndex;
}

/**
 * return Index of selected border.
 */
int32_t 
IdentificationItemBorderSurface::getBorderPointIndex() const
{
    return this->borderPointIndex;
}

/**
 * Set index of selected border.
 * @param borderIndex
 *    New value for border index.
 */
void 
IdentificationItemBorderSurface::setBorderPointIndex(const int32_t borderPointIndex)
{
    this->borderPointIndex = borderPointIndex;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
IdentificationItemBorderSurface::toString() const
{
    AString text = IdentificationItem::toString();
    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Border File: " + ((borderFile != NULL) ? borderFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Border: " + ((border != NULL) ? border->getName() : "INVALID") + "\n");
    text += ("Border Index: " + AString::number(borderIndex) + "\n");
    text += ("Border Point Index: " + AString::number(borderPointIndex) + "\n");
    return text;
}

