
#ifndef __BRAIN_OPENGL_H__
#define __BRAIN_OPENGL_H__

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

#include <stdint.h>

#include "CaretObject.h"

namespace caret {
    
    class Border;
    class BrainOpenGLTextRenderInterface;
    class BrainOpenGLViewportContent;
    class IdentificationManager;
    class SurfaceProjectedItem;
    
    /**
     * Performs drawing of graphics using OpenGL.
     */
    class BrainOpenGL : public CaretObject {
        
    protected:
        BrainOpenGL(BrainOpenGLTextRenderInterface* textRenderer);

    public:
        virtual ~BrainOpenGL();
        
        /**
         * Initialize the OpenGL system.
         */
        virtual void initializeOpenGL() = 0;
        
        /**
         * Draw models in their respective viewports.
         *
         * @param viewportContents
         *    Viewport info for drawing.
         */
        virtual void drawModels(std::vector<BrainOpenGLViewportContent*>& viewportContents) = 0;
        
        /**
         * Selection on a model.
         *
         * @param viewportContent
         *    Viewport content in which mouse was clicked
         * @param mouseX
         *    X position of mouse click
         * @param mouseY
         *    Y position of mouse click
         */
        virtual void selectModel(BrainOpenGLViewportContent* viewportContent,
                         const int32_t mouseX,
                         const int32_t mouseY) = 0;
        
        /**
         * Project the given window coordinate to the active models.
         * If the projection is successful, The 'original' XYZ
         * coordinate in 'projectionOut' will be valid.  In addition,
         * the barycentric coordinate may also be valid in 'projectionOut'.
         *
         * @param viewportContent
         *    Viewport content in which mouse was clicked
         * @param mouseX
         *    X position of mouse click
         * @param mouseY
         *    Y position of mouse click
         */
        virtual void projectToModel(BrainOpenGLViewportContent* viewportContent,
                                    const int32_t mouseX,
                                    const int32_t mouseY,
                                    SurfaceProjectedItem& projectionOut) = 0;
        
        /**
         * Initializle the orthographic viewport.
         * @param windowIndex
         *    Index of the window.
         * @param width
         *    Width of the window.
         * @param height
         *    Height of the window.
         */
        virtual void updateOrthoSize(const int32_t windowIndex, 
                             const int32_t width, 
                             const int32_t height) = 0;

        /**
         * @return Half-size of the model window height.
         */
        static float getModelViewingHalfWindowHeight() { return 90.0f; }
        
        /**
         * @return The identification manager.
         */
        IdentificationManager* getIdentificationManager();

        void setBorderBeingDrawn(Border* borderBeingDrawn);
        
        static void getMinMaxPointSize(float& minPointSizeOut, float& maxPointSizeOut);
        
        static void getMinMaxLineWidth(float& minLineWidthOut, float& maxLineWidthOut);
        
    private:
        BrainOpenGL(const BrainOpenGL&);
        BrainOpenGL& operator=(const BrainOpenGL&);
        
        
    protected:
        /** Optional text rendering (if not null) */
        BrainOpenGLTextRenderInterface* textRenderer;
        
        Border* borderBeingDrawn;
        
        /** version number of OpenGL */
        static float versionOfOpenGL;

        /** minimum point size */
        static float minPointSize;
        
        /** maximum point size */
        static float maxPointSize;
        
        /** minimum line width */
        static float minLineWidth;
        
        /** maximum line width */
        static float maxLineWidth;
        
    private:
        /** Identification manager */
        IdentificationManager* identificationManager;
    };

#ifdef __BRAIN_OPENGL_DEFINE_H
    float BrainOpenGL::versionOfOpenGL = 0.0f;
    float BrainOpenGL::minPointSize = 1.0f;
    float BrainOpenGL::maxPointSize = 10.0f;    
    float BrainOpenGL::minLineWidth = 1.0f;
    float BrainOpenGL::maxLineWidth = 10.0f;    
#endif //__BRAIN_OPENGL_DEFINE_H

} // namespace


#endif // __BRAIN_OPENGL_H__
