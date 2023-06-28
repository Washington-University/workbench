#ifndef __VOLUME_VERTICES_EDGES_FACES_H__
#define __VOLUME_VERTICES_EDGES_FACES_H__

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

    class VolumeMappableInterface;
    
    class VolumeVerticesEdgesFaces : public CaretObject {
        
    public:
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
            
            const Plane& plane() const { return m_plane; }
            
            const Vector3D& v1() const { return m_v1; }
            
            const Vector3D& v2() const { return m_v2; }
            
            const Vector3D& v3() const { return m_v3; }
            
            const Vector3D& v4() const { return m_v4; }
            
        private:
            Plane m_plane;
            
            Vector3D m_v1;
            
            Vector3D m_v2;
            
            Vector3D m_v3;
            
            Vector3D m_v4;
        };
        
        class Edge {
        public:
            Edge(const Vector3D& v1,
                 const Vector3D& v2)
            : m_v1(v1),
            m_v2(v2) {  }
            
            const Vector3D& v1() const { return m_v1; }
            
            const Vector3D& v2() const { return m_v2; }
            
        private:
            Vector3D m_v1;
            
            Vector3D m_v2;
        };
        
        static std::unique_ptr<VolumeVerticesEdgesFaces> newInstance(const VolumeMappableInterface* volume);
        
        static std::unique_ptr<VolumeVerticesEdgesFaces> newInstance(const VolumeMappableInterface* volume,
                                                                     const Matrix4x4& matrix);
        
        virtual ~VolumeVerticesEdgesFaces();
        
        VolumeVerticesEdgesFaces(const VolumeVerticesEdgesFaces& obj) = delete;

        VolumeVerticesEdgesFaces& operator=(const VolumeVerticesEdgesFaces& obj) = delete;
        
        bool isValid() const;

        const std::vector<Vector3D>& getVertices() const;
        
        const std::vector<Edge>& getEdges() const;
        
        const std::vector<Face>& getFaces() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        VolumeVerticesEdgesFaces(const VolumeMappableInterface* volume,
                                 const Matrix4x4& matrix,
                                 const bool matrixValidFlag);

        void createFaces() const;
        
        // ADD_NEW_MEMBERS_HERE

        const VolumeMappableInterface* m_volume;
        
        const Matrix4x4 m_matrix;
        
        const bool m_matrixValidFlag;
        
        bool m_validFlag = false;
        
        Vector3D m_xyz000;
        Vector3D m_xyzI00;
        Vector3D m_xyzIJ0;
        Vector3D m_xyz0J0;
        
        Vector3D m_xyz00K;
        Vector3D m_xyzI0K;
        Vector3D m_xyzIJK;
        Vector3D m_xyz0JK;

        std::vector<Vector3D> m_vertices;
        
        std::vector<Edge> m_edges;
        
        mutable std::vector<Face> m_faces;

        mutable bool m_facesValidFlag = false;
      
        static constexpr bool s_debugFlag = false;
    };
    
#ifdef __VOLUME_VERTICES_EDGES_FACES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_VERTICES_EDGES_FACES_DECLARE__

} // namespace
#endif  //__VOLUME_VERTICES_EDGES_FACES_H__
