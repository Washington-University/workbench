#ifndef __BRAIN_OPEN_GL_SHAPE__H_
#define __BRAIN_OPEN_GL_SHAPE__H_

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

#include <set>

#include "BrainOpenGL.h"

namespace caret {

    class BrainOpenGLShape : public CaretObject {
        
    public:
        BrainOpenGLShape();
        
        virtual ~BrainOpenGLShape();
        
        // ADD_NEW_METHODS_HERE

        void draw(const uint8_t rgba[4]);
        
        void draw(const float rgba[4]);
        
        static void setImmediateModeOverride(const bool override);
        
    private:
        BrainOpenGLShape(const BrainOpenGLShape&);

        BrainOpenGLShape& operator=(const BrainOpenGLShape&);
        
        void releaseBufferIDInternal(const GLuint bufferID);
        
        void releaseDisplayListInternal(const GLuint displayList);
        
    protected:
        /**
         * Setup for drawing the shape.
         *
         * @param drawMode
         *    How the shape will be drawn.
         */
        virtual void setupShape(const BrainOpenGL::DrawMode drawMode) = 0;
        
        /**
         * Draw the shape using the given drawing mode.
         *
         * @param drawMode
         *    How the shape will be drawn.
         * @param rgba
         *    RGBA coloring ranging 0 to 255.
         */
        virtual void drawShape(const BrainOpenGL::DrawMode drawMode,
                               const uint8_t rgba[4]) = 0;
        
        /**
         * Draw the shape using the given drawing mode.
         *
         * @param drawMode
         *    How the shape will be drawn.
         * @param rgba
         *    RGBA coloring ranging 0.0 to 1.0.
         */
        virtual void drawShape(const BrainOpenGL::DrawMode drawMode,
                               const float rgba[4]) = 0;
        
        GLuint createBufferID();
        
        void releaseBufferID(const GLuint bufferID);
        
        GLuint createDisplayList();
        
        void releaseDisplayList(const GLuint displayList);
        
        void contatenateTriangleStrips(const std::vector<GLuint>& vertices,
                                       const std::vector<GLuint>& stripStartIndices,
                                       const std::vector<GLuint>& stripEndIndices,
                                       std::vector<GLuint>& triangleStripVerticesOut) const;
        
        void printTriangleStrip(const std::vector<GLuint>& triangleStrip) const;
        
        void printTriangleFan(const std::vector<GLuint>& triangleFan) const;
        
        void contatenateTriangleStrips(const std::vector<std::vector<GLuint> >& triangleStrips,
                                       std::vector<GLuint>& triangleStripOut) const;
        
    private:
        
        // ADD_NEW_MEMBERS_HERE

        std::set<GLuint> m_bufferIDs;
        
        std::set<GLuint> m_displayLists;
        
        bool m_shapeSetupComplete;
        
        static BrainOpenGL::DrawMode s_drawMode;
        
        static bool s_drawModeInitialized;
        
        static bool s_immediateModeOverride;
    };
    
#ifdef __BRAIN_OPEN_GL_SHAPE_DECLARE__
    BrainOpenGL::DrawMode BrainOpenGLShape::s_drawMode = BrainOpenGL::DRAW_MODE_INVALID;
    bool BrainOpenGLShape::s_drawModeInitialized = false;
    bool BrainOpenGLShape::s_immediateModeOverride = false;
#endif // __BRAIN_OPEN_GL_SHAPE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_GL_SHAPE__H_
