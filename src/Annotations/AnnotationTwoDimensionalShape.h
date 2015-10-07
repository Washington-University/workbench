#ifndef __ANNOTATION_TWO_DIMENSIONAL_SHAPE_H__
#define __ANNOTATION_TWO_DIMENSIONAL_SHAPE_H__

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
    
    class AnnotationTwoDimensionalShape : public Annotation {
        
    public:
        AnnotationTwoDimensionalShape(const AnnotationTypeEnum::Enum type,
                                      const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationTwoDimensionalShape();
        
        AnnotationTwoDimensionalShape(const AnnotationTwoDimensionalShape& obj);

        AnnotationTwoDimensionalShape& operator=(const AnnotationTwoDimensionalShape& obj);
        
        AnnotationCoordinate* getCoordinate();
        
        const AnnotationCoordinate* getCoordinate() const;
        
        float getHeight() const;
        
        void setHeight(const float height);
        
        float getWidth() const;
        
        void setWidth(const float width);
        
        float getRotationAngle() const;
        
        void setRotationAngle(const float rotationAngle);
        
        virtual bool isModified() const;

        virtual void clearModified();
        
        virtual void applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                              const float viewportWidth,
                                              const float viewportHeight,
                                              const float mouseX,
                                              const float mouseY,
                                              const float mouseDX,
                                              const float mouseDY);
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation);
        
        virtual bool getShapeBounds(const float viewportWidth,
                                    const float viewportHeight,
                                    const float viewportXYZ[3],
                                    float bottomLeftOut[3],
                                    float bottomRightOut[3],
                                    float topRightOut[3],
                                    float topLeftOut[3]) const;
        
        void getSideHandleMouseDelta(const AnnotationSizingHandleTypeEnum::Enum sizeHandle,
                                     const float leftToRightShapeVector[3],
                                     const float bottomToTopShapeVector[3],
                                     const float mouseDX,
                                     const float mouseDY,
                                     float& shapeDxOut,
                                     float& shapeDyOut);
        
        void setWidthAndHeightFromBounds(const float xyzOne[3],
                                         const float xyzTwo[3],
                                         const float spaceWidth,
                                         const float spaceHeight);
        
        static void setUserDefaultHeight(const float height);
        
        static void setUserDefaultWidth(const float width);
        
        static void setUserDefaultRotationAngle(const float rotationAngle);
        
       // ADD_NEW_METHODS_HERE

        
        
        
        
        
    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationTwoDimensionalShape(const AnnotationTwoDimensionalShape& obj);

        void initializeMembersAnnotationTwoDimensionalShape();
        
        void addToXYZWithXY(float xyz[3],
                            const float addX,
                            const float addY);
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        CaretPointer<AnnotationCoordinate> m_coordinate;
        
        float m_rotationAngle;
        
        float m_width;
        
        float m_height;
        
        static float s_userDefaultRotationAngle;
        
        static float s_userDefaultWidth;
        
        static float s_userDefaultHeight;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TWO_DIMENSIONAL_SHAPE_DECLARE__
    float AnnotationTwoDimensionalShape::s_userDefaultRotationAngle = 0.0;
    
    float AnnotationTwoDimensionalShape::s_userDefaultWidth = 25.0;
    
    float AnnotationTwoDimensionalShape::s_userDefaultHeight = 25.0;
#endif // __ANNOTATION_TWO_DIMENSIONAL_SHAPE_DECLARE__

} // namespace
#endif  //__ANNOTATION_TWO_DIMENSIONAL_SHAPE_H__
