#ifndef __EVENT_SURFACE_FILE_GET_H__
#define __EVENT_SURFACE_FILE_GET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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


#include <cstdint>
#include <memory>

#include "Event.h"
#include "StructureEnum.h"

namespace caret {

    class SurfaceFile;
    
    class EventSurfaceFileGet : public Event {
        
    public:
        EventSurfaceFileGet(const StructureEnum::Enum structure,
                            const int32_t numberOfVertices);
        
        virtual ~EventSurfaceFileGet();
        
        EventSurfaceFileGet(const EventSurfaceFileGet&) = delete;

        EventSurfaceFileGet& operator=(const EventSurfaceFileGet&) = delete;
        
        void addSurfaceFile(const SurfaceFile* surfaceFile);
        
        StructureEnum::Enum getStructure() const;
        
        int32_t getNumberOfVertices() const;

        int32_t getNumberOfSurfaceFiles() const;
        
        const SurfaceFile* getSurfaceFile(const int32_t index) const;
        
        const SurfaceFile* getMidthicknessAnatomicalSurface() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        const StructureEnum::Enum m_structure;
        
        const int32_t m_numberOfVertices;
        
        std::vector<const SurfaceFile*> m_surfaceFiles;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_SURFACE_FILE_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_SURFACE_FILE_GET_DECLARE__

} // namespace
#endif  //__EVENT_SURFACE_FILE_GET_H__
