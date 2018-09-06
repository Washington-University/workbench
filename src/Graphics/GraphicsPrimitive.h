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
    class GraphicsPrimitiveV3fN3f;
    class GraphicsPrimitiveV3fT3f;
    
    class GraphicsPrimitive : public CaretObject, public EventListenerInterface {
        
    public:
        /**
         * Data type of vertices
         */
        enum class VertexDataType {
            /** Vertices are three float values per vertex containing X, Y, Z */
            FLOAT_XYZ
        };
        
        /**
         * Data type of normal vectors
         */
        enum class NormalVectorDataType {
            /** No normal vectors */
            NONE,
            /** Normal vectors are three float values per vertex contain X, Y, Z ranging 0.0 to 1.0 */
            FLOAT_XYZ
        };
        
        /**
         * Data type of colors
         */
        enum class ColorDataType {
            /** No color components */
            NONE,
            /** Four float values per vertex containing Red, Green, Blue, Alpha ranging 0.0 to 1.0 */
            FLOAT_RGBA,
            /** Four unsigned byte values per vertex containing Red, Green, Blue, Alpha ranging 0 to 255 */
            UNSIGNED_BYTE_RGBA
        };
       
        /**
         * Coloring for each vertex thype
         */
        enum class VertexColorType {
            /** No color components */
            NONE,
            /** One RGBA for all vertices */
            SOLID_RGBA,
            /** Unique RGBA for each vertex */
            PER_VERTEX_RGBA
        };
        
        /**
         * Data type of texture components
         */
        enum class TextureDataType {
            /** No texture coordinates */
            NONE,
            /** Three float values per vertex contains S, T, and R texture coordinates */
            FLOAT_STR
        };
        
        /**
         * Type of primitives for drawing.  There are NO primitives equivalent to
         * OpenGL's GL_QUAD_STRIP and GL_POLYGON.  The reason is that these
         * primitive types are removed in 3.x (probably 3.2).
         *
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
             * Like OPENGL_LINE_LOOP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a BEVEL join at vertices.
             * Draws in MODEL space so lines are affected by model transformations
             */
            MODEL_SPACE_POLYGONAL_LINE_LOOP_BEVEL_JOIN,
            /**
             * Like OPENGL_LINE_LOOP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a MITER join at vertices
             * Draws in MODEL space so lines are affected by model transformations
             */
            MODEL_SPACE_POLYGONAL_LINE_LOOP_MITER_JOIN,
            /**
             * Like OPENGL_LINE_STRIP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a BEVEL join at vertices
             * Draws in MODEL space so lines are affected by model transformations
             */
            MODEL_SPACE_POLYGONAL_LINE_STRIP_BEVEL_JOIN,
            /**
             * Like OPENGL_LINE_STRIP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a MITER join at vertices
             * Draws in MODEL space so lines are affected by model transformations
             */
            MODEL_SPACE_POLYGONAL_LINE_STRIP_MITER_JOIN,
            /**
             * Like OPENGL_LINES but there is no limit on line width as it draws the lines using polygons.
             * Draws in MODEL space so lines are affected by model transformations
             */
            MODEL_SPACE_POLYGONAL_LINES,
            /**
             * Like OPENGL_LINE_LOOP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a BEVEL join at vertices.
             * Lines drawn in window space so that lines always face user
             */
            POLYGONAL_LINE_LOOP_BEVEL_JOIN,
            /**
             * Like OPENGL_LINE_LOOP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a MITER join at vertices
             * Lines drawn in window space so that lines always face user
             */
            POLYGONAL_LINE_LOOP_MITER_JOIN,
            /**
             * Like OPENGL_LINE_STRIP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a BEVEL join at vertices
             * Lines drawn in window space so that lines always face user
             */
            POLYGONAL_LINE_STRIP_BEVEL_JOIN,
            /**
             * Like OPENGL_LINE_STRIP but there is no limit on line width as it draws the lines using polygons
             * and polygons that form the line use a MITER join at vertices
             * Lines drawn in window space so that lines always face user
             */
            POLYGONAL_LINE_STRIP_MITER_JOIN,
            /**
             * Like OPENGL_LINES but there is no limit on line width as it draws the lines using polygons.
             * Lines drawn in window space so that lines always face user
             */
            POLYGONAL_LINES,
            /*
             * Draws sphere at each vertex
             */
            SPHERES
        };
        
        /**
         * Type for point size
         */
        enum class PointSizeType {
            /**
             * Millimeters (all models surface, volume, etc. are drawn
             * in millimeters).
             */
            MILLIMETERS,
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
         * Type for line width
         */
        enum class LineWidthType {
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
         * Type for sphere size
         */
        enum class SphereSizeType {
            /**
             * Millimeters (all models surface, volume, etc. are drawn 
             * in millimeters).
             */
            MILLIMETERS
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
        GraphicsPrimitive(const VertexDataType       vertexDataType,
                          const NormalVectorDataType normalVectorDataType,
                          const ColorDataType        colorDataType,
                          const VertexColorType      vertexColorType,
                          const TextureDataType      textureDataType,
                          const PrimitiveType        primitiveType);
        
        GraphicsPrimitive(const GraphicsPrimitive& obj);
        
    private:
        GraphicsPrimitive& operator=(const GraphicsPrimitive& obj);
        
    public:
        static GraphicsPrimitiveV3f* newPrimitiveV3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                              const float floatRGBA[4]);
        
        static GraphicsPrimitiveV3f* newPrimitiveV3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                              const uint8_t unsignedByteRGBA[4]);
        
        static GraphicsPrimitiveV3fN3f* newPrimitiveV3fN3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                                           const uint8_t unsignedByteRGBA[4]);
        
        static GraphicsPrimitiveV3fC4f* newPrimitiveV3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fC4ub* newPrimitiveV3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fT3f* newPrimitiveV3fT3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                                           const uint8_t* imageBytesRGBA,
                                                           const int32_t imageWidth,
                                                           const int32_t imageHeight);
        
        virtual ~GraphicsPrimitive();
        
        void reserveForNumberOfVertices(const int32_t numberOfVertices);
        
        UsageType getUsageTypeCoordinates() const;
        
        UsageType getUsageTypeNormals() const;
        
        UsageType getUsageTypeColors() const;
        
        UsageType getUsageTypeTextureCoordinates() const;
        
        void setUsageTypeAll(const UsageType usage);
        
        void setUsageTypeCoordinates(const UsageType usage);
        
        void setUsageTypeNormals(const UsageType usage);
        
        void setUsageTypeColors(const UsageType usage);
        
        void setUsageTypeTextureCoordinates(const UsageType usage);
        
        virtual void receiveEvent(Event* event);
        
        bool isValid() const;
        
        /**
         * @return Data type of the vertices.
         */
        inline VertexDataType  getVertexDataType() const { return m_vertexDataType; }
        
        /**
         * @return Data type of the normal vectors.
         */
        inline NormalVectorDataType getNormalVectorDataType () const { return m_normalVectorDataType; }
        
        /**
         * @return Data type of the colors.
         */
        inline ColorDataType  getColorDataType() const { return m_colorDataType; }
        
        /**
         * @return Type of vertex colors
         */
        inline VertexColorType getVertexColorType() const { return m_vertexColorType; }
        
        /**
         * @return Type of primitive.
         */
        inline PrimitiveType getPrimitiveType() const { return m_primitiveType; }
        
        /**
         * @return Data type of texture.
         */
        inline TextureDataType getTextureDataType() const { return m_textureDataType; }
        
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
        
        void replaceAllVertexSolidByteRGBA(const uint8_t rgba[4]);
        
        void replaceAllVertexSolidFloatRGBA(const float rgba[4]);
        
        bool getVertexBounds(BoundingBox& boundingBoxOut) const;
        
        void addPrimitiveRestart();
        
        void getPointDiameter(PointSizeType& sizeTypeOut,
                              float& pointDiameterOut) const;
        
        void setPointDiameter(const PointSizeType sizeType,
                              const float pointDiameter) const;
        
        void getLineWidth(LineWidthType& widthTypeOut,
                          float& lineWidthOut) const;
        
        void setLineWidth(const LineWidthType widthType,
                          const float lineWidth) const;
        
        void getSphereDiameter(SphereSizeType& sizeTypeOut,
                               float& sphereDiameterOut) const;
        
        void setSphereDiameter(const SphereSizeType sizeType,
                               const float sphereDiameter) const;
        
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
        
        void addVertexProtected(const float xyz[3],
                                const float normalVector[3],
                                const float rgbaFloat[4],
                                const uint8_t rgbaByte[4],
                                const float textureSTR[3]);
        
        AString getPrimitiveTypeAsText() const;
        
        AString getLineWidthTypeAsText(const LineWidthType lineWidthType) const;
        
        AString getPointSizeTypeAsText(const PointSizeType sizeType) const;
        
        AString getSphereSizeTypeAsText(const SphereSizeType sizeType) const;
        
        AString getVertexColorTypeAsText(const VertexColorType vertexColorType) const;
        
        const VertexDataType  m_vertexDataType;
        
        const NormalVectorDataType m_normalVectorDataType;
        
        const ColorDataType  m_colorDataType;
        
        const VertexColorType m_vertexColorType;
        
        const TextureDataType m_textureDataType;
        
        const PrimitiveType m_primitiveType;

        mutable bool m_boundingBoxValid = false;
        
        UsageType m_usageTypeCoordinates = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        UsageType m_usageTypeNormals = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        UsageType m_usageTypeColors = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        UsageType m_usageTypeTextureCoordinates = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        std::unique_ptr<GraphicsEngineDataOpenGL> m_graphicsEngineDataForOpenGL;
        
        int32_t m_textureImageWidth = -1;
        
        int32_t m_textureImageHeight = -1;
        
        mutable PointSizeType m_pointSizeType = PointSizeType::PIXELS;
        
        mutable float m_pointDiameterValue = 1.0f;
        
        mutable LineWidthType m_lineWidthType = LineWidthType::PIXELS;
        
        mutable float m_lineWidthValue = 1.0f;
        
        mutable SphereSizeType m_sphereSizeType = SphereSizeType::MILLIMETERS;
        
        mutable float m_sphereDiameterValue = 1.0f;
        
        mutable std::unique_ptr<BoundingBox> m_boundingBox;
        
        std::set<int32_t> m_polygonalLinePrimitiveRestartIndices;
        
        int32_t m_triangleStripPrimitiveRestartIndex = -1;
        
    private:
        
        void copyHelperGraphicsPrimitive(const GraphicsPrimitive& obj);
        
        void copyVertex(const int32_t copyFromIndex,
                        const int32_t copyToIndex);
        
        void fillTriangleStripPrimitiveRestartVertices();
        
        std::vector<float> m_xyz;
        
        std::vector<float> m_floatNormalVectorXYZ;
        
        std::vector<float> m_floatRGBA;
        
        std::vector<uint8_t> m_unsignedByteRGBA;
        
        std::vector<float> m_floatTextureSTR;
        
        std::vector<uint8_t> m_textureImageBytesRGBA;
        friend class GraphicsEngineDataOpenGL;
        friend class GraphicsOpenGLPolylineTriangles;
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
