#ifndef __GRAPHICS_OPEN_G_L_LINE_DRAWING_H__
#define __GRAPHICS_OPEN_G_L_LINE_DRAWING_H__

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

#include "CaretObject.h"
#include "Matrix4x4.h"

namespace caret {

    class GraphicsPrimitive;
    
    class GraphicsOpenGLLineDrawing : public CaretObject {
        
    public:
//        GraphicsOpenGLLineDrawing(const GraphicsPrimitive* primitive,
//                                  const float lineThicknessPixels);
        
        virtual ~GraphicsOpenGLLineDrawing();
        
        //bool run(AString& errorMessageOut);

        static bool drawLinesSolidColor(const std::vector<float>& xyz,
                                        const float rgba[4],
                                        const float lineThicknessPixels);
        
        static bool drawLineStripSolidColor(const std::vector<float>& xyz,
                                        const float rgba[4],
                                        const float lineThicknessPixels);
        
        static bool drawLineLoopSolidColor(const std::vector<float>& xyz,
                                        const float rgba[4],
                                        const float lineThicknessPixels);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        /**
         * Type of line coloring
         */
        enum class ColorType {
            /** color at each vertex */
            RGBA_PER_VERTEX,
            /** one color for all vertices */
            RGBA_SOLID
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
        
        GraphicsOpenGLLineDrawing(const std::vector<float>& xyz,
                                  const std::vector<float>& rgba,
                                  const float lineThicknessPixels,
                                  const ColorType colorType,
                                  const LineType lineType);
        
        GraphicsOpenGLLineDrawing(const GraphicsOpenGLLineDrawing& obj);
        
        GraphicsOpenGLLineDrawing& operator=(const GraphicsOpenGLLineDrawing& obj) const;
        
        static bool drawLinesPrivate(const std::vector<float>& xyz,
                                     const std::vector<float>& rgba,
                                     const float lineThicknessPixels,
                                     const ColorType colorType,
                                     const LineType lineType);
        
        bool performDrawing();
        
        void saveOpenGLState();
        
        void restoreOpenGLState();
        
        void createProjectionMatrix();
        
        void convertFromModelToWindowCoordinate(const float modelXYZ[3],
                                                float windowXYZOut[3]) const;
        
        //void convertPointsToWindowCoordinates();
        
        void createWindowCoordinatesFromVertices();
        
        void convertLineSegmentsToQuads();
        
        void createQuadFromWindowVertices(const int32_t indexOne,
                                          const int32_t indexTwo);
        
        void drawQuads();
        
        const GraphicsPrimitive* m_inputPrimitive;
        
        const std::vector<float>& m_inputXYZ;
        
        const std::vector<float>& m_inputRGBA;
        
        const float m_lineThicknessPixels;
        
        const ColorType m_colorType;
        
        const LineType m_lineType;
        
        GraphicsPrimitive* m_outputPrimitive = NULL;
        
        std::vector<float> m_vertexWindowXYZ;
        
        std::vector<float> m_windowQuadsXYZ;
        
        std::vector<float> m_windowQuadsRGBA;
        
        bool m_debugFlag = false;
        
        int m_savedPolygonMode[2];
        
        int m_savedViewport[4];
        
        Matrix4x4 m_projectionMatrix;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_OPEN_G_L_LINE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OPEN_G_L_LINE_DRAWING_DECLARE__

} // namespace
#endif  //__GRAPHICS_OPEN_G_L_LINE_DRAWING_H__
