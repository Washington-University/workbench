

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <iostream>
#ifdef CARET_OS_WINDOWS
#include <Windows.h>
#endif
#ifdef CARET_OS_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define __BRAIN_OPENGL_DEFINE_H
#include "BrainOpenGL.h"
#undef __BRAIN_OPENGL_DEFINE_H
#include "BrainStructure.h"
#include "Surface.h"
#include "ModelDisplayControllerSurface.h"
#include <cstdlib>

using namespace caret;

/**
 * Constructor.
 */
BrainOpenGL::BrainOpenGL()
{
    
}

/**
 * Get the BrainOpenGL for drawing.  Users
 * should never call this.  Get BrainOpenGL
 * from GuiGlobals.
 *
 * @return 
 *    Pointer to BrainOpenGL for drawing.
 */
BrainOpenGL* 
BrainOpenGL::getBrainOpenGL()
{
    if (BrainOpenGL::brainOpenGLSingleton == NULL) {
        BrainOpenGL::brainOpenGLSingleton = new BrainOpenGL();
    }
    return BrainOpenGL::brainOpenGLSingleton;
}

/**
 * Destructor.
 */
BrainOpenGL::~BrainOpenGL()
{
    
}

/**
 * Draw a model.
 */
void 
BrainOpenGL::drawModel(Brain* brain,
                        const int32_t windowIndex,
                        const int32_t viewport[4],
                        ModelDisplayController* controller)
{
    this->brain = brain;
    this->windowIndex = windowIndex;
    
    float backgroundColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearColor(backgroundColor[0],
                 backgroundColor[1],
                 backgroundColor[2],
                 backgroundColor[3]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(viewport[0], 
               viewport[1], 
               viewport[2], 
               viewport[3]);
    
    if(controller == NULL) {
        return;
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    this->setOrthographicProjection(viewport);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const float* translation = controller->getTranslation(this->windowIndex);
    glTranslatef(translation[0], 
                 translation[1], 
                 translation[2]);
    
    Matrix4x4* rotationMatrix = controller->getViewingRotationMatrix(this->windowIndex);
    double rotationMatrixElements[16];
    rotationMatrix->getMatrixForOpenGL(rotationMatrixElements);
    glMultMatrixd(rotationMatrixElements);
    
    const float scale = controller->getScaling(this->windowIndex);
    glScalef(scale, 
             scale, 
             scale);
    
    ModelDisplayControllerSurface* surfaceController = dynamic_cast<ModelDisplayControllerSurface*>(controller);
    if (surfaceController != NULL) {
        this->drawSurface(surfaceController->getSurface());
    }
    
    this->checkForOpenGLError(controller, "At end of drawModel()");
}

void 
BrainOpenGL::initializeMembersBrainOpenGL()
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        orthographicLeft[i] = -1.0f;
        orthographicRight[i] = -1.0f;
        orthographicBottom[i] = -1.0f;
        orthographicTop[i] = -1.0f;
        orthographicFar[i] = -1.0f;
        orthographicNear[i] = -1.0f;
    }
    
    this->initializedOpenGLFlag = false;
}
/**
 * Initialize OpenGL.
 */
void 
BrainOpenGL::initializeOpenGL()
{
    if (BrainOpenGL::versionOfOpenGL == 0.0) {
        //
        // Note: The version string might be something like 1.2.4.  std::atof()
        // will get just the 1.2 which is okay.
        //
        const char* versionStr = (char*)(glGetString(GL_VERSION));
        BrainOpenGL::versionOfOpenGL = std::atof(versionStr);
        //std::cout << "OpenGL version: " << BrainOpenGL::versionOfOpenGL << std::endl;
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glFrontFace(GL_CCW);
#ifndef GL_VERSION_1_3 
    glEnable(GL_NORMALIZE);
#else
    if (BrainOpenGL::versionOfOpenGL >= 1.3) {
        glEnable(GL_RESCALE_NORMAL);
    }
    else {
        glEnable(GL_NORMALIZE);
    }
#endif
    
    //
    // Avoid drawing backfacing polygons
    //
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    glShadeModel(GL_SMOOTH);
    
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    float lightColor[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    
    float materialColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    
    float ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient); 
    
    if (this->initializedOpenGLFlag) {
        return;
    }
    this->initializedOpenGLFlag = true;
    
    /*
     * Remaining items need to executed only once.
     */
}

/**
 *
 */
void 
BrainOpenGL::updateOrthoSize(const int32_t windowIndex, 
                             const int32_t width,
                             const int32_t height)
{
    const double aspectRatio = (static_cast<double>(width)) /
                               (static_cast<double>(height));
    orthographicRight[windowIndex]  =    getModelViewingHalfWindowHeight() * aspectRatio;
    orthographicLeft[windowIndex]   =   -getModelViewingHalfWindowHeight() * aspectRatio;
    orthographicTop[windowIndex]    =    getModelViewingHalfWindowHeight();
    orthographicBottom[windowIndex] =   -getModelViewingHalfWindowHeight();
    orthographicNear[windowIndex]   = -5000.0; //-500.0; //-10000.0;
    orthographicFar[windowIndex]    =  5000.0; //500.0; // 10000.0;
}

/**
 * Draw a surface.
 */
void 
BrainOpenGL::drawSurface(const Surface* surface)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    float lightPosition[] = { 0.0f, 0.0f, 1000.0f, 0.0f };
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
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_LIGHTING);
    //glEnable(GL_COLOR_MATERIAL);
    
    this->drawSurfaceTriangles(surface);
}

void 
BrainOpenGL::drawSurfaceTriangles(const Surface* surface)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, 
                    GL_FLOAT, 
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getCoordinate(0)));
    glColorPointer(4, 
                   GL_UNSIGNED_BYTE, 
                   0, 
                   reinterpret_cast<const GLvoid*>(surface->getNodeColor(0)));
    glNormalPointer(GL_FLOAT, 
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getNormalVector(0)));
    
    const int numTriangles = surface->getNumberOfTriangles();
    glDrawElements(GL_TRIANGLES, 
                   (3 * numTriangles), 
                   GL_UNSIGNED_INT,
                   reinterpret_cast<const GLvoid*>(surface->getTriangle(0)));
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

/**
 * Check for OpenGL errors.
 */
void 
BrainOpenGL::checkForOpenGLError()
{
    
}

/**
 * Setup the orthographic projection.
 */
void 
BrainOpenGL::setOrthographicProjection(const int32_t viewport[4])
{
     double defaultOrthoWindowSize = BrainOpenGL::getModelViewingHalfWindowHeight();
     double width = viewport[2];
     double height = viewport[3];
     double aspectRatio = (width / height);
     double orthographicRight  =    defaultOrthoWindowSize * aspectRatio;
     double orthographicLeft   =   -defaultOrthoWindowSize * aspectRatio;
     double orthographicTop    =    defaultOrthoWindowSize;
     double orthographicBottom =   -defaultOrthoWindowSize;
     double orthographicNear   = -1000.0; //-500.0; //-10000.0;
     double orthographicFar    =  1000.0; //500.0; // 10000.0;
    //gl.glMatrixMode(GL.GL_PROJECTION);
    //gl.glLoadIdentity();
     glOrtho(orthographicLeft, orthographicRight, 
               orthographicBottom, orthographicTop, 
               orthographicNear, orthographicFar);    
}

/**
 * check for an OpenGL Error.
 */
void 
BrainOpenGL::checkForOpenGLError(const ModelDisplayController* modelController,
                                      const AString& msg)
{
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        std::cout << std::endl;
        std::cout << "OpenGL Error: " << (char*)gluErrorString(errorCode) << std::endl;
        std::cout << "OpenGL Version: " << (char*)(glGetString(GL_VERSION)) << std::endl;
        std::cout << "OpenGL Vendor:  " << (char*)(glGetString(GL_VENDOR)) << std::endl;
        if (msg.length() == 0) {
            std::cout << msg.toStdString() << std::endl;
        }
        if (modelController != NULL) {
            std::cout << "While drawing brain model " << modelController->getNameForGUI(true).toStdString() << std::endl;
        }
        std::cout << "In window number " << this->windowIndex << std::endl;
        GLint nameStackDepth, modelStackDepth, projStackDepth;
        glGetIntegerv(GL_PROJECTION_STACK_DEPTH,
                      &projStackDepth);
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,
                      &modelStackDepth);
        glGetIntegerv(GL_NAME_STACK_DEPTH,
                      &nameStackDepth);
        std::cout << "Projection Matrix Stack Depth " << projStackDepth << std::endl;
        std::cout << "Model Matrix Stack Depth " << modelStackDepth << std::endl;
        std::cout << "Name Matrix Stack Depth " << nameStackDepth << std::endl;
        std::cout << std::endl;
    }
}

