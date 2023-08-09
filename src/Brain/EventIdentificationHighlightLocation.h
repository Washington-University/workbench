#ifndef __EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION_H__
#define __EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION_H__

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
#include "Vector3D.h"

namespace caret {

    /// Highlight location of an indentification
    class EventIdentificationHighlightLocation : public Event {
        
    public:
        enum LOAD_FIBER_ORIENTATION_SAMPLES_MODE {
            LOAD_FIBER_ORIENTATION_SAMPLES_MODE_YES,
            LOAD_FIBER_ORIENTATION_SAMPLES_MODE_NO
        };
        
        EventIdentificationHighlightLocation(const int32_t tabIndex,
                                             const Vector3D& stereotaxicXYZ,
                                             const Vector3D& voxelCenterXYZ,
                                             const LOAD_FIBER_ORIENTATION_SAMPLES_MODE loadFiberOrientationSamplesMode);
        
        virtual ~EventIdentificationHighlightLocation();
        
        const Vector3D getStereotaxicXYZ() const;
        
        const Vector3D getVoxelCenterXYZ() const;

        bool isTabSelected(const int32_t tabIndex) const;
        
        LOAD_FIBER_ORIENTATION_SAMPLES_MODE getLoadFiberOrientationSamplesMode() const;
        
    private:
        EventIdentificationHighlightLocation(const EventIdentificationHighlightLocation&);
        
        EventIdentificationHighlightLocation& operator=(const EventIdentificationHighlightLocation&);
        
        const int32_t m_tabIndex;
        
        Vector3D m_stereotaxicXYZ;

        Vector3D m_voxelCenterXYZ;
        
        const LOAD_FIBER_ORIENTATION_SAMPLES_MODE m_loadFiberOrientationSamplesMode;
        
    };

} // namespace

#endif // __EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION_H__
