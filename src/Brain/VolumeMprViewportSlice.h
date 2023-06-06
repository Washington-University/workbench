#ifndef __VOLUME_MPR_VIEWPORT_SLICE_H__
#define __VOLUME_MPR_VIEWPORT_SLICE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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
#include "GraphicsViewport.h"
#include "Plane.h"
#include "Vector3D.h"


namespace caret {

    class EventOpenGLObjectToWindowTransform;
    
    class VolumeMprViewportSlice : public CaretObject {
        
    public:
        VolumeMprViewportSlice(const GraphicsViewport& viewport,
                         const Plane& plane);
        
        virtual ~VolumeMprViewportSlice();
        
        VolumeMprViewportSlice(const VolumeMprViewportSlice& obj);

        VolumeMprViewportSlice& operator=(const VolumeMprViewportSlice& obj);
        
        std::vector<Vector3D> getTriangleStripXYZ() const;
        
        bool containsWindowXY(const int32_t windowX,
                              const int32_t windowY) const;
        
        Vector3D mapWindowXyzToSliceXYZ(const Vector3D& windowXYZ) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperVolumeMprViewportSlice(const VolumeMprViewportSlice& obj);

        Vector3D mapToSlicePlane(const EventOpenGLObjectToWindowTransform& transformEvent,
                                 const Vector3D windowXYZ) const;
        
        GraphicsViewport m_viewport;
        Plane m_plane;
        
        Vector3D m_bottomLeftXYZ;
        Vector3D m_bottomRightXYZ;
        Vector3D m_topRightXYZ;
        Vector3D m_topLeftXYZ;
        
        std::vector<Vector3D> m_triangleStripXYZ;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_MPR_VIEWPORT_SLICE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MPR_VIEWPORT_SLICE_DECLARE__

} // namespace
#endif  //__VOLUME_MPR_VIEWPORT_SLICE_H__
