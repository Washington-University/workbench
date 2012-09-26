
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

#ifdef CARET_OS_WINDOWS
#include <Windows.h>
#endif
#ifdef CARET_OS_MACOSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "CaretObject.h"

#undef BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
#undef BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE
#undef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS

#define BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS 1
#define BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE 1
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1

//#ifdef GL_VERSION_1_1
//#define BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE 1
//#define BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS 1
//#endif // GL_VERSION_1_1
//
#ifdef GL_VERSION_2_1
#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
#endif // GL_VERSION_2_1
//
//#ifdef GL_VERSION_3_0
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
//#endif // GL_VERSION_3_0
//
//#ifdef GL_VERSION_4_0
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
//#endif // GL_VERSION_4_0


#include "CaretObject.h"

namespace caret {
    
    class Border;
    class BrainOpenGLTextRenderInterface;
    class BrainOpenGLViewportContent;
    class SurfaceProjectedItem;
    
    /**
     * Performs drawing of graphics using OpenGL.
     */
    class BrainOpenGL : public CaretObject {
        
    protected:
        BrainOpenGL(BrainOpenGLTextRenderInterface* textRenderer);

    public:
        /** Mode for how the shape is drawn, set to most optimal for compilation and runtime systems */
        enum DrawMode {
            /** Draw using display lists */
            DRAW_MODE_DISPLAY_LISTS,
            /** Draw using immediate mode */
            DRAW_MODE_IMMEDIATE,
            /** Invalid */
            DRAW_MODE_INVALID,
            /** Draw using vertex buffers */
            DRAW_MODE_VERTEX_BUFFERS
        };
        
        virtual ~BrainOpenGL();
        
        /**
         * Initialize the OpenGL system.
         */
        virtual void initializeOpenGL();
        
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
         * @param applySelectionBackgroundFiltering
         *    If true (which is in most cases), if there are multiple items
         *    identified, those items "behind" other items are not reported.
         *    For example, suppose a focus is identified and there is a node
         *    the focus.  If this parameter is true, the node will NOT be
         *    identified.  If this parameter is false, the node will be
         *    identified.
         */
        virtual void selectModel(BrainOpenGLViewportContent* viewportContent,
                                 const int32_t mouseX,
                                 const int32_t mouseY,
                                 const bool applySelectionBackgroundFiltering) = 0;
        
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
         * @return Half-size of the model window height.
         */
        static float getModelViewingHalfWindowHeight() { return 90.0f; }
        
        void setBorderBeingDrawn(Border* borderBeingDrawn);
        
        static void getMinMaxPointSize(float& minPointSizeOut, float& maxPointSizeOut);
        
        static void getMinMaxLineWidth(float& minLineWidthOut, float& maxLineWidthOut);
        
        static AString getRuntimeVersionOfOpenGL();
        
        static bool isRuntimeVersionOfOpenGLSupported(const AString& versionNumber);
        
        static QString getBestDrawingModeName();
        
        static AString getOpenGLInformation();
        
        /**
         * @return The best drawing mode given the limitations of
         * the compile and run-time systems.
         */
        static DrawMode getBestDrawingMode() {
            return s_drawingMode;
        }
        
        /**
         * @return True if display list drawing is supported.
         */
        inline static bool isDisplayListsSupported() {
            return s_supportsDisplayLists;
        }
        
        /**
         * @return True if immediate mode drawing is supported.
         */
        inline static bool isImmediateSupported() {
            return s_supportsImmediateMode;
        }
        
        /**
         * @return True if vertex buffer drawing is supported.
         */
        inline static bool isVertexBuffersSupported() {
            return s_supportsVertexBuffers;
        }
        
    private:
        BrainOpenGL(const BrainOpenGL&);
        BrainOpenGL& operator=(const BrainOpenGL&);
        
        
    protected:
        /** Optional text rendering (if not null) */
        BrainOpenGLTextRenderInterface* textRenderer;
        
        Border* borderBeingDrawn;
        
        /** minimum point size */
        static float s_minPointSize;
        
        /** maximum point size */
        static float s_maxPointSize;
        
        /** minimum line width */
        static float s_minLineWidth;
        
        /** maximum line width */
        static float s_maxLineWidth;
        
    private:
        static void getOpenGLMajorMinorVersions(const AString& versionString,
                                                AString& majorVersionOut,
                                                AString& minorVersionOut);
        
        /** version of OpenGL */
        static AString s_versionOfOpenGL;
        
        /** major version number of OpenGL */
        static AString s_majorVersionOfOpenGL;
        
        /** minor version number of OpenGL */
        static AString s_minorVersionOfOpenGL;
        
        static DrawMode s_drawingMode;
        
        static bool s_supportsDisplayLists;
        
        static bool s_supportsImmediateMode;
        
        static bool s_supportsVertexBuffers;
        
        static AString s_openGLInformation;
    };

#ifdef __BRAIN_OPENGL_DEFINE_H
    AString BrainOpenGL::s_versionOfOpenGL = "";
    AString BrainOpenGL::s_majorVersionOfOpenGL = "";
    AString BrainOpenGL::s_minorVersionOfOpenGL = "";
    float BrainOpenGL::s_minPointSize = 1.0f;
    float BrainOpenGL::s_maxPointSize = 10.0f;
    float BrainOpenGL::s_minLineWidth = 1.0f;
    float BrainOpenGL::s_maxLineWidth = 10.0f;
    BrainOpenGL::DrawMode BrainOpenGL::s_drawingMode = BrainOpenGL::DRAW_MODE_INVALID;
    bool BrainOpenGL::s_supportsDisplayLists  = false;
    bool BrainOpenGL::s_supportsImmediateMode = false;
    bool BrainOpenGL::s_supportsVertexBuffers = false;
    AString BrainOpenGL::s_openGLInformation = "";
#endif //__BRAIN_OPENGL_DEFINE_H

} // namespace


#endif // __BRAIN_OPENGL_H__
