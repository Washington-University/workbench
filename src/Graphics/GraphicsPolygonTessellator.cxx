
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

#define __GRAPHICS_POLYGON_TESSELLATOR_DECLARE__
#include "GraphicsPolygonTessellator.h"
#undef __GRAPHICS_POLYGON_TESSELLATOR_DECLARE__

#include "CaretAssert.h"

using namespace caret;

static bool debugFlag = false;

/**
 * \class caret::GraphicsPolygonTessellator 
 * \brief Convert a polygon, possibly concave, into triangles
 * \ingroup Graphics
 */

/**
 * Constructor.
 * @param polygonVertices
 *    Vertices of the polygon
 * @param polygonNormalVector
 *    Normal vector of the polygon
 */
GraphicsPolygonTessellator::GraphicsPolygonTessellator(const std::vector<Vertex>& polygonVertices,
                                                       const Vector3D& polygonNormalVector)
: CaretObject(),
m_polygonVertices(polygonVertices),
m_polygonNormalVector(polygonNormalVector)
{
    
}

/**
 * Destructor.
 */
GraphicsPolygonTessellator::~GraphicsPolygonTessellator()
{
    /*
     * Combine vertex objects were created by the
     * "combine" OpenGL callback.
     */
    for (auto& v : m_combinedVertices) {
        delete v;
    }
    m_combinedVertices.clear();
}

/**
 * Tessellate the given polygon
 * @param triangleVerticesOut
 *    Output with vertices for triangles
 * @param errorMessageOut
 *    Contains information about error
 * @return
 *    True if tessellation was successful, else false.
 */
bool
GraphicsPolygonTessellator::tessellate(std::vector<Vertex>& triangleVerticesOut,
                                       AString& errorMessageOut)
{
    triangleVerticesOut.clear();
    errorMessageOut.clear();
    
    const int32_t numPolygonVertices(m_polygonVertices.size());
    if (numPolygonVertices < 3) {
        errorMessageOut = "Polygon must contain at least three vertices";
        return false;
    }
    else if (numPolygonVertices == 3) {
        triangleVerticesOut = m_polygonVertices;
        return true;
    }
    
    /*
     * Create the tessellator
     */
    GLUtesselator* tessellator(gluNewTess());
    if (tessellator == NULL) {
        errorMessageOut = "Failed to create new tessellator with gluNewTess()";
        return false;
    }
    
    /*
     * Edge flag should force tessellator to only output
     * triangles and not output fans or strips
     */
    glPushAttrib(GL_CURRENT_BIT);
    glEdgeFlag(true);
    
    /*
     * No line loops
     */
    gluTessProperty(tessellator,
                    GLU_TESS_BOUNDARY_ONLY,
                    GL_FALSE);
    
    /*
     * Combine vertices tolerance
     */
    gluTessProperty(tessellator,
                    GLU_TESS_TOLERANCE,
                    0.0);
    
    /*
     * Winding rule
     */
    gluTessProperty(tessellator,
                    GLU_TESS_WINDING_RULE,
                    GLU_TESS_WINDING_ODD);
    
    /*
     * Normal vector of polygon
     */
    gluTessNormal(tessellator,
                  m_polygonNormalVector[0],
                  m_polygonNormalVector[1],
                  m_polygonNormalVector[2]);
    
    /*
     * Similar to glBegin()
     */
    gluTessCallback(tessellator,
                    GLU_TESS_BEGIN_DATA,
                    (GLvoid(*)())GraphicsPolygonTessellator::beginCallbackData);
    
    /*
     * Similar to glEnd();
     */
    gluTessCallback(tessellator,
                    GLU_TESS_END_DATA,
                    (GLvoid(*)())GraphicsPolygonTessellator::endCallbackData);
    
    /*
     * Similar to glVertex()
     */
    gluTessCallback(tessellator,
                    GLU_TESS_VERTEX_DATA,
                    (GLvoid(*)())GraphicsPolygonTessellator::vertexCallbackData);
    
    /*
     * Called if the tessellator reports an error
     */
    gluTessCallback(tessellator,
                    GLU_TESS_ERROR_DATA,
                    (GLvoid(*)())GraphicsPolygonTessellator::errorCallbackData);
    
    /*
     * Called by tessellator to combine vertices if they are "close"
     */
    gluTessCallback(tessellator,
                    GLU_TESS_COMBINE_DATA,
                    (GLvoid(*)())GraphicsPolygonTessellator::combineCallbackData);
    
    /*
     * We don't use this BUT having this callback attached
     * to the tessellator ensures that only GL_TRIANGLES is
     * set to the begin callback.  We don't want the other
     * types (triangle fan and strip)
     */
    gluTessCallback(tessellator,
                    GLU_TESS_EDGE_FLAG_DATA,
                    (GLvoid(*)())GraphicsPolygonTessellator::edgeFlagCallbackData);
    /*
     * OpenGL wants double for vertex data
     */
    std::vector<GLdouble> doubleXYZ;
    for (int32_t i = 0; i < numPolygonVertices; i++) {
        CaretAssertVectorIndex(m_polygonVertices, i);
        const Vertex v(m_polygonVertices[i]);
        if (debugFlag) {
            std::cout << "Input to tessellator " << v.m_vertexIndex << ": " << v.m_xyz.toString() << std::endl;
        }
        doubleXYZ.push_back(v.m_xyz[0]);
        doubleXYZ.push_back(v.m_xyz[1]);
        doubleXYZ.push_back(v.m_xyz[2]);
    }
    CaretAssert(static_cast<int32_t>(doubleXYZ.size()) == (numPolygonVertices * 3));
    CaretAssert(static_cast<int32_t>(m_polygonVertices.size()) == numPolygonVertices);
    
    /*
     * Begin polygon with user data set to 'this'
     */
    GLvoid* userData(this);
    gluTessBeginPolygon(tessellator,
                        userData);
    gluTessBeginContour(tessellator);
    for (int32_t i = 0; i < numPolygonVertices; i++) {
        const int32_t i3(i * 3);
        CaretAssertVectorIndex(m_polygonVertices, i);
        CaretAssertVectorIndex(doubleXYZ, i3 + 2);
        gluTessVertex(tessellator,
                      &doubleXYZ[i3],
                      (GLvoid*)&m_polygonVertices[i]);
    }
    gluTessEndContour(tessellator);
    gluTessEndPolygon(tessellator);
    
    glPopAttrib();
    
    gluDeleteTess(tessellator);
    
    if ( ! m_beginErrorMessage.isEmpty()) {
        errorMessageOut.appendWithNewLine(m_beginErrorMessage);
    }
    if ( ! m_tesselllatorErrorMessage.isEmpty()) {
        errorMessageOut.appendWithNewLine(m_tesselllatorErrorMessage);
    }
    
    triangleVerticesOut = m_tessellatedVertices;
    /*
     * Valid output if no error messages
     */
    return (errorMessageOut.isEmpty());
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsPolygonTessellator::toString() const
{
    return "GraphicsPolygonTessellator";
}

/**
 * Called by tessellator to indicate the start of a primitive
 * @param type
 *    Type of primitive such as GL_TRIANGLES
 * @param userData
 *    Pointer to instance of this class
 */
void CALLBACK
GraphicsPolygonTessellator::beginCallbackData(GLenum type,
                                              GLvoid* userData)
{
    CaretAssert(userData);
    GraphicsPolygonTessellator* tessellator(static_cast<GraphicsPolygonTessellator*>(userData));
    CaretAssert(tessellator);

    if (type == GL_TRIANGLES) {
        if (debugFlag) {
            std::cout << "Begin Triangles" << std::endl;
        }
    }
    else {
        AString typeName("Unknown");
        if (type == GL_TRIANGLE_FAN) {
            typeName = "Triangle Fan";
        }
        else if (type == GL_TRIANGLE_STRIP) {
            typeName = "Triangle Strip";
        }
        else if (type == GL_LINE_LOOP) {
            typeName = "Line Loop";
        }
        
        tessellator->m_beginErrorMessage
        = ("Primitive type passed to beginCallback is not GL_TRIANGLES but is "
           + typeName);
        
        if (debugFlag) {
            std::cout << "Begin NOT Triangles but is: " << typeName << std::endl;
        }
    }
}

/**
 * Called by tessellator to indicate the end of a primitive
 * @param userData
 *    Pointer to instance of this class
 */
void CALLBACK
GraphicsPolygonTessellator::endCallbackData(GLvoid* userData)
{
    CaretAssert(userData);
    GraphicsPolygonTessellator* tessellator(static_cast<GraphicsPolygonTessellator*>(userData));
    CaretAssert(tessellator);
}

/**
 * Called by tessellator to indicate an error in the tessellator
 * @param errorCode
 *    Code for the error
 * @param userData
 *    Pointer to instance of this class
 */
void CALLBACK
GraphicsPolygonTessellator::errorCallbackData(GLenum errorCode,
                                              GLvoid* userData)
{
    CaretAssert(userData);
    GraphicsPolygonTessellator* tessellator(static_cast<GraphicsPolygonTessellator*>(userData));
    CaretAssert(tessellator);

    tessellator->m_tesselllatorErrorMessage
    = ("Error code "
       + AString::number((int)errorCode)
       + ": "
       + AString((char*)gluErrorString(errorCode)));
}

/**
 * Called by tessellator that provides a vertex and user data
 * @param vertexPtr
 *    Data for the vertex
 * @param userData
 *    Pointer to instance of this class
 */
void CALLBACK
GraphicsPolygonTessellator::vertexCallbackData(GLvoid* vertexPtr,
                                               GLvoid* userData)
{
    CaretAssert(userData);
    GraphicsPolygonTessellator* tessellator(static_cast<GraphicsPolygonTessellator*>(userData));
    CaretAssert(tessellator);

    CaretAssert(vertexPtr);
    
    const Vertex* polygonVertexPtr(static_cast<Vertex*>(vertexPtr));
    CaretAssert(polygonVertexPtr);
    
    tessellator->m_tessellatedVertices.push_back(*polygonVertexPtr);
    
    if (debugFlag) {
        std::cout << "   " << polygonVertexPtr->m_vertexIndex
        << ": XYZ: " << polygonVertexPtr->m_xyz.toString() << std::endl;
    }
}

/**
 * Called by the tessellator when it combines several vertices into one.  This
 * seems to occur when the user drags a vertex causing the polygon
 * to intersect itself.
 *
 * @param coords
 *       Location of the new vertex
 * @param vertexData,
 *    Up to four vertices that were combined
 * @param weights
 *    Up to four weights that add up to one.   A weight of zero indicates
 *    corresponding vertexData is invalid
 * @param dataOut
 *    Output containing new vertex and possibly data that is allocated by the function
 * @param userData
 *    Pointer to instance of this class
 */
void CALLBACK
GraphicsPolygonTessellator::combineCallbackData(GLdouble coords[3],
                                                GLvoid*  vertexData[4],
                                                GLfloat  weights[4],
                                                GLvoid** dataOut,
                                                GLvoid*  userData)
{
    CaretAssert(userData);
    GraphicsPolygonTessellator* tessellator(static_cast<GraphicsPolygonTessellator*>(userData));
    CaretAssert(tessellator);
    
    float maxWeight(-1.0);
    int32_t maxWeightIndex(-1);
    if (debugFlag) {
        std::cout << "Combine: " << AString::fromNumbers(coords, 3) << std::endl;
    }
    for (int32_t i = 0; i < 4; i++) {
        if (vertexData[i] != NULL) {
            if (weights[i] > maxWeight) {
                maxWeight = weights[i];
                maxWeightIndex = i;
            }
            const Vertex* polygonVertexPtr(static_cast<Vertex*>(vertexData[i]));
            CaretAssert(polygonVertexPtr);
            if (debugFlag) {
                std::cout << "  " << i << ": vertex=" << polygonVertexPtr->m_vertexIndex
                << " weight=" << weights[i]
                << std::endl;
            }
        }
        
        if (maxWeightIndex > 0) {
            const Vertex* polygonVertexPtr(static_cast<Vertex*>(vertexData[i]));
            CaretAssert(polygonVertexPtr);
            
            Vertex* v(new Vertex(polygonVertexPtr->m_vertexIndex,
                                 Vector3D(coords[0],
                                          coords[1],
                                          coords[2])));
            *dataOut = static_cast<GLvoid*>(v);

            /*
             * Keep track of these Vertex objects so that there
             * is not a memory leak.
             */
            tessellator->m_combinedVertices.push_back(v);
        }
    }
}

/**
 * Called by the tessellator if edge lies on polygon boundary
 * @param flag
 *    The edge flag
 * @param userData
 *    Pointer to instance of this class
 */
void CALLBACK
GraphicsPolygonTessellator::edgeFlagCallbackData(GLboolean /*flag*/,
                                                 GLvoid* /*userData*/)
{
    /*
     * By attaching this function to the OpenGL tessellator,
     * the tesselllator will only output triangles.  See
     * note where this is setup as a callback at
     * gluTessCallback().
     *
     * CaretAssert(userData);
     * GraphicsPolygonTessellator* tessellator(static_cast<GraphicsPolygonTessellator*>(userData));
     * CaretAssert(tessellator);
     */
}


