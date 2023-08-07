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
        AnnotationPolyhedron(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationPolyhedron();
        
        AnnotationPolyhedron(const AnnotationPolyhedron& obj);
        
        AnnotationPolyhedron& operator=(const AnnotationPolyhedron& obj);

        virtual AnnotationPolyhedron* castToPolyhedron() override;
        
        virtual const AnnotationPolyhedron* castToPolyhedron() const override;
        
        bool finishNewPolyhedron(const Plane& plane,
                                 const float polyhedronDepth,
                                 AString& errorMessageOut);
        
        Plane getPlane() const;
        
        void setPlane(const Plane& plane);
        
       // ADD_NEW_METHODS_HERE
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationPolyhedron(const AnnotationPolyhedron& obj);
        
        void initializeMembersAnnotationPolyhedron();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        Plane m_plane;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_POLYHEDRON_DECLARE__
#endif // __ANNOTATION_POLYHEDRON_DECLARE__

} // namespace
#endif  //__ANNOTATION_POLYHEDRON_H__
