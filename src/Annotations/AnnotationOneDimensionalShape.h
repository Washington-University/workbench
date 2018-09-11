#ifndef __ANNOTATION_ONE_DIMENSIONAL_SHAPE_H__
#define __ANNOTATION_ONE_DIMENSIONAL_SHAPE_H__

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


#include "Annotation.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationCoordinate;

    class AnnotationOneDimensionalShape : public Annotation {
        
    public:
        AnnotationOneDimensionalShape(const AnnotationTypeEnum::Enum type,
                                      const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationOneDimensionalShape();
        
        AnnotationOneDimensionalShape(const AnnotationOneDimensionalShape& obj);

        AnnotationOneDimensionalShape& operator=(const AnnotationOneDimensionalShape& obj);
        
        virtual AnnotationOneDimensionalShape* castToOneDimensionalShape() override;
        
        virtual const AnnotationOneDimensionalShape* castToOneDimensionalShape() const override;
        
        virtual AnnotationTwoDimensionalShape* castToTwoDimensionalShape() override;
        
        virtual const AnnotationTwoDimensionalShape* castToTwoDimensionalShape() const override;
        
        AnnotationCoordinate* getStartCoordinate();
        
        const AnnotationCoordinate* getStartCoordinate() const;
        
        AnnotationCoordinate* getEndCoordinate();
        
        const AnnotationCoordinate* getEndCoordinate() const;
        
        virtual AnnotationSurfaceOffsetVectorTypeEnum::Enum getSurfaceOffsetVectorType() const override;
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        virtual bool isSizeHandleValid(const AnnotationSizingHandleTypeEnum::Enum sizingHandle) const;
        
        virtual bool applySpatialModification(const AnnotationSpatialModification& spatialModification);
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation);
        
        float getRotationAngle(const float viewportWidth,
                               const float viewportHeight) const;
        
        void setRotationAngle(const float viewportWidth,
                              const float viewportHeight,
                              const float rotationAngle);
        
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationOneDimensionalShape(const AnnotationOneDimensionalShape& obj);

        void initializeMembersAnnotationOneDimensionalShape();
        
        bool applySpatialModificationChartSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationSurfaceSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationStereotaxicSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationTabOrWindowSpace(const AnnotationSpatialModification& spatialModification);
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;
        
        CaretPointer<AnnotationCoordinate> m_startCoordinate;
        
        CaretPointer<AnnotationCoordinate> m_endCoordinate;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_ONE_DIMENSIONAL_SHAPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_ONE_DIMENSIONAL_SHAPE_DECLARE__

} // namespace
#endif  //__ANNOTATION_ONE_DIMENSIONAL_SHAPE_H__
