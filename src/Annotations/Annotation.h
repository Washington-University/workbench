#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

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


#include "AnnotationAlignHorizontalEnum.h"
#include "AnnotationAlignVerticalEnum.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationDrawingTypeEnum.h"
#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class Annotation : public CaretObject, public SceneableInterface {
        
    public:
        Annotation(const AnnotationDrawingTypeEnum::Enum drawingType);
        
        virtual ~Annotation();
        
        Annotation(const Annotation& obj);

        Annotation& operator=(const Annotation& obj);
        
        AnnotationDrawingTypeEnum::Enum getDrawingType() const;
        
        AnnotationAlignHorizontalEnum::Enum getHorizontalAlignment() const;
        
        void setHorizontalAlignment(const AnnotationAlignHorizontalEnum::Enum alignment);
        
        AnnotationAlignVerticalEnum::Enum getVerticalAlignment() const;
        
        void setVerticalAlignment(const AnnotationAlignVerticalEnum::Enum alignment);
        
        AnnotationCoordinateSpaceEnum::Enum getCoordinateSpace() const;

        void setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace);
        
        const float* getXYZ() const;
        
        void getXYZ(float xyzOut[3]) const;
        
        void setXYZ(const float xyz[3]);
        
        void setXYZ(const float x,
                    const float y,
                    const float z);
        
        int32_t getTabIndex() const;
        
        void setTabIndex(const int32_t tabIndex);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperAnnotation(const Annotation& obj);

        const AnnotationDrawingTypeEnum::Enum m_drawingType;
        
        SceneClassAssistant* m_sceneAssistant;

        AnnotationAlignHorizontalEnum::Enum  m_alignmentHorizontal;
        
        AnnotationAlignVerticalEnum::Enum  m_alignmentVertical;
        
        AnnotationCoordinateSpaceEnum::Enum  m_coordinateSpace;
        
        float m_xyz[3];
        
        int32_t m_tabIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_H__
