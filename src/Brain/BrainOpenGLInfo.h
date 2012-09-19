#ifndef __BRAIN_OPEN_G_L_INFO__H_
#define __BRAIN_OPEN_G_L_INFO__H_

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

//#define BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS 1
#define BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE 1
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1

//#ifdef GL_VERSION_1_1
//#define BRAIN_OPENGL_INFO_SUPPORTS_IMMEDIATE 1
//#define BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS 1
//#endif // GL_VERSION_1_1
//
//#ifdef GL_VERSION_2_1
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
//#endif // GL_VERSION_2_1
//
//#ifdef GL_VERSION_3_0
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
//#endif // GL_VERSION_3_0
//
//#ifdef GL_VERSION_4_0
//#define BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS 1
//#endif // GL_VERSION_4_0

namespace caret {

    class BrainOpenGLInfo {
        
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
        
        static QString getBestDrawingModeName();
        
        /**
         * @return The best drawing mode given the limitations of
         * the compile and run-time systems.
         */
        static DrawMode getBestDrawingMode() {
            if (s_drawModeInitialized == false) initializeDrawMode();
            return s_drawingMode;
        }
        
        /**
         * @return True if display list drawing is supported.
         */
        inline static bool isDisplayListsSupported() {
            if (s_drawModeInitialized == false) initializeDrawMode();
            return s_supportsDisplayLists;
        }
        
        /**
         * @return True if immediate mode drawing is supported.
         */
        inline static bool isImmediateSupported() {
            if (s_drawModeInitialized == false) initializeDrawMode();
            return s_supportsImmediateMode;
        }
        
        /**
         * @return True if vertex buffer drawing is supported.
         */
        inline static bool isVertexBuffersSupported() {
            if (s_drawModeInitialized == false) initializeDrawMode();
            return s_supportsVertexBuffers;
        }
        
    private:
        BrainOpenGLInfo();
        
        virtual ~BrainOpenGLInfo();
        
        BrainOpenGLInfo(const BrainOpenGLInfo&);

        BrainOpenGLInfo& operator=(const BrainOpenGLInfo&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        
        static void initializeDrawMode();

        // ADD_NEW_MEMBERS_HERE

        static DrawMode s_drawingMode;
        
        static bool s_drawModeInitialized;
        
        static bool s_supportsDisplayLists;
        
        static bool s_supportsImmediateMode;
        
        static bool s_supportsVertexBuffers;
    };
    
#ifdef __BRAIN_OPEN_G_L_INFO_DECLARE__
    BrainOpenGLInfo::DrawMode BrainOpenGLInfo::s_drawingMode = BrainOpenGLInfo::DRAW_MODE_INVALID;
    bool BrainOpenGLInfo::s_drawModeInitialized   = false;
    bool BrainOpenGLInfo::s_supportsDisplayLists  = false;
    bool BrainOpenGLInfo::s_supportsImmediateMode = false;
    bool BrainOpenGLInfo::s_supportsVertexBuffers = false;
#endif // __BRAIN_OPEN_G_L_INFO_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_INFO__H_
