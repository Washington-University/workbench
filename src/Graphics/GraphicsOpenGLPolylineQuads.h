#ifndef __GRAPHICS_OPENGL_POLYLINE_QUADS_H__
#define __GRAPHICS_OPENGL_POLYLINE_QUADS_H__

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
#include "Matrix4x4.h"

namespace caret {

    class GraphicsPrimitive;
    class GraphicsPrimitiveV3fC4f;
    class GraphicsPrimitiveV3fC4ub;
    
    class GraphicsOpenGLPolylineQuads : public CaretObject {
        
    public:
        /**
         * Join tyupe of how connected lines are drawn to remove gaps between segments
         */
        enum JoinType {
            /** None */
            NONE,
            /* Extends 'outsides' of lines to meet forming a pointed shape */
            MITER
        };
        
        virtual ~GraphicsOpenGLPolylineQuads();
        
        static GraphicsPrimitive* convertWorkbenchLinePrimitiveTypeToOpenGL(void* openglContextPointer,
                                                                            const GraphicsPrimitive* primitive,
                                                                            AString& errorMessageOut);
        
        static bool draw(void* openglContextPointer,
                         const GraphicsPrimitive* primitive);
        
        static bool drawLinesPerVertexFloatColor(void* openglContextPointer,
                                            const std::vector<float>& xyz,
                                            const std::vector<float>& rgba,
                                            const float lineThicknessPixels);
        
        static bool drawLinesSolidFloatColor(void* openglContextPointer,
                                        const std::vector<float>& xyz,
                                        const float rgba[4],
                                        const float lineThicknessPixels);
        
        static bool drawLineStripSolidFloatColor(void* openglContextPointer,
                                            const std::vector<float>& xyz,
                                        const float rgba[4],
                                        const float lineThicknessPixels);
        
        static bool drawLineLoopSolidFloatColor(void* openglContextPointer,
                                           const std::vector<float>& xyz,
                                           const float rgba[4],
                                           const float lineThicknessPixels);
        
        
        static bool drawLinesPerVertexByteColor(void* openglContextPointer,
                                                 const std::vector<float>& xyz,
                                                 const std::vector<uint8_t>& rgba,
                                                 const float lineThicknessPixels);
        
        static bool drawLinesSolidByteColor(void* openglContextPointer,
                                             const std::vector<float>& xyz,
                                             const uint8_t rgba[4],
                                             const float lineThicknessPixels);
        
        static bool drawLineStripSolidByteColor(void* openglContextPointer,
                                                 const std::vector<float>& xyz,
                                                 const uint8_t rgba[4],
                                                 const float lineThicknessPixels);
        
        static bool drawLineLoopSolidByteColor(void* openglContextPointer,
                                                const std::vector<float>& xyz,
                                                const uint8_t rgba[4],
                                                const float lineThicknessPixels);
        
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
         * Type of line drawing
         */
        enum class LineType {
            /** Each pair of vertices is an independent line segment (GL_LINES) */
            LINES,
            /** A connected set of lines forming a loop (last is automaticall connected to first) */
            LINE_LOOP,
            /** A connected set of lines (last is not connected to first) */
            LINE_STRIP
        };

        /**
         * Indices into input vertices of line
         * that formed the quad
         */
        class QuadInfo {
        public:
            QuadInfo(const int32_t vertexOneIndex,
                     const int32_t vertexTwoIndex)
            : m_vertexOneIndex(vertexOneIndex),
            m_vertexTwoIndex(vertexTwoIndex) { }
            
            int32_t m_vertexOneIndex;
            int32_t m_vertexTwoIndex;
        };
        
        /**
         * Quads created by joining of lines
         */
        class QuadJoin {
        public:
            QuadJoin(const float xyzOne[3],
                     const float xyzTwo[3],
                     const float xyzThree[3],
                     const float xyzFour[3],
                     const int32_t primitiveOneTwoIndex,
                     const int32_t primitiveThreeFourIndex)
            : m_primitiveOneTwoIndex(primitiveOneTwoIndex),
            m_primitiveThreeFourIndex(primitiveThreeFourIndex) {
                m_xyz.insert(m_xyz.end(), xyzOne,   xyzOne + 3);
                m_xyz.insert(m_xyz.end(), xyzTwo,   xyzTwo + 3);
                m_xyz.insert(m_xyz.end(), xyzThree, xyzThree + 3);
                m_xyz.insert(m_xyz.end(), xyzFour,  xyzFour + 3);
            }
            
            std::vector<float> m_xyz;
            int32_t m_primitiveOneTwoIndex;
            int32_t m_primitiveThreeFourIndex;
        };
        
        GraphicsOpenGLPolylineQuads(void* openglContextPointer,
                                  const std::vector<float>& xyz,
                                  const std::vector<float>& floatRGBA,
                                  const std::vector<uint8_t>& byteRGBA,
                                  const std::set<int32_t>& vertexPrimitiveRestartIndices,
                                  const float lineThicknessPixels,
                                  const ColorType colorType,
                                  const LineType lineType);
        
        GraphicsOpenGLPolylineQuads(const GraphicsOpenGLPolylineQuads& obj);
        
        GraphicsOpenGLPolylineQuads& operator=(const GraphicsOpenGLPolylineQuads& obj) const;
        
        static bool drawLinesPrivate(void* openglContextPointer,
                                     const std::vector<float>& xyz,
                                     const std::vector<float>& floatRGBA,
                                     const std::vector<uint8_t>& byteRGBA,
                                     const std::set<int32_t>& vertexPrimitiveRestartIndices,
                                     const float lineThicknessPixels,
                                     const ColorType colorType,
                                     const LineType lineType);
        
        bool performDrawing();
        
        GraphicsPrimitive* convertLinesToPolygons(AString& errorMessageOut);
        
        void saveOpenGLState();
        
        void restoreOpenGLState();
        
        void createProjectionMatrix();
        
        void convertFromModelToWindowCoordinate(const float modelXYZ[3],
                                                float windowXYZOut[3]) const;
        
        void createWindowCoordinatesFromVertices();
        
        void convertLineSegmentsToQuads();
        
        void createQuadFromWindowVertices(const int32_t indexOne,
                                          const int32_t indexTwo);
        
        void drawQuads();
        
        void performJoin(const int32_t lineVertexIndexOne,
                         const int32_t lineVertexIndexTwo,
                         const int32_t lineVertexIndexThree,
                         const int32_t quadOneVertexIndex,
                         const int32_t quadTwoVertexIndex);
        
        void joinQuads();
        
        void addMiterJoinQuads();
        
        void createMiterJoinVertex(const float v1[3],
                                   const float v2[3],
                                   const float miterLength,
                                   float xyzOut[3]);
        
        void* m_openglContextPointer;
        
        const std::vector<float>& m_inputXYZ;
        
        const std::vector<float>& m_inputFloatRGBA;
        
        const std::vector<uint8_t>& m_inputByteRGBA;
        
        std::set<int32_t> m_vertexPrimitiveRestartIndices;
        
        const float m_lineThicknessPixels;
        
        const ColorType m_colorType;
        
        const LineType m_lineType;
        
        std::vector<float> m_vertexWindowXYZ;
        
        std::vector<int32_t> m_vertexWindowInputIndices;
        
        //std::vector<float> m_windowQuadsXYZ;
        
        //std::vector<float> m_windowQuadsRGBA;
        
        JoinType m_joinType = JoinType::MITER;
        
        bool m_debugFlag = false;
        
        int m_savedPolygonMode[2];
        
        int m_savedViewport[4];
        
        Matrix4x4 m_projectionMatrix;
        
        /*
         * The base class graphics primitive that points to 
         * either the Float or Byte Color primitive and can
         * be used for non-color operations.  NEVER delete
         * this as it points to one the color primitive
         */
        GraphicsPrimitive* m_primitive = NULL;
        
        std::unique_ptr<GraphicsPrimitiveV3fC4f> m_primitiveFloatColor;
        
        std::unique_ptr<GraphicsPrimitiveV3fC4ub> m_primitiveByteColor;
        
        std::vector<QuadInfo> m_quadInfo;
        
        std::vector<QuadJoin> m_quadJoins;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_OPENGL_POLYLINE_QUADS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OPENGL_POLYLINE_QUADS_DECLARE__

} // namespace
#endif  //__GRAPHICS_OPENGL_POLYLINE_QUADS_H__
