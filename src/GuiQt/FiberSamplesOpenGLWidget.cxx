
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

#include <cmath>

#include <QCheckBox>
#include <QLabel>
#include <QMouseEvent>
#include <QSizePolicy>

#define __FIBER_SAMPLES_OPEN_G_L_WIDGET_DECLARE__
#include "FiberSamplesOpenGLWidget.h"
#undef __FIBER_SAMPLES_OPEN_G_L_WIDGET_DECLARE__

using namespace caret;

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrainOpenGLShapeRing.h"
#include "BrainOpenGLShapeSphere.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "Fiber.h"
#include "FiberOrientation.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"

/**
 * \class caret::FiberSamplesOpenGLWidget 
 * \brief OpenGL Widget for drawing fiber samples on a sphere
 * \defgroup GuiQt
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
FiberSamplesOpenGLWidget::FiberSamplesOpenGLWidget(const int32_t browserWindowIndex,
                                                   QCheckBox* enabledCheckBox,
                                                   QLabel* fiberMeanLabels[3],
                                                   QLabel* fiberVarianceLabels[3],
                                                   QWidget* parent)
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
: QOpenGLWidget(parent)
#else
: QGLWidget(parent)
#endif
{
    m_browserWindowIndex = browserWindowIndex;
    m_enabledCheckBox = enabledCheckBox;
    for (int32_t i = 0; i < 3; i++) {
        m_fiberMeanLabels[i] = fiberMeanLabels[i];
        m_fiberVarianceLabels[i] = fiberVarianceLabels[i];
    }
    m_ring = NULL;
    m_sphereBig = NULL;
    m_sphereSmall = NULL;
    
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::Expanding);
}

/**
 * Destructor.
 */
FiberSamplesOpenGLWidget::~FiberSamplesOpenGLWidget()
{
    makeCurrent();
    
    if (m_ring != NULL) {
        delete m_ring;
    }
    if (m_sphereBig != NULL) {
        delete m_sphereBig;
    }
    if (m_sphereSmall != NULL) {
        delete m_sphereSmall;
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
        
    createShapes();
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

    
    const GLfloat lineStart = -s_sphereBigRadius * 5.0;
    const GLfloat lineEnd   =  s_sphereBigRadius * 5.0;
    
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
//    glColor3f(0.7,
//              0.7,
//              0.7);

    const float rgba[4] = {
        0.7,
        0.7,
        0.7,
        1.0
    };
    m_sphereBig->draw(rgba);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    
    if (m_enabledCheckBox->isChecked()) {
        drawOrientations();
    }
    
    glPopMatrix();
    
}

/**
 * Draw the fibers on the sphere
 */
void
FiberSamplesOpenGLWidget::drawOrientations()
{
    glDisable(GL_CULL_FACE);
    
    DisplayPropertiesFiberOrientation* dpfo = GuiManager::get()->getBrain()->getDisplayPropertiesFiberOrientation();
    
    std::vector<FiberOrientationSamplesVector> xVectors;
    std::vector<FiberOrientationSamplesVector> yVectors;
    std::vector<FiberOrientationSamplesVector> zVectors;
    FiberOrientation* fiberOrientation;
    AString errorMessage;
    Brain* brain = GuiManager::get()->getBrain();
    if (brain->getFiberOrientationSphericalSamplesVectors(xVectors,
                                             yVectors,
                                             zVectors,
                                             fiberOrientation,
                                             errorMessage)) {
        
        const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(this->m_browserWindowIndex);
        const FiberOrientationColoringTypeEnum::Enum coloringType = dpfo->getColoringType(displayGroup,
                                                                                          m_browserWindowIndex);
        const float minimumMagnitude = dpfo->getMinimumMagnitude(displayGroup,
                                                                 m_browserWindowIndex);

        /*
         * First orientations
         */
        const int32_t numVectorsX = static_cast<int32_t>(xVectors.size());
        for (int32_t i = 0; i < numVectorsX; i++) {
            if (xVectors[i].magnitude < minimumMagnitude) {
                continue;
            }
            float xyz[3] = {
                xVectors[i].direction[0] * s_sphereBigRadius,
                xVectors[i].direction[1] * s_sphereBigRadius,
                xVectors[i].direction[2] * s_sphereBigRadius
            };
            
            float rgba[4] = { 0.0, 0.0, 0.0, 255.0 };
            switch (coloringType) {
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                    //glColor3f(1.0, 0.0, 0.0);
                    rgba[0] = 1.0;
                    break;
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                    //glColor3fv(xVectors[i].rgb);
                    rgba[0] = xVectors[i].rgb[0];
                    rgba[1] = xVectors[i].rgb[1];
                    rgba[2] = xVectors[i].rgb[2];
                    break;
            }
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            m_sphereSmall->draw(rgba);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-xyz[0], -xyz[1], -xyz[2]);
            m_sphereSmall->draw(rgba);
            glPopMatrix();
        }
        
        /*
         * Second orientations
         */
        const int32_t numVectorsY = static_cast<int32_t>(yVectors.size());
        for (int32_t i = 0; i < numVectorsY; i++) {
            if (yVectors[i].magnitude < minimumMagnitude) {
                continue;
            }
            float xyz[3] = {
                yVectors[i].direction[0] * s_sphereBigRadius,
                yVectors[i].direction[1] * s_sphereBigRadius,
                yVectors[i].direction[2] * s_sphereBigRadius
            };
            float rgba[4] = { 0.0, 0.0, 0.0, 255.0 };
            switch (coloringType) {
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                    //glColor3f(0.0, 0.0, 1.0);  // BLUE (RBG!)
                    rgba[2] = 1.0;
                    break;
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                    //glColor3fv(yVectors[i].rgb);
                    rgba[0] = yVectors[i].rgb[0];
                    rgba[1] = yVectors[i].rgb[1];
                    rgba[2] = yVectors[i].rgb[2];
                    break;
            }
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            m_sphereSmall->draw(rgba);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-xyz[0], -xyz[1], -xyz[2]);
            m_sphereSmall->draw(rgba);
            glPopMatrix();
        }
        
        /*
         * Third orientations
         */
        const int32_t numVectorsZ = static_cast<int32_t>(zVectors.size());
        for (int32_t i = 0; i < numVectorsZ; i++) {
            if (zVectors[i].magnitude < minimumMagnitude) {
                continue;
            }
            float xyz[3] = {
                zVectors[i].direction[0] * s_sphereBigRadius,
                zVectors[i].direction[1] * s_sphereBigRadius,
                zVectors[i].direction[2] * s_sphereBigRadius
            };
            float rgba[4] = { 0.0, 0.0, 0.0, 255.0 };
            switch (coloringType) {
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                    //glColor3f(0.0, 1.0, 0.0);  // GREEN (RBG!)
                    rgba[1] = 1.0;
                    break;
                case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                    //glColor3fv(zVectors[i].rgb);
                    rgba[0] = zVectors[i].rgb[0];
                    rgba[1] = zVectors[i].rgb[1];
                    rgba[2] = zVectors[i].rgb[2];
                    break;
            }
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            m_sphereSmall->draw(rgba);
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(-xyz[0], -xyz[1], -xyz[2]);
            m_sphereSmall->draw(rgba);
            glPopMatrix();
        }
        
        double viewingMatrixArray[16];
        glGetDoublev(GL_MODELVIEW_MATRIX,
                     viewingMatrixArray);
        Matrix4x4 viewingMatrix;
        viewingMatrix.setMatrixFromOpenGL(viewingMatrixArray);
        Matrix4x4 inverseViewingMatrix(viewingMatrix);
        inverseViewingMatrix.invert();
        inverseViewingMatrix.setTranslation(0.0, 0.0, 0.0);
        double inverseViewingMatrixArray[16];
        inverseViewingMatrix.getMatrixForOpenGL(inverseViewingMatrixArray);
        
        /*
         * Orientation Ellipse
         */
        const float radiansToDegrees = 180.0 / M_PI;
        if (fiberOrientation != NULL) {
            const int32_t numFibers = fiberOrientation->m_numberOfFibers;
            for (int32_t j = 0; j < numFibers; j++) {
                const Fiber* fiber = fiberOrientation->m_fibers[j];
                if (fiber->m_valid) {
                    if (j < 3) {
                        m_fiberMeanLabels[j]->setText(AString::number(fiber->m_meanF));
                        m_fiberVarianceLabels[j]->setText(AString::number(fiber->m_varF));
                    }
                    
                    /*
                     * Only draw if magnitude exceeds minimum magnitude
                     */
                    if (fiber->m_meanF < minimumMagnitude) {
                        continue;
                    }

                    float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
                    switch (coloringType) {
                        case FiberOrientationColoringTypeEnum::FIBER_COLORING_FIBER_INDEX_AS_RGB:
                        {
                            const int32_t colorIndex = j % 3;
                            switch (colorIndex) {
                                case 0:
                                    rgba[0] = 1.0;
                                    //glColor3f(1.0, 0.0, 0.0);
                                    break;
                                case 1:
                                    rgba[2] = 1.0;
                                    //glColor3f(0.0, 0.0, 1.0);
                                    break;
                                case 2:
                                    rgba[1] = 1.0;
                                    //glColor3f(0.0, 1.0, 0.0);
                                    break;
                            }
                        }
                            break;
                        case FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB:
                            //glColor3fv(fiber->m_directionUnitVectorRGB);
                            rgba[0] = fiber->m_directionUnitVectorRGB[0];
                            rgba[1] = fiber->m_directionUnitVectorRGB[1];
                            rgba[2] = fiber->m_directionUnitVectorRGB[2];
                            break;
                    }
                    
                    const float maxAngle = M_PI_2 * 0.95;
                    float baseMajorAngle = fiber->m_fanningMajorAxisAngle;
                    if (baseMajorAngle > maxAngle) {
                        baseMajorAngle = maxAngle;
                    }
                    float baseMinorAngle = fiber->m_fanningMinorAxisAngle;
                    if (baseMinorAngle > maxAngle) {
                        baseMinorAngle = maxAngle;
                    }
                    
                    const float z = s_sphereBigRadius;
                    const float baseRadiusScaling = 1.0;
                    const float maxWidth = z;
                    const float majorAxis = std::min(z * std::tan(baseMajorAngle) * baseRadiusScaling,
                                                     maxWidth);
                    const float minorAxis = std::min(z * std::tan(baseMinorAngle) * baseRadiusScaling,
                                                     maxWidth);
                    
                    glPushMatrix();
                    glRotatef(-fiber->m_phi * radiansToDegrees, 0.0, 0.0, 1.0);
                    glRotatef(-fiber->m_theta * radiansToDegrees, 0.0, 1.0, 0.0);
                    glRotatef(-fiber->m_psi * radiansToDegrees, 0.0, 0.0, 1.0);

                    glPushMatrix();
                    glTranslatef(0.0, 0.0, s_sphereBigRadius);
                    glScalef(majorAxis * 4.0,
                             minorAxis * 4.0,
                             1.0);
                    m_ring->draw(rgba);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glTranslatef(0.0, 0.0, -s_sphereBigRadius);
                    glScalef(majorAxis * 4.0,
                             minorAxis * 4.0,
                             1.0);
                    m_ring->draw(rgba);
                    glPopMatrix();
                    
                    glPopMatrix();
                    
                    
                    
                }
            }
        }
    }
    else {
        if (errorMessage.isEmpty() == false) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
    }
    
    glEnable(GL_CULL_FACE);
}


/**
 * Create the shapes on which fibers are viewed.
 */
void
FiberSamplesOpenGLWidget::createShapes()
{
    if (m_sphereBig == NULL) {
        m_sphereBig = new BrainOpenGLShapeSphere(25,
                                          s_sphereBigRadius);
    }
    if (m_sphereSmall == NULL) {
        m_sphereSmall = new BrainOpenGLShapeSphere(5,
                                              s_sphereSmallRadius);
    }
    
    if (m_ring == NULL) {
        m_ring = new BrainOpenGLShapeRing(10,
                                     0.75,
                                     1.0);
    }
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
//    const Qt::KeyboardModifiers keyModifiers = me->modifiers();
    
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
//    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    
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
                m_rotationMatrix.rotateY(dx);
            }
            
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        }
    }
    
    me->accept();
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    this->update();
#else
    this->updateGL();
#endif
}

