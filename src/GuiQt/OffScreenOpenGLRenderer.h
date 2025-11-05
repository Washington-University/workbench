#ifndef __OFF_SCREEN_OPEN_G_L_RENDERER_H__
#define __OFF_SCREEN_OPEN_G_L_RENDERER_H__

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



#include <memory>

#include <QObject>

class QGLWidget;
class QOpenGLFramebufferObject;
class QOpenGLFramebufferObjectFormat;
class QOffscreenSurface;
class QOpenGLContext;
class QOpenGLWidget;
class QSurfaceFormat;
class QWidget;

namespace caret {

    class OffScreenOpenGLRenderer : public QObject {
        
        Q_OBJECT

    public:
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        OffScreenOpenGLRenderer(QOpenGLWidget* openglWidget,
#else
        OffScreenOpenGLRenderer(QGLWidget* openglWidget,
#endif
                                const int32_t width,
                                const int32_t height);
        
        virtual ~OffScreenOpenGLRenderer();

        bool isError() const;

        QString getErrorMessage() const;
        
        QImage getImage();
        
        // ADD_NEW_METHODS_HERE

    private:
        static QString QOpenGLFramebufferObjectFormatToString(const QOpenGLFramebufferObjectFormat& format);
        
        OffScreenOpenGLRenderer(const OffScreenOpenGLRenderer&);

        OffScreenOpenGLRenderer& operator=(const OffScreenOpenGLRenderer&);
        
        std::unique_ptr<QOffscreenSurface> m_offScreenSurface;
        
        std::unique_ptr<QOpenGLFramebufferObject> m_frameBuffer;
        
        QOpenGLContext* m_openglContext;
        
        QString m_errorMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OFF_SCREEN_OPEN_G_L_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OFF_SCREEN_OPEN_G_L_RENDERER_DECLARE__

} // namespace
#endif  //__OFF_SCREEN_OPEN_G_L_RENDERER_H__
