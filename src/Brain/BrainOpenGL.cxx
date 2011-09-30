

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

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemSurfaceTriangle.h"
#include "IdentificationWithColor.h"
#include "IdentificationManager.h"
#include "Surface.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include <cstdlib>

using namespace caret;

/**
 * Constructor.
 */
BrainOpenGL::BrainOpenGL()
{
    this->identificationManager = new IdentificationManager();
    this->colorIdentification   = new IdentificationWithColor();
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
    delete this->identificationManager;
    this->identificationManager = NULL;
    
    delete this->colorIdentification;
    this->colorIdentification = NULL;
}

/**
 * Selection on a model.
 *
 * @param modelDisplayController
 *    Model display controller that is drawn (NULL if nothing to draw).
 * @param browserTabContent
 *    Content in the browser' tab.
 * @param windowTabIndex
 *    Index of window TAB in which controller is drawn.
 * @param viewport
 *    Viewport for drawing.
 * @param mouseX
 *    X position of mouse click
 * @param mouseY
 *    Y position of mouse click
 */
void 
BrainOpenGL::selectModel(ModelDisplayController* modelDisplayController,
                 BrowserTabContent* browserTabContent,
                 const int32_t windowTabIndex,
                 const int32_t viewport[4],
                 const int32_t mouseX,
                 const int32_t mouseY)
{
    this->mouseX = mouseX;
    this->mouseY = mouseY;
    
    //this->identificationManager->reset();
    this->colorIdentification->reset();

    this->drawModelInternal(MODE_IDENTIFICATION,
                            modelDisplayController, 
                            browserTabContent, 
                            windowTabIndex, 
                            viewport);
}

/**
 * Draw a model.
 *
 * @param modelDisplayController
 *    Model display controller that is drawn (NULL if nothing to draw).
 * @param browserTabContent
 *    Content in the browser' tab.
 * @param windowTabIndex
 *    Index of window TAB in which controller is drawn.
 * @param viewport
 *    Viewport for drawing.
 */
void 
BrainOpenGL::drawModel(ModelDisplayController* modelDisplayController,
                       BrowserTabContent* browserTabContent,
                       const int32_t windowTabIndex,
                       const int32_t viewport[4])
{
    this->drawModelInternal(MODE_DRAWING,
                            modelDisplayController, 
                            browserTabContent, 
                            windowTabIndex, 
                            viewport);
}
    
/**
 * Draw a model.
 *
 * @param mode
 *    The mode of operations (draw, select, etc).
 * @param modelDisplayController
 *    Model display controller that is drawn (NULL if nothing to draw).
 * @param browserTabContent
 *    Content in the browser' tab.
 * @param windowTabIndex
 *    Index of window TAB in which controller is drawn.
 * @param viewport
 *    Viewport for drawing.
 */
void 
BrainOpenGL::drawModelInternal(Mode mode,
                               ModelDisplayController* modelDisplayController,
                       BrowserTabContent* browserTabContent,
                       const int32_t windowTabIndex,
                       const int32_t viewport[4])
{
    this->mode = mode;
    
    this->browserTabContent = browserTabContent;
    this->windowTabIndex = windowTabIndex;
    
    if (this->mode == MODE_IDENTIFICATION) {
        /*
         * For identification, set the background
         * to white.
         */
        glClearColor(1.0, 1.0, 1.0, 0.0);
    }
    else {
        float backgroundColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glClearColor(backgroundColor[0],
                     backgroundColor[1],
                     backgroundColor[2],
                     backgroundColor[3]);
    }
    
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
        ModelDisplayControllerVolume* volumeController = dynamic_cast<ModelDisplayControllerVolume*>(modelDisplayController);
        ModelDisplayControllerWholeBrain* wholeBrainController = dynamic_cast<ModelDisplayControllerWholeBrain*>(modelDisplayController);
        if (surfaceController != NULL) {
            this->drawSurface(surfaceController->getSurface());
        }
        else if (volumeController != NULL) {
            this->drawVolume(browserTabContent,
                             volumeController);
        }
        else if (wholeBrainController != NULL) {
            this->drawWholeBrain(browserTabContent,
                                 wholeBrainController);
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
        CaretLogConfig("OpenGL version: " + AString::number(BrainOpenGL::versionOfOpenGL));
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
 * Enable lighting based upon the current mode.
 */
void 
BrainOpenGL::enableLighting()
{
    float lightPosition[] = { 0.0f, 0.0f, 1000.0f, 0.0f };
    switch (this->mode) {
        case MODE_DRAWING:
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
            break;
        case MODE_IDENTIFICATION:
            this->disableLighting();
            break;
    }
}

/**
 * Disable lighting.
 */
void 
BrainOpenGL::disableLighting()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

/**
 * Draw a surface.
 * @param surface
 *    Surface that is drawn.
 */
void 
BrainOpenGL::drawSurface(const Surface* surface)
{
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    
    this->enableLighting();
    
    switch (this->mode) {
        case MODE_DRAWING:
            this->drawSurfaceTrianglesWithVertexArrays(surface);
            break;
        case MODE_IDENTIFICATION:
            this->drawSurfaceNodes(surface);
            this->drawSurfaceTriangles(surface);
            break;
    }
    
    this->disableLighting();
}

/**
 * Draw a surface as individual triangles.
 * @param surface
 *    Surface that is drawn.
 */
void 
BrainOpenGL::drawSurfaceTriangles(const Surface* surface)
{
    const int numTriangles = surface->getNumberOfTriangles();
    
    const int32_t* triangles = surface->getTriangle(0);
    const float* coordinates = surface->getCoordinate(0);
    const float* normals     = surface->getNormalVector(0);
    const float* rgbaColoring = this->browserTabContent->getSurfaceColoring(surface);

    IdentificationItemSurfaceTriangle* triangleID = 
        this->identificationManager->getSurfaceTriangleIdentification();
    bool isSelect = false;
    if (this->isIdentifyMode()) {
        if (triangleID->isEnabledForSelection()) {
            isSelect = true;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        else {
            return;
        }
    }
    
    uint8_t rgb[3];
    
    glBegin(GL_TRIANGLES);
    for (int32_t i = 0; i < numTriangles; i++) {
        const int32_t i3 = i * 3;
        const int32_t n1 = triangles[i3];
        const int32_t n2 = triangles[i3+1];
        const int32_t n3 = triangles[i3+2];
        
        if (isSelect) {
            this->colorIdentification->addItem(rgb, IdentificationItemDataTypeEnum::SURFACE_TRIANGLE, i);
            glColor3ubv(rgb);
            glNormal3fv(&normals[n1*3]);
            glVertex3fv(&coordinates[n1*3]);
            glNormal3fv(&normals[n2*3]);
            glVertex3fv(&coordinates[n2*3]);
            glNormal3fv(&normals[n3*3]);
            glVertex3fv(&coordinates[n3*3]);
        }
        else {
            glColor4fv(&rgbaColoring[n1*4]);
            glNormal3fv(&normals[n1*3]);
            glVertex3fv(&coordinates[n1*3]);
            glColor4fv(&rgbaColoring[n2*4]);
            glNormal3fv(&normals[n2*3]);
            glVertex3fv(&coordinates[n2*3]);
            glColor4fv(&rgbaColoring[n3*4]);
            glNormal3fv(&normals[n3*3]);
            glVertex3fv(&coordinates[n3*3]);
        }
    }
    glEnd();
    
    if (isSelect) {
        const int triangleIndex = this->getIndexFromColorSelection(IdentificationItemDataTypeEnum::SURFACE_TRIANGLE, this->mouseX, this->mouseY);
        
        CaretLogFine("Selected Triangle: " + QString::number(triangleIndex));
        
    }
}

/**
 * Draw a surface as individual nodes.
 * @param surface
 *    Surface that is drawn.
 */
void 
BrainOpenGL::drawSurfaceNodes(const Surface* surface)
{
    const int numNodes = surface->getNumberOfNodes();
    
    const float* coordinates = surface->getCoordinate(0);
    const float* normals     = surface->getNormalVector(0);
    const float* rgbaColoring = this->browserTabContent->getSurfaceColoring(surface);
    
    IdentificationItemSurfaceNode* nodeID = 
    this->identificationManager->getSurfaceNodeIdentification();
    bool isSelect = false;
    if (this->isIdentifyMode()) {
        if (nodeID->isEnabledForSelection()) {
            isSelect = true;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);            
        }
        else {
            return;
        }
    }
    
    uint8_t rgb[3];
    
    glPointSize(2.0);
    glBegin(GL_POINTS);
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t i3 = i * 3;
        
        if (isSelect) {
            this->colorIdentification->addItem(rgb, IdentificationItemDataTypeEnum::SURFACE_NODE, i);
            glColor3ubv(rgb);
            glNormal3fv(&normals[i3]);
            glVertex3fv(&coordinates[i3]);
        }
        else {
            glColor4fv(&rgbaColoring[i*4]);
            glNormal3fv(&normals[i3]);
            glVertex3fv(&coordinates[i3]);
        }
    }
    glEnd();
    
    if (isSelect) {
        const int nodeIndex = this->getIndexFromColorSelection(IdentificationItemDataTypeEnum::SURFACE_NODE, this->mouseX, this->mouseY);
        
        CaretLogFine("Selected Node: " + QString::number(nodeIndex));
        
    }
}


/**
 * Draw a surface triangles with vertex arrays.
 * @param surface
 *    Surface that is drawn.
 */
void 
BrainOpenGL::drawSurfaceTrianglesWithVertexArrays(const Surface* surface)
{
    const float* rgbaColoring = this->browserTabContent->getSurfaceColoring(surface);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, 
                    GL_FLOAT, 
                    0, 
                    reinterpret_cast<const GLvoid*>(surface->getCoordinate(0)));
    glColorPointer(4, 
                   GL_FLOAT, 
                   0, 
                   reinterpret_cast<const GLvoid*>(rgbaColoring));
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
 * Draw the volume slices.
 * @param browserTabContent
 *    Content of the window.
 * @param volumeController
 *    Controller for slices.
 */
void 
BrainOpenGL::drawVolume(BrowserTabContent* browserTabContent,
                        ModelDisplayControllerVolume* volumeController)
{
    
}

/**
 * Draw the whole brain.
 * @param browserTabContent
 *    Content of the window.
 * @param wholeBrainController
 *    Controller for whole brain.
 */
void 
BrainOpenGL::drawWholeBrain(BrowserTabContent* browserTabContent,
                            ModelDisplayControllerWholeBrain* wholeBrainController)
{
    const int32_t tabNumberIndex = browserTabContent->getTabNumber();
    const SurfaceTypeEnum::Enum surfaceType = wholeBrainController->getSelectedSurfaceType(tabNumberIndex);
    
    
    Brain* brain = wholeBrainController->getBrain();
    const int32_t numberOfBrainStructures = brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numberOfBrainStructures; i++) {
        BrainStructure* brainStructure = brain->getBrainStructure(i);
        const int32_t numberOfSurfaces = brainStructure->getNumberOfSurfaces();
        for (int32_t j = 0; j < numberOfSurfaces; j++) {
            const Surface* surface = brainStructure->getSurface(j);
                        
            if (surface->getSurfaceType() == surfaceType) {
                
                float dx = 0.0;
                float dy = 0.0;
                float dz = 0.0;
                
                bool drawIt = false;
                const StructureEnum::Enum structure = surface->getStructure();
                switch (structure) {
                    case StructureEnum::CORTEX_LEFT:
                        drawIt = wholeBrainController->isLeftEnabled(tabNumberIndex);
                        dx = -wholeBrainController->getLeftRightSeparation(tabNumberIndex);
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        drawIt = wholeBrainController->isRightEnabled(tabNumberIndex);
                        dx = wholeBrainController->getLeftRightSeparation(tabNumberIndex);
                        break;
                    case StructureEnum::CEREBELLUM:
                        drawIt = wholeBrainController->isCerebellumEnabled(tabNumberIndex);
                        dy = wholeBrainController->getCerebellumSeparation(tabNumberIndex);
                        break;
                    default:
                        break;
                }
                
                if (drawIt) {
                    glPushMatrix();
                    glTranslatef(dx, dy, dz);
                    this->drawSurface(surface);
                    glPopMatrix();
                }
            }
        }
    }
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

/**
 * @return The identification manager.
 */
IdentificationManager* 
BrainOpenGL::getIdentificationManager()
{
    return this->identificationManager;
}

int32_t 
BrainOpenGL::getIndexFromColorSelection(IdentificationItemDataTypeEnum::Enum dataType,
                                        const int32_t x,
                                        const int32_t y)
{
    // Figure out item was picked using color in color buffer
    //
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    uint8_t pixels[3];
    glReadPixels((int)x,
                    (int)y,
                    1,
                    1,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    pixels);
    
    int32_t itemIndex = -1;
    
    CaretLogFine("ID color is "
                 + QString::number(pixels[0]) + ", "
                 + QString::number(pixels[1]) + ", "
                 + QString::number(pixels[2]));
    
    this->colorIdentification->getItem(pixels, dataType, &itemIndex);
    
    if (itemIndex >= 0) {
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        float depth;
        glReadPixels(x,
                     y,
                        1,
                        1,
                        GL_DEPTH_COMPONENT,
                        GL_FLOAT,
                        &depth);
    }

    return itemIndex;
}

