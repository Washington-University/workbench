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
#include "Plane.h"
#include "Vector3D.h"

namespace caret {

    class Matrix4x4;
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
        
        bool intersectWithRay(const Vector3D& rayOriginXYZ,
                              const Vector3D& rayDirectionVector,
                              std::vector<Vector3D>& intersectionPointsOut,
                              AString& errorMessageOut) const;

        // ADD_NEW_METHODS_HERE

    private:
        class Face {
        public:
            Face(const Vector3D& v1,
                 const Vector3D& v2,
                 const Vector3D& v3,
                 const Vector3D& v4)
            : m_v1(v1),
            m_v2(v2),
            m_v3(v3),
            m_v4(v4) {
                m_plane = Plane(m_v1, m_v2, m_v3);
            }
            
            Plane m_plane;
            
            Vector3D m_v1;
            
            Vector3D m_v2;
            
            Vector3D m_v3;
            
            Vector3D m_v4;
        };
        
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
        
        void orientIntersectionPoints(const Plane& plane,
                                      const Vector3D& centerOut,
                                      std::vector<Vector3D>& intersectionPointsInOut) const;
        
        void sortIntersectionPoints(const Plane& plane,
                                    Vector3D& centerOut,
                                    std::vector<Vector3D>& intersectionPointsInOut) const;
        
        void createFaces() const;
        
        const VolumeMappableInterface* m_volume;
        
        const Matrix4x4 m_matrix;
        
        std::vector<LineSegment> m_edges;
        
        mutable std::vector<Face> m_faces;
        
        mutable bool m_facesValidFlag = false;
        
        bool m_validFlag = false;
        
        Vector3D m_xyz000;
        Vector3D m_xyzI00;
        Vector3D m_xyzIJ0;
        Vector3D m_xyz0J0;
        
        Vector3D m_xyz00K;
        Vector3D m_xyzI0K;
        Vector3D m_xyzIJK;
        Vector3D m_xyz0JK;

        static bool s_debugFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_PLANE_INTERSECTION_DECLARE__
    bool VolumePlaneIntersection::s_debugFlag = false;
#endif // __VOLUME_PLANE_INTERSECTION_DECLARE__

} // namespace
#endif  //__VOLUME_PLANE_INTERSECTION_H__
