#ifndef __ANNOTATION_POLY_LINE_H__
#define __ANNOTATION_POLY_LINE_H__

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


#include "AnnotationMultiCoordinateShape.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationPolyLine : public AnnotationMultiCoordinateShape {
        
    public:
        AnnotationPolyLine(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationPolyLine();
        
        AnnotationPolyLine(const AnnotationPolyLine& obj);
        
        AnnotationPolyLine& operator=(const AnnotationPolyLine& obj);

       // ADD_NEW_METHODS_HERE
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationPolyLine(const AnnotationPolyLine& obj);
        
        void initializeMembersAnnotationPolyLine();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_POLY_LINE_DECLARE__
#endif // __ANNOTATION_POLY_LINE_DECLARE__

} // namespace
#endif  //__ANNOTATION_POLY_LINE_H__
