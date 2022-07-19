#ifndef __GRAPHICS_ENGINE_DATA_OPEN_G_L_H__
#define __GRAPHICS_ENGINE_DATA_OPEN_G_L_H__

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

#include "CaretOpenGLInclude.h"
#include "GraphicsEngineData.h"
#include "GraphicsPrimitive.h"

namespace caret {

    class GraphicsOpenGLBufferObject;
    class GraphicsOpenGLTextureName;
    class GraphicsPrimitive;
    class GraphicsPrimitiveSelectionHelper;
    
    class GraphicsEngineDataOpenGL : public GraphicsEngineData {
        
    public:
        GraphicsEngineDataOpenGL();
        
        virtual ~GraphicsEngineDataOpenGL();

        void deleteBuffers();
        
        static void draw(GraphicsPrimitive* primitive);
        
        static void drawWithSelection(GraphicsPrimitive* primitive,
                                      const int32_t pixelX,
                                      const int32_t pixelY,
                                      int32_t& selectedPrimitiveIndexOut,
                                      float&   selectedPrimitiveDepthOut);
        
        void invalidateCoordinates();
        
        void invalidateColors();
        
        void invalidateTextureCoordinates();
        
        // ADD_NEW_METHODS_HERE

    private:
        enum class PrivateDrawMode {
            DRAW_NORMAL,
            DRAW_SELECTION,
        };
        
        enum SpaceMode {
            MODEL,
            WINDOW,
        };
        
        GraphicsEngineDataOpenGL(const GraphicsEngineDataOpenGL&);

        GraphicsEngineDataOpenGL& operator=(const GraphicsEngineDataOpenGL&);
        
        void loadAllBuffers(GraphicsPrimitive* primitive);
        
        void loadCoordinateBuffer(GraphicsPrimitive* primitive);
        
        void loadNormalVectorBuffer(GraphicsPrimitive* primitive);
        
        void loadColorBuffer(GraphicsPrimitive* primitive);
        
        void loadTextureCoordinateBuffer(GraphicsPrimitive* primitive);
        
        void loadTextureImageDataBuffer(GraphicsPrimitive* primitive);
        
        void loadTextureImageDataBuffer2D(GraphicsPrimitive* primitive);
        
        void loadTextureImageDataBuffer3D(GraphicsPrimitive* primitive);
        
        static void drawPrivate(const PrivateDrawMode drawMode,
                                GraphicsPrimitive* primitive,
                                GraphicsPrimitiveSelectionHelper* primitiveSelectionHelper);
        
        static void drawModelOrWindowSpace(const SpaceMode spaceMode,
                                           const PrivateDrawMode drawMode,
                                           GraphicsPrimitive* primitive,
                                           GraphicsPrimitiveSelectionHelper* primitiveSelectionHelper);
        
        static void drawPointsPrimitiveMillimeters(const GraphicsPrimitive* primitive);
        
        static void drawSpheresPrimitive(const GraphicsPrimitive* primitive);
        
        GLenum getOpenGLBufferUsageHint(const GraphicsPrimitive::UsageType primitiveUsageType) const;
        
        static float getLineWidthForDrawingInPixels(const GraphicsPrimitive* primitive);
        
        static float getPointDiameterForDrawingInPixels(const GraphicsPrimitive* primitive);
        
        static void saveOpenGLStateForWindowSpaceDrawing(int32_t polygonMode[2],
                                                  int32_t viewport[4]);
        
        static void restoreOpenGLStateForWindowSpaceDrawing(int32_t polygonMode[2],
                                                     int32_t viewport[4]);
        
        GLsizei m_arrayIndicesCount = 0;
        
        std::unique_ptr<GraphicsOpenGLBufferObject> m_coordinateBufferObject;
        
        GLenum m_coordinateDataType = GL_FLOAT;

        GLint m_coordinatesPerVertex = 0;

        bool m_reloadCoordinatesFlag = false;
        
        bool m_reloadColorsFlag = false;
        
        bool m_reloadTextureCoordinatesFlag = false;
        
        std::unique_ptr<GraphicsOpenGLBufferObject> m_normalVectorBufferObject;
        
        GLenum m_normalVectorDataType = GL_FLOAT;
        
        std::unique_ptr<GraphicsOpenGLBufferObject> m_colorBufferObject;
        
        GLenum m_colorDataType = GL_FLOAT;
        
        GLint m_componentsPerColor = 0;
        
        std::unique_ptr<GraphicsOpenGLBufferObject> m_textureCoordinatesBufferObject;
        
        GLenum m_textureCoordinatesDataType = GL_FLOAT;
        
        GraphicsOpenGLTextureName* m_textureImageDataName = NULL;
        
// ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__

} // namespace
#endif  //__GRAPHICS_ENGINE_DATA_OPEN_G_L_H__
