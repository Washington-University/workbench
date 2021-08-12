
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

#define __SELECTION_ITEM_CZI_IMAGE_DECLARE__
#include "SelectionItemCziImage.h"
#undef __SELECTION_ITEM_CZI_IMAGE_DECLARE__

#include "CziImageFile.h"

using namespace caret;


    
/**
 * \class SelectionItemCziImage
 * \brief Contains information about the selected image.
 */
/**
 * Constructor.
 */
SelectionItemCziImage::SelectionItemCziImage()
: SelectionItem(SelectionItemDataTypeEnum::CZI_IMAGE)
{
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemCziImage::~SelectionItemCziImage()
{
}

/**
 * @return The pixel index with origin at bottom of image.
 * OpenGL typically has origin at bottom left.
 */
PixelIndex
SelectionItemCziImage::getPixelIndexOriginAtBottom() const
{
    return m_pixelIndexOriginAtBottom;
}

/**
 * @return The pixel index with origin at top of image.
 * Images typcially have the origin at the top left.
 */
PixelIndex
SelectionItemCziImage::getPixelIndexOriginAtTop() const
{
    return m_pixelIndexOriginAtTop;
}

/**
 * Set the pixel index
 * @param pixelIndexOriginAtBottom
 *    The pixel index with origin at bottom
 * @param pixelIndexOriginAtTop
 *    The pixel index with origin at top
 */
void
SelectionItemCziImage::setPixelIndex(const PixelIndex& pixelIndexOriginAtBottom,
                                     const PixelIndex& pixelIndexOriginAtTop)
{
    m_pixelIndexOriginAtBottom = pixelIndexOriginAtBottom;
    m_pixelIndexOriginAtTop    = pixelIndexOriginAtTop;
}

/**
 * @param The tab index.
 */
int32_t
SelectionItemCziImage::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Set the tab index.
 * @param tabIndex
 *    Index of the tab.
 */
void
SelectionItemCziImage::setTabIndex(const int32_t tabIndex)
{
    m_tabIndex = tabIndex;
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
SelectionItemCziImage::getPixelRGBA(uint8_t pixelRGBAOut[4]) const
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
SelectionItemCziImage::setPixelRGBA(const uint8_t pixelRGBA[4])
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
SelectionItemCziImage::reset()
{
    SelectionItem::reset();
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemCziImage::resetPrivate()
{
    m_imageFile = NULL;
    m_pixelIndexOriginAtBottom.setIJK(-1, -1, -1);
    m_pixelIndexOriginAtTop.setIJK(-1, -1, -1);
    m_pixelRGBA[0] = 0;
    m_pixelRGBA[1] = 0;
    m_pixelRGBA[2] = 0;
    m_pixelRGBA[3] = 0;
    m_pixelRGBAValidFlag = false;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemCziImage::isValid() const
{
    return (m_imageFile != NULL);
}

/**
 * @return Image that was selected (NULL if not valid).
 */
const CziImageFile*
SelectionItemCziImage::getCziImageFile() const
{
    return m_imageFile;
}

/**
 * @return Image that was selected (NULL if not valid).
 */
CziImageFile*
SelectionItemCziImage::getCziImageFile()
{
    return m_imageFile;
}

/**
 * Set the image that was selected.
 *
 * @param imageFile
 *    Pointer to selected image (NULL if not valid).
 */
void 
SelectionItemCziImage::setCziImageFile(CziImageFile* imageFile)
{
    m_imageFile = imageFile;
}


/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemCziImage::toString() const
{
    AString text = SelectionItem::toString();
    text += ("CziImageFile: " + ((m_imageFile != NULL) ? m_imageFile->getFileNameNoPath() : "INVALID") + "\n");
    return text;
}
