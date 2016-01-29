
#ifndef __BRAIN_OPENGL_H__
#define __BRAIN_OPENGL_H__

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

#include <set>
#include <stdint.h>

#include "CaretOpenGLInclude.h"

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

#ifdef GL_VERSION_3_0
#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
#endif // GL_VERSION_3_0

#ifdef GL_VERSION_4_0
#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
#endif // GL_VERSION_4_0


#include "CaretObject.h"

namespace caret {
    
    class Border;
    class Brain;
    class BrainOpenGLTextRenderInterface;
    class BrainOpenGLTextureManager;
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
        
        BrainOpenGLTextRenderInterface* getTextRenderer();
        
        void setTextRenderer(BrainOpenGLTextRenderInterface* textRenderer);
        
        /**
         * Draw models in their respective viewports.
         *
         * @param brain
         *    The brain (must be valid!)
         * @param viewportContents
         *    Viewport info for drawing.
         * @param tileTabsEnabledFlag
         *    Indicates if tile tabs is enabled.
         */
        virtual void drawModels(Brain* brain,
                                std::vector<BrainOpenGLViewportContent*>& viewportContents,
                                const bool tileTabsEnabledFlag) = 0;
        
        /**
         * Selection on a model.
         *
         * @param brain
         *    The brain (must be valid!)
         * @param viewportContent
         *    Viewport content in which mouse was clicked
         * @param tileTabsEnabledFlag
         *    Indicates if tile tabs is enabled.
         * @param mouseX
         *    X position of mouse click
         * @param mouseY
         *    Y position of mouse click
         * @param applySelectionBackgroundFiltering
         *    If true (which is in most cases), if there are multiple items
         *    selected, those items "behind" other items are not reported.
         *    For example, suppose a focus is selected and there is a node
         *    the focus.  If this parameter is true, the node will NOT be
         *    selected.  If this parameter is false, the node will be
         *    selected.
         */
        virtual void selectModel(Brain* brain,
                                 BrainOpenGLViewportContent* viewportContent,
                                 const bool tileTabsEnabledFlag,
                                 const int32_t mouseX,
                                 const int32_t mouseY,
                                 const bool applySelectionBackgroundFiltering) = 0;
        
        /**
         * Project the given window coordinate to the active models.
         * If the projection is successful, The 'original' XYZ
         * coordinate in 'projectionOut' will be valid.  In addition,
         * the barycentric coordinate may also be valid in 'projectionOut'.
         *
         * @param brain
         *    The brain (must be valid!)
         * @param viewportContent
         *    Viewport content in which mouse was clicked
         * @param tileTabsEnabledFlag
         *    Indicates if tile tabs is enabled.
         * @param mouseX
         *    X position of mouse click
         * @param mouseY
         *    Y position of mouse click
         */
        virtual void projectToModel(Brain* brain,
                                    BrainOpenGLViewportContent* viewportContent,
                                    const bool tileTabsEnabledFlag,
                                    const int32_t mouseX,
                                    const int32_t mouseY,
                                    SurfaceProjectedItem& projectionOut) = 0;
        
        virtual BrainOpenGLTextureManager* getTextureManager() = 0;
        
        /**
         * @return Half-size of the model window height.
         */
        static float getModelViewingHalfWindowHeight() { return 90.0f; }
        
        void setBorderBeingDrawn(Border* borderBeingDrawn);
        
        bool isDrawHighlightedEndPoints() const;
        
        void setDrawHighlightedEndPoints(const bool drawHighlightedEndPoints);
        
        void getBackgroundColor(uint8_t backgroundColor[3]) const;
        
        static void getMinMaxPointSize(float& minPointSizeOut, float& maxPointSizeOut);
        
        static void getMinMaxLineWidth(float& minLineWidthOut, float& maxLineWidthOut);
        
        static bool testForVersionOfOpenGLSupported(const AString& versionOfOpenGL);
        
        static QString getBestDrawingModeName();
        
        AString getOpenGLInformation();
        
        static DrawMode getBestDrawingMode();
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
        
        virtual AString getStateOfOpenGL() const;
        
    private:
        BrainOpenGL(const BrainOpenGL&);
        BrainOpenGL& operator=(const BrainOpenGL&);
        
        
    protected:
        AString getOpenGLEnabledEnumAsText(const AString& enumName,
                                    const GLenum enumValue) const;
        
        AString getOpenGLBooleanAsText(const AString& enumName,
                                           const GLenum enumValue) const;
        
        AString getOpenGLFloatAsText(const AString& enumName,
                                     const GLenum enumValue,
                                     const int32_t numberOfValues) const;
        
        AString getOpenGLLightAsText(const AString& enumName,
                                     const GLenum lightEnum,
                                     const GLenum enumValue,
                                     const int32_t numberOfValues) const;
        
        Border* borderBeingDrawn;
        
        bool m_drawHighlightedEndPoints;
        
        uint8_t m_foregroundColorByte[4];
        float m_foregroundColorFloat[4];

        uint8_t m_backgroundColorByte[4];
        float m_backgroundColorFloat[4];
        
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
        
        /** runtime library version of OpenGL */
        static AString s_runtimeLibraryVersionOfOpenGL;
        
        /** runtime library major version number of OpenGL */
        static AString s_runtimeLibraryMajorVersionOfOpenGL;
        
        /** runtime library minor version number of OpenGL */
        static AString s_runtimeLibraryMinorVersionOfOpenGL;
        
        static bool s_supportsDisplayLists;
        
        static bool s_supportsImmediateMode;
        
        static bool s_supportsVertexBuffers;
        
        /** Optional text rendering (if not null) */
        BrainOpenGLTextRenderInterface* m_textRenderer;
        
        AString m_openGLExtensionsInformation;
    };

#ifdef __BRAIN_OPENGL_DEFINE_H
    AString BrainOpenGL::s_runtimeLibraryVersionOfOpenGL = "";
    AString BrainOpenGL::s_runtimeLibraryMajorVersionOfOpenGL = "";
    AString BrainOpenGL::s_runtimeLibraryMinorVersionOfOpenGL = "";
    float BrainOpenGL::s_minPointSize = 1.0f;
    float BrainOpenGL::s_maxPointSize = 10.0f;
    float BrainOpenGL::s_minLineWidth = 1.0f;
    float BrainOpenGL::s_maxLineWidth = 10.0f;
    bool BrainOpenGL::s_supportsDisplayLists  = false;
    bool BrainOpenGL::s_supportsImmediateMode = false;
    bool BrainOpenGL::s_supportsVertexBuffers = false;
#endif //__BRAIN_OPENGL_DEFINE_H

} // namespace


#endif // __BRAIN_OPENGL_H__
