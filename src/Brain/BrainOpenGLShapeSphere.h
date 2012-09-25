#ifndef __BRAIN_OPEN_G_L_SHAPE_SPHERE__H_
#define __BRAIN_OPEN_G_L_SHAPE_SPHERE__H_

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

#include <stdint.h>

#include "BrainOpenGLShape.h"

namespace caret {

    class BrainOpenGLShapeSphere : public BrainOpenGLShape {
        
    public:
        BrainOpenGLShapeSphere(const int32_t numberOfLatitudeAndLongitude);
        
        virtual ~BrainOpenGLShapeSphere();
        
    private:
        BrainOpenGLShapeSphere(const BrainOpenGLShapeSphere&);

        BrainOpenGLShapeSphere& operator=(const BrainOpenGLShapeSphere&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        void drawShape(const BrainOpenGL::DrawMode drawMode);
        
        void setupShape(const BrainOpenGL::DrawMode drawMode);
        
    private:

        // ADD_NEW_MEMBERS_HERE
        
        const int32_t m_numberOfLatitudeAndLongitude;
        
        GLuint m_vertexBufferID;
        GLuint m_normalBufferID;
        GLuint m_triangleStripBufferID;

        GLuint m_displayList;
        
        std::vector<GLfloat> m_coordinates;
        std::vector<GLfloat> m_normals;
        
        std::vector<std::vector<GLuint> > m_triangleStrips;
        std::vector<GLuint> m_singleTriangleStrip;        
    };
    
#ifdef __BRAIN_OPEN_G_L_SHAPE_SPHERE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_SHAPE_SPHERE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_SHAPE_SPHERE__H_
