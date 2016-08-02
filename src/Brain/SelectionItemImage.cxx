
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

#define __SELECTION_ITEM_IMAGE_DECLARE__
#include "SelectionItemImage.h"
#undef __SELECTION_ITEM_IMAGE_DECLARE__

#include "ImageFile.h"

using namespace caret;


    
/**
 * \class SelectionItemImage 
 * \brief Contains information about the selected image.
 */
/**
 * Constructor.
 */
SelectionItemImage::SelectionItemImage()
: SelectionItem(SelectionItemDataTypeEnum::IMAGE)
{
    m_imageFile = NULL;
    m_pixelI = -1;
    m_pixelJ = -1;
}

/**
 * Destructor.
 */
SelectionItemImage::~SelectionItemImage()
{
}

/**
 * @return Pixel I.
 */
int32_t
SelectionItemImage::getPixelI() const
{
    return m_pixelI;
}

/**
 * @return Pixel J.
 */
int32_t
SelectionItemImage::getPixelJ() const
{
    return m_pixelJ;
}

/**
 * Set pixel I
 *
 * @param i
 *    Value for I.
 */
void
SelectionItemImage::setPixelI(const int32_t i)
{
    m_pixelI = i;
}

/**
 * Set pixel J
 *
 * @param j
 *    Value for J.
 */
void
SelectionItemImage::setPixelJ(const int32_t j)
{
    m_pixelJ = j;
}

/**
 * Reset this selection item.
 */
void
SelectionItemImage::reset()
{
    SelectionItem::reset();
    m_imageFile = NULL;
    m_pixelI = -1;
    m_pixelJ = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemImage::isValid() const
{
    return (m_imageFile != NULL);
}

/**
 * @return Image that was selected (NULL if not valid).
 */
const ImageFile*
SelectionItemImage::getImageFile() const
{
    return m_imageFile;
}

/**
 * @return Image that was selected (NULL if not valid).
 */
ImageFile*
SelectionItemImage::getImageFile()
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
SelectionItemImage::setImageFile(ImageFile* imageFile)
{
    m_imageFile = imageFile;
}


/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemImage::toString() const
{
    AString text = SelectionItem::toString();
    text += ("ImageFile: " + ((m_imageFile != NULL) ? m_imageFile->getFileNameNoPath() : "INVALID") + "\n");
    return text;
}
