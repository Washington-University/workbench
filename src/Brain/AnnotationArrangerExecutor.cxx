
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

#include <algorithm>
#include <limits>
#include <numeric>

#define __ANNOTATION_ARRANGER_EXECUTOR_DECLARE__
#include "AnnotationArrangerExecutor.h"
#undef __ANNOTATION_ARRANGER_EXECUTOR_DECLARE__

#include "AnnotationArrangerInputs.h"
#include "AnnotationCoordinate.h"
#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationText.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "EventGetViewportSize.h"
#include "EventManager.h"

using namespace caret;


    
/**
 * \class caret::AnnotationArrangerExecutor 
 * \brief Performs alignment, distribution of annotations
 * \ingroup Brain
 */

/**
 * Constructor.
 */
AnnotationArrangerExecutor::AnnotationArrangerExecutor()
: CaretObject(),
m_annotationManager(NULL)
{
    
}

/**
 * Destructor.
 */
AnnotationArrangerExecutor::~AnnotationArrangerExecutor()
{
}

/**
 * For each annotation, get a coordinate value that is will be
 * used to align the annotations.  For example, if aligning
 * to "top", "alignToValuesOut" will contain the maximum Y-value
 * for each annotation.
 *
 * @param alignmentType
 *     Type of alignment.
 * @param annotations
 *     Annotations that will be aligned.
 * @param windowViewport
 *     Viewport of the window containing the annotations.
 * @param alignsToValueOut
 *     Output containing the alignment value for each annotation.
 * @param errorMessageOut
 *     Contains description of error.
 * @return
 *     True if successful, false if error.
 */
bool
AnnotationArrangerExecutor::getAlignmentToValues(const AnnotationArrangerInputs& arrangerInputs,
                                                 std::vector<Annotation*>& annotations,
                                                 const int32_t windowViewport[4],
                                                 std::vector<float>& alignToValuesOut,
                                                 AString& errorMessageOut)
{
    const AnnotationAlignmentEnum::Enum alignmentType = arrangerInputs.getAlignment();
    
    alignToValuesOut.clear();
    alignToValuesOut.reserve(annotations.size());
    
    for (std::vector<Annotation*>::iterator annIter = annotations.begin();
         annIter != annotations.end();
         annIter++) {
        const Annotation* ann = *annIter;
        CaretAssert(ann);
        
        int32_t annViewport[4] = { 0, 0, 0, 0 };
        switch (ann->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::PIXELS:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
            {
                EventGetViewportSize vpEvent(EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX,
                                             ann->getTabIndex());
                EventManager::get()->sendEvent(vpEvent.getPointer());
                if (vpEvent.isViewportSizeValid()) {
                    vpEvent.getViewportSize(annViewport);
                }
                else {
                    errorMessageOut = ("Failed to get viewport size for tab index "
                                       + QString::number(ann->getTabIndex() + 1));
                    return false;
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                annViewport[0] = windowViewport[0];
                annViewport[1] = windowViewport[1];
                annViewport[2] = windowViewport[2];
                annViewport[3] = windowViewport[3];
                break;
        }
        
        const AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<const AnnotationOneDimensionalShape*>(ann);
        const AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<const AnnotationTwoDimensionalShape*>(ann);
        
        float alignmentValue = 0.0;
        
        if (oneDimAnn != NULL) {
            float xyz1[3];
            oneDimAnn->getStartCoordinate()->getXYZ(xyz1);
            float xyz2[3];
            oneDimAnn->getEndCoordinate()->getXYZ(xyz2);
            
            switch (alignmentType) {
                case AnnotationAlignmentEnum::ALIGN_BOTTOM:
                {
                    alignmentValue = std::min(xyz1[1], xyz2[1]);
                }
                    break;
                case AnnotationAlignmentEnum::ALIGN_CENTER:
                {
                    alignmentValue = (xyz1[0] + xyz2[0]) / 2.0;
                }
                    break;
                case AnnotationAlignmentEnum::ALIGN_LEFT:
                {
                    alignmentValue = std::min(xyz1[0], xyz2[0]);
                }
                    break;
                case AnnotationAlignmentEnum::ALIGN_MIDDLE:
                {
                    alignmentValue = (xyz1[1] + xyz2[1]) / 2.0;
                }
                    break;
                case AnnotationAlignmentEnum::ALIGN_RIGHT:
                {
                    alignmentValue = std::max(xyz1[0], xyz2[0]);
                }
                    break;
                case AnnotationAlignmentEnum::ALIGN_TOP:
                {
                    alignmentValue = std::max(xyz1[1], xyz2[1]);
                }
                    break;
            }
        }
        else if (twoDimAnn != NULL) {
            float xyz[3];
            twoDimAnn->getCoordinate()->getXYZ(xyz);
            
            float viewportXYZ[3] = { 0.0, 0.0, 0.0 };
            Annotation::relativeXYZToViewportXYZ(xyz,
                                                 annViewport[2],
                                                 annViewport[3],
                                                 viewportXYZ);
            
            float bottomLeft[3];
            float bottomRight[3];
            float topRight[3];
            float topLeft[3];
            
            bool boundsValidFlag = false;
            
            if (twoDimAnn->getType() == AnnotationTypeEnum::TEXT) {
                const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(twoDimAnn);
                CaretAssert(textAnn);
                arrangerInputs.getTextRender()->getBoundsForTextAtViewportCoords(*textAnn,
                                                                                 viewportXYZ[0],
                                                                                 viewportXYZ[1],
                                                                                 viewportXYZ[2],
                                                                                 bottomLeft,
                                                                                 bottomRight,
                                                                                 topRight,
                                                                                 topLeft);
                boundsValidFlag = true;
            }
            else {
                boundsValidFlag = twoDimAnn->getShapeBounds(annViewport[2],
                                                           annViewport[3],
                                                           viewportXYZ,
                                                           bottomLeft,
                                                           bottomRight,
                                                           topRight,
                                                           topLeft);
            }
            if (boundsValidFlag) {
                
                switch (alignmentType) {
                    case AnnotationAlignmentEnum::ALIGN_BOTTOM:
                    {
                        alignmentValue = std::min(std::min(bottomLeft[1], bottomRight[1]),
                                                  std::min(topRight[1], topLeft[1]));
                    }
                        break;
                    case AnnotationAlignmentEnum::ALIGN_CENTER:
                    {
                        alignmentValue = annViewport[0] + viewportXYZ[0];
                    }
                        break;
                    case AnnotationAlignmentEnum::ALIGN_LEFT:
                    {
                        alignmentValue = std::min(std::min(bottomLeft[0], bottomRight[0]),
                                                  std::min(topRight[0], topLeft[0]));
                    }
                        break;
                    case AnnotationAlignmentEnum::ALIGN_MIDDLE:
                    {
                        alignmentValue = annViewport[1] + viewportXYZ[1];
                    }
                        break;
                    case AnnotationAlignmentEnum::ALIGN_RIGHT:
                    {
                        alignmentValue = std::max(std::max(bottomLeft[0], bottomRight[0]),
                                                  std::max(topRight[0], topLeft[0]));
                    }
                        break;
                    case AnnotationAlignmentEnum::ALIGN_TOP:
                    {
                        alignmentValue = std::max(std::max(bottomLeft[1], bottomRight[1]),
                                                  std::max(topRight[1], topLeft[1]));
                    }
                        break;
                }
            }
            else {
                errorMessageOut = ("Failed to get bounds for an annotation: "
                                   + twoDimAnn->toString());
                return false;
            }
        }
        
        alignToValuesOut.push_back(alignmentValue);
    }
    
    CaretAssert(annotations.size() == alignToValuesOut.size());
    
    return (alignToValuesOut.size() > 0);
}

/**
 * Apply alignment modification to selected annotations
 *
 * @param arrangerInputs
 *     Contains information about the alignment.
 * @param errorMessageOut
 *     Contains description of error.
 * @return
 *     True if the alignment was successful, else false.
 */
bool
AnnotationArrangerExecutor::alignAnnotations(AnnotationManager* annotationManager,
                                             const AnnotationArrangerInputs& arrangerInputs,
                                             AString& errorMessageOut)
{
    m_annotationManager = annotationManager;
    CaretAssert(m_annotationManager);
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> spaces;
    spaces.push_back(AnnotationCoordinateSpaceEnum::TAB);
    spaces.push_back(AnnotationCoordinateSpaceEnum::WINDOW);
    
    std::vector<Annotation*> annotations = m_annotationManager->getSelectedAnnotationsInSpaces(arrangerInputs.getWindowIndex(),
                                                                          spaces);
    
    const int32_t numAnnotations = static_cast<int32_t>(annotations.size());
    if (numAnnotations <= 0) {
        errorMessageOut = ("Alignment is performed only on tab and window annotations"
                           " and none are selected.");
        return false;
    }
    
    int32_t windowViewport[4];
    EventGetViewportSize vpEvent(EventGetViewportSize::MODE_WINDOW_INDEX,
                                 arrangerInputs.getWindowIndex());
    EventManager::get()->sendEvent(vpEvent.getPointer());
    if (vpEvent.isViewportSizeValid()) {
        vpEvent.getViewportSize(windowViewport);
    }
    else {
        errorMessageOut = ("Failed to get viewport size for window index "
                           + QString::number(arrangerInputs.getWindowIndex() + 1));
        return false;
    }
    
    const AnnotationAlignmentEnum::Enum alignmentType = arrangerInputs.getAlignment();
    
    std::vector<float> annotationAlignmentValues;
    
    if ( ! getAlignmentToValues(arrangerInputs,
                                annotations,
                                windowViewport,
                                annotationAlignmentValues,
                                errorMessageOut)) {
        return false;
    }
    
    CaretAssert(numAnnotations == static_cast<int32_t>(annotationAlignmentValues.size()));
    
    
    float alignToValue = 0.0;
    switch (alignmentType) {
        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
            alignToValue = *std::min_element(annotationAlignmentValues.begin(),
                                             annotationAlignmentValues.end());
            break;
        case AnnotationAlignmentEnum::ALIGN_CENTER:
            alignToValue = (std::accumulate(annotationAlignmentValues.begin(),
                                            annotationAlignmentValues.end(),
                                            0.0f) / static_cast<float>(annotationAlignmentValues.size()));
            break;
        case AnnotationAlignmentEnum::ALIGN_LEFT:
            alignToValue = *std::min_element(annotationAlignmentValues.begin(),
                                             annotationAlignmentValues.end());
            break;
        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
            alignToValue = (std::accumulate(annotationAlignmentValues.begin(),
                                            annotationAlignmentValues.end(),
                                            0.0f) / static_cast<float>(annotationAlignmentValues.size()));
            break;
        case AnnotationAlignmentEnum::ALIGN_RIGHT:
            alignToValue = *std::max_element(annotationAlignmentValues.begin(),
                                             annotationAlignmentValues.end());
            break;
        case AnnotationAlignmentEnum::ALIGN_TOP:
            alignToValue = *std::max_element(annotationAlignmentValues.begin(),
                                             annotationAlignmentValues.end());
            break;
    }
    
    std::cout << "Align to value: " << alignToValue << std::endl;
    
    switch (alignmentType) {
        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
            break;
        case AnnotationAlignmentEnum::ALIGN_CENTER:
            break;
        case AnnotationAlignmentEnum::ALIGN_LEFT:
            break;
        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
            break;
        case AnnotationAlignmentEnum::ALIGN_RIGHT:
            break;
        case AnnotationAlignmentEnum::ALIGN_TOP:
            break;
    }
    
    //    int32_t viewport[4];
    //    if (getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB) {
    //        alignmentModification.getTabViewport(viewport);
    //    }
    //    else if (getCoordinateSpace() == AnnotationCoordinateSpaceEnum::WINDOW) {
    //        alignmentModification.getWindowViewport(viewport);
    //    }
    //    else {
    //        CaretAssert(0);
    //        return false;
    //    }
    //
    //    const float viewportWidth  = viewport[2];
    //    const float viewportHeight = viewport[3];
    //
    //    float xyz[3];
    //    getCoordinate()->getXYZ(xyz);
    //
    //    float bottomLeft[3] = { 0.0, 0.0, 0.0 };
    //    float bottomRight[3] = { 0.0, 0.0, 0.0 };
    //    float topRight[3] = { 0.0, 0.0, 0.0 };
    //    float topLeft[3] = { 0.0, 0.0, 0.0 };
    //    if (getShapeBounds(viewportWidth,
    //                       viewportHeight,
    //                       xyz,
    //                       bottomLeft,
    //                       bottomRight,
    //                       topRight,
    //                       topLeft)) {
    //        
    //    }
    //    
    //    return false;
    
    return true;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationArrangerExecutor::toString() const
{
    return "AnnotationArrangerExecutor";
}

