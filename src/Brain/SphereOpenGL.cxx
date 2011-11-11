
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#include <cmath>

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

#define __SPHERE_OPEN_G_L_DECLARE__
#include "SphereOpenGL.h"
#undef __SPHERE_OPEN_G_L_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class SphereOpenGL 
 * \brief Generates spheres for OpenGL drawing.
 *
 * Generates spheres for OpenGL drawing.
 */

/**
 * Constructor.
 */
SphereOpenGL::SphereOpenGL(const float radius)
: CaretObject()
{
    const int numLat = 10;
    const int numLon = 10;
    
    const float degToRad = M_PI / 180.0;
    
    const float dLat = 180.0 / numLat;
    for (int iLat = 0; iLat <= numLat; iLat++) {
        const float latDeg = -90.0 + (iLat * dLat);
        const float latRad = latDeg * degToRad;
        
        const float z1 = radius * std::sin(latRad);
        
        const float latDeg2 = -90.0 + ((iLat + 1) * dLat);
        const float latRad2 = latDeg2 * degToRad;
        
        const float z2 = radius * std::sin(latRad2);
        
        this->quadStripVerticesStartIndex.push_back(static_cast<int>(this->vertices.size()));
        
        const float dLon = 360.0 / numLon;
        for (int iLon = 0; iLon <= numLon; iLon++) {
            const float lonDeg = iLon * dLon;
            const float lonRad = lonDeg * degToRad;
            
            const float x1 = radius * std::cos(latRad) * std::cos(lonRad);
            const float y1 = radius * std::cos(latRad) * std::sin(lonRad);
            
            const float x2 = radius * std::cos(latRad2) * std::cos(lonRad);
            const float y2 = radius * std::cos(latRad2) * std::sin(lonRad);
            
            this->vertices.push_back(static_cast<int>(coordinates.size() / 3));
            this->coordinates.push_back(x2);
            this->coordinates.push_back(y2);
            this->coordinates.push_back(z2);
            const float length2 = std::sqrt(x2*x2 + y2*y2 + z2*z2);
            this->normals.push_back(x2 / length2);
            this->normals.push_back(y2 / length2);
            this->normals.push_back(z2 / length2);
            
            this->vertices.push_back(static_cast<int>(coordinates.size() / 3));
            this->coordinates.push_back(x1);
            this->coordinates.push_back(y1);
            this->coordinates.push_back(z1);
            const float length1 = std::sqrt(x1*x1 + y1*y1 + z1*z1);
            this->normals.push_back(x1 / length1);
            this->normals.push_back(y1 / length1);
            this->normals.push_back(z1 / length1);            
        }

        this->quadStripVerticesEndIndex.push_back(static_cast<int>(this->vertices.size()));
    }
    
}

/**
 * Destructor.
 */
SphereOpenGL::~SphereOpenGL()
{
    
}

/**
 * Draw the sphere using quad strips.
 */
void 
SphereOpenGL::drawWithQuadStrips()
{
    const int32_t numQuadStrips = this->quadStripVerticesEndIndex.size();
    for (int32_t i = 0; i < numQuadStrips; i++) {
        const int32_t vertexStart = this->quadStripVerticesStartIndex[i];
        const int32_t vertexEnd = this->quadStripVerticesEndIndex[i];
        glBegin(GL_QUAD_STRIP);
        for (int32_t iVertex = vertexStart; iVertex < vertexEnd; iVertex++) {
            CaretAssertVectorIndex(this->vertices, iVertex);
            const int32_t vertexIndex = this->vertices[iVertex];
            const int32_t v3 = vertexIndex * 3;
            
            CaretAssertVectorIndex(this->normals, v3);
            CaretAssertVectorIndex(this->coordinates, v3);
            
            glNormal3fv(&this->normals[v3]);
            glVertex3fv(&this->coordinates[v3]);
        }
        glEnd();
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SphereOpenGL::toString() const
{
    return "SphereOpenGL";
}
