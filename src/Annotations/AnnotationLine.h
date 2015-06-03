#ifndef __ANNOTATION_LINE_H__
#define __ANNOTATION_LINE_H__

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


#include "AnnotationOneDimensionalShape.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationLine : public AnnotationOneDimensionalShape {
        
    public:
        AnnotationLine();
        
        virtual ~AnnotationLine();
        
        AnnotationLine(const AnnotationLine& obj);
        
        AnnotationLine& operator=(const AnnotationLine& obj);

        bool isDisplayStartArrow() const;
        
        void setDisplayStartArrow(const bool displayArrow);
        
        bool isDisplayEndArrow() const;
        
        void setDisplayEndArrow(const bool displayArrow);
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationLine(const AnnotationLine& obj);
        
        void initializeMembersAnnotationLine();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        bool m_displayStartArrow;
        
        bool m_displayEndArrow;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_LINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_LINE_DECLARE__

} // namespace
#endif  //__ANNOTATION_LINE_H__
