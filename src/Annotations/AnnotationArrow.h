#ifndef __ANNOTATION_ARROW_H__
#define __ANNOTATION_ARROW_H__

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


#include "AnnotationTwoCoordinateShape.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationArrow : public AnnotationTwoCoordinateShape {
        
    public:
        AnnotationArrow(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationArrow();
        
        AnnotationArrow(const AnnotationArrow& obj);
        
        AnnotationArrow& operator=(const AnnotationArrow& obj);

        bool isDisplayStartArrow() const;
        
        void setDisplayStartArrow(const bool displayArrow);
        
        bool isDisplayEndArrow() const;
        
        void setDisplayEndArrow(const bool displayArrow);
        
        static void setUserDefaultDisplayStartArrow(const bool displayArrow);
        
        static void setUserDefaultDisplayEndArrow(const bool displayArrow);
        
       // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationArrow(const AnnotationArrow& obj);
        
        void initializeMembersAnnotationArrow();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        bool m_displayStartArrow;
        
        bool m_displayEndArrow;
        
        // defaults
        static bool s_userDefaultDisplayStartArrow;
        
        static bool s_userDefaultDisplayEndArrow;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_ARROW_DECLARE__
    bool AnnotationArrow::s_userDefaultDisplayStartArrow = false;
    
    bool AnnotationArrow::s_userDefaultDisplayEndArrow = false;
#endif // __ANNOTATION_ARROW_DECLARE__

} // namespace
#endif  //__ANNOTATION_ARROW_H__
