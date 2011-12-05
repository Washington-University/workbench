
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
#include "ConnectivityLoaderManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventInformationTextDisplay.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemVoxel.h"
#include "IdentificationManager.h"
#include "MouseEvent.h"
#include "ModelDisplayController.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class UserInputModeView 
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
    
    IdentificationItemSurfaceNode* idNode = idManager->getSurfaceNodeIdentification();
    Surface* surface = idNode->getSurface();
    const int32_t nodeIndex = idNode->getNodeNumber();
    if ((surface != NULL) &&
        (nodeIndex >= 0)) {
        try {
            updateGraphicsFlag = connMan->loadDataForSurfaceNode(surface, nodeIndex);
            updateGraphicsFlag = true;
            
            BrainStructure* brainStructure = surface->getBrainStructure();
            CaretAssert(brainStructure);
            
            BrainStructureNodeAttributes* nodeAtts = brainStructure->getNodeAttributes(nodeIndex);
            nodeAtts->setIdentified(true);

        }
        catch (DataFileException e) {
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
            try {
                updateGraphicsFlag = connMan->loadDataForVoxelAtCoordinate(xyz);
            }
            catch (DataFileException e) {
                QMessageBox::critical(openGLWidget, "", e.whatString());
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
    ModelDisplayController* modelController = browserTabContent->getModelControllerForTransformation();
    if (modelController != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        const float dx = mouseEvent->getDx();
        const float dy = mouseEvent->getDy();
        
        //
        // Mouse moved with just left button down
        //
        if (mouseEvent->isAnyKeyDown() == false) {
            //if (modelController->isRotationAllowed()) {
            /*
             * Special case when the surface is a right surface that is
             * lateral/medial yoked.
             * Matrix index 0 is used in almost all cases.
             * Matrix index 1 is ONLY used when the surface is a right surface
             * that is lateral/medial yoked.
             */ 
            if (browserTabContent->isDisplayedModelSurfaceRightLateralMedialYoked()) {
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 0);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(-dx);
                
                /*
                 * Matrix "1" is used for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRight = modelController->getViewingRotationMatrix(tabIndex, 1);
                rotationMatrixRight->rotateX(dy);
                rotationMatrixRight->rotateY(dx);
            }
            else {
                Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(tabIndex, 0);
                rotationMatrix->rotateX(-dy);
                rotationMatrix->rotateY(dx);
                
                /*
                 * Matrix "1" is used for a right medial/lateral yoked surface
                 */
                Matrix4x4* rotationMatrixRight = modelController->getViewingRotationMatrix(tabIndex, 1);
                rotationMatrixRight->rotateX(dy);
                rotationMatrixRight->rotateY(-dx);
            }
            //}
        }
        //
        // Mouse moved with control key and left mouse button down
        //
        else if (mouseEvent->isControlKeyDown()) {
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
            float t2[] = { t1[0] + dx, t1[1] + dy, t2[2] };
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
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
UserInputModeView::toString() const
{
    return "UserInputModeView";
}
