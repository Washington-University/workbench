
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

#define __SELECTION_ITEM_HISTOLOGY_STEREOTAXIC_COORDINATE_DECLARE__
#include "SelectionItemHistologyStereotaxicCoordinate.h"
#undef __SELECTION_ITEM_HISTOLOGY_STEREOTAXIC_COORDINATE_DECLARE__

#include "HistologySlicesFile.h"
#include "MediaFile.h"

using namespace caret;


    
/**
 * \class SelectionItemHistologyStereotaxicCoordinate
 * \brief Contains information about the selected histology slice.
 */
/**
 * Constructor.
 */
SelectionItemHistologyStereotaxicCoordinate::SelectionItemHistologyStereotaxicCoordinate()
: SelectionItem(SelectionItemDataTypeEnum::HISTOLOGY_STEREOTAXIC_COORDINATE)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemHistologyStereotaxicCoordinate::~SelectionItemHistologyStereotaxicCoordinate()
{
}

/**
 * @return The histology slices file
 */
const HistologySlicesFile*
SelectionItemHistologyStereotaxicCoordinate::getHistologySlicesFile() const
{
    return m_histologySlicesFile;
}

/**
 * Set the histology slices file
 */
void
SelectionItemHistologyStereotaxicCoordinate::setHistologySlicesFile(HistologySlicesFile* histologySlicesFile)
{
    m_histologySlicesFile = histologySlicesFile;
}

/**
 * @return The histology coordinate
 */
HistologyCoordinate
SelectionItemHistologyStereotaxicCoordinate::getCoordinate() const
{
    return m_histologyCoordinate;
}

/**
 * Set the histology coordinate
 * @param histlogyCoordinate
 *    New value for histology coordinate
 */
void
SelectionItemHistologyStereotaxicCoordinate::setCoordinate(const HistologyCoordinate& histologyCoordinate)
{
    m_histologyCoordinate = histologyCoordinate;
}

/**
 * @param The tab index.
 */
int32_t
SelectionItemHistologyStereotaxicCoordinate::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Set the tab index.
 * @param tabIndex
 *    Index of the tab.
 */
void
SelectionItemHistologyStereotaxicCoordinate::setTabIndex(const int32_t tabIndex)
{
    m_tabIndex = tabIndex;
}

/**
 * @param The overlay index.
 */
int32_t
SelectionItemHistologyStereotaxicCoordinate::getOverlayIndex() const
{
    return m_overlayIndex;
}

/**
 * Set the overlay index.
 * @param overlayIndex
 *    Index of the overlay.
 */
void
SelectionItemHistologyStereotaxicCoordinate::setOverlayIndex(const int32_t overlayIndex)
{
    m_overlayIndex = overlayIndex;
}

/**
 * Get the pixel RGBA value.
 *
 * @param pixelRGBAOut
 *     Output containing pixel RGBA.
 * @return
 *     True if the RGBA values are valid
 */
bool
SelectionItemHistologyStereotaxicCoordinate::getPixelRGBA(uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = m_pixelRGBA[0];
    pixelRGBAOut[1] = m_pixelRGBA[1];
    pixelRGBAOut[2] = m_pixelRGBA[2];
    pixelRGBAOut[3] = m_pixelRGBA[3];
    return m_pixelRGBAValidFlag;
}

/**
 * @return True if the pixel RGBA is valid
 */
bool
SelectionItemHistologyStereotaxicCoordinate::isPixelRGBAValid() const
{
    return m_pixelRGBAValidFlag;
}


/**
 * Set the pixel RGBA value.
 *
 * @param pixelRGBA
 *     Pixel RGBA.
 */
void
SelectionItemHistologyStereotaxicCoordinate::setPixelRGBA(const uint8_t pixelRGBA[4])
{
    m_pixelRGBA[0] = pixelRGBA[0];
    m_pixelRGBA[1] = pixelRGBA[1];
    m_pixelRGBA[2] = pixelRGBA[2];
    m_pixelRGBA[3] = pixelRGBA[3];
    m_pixelRGBAValidFlag = true;
}

/**
  * Reset this selection item.
 * This method is virtual so cannot be called from constructor.
  */
void
SelectionItemHistologyStereotaxicCoordinate::reset()
{
    SelectionItem::reset();
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemHistologyStereotaxicCoordinate::resetPrivate()
{
    m_histologySlicesFile = NULL;
    m_histologyCoordinate = HistologyCoordinate();
    m_pixelRGBA[0] = 0;
    m_pixelRGBA[1] = 0;
    m_pixelRGBA[2] = 0;
    m_pixelRGBA[3] = 0;
    m_pixelRGBAValidFlag = false;
    m_tabIndex = -1;
    m_overlayIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemHistologyStereotaxicCoordinate::isValid() const
{
    return ((m_histologySlicesFile != NULL)
            && m_histologyCoordinate.isValid());
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemHistologyStereotaxicCoordinate::toString() const
{
    AString text = SelectionItem::toString();
    text += m_histologyCoordinate.toString();
    return text;
}
