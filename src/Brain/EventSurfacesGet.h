#ifndef __EVENT_SURFACES_GET_H__
#define __EVENT_SURFACES_GET_H__

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

#include "Event.h"

#include "StructureEnum.h"
#include "SurfaceTypeEnum.h"

namespace caret {

    class Surface;
    
    /// Get surfaces
    class EventSurfacesGet : public Event {
        
    public:
        EventSurfacesGet();
        
        virtual ~EventSurfacesGet();
        
        void addSurface(Surface* surface);
        
        void addStructureConstraint(const StructureEnum::Enum structure);
        
        void addSurfaceTypeConstraint(const SurfaceTypeEnum::Enum surfaceType);
        
        std::vector<Surface*> getSurfaces() const;        
        
    private:
        EventSurfacesGet(const EventSurfacesGet&);
        
        EventSurfacesGet& operator=(const EventSurfacesGet&);
        
        std::vector<Surface*> surfaces;
        
        std::vector<StructureEnum::Enum> structureConstraints;
        
        std::vector<SurfaceTypeEnum::Enum> surfaceTypeConstraints;
    };

} // namespace

#endif // __EVENT_SURFACES_GET_H__
