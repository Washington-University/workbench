#ifndef __GRAPHICS_FACTORY_H__
#define __GRAPHICS_FACTORY_H__

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
#include "GraphicsPrimitive.h"


namespace caret {
    class GraphicsEngineOpenGL;
    class GraphicsPrimitiveV3fC4f;
    class GraphicsPrimitiveV3fC4ub;

    class GraphicsFactory : public CaretObject, public EventListenerInterface {
        
    public:
        GraphicsFactory();
        
        virtual ~GraphicsFactory();
        
        static void createGraphicsFactory();
        
        static void deleteGraphicsFactory();
        
        static GraphicsFactory* get();
        
        GraphicsEngineOpenGL* getGraphicsEngineOpenGL();
        
        GraphicsPrimitiveV3fC4f* newPrimitiveV3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        GraphicsPrimitiveV3fC4ub* newPrimitiveV3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

    private:
        GraphicsFactory(const GraphicsFactory&);

        GraphicsFactory& operator=(const GraphicsFactory&);
        
        static GraphicsFactory* s_singletonGraphicsFactory;

        std::unique_ptr<GraphicsEngineOpenGL> m_graphicsEngineOpenGL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_FACTORY_DECLARE__
    GraphicsFactory* GraphicsFactory::s_singletonGraphicsFactory = NULL;
#endif // __GRAPHICS_FACTORY_DECLARE__

} // namespace
#endif  //__GRAPHICS_FACTORY_H__
