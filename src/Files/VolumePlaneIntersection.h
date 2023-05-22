#ifndef __VOLUME_PLANE_INTERSECTION_H__
#define __VOLUME_PLANE_INTERSECTION_H__

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
#include "Matrix4x4.h"
#include "Vector3D.h"

namespace caret {

    class Matrix4x4;
    class Plane;
    class VolumeMappableInterface;
    
    class VolumePlaneIntersection : public CaretObject {
        
    public:
        VolumePlaneIntersection(const VolumeMappableInterface* volume);
        
        VolumePlaneIntersection(const VolumeMappableInterface* volume,
                                const Matrix4x4& matrix);
        
        virtual ~VolumePlaneIntersection();
        
        VolumePlaneIntersection(const VolumePlaneIntersection&) = delete;

        VolumePlaneIntersection& operator=(const VolumePlaneIntersection&) = delete;

        bool intersectWithPlane(const Plane& plane,
                                Vector3D& centerOut,
                                std::vector<Vector3D>& intersectionPointsOut,
                                AString& errorMessageOut) const;
        
        void orientIntersectionPoints(const Plane& plane,
                                      const Vector3D& centerOut,
                                      std::vector<Vector3D>& intersectionPointsInOut) const;
        
        void sortIntersectionPoints(const Plane& plane,
                                    Vector3D& centerOut,
                                    std::vector<Vector3D>& intersectionPointsInOut) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        class LineSegment {
        public:
            LineSegment(const Vector3D& v1,
                        const Vector3D& v2)
            : m_v1(v1),
              m_v2(v2) {  }
             
            const Vector3D& v1() const { return m_v1; }
            
            const Vector3D& v2() const { return m_v2; }
            
        private:
            Vector3D m_v1;
            
            Vector3D m_v2;
        };
        
        const VolumeMappableInterface* m_volume;
        
        const Matrix4x4 m_matrix;
        
        std::vector<LineSegment> m_edges;
        
        AString m_constructorErrorMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_PLANE_INTERSECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_PLANE_INTERSECTION_DECLARE__

} // namespace
#endif  //__VOLUME_PLANE_INTERSECTION_H__
