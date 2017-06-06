
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

#define __ANNOTATION_COORDINATE_INFORMATION_DECLARE__
#include "AnnotationCoordinateInformation.h"
#undef __ANNOTATION_COORDINATE_INFORMATION_DECLARE__

#include "Annotation.h"
#include "AnnotationCoordinate.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationTwoDimensionalShape.h"
#include "BrainOpenGLWidget.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventIdentificationRequest.h"
#include "EventManager.h"
#include "MouseEvent.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"

using namespace caret;



/**
 * \class caret::AnnotationCoordinateInformation
 * \brief Contains annotation coordinate types valid for a window coordinate
 * \ingroup GuiQt
 *
 * Annotations support several different coordinate systems.  This class
 * contains validity for each these different coordinate systems.
 */

/**
 * Constructor.
 */
AnnotationCoordinateInformation::AnnotationCoordinateInformation()
{
    reset();
}

/**
 * Destructor.
 */
AnnotationCoordinateInformation::~AnnotationCoordinateInformation()
{
}

/**
 * Reset all coordinate information to invalid.
 */
void
AnnotationCoordinateInformation::reset() {
    m_modelXYZValid    = false;
    m_surfaceNodeValid = false;
    m_surfaceStructure = StructureEnum::INVALID;
    m_surfaceNumberOfNodes = 0;
    m_surfaceNodeIndex = -1;
    m_surfaceNodeOffset = 0.0;
    m_surfaceNodeVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
    m_tabIndex         = -1;
    m_tabWidth         = 0;
    m_tabHeight        = 0;
    m_windowIndex      = -1;
    m_windowWidth      = 0;
    m_windowHeight     = 0;
    m_modelXYZ[0]  = 0.0;
    m_modelXYZ[1]  = 0.0;
    m_modelXYZ[2]  = 0.0;
    m_tabXYZ[0]    = 0.0;
    m_tabXYZ[1]    = 0.0;
    m_tabXYZ[2]    = 0.0;
    m_tabPixelXYZ[0]    = 0.0;
    m_tabPixelXYZ[1]    = 0.0;
    m_tabPixelXYZ[2]    = 0.0;
    m_windowXYZ[0] = 0.0;
    m_windowXYZ[1] = 0.0;
    m_windowXYZ[2] = 0.0;
    m_windowPixelXYZ[0] = 0.0;
    m_windowPixelXYZ[1] = 0.0;
    m_windowPixelXYZ[2] = 0.0;
}

bool
AnnotationCoordinateInformation::isCoordinateSpaceValid(const AnnotationCoordinateSpaceEnum::Enum space) const
{
    bool validSpaceFlag = false;
    
    switch (space) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            validSpaceFlag = m_modelXYZValid;
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            validSpaceFlag = m_surfaceNodeValid;
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            validSpaceFlag = (m_tabIndex >= 0);
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssertMessage(0, "Should not create/move viewport annotations");
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            validSpaceFlag = (m_windowIndex >= 0);
            break;
    }
    
    return validSpaceFlag;
}

/**
 * Get the valid coordinate spaces for the two annotation coordinate information.
 * If both coordinates are valid, the space must be valid for both coordinates, AND, if tab or window
 * space the tab or window indices must also be the same.
 *
 * @param coordInfoOne
 *     First coordinate information.
 * @param coordInfoTwo
 *     Second coordinate information (optional, NULL if not valid).
 * @param spacesOut
 *     Output containing spaces valid for both.
 */
void
AnnotationCoordinateInformation::getValidCoordinateSpaces(const AnnotationCoordinateInformation* coordInfoOne,
                                                          const AnnotationCoordinateInformation* coordInfoTwo,
                                                          std::vector<AnnotationCoordinateSpaceEnum::Enum>& spacesOut)
{
    CaretAssert(coordInfoOne);
    spacesOut.clear();
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> allSpaces;
    AnnotationCoordinateSpaceEnum::getAllEnums(allSpaces);
    
    for (std::vector<AnnotationCoordinateSpaceEnum::Enum>::iterator spaceIter = allSpaces.begin();
         spaceIter != allSpaces.end();
         spaceIter++) {
        const AnnotationCoordinateSpaceEnum::Enum space = *spaceIter;
        
        switch (space) {
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            case AnnotationCoordinateSpaceEnum::SURFACE:
            case AnnotationCoordinateSpaceEnum::TAB:
            case AnnotationCoordinateSpaceEnum::WINDOW:
                /*
                 * See if space for first coord info is valid
                 */
                if (coordInfoOne->isCoordinateSpaceValid(space)) {
                    bool addItFlag = true;
                    if (coordInfoTwo != NULL) {
                        /*
                         * See if same space is valid for second coord info
                         */
                        addItFlag = coordInfoTwo->isCoordinateSpaceValid(space);
                        
                        switch (space) {
                            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                                /*
                                 * Both coord info's must be in the SAME TAB
                                 */
                                if (coordInfoOne->m_tabIndex != coordInfoTwo->m_tabIndex) {
                                    addItFlag = false;
                                }
                            case AnnotationCoordinateSpaceEnum::SURFACE:
                                /*
                                 * Both coord info's must be on same surface and
                                 * in the SAME TAB
                                 */
                                if ((coordInfoOne->m_tabIndex != coordInfoTwo->m_tabIndex)
                                    || (coordInfoOne->m_surfaceNumberOfNodes != coordInfoTwo->m_surfaceNumberOfNodes)
                                    || (coordInfoOne->m_surfaceStructure != coordInfoTwo->m_surfaceStructure)) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::TAB:
                                if (coordInfoOne->m_tabIndex != coordInfoTwo->m_tabIndex) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::VIEWPORT:
//                                if (coordInfoOne->m_windowIndex != coordInfoTwo->m_windowIndex) {
//                                    addItFlag = false;
//                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::WINDOW:
                                if (coordInfoOne->m_windowIndex != coordInfoTwo->m_windowIndex) {
                                    addItFlag = false;
                                }
                                break;
                        }
                    }
                    
                    if (addItFlag) {
                        spacesOut.push_back(space);
                    }
                }
                break;
        }
    }
}


/**
 * Get the different types of coordinates at the given mouse location.
 *
 * @param mouseEvent
 *     Mouse event containing mouse location information.
 * @param coordInfoOut
 *     Output containing coordinate information.
 */
void
AnnotationCoordinateInformation::createCoordinateInformationFromXY(const MouseEvent& mouseEvent,
                                                                AnnotationCoordinateInformation& coordInfoOut)
{
    createCoordinateInformationFromXY(mouseEvent.getOpenGLWidget(),
                                   mouseEvent.getViewportContent(),
                                   mouseEvent.getX(),
                                   mouseEvent.getY(),
                                   coordInfoOut);
}

/**
 * Get the different types of coordinates at the given mouse location.
 *
 * @param mouseEvent
 *     Mouse event containing mouse location information.
 * @param windowX
 *     X-coordinate in the window.
 * @param windowY
 *     Y-coordinate in the window.
 * @param coordInfoOut
 *     Output containing coordinate information.
 */
void
AnnotationCoordinateInformation::createCoordinateInformationFromXY(const MouseEvent& mouseEvent,
                                                                   const int32_t windowX,
                                                                   const int32_t windowY,
                                                                   AnnotationCoordinateInformation& coordInfoOut)
{
    createCoordinateInformationFromXY(mouseEvent.getOpenGLWidget(),
                                      mouseEvent.getViewportContent(),
                                      windowX,
                                      windowY,
                                      coordInfoOut);
}

/**
 * Get the different types of coordinates at the given mouse location.
 *
 * @param openGLWidget
 *     The OpenGL Widget.
 * @param viewportContent
 *     The content of the viewport.
 * @param windowX
 *     X-coordinate in the window.
 * @param windowY
 *     Y-coordinate in the window.
 * @param coordInfoOut
 *     Output containing coordinate information.
 */
void
AnnotationCoordinateInformation::createCoordinateInformationFromXY(BrainOpenGLWidget* openGLWidget,
                                                                BrainOpenGLViewportContent* viewportContent,
                                                                const int32_t windowX,
                                                                const int32_t windowY,
                                                                AnnotationCoordinateInformation& coordInfoOut)
{
    coordInfoOut.reset();
    
    SelectionManager* idManager =
    openGLWidget->performIdentification(windowX,
                                        windowY,
                                        false);
    
    SelectionItemVoxel* voxelID = idManager->getVoxelIdentification();
    SelectionItemSurfaceNode*  surfaceNodeIdentification = idManager->getSurfaceNodeIdentification();
    if (surfaceNodeIdentification->isValid()) {
        surfaceNodeIdentification->getModelXYZ(coordInfoOut.m_modelXYZ);
        coordInfoOut.m_modelXYZValid = true;
        
        const Surface* surface = surfaceNodeIdentification->getSurface();
        CaretAssert(surface);
        coordInfoOut.m_surfaceNumberOfNodes = surface->getNumberOfNodes();
        coordInfoOut.m_surfaceStructure     = surface->getStructure();
        coordInfoOut.m_surfaceNodeIndex     = surfaceNodeIdentification->getNodeNumber();
        coordInfoOut.m_surfaceNodeOffset    = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
        coordInfoOut.m_surfaceNodeVector    = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
        coordInfoOut.m_surfaceNodeValid     = true;
    }
    else if (voxelID->isValid()) {
        voxelID->getModelXYZ(coordInfoOut.m_modelXYZ);
        coordInfoOut.m_modelXYZValid = true;
    }
    
    /*
     * In tile tabs, some regions may not contain a tab such
     * as three tabs in a two-by-two configuration
     * or if the user has clicked in a margin
     */
    BrowserTabContent* tabContent = viewportContent->getBrowserTabContent();
    if (tabContent != NULL) {
        int tabViewport[4];
        viewportContent->getModelViewport(tabViewport);
        const float tabX = 100.0 * (windowX - tabViewport[0]) / static_cast<float>(tabViewport[2]);
        const float tabY = 100.0 * (windowY - tabViewport[1]) / static_cast<float>(tabViewport[3]);
        if ((tabX >= 0.0)
            && (tabX < 100.0)
            && (tabY >= 0.0)
            && (tabY <= 100.0)) {
            coordInfoOut.m_tabXYZ[0] = tabX;
            coordInfoOut.m_tabXYZ[1] = tabY;
            coordInfoOut.m_tabXYZ[2] = 0.0;
            coordInfoOut.m_tabPixelXYZ[0] = (windowX - tabViewport[0]);
            coordInfoOut.m_tabPixelXYZ[1] = (windowY - tabViewport[1]);
            coordInfoOut.m_tabPixelXYZ[2] = 0;
            coordInfoOut.m_tabIndex  = tabContent->getTabNumber();
            coordInfoOut.m_tabWidth  = tabViewport[2];
            coordInfoOut.m_tabHeight = tabViewport[3];
        }
    }
    
    int windowViewport[4];
    viewportContent->getWindowViewport(windowViewport);
    coordInfoOut.m_windowPixelXYZ[0] = windowX - windowViewport[0];
    coordInfoOut.m_windowPixelXYZ[1] = windowY - windowViewport[1];
    coordInfoOut.m_windowPixelXYZ[2] = 0.0;
    coordInfoOut.m_windowIndex  = viewportContent->getWindowIndex();
    coordInfoOut.m_windowWidth  = windowViewport[2];
    coordInfoOut.m_windowHeight = windowViewport[3];
    
    /*
     * Normalize window coordinates (width and height range [0, 100]
     */
    coordInfoOut.m_windowXYZ[0] = 100.0 * (coordInfoOut.m_windowPixelXYZ[0] / windowViewport[2]);
    coordInfoOut.m_windowXYZ[1] = 100.0 * (coordInfoOut.m_windowPixelXYZ[1] / windowViewport[3]);
    coordInfoOut.m_windowXYZ[2] = 0.0;
}

/**
 * Set the coordinates for the annotation.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @parm coordInfoOne
 *     Data for the first coordinate.
 * @parm coordInfoTwo
 *     Data for the second coordinate.
 */
bool
AnnotationCoordinateInformation::setAnnotationCoordinatesForSpace(Annotation* annotation,
                                                                  const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                                  const AnnotationCoordinateInformation* coordInfoOne,
                                                                  const AnnotationCoordinateInformation* coordInfoTwo)
{
    CaretAssert(annotation);
    
    bool validCoordinateFlag = false;
    
    AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
    
    if (oneDimAnn != NULL) {
        validCoordinateFlag = setOneDimAnnotationCoordinatesForSpace(oneDimAnn,
                                                                     coordinateSpace,
                                                                     coordInfoOne,
                                                                     coordInfoTwo);
    }
    else if (twoDimAnn != NULL) {
        validCoordinateFlag = setTwoDimAnnotationCoordinatesForSpace(twoDimAnn,
                                                                     coordinateSpace,
                                                                     coordInfoOne,
                                                                     coordInfoTwo);
    }
    
    return validCoordinateFlag;
}

/**
 * Set the coordinates for the one-dimensional annotation.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @parm coordInfoOne
 *     Data for the first coordinate.
 * @parm coordInfoTwo
 *     Data for the second coordinate.
 */
bool
AnnotationCoordinateInformation::setOneDimAnnotationCoordinatesForSpace(AnnotationOneDimensionalShape* annotation,
                                                                        const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                                        const AnnotationCoordinateInformation* coordInfoOne,
                                                                        const AnnotationCoordinateInformation* coordInfoTwo)
{
    bool validCoordinateFlag = false;
    
    CaretAssert(annotation);
    CaretAssert(coordInfoOne);
    
    AnnotationCoordinate* startCoordinate = annotation->getStartCoordinate();
    CaretAssert(startCoordinate);
    AnnotationCoordinate* endCoordinate   = annotation->getEndCoordinate();
    CaretAssert(endCoordinate);
    
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (coordInfoOne->m_modelXYZValid) {
                startCoordinate->setXYZ(coordInfoOne->m_modelXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_modelXYZValid) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_modelXYZ);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (coordInfoOne->m_surfaceNodeValid) {
                const float surfaceOffsetLength = startCoordinate->getSurfaceOffsetLength();
                const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = startCoordinate->getSurfaceOffsetVectorType();
                startCoordinate->setSurfaceSpace(coordInfoOne->m_surfaceStructure,
                                                 coordInfoOne->m_surfaceNumberOfNodes,
                                                 coordInfoOne->m_surfaceNodeIndex,
                                                 surfaceOffsetLength,
                                                 surfaceOffsetVector);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_surfaceNodeValid) {
                        if (endCoordinate != NULL) {
                            const float surfaceOffsetLength = endCoordinate->getSurfaceOffsetLength();
                            endCoordinate->setSurfaceSpace(coordInfoTwo->m_surfaceStructure,
                                                           coordInfoTwo->m_surfaceNumberOfNodes,
                                                           coordInfoTwo->m_surfaceNodeIndex,
                                                           surfaceOffsetLength,
                                                           surfaceOffsetVector);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (coordInfoOne->m_tabIndex >= 0) {
                startCoordinate->setXYZ(coordInfoOne->m_tabXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(coordInfoOne->m_tabIndex);
                
                validCoordinateFlag = true;
                
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_tabIndex >= 0) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_tabXYZ);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_tabXYZ[0],
                        coordInfoOne->m_tabXYZ[1],
                        coordInfoOne->m_tabXYZ[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                        endCoordinate->setXYZ(xyz);
                    }
                    else {
                        xyz[1] += 25.0;
                        endCoordinate->setXYZ(coordInfoOne->m_tabXYZ);
                        startCoordinate->setXYZ(xyz);
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (coordInfoOne->m_windowIndex >= 0) {
                startCoordinate->setXYZ(coordInfoOne->m_windowXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(coordInfoOne->m_windowIndex);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_windowIndex >= 0) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_windowXYZ);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_windowXYZ[0],
                        coordInfoOne->m_windowXYZ[1],
                        coordInfoOne->m_windowXYZ[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                        endCoordinate->setXYZ(xyz);
                    }
                    else {
                        xyz[1] += 25.0;
                        endCoordinate->setXYZ(coordInfoOne->m_windowXYZ);
                        startCoordinate->setXYZ(xyz);
                    }
                }
            }
            break;
    }
    
    return validCoordinateFlag;
}

/**
 * Set the coordinates for the two-dimensional annotation.
 * If both coordinates are valid (not NULL), the annotation is
 * placed at the average of the two coordinates.  Otherwise,
 * the annotation is placed at the first coordinate.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @parm coordInfoOne
 *     Data for the first coordinate.
 * @parm optionalCoordInfoTwo
 *     Data for the optional second coordinate.
 */
bool
AnnotationCoordinateInformation::setTwoDimAnnotationCoordinatesForSpace(AnnotationTwoDimensionalShape* annotation,
                                                                        const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                                        const AnnotationCoordinateInformation* coordInfoOne,
                                                                        const AnnotationCoordinateInformation* optionalCoordInfoTwo)
{
    bool validCoordinateFlag = false;
    
    CaretAssert(annotation);
    CaretAssert(coordInfoOne);
    
    bool setWidthHeightWithTabCoordsFlag    = false;
    bool setWidthHeightWithWindowCoordsFlag = false;
    
    AnnotationCoordinate* coordinate = annotation->getCoordinate();
    
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (coordInfoOne->m_modelXYZValid) {
                coordinate->setXYZ(coordInfoOne->m_modelXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_modelXYZValid) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_modelXYZ[0] + optionalCoordInfoTwo->m_modelXYZ[0]) / 2.0,
                            (coordInfoOne->m_modelXYZ[1] + optionalCoordInfoTwo->m_modelXYZ[1]) / 2.0,
                            (coordInfoOne->m_modelXYZ[2] + optionalCoordInfoTwo->m_modelXYZ[2]) / 2.0
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (coordInfoOne->m_surfaceNodeValid) {
                coordinate->setSurfaceSpace(coordInfoOne->m_surfaceStructure,
                                            coordInfoOne->m_surfaceNumberOfNodes,
                                            coordInfoOne->m_surfaceNodeIndex);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if ((optionalCoordInfoTwo->m_surfaceNodeValid)
                        && (optionalCoordInfoTwo->m_surfaceStructure == coordInfoOne->m_surfaceStructure)) {
                        if ((optionalCoordInfoTwo->m_windowIndex == coordInfoOne->m_windowIndex)
                            && (coordInfoOne->m_windowIndex >= 0)) {
                            const float windowWidth  = coordInfoOne->m_windowWidth;
                            const float windowHeight = coordInfoOne->m_windowHeight;
                            const float x1 = coordInfoOne->m_windowXYZ[0] * windowWidth;
                            const float y1 = coordInfoOne->m_windowXYZ[1] * windowHeight;
                            const float x2 = optionalCoordInfoTwo->m_windowXYZ[0] * windowWidth;
                            const float y2 = optionalCoordInfoTwo->m_windowXYZ[1] * windowHeight;
                            const int32_t windowX = static_cast<int32_t>((x1 + x2)) / 2.0;
                            const int32_t windowY = static_cast<int32_t>((y1 + y2)) / 2.0;
                            
                            EventIdentificationRequest idRequest(coordInfoOne->m_windowIndex,
                                                                 static_cast<int32_t>(windowX),
                                                                 static_cast<int32_t>(windowY));
                            EventManager::get()->sendEvent(idRequest.getPointer());
                            SelectionManager* sm = idRequest.getSelectionManager();
                            if (sm != NULL) {
                                const SelectionItemSurfaceNode* nodeID = sm->getSurfaceNodeIdentification();
                                CaretAssert(nodeID);
                                if (nodeID->isValid()) {
                                    if (nodeID->getSurface()->getStructure() == coordInfoOne->m_surfaceStructure) {
                                        coordinate->setSurfaceSpace(coordInfoOne->m_surfaceStructure,
                                                                    coordInfoOne->m_surfaceNumberOfNodes,
                                                                    nodeID->getNodeNumber());
                                        setWidthHeightWithTabCoordsFlag = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (coordInfoOne->m_tabIndex >= 0) {
                coordinate->setXYZ(coordInfoOne->m_tabXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(coordInfoOne->m_tabIndex);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_tabIndex == coordInfoOne->m_tabIndex) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_tabXYZ[0] + optionalCoordInfoTwo->m_tabXYZ[0]) / 2.0,
                            (coordInfoOne->m_tabXYZ[1] + optionalCoordInfoTwo->m_tabXYZ[1]) / 2.0,
                            (coordInfoOne->m_tabXYZ[2] + optionalCoordInfoTwo->m_tabXYZ[2]) / 2.0
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (coordInfoOne->m_windowIndex >= 0) {
                coordinate->setXYZ(coordInfoOne->m_windowXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(coordInfoOne->m_windowIndex);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_windowIndex == coordInfoOne->m_windowIndex) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_windowXYZ[0] + optionalCoordInfoTwo->m_windowXYZ[0]) / 2.0,
                            (coordInfoOne->m_windowXYZ[1] + optionalCoordInfoTwo->m_windowXYZ[1]) / 2.0,
                            (coordInfoOne->m_windowXYZ[2] + optionalCoordInfoTwo->m_windowXYZ[2]) / 2.0
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithWindowCoordsFlag = true;
                    }
                }
            }
            break;
    }
    
    if (setWidthHeightWithTabCoordsFlag) {
        if (coordInfoOne->m_tabIndex >= 0) {
            if (optionalCoordInfoTwo != NULL) {
                if (coordInfoOne->m_tabIndex == optionalCoordInfoTwo->m_tabIndex) {
                    const float tabWidth  = coordInfoOne->m_tabWidth;
                    const float tabHeight = coordInfoOne->m_tabHeight;
                    
                    const float oneXYZ[3] = {
                        coordInfoOne->m_tabXYZ[0],
                        coordInfoOne->m_tabXYZ[1],
                        coordInfoOne->m_tabXYZ[2]
                    };
                    const float twoXYZ[3] = {
                        optionalCoordInfoTwo->m_tabXYZ[0],
                        optionalCoordInfoTwo->m_tabXYZ[1],
                        optionalCoordInfoTwo->m_tabXYZ[2]
                    };
                    
                    annotation->setWidthAndHeightFromBounds(oneXYZ,
                                                            twoXYZ,
                                                            tabWidth,
                                                            tabHeight);
                }
            }
        }
    }
    else if (setWidthHeightWithWindowCoordsFlag) {
        if (coordInfoOne->m_windowIndex >= 0) {
            if (optionalCoordInfoTwo != NULL) {
                if (coordInfoOne->m_windowIndex == optionalCoordInfoTwo->m_windowIndex) {
                    const float windowWidth  = coordInfoOne->m_windowWidth;
                    const float windowHeight = coordInfoOne->m_windowHeight;
                    
                    const float oneXYZ[3] = {
                        coordInfoOne->m_windowXYZ[0],
                        coordInfoOne->m_windowXYZ[1],
                        coordInfoOne->m_windowXYZ[2]
                    };
                    const float twoXYZ[3] = {
                        optionalCoordInfoTwo->m_windowXYZ[0],
                        optionalCoordInfoTwo->m_windowXYZ[1],
                        optionalCoordInfoTwo->m_windowXYZ[2]
                    };
                    
                    annotation->setWidthAndHeightFromBounds(oneXYZ,
                                                            twoXYZ,
                                                            windowWidth,
                                                            windowHeight);
                }
            }
        }
    }
    
    return validCoordinateFlag;
}

