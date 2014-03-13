#ifndef __EVENT_MODEL_SURFACE_GET_H__
#define __EVENT_MODEL_SURFACE_GET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <vector>

#include "Event.h"

namespace caret {

    class ModelSurface;
    class Surface;
    
    /// Find the Surface model that contains a specific Surface.
    class EventModelSurfaceGet : public Event {
        
    public:
        EventModelSurfaceGet(const Surface* surface);
        
        virtual ~EventModelSurfaceGet();
        
        ModelSurface* getModelSurface();
        
        void setModelSurface(ModelSurface* modelSurface);
        
        const Surface* getSurface() const;
        
    private:
        EventModelSurfaceGet(const EventModelSurfaceGet&);
        
        EventModelSurfaceGet& operator=(const EventModelSurfaceGet&);
        
        ModelSurface* m_modelSurface;
        
        const Surface* surface;
    };

} // namespace

#endif // __EVENT_MODEL_SURFACE_GET_H__
