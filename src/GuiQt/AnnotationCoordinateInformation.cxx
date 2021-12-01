
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
#include "AnnotationClipboard.h"
#include "AnnotationCoordinate.h"
#include "AnnotationTwoCoordinateShape.h"
#include "AnnotationMultiCoordinateShape.h"
#include "AnnotationOneCoordinateShape.h"
#include "BrainOpenGLWidget.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventIdentificationRequest.h"
#include "EventManager.h"
#include "MediaFile.h"
#include "MouseEvent.h"
#include "SelectionItemMedia.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SpacerTabContent.h"
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

AnnotationCoordinateInformation::AnnotationCoordinateInformation(const AnnotationCoordinateInformation& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationCoordinateInformation(obj);
}

AnnotationCoordinateInformation&
AnnotationCoordinateInformation::operator=(const AnnotationCoordinateInformation& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationCoordinateInformation(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationCoordinateInformation::copyHelperAnnotationCoordinateInformation(const AnnotationCoordinateInformation& obj)
{
    m_modelSpaceInfo     = obj.m_modelSpaceInfo;
    m_tabSpaceInfo       = obj.m_tabSpaceInfo;
    m_windowSpaceInfo    = obj.m_windowSpaceInfo;
    m_chartSpaceInfo     = obj.m_chartSpaceInfo;
    m_surfaceSpaceInfo   = obj.m_surfaceSpaceInfo;
    m_spacerTabSpaceInfo = obj.m_spacerTabSpaceInfo;
    m_mediaSpaceInfo     = obj.m_mediaSpaceInfo;
}


/**
 * Reset all coordinate information to invalid.
 */
void
AnnotationCoordinateInformation::reset() {
    m_modelSpaceInfo   = ModelSpaceInfo();
    m_tabSpaceInfo     = TabWindowSpaceInfo();
    m_windowSpaceInfo  = TabWindowSpaceInfo();
    m_chartSpaceInfo   = ChartSpaceInfo();
    m_surfaceSpaceInfo = SurfaceSpaceInfo();
    m_spacerTabSpaceInfo  = SpacerTabSpaceInfo();
    m_mediaSpaceInfo   = MediaFileNameAndPixelSpaceInfo();
}

/**
 * @return String describing content of this instance
 */
AString
AnnotationCoordinateInformation::toString() const
{
    AString msg;
    
    return msg;
}

/**
 * @return True if the given  coordinate space is valid for this instance, else false.
 */
bool
AnnotationCoordinateInformation::isCoordinateSpaceValid(const AnnotationCoordinateSpaceEnum::Enum space) const
{
    bool validSpaceFlag = false;
    
    switch (space) {
        case AnnotationCoordinateSpaceEnum::CHART:
            validSpaceFlag = m_chartSpaceInfo.m_validFlag;
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            validSpaceFlag = m_mediaSpaceInfo.m_validFlag;
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            validSpaceFlag = m_spacerTabSpaceInfo.m_spacerTabIndex.isValid();
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            validSpaceFlag = m_modelSpaceInfo.m_validFlag;
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            validSpaceFlag = m_surfaceSpaceInfo.m_validFlag;
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            validSpaceFlag = (m_tabSpaceInfo.m_index >= 0);
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssertMessage(0, "Should not create/move viewport annotations");
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            validSpaceFlag = (m_windowSpaceInfo.m_index >= 0);
            break;
    }
    
    return validSpaceFlag;
}

/**
 * Examine all of the given coord info and find coord info that is valid for all (ie: same tab indices for tab space, same window indices for window space, etc.)
 * @param annotationCoordInfo
 *    The coordinate information
 * @return
 *    A coord info instance that indices spaces valid or not valid for all input
 */
AnnotationCoordinateInformation
AnnotationCoordinateInformation::getValidCoordInfoForAll(const std::vector<std::unique_ptr<AnnotationCoordinateInformation>>& annotationCoordInfo)
{
    AnnotationCoordinateInformation validForAllCoordInfoOut;
    
    if (annotationCoordInfo.empty()) {
        return validForAllCoordInfoOut;
    }
    
    if (annotationCoordInfo.size() == 1) {
        CaretAssertVectorIndex(annotationCoordInfo, 0);
        return *annotationCoordInfo[0];
    }
    
    std::vector<bool> chartSpaceValidFlags;
    std::vector<bool> modelSpaceValidFlags;
    
    std::set<int32_t> tabUniqueIndices;
    std::vector<int32_t> tabIndices;
    
    std::vector<int32_t> spacerColumnIndices;
    std::vector<int32_t> spacerRowIndices;
    std::vector<int32_t> spacerWindowIndices;

    std::vector<int32_t> spacerTabIndices;
    
    std::vector<int32_t> surfaceNumberOfNodes;
    std::vector<StructureEnum::Enum> surfaceStructures;
    
    std::vector<int32_t> windowIndices;
    std::set<int32_t> windowUniqueIndices;

    std::vector<AString> mediaFileNames;
    
    for (const auto& aci : annotationCoordInfo) {
        if (aci->m_modelSpaceInfo.m_validFlag) {
            modelSpaceValidFlags.push_back(true);
        }
        if (aci->m_tabSpaceInfo.m_validFlag) {
            CaretAssertArrayIndex("TabIndices", BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, aci->m_tabSpaceInfo.m_index);
            tabIndices.push_back(aci->m_tabSpaceInfo.m_index);
            tabUniqueIndices.insert(aci->m_tabSpaceInfo.m_index);
        }
        if (aci->m_windowSpaceInfo.m_validFlag) {
            CaretAssertArrayIndex("WindowIndices", BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, aci->m_windowSpaceInfo.m_index);
            windowIndices.push_back(aci->m_windowSpaceInfo.m_index);
            windowUniqueIndices.insert(aci->m_windowSpaceInfo.m_index);
        }
        if (aci->m_spacerTabSpaceInfo.m_validFlag) {
            const SpacerTabIndex& spacerTabIndex = aci->m_spacerTabSpaceInfo.m_spacerTabIndex;
            spacerColumnIndices.push_back(spacerTabIndex.getColumnIndex());
            spacerRowIndices.push_back(spacerTabIndex.getRowIndex());
            spacerWindowIndices.push_back(spacerTabIndex.getWindowIndex());
        }
        if (aci->m_chartSpaceInfo.m_validFlag) {
            chartSpaceValidFlags.push_back(true);
        }
        if (aci->m_surfaceSpaceInfo.m_validFlag) {
            surfaceNumberOfNodes.push_back(aci->m_surfaceSpaceInfo.m_numberOfNodes);
            surfaceStructures.push_back(aci->m_surfaceSpaceInfo.m_structure);
        }
        if (aci->m_mediaSpaceInfo.m_validFlag) {
            mediaFileNames.push_back(aci->m_mediaSpaceInfo.m_mediaFileName);
        }
    }
    
    const uint32_t numCoordInfo(annotationCoordInfo.size());
    
    if (modelSpaceValidFlags.size() == numCoordInfo) {
        validForAllCoordInfoOut.m_modelSpaceInfo.m_validFlag = true;
    }
    
    if (tabIndices.size() == numCoordInfo) {
        std::set<int32_t> uniqueIndices(tabIndices.begin(),
                                        tabIndices.end());
        if (uniqueIndices.size() == 1) {
            validForAllCoordInfoOut.m_tabSpaceInfo.m_validFlag = true;
            validForAllCoordInfoOut.m_tabSpaceInfo.m_index = *uniqueIndices.begin();
        }
    }

    if (windowIndices.size() == numCoordInfo) {
        std::set<int32_t> uniqueIndices(windowIndices.begin(),
                                        windowIndices.end());
        if (uniqueIndices.size() == 1) {
            if (uniqueIndices.size() == 1) {
                validForAllCoordInfoOut.m_windowSpaceInfo.m_validFlag = true;
                validForAllCoordInfoOut.m_windowSpaceInfo.m_index = *uniqueIndices.begin();
            }
        }
    }
    
    if ((spacerColumnIndices.size() == numCoordInfo)
        && (spacerRowIndices.size() == numCoordInfo)
        && (spacerWindowIndices.size() == numCoordInfo)) {
        std::set<int32_t> uniqueColumnIndices(spacerColumnIndices.begin(),
                                              spacerColumnIndices.end());
        std::set<int32_t> uniqueRowIndices(spacerRowIndices.begin(),
                                           spacerRowIndices.end());
        std::set<int32_t> uniqueWindowIndices(spacerWindowIndices.begin(),
                                              spacerWindowIndices.end());
        if ((uniqueColumnIndices.size() == 1)
            && (uniqueRowIndices.size() == 1)
            && (uniqueWindowIndices.size() == 1)) {
            validForAllCoordInfoOut.m_spacerTabSpaceInfo.m_validFlag = true;
            validForAllCoordInfoOut.m_spacerTabSpaceInfo.m_spacerTabIndex = SpacerTabIndex(*uniqueWindowIndices.begin(),
                                                                                           *uniqueRowIndices.begin(),
                                                                                           *uniqueColumnIndices.begin());
        }
    }
    
    if (chartSpaceValidFlags.size() == numCoordInfo) {
        validForAllCoordInfoOut.m_chartSpaceInfo.m_validFlag = true;
    }
    
    if ((surfaceStructures.size() == numCoordInfo)
        && (surfaceNumberOfNodes.size() == numCoordInfo)) {
        std::set<StructureEnum::Enum> uniqueStructures(surfaceStructures.begin(),
                                                       surfaceStructures.end());
        std::set<int32_t> uniqueNumberOfNodes(surfaceNumberOfNodes.begin(),
                                              surfaceNumberOfNodes.end());
        if ((uniqueStructures.size() == 1)
            && (uniqueNumberOfNodes.size() == 1)) {
            validForAllCoordInfoOut.m_surfaceSpaceInfo.m_validFlag = true;
            validForAllCoordInfoOut.m_surfaceSpaceInfo.m_structure = *uniqueStructures.begin();
            validForAllCoordInfoOut.m_surfaceSpaceInfo.m_numberOfNodes = *uniqueNumberOfNodes.begin();
        }
    }
    
    if (mediaFileNames.size() == numCoordInfo) {
        std::set<AString> uniqueFileNames(mediaFileNames.begin(),
                                          mediaFileNames.end());
        if (uniqueFileNames.size() == 1) {
            validForAllCoordInfoOut.m_mediaSpaceInfo.m_validFlag = true;
            validForAllCoordInfoOut.m_mediaSpaceInfo.m_mediaFileName = *uniqueFileNames.begin();
        }
    }
    
    return validForAllCoordInfoOut;
    
    /*
     ModelSpaceInfo m_modelSpaceInfo;
     
     TabWindowSpaceInfo m_tabSpaceInfo;
     
     TabWindowSpaceInfo m_windowSpaceInfo;
     
     SpacerTabSpaceInfo m_spacerTabSpaceInfo;
     
     ChartSpaceInfo m_chartSpaceInfo;
     
     SurfaceSpaceInfo m_surfaceSpaceInfo;

     */
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
            case AnnotationCoordinateSpaceEnum::CHART:
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            case AnnotationCoordinateSpaceEnum::SPACER:
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
                            case AnnotationCoordinateSpaceEnum::CHART:
                                /*
                                 * Both coord info's must be in the SAME TAB
                                 */
                                if (coordInfoOne->m_tabSpaceInfo.m_index != coordInfoTwo->m_tabSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                                /*
                                 * Must be on same image
                                 */
                                if (coordInfoOne->m_mediaSpaceInfo.m_mediaFileName != coordInfoTwo->m_mediaSpaceInfo.m_mediaFileName) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::SPACER:
                                if (coordInfoOne->m_spacerTabSpaceInfo.m_spacerTabIndex != coordInfoTwo->m_spacerTabSpaceInfo.m_spacerTabIndex) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                                /*
                                 * Both coord info's must be in the SAME TAB
                                 */
                                if (coordInfoOne->m_tabSpaceInfo.m_index != coordInfoTwo->m_tabSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::SURFACE:
                                /*
                                 * Both coord info's must be on same surface and
                                 * in the SAME TAB
                                 */
                                if ((coordInfoOne->m_tabSpaceInfo.m_index != coordInfoTwo->m_tabSpaceInfo.m_index)
                                    || (coordInfoOne->m_surfaceSpaceInfo.m_numberOfNodes != coordInfoTwo->m_surfaceSpaceInfo.m_numberOfNodes)
                                    || (coordInfoOne->m_surfaceSpaceInfo.m_structure != coordInfoTwo->m_surfaceSpaceInfo.m_structure)) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::TAB:
                                if (coordInfoOne->m_tabSpaceInfo.m_index != coordInfoTwo->m_tabSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                                //if (coordInfoOne->m_windowIndex != coordInfoTwo->m_windowIndex) {
                                //    addItFlag = false;
                                //}
                                break;
                            case AnnotationCoordinateSpaceEnum::WINDOW:
                                if (coordInfoOne->m_windowSpaceInfo.m_index != coordInfoTwo->m_windowSpaceInfo.m_index) {
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
 * Get the valid coordinate spaces for all annotation coordinate information.
 * The space must be valid  for all coordinates, AND, if tab or window
 * space the tab or window indices must also be the same.
 *
 * @param coordInfoMulti
 *     All coordinate information.
 * @param spacesOut
 *     Output containing spaces valid for both.
 */
void
AnnotationCoordinateInformation::getValidCoordinateSpaces(const std::vector<std::unique_ptr<AnnotationCoordinateInformation>>& coordInfoMulti,
                                                          std::vector<AnnotationCoordinateSpaceEnum::Enum>& spacesOut)
{
    spacesOut.clear();
    
    const int32_t numCoordInfo(static_cast<int32_t>(coordInfoMulti.size()));
    if (numCoordInfo <= 0) {
        return;
    }
    CaretAssertVectorIndex(coordInfoMulti, 0);
    const auto firstCoordInfo(coordInfoMulti[0].get());
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> allSpaces;
    AnnotationCoordinateSpaceEnum::getAllEnums(allSpaces);
    
    for (const auto space : allSpaces) {
        switch (space) {
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::CHART:
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            case AnnotationCoordinateSpaceEnum::SPACER:
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            case AnnotationCoordinateSpaceEnum::SURFACE:
            case AnnotationCoordinateSpaceEnum::TAB:
            case AnnotationCoordinateSpaceEnum::WINDOW:
                /*
                 * See if space for first coord info is valid
                 */
                if (firstCoordInfo->isCoordinateSpaceValid(space)) {
                    bool addItFlag = true;
                    
                    for(int32_t i = 1; i < numCoordInfo; i++) {
                        CaretAssertVectorIndex(coordInfoMulti, i);
                        const auto coordInfo(coordInfoMulti[i].get());
 
                        /*
                         * See if same space is valid for second coord info
                         */
                        addItFlag = coordInfo->isCoordinateSpaceValid(space);
                        
                        switch (space) {
                            case AnnotationCoordinateSpaceEnum::CHART:
                                /*
                                 * Both coord info's must be in the SAME TAB
                                 */
                                if (firstCoordInfo->m_tabSpaceInfo.m_index != coordInfo->m_tabSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                                /*
                                 * Must be on same media file
                                 */
                                if (firstCoordInfo->m_mediaSpaceInfo.m_mediaFileName != coordInfo->m_mediaSpaceInfo.m_mediaFileName) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::SPACER:
                                if (firstCoordInfo->m_spacerTabSpaceInfo.m_spacerTabIndex != coordInfo->m_spacerTabSpaceInfo.m_spacerTabIndex) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                                /*
                                 * Both coord info's must be in the SAME TAB
                                 */
                                if (firstCoordInfo->m_tabSpaceInfo.m_index != coordInfo->m_tabSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::SURFACE:
                                /*
                                 * Both coord info's must be on same surface and
                                 * in the SAME TAB
                                 */
                                if ((firstCoordInfo->m_tabSpaceInfo.m_index != coordInfo->m_tabSpaceInfo.m_index)
                                    || (firstCoordInfo->m_surfaceSpaceInfo.m_numberOfNodes != coordInfo->m_surfaceSpaceInfo.m_numberOfNodes)
                                    || (firstCoordInfo->m_surfaceSpaceInfo.m_structure != coordInfo->m_surfaceSpaceInfo.m_structure)) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::TAB:
                                if (firstCoordInfo->m_tabSpaceInfo.m_index != coordInfo->m_tabSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                                //if (coordInfoOne->m_windowIndex != coordInfoTwo->m_windowIndex) {
                                //    addItFlag = false;
                                //}
                                break;
                            case AnnotationCoordinateSpaceEnum::WINDOW:
                                if (firstCoordInfo->m_windowSpaceInfo.m_index != coordInfo->m_windowSpaceInfo.m_index) {
                                    addItFlag = false;
                                }
                                break;
                        }
                        
                        /*
                         * If an annotation does not match space of first,
                         * then the space is not valid
                         */
                        if ( ! addItFlag) {
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
    SelectionItemMedia* mediaID = idManager->getMediaIdentification();
    
    if (surfaceNodeIdentification->isValid()) {
        surfaceNodeIdentification->getModelXYZ(coordInfoOut.m_modelSpaceInfo.m_xyz);
        coordInfoOut.m_modelSpaceInfo.m_validFlag = true;
        
        const Surface* surface = surfaceNodeIdentification->getSurface();
        CaretAssert(surface);
        coordInfoOut.m_surfaceSpaceInfo.m_numberOfNodes = surface->getNumberOfNodes();
        coordInfoOut.m_surfaceSpaceInfo.m_structure     = surface->getStructure();
        coordInfoOut.m_surfaceSpaceInfo.m_nodeIndex     = surfaceNodeIdentification->getNodeNumber();
        coordInfoOut.m_surfaceSpaceInfo.m_nodeOffsetLength    = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
        coordInfoOut.m_surfaceSpaceInfo.m_nodeVectorOffsetType    = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
        surface->getNormalVector(surfaceNodeIdentification->getNodeNumber(),
                                 coordInfoOut.m_surfaceSpaceInfo.m_nodeNormalVector);
        coordInfoOut.m_surfaceSpaceInfo.m_validFlag     = true;
    }
    else if (voxelID->isValid()) {
        voxelID->getModelXYZ(coordInfoOut.m_modelSpaceInfo.m_xyz);
        coordInfoOut.m_modelSpaceInfo.m_validFlag = true;
    }
    else if (mediaID->isValid()) {
        mediaID->getModelXYZ(coordInfoOut.m_modelSpaceInfo.m_xyz);
        mediaID->getModelXYZ(coordInfoOut.m_mediaSpaceInfo.m_xyz);

        coordInfoOut.m_mediaSpaceInfo.m_mediaFileName = mediaID->getMediaFile()->getFileNameNoPath();
        coordInfoOut.m_mediaSpaceInfo.m_validFlag = true;
        
        coordInfoOut.m_modelSpaceInfo.m_validFlag = true;
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
            coordInfoOut.m_tabSpaceInfo.m_xyz[0] = tabX;
            coordInfoOut.m_tabSpaceInfo.m_xyz[1] = tabY;
            coordInfoOut.m_tabSpaceInfo.m_xyz[2] = 0.0;
            coordInfoOut.m_tabSpaceInfo.m_pixelXYZ[0] = (windowX - tabViewport[0]);
            coordInfoOut.m_tabSpaceInfo.m_pixelXYZ[1] = (windowY - tabViewport[1]);
            coordInfoOut.m_tabSpaceInfo.m_pixelXYZ[2] = 0.0;
            coordInfoOut.m_tabSpaceInfo.m_index = tabContent->getTabNumber();
            coordInfoOut.m_tabSpaceInfo.m_width = tabViewport[2];
            coordInfoOut.m_tabSpaceInfo.m_height = tabViewport[3];
            coordInfoOut.m_tabSpaceInfo.m_validFlag = true;
        }
    }
    
    if (tabContent != NULL) {
        Matrix4x4 projectionMatrix;
        Matrix4x4 modelviewMatrix;
        int viewport[4];
        if (viewportContent->getChartDataMatricesAndViewport(projectionMatrix,
                                                             modelviewMatrix,
                                                             viewport)) {
            if ((windowX >= viewport[0])
                && (windowX < (viewport[0] + viewport[2]))
                && (windowY >= viewport[1])
                && (windowY < (viewport[1] + viewport[3]))) {
                double projectionArray[16];
                projectionMatrix.getMatrixForOpenGL(projectionArray);
                double modelviewArray[16];
                modelviewMatrix.getMatrixForOpenGL(modelviewArray);
                
                double chartX = 0.0;
                double chartY = 0.0;
                double chartZ = 0.0;
                if (gluUnProject(windowX, windowY, 0.0,
                                 modelviewArray, projectionArray, viewport,
                                 &chartX, &chartY, &chartZ) == GL_TRUE) {
                    coordInfoOut.m_chartSpaceInfo.m_xyz[0] = static_cast<float>(chartX);
                    coordInfoOut.m_chartSpaceInfo.m_xyz[1] = static_cast<float>(chartY);
                    coordInfoOut.m_chartSpaceInfo.m_xyz[2] = static_cast<float>(chartZ);
                    coordInfoOut.m_chartSpaceInfo.m_validFlag = true;
                }
            }
        }
    }
    
    SpacerTabContent* spacerTabContent = viewportContent->getSpacerTabContent();
    if (spacerTabContent != NULL) {
        int tabViewport[4];
        viewportContent->getModelViewport(tabViewport);
        const float tabX = 100.0 * (windowX - tabViewport[0]) / static_cast<float>(tabViewport[2]);
        const float tabY = 100.0 * (windowY - tabViewport[1]) / static_cast<float>(tabViewport[3]);
        if ((tabX >= 0.0)
            && (tabX < 100.0)
            && (tabY >= 0.0)
            && (tabY <= 100.0)) {
            coordInfoOut.m_spacerTabSpaceInfo.m_xyz[0] = tabX;
            coordInfoOut.m_spacerTabSpaceInfo.m_xyz[1] = tabY;
            coordInfoOut.m_spacerTabSpaceInfo.m_xyz[2] = 0.0;
            coordInfoOut.m_spacerTabSpaceInfo.m_pixelXYZ[0] = (windowX - tabViewport[0]);
            coordInfoOut.m_spacerTabSpaceInfo.m_pixelXYZ[1] = (windowY - tabViewport[1]);
            coordInfoOut.m_spacerTabSpaceInfo.m_pixelXYZ[2] = 0.0;
            coordInfoOut.m_spacerTabSpaceInfo.m_spacerTabIndex = spacerTabContent->getSpacerTabIndex();
            coordInfoOut.m_spacerTabSpaceInfo.m_validFlag = spacerTabContent->getSpacerTabIndex().isValid();
            coordInfoOut.m_spacerTabSpaceInfo.m_width = tabViewport[2];
            coordInfoOut.m_spacerTabSpaceInfo.m_height = tabViewport[3];
        }
    }
    
    int windowViewport[4];
    viewportContent->getWindowViewport(windowViewport);
    coordInfoOut.m_windowSpaceInfo.m_pixelXYZ[0] = windowX - windowViewport[0];
    coordInfoOut.m_windowSpaceInfo.m_pixelXYZ[1] = windowY - windowViewport[1];
    coordInfoOut.m_windowSpaceInfo.m_pixelXYZ[2] = 0.0;
    coordInfoOut.m_windowSpaceInfo.m_index  = viewportContent->getWindowIndex();
    coordInfoOut.m_windowSpaceInfo.m_width  = windowViewport[2];
    coordInfoOut.m_windowSpaceInfo.m_height = windowViewport[3];
    coordInfoOut.m_windowSpaceInfo.m_validFlag = true;
    
    /*
     * Normalize window coordinates (width and height range [0, 100]
     */
    coordInfoOut.m_windowSpaceInfo.m_xyz[0] = 100.0 * (coordInfoOut.m_windowSpaceInfo.m_pixelXYZ[0] / windowViewport[2]);
    coordInfoOut.m_windowSpaceInfo.m_xyz[1] = 100.0 * (coordInfoOut.m_windowSpaceInfo.m_pixelXYZ[1] / windowViewport[3]);
    coordInfoOut.m_windowSpaceInfo.m_xyz[2] = 0.0;
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
                                                                  const AnnotationCoordinateInformation* coordInfoTwo,
                                                                  const std::vector<std::unique_ptr<AnnotationCoordinateInformation>>& coordInfoMulti)
{
    CaretAssert(annotation);
    
    bool validCoordinateFlag = false;
    
    AnnotationTwoCoordinateShape* twoCoordAnn = annotation->castToTwoCoordinateShape();
    AnnotationOneCoordinateShape* oneCoordAnn = annotation->castToOneCoordinateShape();
    AnnotationMultiCoordinateShape* multiCoordAnn = annotation->castToMultiCoordinateShape();
    if (twoCoordAnn != NULL) {
        validCoordinateFlag = setOneDimAnnotationCoordinatesForSpace(twoCoordAnn,
                                                                     coordinateSpace,
                                                                     coordInfoOne,
                                                                     coordInfoTwo);
    }
    else if (oneCoordAnn != NULL) {
        validCoordinateFlag = setTwoDimAnnotationCoordinatesForSpace(oneCoordAnn,
                                                                     coordinateSpace,
                                                                     coordInfoOne,
                                                                     coordInfoTwo);
    }
    else if (multiCoordAnn != NULL) {
        validCoordinateFlag = setMultiDimAnnotationCoordinatesForSpace(multiCoordAnn,
                                                                       coordinateSpace,
                                                                       coordInfoMulti);
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
AnnotationCoordinateInformation::setOneDimAnnotationCoordinatesForSpace(AnnotationTwoCoordinateShape* annotation,
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
        case AnnotationCoordinateSpaceEnum::CHART:
            if (coordInfoOne->m_chartSpaceInfo.m_validFlag) {
                startCoordinate->setXYZ(coordInfoOne->m_chartSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::CHART);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_chartSpaceInfo.m_validFlag) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_chartSpaceInfo.m_xyz);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            if (coordInfoOne->m_mediaSpaceInfo.m_validFlag) {
                startCoordinate->setMediaFileNameAndPixelSpace(coordInfoOne->m_mediaSpaceInfo.m_mediaFileName,
                                                               coordInfoOne->m_mediaSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_mediaSpaceInfo.m_validFlag) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setMediaFileNameAndPixelSpace(coordInfoTwo->m_mediaSpaceInfo.m_mediaFileName,
                                                                         coordInfoTwo->m_mediaSpaceInfo.m_xyz);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            if (coordInfoOne->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid()) {
                startCoordinate->setXYZ(coordInfoOne->m_spacerTabSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SPACER);
                annotation->setSpacerTabIndex(coordInfoOne->m_spacerTabSpaceInfo.m_spacerTabIndex);
                
                validCoordinateFlag = true;
                
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid()) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_spacerTabSpaceInfo.m_xyz);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_spacerTabSpaceInfo.m_xyz[0],
                        coordInfoOne->m_spacerTabSpaceInfo.m_xyz[1],
                        coordInfoOne->m_spacerTabSpaceInfo.m_xyz[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                        endCoordinate->setXYZ(xyz);
                    }
                    else {
                        xyz[1] += 25.0;
                        endCoordinate->setXYZ(coordInfoOne->m_spacerTabSpaceInfo.m_xyz);
                        startCoordinate->setXYZ(xyz);
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (coordInfoOne->m_modelSpaceInfo.m_validFlag) {
                startCoordinate->setXYZ(coordInfoOne->m_modelSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_modelSpaceInfo.m_validFlag) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_modelSpaceInfo.m_xyz);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (coordInfoOne->m_surfaceSpaceInfo.m_validFlag) {
                const float surfaceOffsetLength = startCoordinate->getSurfaceOffsetLength();
                const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = startCoordinate->getSurfaceOffsetVectorType();
                startCoordinate->setSurfaceSpace(coordInfoOne->m_surfaceSpaceInfo.m_structure,
                                                 coordInfoOne->m_surfaceSpaceInfo.m_numberOfNodes,
                                                 coordInfoOne->m_surfaceSpaceInfo.m_nodeIndex,
                                                 surfaceOffsetLength,
                                                 surfaceOffsetVector);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_surfaceSpaceInfo.m_validFlag) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setSurfaceSpace(coordInfoTwo->m_surfaceSpaceInfo.m_structure,
                                                           coordInfoTwo->m_surfaceSpaceInfo.m_numberOfNodes,
                                                           coordInfoTwo->m_surfaceSpaceInfo.m_nodeIndex,
                                                           surfaceOffsetLength,
                                                           surfaceOffsetVector);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (coordInfoOne->m_tabSpaceInfo.m_index >= 0) {
                startCoordinate->setXYZ(coordInfoOne->m_tabSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(coordInfoOne->m_tabSpaceInfo.m_index);
                
                validCoordinateFlag = true;
                
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_tabSpaceInfo.m_index >= 0) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_tabSpaceInfo.m_xyz);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_tabSpaceInfo.m_xyz[0],
                        coordInfoOne->m_tabSpaceInfo.m_xyz[1],
                        coordInfoOne->m_tabSpaceInfo.m_xyz[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                        endCoordinate->setXYZ(xyz);
                    }
                    else {
                        xyz[1] += 25.0;
                        endCoordinate->setXYZ(coordInfoOne->m_tabSpaceInfo.m_xyz);
                        startCoordinate->setXYZ(xyz);
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (coordInfoOne->m_windowSpaceInfo.m_index >= 0) {
                startCoordinate->setXYZ(coordInfoOne->m_windowSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(coordInfoOne->m_windowSpaceInfo.m_index);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_windowSpaceInfo.m_index >= 0) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_windowSpaceInfo.m_xyz);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_windowSpaceInfo.m_xyz[0],
                        coordInfoOne->m_windowSpaceInfo.m_xyz[1],
                        coordInfoOne->m_windowSpaceInfo.m_xyz[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                        endCoordinate->setXYZ(xyz);
                    }
                    else {
                        xyz[1] += 25.0;
                        endCoordinate->setXYZ(coordInfoOne->m_windowSpaceInfo.m_xyz);
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
AnnotationCoordinateInformation::setTwoDimAnnotationCoordinatesForSpace(AnnotationOneCoordinateShape* annotation,
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
        case AnnotationCoordinateSpaceEnum::CHART:
            if (coordInfoOne->m_chartSpaceInfo.m_validFlag) {
                coordinate->setXYZ(coordInfoOne->m_chartSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::CHART);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_chartSpaceInfo.m_validFlag) {
                        float centerXYZ[3] = {
                            (float)(coordInfoOne->m_chartSpaceInfo.m_xyz[0] + optionalCoordInfoTwo->m_chartSpaceInfo.m_xyz[0]) / 2.0f,
                            (float)(coordInfoOne->m_chartSpaceInfo.m_xyz[1] + optionalCoordInfoTwo->m_chartSpaceInfo.m_xyz[1]) / 2.0f,
                            (float)(coordInfoOne->m_chartSpaceInfo.m_xyz[2] + optionalCoordInfoTwo->m_chartSpaceInfo.m_xyz[2]) / 2.0f
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            if (coordInfoOne->m_mediaSpaceInfo.m_validFlag) {
                coordinate->setMediaFileNameAndPixelSpace(coordInfoOne->m_mediaSpaceInfo.m_mediaFileName,
                                                          coordInfoOne->m_mediaSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_mediaSpaceInfo.m_validFlag) {
                        float centerXYZ[3] = {
                            (float)(coordInfoOne->m_mediaSpaceInfo.m_xyz[0] + optionalCoordInfoTwo->m_mediaSpaceInfo.m_xyz[0]) / 2.0f,
                            (float)(coordInfoOne->m_mediaSpaceInfo.m_xyz[1] + optionalCoordInfoTwo->m_mediaSpaceInfo.m_xyz[1]) / 2.0f,
                            (float)(coordInfoOne->m_mediaSpaceInfo.m_xyz[2] + optionalCoordInfoTwo->m_mediaSpaceInfo.m_xyz[2]) / 2.0f
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            if (coordInfoOne->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid()) {
                coordinate->setXYZ(coordInfoOne->m_spacerTabSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SPACER);
                annotation->setSpacerTabIndex(coordInfoOne->m_spacerTabSpaceInfo.m_spacerTabIndex);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid() == coordInfoOne->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid()) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_spacerTabSpaceInfo.m_xyz[0] + optionalCoordInfoTwo->m_spacerTabSpaceInfo.m_xyz[0]) / 2.0f,
                            (coordInfoOne->m_spacerTabSpaceInfo.m_xyz[1] + optionalCoordInfoTwo->m_spacerTabSpaceInfo.m_xyz[1]) / 2.0f,
                            (coordInfoOne->m_spacerTabSpaceInfo.m_xyz[2] + optionalCoordInfoTwo->m_spacerTabSpaceInfo.m_xyz[2]) / 2.0f
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (coordInfoOne->m_modelSpaceInfo.m_validFlag) {
                coordinate->setXYZ(coordInfoOne->m_modelSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_modelSpaceInfo.m_validFlag) {
                        float centerXYZ[3] = {
                            (float)(coordInfoOne->m_modelSpaceInfo.m_xyz[0] + optionalCoordInfoTwo->m_modelSpaceInfo.m_xyz[0]) / 2.0f,
                            (float)(coordInfoOne->m_modelSpaceInfo.m_xyz[1] + optionalCoordInfoTwo->m_modelSpaceInfo.m_xyz[1]) / 2.0f,
                            (float)(coordInfoOne->m_modelSpaceInfo.m_xyz[2] + optionalCoordInfoTwo->m_modelSpaceInfo.m_xyz[2]) / 2.0f
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (coordInfoOne->m_surfaceSpaceInfo.m_validFlag) {
                coordinate->setSurfaceSpace(coordInfoOne->m_surfaceSpaceInfo.m_structure,
                                            coordInfoOne->m_surfaceSpaceInfo.m_numberOfNodes,
                                            coordInfoOne->m_surfaceSpaceInfo.m_nodeIndex);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                annotation->initializeSurfaceSpaceWithTangentOffsetRotation(coordInfoOne->m_surfaceSpaceInfo.m_structure,
                                                                            coordInfoOne->m_surfaceSpaceInfo.m_nodeNormalVector);
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if ((optionalCoordInfoTwo->m_surfaceSpaceInfo.m_validFlag)
                        && (optionalCoordInfoTwo->m_surfaceSpaceInfo.m_structure == coordInfoOne->m_surfaceSpaceInfo.m_structure
                            )) {
                        if ((optionalCoordInfoTwo->m_windowSpaceInfo.m_index == coordInfoOne->m_windowSpaceInfo.m_index)
                            && (coordInfoOne->m_windowSpaceInfo.m_index >= 0)) {
                            const float windowWidth  = coordInfoOne->m_windowSpaceInfo.m_width;
                            const float windowHeight = coordInfoOne->m_windowSpaceInfo.m_height;
                            const float x1 = coordInfoOne->m_windowSpaceInfo.m_xyz[0] * windowWidth;
                            const float y1 = coordInfoOne->m_windowSpaceInfo.m_xyz[1] * windowHeight;
                            const float x2 = optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[0] * windowWidth;
                            const float y2 = optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[1] * windowHeight;
                            const int32_t windowX = static_cast<int32_t>((x1 + x2)) / 2.0;
                            const int32_t windowY = static_cast<int32_t>((y1 + y2)) / 2.0;
                            
                            EventIdentificationRequest idRequest(coordInfoOne->m_windowSpaceInfo.m_index,
                                                                 static_cast<int32_t>(windowX),
                                                                 static_cast<int32_t>(windowY));
                            EventManager::get()->sendEvent(idRequest.getPointer());
                            SelectionManager* sm = idRequest.getSelectionManager();
                            if (sm != NULL) {
                                const SelectionItemSurfaceNode* nodeID = sm->getSurfaceNodeIdentification();
                                CaretAssert(nodeID);
                                if (nodeID->isValid()) {
                                    if (nodeID->getSurface()->getStructure() == coordInfoOne->m_surfaceSpaceInfo.m_structure) {
                                        coordinate->setSurfaceSpace(coordInfoOne->m_surfaceSpaceInfo.m_structure,
                                                                    coordInfoOne->m_surfaceSpaceInfo.m_numberOfNodes,
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
            if (coordInfoOne->m_tabSpaceInfo.m_index >= 0) {
                coordinate->setXYZ(coordInfoOne->m_tabSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(coordInfoOne->m_tabSpaceInfo.m_index);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_tabSpaceInfo.m_index == coordInfoOne->m_tabSpaceInfo.m_index) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_tabSpaceInfo.m_xyz[0] + optionalCoordInfoTwo->m_tabSpaceInfo.m_xyz[0]) / 2.0f,
                            (coordInfoOne->m_tabSpaceInfo.m_xyz[1] + optionalCoordInfoTwo->m_tabSpaceInfo.m_xyz[1]) / 2.0f,
                            (coordInfoOne->m_tabSpaceInfo.m_xyz[2] + optionalCoordInfoTwo->m_tabSpaceInfo.m_xyz[2]) / 2.0f
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
            if (coordInfoOne->m_windowSpaceInfo.m_index >= 0) {
                coordinate->setXYZ(coordInfoOne->m_windowSpaceInfo.m_xyz);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(coordInfoOne->m_windowSpaceInfo.m_index);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_windowSpaceInfo.m_index == coordInfoOne->m_windowSpaceInfo.m_index) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_windowSpaceInfo.m_xyz[0] + optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[0]) / 2.0f,
                            (coordInfoOne->m_windowSpaceInfo.m_xyz[1] + optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[1]) / 2.0f,
                            (coordInfoOne->m_windowSpaceInfo.m_xyz[2] + optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[2]) / 2.0f
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithWindowCoordsFlag = true;
                    }
                }
            }
            break;
    }
    
    if (setWidthHeightWithTabCoordsFlag) {
        if (coordInfoOne->m_tabSpaceInfo.m_index >= 0) {
            if (optionalCoordInfoTwo != NULL) {
                if (coordInfoOne->m_tabSpaceInfo.m_index == optionalCoordInfoTwo->m_tabSpaceInfo.m_index) {
                    annotation->setWidthAndHeightFromBounds(coordInfoOne->m_tabSpaceInfo.m_xyz,
                                                            optionalCoordInfoTwo->m_tabSpaceInfo.m_xyz,
                                                            coordInfoOne->m_tabSpaceInfo.m_width,
                                                            coordInfoOne->m_tabSpaceInfo.m_height);
                }
            }
        }
    }
    else if (setWidthHeightWithWindowCoordsFlag) {
        if (coordInfoOne->m_windowSpaceInfo.m_index >= 0) {
            if (optionalCoordInfoTwo != NULL) {
                if (coordInfoOne->m_windowSpaceInfo.m_index == optionalCoordInfoTwo->m_windowSpaceInfo.m_index) {
                    const float windowWidth  = coordInfoOne->m_windowSpaceInfo.m_width;
                    const float windowHeight = coordInfoOne->m_windowSpaceInfo.m_height;
                    
                    const float oneXYZ[3] = {
                        coordInfoOne->m_windowSpaceInfo.m_xyz[0],
                        coordInfoOne->m_windowSpaceInfo.m_xyz[1],
                        coordInfoOne->m_windowSpaceInfo.m_xyz[2]
                    };
                    const float twoXYZ[3] = {
                        optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[0],
                        optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[1],
                        optionalCoordInfoTwo->m_windowSpaceInfo.m_xyz[2]
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

/**
 * Set the coordinates for the multi-coordinate annotation.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @param coordInfoMulti
 *     Info for the coordinates
 */
bool
AnnotationCoordinateInformation::setMultiDimAnnotationCoordinatesForSpace(AnnotationMultiCoordinateShape* annotation,
                                                                          const AnnotationCoordinateSpaceEnum::Enum space,
                                                                          const std::vector<std::unique_ptr<AnnotationCoordinateInformation>>& coordInfoMulti)
{
    if (coordInfoMulti.empty()) {
        return false;
    }
    
    CaretAssert(annotation);
    
    /*
     * Set annotation parameters using first coord info
     */
    bool validSpaceFlag(false);
    CaretAssertVectorIndex(coordInfoMulti, 0);
    const auto& firstCoordInfo = coordInfoMulti[0];
    switch (space) {
        case AnnotationCoordinateSpaceEnum::CHART:
            if (firstCoordInfo->m_chartSpaceInfo.m_validFlag) {
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::CHART);
                validSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            if (firstCoordInfo->m_mediaSpaceInfo.m_validFlag) {
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
                validSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            if (firstCoordInfo->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid()) {
                annotation->setSpacerTabIndex(firstCoordInfo->m_spacerTabSpaceInfo.m_spacerTabIndex);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SPACER);
                validSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (firstCoordInfo->m_modelSpaceInfo.m_validFlag) {
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                validSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (firstCoordInfo->m_surfaceSpaceInfo.m_validFlag) {
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                validSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (firstCoordInfo->m_tabSpaceInfo.m_validFlag) {
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(firstCoordInfo->m_tabSpaceInfo.m_index);
                validSpaceFlag = true;
            }
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (firstCoordInfo->m_windowSpaceInfo.m_index >= 0) {
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(firstCoordInfo->m_windowSpaceInfo.m_index);
                validSpaceFlag = true;
            }
            break;
    }

    if ( ! validSpaceFlag) {
        return false;
    }
    
    /*
     * Set coordinates
     */
    std::vector<std::unique_ptr<const AnnotationCoordinate>> coordinates;
    
    for (auto& coordInfo : coordInfoMulti) {
        AnnotationCoordinate* ac(new AnnotationCoordinate(AnnotationAttributesDefaultTypeEnum::USER));
        switch (space) {
            case AnnotationCoordinateSpaceEnum::CHART:
                if (coordInfo->m_chartSpaceInfo.m_validFlag) {
                    ac->setXYZ(coordInfo->m_chartSpaceInfo.m_xyz);
                }
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                if (coordInfo->m_mediaSpaceInfo.m_validFlag) {
                    ac->setMediaFileNameAndPixelSpace(coordInfo->m_mediaSpaceInfo.m_mediaFileName,
                                                      coordInfo->m_mediaSpaceInfo.m_xyz);
                }
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                if (coordInfo->m_spacerTabSpaceInfo.m_spacerTabIndex.isValid()) {
                    ac->setXYZ(coordInfo->m_spacerTabSpaceInfo.m_xyz);
                }
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                if (coordInfo->m_modelSpaceInfo.m_validFlag) {
                    ac->setXYZ(coordInfo->m_modelSpaceInfo.m_xyz);
                }
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                if (coordInfo->m_surfaceSpaceInfo.m_validFlag) {
                    const float surfaceOffsetLength = ac->getSurfaceOffsetLength();
                    const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = ac->getSurfaceOffsetVectorType();
                    ac->setSurfaceSpace(coordInfo->m_surfaceSpaceInfo.m_structure,
                                        coordInfo->m_surfaceSpaceInfo.m_numberOfNodes,
                                        coordInfo->m_surfaceSpaceInfo.m_nodeIndex,
                                        surfaceOffsetLength,
                                        surfaceOffsetVector);
                }
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                if (coordInfo->m_tabSpaceInfo.m_validFlag) {
                    ac->setXYZ(coordInfo->m_tabSpaceInfo.m_xyz);
                }
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                if (coordInfo->m_windowSpaceInfo.m_index >= 0) {
                    ac->setXYZ(coordInfo->m_windowSpaceInfo.m_xyz);
                }
                break;
        }
        
        std::unique_ptr<AnnotationCoordinate> ptr(ac);
        coordinates.push_back(std::move(ptr));
    }
    
    annotation->replaceAllCoordinates(coordinates);
    
    return (annotation->getNumberOfCoordinates() > 0);
}

/**
 * Create coordinates for annotation on clipboard
 * @param mouseEvent
 *    The mouse event from the pasting operation
 * @param clipboard
 *    The annotation clipboard
 * @param mouseCoordInfo
 *    Coordinate information at the position of the mouse
 * @param coordInfoOut
 *    Output with coordinate information for the pasted location of each coordinate in the annotation
 */
bool
AnnotationCoordinateInformation::createCoordinateInformationForPasting(const MouseEvent& mouseEvent,
                                                                       const AnnotationClipboard* clipboard,
                                                                       AnnotationCoordinateInformation& mouseCoordInfoOut,
                                                                       std::vector<std::unique_ptr<AnnotationCoordinateInformation>>& coordInfoOut,
                                                                       AString& errorMessageOut)
{
    CaretAssert(clipboard);
    coordInfoOut.clear();
    
    if (clipboard->isEmpty()) {
        errorMessageOut = "No annotation is on the clipboard.";
        return false;
    }
    
    BrainOpenGLWidget* openGLWidget(mouseEvent.getOpenGLWidget());
    CaretAssert(openGLWidget);
    BrainOpenGLViewportContent* viewportContent(mouseEvent.getViewportContent());
    CaretAssert(viewportContent);
    
    /*
     * Set valid annotation coordinates at position of mouse
     */
    createCoordinateInformationFromXY(openGLWidget,
                                      viewportContent,
                                      mouseEvent.getX(),
                                      mouseEvent.getY(),
                                      mouseCoordInfoOut);
        
    CaretAssert(clipboard->getAnnotation(0));

    /*
     * Get the window coordinates for each of the annotation's coordinates
     * from when the annotation was cut/copied
     */
    const std::vector<Vector3D>& annotationCutCopyWindowCoords(clipboard->getAnnotationWindowCoordinates());
    const int32_t numCoords(annotationCutCopyWindowCoords.size());
    if (numCoords < 1) {
        errorMessageOut = "No window coordinates for the annotation were found on the clipboard.";
        return false;
    }

    /*
     * Center of gravity of the annotation's window coordinates
     */
    Vector3D annotationCutCopyCOG(0.0, 0.0, 0.0);
    for (int32_t i = 0; i < numCoords; i++) {
        annotationCutCopyCOG += annotationCutCopyWindowCoords[i];
    }
    CaretAssert(numCoords >= 1);
    annotationCutCopyCOG /= static_cast<float>(numCoords);
    
    /*
     * Create pasting location for all of an annotation's coordinates
     */
    Vector3D mouseWindowCoords(mouseEvent.getX(),
                               mouseEvent.getY(),
                               0.0);
    std::vector<Vector3D> annotationPasteWindowCoords;
    for (int32_t i = 0; i < numCoords; i++) {
        /*
         * Coordinate is now relative to COG of cut/copy
         */
        Vector3D v3d = annotationCutCopyWindowCoords[i] - annotationCutCopyCOG;
        
        /*
         * Set to window coordinate where it is pasted
         */
        v3d += mouseWindowCoords;
        
        annotationPasteWindowCoords.push_back(v3d);
    }
    CaretAssert(annotationPasteWindowCoords.size() == annotationCutCopyWindowCoords.size());
    
    
    for (auto& wc : annotationPasteWindowCoords) {
        std::unique_ptr<AnnotationCoordinateInformation> coordInfo(new AnnotationCoordinateInformation());
        createCoordinateInformationFromXY(openGLWidget,
                                          viewportContent,
                                          wc[0],
                                          wc[1],
                                          *coordInfo);
        coordInfoOut.push_back(std::move(coordInfo));
    }
    
    return true;
}
