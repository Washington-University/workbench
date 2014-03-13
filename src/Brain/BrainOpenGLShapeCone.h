#ifndef __BRAIN_OPEN_GL_SHAPE_CONE_H_
#define __BRAIN_OPEN_GL_SHAPE_CONE_H_

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

    class BrainOpenGLShapeCone : public BrainOpenGLShape {
        
    public:
        BrainOpenGLShapeCone(const int32_t numberOfSides);
        
        virtual ~BrainOpenGLShapeCone();
        
    private:
        BrainOpenGLShapeCone(const BrainOpenGLShapeCone&);

        BrainOpenGLShapeCone& operator=(const BrainOpenGLShapeCone&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        void drawShape(const BrainOpenGL::DrawMode drawMode,
                       const uint8_t rgba[4]);
        
        void drawShape(const BrainOpenGL::DrawMode drawMode,
                       const float rgba[4]);
        
        void setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode);
        
    private:

        // ADD_NEW_MEMBERS_HERE
        
        const int32_t m_numberOfSides;
        
        GLuint m_coordinatesBufferID;
        
        GLuint m_coordinatesRgbaByteBufferID;

        GLuint m_sidesNormalBufferID;
        GLuint m_sidesTriangleFanBufferID;
        
        GLuint m_capNormalBufferID;
        GLuint m_capTriangleFanBufferID;
        
        GLuint m_displayList;
        
        std::vector<GLfloat> m_coordinates;
        std::vector<GLubyte> m_rgbaByte;
        
        std::vector<GLfloat> m_sideNormals;
        std::vector<GLuint> m_sidesTriangleFan;
        
        std::vector<GLuint>  m_capTriangleFan;
        std::vector<GLfloat> m_capNormals;
        
        bool m_isApplyColoring;
    };
    
#ifdef __BRAIN_OPEN_GL_SHAPE_CONE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_GL_SHAPE_CONE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_GL_SHAPE_CONE_H_
