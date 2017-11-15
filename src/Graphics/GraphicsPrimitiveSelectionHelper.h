#ifndef __GRAPHICS_PRIMITIVE_SELECTION_HELPER_H__
#define __GRAPHICS_PRIMITIVE_SELECTION_HELPER_H__

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
    
    class GraphicsPrimitiveSelectionHelper : public CaretObject {
        
    public:
        GraphicsPrimitiveSelectionHelper(GraphicsPrimitive* parentGraphicsPrimitive);
        
        virtual ~GraphicsPrimitiveSelectionHelper();

        void setupSelectionBeforeDrawing();
        
        const std::vector<uint8_t>& getSelectionEncodedRGBA() const;

        int32_t getPrimitiveIndexFromEncodedRGBA(const uint8_t rgba[4]) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        GraphicsPrimitiveSelectionHelper(const GraphicsPrimitiveSelectionHelper&);

        GraphicsPrimitiveSelectionHelper& operator=(const GraphicsPrimitiveSelectionHelper&);
        
        GraphicsPrimitive* m_parentGraphicsPrimitive;
        
        int32_t m_numberOfVerticesPerPrimitive = 0;

        int32_t m_vertexOffsetForPrimitive = 0;
        
        std::vector<uint8_t> m_selectionEncodedRGBA;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_SELECTION_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_SELECTION_HELPER_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_SELECTION_HELPER_H__
