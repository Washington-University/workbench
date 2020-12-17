#ifndef __ANNOTATION_MULTI_COORDINATE_SHAPE_H__
#define __ANNOTATION_MULTI_COORDINATE_SHAPE_H__

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

#include <memory>

#include "Annotation.h"

namespace caret {

    class AnnotationCoordinate;

    class AnnotationMultiCoordinateShape : public Annotation {
        
    public:
        AnnotationMultiCoordinateShape(const AnnotationTypeEnum::Enum type,
                                      const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationMultiCoordinateShape();
        
        AnnotationMultiCoordinateShape(const AnnotationMultiCoordinateShape& obj);

        AnnotationMultiCoordinateShape& operator=(const AnnotationMultiCoordinateShape& obj);
        
        void addCoordinate(AnnotationCoordinate* coord);
        
        virtual AnnotationMultiCoordinateShape* castToMultiCoordinateShape() override;
        
        virtual const AnnotationMultiCoordinateShape* castToMultiCoordinateShape() const override;
        
        int32_t getNumberOfCoordinates() const;
        
        AnnotationCoordinate* getCoordinate(const int32_t index);
        
        const AnnotationCoordinate* getCoordinate(const int32_t index) const;
        
        virtual AnnotationSurfaceOffsetVectorTypeEnum::Enum getSurfaceOffsetVectorType() const override;
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        virtual bool isSizeHandleValid(const AnnotationSizingHandleTypeEnum::Enum sizingHandle) const;
        
        virtual bool applySpatialModification(const AnnotationSpatialModification& spatialModification);
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation);
                
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationMultiCoordinateShape(const AnnotationMultiCoordinateShape& obj);

        void initializeMembersAnnotationMultiCoordinateShape();
        
        bool applySpatialModificationChartSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationSurfaceSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationStereotaxicSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationTabOrWindowSpace(const AnnotationSpatialModification& spatialModification);
        
        bool applySpatialModificationSpacerTabSpace(const AnnotationSpatialModification& spatialModification);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        std::vector<std::unique_ptr<AnnotationCoordinate>> m_coordinates;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_MULTI_COORDINATE_SHAPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_MULTI_COORDINATE_SHAPE_DECLARE__

} // namespace
#endif  //__ANNOTATION_MULTI_COORDINATE_SHAPE_H__
