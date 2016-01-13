
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
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "CaretException.h"
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
 * Apply alignment modification to selected annotations
 *
 * @param annotationManager
 *     The annotation manager.
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
    
    errorMessageOut.clear();
    
    try {
        alignAnnotationsPrivate(arrangerInputs);
    }
    catch (const CaretException& caretException) {
        errorMessageOut = caretException.whatString();
        return false;
    }
    
    return true;
}

/**
 * Apply alignment modification to selected annotations
 *
 * @param arrangerInputs
 *     Contains information about the alignment.
 * @throw  CaretException
 *     If there is an error.
 */
void
AnnotationArrangerExecutor::alignAnnotationsPrivate(const AnnotationArrangerInputs& arrangerInputs)
{
    std::vector<AnnotationCoordinateSpaceEnum::Enum> spaces;
    spaces.push_back(AnnotationCoordinateSpaceEnum::TAB);
    spaces.push_back(AnnotationCoordinateSpaceEnum::WINDOW);
    
    const std::vector<Annotation*> allSpaceAnnotations = m_annotationManager->getSelectedAnnotations(arrangerInputs.getWindowIndex());
    std::vector<Annotation*> annotations = m_annotationManager->getSelectedAnnotationsInSpaces(arrangerInputs.getWindowIndex(),
                                                                                               spaces);
    if (allSpaceAnnotations.size() != annotations.size()) {
        QString spaceString;
        for (std::vector<AnnotationCoordinateSpaceEnum::Enum>::iterator spaceIter = spaces.begin();
             spaceIter != spaces.end();
             spaceIter++) {
            spaceString += ("\n   " + AnnotationCoordinateSpaceEnum::toGuiName(*spaceIter) + " ");
        }
        
        throw CaretException("Annotations are selected that cannot be aligned due to their coordinate space.  "
                             "Annotations MUST BE in one of these spaces:"
                             + spaceString);
    }
    
    const int32_t numAnnotations = static_cast<int32_t>(annotations.size());
    if (numAnnotations < 1) {
        throw CaretException("No annotation are selected.");
    }
    if (numAnnotations < 2) {
        std::cout << "Less than two annotations selected, need to throw exception" << std::endl;
        //throw CaretException("At least two annotations must be selected for alignment.");
    }
    
    EventGetViewportSize vpEvent(EventGetViewportSize::MODE_WINDOW_INDEX,
                                 arrangerInputs.getWindowIndex());
    EventManager::get()->sendEvent(vpEvent.getPointer());
    if (vpEvent.isViewportSizeValid()) {
        vpEvent.getViewportSize(m_windowViewport);
    }
    else {
        throw CaretException("Failed to get viewport size for window index "
                             + QString::number(arrangerInputs.getWindowIndex() + 1));
    }
    
    setupAnnotationInfo(arrangerInputs);

    BoundingBox allBoundingBox;
    allBoundingBox.resetForUpdate();
    
    for (std::vector<AnnotationInfo>::iterator annInfoIter = m_annotationInfo.begin();
         annInfoIter != m_annotationInfo.end();
         annInfoIter++) {
        const AnnotationInfo& annInfo = *annInfoIter;
        
        allBoundingBox.update(annInfo.m_windowBoundingBox.getMinXYZ());
        allBoundingBox.update(annInfo.m_windowBoundingBox.getMaxXYZ());
    }

    printAnnotationInfo("BEFORE");
    
    const AnnotationAlignmentEnum::Enum alignmentType = arrangerInputs.getAlignment();
    
    
    float alignToValue = 0.0;
    switch (alignmentType) {
        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
            alignToValue = allBoundingBox.getMinY();
            break;
        case AnnotationAlignmentEnum::ALIGN_CENTER:
            alignToValue = (allBoundingBox.getMinX()
                               + allBoundingBox.getMaxX()) / 2.0;
            break;
        case AnnotationAlignmentEnum::ALIGN_LEFT:
            alignToValue = allBoundingBox.getMinX();
            break;
        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
            alignToValue = (allBoundingBox.getMinY()
                               + allBoundingBox.getMaxY()) / 2.0;
            break;
        case AnnotationAlignmentEnum::ALIGN_RIGHT:
            alignToValue = allBoundingBox.getMaxX();
            break;
        case AnnotationAlignmentEnum::ALIGN_TOP:
            alignToValue = allBoundingBox.getMaxY();
            break;
    }
    
    std::cout << "Bounding box for all: " << qPrintable(allBoundingBox.toString()) << std::endl;
    std::cout << "New Align to value: " << alignToValue << std::endl;
    
    std::vector<Annotation*> beforeMoving;
    std::vector<Annotation*> afterMoving;
    for (std::vector<AnnotationInfo>::iterator annInfoIter = m_annotationInfo.begin();
         annInfoIter != m_annotationInfo.end();
         annInfoIter++) {
        
        AnnotationInfo& annInfo = *annInfoIter;
        alignAnnotationToValue(arrangerInputs,
                               alignToValue,
                               annInfo,
                               beforeMoving,
                               afterMoving);
        
    }
    
    if ( ! beforeMoving.empty()) {
        CaretAssert(beforeMoving.size() == afterMoving.size());
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLocationAndSize(beforeMoving,
                                            afterMoving);
        
        m_annotationManager->applyCommand(undoCommand);
    }
    
//    /*
//     * For debugging
//     */
//    setupAnnotationInfo(arrangerInputs);
//    printAnnotationInfo("AFTER");
}

/**
 * Setup the annotation information.
 *
 * @param arrangerInputs
 *     Inputs for arranging annotations.
 */
void
AnnotationArrangerExecutor::setupAnnotationInfo(const AnnotationArrangerInputs& arrangerInputs)
{
    m_annotationInfo.clear();

    std::vector<AnnotationCoordinateSpaceEnum::Enum> spaces;
    spaces.push_back(AnnotationCoordinateSpaceEnum::TAB);
    spaces.push_back(AnnotationCoordinateSpaceEnum::WINDOW);
    
    std::vector<Annotation*> annotations = m_annotationManager->getSelectedAnnotationsInSpaces(arrangerInputs.getWindowIndex(),
                                                                                               spaces);
    for (std::vector<Annotation*>::iterator annIter = annotations.begin();
         annIter != annotations.end();
         annIter++) {
        Annotation* annotation = *annIter;
        CaretAssert(annotation);
        
        int32_t annViewport[4] = { 0, 0, 0, 0 };
        switch (annotation->getCoordinateSpace()) {
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
                getTabViewport(annotation->getTabIndex(),
                               annViewport);
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                annViewport[0] = m_windowViewport[0];
                annViewport[1] = m_windowViewport[1];
                annViewport[2] = m_windowViewport[2];
                annViewport[3] = m_windowViewport[3];
                break;
        }
        
        const AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<const AnnotationOneDimensionalShape*>(annotation);
        const AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<const AnnotationTwoDimensionalShape*>(annotation);
        
        float viewportPixelOneXYZ[3] = { 0.0, 0.0, 0.0 };
        float viewportPixelTwoXYZ[3] = { 0.0, 0.0, 0.0 };
        
        BoundingBox windowBoundingBox;
        windowBoundingBox.resetForUpdate();
        
        if (oneDimAnn != NULL) {
            float xyzRel1[3];
            oneDimAnn->getStartCoordinate()->getXYZ(xyzRel1);
            float xyzRel2[3];
            oneDimAnn->getEndCoordinate()->getXYZ(xyzRel2);
            
            /*
             * Convert range [0.0, 100.0] to viewport XY.
             */
            Annotation::relativeXYZToViewportXYZ(xyzRel1,
                                                 annViewport[2],
                                                 annViewport[3],
                                                 viewportPixelOneXYZ);
            Annotation::relativeXYZToViewportXYZ(xyzRel2,
                                                 annViewport[2],
                                                 annViewport[3],
                                                 viewportPixelTwoXYZ);
            
            windowBoundingBox.update(annViewport[0] + viewportPixelOneXYZ[0],
                                     annViewport[1] + viewportPixelOneXYZ[1],
                                     viewportPixelOneXYZ[2]);
            windowBoundingBox.update(annViewport[0] + viewportPixelTwoXYZ[0],
                                     annViewport[1] + viewportPixelTwoXYZ[1],
                                     viewportPixelTwoXYZ[2]);
            
//            bool oneFlag = false;
//            bool twoFlag = false;
//            bool midFlag = false;
//            switch (arrangerInputs.getAlignment()) {
//                case AnnotationAlignmentEnum::ALIGN_BOTTOM:
//                {
//                    if (pixelXYZ1[1] < pixelXYZ2[1]) oneFlag = true;
//                    else twoFlag = true;
//                }
//                    break;
//                case AnnotationAlignmentEnum::ALIGN_CENTER:
//                {
//                    midFlag = true;
//                }
//                    break;
//                case AnnotationAlignmentEnum::ALIGN_LEFT:
//                {
//                    if (pixelXYZ1[0] < pixelXYZ2[0]) oneFlag = true;
//                    else twoFlag = true;
//                }
//                    break;
//                case AnnotationAlignmentEnum::ALIGN_MIDDLE:
//                {
//                    midFlag = true;
//                }
//                    break;
//                case AnnotationAlignmentEnum::ALIGN_RIGHT:
//                {
//                    if (pixelXYZ1[0] > pixelXYZ2[0]) oneFlag = true;
//                    else twoFlag = true;
//                }
//                    break;
//                case AnnotationAlignmentEnum::ALIGN_TOP:
//                {
//                    if (pixelXYZ1[1] > pixelXYZ2[1]) oneFlag = true;
//                    else twoFlag = true;
//                }
//                    break;
//            }
//            
//            if (oneFlag) {
//                viewportPixelOneXYZ[0] = pixelXYZ1[0];
//                viewportPixelOneXYZ[1] = pixelXYZ1[1];
//                viewportPixelXYZ[2] = pixelXYZ1[2];
//            }
//            else if (twoFlag) {
//                viewportPixelXYZ[0] = pixelXYZ2[0];
//                viewportPixelXYZ[1] = pixelXYZ2[1];
//                viewportPixelXYZ[2] = pixelXYZ2[2];
//            }
//            else if (midFlag) {
//                viewportPixelXYZ[0] = (pixelXYZ1[0] + pixelXYZ2[0]);
//                viewportPixelXYZ[1] = (pixelXYZ1[1] + pixelXYZ2[1]);
//                viewportPixelXYZ[2] = (pixelXYZ1[2] + pixelXYZ2[2]);
//            }
//            else {
//                CaretAssert(0);
//            }
        }
        else if (twoDimAnn != NULL) {
            float xyz[3];
            twoDimAnn->getCoordinate()->getXYZ(xyz);
            
            /*
             * Convert range [0.0, 100.0] to viewport XY.
             */
            Annotation::relativeXYZToViewportXYZ(xyz,
                                                 annViewport[2],
                                                 annViewport[3],
                                                 viewportPixelOneXYZ);
            
            float bottomLeft[3];
            float bottomRight[3];
            float topRight[3];
            float topLeft[3];
            
            bool boundsValidFlag = false;
            
            if (twoDimAnn->getType() == AnnotationTypeEnum::TEXT) {
                const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(twoDimAnn);
                CaretAssert(textAnn);
                arrangerInputs.getTextRender()->getBoundsForTextAtViewportCoords(*textAnn,
                                                                                 viewportPixelOneXYZ[0],
                                                                                 viewportPixelOneXYZ[1],
                                                                                 viewportPixelOneXYZ[2],
                                                                                 annViewport[3],
                                                                                 bottomLeft,
                                                                                 bottomRight,
                                                                                 topRight,
                                                                                 topLeft);
                boundsValidFlag = true;
            }
            else {
                boundsValidFlag = twoDimAnn->getShapeBounds(annViewport[2],
                                                            annViewport[3],
                                                            viewportPixelOneXYZ,
                                                            bottomLeft,
                                                            bottomRight,
                                                            topRight,
                                                            topLeft);
            }
            
            if (boundsValidFlag) {
                windowBoundingBox.update(annViewport[0] + bottomLeft[0],
                                         annViewport[1] + bottomLeft[1],
                                         bottomLeft[2]);
                windowBoundingBox.update(annViewport[0] + bottomRight[0],
                                         annViewport[1] + bottomRight[1],
                                         bottomRight[2]);
                windowBoundingBox.update(annViewport[0] + topRight[0],
                                         annViewport[1] + topRight[1],
                                         topRight[2]);
                windowBoundingBox.update(annViewport[0] + topLeft[0],
                                         annViewport[1] + topLeft[1],
                                         topLeft[2]);
            }
            else {
                throw CaretException("Failed to get bounds for an annotation: "
                                     + twoDimAnn->toString());
            }
        }
        
        AnnotationInfo annInfo(annotation,
                               annViewport,
                               windowBoundingBox,
                               viewportPixelOneXYZ,
                               viewportPixelTwoXYZ);
        m_annotationInfo.push_back(annInfo);
    }
    
    CaretAssert(annotations.size() == m_annotationInfo.size());
}

/**
 * Print the annotation info.
 */
void
AnnotationArrangerExecutor::printAnnotationInfo(const QString& title)
{
    std::cout << std::endl;
    std::cout << qPrintable(title) << std::endl;
    for (std::vector<AnnotationInfo>::iterator annInfoIter = m_annotationInfo.begin();
         annInfoIter != m_annotationInfo.end();
         annInfoIter++) {
        const AnnotationInfo& annInfo = *annInfoIter;
        annInfo.print();
    }
    std::cout << std::endl;
}


/**
 * Align the annotations to the given value.
 *
 * @param arrangerInputs
 *     Contains information about the alignment.
 * @param alignToWindowValue
 *     Align to value.
 * @param annotationInfo
 *     Information for annotation that is aligned.
 * @return 
 *     True if the annotation was moved, else false.
 * @throw  CaretException
 *     If there is an error.
 */
void
AnnotationArrangerExecutor::alignAnnotationToValue(const AnnotationArrangerInputs& arrangerInputs,
                                                   const float alignToWindowValue,
                                                   AnnotationInfo& annotationInfo,
                                                   std::vector<Annotation*>& annotationsBeforeMoving,
                                                   std::vector<Annotation*>& annotationsAfterMoving)
{
    float dx = 0.0;
    float dy = 0.0;
    
    
    switch (arrangerInputs.getAlignment()) {
        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
            dy = alignToWindowValue - annotationInfo.m_windowBoundingBox.getMinY();
            break;
        case AnnotationAlignmentEnum::ALIGN_CENTER:
            dx = alignToWindowValue - (annotationInfo.m_windowBoundingBox.getMinX()
                                       + annotationInfo.m_windowBoundingBox.getMaxX()) / 2.0;
            break;
        case AnnotationAlignmentEnum::ALIGN_LEFT:
            dx = alignToWindowValue - annotationInfo.m_windowBoundingBox.getMinX();
            break;
        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
            dy = alignToWindowValue - (annotationInfo.m_windowBoundingBox.getMinY()
                                       + annotationInfo.m_windowBoundingBox.getMaxY()) / 2.0;
            break;
        case AnnotationAlignmentEnum::ALIGN_RIGHT:
            dx = alignToWindowValue - annotationInfo.m_windowBoundingBox.getMaxX();
            break;
        case AnnotationAlignmentEnum::ALIGN_TOP:
            dy = alignToWindowValue - annotationInfo.m_windowBoundingBox.getMaxY();
            break;
    }
    std::cout << "Moving " << qPrintable(annotationInfo.m_annotation->toString()) << " by dx=" << dx << ", dy=" << dy << std::endl;
    

    Annotation* annotationModified = annotationInfo.m_annotation->clone();
    AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(annotationModified);
    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotationModified);
    
    bool modifiedFlag = false;
    
    if (dy != 0.0) {
//            const float windowY   = dy + annotationInfo.m_windowPixelOneXY[1];
//            const float vpY       = windowY - annotationInfo.m_viewport[1];
//            const float relativeY = (vpY / annotationInfo.m_viewport[3]) * 100.0;
        const float windowOneY   = dy + annotationInfo.m_windowPixelOneXY[1];
        const float vpOneY       = windowOneY - annotationInfo.m_viewport[1];
        const float relativeOneY = (vpOneY / annotationInfo.m_viewport[3]) * 100.0;
        if (twoDimAnn != NULL) {
            if ((relativeOneY >= 0.0)
                && (relativeOneY <= 100.0)) {
                float xyz[3];
                twoDimAnn->getCoordinate()->getXYZ(xyz);
                xyz[1] = relativeOneY;
                twoDimAnn->getCoordinate()->setXYZ(xyz);
                modifiedFlag = true;
            }
        }
        else if (oneDimAnn != NULL) {
            const float windowTwoY   = dy + annotationInfo.m_windowPixelTwoXY[1];
            const float vpTwoY       = windowTwoY - annotationInfo.m_viewport[1];
            const float relativeTwoY = (vpTwoY / annotationInfo.m_viewport[3]) * 100.0;
            if ((relativeOneY >= 0.0)
                && (relativeOneY <= 100.0)
                && (relativeTwoY >= 0.0)
                && (relativeTwoY <= 100.0)) {
                float xyz[3];
                oneDimAnn->getStartCoordinate()->getXYZ(xyz);
                xyz[1] = relativeOneY;
                oneDimAnn->getStartCoordinate()->setXYZ(xyz);

                oneDimAnn->getEndCoordinate()->getXYZ(xyz);
                xyz[1] = relativeTwoY;
                oneDimAnn->getEndCoordinate()->setXYZ(xyz);
                modifiedFlag = true;
            }

//            const float windowOneY   = dy + annotationInfo.m_windowPixelOneXY[1];
//            const float vpOneY       = windowOneY - annotationInfo.m_viewport[1];
//            const float relativeOneY = (vpOneY / annotationInfo.m_viewport[3]) * 100.0;
//            if ((relativeOneY >= 0.0)
//                && (relativeOneY <= 100.0)) {
//                float xyz[3];
//                oneDimAnn->getStartCoordinate()->getXYZ(xyz);
//                xyz[1] = relativeOneY;
//                oneDimAnn->getStartCoordinate()->setXYZ(xyz);
//                modifiedFlag = true;
//            }
        }
    }
    
    if (dx != 0.0) {
        const float windowOneX   = dx + annotationInfo.m_windowPixelOneXY[0];
        const float vpOneX       = windowOneX - annotationInfo.m_viewport[0];
        const float relativeOneX = (vpOneX / annotationInfo.m_viewport[2]) * 100.0;
        
        if (twoDimAnn != NULL) {
            if ((relativeOneX >= 0.0)
                && (relativeOneX <= 100.0)) {
                float xyz[3];
                twoDimAnn->getCoordinate()->getXYZ(xyz);
                xyz[0] = relativeOneX;
                twoDimAnn->getCoordinate()->setXYZ(xyz);
                modifiedFlag = true;
            }
        }
        else if (oneDimAnn != NULL) {
            const float windowTwoX   = dx + annotationInfo.m_windowPixelTwoXY[0];
            const float vpTwoX       = windowTwoX - annotationInfo.m_viewport[0];
            const float relativeTwoX = (vpTwoX / annotationInfo.m_viewport[2]) * 100.0;
            if ((relativeOneX >= 0.0)
                && (relativeOneX <= 100.0)
                && (relativeTwoX >= 0.0)
                && (relativeTwoX <= 100.0)) {
                float xyz[3];
                oneDimAnn->getStartCoordinate()->getXYZ(xyz);
                xyz[0] = relativeOneX;
                oneDimAnn->getStartCoordinate()->setXYZ(xyz);
                
                oneDimAnn->getEndCoordinate()->getXYZ(xyz);
                xyz[0] = relativeTwoX;
                oneDimAnn->getEndCoordinate()->setXYZ(xyz);
                modifiedFlag = true;
            }
        }
    }
    
    if (modifiedFlag) {
        annotationsBeforeMoving.push_back(annotationInfo.m_annotation);
        annotationsAfterMoving.push_back(annotationModified);
    }
    else {
        delete annotationModified;
    }
}

/**
 * Get the viewport for the given tab.  Viewports are cached for efficiency.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param tabViewportOut
 *     Viewport of the tab.
 * @throw  CaretException
 *     If there is an error.
 */
void
AnnotationArrangerExecutor::getTabViewport(const int32_t tabIndex,
                                           int32_t tabViewportOut[4])
{
    
    std::map<int32_t, ViewportArray>::iterator iter = m_tabViewports.find(tabIndex);
    if (iter != m_tabViewports.end()) {
        tabViewportOut[0] = iter->second.m_viewport[0];
        tabViewportOut[1] = iter->second.m_viewport[1];
        tabViewportOut[2] = iter->second.m_viewport[2];
        tabViewportOut[3] = iter->second.m_viewport[3];
        return;
    }
    
    EventGetViewportSize vpEvent(EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX,
                                 tabIndex);
    EventManager::get()->sendEvent(vpEvent.getPointer());
    if (vpEvent.isViewportSizeValid()) {
        ViewportArray viewportArray;
        vpEvent.getViewportSize(tabViewportOut);
        
        viewportArray.m_viewport[0] = tabViewportOut[0];
        viewportArray.m_viewport[1] = tabViewportOut[1];
        viewportArray.m_viewport[2] = tabViewportOut[2];
        viewportArray.m_viewport[3] = tabViewportOut[3];
        m_tabViewports.insert(std::make_pair(tabIndex, viewportArray));
    }
    else {
        throw CaretException("Failed to get viewport size for tab index "
                             + QString::number(tabIndex + 1));
    }
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



AnnotationArrangerExecutor::AnnotationInfo::AnnotationInfo(Annotation* annotation,
                                                           const int32_t viewport[4],
                                                           const BoundingBox windowBoundingBox,
                                                           float viewportPixelOneXY[2],
                                                           float viewportPixelTwoXY[2])
: m_annotation(annotation)
{
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];
    
    m_viewportPixelOneXY[0] = viewportPixelOneXY[0];
    m_viewportPixelOneXY[1] = viewportPixelOneXY[1];
    
    m_windowBoundingBox = windowBoundingBox;
    
    m_windowPixelOneXY[0] = m_viewport[0] + viewportPixelOneXY[0];
    m_windowPixelOneXY[1] = m_viewport[1] + viewportPixelOneXY[1];
    
    m_windowPixelTwoXY[0] = m_viewport[0] + viewportPixelTwoXY[0];
    m_windowPixelTwoXY[1] = m_viewport[1] + viewportPixelTwoXY[1];
}


void
AnnotationArrangerExecutor::AnnotationInfo::print() const
{
    QString msg(m_annotation->toString()
                + "\n   Window One XY: "
                + AString::fromNumbers(m_windowPixelOneXY, 2, ",")
                + "\n   Window Two XY: "
                + AString::fromNumbers(m_windowPixelTwoXY, 2, ",")
                + "\n   Viewport XY: "
                + AString::fromNumbers(m_viewportPixelOneXY, 2, ",")
                + "\n   Viewport: "
                + AString::fromNumbers(m_viewport, 4, ",")
                + "\n   Bounds: "
                + m_windowBoundingBox.toString());
    std::cout << qPrintable(msg) << std::endl;
}