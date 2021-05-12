#ifndef __GRAPHICS_REGION_SELECTION_BOX_H__
#define __GRAPHICS_REGION_SELECTION_BOX_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

    class GraphicsRegionSelectionBox : public CaretObject {
        
    public:
        enum class Status {
            INVALID,
            VALID
        };
        
        GraphicsRegionSelectionBox();
        
        virtual ~GraphicsRegionSelectionBox();
        
        GraphicsRegionSelectionBox(const GraphicsRegionSelectionBox& obj);

        GraphicsRegionSelectionBox& operator=(const GraphicsRegionSelectionBox& obj);

        Status getStatus() const;
        
        void setStatus(const Status status);
        
        void getBounds(float& minX,
                       float& minY,
                       float& maxX,
                       float& maxY) const;
        
        void initialize(const float x,
                        const float y);
        
        void update(const float x,
                    const float y);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperGraphicsRegionSelectionBox(const GraphicsRegionSelectionBox& obj);

        Status m_status = Status::INVALID;
        
        float m_x1 = 0.0;
        
        float m_y1 = 0.0;
        
        float m_x2 = 0.0;
        
        float m_y2 = 0.0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_REGION_SELECTION_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_REGION_SELECTION_BOX_DECLARE__

} // namespace
#endif  //__GRAPHICS_REGION_SELECTION_BOX_H__
