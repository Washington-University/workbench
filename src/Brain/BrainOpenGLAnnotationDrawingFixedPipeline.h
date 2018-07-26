#ifndef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__
#define __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__

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
#include <stdint.h>

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationSizingHandleTypeEnum.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretObject.h"
#include "CaretOpenGLInclude.h"
#include "Plane.h"


namespace caret {

    class Annotation;
    class AnnotationBox;
    class AnnotationColorBar;
    class AnnotationCoordinate;
    class AnnotationFile;
    class AnnotationImage;
    class AnnotationLine;
    class AnnotationOval;
    class AnnotationText;
    class AnnotationTwoDimensionalShape;
    class Brain;
    class BrainOpenGLFixedPipeline;
    class EventOpenGLObjectToWindowTransform;
    class Surface;
    
    class BrainOpenGLAnnotationDrawingFixedPipeline : public CaretObject {
        
    public:
        class Inputs {
        public:
            enum WindowDrawingMode {
                WINDOW_DRAWING_NO,
                WINDOW_DRAWING_YES
            };
            
            Inputs(Brain* brain,
                   const BrainOpenGLFixedPipeline::Mode drawingMode,
                   const float centerToEyeDistance,
                   const int32_t windowIndex,
                   const int32_t tabIndex,
                   const WindowDrawingMode windowDrawingMode)
            : m_brain(brain),
            m_drawingMode(drawingMode),
            m_centerToEyeDistance(centerToEyeDistance),
            m_windowIndex(windowIndex),
            m_tabIndex(tabIndex),
            m_windowDrawingMode(windowDrawingMode) {
            }
            
            Brain* m_brain;
            const BrainOpenGLFixedPipeline::Mode m_drawingMode;
            const float m_centerToEyeDistance;
            const int32_t m_windowIndex;
            const int32_t m_tabIndex;
            const WindowDrawingMode m_windowDrawingMode;
        };
        
        BrainOpenGLAnnotationDrawingFixedPipeline(BrainOpenGLFixedPipeline* brainOpenGLFixedPipeline);
        
        virtual ~BrainOpenGLAnnotationDrawingFixedPipeline();
        
        void drawAnnotations(Inputs* inputs,
                             const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                             std::vector<AnnotationColorBar*>& colorBars,
                             std::vector<Annotation*>& notInFileAnnotations,
                             const Surface* surfaceDisplayed);

        void drawModelSpaceAnnotationsOnVolumeSlice(Inputs* inputs,
                                                    const Plane& plane,
                                                    const float sliceThickness);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class SelectionInfo {
        public:
            SelectionInfo(AnnotationFile* annotationFile,
                          Annotation* annotation,
                          AnnotationSizingHandleTypeEnum::Enum sizingHandle,
                          const float windowXYZ[3]) {
                m_annotationFile = annotationFile;
                m_annotation     = annotation;
                m_sizingHandle   = sizingHandle;
                m_windowXYZ[0]   = windowXYZ[0];
                m_windowXYZ[1]   = windowXYZ[1];
                m_windowXYZ[2]   = windowXYZ[2];
            }
            
            AnnotationFile* m_annotationFile;
            
            Annotation* m_annotation;
            
            AnnotationSizingHandleTypeEnum::Enum m_sizingHandle;
            
            double m_windowXYZ[3];
        };
        
        class ColorBarLine {
        public:
            ColorBarLine(const std::vector<float>& lineCoords,
                         const float rgba[4]) {
                m_lineCoords = lineCoords;
                m_rgba[0] = rgba[0];
                m_rgba[1] = rgba[1];
                m_rgba[2] = rgba[2];
                m_rgba[3] = rgba[3];
            }
            
            std::vector<float> m_lineCoords;
            float m_rgba[4];
        };
        
        /**
         * Used to save viewport, model view, projection
         */
        class TransformationInfo {
        public:
            void save();
            void restore();
            
            GLint m_viewport[4];
            GLdouble m_modelViewMatrix[16];
            GLdouble m_projectionMatrix[16];
            bool m_valid = false;
        };
        
        BrainOpenGLAnnotationDrawingFixedPipeline(const BrainOpenGLAnnotationDrawingFixedPipeline&);

        BrainOpenGLAnnotationDrawingFixedPipeline& operator=(const BrainOpenGLAnnotationDrawingFixedPipeline&);
        
        void drawAnnotationsInternal(const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                                     std::vector<AnnotationColorBar*>& colorBars,
                                     std::vector<Annotation*>& viewportAnnotations,
                                     const Surface* surfaceDisplayed,
                                     const float sliceThickness);
        
        bool getAnnotationDrawingSpaceCoordinate(const Annotation* annotation,
                                                 const AnnotationCoordinate* coordinate,
                                                 const Surface* surfaceDisplayed,
                                                 float xyzOut[3]) const;
        
        bool getAnnotationTwoDimShapeBounds(const AnnotationTwoDimensionalShape* annotation2D,
                                 const float windowXYZ[3],
                                 float bottomLeftOut[3],
                                 float bottomRightOut[3],
                                 float topRightOut[3],
                                 float topLeftOut[3]) const;
        
        void applyRotationToShape(const float rotationAngle,
                                  const float rotationPoint[3],
                                  float bottomLeftOut[3],
                                  float bottomRightOut[3],
                                  float topRightOut[3],
                                  float topLeftOut[3]) const;
        
        void drawAnnotation(AnnotationFile* annotationFile,
                            Annotation* annotation,
                            const Surface* surfaceDisplayed);
        
        void drawColorBar(AnnotationFile* annotationFile,
                          AnnotationColorBar* colorBar);
        
        bool drawBox(AnnotationFile* annotationFile,
                     AnnotationBox* box,
                       const Surface* surfaceDisplayed);
        
        bool drawImage(AnnotationFile* annotationFile,
                       AnnotationImage* image,
                      const Surface* surfaceDisplayed);
        
        bool drawLine(AnnotationFile* annotationFile,
                      AnnotationLine* line,
                      const Surface* surfaceDisplayed);
        
        bool drawOval(AnnotationFile* annotationFile,
                      AnnotationOval* oval,
                      const Surface* surfaceDisplayed);
        
        bool drawText(AnnotationFile* annotationFile,
                      AnnotationText* text,
                       const Surface* surfaceDisplayed);
        
        bool drawTextTangentOffset(AnnotationFile* annotationFile,
                                   AnnotationText* text,
                                   const Surface* surfaceDisplayed);
        
        void drawColorBarSections(const AnnotationColorBar* colorBar,
                                  const float bottomLeft[3],
                                  const float bottomRight[3],
                                  const float topRight[3],
                                  const float topLeft[3],
                                  const float sectionsHeightInPixels);
        
        void drawColorBarText(const AnnotationColorBar* colorBar,
                               const float bottomLeft[3],
                               const float bottomRight[3],
                               const float topRight[3],
                               const float topLeft[3],
                               const float textHeightInPixels,
                               const float offsetFromTopInPixels);

        void drawColorBarTickMarks(const AnnotationColorBar* colorBar,
                                   const float bottomLeft[3],
                                   const float bottomRight[3],
                                   const float topRight[3],
                                   const float topLeft[3],
                                   const float tickMarksHeightInPixels,
                                   const float offsetFromBottomInPixels);

        void drawSizingHandle(const AnnotationSizingHandleTypeEnum::Enum handleType,
                              AnnotationFile* annotationFile,
                              Annotation* annotation,
                              const float xyz[3],
                              const float halfWidthHeight,
                              const float rotationAngle);
        
        void drawAnnotationTwoDimSizingHandles(AnnotationFile* annotationFile,
                                               Annotation* annotation,
                                               const float bottomLeft[3],
                                               const float bottomRight[3],
                                               const float topRight[3],
                                               const float topLeft[3],
                                               const float lineThickness,
                                               const float rotationAngle);

        void drawAnnotationOneDimSizingHandles(AnnotationFile* annotationFile,
                                               Annotation* annotation,
                                               const float firstPoint[3],
                                               const float secondPoint[3],
                                               const float lineThickness);
        
        bool isDrawnWithDepthTesting(const Annotation* annotation,
                                     const Surface* surface);
        
        bool setDepthTestingStatus(const bool newDepthTestingStatus);
        
        void getIdentificationColor(uint8_t identificationColorOut[4]);
        
        bool convertModelToWindowCoordinate(const float modelXYZ[3],
                                            float windowXYZOut[3]) const;
        
        void viewportToOpenGLWindowCoordinate(const float viewportXYZ[3],
                                              float openGLXYZOut[3]) const;
        
        void createLineCoordinates(const float lineHeadXYZ[3],
                                   const float lineTailXYZ[3],
                                   const float lineThickness,
                                   const bool validStartArrow,
                                   const bool validEndArrow,
                                   std::vector<float>& lineCoordinatesOut,
                                   std::vector<float>& startArrowCoordinatesOut,
                                   std::vector<float>& endArrowCoordinatesOut) const;
        
        void getTextLineToBrainordinateLineCoordinates(const AnnotationText* text,
                                                       const Surface* surfaceDisplayed,
                                                       const float bottomLeft[3],
                                                       const float bottomRight[3],
                                                       const float topRight[3],
                                                       const float topLeft[3],
                                                       std::vector<float>& lineCoordinatesOut,
                                                       std::vector<float>& arrowCoordinatesOut) const;
        
        static void expandBox(float bottomLeft[3],
                       float bottomRight[3],
                       float topRight[3],
                       float topLeft[3],
                       const float extraSpaceX,
                       const float extraSpaceY);
        
        void setSelectionBoxColor();
        
        void startOpenGLForDrawing(GLint* savedShadeModelOut,
                                   GLboolean* savedLightingEnabledOut);
        
        void endOpenGLForDrawing(GLint savedShadeModel,
                                 GLboolean savedLightingEnabled);
        
        void clipLineAtTextBox(const float bottomLeft[3],
                               const float bottomRight[3],
                               const float topRight[3],
                               const float topLeft[3],
                               const float startXYZ[3],
                               float endXYZ[3]) const;
        
        void convertObsoleteLineWidthPixelsToPercentageWidth(const Annotation* annotation) const;
        
        float getLineWidthFromPercentageHeight(const float percentageHeight) const;
        
        float getLineWidthPercentageInSelectionMode(const Annotation* annotation) const;
        
        float estimateColorBarWidth(const AnnotationColorBar* colorBar,
                                    const float textHeightInPixels) const;
        
        BrainOpenGLFixedPipeline* m_brainOpenGLFixedPipeline;
        
        Inputs* m_inputs;
        
        /**
         * Dummy annotation file is used for annotations that 
         * do not belong to a file.  This includes the 
         * "annotation being drawn" and AnnotationColorBar's.
         */
        AnnotationFile* m_dummyAnnotationFile;
        
        /** Tracks items drawn for selection */
        std::vector<SelectionInfo> m_selectionInfo;
        
        /** In selection mode */
        bool m_selectionModeFlag;
        
        /** OpenGL Model Matrix */
        GLdouble m_modelSpaceModelMatrix[16];
        
        /** OpenGL Projection Matrix */
        GLdouble m_modelSpaceProjectionMatrix[16];
        
        /** OpenGL Viewport */
        GLint m_modelSpaceViewport[4];
        
        /** volume space plane */
        Plane m_volumeSpacePlane;
        
        /** Validity of volume space plane */
        bool m_volumeSpacePlaneValid;
        
        /** Thickness of volume slice when drawing annotations on volume slices */
        float m_volumeSliceThickness;
        
        /** Color for selection box and sizing handles */
        uint8_t m_selectionBoxRGBA[4];
        
        float m_lineWidthMinimum = 1.0f;
        
        BrainOpenGLTextRenderInterface::DrawingFlags m_textDrawingFlags;
        
        std::unique_ptr<EventOpenGLObjectToWindowTransform> m_transformEvent;
        
        static constexpr float s_sizingHandleLineWidthInPixels = 2.0f;
        static constexpr float s_selectionLineMinimumPixelWidth = 5.0f;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__
