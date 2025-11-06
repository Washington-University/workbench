
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "BrainOpenGLWidget.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"

#define __OFF_SCREEN_OPEN_G_L_RENDERER_DECLARE__
#include "OffScreenOpenGLRenderer.h"
#undef __OFF_SCREEN_OPEN_G_L_RENDERER_DECLARE__

#include <QOpenGLFramebufferObject>
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#include <QOpenGLWidget>
#else
#include <QGLFormat>
#include <QGLWidget>
#endif
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QSurfaceFormat>

#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::OffScreenOpenGLRenderer 
 * \brief Performs offscreen rendering of a window.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param openGLWidgetInstance
 *     An instance of either QGLWidget or QOpenGLWidget
 * @param width
 *     Width for image (may be different, perhaps larger, than width of 'openglWidget'
 * @param height
 *     Height for image (may be different, perhaps larger, than hight of 'openglWidget'
 */
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
OffScreenOpenGLRenderer::OffScreenOpenGLRenderer(QOpenGLWidget* openglWidget,
#else
OffScreenOpenGLRenderer::OffScreenOpenGLRenderer(QGLWidget* openglWidget,
#endif
                                                 const int32_t width,
                                                 const int32_t height)
: QObject()
{
    CaretAssert(openglWidget);
    
    m_openglContext = NULL;;
    QSurfaceFormat surfaceFormat;

    /*
     * Copy the surface format
     */
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    surfaceFormat = openglWidget->format();
    m_openglContext = openglWidget->context();
#else
    surfaceFormat = QGLFormat::toSurfaceFormat(openglWidget->format());
    CaretAssert(openglWidget->context());
    m_openglContext = openglWidget->context()->contextHandle();
#endif
//    QGLWidget* glWidget = dynamic_cast<QGLWidget*>(openGLWidgetInstance);
//    QOpenGLWidget* openglWidget = dynamic_cast<QOpenGLWidget*>(openGLWidgetInstance);
//    if (glWidget != NULL) {
//    }
//    else if (openglWidget != NULL) {
//    }
//    else {
//        m_errorMessage = "Unable to cast openglQtWidget to QGLWidget nor QOpenGLWidget";
//        return;
//    }
    
    CaretAssert(m_openglContext);
    if ( ! m_openglContext->isValid()) {
        m_errorMessage = "The OpenGL Context is not valid";
        return;
    }
    
    // since context is already valid, do not need to do this: openglContext->setFormat(surfaceFormat);
    
    m_offScreenSurface.reset(new QOffscreenSurface());
    m_offScreenSurface->setFormat(surfaceFormat);
    m_offScreenSurface->create();
    if ( ! m_offScreenSurface->isValid()) {
        m_errorMessage = "Offscreen surface is not valid after setFormat() and create()";
        return;
    }

    if ( ! m_openglContext->makeCurrent(m_offScreenSurface.get())) {
        m_errorMessage = "Unable to makeCurrent() using Off Screen Surface";
        return;
    }

    QOpenGLFramebufferObjectFormat frameBufferFormat;
    frameBufferFormat.setSamples(2);
    frameBufferFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    frameBufferFormat.setTextureTarget(GL_TEXTURE_2D);

    m_frameBuffer.reset(new QOpenGLFramebufferObject(width,
                                                     height,
                                                     frameBufferFormat));
    if ( ! m_frameBuffer->isValid()) {
        m_errorMessage = "Unable to create frame buffer object for Off Screen Rendering";
        m_openglContext->doneCurrent();
        return;
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    CaretLogInfo("Requested and Active QSurfaceFormats equal: "
                 + AString::fromBool(m_offScreenSurface->requestedFormat() == m_offScreenSurface->format())
                 + "\nOffScreen Surface Requested Format:\n"
                 + BrainOpenGLWidget::QSurfaceFormatToString(m_offScreenSurface->requestedFormat())
                 + "\nOffScreen Surface Active Format:\n"
                 + BrainOpenGLWidget::QSurfaceFormatToString(m_offScreenSurface->format()));
    
    CaretLogInfo("Requested and Active QOpenGLFramebufferObjectFormats equal: "
                 + AString::fromBool(frameBufferFormat == m_frameBuffer->format())
                 + "\nReqested QOpenGLFramebufferObjectFormat:\n"
                 + QOpenGLFramebufferObjectFormatToString(frameBufferFormat)
                 + "\nActive QOpenGLFramebufferObjectFormat:\n"
                 + QOpenGLFramebufferObjectFormatToString(m_frameBuffer->format()));
#endif
}

/**
 * Destructor.
 */
OffScreenOpenGLRenderer::~OffScreenOpenGLRenderer()
{
    m_openglContext->doneCurrent();
}

bool
OffScreenOpenGLRenderer::isError() const
{
    return ( ! m_errorMessage.isEmpty());
}

QString
OffScreenOpenGLRenderer::getErrorMessage() const
{
    return m_errorMessage;
}

QImage
OffScreenOpenGLRenderer::getImage()
{
    QImage image;
    
    if ( ! m_frameBuffer) {
        m_errorMessage = "Frame buffer is invalid while trying to get image";
        return image;
    }
    
    image = m_frameBuffer->toImage();
    
    return image;
}

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
/**
 * @return Text representation of QOpenGLFramebufferObjectFormat
 */
QString
OffScreenOpenGLRenderer::QOpenGLFramebufferObjectFormatToString(const QOpenGLFramebufferObjectFormat& format)
{
    AString attString;
    switch (format.attachment()) {
        case QOpenGLFramebufferObject::NoAttachment:
            attString = "QOpenGLFramebufferObject::NoAttachment";
            break;
        case QOpenGLFramebufferObject::CombinedDepthStencil:
            attString = "QOpenGLFramebufferObject::CombinedDepthStencil";
            break;
        case QOpenGLFramebufferObject::Depth:
            attString = "QOpenGLFramebufferObject::Depth";
            break;
    }
    
    AString text;
    
    text += ("Attachment: " + attString
             + "\nMipmap: " + AString::fromBool(format.mipmap())
             + "\nSamples: " + AString::number(format.samples())
             + "\nInternal texture format (int value of GLenum): "
                + AString::number(static_cast<long>(format.internalTextureFormat()))
             );
    
    return text;
}
#endif


