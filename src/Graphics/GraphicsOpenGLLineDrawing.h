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



namespace caret {

    class GraphicsPrimitive;
    
    class GraphicsOpenGLLineDrawing : public CaretObject {
        
    public:
        GraphicsOpenGLLineDrawing(const GraphicsPrimitive* primitive,
                                  const float lineThicknessPixels);
        
        virtual ~GraphicsOpenGLLineDrawing();
        
        bool run(AString& errorMessageOut);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        GraphicsOpenGLLineDrawing(const GraphicsOpenGLLineDrawing& obj);
        
        GraphicsOpenGLLineDrawing& operator=(const GraphicsOpenGLLineDrawing& obj);
        
        void convertPointsToWindowCoordinates();
        
        void convertLineSegmentsToQuads();
        
        void drawQuads();
        
        const GraphicsPrimitive* m_inputPrimitive;
        
        const float m_lineThicknessPixels;
        
        GraphicsPrimitive* m_outputPrimitive = NULL;
        
        std::vector<float> m_windowLineSegmentsXYZ;
        
        std::vector<float> m_windowQuadsXYZ;
        
        bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_OPEN_G_L_LINE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OPEN_G_L_LINE_DRAWING_DECLARE__

} // namespace
#endif  //__GRAPHICS_OPEN_G_L_LINE_DRAWING_H__
