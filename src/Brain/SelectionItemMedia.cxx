
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

#define __SELECTION_ITEM_MEDIA_DECLARE__
#include "SelectionItemMedia.h"
#undef __SELECTION_ITEM_MEDIA_DECLARE__

#include "CziImageFile.h"

using namespace caret;


    
/**
 * \class SelectionItemMedia
 * \brief Contains information about the selected media.
 */
/**
 * Constructor.
 */
SelectionItemMedia::SelectionItemMedia()
: SelectionItem(SelectionItemDataTypeEnum::MEDIA)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemMedia::~SelectionItemMedia()
{
}

/**
 * @return The pixel logical index
 */
PixelLogicalIndex
SelectionItemMedia::getPixelLogicalIndex() const
{
    return m_pixelLogicalIndex;
}

/**
 * Set the pixel logical index
 * @param pixelLogicalIndex
 *    The pixel logical index
 */
void
SelectionItemMedia::setPixelLogicalIndex(const PixelLogicalIndex& pixelLogicalIndex)
{
    m_pixelLogicalIndex = pixelLogicalIndex;
}

/**
 * @param The tab index.
 */
int32_t
SelectionItemMedia::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Set the tab index.
 * @param tabIndex
 *    Index of the tab.
 */
void
SelectionItemMedia::setTabIndex(const int32_t tabIndex)
{
    m_tabIndex = tabIndex;
}

/**
 * @param The overlay index.
 */
int32_t
SelectionItemMedia::getOverlayIndex() const
{
    return m_overlayIndex;
}

/**
 * Set the overlay index.
 * @param overlayIndex
 *    Index of the overlay.
 */
void
SelectionItemMedia::setOverlayIndex(const int32_t overlayIndex)
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
SelectionItemMedia::getPixelRGBA(uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = m_pixelRGBA[0];
    pixelRGBAOut[1] = m_pixelRGBA[1];
    pixelRGBAOut[2] = m_pixelRGBA[2];
    pixelRGBAOut[3] = m_pixelRGBA[3];
    return m_pixelRGBAValidFlag;
}

/**
 * Set the pixel RGBA value.
 *
 * @param pixelRGBA
 *     Pixel RGBA.
 */
void
SelectionItemMedia::setPixelRGBA(const uint8_t pixelRGBA[4])
{
    m_pixelRGBA[0] = pixelRGBA[0];
    m_pixelRGBA[1] = pixelRGBA[1];
    m_pixelRGBA[2] = pixelRGBA[2];
    m_pixelRGBA[3] = pixelRGBA[3];
    m_pixelRGBAValidFlag = true;
}

/**
 * Get the stereotaxic coordinate of the selected pixel
 * @param stereotaxicXYZOut
 *   Output containing the coordinate
 * @return True if output coordinate is valid, else false.
 */
bool
SelectionItemMedia::getStereotaxicXYZ(std::array<float, 3>& stereotaxicXYZOut)
{
    const CziImageFile* cziImageFile = m_mediaFile->castToCziImageFile();
    if (cziImageFile != NULL) {
        return cziImageFile->pixelIndexToStereotaxicXYZ(m_pixelLogicalIndex,
                                                        true,
                                                        stereotaxicXYZOut);
    }
    return false;
}

/**
  * Reset this selection item.
 * This method is virtual so cannot be called from constructor.
  */
void
SelectionItemMedia::reset()
{
    SelectionItem::reset();
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemMedia::resetPrivate()
{
    m_mediaFile = NULL;
    m_pixelLogicalIndex.setI(-1000000);
    m_pixelLogicalIndex.setJ(-1000000);
    m_pixelLogicalIndex.setK(-1000000);
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
SelectionItemMedia::isValid() const
{
    return (m_mediaFile != NULL);
}

/**
 * @return Image that was selected (NULL if not valid).
 */
const MediaFile*
SelectionItemMedia::getMediaFile() const
{
    return m_mediaFile;
}

/**
 * @return Image that was selected (NULL if not valid).
 */
MediaFile*
SelectionItemMedia::getMediaFile()
{
    return m_mediaFile;
}

/**
 * Set the media that was selected.
 *
 * @param mediaFile
 *    Pointer to selected media (NULL if not valid).
 */
void 
SelectionItemMedia::setMediaFile(MediaFile* mediaFile)
{
    m_mediaFile = mediaFile;
}


/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemMedia::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Media File: " + ((m_mediaFile != NULL) ? m_mediaFile->getFileNameNoPath() : "INVALID") + "\n");
    return text;
}
