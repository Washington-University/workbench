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
#include "SpacerTabIndex.h"
#include "StructureEnum.h"

class QLabel;

namespace caret {

    class Annotation;
    class AnnotationTwoCoordinateShape;
    class AnnotationOneCoordinateShape;
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
        
        class SpaceInfo {
        public:
            bool m_validFlag = false;
        };
        
        class ModelSpaceInfo : public SpaceInfo {
        public:
            double m_xyz[3] = { 0.0, 0.0, 0.0 };
        };
        
        class TabWindowSpaceInfo : public SpaceInfo {
        public:
            float m_width = 0.0f;
            float m_height = 0.0f;
            float m_xyz[3] = { 0.0f, 0.0f, 0.0f };
            float m_pixelXYZ[3] = { 0.0f, 0.0f, 0.0f };
            int32_t m_index = -1;
        };
        
        class SpacerTabSpaceInfo : public SpaceInfo {
        public:
            float m_width = 0.0f;
            float m_height = 0.0f;
            float m_xyz[3] = { 0.0f, 0.0f, 0.0f };
            float m_pixelXYZ[3] = { 0.0f, 0.0f, 0.0f };
            SpacerTabIndex m_spacerTabIndex;
        };
        
        class ChartSpaceInfo : public SpaceInfo {
        public:
            float m_xyz[3] = { 0.0f, 0.0f, 0.0f };
        };
        
        class SurfaceSpaceInfo : public SpaceInfo {
        public:
            StructureEnum::Enum m_structure = StructureEnum::INVALID;
            int32_t m_numberOfNodes = 0;
            int32_t m_nodeIndex = -1;
            float m_nodeOffsetLength = 0.0f;
            float m_nodeNormalVector[3] = { 0.0f, 0.0f, 1.0f };
            AnnotationSurfaceOffsetVectorTypeEnum::Enum m_nodeVectorOffsetType = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
        };
        
        ModelSpaceInfo m_modelSpaceInfo;
        
        TabWindowSpaceInfo m_tabSpaceInfo;
        
        TabWindowSpaceInfo m_windowSpaceInfo;
        
        SpacerTabSpaceInfo m_spacerTabSpaceInfo;
        
        ChartSpaceInfo m_chartSpaceInfo;
        
        SurfaceSpaceInfo m_surfaceSpaceInfo;
        
    private:
        AnnotationCoordinateInformation(const AnnotationCoordinateInformation&);

        AnnotationCoordinateInformation& operator=(const AnnotationCoordinateInformation&);
        
        static bool setOneDimAnnotationCoordinatesForSpace(AnnotationTwoCoordinateShape* annotation,
                                                           const AnnotationCoordinateSpaceEnum::Enum space,
                                                           const AnnotationCoordinateInformation* coordInfoOne,
                                                           const AnnotationCoordinateInformation* coordInfoTwo);
        
        static bool setTwoDimAnnotationCoordinatesForSpace(AnnotationOneCoordinateShape* annotation,
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
