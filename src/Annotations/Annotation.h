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


#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationShapeDimensionEnum.h"
#include "AnnotationTypeEnum.h"
#include "CaretObjectTracksModification.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"


namespace caret {
    class SceneClassAssistant;

    class Annotation : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        Annotation(const AnnotationTypeEnum::Enum type);
        
        virtual ~Annotation();
        
        Annotation(const Annotation& obj);

        Annotation& operator=(const Annotation& obj);
        
        AnnotationTypeEnum::Enum getType() const ;
        
        AnnotationCoordinateSpaceEnum::Enum getCoordinateSpace() const;

        void setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace);
        
        const float* getXYZ() const;
        
        void getXYZ(float xyzOut[3]) const;
        
        void setXYZ(const float xyz[3]);
        
        void setXYZ(const double xyz[3]);
        
        void setXYZ(const float x,
                    const float y,
                    const float z);
        
        float getHeight2D() const;
        
        void setHeight2D(const float height);
        
        float getWidth2D() const;
        
        void setWidth2D(const float width);
        
        float getLength1D() const;
        
        void setLength1D(const float length1D);
        
        float getRotationAngle() const;
        
        void setRotationAngle(const float rotationAngle);
        
        AnnotationShapeDimensionEnum::Enum getShapeDimension() const;
        
        void getSurfaceSpace(StructureEnum::Enum& structureOut,
                             int32_t& surfaceNumberOfNodesOut,
                             int32_t& surfaceNodeIndexOut) const;
        
        void setSurfaceSpace(const StructureEnum::Enum structure,
                            const int32_t surfaceNumberOfNodes,
                            const int32_t surfaceNodeIndex);
        
        int32_t getTabIndex() const;
        
        void setTabIndex(const int32_t tabIndex);
        
        void getForegroundColor(float rgbaOut[4]) const;
        
        void getForegroundColor(uint8_t rgbaOut[4]) const;
        
        const float* getForegroundColor() const;
        
        void setForegroundColor(const float rgba[4]);
        
        void setForegroundColor(const uint8_t rgba[4]);
        
        void getBackgroundColor(float rgbaOut[4]) const;
        
        void getBackgroundColor(uint8_t rgbaOut[4]) const;
        
        const float* getBackgroundColor() const;
        
        void setBackgroundColor(const float rgba[4]);
        
        void setBackgroundColor(const uint8_t rgba[4]);
        
        bool isSelected() const;
        
        void setSelected(const bool selectedStatus) const;
        
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

        void initializeAnnotationMembers();
        
        SceneClassAssistant* m_sceneAssistant;
        
        const AnnotationTypeEnum::Enum m_type;
        
        AnnotationShapeDimensionEnum::Enum m_shapeDimension;
        
        AnnotationCoordinateSpaceEnum::Enum  m_coordinateSpace;
        
        float m_xyz[3];
        
        float m_width2D;
        
        float m_height2D;
        
        float m_length1D;
        
        float m_rotationAngle;
        
        int32_t m_surfaceSpaceNodeIndex;
        
        int32_t m_surfaceSpaceNumberOfNodes;
        
        StructureEnum::Enum m_surfaceSpaceStructure;
        
        int32_t m_tabIndex;
        
        float m_colorForeground[4];
        
        float m_colorBackground[4];
        
        mutable bool m_selectedFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_H__
