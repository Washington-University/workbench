
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __OFF_SCREEN_SCENE_RENDERER_O_S_MESA_DECLARE__
#include "OffScreenSceneRendererOSMesa.h"
#undef __OFF_SCREEN_SCENE_RENDERER_O_S_MESA_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
#include "ImageFile.h"

using namespace caret;


    
/**
 * \class caret::OffScreenSceneRendererOSMesa 
 * \brief Offscreen Renderer that uses Mesa3D
 * \ingroup Brain
 */

/**
 * Constructor.
 */
OffScreenSceneRendererOSMesa::OffScreenSceneRendererOSMesa()
: OffScreenSceneRendererBase("OSMesa",
                             "Mesa3D OSMesa software renderer")
{
    
}

/**
 * Destructor.
 */
OffScreenSceneRendererOSMesa::~OffScreenSceneRendererOSMesa()
{
#ifdef HAVE_OSMESA
    if (m_mesaContext != 0) {
        OSMesaDestroyContext(m_mesaContext);
    }
#endif
}

/**
 * @copydoc OffScreenSceneRendererBase::getDrawingContext()
 */
void*
OffScreenSceneRendererOSMesa::getDrawingContext()
{
#ifdef HAVE_OSMESA
    return &m_mesaContext;
#endif
    return NULL;
};

/**
 * @copydoc OffScreenSceneRendererBase::isAvailable()
 */

bool
OffScreenSceneRendererOSMesa::isAvailable() const
{
#ifdef HAVE_OSMESA
    return true;
#endif
    return false;
}

/**
 * @copydoc OffScreenSceneRendererBase::getImage()
 */
const ImageFile*
OffScreenSceneRendererOSMesa::getImageFile() const
{
    if ( ! m_imageBuffer.empty()) {
        if ( ! m_imageBuffer.empty()) {
            m_imageFile.reset(new ImageFile(&m_imageBuffer[0],
                                            m_imageWidth,
                                            m_imageHeight,
                                            ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM));
            return m_imageFile.get();
        }
    }
    return NULL;
}

/**
 * @copydoc OffScreenSceneRendererBase::initialize()
 */
bool
OffScreenSceneRendererOSMesa::initialize(const int32_t imageWidth,
                                         const int32_t imageHeight)
{
    m_imageWidth  = imageWidth;
    m_imageHeight = imageHeight;
    CaretAssert(m_imageWidth > 0);
    CaretAssert(m_imageHeight > 0);

#ifdef HAVE_OSMESA
    /*
     * Create the image buffer
     */
    const uint32_t imageBufferSize(m_imageWidth * m_imageHeight * 4 * sizeof(unsigned char));
    m_imageBuffer.resize(imageBufferSize);
    CaretAssert(m_imageBuffer.size() == imageBufferSize);
    std::fill(m_imageBuffer.begin(),
              m_imageBuffer.end(),
              0);

    /*
     * Create the Mesa Context
     */
    const int depthBits = 16;
    const int stencilBits = 0;
    const int accumBits = 0;
    m_mesaContext = OSMesaCreateContextExt(OSMESA_RGBA,
                                           depthBits,
                                           stencilBits,
                                           accumBits,
                                           NULL);

    if (m_mesaContext == 0) {
        setErrorMessage("Creating Mesa Context failed.");
        return false;
    }
    
    /*
     * Assign buffer to Mesa Context and make current
     */
    if (OSMesaMakeCurrent(m_mesaContext,
                          &m_imageBuffer[0],
                          GL_UNSIGNED_BYTE,
                          m_imageWidth,
                          m_imageHeight) == 0) {
        GLint mesaMaxWidth(0);
        GLint mesaMaxHeight(0);
        OSMesaGetIntegerv(OSMESA_MAX_WIDTH,
                          &mesaMaxWidth);
        OSMesaGetIntegerv(OSMESA_MAX_HEIGHT,
                          &mesaMaxHeight);
        setErrorMessage("Assigning buffer to context and make current failed.  This may occur if the "
                        "image pixel width="
                        + AString::number(m_imageWidth)
                        + " or pixel height="
                        + AString::number(m_imageHeight)
                        + " exceeds the Mesa System's maximum width="
                        + AString::number(mesaMaxWidth)
                        + " or height="
                        + AString::number(mesaMaxHeight)
                        + ".");
        return false;
    }
    return true;
#else
    setErrorMessage("Trying to initialize OSMesa Renderer but it is not available");
    return false;
#endif
}

/**
 * @return Info about this instance
 */
AString
OffScreenSceneRendererOSMesa::toString() const
{
    AString txt("OSMesa Info: ");
#ifdef HAVE_OSMESA
    GLint maxWidth(0), maxHeight(0);
    OSMesaGetIntegerv(OSMESA_MAX_WIDTH, &maxWidth);
    OSMesaGetIntegerv(OSMESA_MAX_HEIGHT, &maxHeight);
    txt.appendWithNewLine("   OSMESA_MAX_WIDTH: " + AString::number(maxWidth));
    txt.appendWithNewLine("   OSMESA_MAX_HEIGHT: " + AString::number(maxHeight));
#else
    txt.append("Invalid");
#endif
    return txt;
}

