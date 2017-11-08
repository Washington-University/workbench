#ifndef __GRAPHICS_PRIMITIVE_H__
#define __GRAPHICS_PRIMITIVE_H__

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


#include <map>
#include <memory>
#include <set>

#include "CaretObject.h"
#include "EventListenerInterface.h"


namespace caret {

    class BoundingBox;
    class GraphicsEngineDataOpenGL;
    class GraphicsPrimitiveV3f;
    class GraphicsPrimitiveV3fC4f;
    class GraphicsPrimitiveV3fC4ub;
    class GraphicsPrimitiveV3fT3f;
    
    class GraphicsPrimitive : public CaretObject, public EventListenerInterface {
        
    public:
        /**
         * Type of vertices
         */
        enum class VertexType {
            /** Vertices are three float values per vertex containing X, Y, Z */
            FLOAT_XYZ
        };
        
        /**
         * Type of normal vectors
         */
        enum class NormalVectorType {
            /** No normal vectors */
            NONE,
            /** Normal vectors are three float values per vertex contain X, Y, Z ranging 0.0 to 1.0 */
            FLOAT_XYZ
        };
        
        /**
         * Type of colors
         */
        enum class ColorType {
            /** No color components */
            NONE,
            /** Four float values per vertex containing Red, Green, Blue, Alpha ranging 0.0 to 1.0 */
            FLOAT_RGBA,
            /** Four unsigned byte values per vertex containing Red, Green, Blue, Alpha ranging 0 to 255 */
            UNSIGNED_BYTE_RGBA
        };
        
        /**
         * Type of texture components
         */
        enum class TextureType {
            /** No texture coordinates */
            NONE,
            /** Three float values per vertex contains S, T, and R texture coordinates */
            FLOAT_STR
        };
        
        /**
         * Type of primitives for drawing
         * Descriptions are copied from the glVertex man page.
         */
        enum class PrimitiveType {
            /**
             * Draws a connected group of line segments from the first vertex to the last.  
             * Vertices n and n+1 define line n.  n-1 lines are drawn. 
             * Note: OpenGL supports a limited range of line widths, usually [0.5, 10] pixels
             */
            OPENGL_LINE_LOOP,
            /** 
             * Treats each pair of vertices as an independent line  segment.
             * Vertices  2n-1  and  2n define line n.  n/2 lines are drawn.
             * Note: OpenGL supports a limited range of line widths, usually [0.5, 10] pixels
             */
            OPENGL_LINE_STRIP,
            /** 
             * Draws a connected group of line segments from the first vertex to the last,  then  back
             * to the first.  Vertices n and n+1 define line n.  The last line, however, is defined by
             * vertices n and 1.  n lines are drawn. 
             * Note: OpenGL supports a limited range of line widths, usually [0.5, 10] pixels
             */
            OPENGL_LINES,
            /**
             * Treats each vertex as a single point.  Vertex n defines point n.  n points are drawn.
             */
            OPENGL_POINTS,
            /** 
             * Draws a single, convex polygon.  Vertices 1 through n define this polygon. 
             */
            OPENGL_POLYGON,
            /** 
             * Draws  a connected group of quadrilaterals.  One quadrilateral is defined for each pair
             * of vertices presented after the first pair.  Vertices 2n-1, 2n, 2n+2, and  2n+1  define
             * quadrilateral  n.  n/2 quadrilaterals are drawn.  Note that the order in which vertices
             * are used to construct a quadrilateral from strip data is different from that used  with
             * independent data.
             */
            OPENGL_QUAD_STRIP,
            /** 
             * Treats each group of four vertices as an  independent  quadrilateral.
             * Vertices  4n-3, 4n-2, 4n-3, and 4n define quadrilateral n.  n/4 quadrilaterals are drawn.
             */
            OPENGL_QUADS,
            /**
             * Draws a connected group of triangles.  One triangle is defined  for  each  vertex
             * presented  after the first two vertices.  Vertices 1, n+1, and n+2 define triangle n.  n-2
             * triangles are drawn.
             */
            OPENGL_TRIANGLE_FAN,
            /** 
             * Draws  a  connected  group  of triangles.  One triangle is defined for each vertex
             * presented after the first two vertices.  For odd n, vertices n, n+1, and n+2 define
             * triangle  n.   For  even  n,  vertices n+1, n, and n+2 define triangle n.  n-2 triangles are
             * drawn.
             */
            OPENGL_TRIANGLE_STRIP,
            /** 
             * Treats each triplet of vertices as an independent triangle.  Vertices 3n-2,  3n-1,  and
             * 3n define triangle n.  n/3 triangles are drawn. 
             */
            OPENGL_TRIANGLES,
            /**
             * Like OPENGL_LINE_LOOP but there is no limit on line width as it draws the lines using polygons.
             */
            WORKBENCH_LINE_LOOP,
            /**
             * Like OPENGL_LINE_STRIP but there is no limit on line width as it draws the lines using polygons.
             */
            WORKBENCH_LINE_STRIP,
            /**
             * Like OPENGL_LINES but there is no limit on line width as it draws the lines using polygons.
             */
            WORKBENCH_LINES,
        };
        
        /**
         * Type for point size and line width
         */
        enum class SizeType {
            /**
             * Size of point or width of line is a percentage of viewport height.
             * Ranges [0.0, 100.0]
             */
            PERCENTAGE_VIEWPORT_HEIGHT,
            /**
             * Size of point or width of line is in pixels
             */
            PIXELS
        };
        
        /**
         * Hint to graphics engine on how the primitive is used.
         * The hint must be set prior to loading of buffers.
         */
        enum class UsageType {
            /**
             * Buffer is modified once and used a few times
             * Like OpenGL STREAM_DRAW
             */
            MODIFIED_ONCE_DRAWN_FEW_TIMES,
            /**
             * Buffer is modified once and used many times
             * Like OpenGL STATIC_DRAW
             */
            MODIFIED_ONCE_DRAWN_MANY_TIMES,
            /**
             * Buffer is modified many times and used many times
             * Like OpenGL DYNAMIC_DRAW
             */
            MODIFIED_MANY_DRAWN_MANY_TIMES
        };
        
    protected:
        GraphicsPrimitive(const VertexType       vertexType,
                          const NormalVectorType normalVectorType,
                          const ColorType        colorType,
                          const TextureType      textureType,
                          const PrimitiveType    primitiveType);
        
        GraphicsPrimitive(const GraphicsPrimitive& obj);
        
    private:
        GraphicsPrimitive& operator=(const GraphicsPrimitive& obj);
        
    public:
        static GraphicsPrimitiveV3f* newPrimitiveV3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                              const float floatRGBA[4]);
        
        static GraphicsPrimitiveV3f* newPrimitiveV3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                              const uint8_t unsignedByteRGBA[4]);
        
        static GraphicsPrimitiveV3fC4f* newPrimitiveV3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fC4ub* newPrimitiveV3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fT3f* newPrimitiveV3fT3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                                           const uint8_t* imageBytesRGBA,
                                                           const int32_t imageWidth,
                                                           const int32_t imageHeight);
        
        virtual ~GraphicsPrimitive();
        
        void reserveForNumberOfVertices(const int32_t numberOfVertices);
        
        UsageType getUsageType() const;
        
        void setUsageType(const UsageType usage);
        
        virtual void receiveEvent(Event* event);
        
        bool isValid() const;
        
        /**
         * @return Type of the vertices.
         */
        inline VertexType  getVertexType() const { return m_vertexType; }
        
        /**
         * @return Type of the normal vectors.
         */
        inline NormalVectorType getNormalVectorType () const { return m_normalVectorType; }
        
        /**
         * @return Type of the colors.
         */
        inline ColorType  getColorType() const { return m_colorType; }
        
        /**
         * @return Type of primitive.
         */
        inline PrimitiveType getPrimitiveType() const { return m_primitiveType; }
        
        /**
         * @return Type of texture.
         */
        inline TextureType getTextureType() const { return m_textureType; }
        
        /**
         * @return The float coordinates.
         */
        const std::vector<float>& getFloatXYZ() const { return m_xyz; }
        
        void replaceFloatXYZ(const std::vector<float>& xyz);
        
        /**
         * @return The number of vertices
         */
        inline int32_t getNumberOfVertices() const { return (m_xyz.size() / 3); }
        
        void replaceVertexFloatXYZ(const int32_t vertexIndex,
                                   const float xyz[3]);
        
        void getVertexFloatRGBA(const int32_t vertexIndex,
                                float rgbaOut[4]) const;

        void replaceVertexFloatRGBA(const int32_t vertexIndex,
                                    const float rgba[4]);
        
        void getVertexByteRGBA(const int32_t vertexIndex,
                                uint8_t rgbaOut[4]) const;
        
        void replaceVertexByteRGBA(const int32_t vertexIndex,
                                    const uint8_t rgba[4]);
        
        bool getVertexBounds(BoundingBox& boundingBoxOut) const;
        
        void addPrimitiveRestart();
        
        void getPointDiameter(SizeType& sizeTypeOut,
                              float& pointDiameterOut) const;
        
        void setPointDiameter(const SizeType sizeType,
                              const float pointDiameter);
        
        void getLineWidth(SizeType& widthTypeOut,
                          float lineWidthOut) const;
        
        void setLineWidth(const SizeType widthType,
                          const float lineWidth);
        
        GraphicsEngineDataOpenGL* getGraphicsEngineDataForOpenGL();
        
        void setGraphicsEngineDataForOpenGL(GraphicsEngineDataOpenGL* graphicsEngineDataForOpenGL);
        
        virtual AString toString() const override;
        
        virtual void print() const;
        
        /**
         * Clone this primitive.
         */
        virtual GraphicsPrimitive* clone() const = 0;
        
    protected:
        AString toStringPrivate(const bool includeAllDataFlag) const;
        
        void setTextureImage(const uint8_t* imageBytesRGBA,
                             const int32_t imageWidth,
                             const int32_t imageHeight);
        
        void addVertexProtected(const float xyz[3]);
        
        void addVertexProtected(const float x,
                                const float y,
                                const float z);
        
        AString getPrimitiveTypeAsText() const;
        
        AString getSizeTypeAsText(const SizeType sizeType) const;
        
        const VertexType  m_vertexType;
        
        const NormalVectorType m_normalVectorType;
        
        const ColorType  m_colorType;
        
        const TextureType m_textureType;
        
        const PrimitiveType m_primitiveType;

        mutable bool m_boundingBoxValid = false;
        
        UsageType m_usageType = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        std::unique_ptr<GraphicsEngineDataOpenGL> m_graphicsEngineDataForOpenGL;
        
        std::vector<float> m_floatNormalVectorXYZ;
        
        std::vector<float> m_floatRGBA;
        
        std::vector<uint8_t> m_unsignedByteRGBA;
        
        std::vector<float> m_floatTextureSTR;
        
        std::vector<uint8_t> m_textureImageBytesRGBA;
        
        int32_t m_textureImageWidth = -1;
        
        int32_t m_textureImageHeight = -1;
        
        SizeType m_pointSizeType = SizeType::PIXELS;
        
        float m_pointDiameterValue = 1.0f;
        
        SizeType m_lineWidthType = SizeType::PIXELS;
        
        float m_lineWidthValue = 1.0f;
        
        mutable std::unique_ptr<BoundingBox> m_boundingBox;
        
        std::set<int32_t> m_primitiveRestartIndices;
        
    private:
        std::vector<float> m_xyz;
        
        void copyHelperGraphicsPrimitive(const GraphicsPrimitive& obj);

        friend class GraphicsEngineDataOpenGL;
        friend class GraphicsOpenGLLineDrawing;
        friend class GraphicsPrimitiveSelectionHelper;
        
        std::vector<float> m_dummyFloatRGBAVector;
        
        std::vector<uint8_t> m_dummyUnsignedByteRGBAVector;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_H__
