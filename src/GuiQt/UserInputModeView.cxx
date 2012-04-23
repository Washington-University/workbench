
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
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrainStructureNodeAttributes.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "ConnectivityLoaderManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventInformationTextDisplay.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "EventUpdateTimeCourseDialog.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemSurfaceNodeIdentificationSymbol.h"
#include "IdentificationItemVoxel.h"
#include "IdentificationManager.h"
#include "MouseEvent.h"
#include "Model.h"
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
                       BrowserTabContent* browserTabContent,
                       BrainOpenGLWidget* openGLWidget)
{
    switch (mouseEvent->getMouseEventType()) {
        case MouseEventTypeEnum::INVALID:
            break;
        case MouseEventTypeEnum::LEFT_CLICKED:
            this->processIdentification(mouseEvent, 
                                        browserTabContent, 
                                        openGLWidget);
            break;
        case MouseEventTypeEnum::LEFT_DRAGGED:
            this->processModelViewTransformation(mouseEvent, 
                                                 browserTabContent, 
                                                 openGLWidget);
            break;
        case MouseEventTypeEnum::LEFT_PRESSED:
            break;
        case MouseEventTypeEnum::LEFT_RELEASED:
            break;
        case MouseEventTypeEnum::WHEEL_MOVED:
            this->processModelViewTransformation(mouseEvent, 
                                                 browserTabContent, 
                                                 openGLWidget);
            break;
    }
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
 */
void 
UserInputModeView::processIdentification(MouseEvent* mouseEvent,
                                         BrowserTabContent* /*browserTabContent*/,
                                         BrainOpenGLWidget* openGLWidget)
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* connMan = brain->getConnectivityLoaderManager();
    
    IdentificationManager* idManager =
        openGLWidget->performIdentification(mouseEvent->getX(), mouseEvent->getY());
    
    bool updateGraphicsFlag = false;
    

    IdentificationItemSurfaceNodeIdentificationSymbol* idSymbol = idManager->getSurfaceNodeIdentificationSymbol();
    if ((idSymbol->getSurface() != NULL)
        && (idSymbol->getNodeNumber() >= 0)) {
        EventIdentificationSymbolRemoval idRemoval(idSymbol->getSurface()->getStructure(),
                                                   idSymbol->getNodeNumber());
        EventManager::get()->sendEvent(idRemoval.getPointer());
        updateGraphicsFlag = true;
    }
    else {
        
        IdentificationItemSurfaceNode* idNode = idManager->getSurfaceNodeIdentification();
        Surface* surface = idNode->getSurface();
        const int32_t nodeIndex = idNode->getNodeNumber();

        if ((surface != NULL) &&
            (nodeIndex >= 0)) {
            try {
                connMan->loadDataForSurfaceNode(surface, nodeIndex);
                connMan->loadTimeLineForSurfaceNode(surface, nodeIndex);
                updateGraphicsFlag = true;
                
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
                EventIdentificationHighlightLocation idLocation(idManager,
                                                                brainStructure,
                                                                brainStructure->getStructure(),
                                                                nodeIndex,
                                                                brainStructure->getNumberOfNodes(),
                                                                xyz);
                EventManager::get()->sendEvent(idLocation.getPointer());                
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
                QMessageBox::critical(openGLWidget, "", e.whatString());
            }
        }
        
        const IdentificationItemVoxel* idVoxel = idManager->getVoxelIdentification();
        if (idVoxel->isValid()) {
            const VolumeFile* volumeFile = idVoxel->getVolumeFile();
            int64_t voxelIJK[3];
            idVoxel->getVoxelIJK(voxelIJK);
            if (volumeFile != NULL) {
                float xyz[3];
                volumeFile->indexToSpace(voxelIJK, xyz);
                
                EventIdentificationHighlightLocation idLocation(idManager,
                                                                volumeFile,
                                                                voxelIJK,
                                                                xyz);
                EventManager::get()->sendEvent(idLocation.getPointer());
                                
                updateGraphicsFlag = true;
                
                try {
                    connMan->loadDataForVoxelAtCoordinate(xyz);
                }
                catch (const DataFileException& e) {
                    QMessageBox::critical(openGLWidget, "", e.whatString());
                }
                try {
                    connMan->loadTimeLineForVoxelAtCoordinate(xyz);
                }
                catch (const DataFileException& e) {
                    QMessageBox::critical(openGLWidget, "", e.whatString());
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
    }
    
    const BrowserTabContent* btc = NULL;
    const AString idMessage = idManager->getIdentificationText(btc,
                                                               brain);
    
    EventManager::get()->sendEvent(EventInformationTextDisplay(idMessage,
                                                               EventInformationTextDisplay::TYPE_HTML).getPointer());

    if (updateGraphicsFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
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
 */
void 
UserInputModeView::processModelViewTransformation(MouseEvent* mouseEvent,
                                                  BrowserTabContent* browserTabContent,
                                                  BrainOpenGLWidget* /*openGLWidget*/)
{
    Model* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        const float dx = mouseEvent->getDx();
        const float dy = mouseEvent->getDy();
        
        /*
         * Is this a mouse wheel event?
         */
        const bool isWheelEvent = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::WHEEL_MOVED);
            
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
                                                                                      Model::ROTATION_MATRIX_NORMAL);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(-dx);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                      Model::ROTATION_MATRIX_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(dy);
                rotationMatrixRightLatMedYoked->rotateY(dx);
            }
            else {
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                      Model::ROTATION_MATRIX_NORMAL);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(dx);
                
                /*
                 * Matrix for a left surface opposite view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontLeftOpp = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                     Model::ROTATION_MATRIX_SURFACE_MONTAGE_LEFT_OPPOSITE);
                rotationMatrixSurfMontLeftOpp->rotateX(-dy);
                rotationMatrixSurfMontLeftOpp->rotateY(dx);
                
                /*
                 * Matrix for a right surface view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontRight = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                     Model::ROTATION_MATRIX_SURFACE_MONTAGE_RIGHT);
                rotationMatrixSurfMontRight->rotateX(dy); //-dy);
                rotationMatrixSurfMontRight->rotateY(-dx);
                
                /*
                 * Matrix for a right surface opposite view in surface montage
                 */
                Matrix4x4* rotationMatrixSurfMontRightOpp = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                                   Model::ROTATION_MATRIX_SURFACE_MONTAGE_RIGHT_OPPOSITE);
                rotationMatrixSurfMontRightOpp->rotateX(dy); //-dy);
                rotationMatrixSurfMontRightOpp->rotateY(-dx);
                
                /*
                 * Matrix for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRightLatMedYoked = modelController->getViewingRotationMatrix(tabIndex, 
                                                                                           Model::ROTATION_MATRIX_RIGHT_LATERAL_MEDIAL_YOKED);
                rotationMatrixRightLatMedYoked->rotateX(dy);
                rotationMatrixRightLatMedYoked->rotateY(-dx);
            }
            //}
        }
        //
        // Mouse moved with control key and left mouse button down
        // OR is this a wheel event
        //
        else if (isWheelEvent
                 || mouseEvent->isControlKeyDown()) {
            float scale = modelController->getScaling(tabIndex);
            if (dy != 0) {
                scale += (dy * 0.05);
            }
            if (scale < 0.01) scale = 0.01;
            modelController->setScaling(tabIndex, scale);
        }
        //
        // Mouse moved with shift key and left mouse button down
        //
        else if (mouseEvent->isShiftKeyDown()) {
            const float* t1 = modelController->getTranslation(tabIndex);
            float t2[] = { t1[0] + dx, t1[1] + dy, t1[2] };
            modelController->setTranslation(tabIndex, t2);
        }
        
        mouseEvent->setGraphicsUpdateOneWindowRequested();
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
