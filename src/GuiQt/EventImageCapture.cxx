
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

#define __EVENT_IMAGE_CAPTURE_DECLARE__
#include "EventImageCapture.h"
#undef __EVENT_IMAGE_CAPTURE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventImageCapture 
 * \brief Event for capturing images.
 * \ingroup GuiQt
 */

/**
 * Constructor for capturing image of window without any resizing.
 *
 * @param browserWindowIndex
 *    The browser window index.
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_IMAGE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_imageSizeX(0),
m_imageSizeY(0)
{
}

/**
 * Constructor for capturing image of window with the given sizing.  If
 * the X & Y sizes are both zero, the no image resizing is performed.
 *
 * @param browserWindowIndex
 *    The browser window index.
 * @param imageSizeX
 *    X-Size for the captured image.
 * @param imageSizeY
 *    Y-Size for the captured image.
 */
EventImageCapture::EventImageCapture(const int32_t browserWindowIndex,
                                     const int32_t imageSizeX,
                                     const int32_t imageSizeY)
: Event(EventTypeEnum::EVENT_IMAGE_CAPTURE),
m_browserWindowIndex(browserWindowIndex),
m_imageSizeX(imageSizeX),
m_imageSizeY(imageSizeY)
{
    m_backgroundColor[0] = 0;
    m_backgroundColor[1] = 0;
    m_backgroundColor[2] = 0;
}


/**
 * Destructor.
 */
EventImageCapture::~EventImageCapture()
{
}

/**
 * @return The browser window index.
 */
int32_t
EventImageCapture::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return The image X size.
 */
int32_t
EventImageCapture::getImageSizeX() const
{
    return m_imageSizeX;
}

/**
 * @return The image Y size.
 */
int32_t
EventImageCapture::getImageSizeY() const
{
    return m_imageSizeY;
}

/**
 * Get the graphics area's background color.
 *
 * @param backgroundColor
 *    RGB components of background color [0, 255]
 */
void
EventImageCapture::getBackgroundColor(uint8_t backgroundColor[3]) const
{
    backgroundColor[0] = m_backgroundColor[0];
    backgroundColor[1] = m_backgroundColor[1];
    backgroundColor[2] = m_backgroundColor[2];
}

/**
 * Set the graphics area's background color.
 *
 * @param backgroundColor
 *    RGB components of background color [0, 255]
 */
void
EventImageCapture::setBackgroundColor(const uint8_t backgroundColor[3])
{
    m_backgroundColor[0] = backgroundColor[0];
    m_backgroundColor[1] = backgroundColor[1];
    m_backgroundColor[2] = backgroundColor[2];
}

/**
 * @return The captured image.
 */
QImage
EventImageCapture::getImage() const
{
    return m_image;
}

/**
 * Set the captured image.
 */
void
EventImageCapture::setImage(const QImage& image)
{
    m_image = image;
}
