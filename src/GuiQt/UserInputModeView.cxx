
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QMessageBox>

#define __USER_INPUT_MODE_VIEW_DECLARE__
#include "UserInputModeView.h"
#undef __USER_INPUT_MODE_VIEW_DECLARE__

#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "ConnectivityLoaderManager.h"
#include "CursorDisplayScoped.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "EventUpdateInformationWindows.h"
#include "EventUpdateTimeCourseDialog.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "IdentifiedItemNode.h"
#include "IdentificationStringBuilder.h"
#include "ModelSurfaceMontage.h"
#include "ModelYokingGroup.h"
#include "MouseEvent.h"
#include "Model.h"
#include "ModelVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "TimeLine.h"
#include "TimeCourseDialog.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeView 
 * \brief Processing user input for VIEW mode.
 *
 * Processes user input in VIEW mode which includes
 * viewing transformation of brain models and
 * identification operations.
 */

/**
 * Constructor.
 */
UserInputModeView::UserInputModeView()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
UserInputModeView::~UserInputModeView()
{
    
}

/**
 * @return The input mode enumerated type.
 */
UserInputModeView::UserInputMode 
UserInputModeView::getUserInputMode() const
{
    return UserInputReceiverInterface::VIEW;
}

/**
 * Called when a mouse events occurs for 'this' 
 * user input receiver.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param browserTabContent
 *     Content of the browser window's tab.
 * @param openGLWidget
 *     OpenGL Widget in which mouse event occurred.
 */
void 
UserInputModeView::processMouseEvent(MouseEvent* mouseEvent,
                       BrainOpenGLViewportContent* viewportContent,
                       BrainOpenGLWidget* openGLWidget)
{
    switch (mouseEvent->getMouseEventType()) {
        case MouseEventTypeEnum::INVALID:
            break;
        case MouseEventTypeEnum::LEFT_CLICKED:
//            this->processIdentification(mouseEvent, 
//                                        viewportContent, 
//                                        openGLWidget);
            break;
        case MouseEventTypeEnum::LEFT_DRAGGED:
//            this->processModelViewTransformation(mouseEvent, 
//                                                 viewportContent, 
//                                                 openGLWidget,
//                                                 this->mousePressX,
//                                                 this->mousePressY);
            break;
        case MouseEventTypeEnum::LEFT_PRESSED:
            this->mousePressX = mouseEvent->getX();
            this->mousePressY = mouseEvent->getY();
            break;
        case MouseEventTypeEnum::LEFT_RELEASED:
            break;
        case MouseEventTypeEnum::WHEEL_MOVED:
            this->processModelViewTransformation(mouseEvent, 
                                                 viewportContent, 
                                                 openGLWidget,
                                                 this->mousePressX,
                                                 this->mousePressY);
            break;
    }
}

/**
 * Process identification..
 *
 * @param mouseEvent
 *     The mouse event.
 * @param browserTabContent
 *     Content of the browser window's tab.
 * @param openGLWidget
 *     OpenGL Widget in which mouse event occurred.
 * @param mouseClickX
 *     Location of where mouse was clicked.
 * @param mouseClickY
 *     Location of where mouse was clicked.
 */
void
UserInputModeView::processModelViewIdentification(BrainOpenGLViewportContent* /*viewportContent*/,
                                           BrainOpenGLWidget* openGLWidget,
                                           const int32_t mouseClickX,
                                           const int32_t mouseClickY)
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* connMan = brain->getConnectivityLoaderManager();
    CiftiConnectivityMatrixDataFileManager* ciftiMan = brain->getCiftiConnectivityMatrixDataFileManager();
    IdentificationManager* identificationManager = brain->getIdentificationManager();
    
    SelectionManager* selectionManager =
    openGLWidget->performIdentification(mouseClickX,
                                        mouseClickY,
                                        true);
    
    bool updateGraphicsFlag = false;
    bool updateInformationFlag = false;
    std::vector<AString> ciftiLoadingInfo;
    
    const QString breakAndIndent("<br>&nbsp;&nbsp;&nbsp;&nbsp;");
    SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = selectionManager->getSurfaceNodeIdentificationSymbol();
    if ((idSymbol->getSurface() != NULL)
        && (idSymbol->getNodeNumber() >= 0)) {
        const Surface* surface = idSymbol->getSurface();
        const int32_t surfaceNumberOfNodes = surface->getNumberOfNodes();
        const int32_t nodeIndex = idSymbol->getNodeNumber();
        const StructureEnum::Enum structure = surface->getStructure();
        
        identificationManager->removeIdentifiedNodeItem(structure,
                                            surfaceNumberOfNodes,
                                            nodeIndex);
        updateGraphicsFlag = true;
        updateInformationFlag = true;
    }
    else {
        IdentifiedItem* identifiedItem = NULL;
        
        const BrowserTabContent* btc = NULL;
        SelectionItemSurfaceNode* idNode = selectionManager->getSurfaceNodeIdentification();
        SelectionItemVoxel* idVoxel = selectionManager->getVoxelIdentification();
        
        /*
         * If there is a voxel ID but no node ID, identify a 
         * node near the voxel coordinate, if it is close by.
         */
        if (idNode->isValid() == false) {
            if (idVoxel->isValid()) {
                double doubleXYZ[3];
                idVoxel->getModelXYZ(doubleXYZ);
                const float voxelXYZ[3] = {
                    doubleXYZ[0],
                    doubleXYZ[1],
                    doubleXYZ[2]
                };
                Surface* surface = brain->getVolumeInteractionSurfaceNearestCoordinate(voxelXYZ,
                                                                                       3.0);
                if (surface != NULL) {
                    const int nodeIndex = surface->closestNode(voxelXYZ);
                    if (nodeIndex >= 0) {
                        idNode->reset();
                        idNode->setBrain(brain);
                        idNode->setSurface(surface);
                        idNode->setNodeNumber(nodeIndex);
                    }
                }
            }
        }
        
        /*
         * Load CIFTI NODE data prior to ID Message so that CIFTI
         * data shown in identification text is correct for the
         * node that was loaded.
         */
        if (idNode->isValid()) {
            Surface* surface = idNode->getSurface();
            const int32_t nodeIndex = idNode->getNodeNumber();
            try {
                TimeLine timeLine;
                connMan->loadDataForSurfaceNode(surface, nodeIndex, ciftiLoadingInfo);
                ciftiMan->loadDataForSurfaceNode(surface, nodeIndex, ciftiLoadingInfo);
                
                surface->getTimeLineInformation(nodeIndex,timeLine);
                connMan->loadTimeLineForSurfaceNode(surface, nodeIndex,timeLine, ciftiLoadingInfo);
                updateGraphicsFlag = true;
                
                QList <TimeLine> tlV;
                connMan->getSurfaceTimeLines(tlV);
                if(tlV.size()!=0)
                {
                    GuiManager::get()->addTimeLines(tlV);
                }
                EventUpdateTimeCourseDialog e;
                EventManager::get()->sendEvent(e.getPointer());
            }
            catch (const DataFileException& e) {
                cursor.restoreCursor();
                QMessageBox::critical(openGLWidget, "", e.whatString());
                cursor.showWaitCursor();
            }
        }
        
        /*
         * Load CIFTI VOXEL data prior to ID Message so that CIFTI
         * data shown in identification text is correct for the
         * voxel that was loaded.
         */
        if (idVoxel->isValid()) {
            const VolumeFile* volumeFile = idVoxel->getVolumeFile();
            int64_t voxelIJK[3];
            idVoxel->getVoxelIJK(voxelIJK);
            if (volumeFile != NULL) {
                float xyz[3];
                volumeFile->indexToSpace(voxelIJK, xyz);
                
                updateGraphicsFlag = true;
                
                try {
                    connMan->loadDataForVoxelAtCoordinate(xyz,
                                                          ciftiLoadingInfo);
                    ciftiMan->loadDataForVoxelAtCoordinate(xyz,
                                                           ciftiLoadingInfo);
                }
                catch (const DataFileException& e) {
                    cursor.restoreCursor();
                    QMessageBox::critical(openGLWidget, "", e.whatString());
                    cursor.showWaitCursor();
                }
                try {
                    connMan->loadTimeLineForVoxelAtCoordinate(xyz,
                                                              ciftiLoadingInfo);
                }
                catch (const DataFileException& e) {
                    cursor.restoreCursor();
                    QMessageBox::critical(openGLWidget, "", e.whatString());
                    cursor.showWaitCursor();
                }
                QList <TimeLine> tlV;
                connMan->getVolumeTimeLines(tlV);
                if(tlV.size()!=0)
                {
                    GuiManager::get()->addTimeLines(tlV);
                }
                EventUpdateTimeCourseDialog e;
                EventManager::get()->sendEvent(e.getPointer());
            }
        }
        /*
         * Generate identification manager
         */
        const AString identificationMessage = selectionManager->getIdentificationText(btc,
                                                                                      brain);
        
        bool issuedIdentificationLocationEvent = false;
        if (idNode->isValid()) {
            Surface* surface = idNode->getSurface();
            const int32_t nodeIndex = idNode->getNodeNumber();
            
            /*
             * Save last selected node which may get used for foci creation.
             */
            selectionManager->setLastSelectedItem(idNode);
            
            
            BrainStructure* brainStructure = surface->getBrainStructure();
            CaretAssert(brainStructure);
            
            float xyz[3];
            const Surface* volumeInteractionSurface = brainStructure->getVolumeInteractionSurface();
            if (volumeInteractionSurface != NULL) {
                volumeInteractionSurface->getCoordinate(nodeIndex,
                                                        xyz);
            }
            else {
                CaretLogWarning("No surface/volume interaction surface for "
                                + StructureEnum::toGuiName(brainStructure->getStructure()));
                xyz[0] = -10000000.0;
                xyz[1] = -10000000.0;
                xyz[2] = -10000000.0;
            }
            
            identifiedItem = new IdentifiedItemNode(identificationMessage,
                                                    surface->getStructure(),
                                                    surface->getNumberOfNodes(),
                                                    nodeIndex);
            if (issuedIdentificationLocationEvent == false) {
                EventIdentificationHighlightLocation idLocation(xyz);
                EventManager::get()->sendEvent(idLocation.getPointer());
                issuedIdentificationLocationEvent = true;
            }
        }
        
        if (idVoxel->isValid()) {
            const VolumeFile* volumeFile = idVoxel->getVolumeFile();
            int64_t voxelIJK[3];
            idVoxel->getVoxelIJK(voxelIJK);
            if (volumeFile != NULL) {
                float xyz[3];
                volumeFile->indexToSpace(voxelIJK, xyz);
                
                if (issuedIdentificationLocationEvent == false) {
                    EventIdentificationHighlightLocation idLocation(xyz);
                    EventManager::get()->sendEvent(idLocation.getPointer());
                    issuedIdentificationLocationEvent = true;
                }
                
                /*
                 * Save last selected node which may get used for foci creation.
                 */
                selectionManager->setLastSelectedItem(idVoxel);                
            }
        }
        
        if (identifiedItem == NULL) {
            if (identificationMessage.isEmpty() == false) {
                identifiedItem = new IdentifiedItem(identificationMessage);
            }
        }
        
        AString ciftiInfo;
        if (ciftiLoadingInfo.empty() == false) {
            IdentificationStringBuilder ciftiIdStringBuilder;
            ciftiIdStringBuilder.addLine(false, "CIFTI Rows loaded", " ");
            for (std::vector<AString>::iterator iter = ciftiLoadingInfo.begin();
                 iter != ciftiLoadingInfo.end();
                 iter++) {
                ciftiIdStringBuilder.addLine(true, *iter);
            }
            
            ciftiInfo = ciftiIdStringBuilder.toString();
        }
        if (ciftiInfo.isEmpty() == false) {
            if (identifiedItem != NULL) {
                identifiedItem->appendText(ciftiInfo);
            }
            else {
                identifiedItem = new IdentifiedItem(ciftiInfo);
            }
        }
        
        if (identifiedItem != NULL) {
            identificationManager->addIdentifiedItem(identifiedItem);
            updateInformationFlag = true;
        }        
    }
    
    if (updateInformationFlag) {
        EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
    }
    
    if (updateGraphicsFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().addToolBar().addToolBox().getPointer());
    }
}

/**
 * Process identification.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param browserTabContent
 *     Content of the browser window's tab.
 * @param openGLWidget
 *     OpenGL Widget in which mouse event occurred.
 */
void 
UserInputModeView::processIdentification(MouseEvent* mouseEvent,
                                         BrainOpenGLViewportContent* viewportContent,
                                         BrainOpenGLWidget* openGLWidget)
{
    UserInputModeView::processModelViewIdentification(viewportContent,
                                                      openGLWidget,
                                                      mouseEvent->getX(),
                                                      mouseEvent->getY());
}

/**
 * Process model viewing transformation.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param browserTabContent
 *     Content of the browser window's tab.
 * @param openGLWidget
 *     OpenGL Widget in which mouse event occurred.
 * @param mousePressedX
 *     Location of where mouse was first pressed.
 * @param mousePressedY
 *     Location of where mouse was first pressed.
 */
void 
UserInputModeView::processModelViewTransformation(MouseEvent* mouseEvent,
                                                  BrainOpenGLViewportContent* viewportContent,
                                                  BrainOpenGLWidget* /*openGLWidget*/,
                                                  const int32_t mousePressedX,
                                                  const int32_t mousePressedY)
{
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    Model* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        float dx = mouseEvent->getDx();
        float dy = mouseEvent->getDy();
        
        /*
         * Is this a mouse wheel event?
         */
        const bool isWheelEvent = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::WHEEL_MOVED);
        
        /*
         * Both ModelVolume and ModelYokingGroup implement the ModelVolumeInterface
         * so both will be non-NULL if it is a ModelVolume with or without yoking.
         */
        ModelVolumeInterface* modelVolumeInterface = dynamic_cast<ModelVolumeInterface*>(modelController);
        ModelVolume* modelVolume = NULL;
        if (modelVolumeInterface != NULL) {
            modelVolume = dynamic_cast<ModelVolume*>(browserTabContent->getModelControllerForDisplay());
        }
        
        if ((modelVolume != NULL)
            && (modelVolumeInterface != NULL))
        {
            if (!isWheelEvent && mouseEvent->isShiftKeyDown())//shift left drag
            {
                const float* t1 = modelController->getTranslation(tabIndex, Model::VIEWING_TRANSFORM_NORMAL);       
                float t2[] = { t1[0], t1[1], t1[2] };
                VolumeFile* vf = modelVolume->getUnderlayVolumeFile(tabIndex);
                BoundingBox mybox = vf->getSpaceBoundingBox();
                float cubesize = std::max(std::max(mybox.getDifferenceX(), mybox.getDifferenceY()), mybox.getDifferenceZ());//factor volume bounding box into slowdown for zoomed in
                float slowdown = 0.005f * cubesize / modelController->getScaling(tabIndex);//when zoomed in, make the movements slower to match - still changes based on viewport currently
                switch (modelVolumeInterface->getSliceViewPlane(tabIndex))
                {
                    case VolumeSliceViewPlaneEnum::ALL:
                    {
                        int viewport[4];
                        viewportContent->getModelViewport(viewport);
                        const int32_t halfWidth  = viewport[2] / 2;
                        const int32_t halfHeight = viewport[3] / 2;
                        const int32_t viewportMousePressedX = mousePressedX - viewport[0];
                        const int32_t viewportMousePressedY = mousePressedY - viewport[1];
                        bool isRight  = false;
                        bool isTop = false;
                        if (viewportMousePressedX > halfWidth) {
                            isRight = true;
                        }
                        if (viewportMousePressedY > halfHeight) {
                            isTop = true;
                        }
                        //CaretLogInfo("right: " + AString::fromBool(isRight) + " top: " + AString::fromBool(isTop));
                        if (isTop)
                        {
                            if (isRight)//coronal
                            {
                                t2[0] += dx * slowdown;
                                t2[2] += dy * slowdown;
                            } else {//parasaggital
                                t2[1] -= dx * slowdown;
                                t2[2] += dy * slowdown;
                            }
                        } else {
                            if (isRight)//axial
                            {
                                t2[0] += dx * slowdown;
                                t2[1] += dy * slowdown;
                            }//bottom left has no slice
                        }
                        break;
                    }
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        t2[0] += dx * slowdown;
                        t2[1] += dy * slowdown;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        t2[0] += dx * slowdown;
                        t2[2] += dy * slowdown;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        t2[1] -= dx * slowdown;
                        t2[2] += dy * slowdown;
                        break;
                }
                modelController->setTranslation(tabIndex, t2);
                EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent->getBrowserWindowIndex()).getPointer());
                return;
            }
        }
        //
        // Mouse moved with just left button down
        //
        if ((isWheelEvent == false) 
            && (mouseEvent->isAnyKeyDown() == false)) {
            /*
             * There are several rotation matrix.  The 'NORMAL' matrix is used
             * in most cases and others are used in special viewing modes
             * such as surface montage and right/left lateral medial yoking
             */ 
            if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(-dx);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                      Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(dy);
                rotationMatrixRightLatMedYoked->rotateY(dx);
            }
            else {
                ModelSurfaceMontage* montageModel = browserTabContent->getDisplayedSurfaceMontageModel();
                if (montageModel != NULL) {
                    std::vector<SurfaceMontageViewport> montageViewports;
                    montageModel->getMontageViewports(tabIndex,
                                                      montageViewports);
                    
                    bool isValid = false;
                    bool isLeft = true;
                    bool isLateral = true;
                    const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
                    for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                        const SurfaceMontageViewport& smv = montageViewports[ivp];
                        if (smv.isInside(mousePressedX,
                                         mousePressedY)) {
                            if (StructureEnum::isLeft(smv.structure)) {
                                isValid = true;
                                isLeft  = true;
                            }
                            else if (StructureEnum::isRight(smv.structure)) {
                                isValid = true;
                                isLeft  = false;
                            }
                            
                            if (isValid) {
                                switch (smv.viewingMatrixIndex) {
                                    case Model::VIEWING_TRANSFORM_COUNT:
                                        isValid = false;
                                        break;
                                    case Model::VIEWING_TRANSFORM_NORMAL:
                                        isLateral = true;
                                        break;
                                    case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
                                        isValid = false;
                                        break;
                                    case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
                                        isLateral = false;
                                        break;
                                    case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
                                        isLateral = true;
                                        break;
                                    case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
                                        isLateral = false;
                                        break;
                                }
                            }
                        }
                        
                        if (isValid) {
                            break;
                        }
                    }
                    
                    if (isValid) {
                        if (isLeft == false) {
                            dx = -dx;
                        }
                        if (isLateral == false) {
                            dy = -dy;
                        }
                    /*
//                     * Need to adjust mouse for location in viewport
//                     */
//                    int viewport[4];
//                    viewportContent->getViewport(viewport);
////                    const int32_t mouseX = mouseEvent->getX() - viewport[0];
////                    const int32_t mouseY = mouseEvent->getY() - viewport[1];
//                    const int32_t halfWidth  = viewport[2] / 2;
//                    const int32_t halfHeight = viewport[3] / 2;
//                    
//                    /*
//                     * Use location of where mouse is originally pressed
//                     * to determine the surface montage surface being
//                     * manipulated.  Otherwise, if mouse is moved out of
//                     * region where originally pressed, the rotations
//                     * would change.
//                     */
//                    const int32_t viewportMousePressedX = mousePressedX - viewport[0];
//                    const int32_t viewportMousePressedY = mousePressedY - viewport[1];
//                    
//                    /*
//                     * Determine hemisphere of surface and if it is lateral or 
//                     * medial view.
//                     * Row2 => Top => Lateral View
//                     * Row1 => Bottom => Medial View
//                     * Columns 1 and 3 => Left Hemisphere
//                     * Columns 2 and 4 => Right Hemisphere
//                     */
//                    bool isRight  = false;
//                    bool isMedial = false;
//                    if (montageModel->isDualConfigurationEnabled(tabIndex)) {
//                        const int32_t quarterWidth = halfWidth / 2;
//                        const int32_t threeQuarterWidth = halfWidth + quarterWidth;
//                        if (viewportMousePressedX > halfWidth) {
//                            isRight = true;
//                        }
//                        
//                        if ((viewportMousePressedX >= quarterWidth)
//                            && (viewportMousePressedX < halfWidth)) {
//                            isMedial = true;
//                        }
//                        else if (viewportMousePressedX >= threeQuarterWidth) {
//                            isMedial = true;
//                        }
//                    }
//                    else {
//                        if (viewportMousePressedX > halfWidth) {
//                            isRight = true;
//                        }
//                        if (viewportMousePressedY < halfHeight) {
//                            isMedial = true;
//                        }
//                    }
//                    
//                    if (isRight) {
//                        dx = -dx;
//                    }
//                    
//                    if (isMedial) {
//                        dy = -dy;
//                    }
                    
                    }
                }
            
            Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex,
                                                                                  Model::VIEWING_TRANSFORM_NORMAL);
            rotationMatrix->rotateX(-dy);
            rotationMatrix->rotateY(dx);
            
            /*
             * Matrix for a left surface opposite view in surface montage
             */
            Matrix4x4* rotationMatrixSurfMontLeftOpp = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                 Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
            rotationMatrixSurfMontLeftOpp->rotateX(-dy);
            rotationMatrixSurfMontLeftOpp->rotateY(dx);
            
            /*
             * Matrix for a right surface view in surface montage
             */
            Matrix4x4* rotationMatrixSurfMontRight = modelController->getViewingRotationMatrix(tabIndex,
                                                                                               Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT);
            rotationMatrixSurfMontRight->rotateX(dy); //-dy);
            rotationMatrixSurfMontRight->rotateY(-dx);
            
            /*
             * Matrix for a right surface opposite view in surface montage
             */
            Matrix4x4* rotationMatrixSurfMontRightOpp = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                  Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
            rotationMatrixSurfMontRightOpp->rotateX(dy); //-dy);
            rotationMatrixSurfMontRightOpp->rotateY(-dx);
            
            /*
             * Matrix for a right medial/lateral yoked surface
             */
            Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                  Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
            rotationMatrixRightLatMedYoked->rotateX(dy);
            rotationMatrixRightLatMedYoked->rotateY(-dx);
            }
        }
        
        //
        // Mouse moved with control key and left mouse button down
        // OR is this a wheel event
        //
        else if (isWheelEvent
                 || mouseEvent->isControlKeyDown()) {
            float scale = modelController->getScaling(tabIndex);
            if (dy != 0) {
                scale *= (1.0f + dy * 0.01);
            }
            if (scale < 0.01) scale = 0.01;
            modelController->setScaling(tabIndex, scale);
        }
        //
        // Mouse moved with shift key and left mouse button down
        //
        else if (mouseEvent->isShiftKeyDown()) {
            ModelSurfaceMontage* montageModel = browserTabContent->getDisplayedSurfaceMontageModel();
            if (montageModel != NULL) {
                std::vector<SurfaceMontageViewport> montageViewports;
                montageModel->getMontageViewports(tabIndex,
                                                  montageViewports);
                
                bool isValid = false;
                bool isLeft = true;
                bool isLateral = true;
                const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
                for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                    const SurfaceMontageViewport& smv = montageViewports[ivp];
                    if (smv.isInside(mousePressedX,
                                     mousePressedY)) {
                        if (StructureEnum::isLeft(smv.structure)) {
                            isValid = true;
                            isLeft  = true;
                        }
                        else if (StructureEnum::isRight(smv.structure)) {
                            isValid = true;
                            isLeft  = false;
                        }
                        
                        if (isValid) {
                            switch (smv.viewingMatrixIndex) {
                                case Model::VIEWING_TRANSFORM_COUNT:
                                    isValid = false;
                                    break;
                                case Model::VIEWING_TRANSFORM_NORMAL:
                                    isLateral = true;
                                    break;
                                case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
                                    isValid = false;
                                    break;
                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
                                    isLateral = false;
                                    break;
                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
                                    isLateral = true;
                                    break;
                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
                                    isLateral = false;
                                    break;
                            }
                        }
                    }
                    
                    if (isValid) {
                        break;
                    }
                }
                if (isValid) {
                    if (isLeft == false) {
                        dx = -dx;
                    }
                    if (isLateral == false) {
                        dx = -dx;
                    }
                    
                    const float* translation = modelController->getTranslation(tabIndex,
                                                                               Model::VIEWING_TRANSFORM_NORMAL);
                    const float tx = translation[0];
                    const float ty = translation[1];
                    const float tz = translation[2];
                    
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_NORMAL,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED,
                                                    tx - dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                }
//                /*
//                 * Single Configuration Layout:
//                 *    S1  S3
//                 *    S2  S4
//                 *
//                 * Dual Configuration Layout:
//                 *    S1  S3  S5  S7   (S5 behaves like S1,  S7 behaves like S3)
//                 *    S2  S4  S6  S8   (S6 behaves like S2,  S8 behaves like S4)
//                 */
//                /*
//                 * Need to adjust mouse for location in viewport
//                 */
//                int viewport[4];
//                viewportContent->getViewport(viewport);
////                const int32_t mouseX = mouseEvent->getX() - viewport[0];
////                const int32_t mouseY = mouseEvent->getY() - viewport[1];
//                const int32_t halfWidth  = viewport[2] / 2;
//                const int32_t halfHeight = viewport[3] / 2;
//
//                /*
//                 * Use location of where mouse is originally pressed
//                 * to determine the surface montage surface being
//                 * manipulated.  Otherwise, if mouse is moved out of
//                 * region where originally pressed, the rotations
//                 * would change.
//                 */
//                const int32_t viewportMousePressedX = mousePressedX - viewport[0];
//                const int32_t viewportMousePressedY = mousePressedY - viewport[1];
//                
//                /*
//                 * Determine which surface S1 to S4  (S5 to S8 duplicate S1 to S4)
//                 */
//                const int32_t quarterWidth = halfWidth / 2;
//                
//                int32_t xp = 0;
//                int32_t yp = (viewportMousePressedY / halfHeight);
////                if (montageModel->isDualConfigurationEnabled(tabIndex)) {
////                    xp = (viewportMousePressedX / quarterWidth); 
////                }
////                else {
////                    xp = (viewportMousePressedX / halfWidth);
////                }
//                
//                float flipX = 1.0;
//                switch (yp) {
//                    case 0:
//                        switch (xp) {
//                            case 0:
//                            case 2:
//                                flipX = -1.0;
//                            default:
//                                break;
//                        }
//                        break;
//                    case 1:
//                        switch (xp) {
//                            case 1:
//                            case 3:
//                                flipX = -1.0;
//                            default:
//                                break;
//                        }
//                        break;
//                }
//                
//                const float* translation = modelController->getTranslation(tabIndex, 
//                                                                           Model::VIEWING_TRANSFORM_NORMAL);
//                const float tx = translation[0];
//                const float ty = translation[1];
//                const float tz = translation[2];
//                
//                modelController->setTranslation(tabIndex,
//                                                Model::VIEWING_TRANSFORM_NORMAL,
//                                                tx + (dx * flipX),
//                                                ty + dy,
//                                                tz);
//                modelController->setTranslation(tabIndex,
//                                                Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED,
//                                                tx - (dx * flipX),
//                                                ty + dy,
//                                                tz);
//                modelController->setTranslation(tabIndex,
//                                                Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE,
//                                                tx + (dx * flipX),
//                                                ty + dy,
//                                                tz);
//                modelController->setTranslation(tabIndex,
//                                                Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT,
//                                                tx + (dx * flipX),
//                                                ty + dy,
//                                                tz);
//                modelController->setTranslation(tabIndex,
//                                                Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE,
//                                                tx + (dx * flipX),
//                                                ty + dy,
//                                                tz);
            }
            else {
                if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
                    dx = -dx;
                }
                const float* t1 = modelController->getTranslation(tabIndex,
                                                                  Model::VIEWING_TRANSFORM_NORMAL);                
                float t2[] = { t1[0] + dx, t1[1] + dy, t1[2] };
                modelController->setTranslation(tabIndex, t2);
            }
        }
        
        /*
         * Update graphics.
         */
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent->getBrowserWindowIndex()).getPointer());
    }
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void 
UserInputModeView::initialize()
{
    
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeView::finish()
{
    
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeView::getCursor() const
{
    
    return CursorEnum::CURSOR_DEFAULT;
}

/**
 * @return A widget for display at the bottom of the
 * Browser Window Toolbar when this mode is active.
 * View mode has no widget so this returns NULL.
 */
QWidget* 
UserInputModeView::getWidgetForToolBar()
{
    return NULL;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
UserInputModeView::toString() const
{
    return "UserInputModeView";
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftClick(const MouseEvent& mouseEvent)
{
    std::cout << "Mouse Left Click" << std::endl;
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
    UserInputModeView::processModelViewIdentification(mouseEvent.getViewportContent(),
                                                      mouseEvent.getOpenGLWidget(),
                                                      mouseEvent.getX(),
                                                      mouseEvent.getY());
}

/**
 * Process a mouse left click with shift key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftClickWithShift(const MouseEvent& mouseEvent)
{
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
    std::cout << "Mouse Left Click With Shift" << std::endl;
}

/**
 * Process a mouse left drag with no keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDrag(const MouseEvent& mouseEvent)
{
    std::cout << "Mouse Left Drag" << std::endl;

    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    Model* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        float dx = mouseEvent.getDx();
        float dy = mouseEvent.getDy();
        
        /*
         * Both ModelVolume and ModelYokingGroup implement the ModelVolumeInterface
         * so both will be non-NULL if it is a ModelVolume with or without yoking.
         */
        ModelVolumeInterface* modelVolumeInterface = dynamic_cast<ModelVolumeInterface*>(modelController);
        ModelVolume* modelVolume = NULL;
        if (modelVolumeInterface != NULL) {
            modelVolume = dynamic_cast<ModelVolume*>(browserTabContent->getModelControllerForDisplay());
        }
        
            /*
             * There are several rotation matrix.  The 'NORMAL' matrix is used
             * in most cases and others are used in special viewing modes
             * such as surface montage and right/left lateral medial yoking
             */
            if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex,
                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(-dx);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                      Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(dy);
                rotationMatrixRightLatMedYoked->rotateY(dx);
            }
            else {
                ModelSurfaceMontage* montageModel = browserTabContent->getDisplayedSurfaceMontageModel();
                if (montageModel != NULL) {
                    std::vector<SurfaceMontageViewport> montageViewports;
                    montageModel->getMontageViewports(tabIndex,
                                                      montageViewports);
                    
                    bool isValid = false;
                    bool isLeft = true;
                    bool isLateral = true;
                    const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
                    for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                        const SurfaceMontageViewport& smv = montageViewports[ivp];
                        if (smv.isInside(mouseEvent.getPressedX(),
                                         mouseEvent.getPressedY())) {
                            if (StructureEnum::isLeft(smv.structure)) {
                                isValid = true;
                                isLeft  = true;
                            }
                            else if (StructureEnum::isRight(smv.structure)) {
                                isValid = true;
                                isLeft  = false;
                            }
                            
                            if (isValid) {
                                switch (smv.viewingMatrixIndex) {
                                    case Model::VIEWING_TRANSFORM_COUNT:
                                        isValid = false;
                                        break;
                                    case Model::VIEWING_TRANSFORM_NORMAL:
                                        isLateral = true;
                                        break;
                                    case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
                                        isValid = false;
                                        break;
                                    case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
                                        isLateral = false;
                                        break;
                                    case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
                                        isLateral = true;
                                        break;
                                    case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
                                        isLateral = false;
                                        break;
                                }
                            }
                        }
                        
                        if (isValid) {
                            break;
                        }
                    }
                    
                    if (isValid) {
                        if (isLeft == false) {
                            dx = -dx;
                        }
                        if (isLateral == false) {
                            dy = -dy;
                        }
                    }
                }
                
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex,
                                                                                      Model::VIEWING_TRANSFORM_NORMAL);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(dx);
                
                /*
                 * Matrix for a left surface opposite view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontLeftOpp = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                     Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE);
                rotationMatrixSurfMontLeftOpp->rotateX(-dy);
                rotationMatrixSurfMontLeftOpp->rotateY(dx);
                
                /*
                 * Matrix for a right surface view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontRight = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                   Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT);
                rotationMatrixSurfMontRight->rotateX(dy); //-dy);
                rotationMatrixSurfMontRight->rotateY(-dx);
                
                /*
                 * Matrix for a right surface opposite view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontRightOpp = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                      Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE);
                rotationMatrixSurfMontRightOpp->rotateX(dy); //-dy);
                rotationMatrixSurfMontRightOpp->rotateY(-dx);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex,
                                                                                                      Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(dy);
                rotationMatrixRightLatMedYoked->rotateY(-dx);
            }
        
        /*
         * Update graphics.
         */
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent.getBrowserWindowIndex()).getPointer());
    }
}

/**
 * Process a mouse left drag with only the alt key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDragWithAlt(const MouseEvent& mouseEvent)
{
    std::cout << "Mouse Left Drag With Alt" << std::endl;
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
}

/**
 * Process a mouse left drag with ctrl key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDragWithCtrl(const MouseEvent& mouseEvent)
{
    std::cout << "Mouse Left Drag With Ctrl" << std::endl;

    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    Model* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        float dy = mouseEvent.getDy();
        
        /*
         * Both ModelVolume and ModelYokingGroup implement the ModelVolumeInterface
         * so both will be non-NULL if it is a ModelVolume with or without yoking.
         */
        ModelVolumeInterface* modelVolumeInterface = dynamic_cast<ModelVolumeInterface*>(modelController);
        ModelVolume* modelVolume = NULL;
        if (modelVolumeInterface != NULL) {
            modelVolume = dynamic_cast<ModelVolume*>(browserTabContent->getModelControllerForDisplay());
        }
        
            float scale = modelController->getScaling(tabIndex);
            if (dy != 0) {
                scale *= (1.0f + dy * 0.01);
            }
            if (scale < 0.01) scale = 0.01;
            modelController->setScaling(tabIndex, scale);
        
        /*
         * Update graphics.
         */
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent.getBrowserWindowIndex()).getPointer());
    }
}

/**
 * Process a mouse left drag with shift key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDragWithShift(const MouseEvent& mouseEvent)
{
    std::cout << "Mouse Left Drag With Shift" << std::endl;

    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    Model* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        float dx = mouseEvent.getDx();
        float dy = mouseEvent.getDy();
        
        /*
         * Both ModelVolume and ModelYokingGroup implement the ModelVolumeInterface
         * so both will be non-NULL if it is a ModelVolume with or without yoking.
         */
        ModelVolumeInterface* modelVolumeInterface = dynamic_cast<ModelVolumeInterface*>(modelController);
        ModelVolume* modelVolume = NULL;
        if (modelVolumeInterface != NULL) {
            modelVolume = dynamic_cast<ModelVolume*>(browserTabContent->getModelControllerForDisplay());
        }
        
        if ((modelVolume != NULL)
            && (modelVolumeInterface != NULL))
        {
                const float* t1 = modelController->getTranslation(tabIndex, Model::VIEWING_TRANSFORM_NORMAL);
                float t2[] = { t1[0], t1[1], t1[2] };
                VolumeFile* vf = modelVolume->getUnderlayVolumeFile(tabIndex);
                BoundingBox mybox = vf->getSpaceBoundingBox();
                float cubesize = std::max(std::max(mybox.getDifferenceX(), mybox.getDifferenceY()), mybox.getDifferenceZ());//factor volume bounding box into slowdown for zoomed in
                float slowdown = 0.005f * cubesize / modelController->getScaling(tabIndex);//when zoomed in, make the movements slower to match - still changes based on viewport currently
                switch (modelVolumeInterface->getSliceViewPlane(tabIndex))
                {
                    case VolumeSliceViewPlaneEnum::ALL:
                    {
                        int viewport[4];
                        viewportContent->getModelViewport(viewport);
                        const int32_t halfWidth  = viewport[2] / 2;
                        const int32_t halfHeight = viewport[3] / 2;
                        const int32_t viewportMousePressedX = mouseEvent.getPressedX() - viewport[0];
                        const int32_t viewportMousePressedY = mouseEvent.getPressedY() - viewport[1];
                        bool isRight  = false;
                        bool isTop = false;
                        if (viewportMousePressedX > halfWidth) {
                            isRight = true;
                        }
                        if (viewportMousePressedY > halfHeight) {
                            isTop = true;
                        }
                        //CaretLogInfo("right: " + AString::fromBool(isRight) + " top: " + AString::fromBool(isTop));
                        if (isTop)
                        {
                            if (isRight)//coronal
                            {
                                t2[0] += dx * slowdown;
                                t2[2] += dy * slowdown;
                            } else {//parasaggital
                                t2[1] -= dx * slowdown;
                                t2[2] += dy * slowdown;
                            }
                        } else {
                            if (isRight)//axial
                            {
                                t2[0] += dx * slowdown;
                                t2[1] += dy * slowdown;
                            }//bottom left has no slice
                        }
                        break;
                    }
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        t2[0] += dx * slowdown;
                        t2[1] += dy * slowdown;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        t2[0] += dx * slowdown;
                        t2[2] += dy * slowdown;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        t2[1] -= dx * slowdown;
                        t2[2] += dy * slowdown;
                        break;
                }
                modelController->setTranslation(tabIndex, t2);
                EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent.getBrowserWindowIndex()).getPointer());
                return;
        }
            ModelSurfaceMontage* montageModel = browserTabContent->getDisplayedSurfaceMontageModel();
            if (montageModel != NULL) {
                std::vector<SurfaceMontageViewport> montageViewports;
                montageModel->getMontageViewports(tabIndex,
                                                  montageViewports);
                
                bool isValid = false;
                bool isLeft = true;
                bool isLateral = true;
                const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
                for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                    const SurfaceMontageViewport& smv = montageViewports[ivp];
                    if (smv.isInside(mouseEvent.getPressedX(),
                                     mouseEvent.getPressedY())) {
                        if (StructureEnum::isLeft(smv.structure)) {
                            isValid = true;
                            isLeft  = true;
                        }
                        else if (StructureEnum::isRight(smv.structure)) {
                            isValid = true;
                            isLeft  = false;
                        }
                        
                        if (isValid) {
                            switch (smv.viewingMatrixIndex) {
                                case Model::VIEWING_TRANSFORM_COUNT:
                                    isValid = false;
                                    break;
                                case Model::VIEWING_TRANSFORM_NORMAL:
                                    isLateral = true;
                                    break;
                                case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
                                    isValid = false;
                                    break;
                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
                                    isLateral = false;
                                    break;
                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
                                    isLateral = true;
                                    break;
                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
                                    isLateral = false;
                                    break;
                            }
                        }
                    }
                    
                    if (isValid) {
                        break;
                    }
                }
                
                if (isValid) {
                    if (isLeft == false) {
                        dx = -dx;
                    }
                    if (isLateral == false) {
                        dx = -dx;
                    }
                    
                    const float* translation = modelController->getTranslation(tabIndex,
                                                                               Model::VIEWING_TRANSFORM_NORMAL);
                    const float tx = translation[0];
                    const float ty = translation[1];
                    const float tz = translation[2];
                    
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_NORMAL,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED,
                                                    tx - dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                    modelController->setTranslation(tabIndex,
                                                    Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE,
                                                    tx + dx,
                                                    ty + dy,
                                                    tz);
                }
            }
            else {
                if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
                    dx = -dx;
                }
                const float* t1 = modelController->getTranslation(tabIndex,
                                                                  Model::VIEWING_TRANSFORM_NORMAL);
                float t2[] = { t1[0] + dx, t1[1] + dy, t1[2] };
                modelController->setTranslation(tabIndex, t2);
            }
        
        /*
         * Update graphics.
         */
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent.getBrowserWindowIndex()).getPointer());
    }
}

