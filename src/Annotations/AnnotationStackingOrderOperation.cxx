
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
 * @param annotations
 *     The annotations that are reordered
 * @param selectedAnnotation
 *     The selected annotation that causes reordering
 */
AnnotationStackingOrderOperation::AnnotationStackingOrderOperation(const std::vector<Annotation*>& annotations,
                                                                   const Annotation* selectedAnnotation)
: CaretObject(),
m_annotations(annotations),
m_selectedAnnotation(selectedAnnotation)
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
AnnotationStackingOrderOperation::runOrdering(const OrderType orderType,
                                              AString& errorMessageOut)
{
    errorMessageOut.clear();
    if (m_annotations.size() <= 1) {
        return true;
    }

    CaretAssert(m_selectedAnnotation);
    
    bool found(false);
    for (auto ann : m_annotations) {
        CaretAssert(ann);
        if (ann == m_selectedAnnotation) {
            found = true;
            break;
        }
    }
    if ( ! found) {
        errorMessageOut = ("Annotation for reordering"
                           + m_selectedAnnotation->toString()
                           + " not found all annotations");
        return false;
    }
    
    if ( ! validateCompatibility(m_annotations,
                                 errorMessageOut)) {
        return false;
    }
    
    /*
     * Sort annotations by current stacking order
     */
    std::vector<OrderInfo> annotationOrderAndContent;
    for (const auto ann : m_annotations) {
        CaretAssert(ann);
        
        annotationOrderAndContent.emplace_back(ann,
                                        ann->getStackingOrder());
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
    CaretAssert(numberOfAnnotations > 2);
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
            if (m_selectedAnnotation->intersectionTest(annotationOrderAndContent[i].m_annotation)) {
                indexOfAnnotationBehind = i;
            }
        }
        else if (indexOfAnnotationInFront < 0) {
            /*
             * Find first (closest) annotation that is in front of and intersects 'selectedAnnotation'
             */
            if (m_selectedAnnotation->intersectionTest(annotationOrderAndContent[i].m_annotation)) {
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
        case OrderType::BRING_FORWARD:
            if (indexOfAnnotationInFront >= 0) {
                /*
                 * Move forward one position
                 */
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfAnnotationInFront);
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
                annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[indexOfAnnotationInFront].m_stackOrder + 1;
            }
            break;
        case OrderType::BRING_TO_FRONT:
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
        case OrderType::SEND_BACKWARD:
            if (indexOfAnnotationBehind >= 0) {
                /*
                 * Move back one position
                 */
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfAnnotationBehind);
                CaretAssertVectorIndex(annotationOrderAndContent, indexOfSelectedAnnotation);
                annotationOrderAndContent[indexOfSelectedAnnotation].m_stackOrder = annotationOrderAndContent[indexOfAnnotationBehind].m_stackOrder - 1;
            }
            break;
        case OrderType::SEND_TO_BACK:
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
        annotationOrderAndContent[i].m_annotation->setStackingOrder(i + 1);
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
 * Examine the annotations (types, coordinate spaces, etc) to verify that
 * the annotations can be reordered.
 */
bool
AnnotationStackingOrderOperation::validateCompatibility(const std::vector<Annotation*>& annotations,
                                                        AString& errorMesssageOut)
{
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

