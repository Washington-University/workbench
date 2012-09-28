
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

#include <QMouseEvent>
#include <QSizePolicy>

#define __FIBER_SAMPLES_OPEN_G_L_WIDGET_DECLARE__
#include "FiberSamplesOpenGLWidget.h"
#undef __FIBER_SAMPLES_OPEN_G_L_WIDGET_DECLARE__

using namespace caret;

#include "BrainOpenGL.h"
#include "BrainOpenGLShapeSphere.h"

/**
 * \class caret::FiberSamplesOpenGLWidget 
 * \brief OpenGL Widget for drawing fiber samples on a sphere
 */

/**
 * Constructor.
 */
FiberSamplesOpenGLWidget::FiberSamplesOpenGLWidget(QWidget* parent)
: QGLWidget(parent)
{
    m_sphere = NULL;
    
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::Expanding);
}

/**
 * Destructor.
 */
FiberSamplesOpenGLWidget::~FiberSamplesOpenGLWidget()
{
    if (m_sphere != NULL) {
        delete m_sphere;
    }
}

/**
 * Called once prior to resizeGL() and paintGL() to
 * make any necessary initializations.
 */
void
FiberSamplesOpenGLWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glFrontFace(GL_CCW);
    glEnable(GL_NORMALIZE);

    //
    // Avoid drawing backfacing polygons
    //
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    glShadeModel(GL_SMOOTH);
        
    createSphere();
}

/**
 * Setup lighting parameters.
 */
void
FiberSamplesOpenGLWidget::setupLighting()
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    float lightColor[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
    
    float materialColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    
    float ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    
    
    float lightPosition[] = { 0.0f, 0.0f, 100.0f, 0.0f };
    glPushMatrix();
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);
    
    //
    // Light 1 position is opposite of light 0
    //
    lightPosition[0] = -lightPosition[0];
    lightPosition[1] = -lightPosition[1];
    lightPosition[2] = -lightPosition[2];
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT1);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
}

/**
 * Gets called whenever the widget is resized.
 * @param width
 *    New width of widget.
 * @param height
 *    New height of widget.
 */
void
FiberSamplesOpenGLWidget::resizeGL(int width,
                                   int height)
{
    setOrthographicProjection(width, height);
}

/**
 * Set the orthographic projection.
 * @param width 
 *    New width.
 * @param height
 *    New height.
 */
void
FiberSamplesOpenGLWidget::setOrthographicProjection(const int width,
                                                    const int height)
{
    m_widgetWidth = width;
    m_widgetHeight = height;
    
    glViewport(0,
               0,
               width,
               height);
    
    const double orthoSize = 125.0;
    const double aspectRatio = (static_cast<double>(width)
                                / static_cast<double>(height));
    double orthoWidth  = orthoSize;
    double orthoHeight = orthoSize;
    if (aspectRatio > 1.0) {
        orthoWidth *= aspectRatio;
    }
    else {
        const float inverseAspectRatio = 1.0 / aspectRatio;
        orthoHeight *= inverseAspectRatio;
    }
    
    const double orthoNearFar = 5000.0;  // allows zooming
    
    double orthographicRight  =  orthoWidth;
    double orthographicLeft   = -orthoWidth;
    double orthographicTop    =  orthoHeight;
    double orthographicBottom = -orthoHeight;
    double orthographicNear   = -orthoNearFar;
    double orthographicFar    =  orthoNearFar;
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    glOrtho(orthographicLeft,
            orthographicRight,
            orthographicBottom,
            orthographicTop,
            orthographicNear,
            orthographicFar);
    
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Called when the widget needs to be repainted.
 */
void
FiberSamplesOpenGLWidget::paintGL()
{
    setOrthographicProjection(width(),
                              height());
    glClearColor(0.0,
                 0.0,
                 0.0,
                 1.0);
    glClear(GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_MODELVIEW);
    
    setupLighting();
    
    glPushMatrix();
    
    glLoadIdentity();

    double rotationMatrixElements[16];
    m_rotationMatrix.getMatrixForOpenGL(rotationMatrixElements);
    glMultMatrixd(rotationMatrixElements);

    
    const GLfloat lineStart = -200.0;
    const GLfloat lineEnd   =  200.0;
    
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    
    /*
     * Axis lines
     */
    glLineWidth(3.0);
    
    glBegin(GL_LINES);

    glColor3f(0.5, 0.0, 0.0);
    glVertex3f(lineStart, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    
    glColor3f(1.0,0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(lineEnd, 0.0, 0.0);
    
    glColor3f(0.0, 0.5, 0.0);
    glVertex3f(0.0, lineStart, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, lineEnd, 0.0);
    
    glColor3f(0.0, 0.0, 0.5);
    glVertex3f(0.0, 0.0, lineStart);
    glVertex3f(0.0, 0.0, 0.0);
    
    glColor3f(0.0,0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, lineEnd);
    
    glEnd();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    
    /*
     * Sphere
     */
    glColor3f(0.7,
              0.7,
              0.7);

    m_sphere->draw();
    
    glPopMatrix();
    
}

/**
 * Create the sphere on which fibers are viewed.
 */
void
FiberSamplesOpenGLWidget::createSphere()
{
    m_sphere = new BrainOpenGLShapeSphere(25,
                                          100);
}

/**
 * Receive mouse press events from Qt.
 * @param me
 *    The mouse event.
 */
void
FiberSamplesOpenGLWidget::mousePressEvent(QMouseEvent* me)
{
    const Qt::MouseButton button = me->button();
    const Qt::KeyboardModifiers keyModifiers = me->modifiers();
    
    if (button == Qt::LeftButton) {
        const int mouseX = me->x();
        const int mouseY = m_widgetHeight - me->y();
        
        m_mousePressX = mouseX;
        m_mousePressY = mouseY;
        
        m_lastMouseX = mouseX;
        m_lastMouseY = mouseY;
    }
    else {
        m_mousePressX = -10000;
        m_mousePressY = -10000;
    }
    
    me->accept();
}

/**
 * Receive mouse button release events from Qt.
 * @param me
 *    The mouse event.
 */
void
FiberSamplesOpenGLWidget::mouseReleaseEvent(QMouseEvent* me)
{
    m_mousePressX = -10000;
    m_mousePressY = -10000;
    
    me->accept();
}

/**
 * Receive mouse move events from Qt.
 * @param me
 *    The mouse event.
 */
void
FiberSamplesOpenGLWidget::mouseMoveEvent(QMouseEvent* me)
{
    const Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    
    if (button == Qt::NoButton) {
        if (me->buttons() == Qt::LeftButton) {
            const int mouseX = me->x();
            const int mouseY = m_widgetHeight - me->y();
            
            const int dx = mouseX - m_lastMouseX;
            const int dy = mouseY - m_lastMouseY;
            const int absDX = (dx >= 0) ? dx : -dx;
            const int absDY = (dy >= 0) ? dy : -dy;
            
            if ((absDX > 0)
                || (absDY > 0)) {
                m_rotationMatrix.rotateX(-dy);
                m_rotationMatrix.rotateY(-dx);
            }
            
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        }
    }
    
    me->accept();
    
    updateGL();
}



