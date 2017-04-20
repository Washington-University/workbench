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

#include "CaretObject.h"

#include "EventListenerInterface.h"


namespace caret {

    class GraphicsEngineDataOpenGL;
    class GraphicsPrimitiveV3f;
    class GraphicsPrimitiveV3fC4f;
    class GraphicsPrimitiveV3fC4ub;
    
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
            /** Four float values per vertex containing Red, Green, Blue, Alpha ranging 0.0 to 1.0 */
            FLOAT_RGBA,
            /** Four unsigned byte values per vertex containing Red, Green, Blue, Alpha ranging 0 to 255 */
            UNSIGNED_BYTE_RGBA
        };
        
        /**
         * Type of primitives for drawing
         * Descriptions are copied from the glVertex man page.
         */
        enum class PrimitiveType {
            /**
             * Draws a connected group of line segments from the first vertex to the last.  
             * Vertices n and n+1 define line n.  n-1 lines are drawn. 
             */
            LINE_LOOP,
            /** 
             * Treats each pair of vertices as an independent line  segment.
             * Vertices  2n-1  and  2n define line n.  n/2 lines are drawn.
             */
            LINE_STRIP,
            /** 
             * Draws a connected group of line segments from the first vertex to the last,  then  back
             * to the first.  Vertices n and n+1 define line n.  The last line, however, is defined by
             * vertices n and 1.  n lines are drawn. 
             */
            LINES,
            /**
             * Treats each vertex as a single point.  Vertex n defines point n.  n points are drawn.
             */
            POINTS,
            /** 
             * Draws a single, convex polygon.  Vertices 1 through n define this polygon. 
             */
            POLYGON,
            /** 
             * Draws  a connected group of quadrilaterals.  One quadrilateral is defined for each pair
             * of vertices presented after the first pair.  Vertices 2n-1, 2n, 2n+2, and  2n+1  define
             * quadrilateral  n.  n/2 quadrilaterals are drawn.  Note that the order in which vertices
             * are used to construct a quadrilateral from strip data is different from that used  with
             * independent data.
             */
            QUAD_STRIP,
            /** 
             * Treats each group of four vertices as an  independent  quadrilateral.
             * Vertices  4n-3, 4n-2, 4n-3, and 4n define quadrilateral n.  n/4 quadrilaterals are drawn.
             */
            QUADS,
            /**
             * Draws a connected group of triangles.  One triangle is defined  for  each  vertex
             * presented  after the first two vertices.  Vertices 1, n+1, and n+2 define triangle n.  n-2
             * triangles are drawn.
             */
            TRIANGLE_FAN,
            /** 
             * Draws  a  connected  group  of triangles.  One triangle is defined for each vertex
             * presented after the first two vertices.  For odd n, vertices n, n+1, and n+2 define
             * triangle  n.   For  even  n,  vertices n+1, n, and n+2 define triangle n.  n-2 triangles are
             * drawn.
             */
            TRIANGLE_STRIP,
            /** 
             * Treats each triplet of vertices as an independent triangle.  Vertices 3n-2,  3n-1,  and
             * 3n define triangle n.  n/3 triangles are drawn. 
             */
            TRIANGLES
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
         * @return The float coordinates.
         */
        const std::vector<float>& getFloatXYZ() const { return m_xyz; }
        
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
        
        const VertexType  m_vertexType;
        
        const NormalVectorType m_normalVectorType;
        
        const ColorType  m_colorType;
        
        const PrimitiveType m_primitiveType;

        UsageType m_usageType = UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES;
        
        std::unique_ptr<GraphicsEngineDataOpenGL> m_graphicsEngineDataForOpenGL;
        
        std::vector<float> m_xyz;
        
        std::vector<float> m_floatNormalVectorXYZ;
        
        std::vector<float> m_floatRGBA;
        
        std::vector<uint8_t> m_unsignedByteRGBA;
        
    private:
        void copyHelperGraphicsPrimitive(const GraphicsPrimitive& obj);

        friend class GraphicsEngineDataOpenGL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_H__
