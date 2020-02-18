
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
    
    if ( ! validateInput(errorMessageOut)) {
        return false;
    }
    
    std::vector<OrderInfo> annotationOrderAndContent;
    if ( ! preOrderAnnotations(annotationOrderAndContent,
                               errorMessageOut)) {
        return false;
    }
    
    const bool debugFlag(false);
    if (debugFlag) {
        std::cout << "PRE-ORDERED" << std::endl;
        printOrderedAnnotations(annotationOrderAndContent);
    }
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

    if ( ! findAnnotationIndices(annotationOrderAndContent,
                                 indexOfSelectedAnnotation,
                                 indexOfAnnotationBehind,
                                 indexOfAnnotationInFront,
                                 errorMessageOut)) {
        return false;
    }
    CaretAssert(indexOfSelectedAnnotation >= 0);

    const int32_t numberOfAnnotations = static_cast<int32_t>(annotationOrderAndContent.size());
    
    if (debugFlag) {
        std::cout << "INDICES: " << std::endl;
        std::cout << "In front: " << indexOfAnnotationInFront << std::endl;
        std::cout << "Selected: " << indexOfSelectedAnnotation << std::endl;
        std::cout << "Behind:   " << indexOfAnnotationBehind << std::endl;
        printOrderedAnnotations(annotationOrderAndContent);
    }
    
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
                annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[indexOfAnnotationInFront].m_stackOrder - 1;
            }
            break;
        case AnnotationStackingOrderTypeEnum::BRING_TO_FRONT:
        {
            /*
             * Move to in front of all annotations
             */
            const int32_t frontIndex = 0;
            CaretAssertVectorIndex(annotationOrderAndContent, frontIndex);
            CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
            annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[frontIndex].m_stackOrder - 1;
        }
            break;
        case AnnotationStackingOrderTypeEnum::SEND_BACKWARD:
            if (indexOfAnnotationBehind >= 0) {
                /*
                 * Move back one position
                 */
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfAnnotationBehind);
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
                annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[indexOfAnnotationBehind].m_stackOrder + 1;
            }
            break;
        case AnnotationStackingOrderTypeEnum::SEND_TO_BACK:
        {
            /*
             * Move to behind all annotations
             */
            const int32_t backIndex = static_cast<int32_t>(annotationOrderAndContent.size() - 1);
            CaretAssertVectorIndex(annotationOrderAndContent, backIndex);
            CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
            annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[backIndex].m_stackOrder + 1;
        }
            break;
    }
    
    /*
     * Now sort again since the sort order has changed
     */
    std::sort(annotationOrderAndContent.begin(),
              annotationOrderAndContent.end());
    
    if (debugFlag) {
        std::cout << "AFTER ORDERING: " << std::endl;
        printOrderedAnnotations(annotationOrderAndContent);
    }
    
    if (debugFlag) {
        std::cout << "FINAL: " << std::endl;
    }
    /*
     * Update annotations with new sort order or Z-coordinate
     */
    for (int32_t indx = 0; indx < numberOfAnnotations; indx++) {
        CaretAssertVectorIndex(annotationOrderAndContent, indx);
        Annotation* ann = annotationOrderAndContent[indx].m_annotation;
        CaretAssert(ann);
        
        /*
         * Order with even numbers.  Users may modify the order
         * and it creates "space" to reduce annotations with same
         * order value.
         */
        const int32_t orderValue = (indx + 1);
        
        if (debugFlag) {
            std::cout << orderValue << " " << ann->toString() << std::endl;
        }
        switch (ann->getType()) {
            case AnnotationTypeEnum::BROWSER_TAB:
            case AnnotationTypeEnum::COLOR_BAR:
            case AnnotationTypeEnum::SCALE_BAR:
                switch (m_mode) {
                    case Mode::MODE_APPLY_NEW_ORDER_TO_ANNOTAIONS:
                        ann->setStackingOrder(orderValue);
                        break;
                    case Mode::MODE_REQUEST_NEW_ORDER_VALUES:
                        m_newStackingOrderResults.push_back(NewStackingOrder(ann, orderValue));
                        break;
                }
                break;
            case AnnotationTypeEnum::BOX:
            case AnnotationTypeEnum::IMAGE:
            case AnnotationTypeEnum::LINE:
            case AnnotationTypeEnum::OVAL:
            case AnnotationTypeEnum::TEXT:
            {
                switch (m_mode) {
                    case Mode::MODE_APPLY_NEW_ORDER_TO_ANNOTAIONS:
                    {
                        AnnotationOneDimensionalShape* oneDim = ann->castToOneDimensionalShape();
                        AnnotationTwoDimensionalShape* twoDim = ann->castToTwoDimensionalShape();
                        if (oneDim != NULL) {
                            setCoordinateZ(oneDim->getStartCoordinate(), orderValue);
                            setCoordinateZ(oneDim->getEndCoordinate(), orderValue);
                        }
                        else if (twoDim != NULL) {
                            setCoordinateZ(twoDim->getCoordinate(), orderValue);
                        }
                        else {
                            CaretAssert(0);
                        }
                        break;
                    }
                    case Mode::MODE_REQUEST_NEW_ORDER_VALUES:
                        m_newStackingOrderResults.push_back(NewStackingOrder(ann, orderValue));
                        break;
                }
            }
                break;
        }
    }
    
    return true;
}

/**
 * Set the Z-component of an annotation coordinate
 *
 * @param coordinate
 *   The annotation coordinate
 * @param z
 *   New Z value
 */
void
AnnotationStackingOrderOperation::setCoordinateZ(AnnotationCoordinate* coordinate,
                                                 const float z)
{
    CaretAssert(coordinate);

    float xyz[3] { 0.0, 0.0, 0.0 };
    coordinate->getXYZ(xyz);
    xyz[2] = z;
    coordinate->setXYZ(xyz);
}

/**
 * Find indices of the seleted, behind, and in front annotations
 *
 * @param indexOfSelectedAnnotation
 *   Index of the selected annotation in the ordered annotations
 * @param indexOfAnnotationBehind
 *   Index of the annotation immediately behind the selected annotation in the ordered annotations
 *   May be invalid (-1) if selected annotation is in back of all annotations
 * @param indexOfAnnotationInFront
 *   Index of the annotation immediately in front of the selected annotation in the ordered annotations
 *   May be invalid (-1) if selected annotation is in front of all annotations
 * @param errorMessageOut
 *   Ouput with error message
 * @return True if successful, else false
 */
bool
AnnotationStackingOrderOperation::findAnnotationIndices(const std::vector<OrderInfo>& annotationOrderAndContent,
                                                        int32_t& indexOfSelectedAnnotation,
                                                        int32_t& indexOfAnnotationBehind,
                                                        int32_t& indexOfAnnotationInFront,
                                                        AString& errorMessageOut)
{
    indexOfSelectedAnnotation = -1;
    indexOfAnnotationBehind   = -1;
    indexOfAnnotationInFront  = -1;
    
    const int32_t numAnn = static_cast<int32_t>(annotationOrderAndContent.size());
    for (int32_t i = 0; i < numAnn; i++) {
        if (annotationOrderAndContent[i].m_annotation == m_selectedAnnotation) {
            indexOfSelectedAnnotation = i;
            break;
        }
    }
    
    if (indexOfSelectedAnnotation >= 0) {
            for (int32_t i = (indexOfSelectedAnnotation - 1); i >= 0; i--) {
                CaretAssertVectorIndex(annotationOrderAndContent, i);
                if (annotationOrderAndContent[i].m_overlapsFlag) {
                    indexOfAnnotationInFront = i;
                    break;
                }
            }

            for (int32_t i = (indexOfSelectedAnnotation + 1); i < numAnn; i++) {
                CaretAssertVectorIndex(annotationOrderAndContent, i);
                if (annotationOrderAndContent[i].m_overlapsFlag) {
                    indexOfAnnotationBehind = i;
                    break;
                }
            }
    }
    else {
        errorMessageOut = "Unable to find index of selected annotation in ordered annotations";
        return false;
    }
    
    return true;
}

/**
 * Sort the annotations front to back prior to applying the ordering operation
 *
 * @param annotationOrderAndContent
 *   Output with annotations and their ordering
 * @param errorMessageOut
 *   Contains error information
 * @return True if input is valid, else false.
 */
bool
AnnotationStackingOrderOperation::preOrderAnnotations(std::vector<OrderInfo>& annotationOrderAndContent,
                                                   AString& errorMessageOut)
{
    /*
     * Sort annotations by current stacking order
     */
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
                 * Coords are float but stack order is integer so scale the coord
                 */
                ann->setStackingOrder(static_cast<int32_t>(xyz[2] * m_coordToStackOrderScaleFactor));
            }
                break;
        }
        
        bool overlapsSelectedAnnotationFlag(false);
        if (ann != m_selectedAnnotation) {
            if (m_selectedAnnotation->intersectionTest(ann, m_windowIndex)) {
                overlapsSelectedAnnotationFlag = true;
            }
        }
        annotationOrderAndContent.emplace_back(ann,
                                               ann->getStackingOrder(),
                                               overlapsSelectedAnnotationFlag);
    }
    
    if (annotationOrderAndContent.size() <= 1) {
        errorMessageOut = "There must be at least two annotations selected for ordering";
        return false;
    }

    
    std::sort(annotationOrderAndContent.begin(),
              annotationOrderAndContent.end());
    
    /*
     * Update order indices so that all are a multiple of 2.
     * Later, when an annotation is moved, its order can
     * simply be incremented or decremented to move in front or behind
     */
    int32_t orderIndex(0);
    for (auto& ao : annotationOrderAndContent) {
        ao.m_stackOrder = orderIndex;
        orderIndex += 2;
    }
    
    return true;
}

/**
 * Print the current ordering of the annotations
 */
void
AnnotationStackingOrderOperation::printOrderedAnnotations(const std::vector<OrderInfo>& annotationOrderAndContent)
{
    for (auto& ao : annotationOrderAndContent) {
        std::cout << ao.m_stackOrder << " " << AString::fromBool(ao.m_overlapsFlag) << " " << ao.m_annotation->toString() << std::endl;
    }
}

/**
 * Validate the input to verify space of selected annotation is supported for
 * reordering and to filter out annoations in other spaces
 *
 * @param errorMessageOut
 * Contains error information
 * @return True if input is valid, else false.
 */
bool
AnnotationStackingOrderOperation::validateInput(AString& errorMessageOut)
{
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
    
    if ( ! filterAnnotationsBySpace()) {
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
                           + " not found in input annotations");
        return false;
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
 * Filter the annotations to find those in same space of the selected annotation
 *
 * @return True there are annotations remaining after filtering
 */
bool
AnnotationStackingOrderOperation::filterAnnotationsBySpace()
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
        for (auto ann : m_annotations) {
            CaretAssert(ann);
            
            if (m_selectedAnnotation->isInSameCoordinateSpace(ann)) {
                if (m_selectedAnnotation == ann) {
                    filteredAnnotations.push_back(ann);
                }
                else {
                    filteredAnnotations.push_back(ann);
                }
            }
        }
    }
    
    
    m_annotations = filteredAnnotations;
    
    return (m_annotations.size() > 1);
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


