#ifndef __ANNOTATION_IMAGE_H__
#define __ANNOTATION_IMAGE_H__

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


#include "AnnotationTwoDimensionalShape.h"



namespace caret {

    class AnnotationImage : public AnnotationTwoDimensionalShape {
        
    public:
        AnnotationImage();
        
        virtual ~AnnotationImage();
        
        AnnotationImage(const AnnotationImage& obj);
        
        AnnotationImage& operator=(const AnnotationImage& obj);
        
        virtual bool isForegroundLineWidthSupported() const;
        

        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationImage(const AnnotationImage& obj);
        
        void initializeMembersAnnotationImage();
        
        SceneClassAssistant* m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_IMAGE_DECLARE__

} // namespace
#endif  //__ANNOTATION_IMAGE_H__
