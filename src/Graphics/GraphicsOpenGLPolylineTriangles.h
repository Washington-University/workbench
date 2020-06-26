#ifndef __GRAPHICS_OPENGL_POLYLINE_TRIANGLES_H__
#define __GRAPHICS_OPENGL_POLYLINE_TRIANGLES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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



#include <memory>
#include <set>

#include "CaretObject.h"

namespace caret {

    class EventOpenGLObjectToWindowTransform;
    class GraphicsPrimitive;
    class GraphicsPrimitiveV3fC4f;
    class GraphicsPrimitiveV3fC4ub;
    
    class GraphicsOpenGLPolylineTriangles : public CaretObject {
        
    public:
        virtual ~GraphicsOpenGLPolylineTriangles();
        
        static GraphicsPrimitive* convertWorkbenchLinePrimitiveTypeToOpenGL(const GraphicsPrimitive* primitive,
                                                                            AString& errorMessageOut);
        
        static GraphicsPrimitive* convertWorkbenchLinePrimitiveTypeToOpenGL(const GraphicsPrimitive* primitive,
                                                                            std::vector<int32_t>* triangleVertexIndicesToLineVertexIndicesOut,
                                                                            AString& errorMessageOut);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        /**
         * Type of line coloring
         */
        enum class ColorType {
            /** Byte RGBA color at each vertex */
            BYTE_RGBA_PER_VERTEX,
            /** One Byte RGBA color for all vertices */
            BYTE_RGBA_SOLID,
            /** Float RGBA color at each vertex */
            FLOAT_RGBA_PER_VERTEX,
            /** One Float RGBA color for all vertices */
            FLOAT_RGBA_SOLID
        };
        
        /**
         * Drawing space
         */
        enum class DrawingSpace {
            /**
             * Lines are drawn using the current modeling transformations in
             * the XY plane.  Z-coordinates should all be the same.
             */
            MODEL_XY,
            /**
             * Coordinates of the lines are converted into window (pixel) coordinates
             * and the lines are drawn in 'window space'.  The lines always
             * face the user even with rotations.
             */
            WINDOW
        };
        
        /**
         * Join tyupe of how connected lines are drawn to remove gaps between segments
         */
        enum class JoinType {
            /** None */
            NONE,
            /** Bevel fills the gap by adding a triangle */
            BEVEL,
            /** Miter extends 'outsides' of lines to meet forming a pointed shape */
            MITER
        };
        
        /**
         * Type of line drawing
         */
        enum class LineType {
            /** Each pair of vertices is an independent line segment (GL_LINES) */
            LINES,
            /** A connected set of lines forming a loop (last is automatically connected to first) */
            LINE_LOOP,
            /** A connected set of lines (last is not connected to first) */
            LINE_STRIP
        };

        /**
         * Turn direction add joint vertex of two lines
         */
        enum class TurnDirection {
            /** Turns left */
            LEFT,
            /** Does not turn (straight) or coincident */
            PARALLEL,
            /** Turns right */
            RIGHT,
        };
        
        /**
         * Indices into input vertices of line
         * that formed the polyline
         */
        class PolylineInfo {
        public:
            PolylineInfo() { }
            
            PolylineInfo(const int32_t windowVertexOneIndex,
                         const int32_t windowVertexTwoIndex,
                         const int32_t triangleOnePrimitiveIndices[3],
                         const int32_t triangleTwoPrimitiveIndices[3])
            : m_windowVertexOneIndex(windowVertexOneIndex),
              m_windowVertexTwoIndex(windowVertexTwoIndex) {
                m_triangleOnePrimitiveIndices[0] = triangleOnePrimitiveIndices[0];
                m_triangleOnePrimitiveIndices[1] = triangleOnePrimitiveIndices[1];
                m_triangleOnePrimitiveIndices[2] = triangleOnePrimitiveIndices[2];
                m_triangleTwoPrimitiveIndices[0] = triangleTwoPrimitiveIndices[0];
                m_triangleTwoPrimitiveIndices[1] = triangleTwoPrimitiveIndices[1];
                m_triangleTwoPrimitiveIndices[2] = triangleTwoPrimitiveIndices[2];
                  m_valid = true;
            }
            
            int32_t m_windowVertexOneIndex;
            int32_t m_windowVertexTwoIndex;
            int32_t m_triangleOnePrimitiveIndices[3];
            int32_t m_triangleTwoPrimitiveIndices[3];
            bool m_valid = false;
        };
        
        /**
         * Triangles created by joining of polylines
         */
        class TriangleJoin {
        public:
            TriangleJoin(const int32_t windowVertexIndex,
                         const int32_t primitiveVertexOneIndex,
                         const int32_t primitiveVertexTwoIndex)
            : m_windowVertexIndex(windowVertexIndex),
            m_primitiveVertexOneIndex(primitiveVertexOneIndex),
            m_primitiveVertexTwoIndex(primitiveVertexTwoIndex)
            {
                
            }
            
            const int32_t m_windowVertexIndex;
            const int32_t m_primitiveVertexOneIndex;
            const int32_t m_primitiveVertexTwoIndex;
        };
        
        GraphicsOpenGLPolylineTriangles(const std::vector<float>& xyz,
                                        const std::vector<float>& floatRGBA,
                                        const std::vector<uint8_t>& byteRGBA,
                                        const std::set<int32_t>& vertexPrimitiveRestartIndices,
                                        const float lineThickness,
                                        const ColorType colorType,
                                        const DrawingSpace drawingSpace,
                                        const LineType lineType,
                                        const JoinType joinType);
        
        GraphicsOpenGLPolylineTriangles(const GraphicsOpenGLPolylineTriangles& obj);
        
        GraphicsOpenGLPolylineTriangles& operator=(const GraphicsOpenGLPolylineTriangles& obj) const;
        
        GraphicsPrimitive* convertLinesToPolygons(AString& errorMessageOut);
        
        void saveOpenGLState();
        
        void restoreOpenGLState();
        
        void createTransform();
        
        void convertFromModelToWindowCoordinate(const float modelXYZ[3],
                                                float windowXYZOut[3]) const;
        
        void createWindowCoordinatesFromVertices();
        
        void convertLineSegmentsToTriangles();
        
        void createTrianglesFromWindowVertices(const int32_t windowVertexOneIndex,
                                               const int32_t windowVertexTwoIndex);
        
        void performBevelJoin(const PolylineInfo& polyOne,
                              const PolylineInfo& polyTwo);
        
        void performMiterJoin(const PolylineInfo& polyOne,
                              const PolylineInfo& polyTwo);
        
        void joinTwoPolyLines(const PolylineInfo& polyOne,
                              const PolylineInfo& polyTwo);
        
        void addJoinTriangles();
        
        void createMiterJoinVertex(const float v1[3],
                                   const float v2[3],
                                   const float miterLength,
                                   float xyzOut[3]);
        
        TurnDirection getTurnDirection(const PolylineInfo& polyOne,
                              const PolylineInfo& polyTwo) const;
        
        void getAverageColorRGBA(const int32_t primitiveVertexOneIndex,
                                 const int32_t primitiveVertexTwoIndex,
                                 float rgbaOut[4]) const;
        
        void getAverageColorRGBA(const int32_t primitiveVertexOneIndex,
                                 const int32_t primitiveVertexTwoIndex,
                                 uint8_t rgbaOut[4]) const;
        
        void updateOutputVertexIndicesToInputVertexIndices(const GraphicsPrimitive* outputPrimitive,
                                                           const int32_t numOutputVerticesBefore,
                                                           const int32_t inputVertexIndex);
        
        const std::vector<float>& m_inputXYZ;
        
        const std::vector<float>& m_inputFloatRGBA;
        
        const std::vector<uint8_t>& m_inputByteRGBA;
        
        std::set<int32_t> m_vertexPrimitiveRestartIndices;
        
        const float m_lineThickness;
        
        const ColorType m_colorType;
        
        const DrawingSpace m_drawingSpace;
        
        const LineType m_lineType;
        
        const JoinType m_joinType;
        
        std::vector<float> m_vertexWindowXYZ;
        
        std::vector<int32_t> m_vertexWindowInputIndices;
        
        /*
         * Maps a vertex in the output triangles to a vertex
         * from the input line.  Typically used be identification
         * operations.  
         */
        std::vector<int32_t> m_triangleVertexIndicesToLineVertexIndices;
        
        bool m_debugFlag = false;
        
        int m_savedPolygonMode[2];
        
        int m_savedViewport[4];
        
        std::unique_ptr<EventOpenGLObjectToWindowTransform> m_transformEvent;
        
        /*
         * The base class graphics primitive that points to 
         * either the Float or Byte Color primitive and can
         * be used for non-color operations.  NEVER delete
         * this as it points to one the color primitive
         */
        GraphicsPrimitive* m_primitive = NULL;
        
        std::unique_ptr<GraphicsPrimitiveV3fC4f> m_primitiveFloatColor;
        
        std::unique_ptr<GraphicsPrimitiveV3fC4ub> m_primitiveByteColor;
        
        PolylineInfo m_lastPolyLineInfo;
        
        std::vector<TriangleJoin> m_triangleJoins;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_OPENGL_POLYLINE_TRIANGLES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OPENGL_POLYLINE_TRIANGLES_DECLARE__

} // namespace
#endif  //__GRAPHICS_OPENGL_POLYLINE_TRIANGLES_H__
