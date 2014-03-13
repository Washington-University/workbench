#ifndef __BRAIN_OPEN_G_L_SHAPE_CUBE__H_
#define __BRAIN_OPEN_G_L_SHAPE_CUBE__H_

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

#include <stdint.h>

#include "BrainOpenGLShape.h"

namespace caret {

    class BrainOpenGLShapeCube : public BrainOpenGLShape {
        
    public:
        /**
         * Type of cube
         */
        enum CUBE_TYPE {
            /** Normal vectors orthogonal to faces */
            NORMAL,
            /** Normal vectors run though vertices which smooths corners */
            ROUNDED
        };
        
        BrainOpenGLShapeCube(const float cubeSize,
                             const CUBE_TYPE cubeType);
        
        virtual ~BrainOpenGLShapeCube();
        
    private:
        BrainOpenGLShapeCube(const BrainOpenGLShapeCube&);

        BrainOpenGLShapeCube& operator=(const BrainOpenGLShapeCube&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        void drawShape(const BrainOpenGL::DrawMode drawMode,
                       const uint8_t rgba[4]);
        
        void drawShape(const BrainOpenGL::DrawMode drawMode,
                       const float rgba[4]);
        
        
        void setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode);
        
    private:

        void addFace(const GLfloat coordinates[][3],
                     const GLint v1,
                     const GLint v2,
                     const GLint v3,
                     const GLint v4,
                     const GLfloat normalVector[3]);
        
        // ADD_NEW_MEMBERS_HERE
        
        const float m_cubeSize;
        const CUBE_TYPE m_cubeType;
        
        GLuint m_vertexBufferID;
        GLuint m_coordinatesRgbaByteBufferID;
        GLuint m_normalBufferID;
        GLuint m_trianglesBufferID;

        GLuint m_displayList;
        
        std::vector<GLfloat> m_coordinates;
        std::vector<GLubyte> m_rgbaByte;
        std::vector<GLfloat> m_normals;
        std::vector<GLuint> m_triangles;
        
        bool m_isApplyColoring;
    };
    
#ifdef __BRAIN_OPEN_G_L_SHAPE_CUBE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_SHAPE_CUBE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_SHAPE_CUBE__H_
