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



#include <memory>

#include "CaretObject.h"

#include "EventListenerInterface.h"


namespace caret {

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
        
        enum class PrimitiveType {
            /** Quads - Four vertices form one quad */
            QUADS,
            /** Triangles - Three vertices form one triangle */
            TRIANGLES
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
        virtual ~GraphicsPrimitive();
        
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
        
        virtual AString toString() const;
        
        /**
         * Clone this primitive.
         */
        virtual GraphicsPrimitive* clone() const = 0;
        
    protected:
        const VertexType  m_vertexType;
        
        const NormalVectorType m_normalVectorType;
        
        const ColorType  m_colorType;
        
        const PrimitiveType m_primitiveType;
        
        std::vector<float> m_xyz;
        
        std::vector<float> m_floatNormalVectorXYZ;
        
        std::vector<float> m_floatRGBA;
        
        std::vector<uint8_t> m_unsignedByteRGBA;
        
    private:
        void copyHelperGraphicsPrimitive(const GraphicsPrimitive& obj);

        friend class GraphicsEngineOpenGL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_H__
