

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
#include "CaretAssert.h"
#include "CaretLogger.h"
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
 *
 * @param modelDisplayController
 *    Model display controller that is drawn (NULL if nothing to draw).
 * @param windowTabIndex
 *    Index of window TAB in which controller is drawn.
 * @param viewport
 *    Viewport for drawing.
 */
void 
BrainOpenGL::drawModel(ModelDisplayController* modelDisplayController,
                       const int32_t windowTabIndex,
                       const int32_t viewport[4])
{
    this->windowTabIndex = windowTabIndex;

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
    
    if(modelDisplayController != NULL) {
        CaretAssert((this->windowTabIndex >= 0) && (this->windowTabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        this->setOrthographicProjection(viewport);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        const float* translation = modelDisplayController->getTranslation(this->windowTabIndex);
        glTranslatef(translation[0], 
                     translation[1], 
                     translation[2]);
        
        Matrix4x4* rotationMatrix = modelDisplayController->getViewingRotationMatrix(this->windowTabIndex);
        double rotationMatrixElements[16];
        rotationMatrix->getMatrixForOpenGL(rotationMatrixElements);
        glMultMatrixd(rotationMatrixElements);
        
        const float scale = modelDisplayController->getScaling(this->windowTabIndex);
        glScalef(scale, 
                 scale, 
                 scale);
        
        ModelDisplayControllerSurface* surfaceController = dynamic_cast<ModelDisplayControllerSurface*>(modelDisplayController);
        if (surfaceController != NULL) {
            this->drawSurface(surfaceController->getSurface());
        }
        else {
            CaretAssertMessage(0, "Unknown type of model display controller for drawing");
        }
    }
    
    this->checkForOpenGLError(modelDisplayController, "At end of drawModel()");
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
        CaretLogInfo("OpenGL version: " + AString::number(BrainOpenGL::versionOfOpenGL));
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
        AString msg;
        msg += ("OpenGL Error: " + AString((char*)gluErrorString(errorCode)) + "\n");
        msg += ("OpenGL Version: " + AString((char*)glGetString(GL_VERSION)) + "\n");
        msg += ("OpenGL Vendor:  " + AString((char*)glGetString(GL_VENDOR)) + "\n");
        if (modelController != NULL) {
            msg += ("While drawing brain model " + modelController->getNameForGUI(true) + "\n");
        }
        msg += ("In window number " + AString::number(this->windowTabIndex) + "\n");
        GLint nameStackDepth, modelStackDepth, projStackDepth;
        glGetIntegerv(GL_PROJECTION_STACK_DEPTH,
                      &projStackDepth);
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,
                      &modelStackDepth);
        glGetIntegerv(GL_NAME_STACK_DEPTH,
                      &nameStackDepth);
        msg += ("Projection Matrix Stack Depth " + AString::number(projStackDepth) + "\n");
        msg += ("Model Matrix Stack Depth " + AString::number(modelStackDepth) + "\n");
        msg += ("Name Matrix Stack Depth " + AString::number(nameStackDepth) + "\n");
        CaretLogSevere(msg);
    }
}

