#ifndef __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_H__
#define __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_H__

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

#include "stdint.h"

#include <vector>

#include "BrainOpenGL.h"

namespace caret {

    class BrainOpenGLPrimitiveDrawing {
        
    public:
        static void drawQuads(const std::vector<float>& coordinates,
                       const std::vector<float>& normals,
                       const std::vector<float>& rgbaColors);
        
        static void drawQuads(const std::vector<float>& coordinates,
                              const std::vector<float>& normals,
                              const std::vector<uint8_t>& rgbaColors);
        
        static void drawQuadIndices(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                    const std::vector<uint8_t>& rgbaColors,
                                    const std::vector<uint32_t>& quadIndices);
        
        static void drawQuadStrips(const std::vector<float>& coordinates,
                                   const std::vector<float>& normals,
                                   const std::vector<uint8_t>& rgbaColors,
                                   const std::vector<uint32_t>& quadStripIndices);
        
        static void drawLineLoop(const std::vector<float>& coordinates,
                              const float rgba[4],
                              const float lineWidth);
        
        static void drawLineLoop(const std::vector<float>& coordinates,
                              const uint8_t rgba[4],
                              const float lineWidth);
        
        static void drawLines(const std::vector<float>& coordinates,
                              const float rgba[4],
                              const float lineWidth);
        
        static void drawLines(const std::vector<float>& coordinates,
                              const uint8_t rgba[4],
                              const float lineWidth);
        
        static void drawLines(const std::vector<float>& coordinates,
                              const std::vector<float>& rgbaColors,
                              const float lineWidth);
        
        static void drawLines(const std::vector<float>& coordinates,
                              const std::vector<uint8_t>& rgbaColors,
                              const float lineWidth);
        
        static void drawLineStrip(const std::vector<float>& coordinates,
                              const float rgba[4],
                              const float lineWidth);
        
        static void drawLineStrip(const std::vector<float>& coordinates,
                              const uint8_t rgba[4],
                              const float lineWidth);
        
        static void drawLineStrip(const std::vector<float>& coordinates,
                              const std::vector<float>& rgbaColors,
                              const float lineWidth);
        
        static void drawLineStrip(const std::vector<float>& coordinates,
                              const std::vector<uint8_t>& rgbaColors,
                              const float lineWidth);
        
        static void drawPolygon(const std::vector<float>& coordinates,
                                const std::vector<float>& normals,
                                const float rgba[4]);
        
        static void drawPolygon(const std::vector<float>& coordinates,
                                const std::vector<float>& normals,
                                const uint8_t rgba[4]);
        
        static void drawRectangleOutline(const float bottomLeft[3],
                                         const float bottomRight[3],
                                         const float topRight[3],
                                         const float topLeft[3],
                                         const float lineThickness,
                                         const float rgba[4]);
        
//        static void drawRectangle(const float bottomLeft[3],
//                                  const float bottomRight[3],
//                                  const float topRight[3],
//                                  const float topLeft[3],
//                                  const float rgba[4]);
        
    private:
        BrainOpenGLPrimitiveDrawing();
        
        ~BrainOpenGLPrimitiveDrawing();
        
        BrainOpenGLPrimitiveDrawing(const BrainOpenGLPrimitiveDrawing&);

        BrainOpenGLPrimitiveDrawing& operator=(const BrainOpenGLPrimitiveDrawing&);
        
        static void drawQuadsImmediateMode(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                    const std::vector<uint8_t>& rgbaColors,
                                           const int64_t coordinateOffset,
                                           const int64_t coordinateCount);
        
        static void drawQuadsVertexArrays(const std::vector<float>& coordinates,
                                   const std::vector<float>& normals,
                                          const std::vector<uint8_t>& rgbaColors,
                                          const int64_t coordinateOffset,
                                          const int64_t coordinateCount);
        
        static void drawQuadStripsVertexArrays(const std::vector<float>& coordinates,
                                   const std::vector<float>& normals,
                                               const std::vector<uint8_t>& rgbaColors,
                                               const std::vector<uint32_t>& quadStripIndices);
        
        static void drawQuadsVertexBuffers(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                           const std::vector<uint8_t>& rgbaColors,
                                           const int64_t coordinateOffset,
                                           const int64_t coordinateCount);
        
        static void drawQuadIndicesVertexArrays(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                    const std::vector<uint8_t>& rgbaColors,
                                    const std::vector<uint32_t>& quadIndices);
        
        static void drawPrimitiveWithVertexArrays(GLenum mode,
                                                  const std::vector<float>& coordinates,
                                                  const std::vector<float>& normals,
                                                  const uint8_t rgba[4],
                                                  const float lineWidth);
        
        static void drawPrimitiveWithVertexArrays(GLenum mode,
                                                  const std::vector<float>& coordinates,
                                                  const std::vector<float>& normals,
                                                  const std::vector<uint8_t>& rgba,
                                                  const float lineWidth);
    };
    
#ifdef __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_PRIMITIVE_DRAWING_H__
