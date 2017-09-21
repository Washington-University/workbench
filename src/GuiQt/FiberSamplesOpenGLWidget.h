#ifndef __FIBER_SAMPLES_OPEN_G_L_WIDGET__H_
#define __FIBER_SAMPLES_OPEN_G_L_WIDGET__H_

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

/*
 * When GLEW is used, CaretOpenGLInclude.h will include "Gl/glew.h".
 * Gl/glew.h MUST BE BEFORE Gl/gl.h and Gl/gl.h is included by
 * QGLWidget so, we must include CaretOpenGL.h before QGLWidget.
 */
#include "CaretOpenGLInclude.h"

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#include <QOpenGLWidget>
#else
#include <QGLWidget>
#endif

#include "Matrix4x4.h"

class QLabel;
class QCheckBox;

namespace caret {

    class BrainOpenGLShapeRing;
    class BrainOpenGLShapeSphere;
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    class FiberSamplesOpenGLWidget : public QOpenGLWidget {
#else
    class FiberSamplesOpenGLWidget : public QGLWidget {
#endif
        Q_OBJECT
        
    public:
        FiberSamplesOpenGLWidget(const int32_t browserWindowIndex,
                                 QCheckBox* enabledCheckBox,
                                 QLabel* fiberMeanLabels[3],
                                 QLabel* fiberVarianceLabels[3],
                                 QWidget* parent = 0);
        
        virtual ~FiberSamplesOpenGLWidget();
        
        // ADD_NEW_METHODS_HERE
        
    protected:
        void initializeGL();
        
        void resizeGL(int width,
                      int height);
        
        void paintGL();
        
        void mouseMoveEvent(QMouseEvent* e);
        
        void mousePressEvent(QMouseEvent* e);
        
        void mouseReleaseEvent(QMouseEvent* e);

    private:
        FiberSamplesOpenGLWidget(const FiberSamplesOpenGLWidget&);

        FiberSamplesOpenGLWidget& operator=(const FiberSamplesOpenGLWidget&);
        
        void createShapes();
        
        void setupLighting();
        
        void setOrthographicProjection(const int width,
                                       const int height);
        
        void drawOrientations();
        
        // ADD_NEW_MEMBERS_HERE
        
        int32_t m_browserWindowIndex;
        
        BrainOpenGLShapeSphere* m_sphereBig;
        
        BrainOpenGLShapeSphere* m_sphereSmall;
        
        BrainOpenGLShapeRing* m_ring;
        
        GLint m_widgetWidth;
        
        GLint m_widgetHeight;
        
        int m_mousePressX;
        
        int m_mousePressY;
        
        int m_lastMouseX;
        
        int m_lastMouseY;
        
        Matrix4x4 m_rotationMatrix;

        QCheckBox* m_enabledCheckBox;
        
        QLabel* m_fiberMeanLabels[3];
        QLabel* m_fiberVarianceLabels[3];
        
        static const float s_sphereBigRadius;
        static const float s_sphereSmallRadius;
    };
    
#ifdef __FIBER_SAMPLES_OPEN_G_L_WIDGET_DECLARE__
    const float FiberSamplesOpenGLWidget::s_sphereBigRadius = 100.0;
    const float FiberSamplesOpenGLWidget::s_sphereSmallRadius = 1.0;
#endif // __FIBER_SAMPLES_OPEN_G_L_WIDGET_DECLARE__

} // namespace
#endif  //__FIBER_SAMPLES_OPEN_G_L_WIDGET__H_
