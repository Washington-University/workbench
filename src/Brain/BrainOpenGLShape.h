#ifndef __BRAIN_OPEN_GL_SHAPE__H_
#define __BRAIN_OPEN_GL_SHAPE__H_

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
         * Setup for drawing the shape with OpenGL.  This may include
         * creating display lists, allocating buffers, and other calls
         * to the OpenGL API.  The base class handles alllocation and
         * deallocation of OpenGL display lists and buffers.
         *
         * @param drawMode
         *    How the shape will be drawn.
         */
        virtual void setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode) = 0;
        
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
        void createShapeIfNeeded();
        
        void releaseOpenGLAllocations();
        
        // ADD_NEW_MEMBERS_HERE

        std::set<GLuint> m_bufferIDs;
        
        std::set<GLuint> m_displayLists;
        
        bool m_shapeSetupComplete;
        
        BrainOpenGL::DrawMode m_drawMode;
        
        static bool s_immediateModeOverride;
    };
    
#ifdef __BRAIN_OPEN_GL_SHAPE_DECLARE__
    bool BrainOpenGLShape::s_immediateModeOverride = false;
#endif // __BRAIN_OPEN_GL_SHAPE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_GL_SHAPE__H_
