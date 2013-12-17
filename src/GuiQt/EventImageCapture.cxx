
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 * Constructor.
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
