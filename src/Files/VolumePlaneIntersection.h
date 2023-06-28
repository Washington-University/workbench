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
    class VolumeVerticesEdgesFaces;
    
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
        
        VolumePlaneIntersection(const VolumeMappableInterface* volume,
                                const Matrix4x4& matrix,
                                const bool matrixValidFlag);
        

        void orientIntersectionPoints(const Plane& plane,
                                      const Vector3D& centerOut,
                                      std::vector<Vector3D>& intersectionPointsInOut) const;
        
        void sortIntersectionPoints(const Plane& plane,
                                    Vector3D& centerOut,
                                    std::vector<Vector3D>& intersectionPointsInOut) const;
        
        void mapPointsBackToVolume(std::vector<Vector3D>& intersectionPointsInOut) const;
        
        const VolumeMappableInterface* m_volume;
        
        const Matrix4x4 m_matrix;
        
        const bool m_matrixValidFlag;
        
        std::unique_ptr<VolumeVerticesEdgesFaces> m_verticesEdgesFaces;

        bool m_validFlag = false;
        
        static bool s_debugFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_PLANE_INTERSECTION_DECLARE__
    bool VolumePlaneIntersection::s_debugFlag = false;
#endif // __VOLUME_PLANE_INTERSECTION_DECLARE__

} // namespace
#endif  //__VOLUME_PLANE_INTERSECTION_H__
