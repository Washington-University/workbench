#ifndef __FIBER_SAMPLES_OPEN_G_L_WIDGET__H_
#define __FIBER_SAMPLES_OPEN_G_L_WIDGET__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#include <QGLWidget>

#include "Matrix4x4.h"

class QLabel;
class QCheckBox;

namespace caret {

    class BrainOpenGLShapeRing;
    class BrainOpenGLShapeSphere;
    
    class FiberSamplesOpenGLWidget : public QGLWidget {
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
