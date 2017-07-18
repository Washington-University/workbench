#ifndef __ANNOTATION_COORDINATE_INFORMATION_H__
#define __ANNOTATION_COORDINATE_INFORMATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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
#include "AnnotationSurfaceOffsetVectorTypeEnum.h"
#include "StructureEnum.h"

class QLabel;

namespace caret {

    class Annotation;
    class AnnotationOneDimensionalShape;
    class AnnotationTwoDimensionalShape;
    class BrainOpenGLWidget;
    class BrainOpenGLViewportContent;
    class MouseEvent;
    
    class AnnotationCoordinateInformation {
        
    public:
        AnnotationCoordinateInformation();
        
        virtual ~AnnotationCoordinateInformation();
        
        bool isCoordinateSpaceValid(const AnnotationCoordinateSpaceEnum::Enum space) const;
        
        void reset();
        
        static void getValidCoordinateSpaces(const AnnotationCoordinateInformation* coordInfoOne,
                                             const AnnotationCoordinateInformation* coordInfoTwo,
                                             std::vector<AnnotationCoordinateSpaceEnum::Enum>& spacesOut);
        
        static void createCoordinateInformationFromXY(const MouseEvent& mouseEvent,
                                                      AnnotationCoordinateInformation& coordInfoOut);
        
        static void createCoordinateInformationFromXY(const MouseEvent& mouseEvent,
                                                      const int32_t windowX,
                                                      const int32_t windowY,
                                                      AnnotationCoordinateInformation& coordInfoOut);
        
        static void createCoordinateInformationFromXY(BrainOpenGLWidget* openGLWidget,
                                                   BrainOpenGLViewportContent* viewportContent,
                                                   const int32_t windowX,
                                                   const int32_t windowY,
                                                   AnnotationCoordinateInformation& coordInfoOut);
        
        static bool setAnnotationCoordinatesForSpace(Annotation* annotation,
                                                     const AnnotationCoordinateSpaceEnum::Enum space,
                                                     const AnnotationCoordinateInformation* coordInfoOne,
                                                     const AnnotationCoordinateInformation* coordInfoTwo);
        
        double m_modelXYZ[3];
        bool   m_modelXYZValid;
        
        float m_tabWidth;
        float m_tabHeight;
        float m_tabXYZ[3];
        float m_tabPixelXYZ[3];
        int32_t m_tabIndex;
        
        float m_windowWidth;
        float m_windowHeight;
        float m_windowXYZ[3];
        float m_windowPixelXYZ[3];
        int32_t m_windowIndex;
        
        StructureEnum::Enum m_surfaceStructure;
        int32_t m_surfaceNumberOfNodes;
        int32_t m_surfaceNodeIndex;
        float m_surfaceNodeOffset;
        AnnotationSurfaceOffsetVectorTypeEnum::Enum m_surfaceNodeVector;
        bool m_surfaceNodeValid;
        
        
        float m_chartXYZ[3];
        bool  m_chartXYZValid;
    private:
        AnnotationCoordinateInformation(const AnnotationCoordinateInformation&);

        AnnotationCoordinateInformation& operator=(const AnnotationCoordinateInformation&);
        
        static bool setOneDimAnnotationCoordinatesForSpace(AnnotationOneDimensionalShape* annotation,
                                                           const AnnotationCoordinateSpaceEnum::Enum space,
                                                           const AnnotationCoordinateInformation* coordInfoOne,
                                                           const AnnotationCoordinateInformation* coordInfoTwo);
        
        static bool setTwoDimAnnotationCoordinatesForSpace(AnnotationTwoDimensionalShape* annotation,
                                                           const AnnotationCoordinateSpaceEnum::Enum space,
                                                           const AnnotationCoordinateInformation* coordInfoOne,
                                                           const AnnotationCoordinateInformation* coordInfoTwo);
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COORDINATE_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COORDINATE_INFORMATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_COORDINATE_INFORMATION_H__
