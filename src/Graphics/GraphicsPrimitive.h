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

#include <array>
#include <map>
#include <memory>
#include <set>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "GraphicsLineMeanDeviationSettings.h"
#include "GraphicsTextureMagnificationFilterEnum.h"
#include "GraphicsTextureMinificationFilterEnum.h"

namespace caret {

    class BoundingBox;
    class GraphicsEngineDataOpenGL;
    class GraphicsPrimitiveV3f;
    class GraphicsPrimitiveV3fC4f;
    class GraphicsPrimitiveV3fC4ub;
    class GraphicsPrimitiveV3fN3f;
    class GraphicsPrimitiveV3fN3fC4f;
    class GraphicsPrimitiveV3fN3fC4ub;
    class GraphicsPrimitiveV3fT2f;
    class GraphicsPrimitiveV3fT3f;
    class Matrix4x4Interface;
    
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
         * Dimension type of texture components
         */
        enum class TextureDimensionType {
            /** No texture coordinates */
            NONE,
            /** User supplie two float values per vertex contain S and T texture coordinates, R is added by code and is always 0.  Used for 2D images */
            FLOAT_STR_2D,
            /** Three float values per vertex contain S, T, and R texture coordinates.  Used for 3D images (volumes) */
            FLOAT_STR_3D
        };
        
        /**
         * Format of pixel data
         */
        enum class TexturePixelFormatType {
            /** None - not texture primitive*/
            NONE,
            /** Blue, green, red */
            BGR,
            /** Blue, green, red, alpha */
            BGRA,
            /** Red, green, blue */
            RGB,
            /** Red, green, blue, alpha */
            RGBA
        };
         
        /*
         * Location of first pixel in the texture image
         */
        enum class TexturePixelOrigin {
            /* None - not texture primitive */
            NONE,
            /* First pixel is at bottom left of texture image*/
            BOTTOM_LEFT,
            /* First pixel is at top left of texture image*/
            TOP_LEFT
        };
        
        /**
         * Texture wrapping type
         */
        enum class TextureWrappingType {
            /** Clamp so max STR is 1.0 (default) */
            CLAMP,
            /** Clamp so max STR is 1.0 (default).  If no texels (voxels) are available, if pixel
                maps to area outside of the volume, the border color is used. */
            CLAMP_TO_BORDER,
            /** Repeat so max STR is greater than 1.0) */
            REPEAT
        };
        
        /**
         * Texture Mip Mapping Type
         */
        enum class TextureMipMappingType {
            /** Mip mapping disabled */
            DISABLED,
            /** Mip mapping enabled */
            ENABLED
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
         * Modes for releasing instance data after OpenGL buffers have been loaded.
         * For large primitives, this can save substantial memory.  Do not use if
         * data will be updated.
         */
        enum class ReleaseInstanceDataMode {
            /**
             * Release of instance data has been completed
             */
            COMPLETED,
            /**
             * Release of instance data is disabled (DEFAULT)
             */
            DISABLED,
            /**
             * Release is enabled but not yet completed (OpenGL buffers not loaded yet)
             */
            ENABLED
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
        GraphicsPrimitive(const VertexDataType        vertexDataType,
                          const NormalVectorDataType  normalVectorDataType,
                          const ColorDataType         colorDataType,
                          const VertexColorType       vertexColorType,
                          const TextureDimensionType  textureDimensionType,
                          const TexturePixelFormatType texturePixelFormatType,
                          const TexturePixelOrigin    texturePixelOrigin,
                          const TextureWrappingType   textureWrappingType,
                          const TextureMipMappingType textureMipMappingType,
                          const GraphicsTextureMagnificationFilterEnum::Enum textureMagnificationFilter,
                          const GraphicsTextureMinificationFilterEnum::Enum textureMinificationFilter,
                          const PrimitiveType         primitiveType);
        
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
        
        static GraphicsPrimitiveV3fN3fC4f* newPrimitiveV3fN3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fN3fC4ub* newPrimitiveV3fN3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fC4f* newPrimitiveV3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fC4ub* newPrimitiveV3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        static GraphicsPrimitiveV3fT2f* newPrimitiveV3fT2f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                                           const uint8_t* imageBytesRGBA,
                                                           const int32_t imageWidth,
                                                           const int32_t imageHeight,
                                                           const int32_t imageRowStride,
                                                           const TexturePixelFormatType texturePixelFormatType,
                                                           const TexturePixelOrigin texturePixelOrigin,
                                                           const TextureWrappingType textureWrappingType,
                                                           const TextureMipMappingType textureMipMappingType,
                                                           const GraphicsTextureMagnificationFilterEnum::Enum textureMagnificationFilter,
                                                           const GraphicsTextureMinificationFilterEnum::Enum textureMinificationFilter,
                                                           const std::array<float, 4>& textureBorderColorRGBA);
        
        static GraphicsPrimitiveV3fT3f* newPrimitiveV3fT3f(const GraphicsPrimitive::PrimitiveType primitiveType,
                                                           const uint8_t* imageBytesRGBA,
                                                           const int32_t imageWidth,
                                                           const int32_t imageHeight,
                                                           const int32_t imageSlices,
                                                           const TexturePixelFormatType texturePixelFormatType,
                                                           const TexturePixelOrigin texturePixelOrigin,
                                                           const TextureWrappingType textureWrappingType,
                                                           const TextureMipMappingType textureMipMappingType,
                                                           const GraphicsTextureMagnificationFilterEnum::Enum textureMagnificationFilter,
                                                           const GraphicsTextureMinificationFilterEnum::Enum textureMinificationFilter,
                                                           const std::array<float, 4>& textureBorderColorRGBA);

        
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
         * @return Mode for release of instance data (xyz, normals, coloring, etc) after buffers are loaded
         */
        ReleaseInstanceDataMode getReleaseInstanceDataMode() const { return m_releaseInstanceDataMode; }
        
        /**
         * Set the mode for release of instance data  (xyz, normals, coloring, etc) after buffers are loaded
         */
        void setReleaseInstanceDataMode(const ReleaseInstanceDataMode releaseDataMode) {
            m_releaseInstanceDataMode = releaseDataMode;
        }
        
        /**
         * @return Pixel format type of texture data
         */
        inline TexturePixelFormatType getTexturePixelFormatType() const { return m_texturePixelFormatType; }
        
        /**
         * @return Location of first pixel in texture image
         */
        inline TexturePixelOrigin getTexturePixelOrigin() const { return m_texturePixelOrigin; }
        
        int32_t getTexturePixelFormatBytesPerPixel() const;

        /**
         * @return Dimension type of texture.
         */
        inline TextureDimensionType getTextureDimensionType() const { return m_textureDimensionType; }
        
        /**
         * @return Type of texture wrapping
         */
        inline TextureWrappingType getTextureWrappingType() const { return m_textureWrappingType; }
        
        /**
         * @return Type of texture mip mapping
         */
        inline TextureMipMappingType getTextureMipMappingType() const { return m_textureMipMappingType; }

        /**
         * @return Type of magnification texture filtering (pixel smaller than texel)
         */
        inline GraphicsTextureMagnificationFilterEnum::Enum getTextureMagnificationFilter() const { return m_textureMagnificationFilter; }
        
        /**
         * @return Type of minification texture filtering (pixel bigger than texel)
         */
        inline GraphicsTextureMinificationFilterEnum::Enum getTextureMinificationFilter() const { return m_textureMinificationFilter; }

        /**
         * Set type of magnification filter (pixel smaller than texel)
         */
        inline void setTextureMagnificationFilter(const GraphicsTextureMagnificationFilterEnum::Enum magFilter) {
            m_textureMagnificationFilter = magFilter;
        }
        
        /**
         * Set type of minification filter (pixel bigger than texel)
         */
        inline void setTextureMinificationFilter(const GraphicsTextureMinificationFilterEnum::Enum minFilter) {
            m_textureMinificationFilter = minFilter;
        }
        
        /** @return The texture border color when used when clamp to border */
        std::array<float, 4> getTextureBorderColorRGBA() const { return m_textureBorderColorRGBA; }
        
        /** Set he texture border color when used when clamp to border @param rgba*/
        void setTextureBorderColorRGBA(const std::array<float, 4>& rgba) {
            m_textureBorderColorRGBA = rgba;
        }
        
        /** @return Width of texture */
        int32_t getTextureImageWidth() const { return m_textureImageWidth; }
        
        /** @return Height of texture */
        int32_t getTextureImageHeight() const { return m_textureImageHeight; }
        
        /** @return Slices of texture */
        int32_t getTextureImageSlices() const { return m_textureImageSlices; }
        
        /**
         * @return The float coordinates.
         */
        const std::vector<float>& getFloatXYZ() const { return m_xyz; }
        
        void getVertexFloatXYZ(const int32_t vertexIndex,
                               float xyzOut[3]) const;
        
        void replaceFloatXYZ(const std::vector<float>& xyz);
        
        void getFloatYComponents(std::vector<float>& yComponentsOut) const;
        
        void setFloatYComponents(const std::vector<float>& yComponents);
        
        /**
         * @return The number of vertices
         */
        inline int32_t getNumberOfVertices() const { return (m_xyz.size() / 3); }
        
        void replaceVertexFloatXYZ(const int32_t vertexIndex,
                                   const float xyz[3]);
        
        void replaceAndTransformVertices(const GraphicsPrimitive* primitive,
                                         const Matrix4x4Interface& matrix);
        
        void transformVerticesFloatXYZ(const Matrix4x4Interface& matrix);
        
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
        
        void replaceVertexTextureSTR(const int32_t vertexIndex,
                                     const float str[3]);
        
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

        void simplfyLines(const int32_t skipVertexCount);
        
        void getMeanAndStandardDeviationForY(float& yMeanOut,
                                             float& yStandardDeviationOut) const;
        
        void applyNewMeanAndDeviationToYComponents(const GraphicsLineMeanDeviationSettings& settings,
                                                   bool& haveNanInfFlagOut);
        
        static AString getNewMeanDeviationOperationDescriptionInHtml();
        
    protected:
        AString toStringPrivate(const bool includeAllDataFlag) const;
        
        void setTextureImage(const uint8_t* imageBytesRGBA,
                             const int32_t imageWidth,
                             const int32_t imageHeight,
                             const int32_t imageSlices,
                             const int32_t rowStride);
        
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
        
        void invalidateVertexMeasurements();
        
        const VertexDataType  m_vertexDataType;
        
        const NormalVectorDataType m_normalVectorDataType;
        
        const ColorDataType  m_colorDataType;
        
        const VertexColorType m_vertexColorType;
        
        const TextureDimensionType m_textureDimensionType;
        
        const TexturePixelFormatType m_texturePixelFormatType;
        
        const TexturePixelOrigin m_texturePixelOrigin;
        
        const TextureWrappingType m_textureWrappingType;
        
        const TextureMipMappingType m_textureMipMappingType;
        
        GraphicsTextureMagnificationFilterEnum::Enum m_textureMagnificationFilter = GraphicsTextureMagnificationFilterEnum::LINEAR;
        
        GraphicsTextureMinificationFilterEnum::Enum m_textureMinificationFilter = GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR;

        std::array<float, 4> m_textureBorderColorRGBA;
        
        const PrimitiveType m_primitiveType;

        ReleaseInstanceDataMode m_releaseInstanceDataMode = ReleaseInstanceDataMode::DISABLED;
        
        mutable bool m_boundingBoxValid = false;
        
        UsageType m_usageTypeCoordinates = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        UsageType m_usageTypeNormals = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        UsageType m_usageTypeColors = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        UsageType m_usageTypeTextureCoordinates = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        std::unique_ptr<GraphicsEngineDataOpenGL> m_graphicsEngineDataForOpenGL;
        
        int32_t m_textureImageWidth = -1;
        
        int32_t m_textureImageHeight = -1;
        
        int32_t m_textureImageSlices = -1;
        
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
        
        void setOpenGLBuffersHaveBeenLoadedByGraphicsEngine();
        
        void applyNewMeanAndDeviationToYComponentsNoNaNs(std::vector<float>& data,
                                                         const GraphicsLineMeanDeviationSettings& settings);
        
        void applyNewMeanAndDeviationToYComponentsWithNaNs(std::vector<float>& data,
                                                           const GraphicsLineMeanDeviationSettings& settings);

        std::vector<float> m_xyz;
        
        std::vector<float> m_floatNormalVectorXYZ;
        
        std::vector<float> m_floatRGBA;
        
        std::vector<uint8_t> m_unsignedByteRGBA;
        
        std::vector<float> m_floatTextureSTR;
        
        std::vector<uint8_t> m_textureImageBytesPtr;

        mutable float m_yMean = 0.0;
        
        mutable float m_yStandardDeviation = -1.0;
        
        friend class GraphicsEngineDataOpenGL;
        friend class GraphicsOpenGLPolylineTriangles;
        friend class GraphicsPrimitiveSelectionHelper;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_H__
