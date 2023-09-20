#ifndef __ANNOTATION_POLYHEDRON_H__
#define __ANNOTATION_POLYHEDRON_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "AnnotationMultiPairedCoordinateShape.h"
#include "CaretPointer.h"
#include "Plane.h"

namespace caret {

    class Plane;
    
    class AnnotationPolyhedron : public AnnotationMultiPairedCoordinateShape {
        
    public:
        class Edge {
        public:
            Edge(const Vector3D& v1,
                 const Vector3D& v2)
            : m_v1(v1), m_v2(v2) { }
            
            const Vector3D m_v1;
            
            const Vector3D m_v2;
        };
        
        class Triangle {
        public:
            Triangle(const Vector3D& v1,
                     const Vector3D& v2,
                     const Vector3D& v3)
            : m_v1(v1), m_v2(v2), m_v3(v3) { }
            
            const Vector3D m_v1;
            
            const Vector3D m_v2;

            const Vector3D m_v3;
        };
        
        AnnotationPolyhedron(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationPolyhedron();
        
        AnnotationPolyhedron(const AnnotationPolyhedron& obj);
        
        AnnotationPolyhedron& operator=(const AnnotationPolyhedron& obj);

        virtual AnnotationPolyhedron* castToPolyhedron() override;
        
        virtual const AnnotationPolyhedron* castToPolyhedron() const override;
        
        bool finishNewPolyhedron(const Plane& plane,
                                 const float polyhedronDepth,
                                 AString& errorMessageOut);
        
        virtual void updateCoordinatesAfterDepthChanged() override;
        
        Plane getPlane() const;
        
        float getDepthMillimeters() const;
        
        float getDepthSlices(const float sliceThickness) const;
        
        void setPlane(const Plane& plane);
        
        void setDepthMillimeters(const float depth);
        
        void setDepthSlices(const float sliceThickness,
                            const int32_t numberOfSlices);
        
        void getEdgesAndTriangles(std::vector<Edge>& edgesOut,
                                  std::vector<Triangle>& trianglesOut) const;
        
        static float slicesToMillimeters(const float sliceThickness,
                                         const float numberOfSlices);
        
        static float millimetersToSlices(const float sliceThickness,
                                         const float millimeters);
        
       // ADD_NEW_METHODS_HERE
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void setFromFileReading(const Plane& plane,
                                const float depth);
        
        void copyHelperAnnotationPolyhedron(const AnnotationPolyhedron& obj);
        
        void initializeMembersAnnotationPolyhedron();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        Plane m_plane;
        
        float m_depthMillimeters;
        
        // ADD_NEW_MEMBERS_HERE

        /* Needs to call setPlane() and setDepth() */
        friend class AnnotationFileXmlReader;
    };
    
#ifdef __ANNOTATION_POLYHEDRON_DECLARE__
#endif // __ANNOTATION_POLYHEDRON_DECLARE__

} // namespace
#endif  //__ANNOTATION_POLYHEDRON_H__
