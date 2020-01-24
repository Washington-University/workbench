
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __ANNOTATION_STACKING_ORDER_OPERATION_DECLARE__
#include "AnnotationStackingOrderOperation.h"
#undef __ANNOTATION_STACKING_ORDER_OPERATION_DECLARE__

#include <algorithm>
#include <set>

#include "Annotation.h"
#include "AnnotationCoordinate.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationText.h"
#include "AnnotationTwoDimensionalShape.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::AnnotationStackingOrderOperation
 * \brief Perform a stacking order operation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param mode
 *     Mode to apply or not apply new stacking order
 * @param annotations
 *     The annotations that are reordered
 * @param selectedAnnotation
 *     The selected annotation that causes reordering
 * @param windowIndex
 *     Index of window
 */
AnnotationStackingOrderOperation::AnnotationStackingOrderOperation(const Mode mode,
                                                                   const std::vector<Annotation*>& annotations,
                                                                   const Annotation* selectedAnnotation,
                                                                   const int32_t windowIndex)
: CaretObject(),
m_mode(mode),
m_annotations(annotations),
m_selectedAnnotation(selectedAnnotation),
m_windowIndex(windowIndex)
{
    CaretAssert(m_selectedAnnotation);
}

/**
 * Destructor.
 */
AnnotationStackingOrderOperation::~AnnotationStackingOrderOperation()
{
}

/**
 * Run an annotation reordering operation
 *
 * @param orderType
 *     Type of ordering
 * @param errorMessageOut
 *     Output with error information
 * @return
 *     True if successful, else false
 */
bool
AnnotationStackingOrderOperation::runOrdering(const AnnotationStackingOrderTypeEnum::Enum orderType,
                                              AString& errorMessageOut)
{
    errorMessageOut.clear();
    if (m_annotations.size() <= 1) {
        return true;
    }


    bool supportedSpaceFlag(false);
    CaretAssert(m_selectedAnnotation);
    switch (m_selectedAnnotation->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            supportedSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            supportedSpaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            supportedSpaceFlag = true;
            break;
    }
    
    if ( ! supportedSpaceFlag) {
        errorMessageOut = ("Ordering for annotation in "
                           + AnnotationCoordinateSpaceEnum::toGuiName(m_selectedAnnotation->getCoordinateSpace())
                           + " coordinate space is not allowed");
        return false;
    }
    
    if ( ! filterAnnotations()) {
        errorMessageOut = "No annotations in same coordinate space overlap the selected annotation";
        return false;
    }
    
    bool found(false);
    for (auto ann : m_annotations) {
        CaretAssert(ann);
        if (ann == m_selectedAnnotation) {
            found = true;
            break;
        }
    }
    if ( ! found) {
        errorMessageOut = ("Annotation for reordering "
                           + m_selectedAnnotation->toString()
                           + " not found in all annotations");
        return false;
    }
    
    if ( ! validateCompatibility(m_annotations,
                                 errorMessageOut)) {
        return false;
    }
    
    bool largerIsFurtherAwayFlag(false);
    switch (m_selectedAnnotation->getType()) {
        case AnnotationTypeEnum::BROWSER_TAB:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::LINE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::TEXT:
            largerIsFurtherAwayFlag = true;
            break;
    }
    
    /*
     * Sort annotations by current stacking order
     */
    int32_t maxStackingOrder(-1);
    std::vector<OrderInfo> annotationOrderAndContent;
    for (const auto ann : m_annotations) {
        CaretAssert(ann);
        
        switch (ann->getType()) {
            case AnnotationTypeEnum::BROWSER_TAB:
            case AnnotationTypeEnum::COLOR_BAR:
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::BOX:
            case AnnotationTypeEnum::IMAGE:
            case AnnotationTypeEnum::LINE:
            case AnnotationTypeEnum::OVAL:
            case AnnotationTypeEnum::TEXT:
            {
                const AnnotationOneDimensionalShape* oneDim = ann->castToOneDimensionalShape();
                const AnnotationTwoDimensionalShape* twoDim = ann->castToTwoDimensionalShape();
                float xyz[3] { 0.0, 0.0, 0.0 };
                if (oneDim != NULL) {
                    oneDim->getStartCoordinate()->getXYZ(xyz);
                }
                else if (twoDim != NULL) {
                    twoDim->getCoordinate()->getXYZ(xyz);
                }
                else {
                    CaretAssert(0);
                }
                /*
                 * Coords are float but stack order is int so scale the coord
                 */
                ann->setStackingOrder(static_cast<int32_t>(xyz[2] * m_coordToStackOrderScaleFactor));
            }
                break;
        }
        
        if (ann->getStackingOrder() > maxStackingOrder) {
            maxStackingOrder = ann->getStackingOrder();
        }
        annotationOrderAndContent.emplace_back(ann,
                                               ann->getStackingOrder());
    }
    
    if (largerIsFurtherAwayFlag) {
        /*
         * Reverse stacking order
         */
        if (maxStackingOrder >= 0) {
            for (auto& aoc : annotationOrderAndContent) {
                aoc.m_stackOrder = maxStackingOrder - aoc.m_stackOrder;
            }
        }
    }
    
    std::sort(annotationOrderAndContent.begin(),
              annotationOrderAndContent.end());
    
    /*
     * Now that that annotations are sorted by stacking order,
     * assign a "stack order" using only even numbers.  This allows
     * the stack order of the selected annotation to change by
     * plus or minus one.  In addition, find the annotation that
     * is closest behind and closest in front of the selected
     * annotation.
     */
    int32_t indexOfSelectedAnnotation(-1);
    int32_t indexOfAnnotationBehind(-1);
    int32_t indexOfAnnotationInFront(-1);
    const int32_t numberOfAnnotations = static_cast<int32_t>(annotationOrderAndContent.size());
    CaretAssert(numberOfAnnotations >= 2);
    if (numberOfAnnotations < 2) {
        errorMessageOut = "There must be at least two items for ordering";
        return false;
    }
    
    for (int32_t i = 0; i < numberOfAnnotations; i++) {
        CaretAssertVectorIndex(annotationOrderAndContent, i);
        annotationOrderAndContent[i].m_stackOrder = i * 2;
        
        if (m_selectedAnnotation == annotationOrderAndContent[i].m_annotation) {
            indexOfSelectedAnnotation = i;
        }
        else if (indexOfSelectedAnnotation < 0) { /* have not yet found index of 'selectedAnnotation' */
            /*
             * Find annotation that is closest to 'selectedAnnotation' but behind and intersects it
             */
            if (m_selectedAnnotation->intersectionTest(annotationOrderAndContent[i].m_annotation,
                                                       m_windowIndex)) {
                indexOfAnnotationBehind = i;
            }
        }
        else if (indexOfAnnotationInFront < 0) {
            /*
             * Find first (closest) annotation that is in front of and intersects 'selectedAnnotation'
             */
            if (m_selectedAnnotation->intersectionTest(annotationOrderAndContent[i].m_annotation,
                                                       m_windowIndex)) {
                indexOfAnnotationInFront = i;
            }
        }
    }
    
    CaretAssert(indexOfSelectedAnnotation >= 0);
    
    /*
     * Now change the stack order of the annotation.
     * Since earlier, the stacking orders were made (0, 2, 4, ...) we only need to
     * add or subtract 1 to the stack order using the reference annotation that is in front or back
     */
    switch (orderType) {
        case AnnotationStackingOrderTypeEnum::BRING_FORWARD:
            if (indexOfAnnotationInFront >= 0) {
                /*
                 * Move forward one position
                 */
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfAnnotationInFront);
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
                annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[indexOfAnnotationInFront].m_stackOrder + 1;
            }
            break;
        case AnnotationStackingOrderTypeEnum::BRING_TO_FRONT:
        {
            /*
             * Move to in front of all annotations
             */
            const int32_t frontIndex = static_cast<int32_t>(annotationOrderAndContent.size() - 1);
            CaretAssertVectorIndex(annotationOrderAndContent, frontIndex);
            CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
            annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[frontIndex].m_stackOrder + 1;
        }
            break;
        case AnnotationStackingOrderTypeEnum::SEND_BACKWARD:
            if (indexOfAnnotationBehind >= 0) {
                /*
                 * Move back one position
                 */
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfAnnotationBehind);
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
                annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[indexOfAnnotationBehind].m_stackOrder - 1;
            }
            break;
        case AnnotationStackingOrderTypeEnum::SEND_TO_BACK:
        {
            /*
             * Move to behind all annotations
             */
            const int32_t backIndex = 0;
            CaretAssertVectorIndex(annotationOrderAndContent, backIndex);
            CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
            annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[backIndex].m_stackOrder - 1;
        }
            break;
    }
    
    /*
     * Now order annotations sequentially to clean up stacking order
     */
    std::sort(annotationOrderAndContent.begin(),
              annotationOrderAndContent.end());
    for (int32_t i = 0; i < numberOfAnnotations; i++) {
        CaretAssertVectorIndex(annotationOrderAndContent, i);
        Annotation* ann = annotationOrderAndContent[i].m_annotation;
        CaretAssert(ann);
        
        switch (ann->getType()) {
            case AnnotationTypeEnum::BROWSER_TAB:
            case AnnotationTypeEnum::COLOR_BAR:
            case AnnotationTypeEnum::SCALE_BAR:
                switch (m_mode) {
                    case Mode::MODE_APPLY_NEW_ORDER_TO_ANNOTAIONS:
                        ann->setStackingOrder(i + 1);
                        break;
                    case Mode::MODE_REQUEST_NEW_ORDER_VALUES:
                        m_newStackingOrderResults.push_back(NewStackingOrder(ann, i + 1));
                        break;
                }
                break;
            case AnnotationTypeEnum::BOX:
            case AnnotationTypeEnum::IMAGE:
            case AnnotationTypeEnum::LINE:
            case AnnotationTypeEnum::OVAL:
            case AnnotationTypeEnum::TEXT:
            {
                const float newStackOrder(i + 1);
                const float stackingZ = (numberOfAnnotations - newStackOrder);
                switch (m_mode) {
                    case Mode::MODE_APPLY_NEW_ORDER_TO_ANNOTAIONS:
                    {
                        AnnotationOneDimensionalShape* oneDim = ann->castToOneDimensionalShape();
                        AnnotationTwoDimensionalShape* twoDim = ann->castToTwoDimensionalShape();
                        float xyz[3] { 0.0, 0.0, 0.0 };
                        if (oneDim != NULL) {
                            oneDim->getStartCoordinate()->getXYZ(xyz);
                        }
                        else if (twoDim != NULL) {
                            twoDim->getCoordinate()->getXYZ(xyz);
                        }
                        else {
                            CaretAssert(0);
                        }
                        /*
                         * Need to invert order since larger is further away
                         */
                        xyz[2] = stackingZ;
                        
                        if (oneDim != NULL) {
                            oneDim->getStartCoordinate()->setXYZ(xyz);
                            const float z = xyz[2];
                            oneDim->getEndCoordinate()->getXYZ(xyz);
                            xyz[2] = z;
                            oneDim->getEndCoordinate()->setXYZ(xyz);
                        }
                        else if (twoDim != NULL) {
                            twoDim->getCoordinate()->setXYZ(xyz);
                        }
                        else {
                            CaretAssert(0);
                        }
                        break;
                    }
                    case Mode::MODE_REQUEST_NEW_ORDER_VALUES:
                        m_newStackingOrderResults.push_back(NewStackingOrder(ann, stackingZ));
                        break;
                }
            }
                break;
        }
    }
    
    return true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationStackingOrderOperation::toString() const
{
    return "AnnotationStackingOrderOperation";
}

/**
 * Filter the annotations that are incompatible with the selected annotation
 *
 * @return True there are annotations remaining after filtering
 */
bool
AnnotationStackingOrderOperation::filterAnnotations()
{
    CaretAssert(m_selectedAnnotation);
    
    std::vector<Annotation*> filteredAnnotations;
    
    if (m_selectedAnnotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
        for (auto ann : m_annotations) {
            CaretAssert(ann);
            if (ann->getType() == AnnotationTypeEnum::BROWSER_TAB) {
                filteredAnnotations.push_back(ann);
            }
        }
    }
    else {
        const AnnotationCoordinateSpaceEnum::Enum coordSpace = m_selectedAnnotation->getCoordinateSpace();
        
        for (auto ann : m_annotations) {
            CaretAssert(ann);
            
            if (m_selectedAnnotation->isInSameCoordinateSpace(ann)) {
                if (m_selectedAnnotation == ann) {
                    filteredAnnotations.push_back(ann);
                }
                else {
                    if (m_selectedAnnotation->intersectionTest(ann,
                                                               m_windowIndex)) {
                        filteredAnnotations.push_back(ann);
                    }
                }
            }
        }
    }
    
    
    m_annotations = filteredAnnotations;
    
    return (m_annotations.size() > 1);
}

/**
 * Examine the annotations (types, coordinate spaces, etc) to verify that
 * the annotations can be reordered.
 *
 * @param annotations
 *     Annotation for reordering
 * @param errorMessageOut
 *     Contains error information
 * @return
 *     True if OK else false if there is an error
 */
bool
AnnotationStackingOrderOperation::validateCompatibility(const std::vector<Annotation*>& annotations,
                                                        AString& errorMesssageOut)
{
    /*
     * filterAnnotations() makes validation unnecessary
     */
    const bool doValidationFlag(false);
    if ( ! doValidationFlag) {
        return true;
    }
    
    
    if (annotations.empty()) {
        errorMesssageOut = "No annotations for reordering";
        return false;
    }
    
    std::set<AnnotationTypeEnum::Enum> types;
    std::set<AnnotationCoordinateSpaceEnum::Enum> spaces;
    std::set<int32_t> spacerIndices;
    std::set<int32_t> tabIndices;
    std::set<int32_t> windowIndices;

    for (const auto a : m_annotations) {
        CaretAssert(a);
        
        types.insert(a->getType());
        
        const AnnotationCoordinateSpaceEnum::Enum space = a->getCoordinateSpace();
        spaces.insert(space);
        
        switch (space) {
            case AnnotationCoordinateSpaceEnum::CHART:
                errorMesssageOut = "Chart space annotations cannot be ordered";
                return false;
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                errorMesssageOut = "Spacer space annotations cannot be ordered";
                return false;
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                errorMesssageOut = "Stereotaxic Space Annotations cannot be ordered (XYZ coordinates determine order)";
                return false;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                errorMesssageOut = "Stereotaxic Space Annotations cannot be ordered (surface coordinates determine order)";
                return false;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                tabIndices.insert(a->getTabIndex());
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                errorMesssageOut = "Viewport space annotations cannot be ordered";
                return false;
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                windowIndices.insert(a->getWindowIndex());
                break;
        }
    }
    
    /*
     * If all are browser tabs, then OK
     */
    if (types.size() == 1) {
        if (*types.begin() == AnnotationTypeEnum::BROWSER_TAB) {
            return true;
        }
    }
    
    if (spaces.size() > 1) {
        errorMesssageOut = "Annotations must be in one space but are in spaces: ";
        for (auto s : spaces) {
            errorMesssageOut.append(" " + AnnotationCoordinateSpaceEnum::toGuiName(s));
        }
        return false;
    }
    else if (spaces.empty()) {
        errorMesssageOut = "Annotations are not in any space";
        return false;
    }
    
    const AnnotationCoordinateSpaceEnum::Enum annSpace = *spaces.begin();
    switch (annSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (tabIndices.size() == 1) {
                return true;
            }
            else {
                errorMesssageOut = "All tab annotations must be in same tab";
                return false;
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (windowIndices.size() == 1) {
                return true;
            }
            else {
                errorMesssageOut = "All window annotations must be in same window";
                return false;
            }
            break;
    }
    
    if (errorMesssageOut.isEmpty()) {
        errorMesssageOut = "Annotation are incompatible for reordering (unknown reason)";
    }
    
    return false;
}

/**
 * @return The results with annotations and their new stack order.  No modifications
 * have been made to the annotations, it is up to the caller to assign the new stack
 * order to the annotations.
 */
std::vector<AnnotationStackingOrderOperation::NewStackingOrder>
AnnotationStackingOrderOperation::getNewStackingOrderResults() const
{
    return m_newStackingOrderResults;
}


