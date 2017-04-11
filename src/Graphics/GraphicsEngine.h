#ifndef __GRAPHICS_ENGINE_H__
#define __GRAPHICS_ENGINE_H__

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

    class GraphicsPrimitive;
    
    class GraphicsEngine : public CaretObject, public EventListenerInterface {
        
    public:
        GraphicsEngine();
        
        virtual ~GraphicsEngine();
        
        /**
         * Draw the given graphics primitive.
         *
         * @param primitive
         *     Primitive that is drawn.
         */
        virtual void draw(GraphicsPrimitive* primitive) = 0;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

    private:
        GraphicsEngine(const GraphicsEngine&);

        GraphicsEngine& operator=(const GraphicsEngine&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_ENGINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_ENGINE_DECLARE__

} // namespace
#endif  //__GRAPHICS_ENGINE_H__
