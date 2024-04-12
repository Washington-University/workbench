#ifndef __GRAPHICS_POLYGON_TESSELLATOR_H__
#define __GRAPHICS_POLYGON_TESSELLATOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

/*
 * Windows requires "CALLBACK" for functions called by the tessellator
 * but unix does not need it so define CALLBACK to nothing if it is
 * not defined (it is defined ONLY on Windows)
 */
#ifndef CALLBACK
#define CALLBACK
#endif

#include <memory>

#include "CaretObject.h"
#include "CaretOpenGLInclude.h"
#include "Vector3D.h"

namespace caret {

    class GraphicsPolygonTessellator : public CaretObject {
        
    public:
        class Vertex : public CaretObject {
        public:
            Vertex(const int32_t vertexIndex,
                   const Vector3D& xyz)
            : m_vertexIndex(vertexIndex),
            m_xyz(xyz) { }
            
            int32_t m_vertexIndex;
            
            Vector3D m_xyz;
        };
        
        GraphicsPolygonTessellator(const std::vector<Vertex>& polygonVertices,
                                   const Vector3D& polygonNormalVector);
        
        virtual ~GraphicsPolygonTessellator();
        
        GraphicsPolygonTessellator(const GraphicsPolygonTessellator&) = delete;

        GraphicsPolygonTessellator& operator=(const GraphicsPolygonTessellator&) = delete;
        
        bool tessellate(std::vector<Vertex>& triangleVerticesOut,
                        AString& errorMessageOut);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        static void CALLBACK beginCallbackData(GLenum type,
                                               GLvoid* userData);
        
        static void CALLBACK endCallbackData(GLvoid* userData);
        
        static void CALLBACK errorCallbackData(GLenum errorCode,
                                               GLvoid* userData);
        
        static void CALLBACK vertexCallbackData(GLvoid* vertex,
                                                GLvoid* userData);
        
        static void CALLBACK combineCallbackData(GLdouble coords[3],
                                                 GLvoid*  vertexData[4],
                                                 GLfloat  weights[4],
                                                 GLvoid** dataOut,
                                                 GLvoid*  userData);

        static void CALLBACK edgeFlagCallbackData(GLboolean flag,
                                                  GLvoid* userData);
        
        const std::vector<Vertex>& m_polygonVertices;
        
        const Vector3D& m_polygonNormalVector;
        
        AString m_beginErrorMessage;
        
        AString m_tesselllatorErrorMessage;
        
        std::vector<Vertex> m_tessellatedVertices;
        
        std::vector<Vertex*> m_combinedVertices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_POLYGON_TESSELLATOR_DECLARE__
#endif // __GRAPHICS_POLYGON_TESSELLATOR_DECLARE__

} // namespace
#endif  //__GRAPHICS_POLYGON_TESSELLATOR_H__
