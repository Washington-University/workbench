
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <QMessageBox>
#include <QLineEdit>

#define __USER_INPUT_MODE_VIEW_DECLARE__
#include "UserInputModeView.h"
#undef __USER_INPUT_MODE_VIEW_DECLARE__

#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUpdateYokedWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventManager.h"
#include "GestureEvent.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "GuiManager.h"
#include "SelectionItemHistologyCoordinate.h"
#include "KeyEvent.h"
#include "MediaOverlaySet.h"
#include "ModelMedia.h"
#include "MouseEvent.h"
#include "ProgressReportingDialog.h"
#include "SelectionItemChartTwoLabel.h"
#include "SelectionItemChartTwoLineLayerVerticalNearest.h"
#include "SelectionItemMediaLogicalCoordinate.h"
#include "SelectionItemMediaPlaneCoordinate.h"
#include "SelectionItemVolumeMprCrosshair.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "UserInputModeViewContextMenu.h"
#include "ViewingTransformations.h"
#include "WuQMessageBox.h"
#include "WuQDataEntryDialog.h"

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
 * @param windowIndex
 *    Index of the window
 */
UserInputModeView::UserInputModeView(const int32_t windowIndex)
: UserInputModeAbstract(UserInputModeEnum::Enum::VIEW),
m_browserWindowIndex(windowIndex)
{
    
}

/**
 * Constructor for subclasses.
 *
 * @param windowIndex
 *    Index of the window
 * @param inputMode
 *    Subclass' input mode.
 */
UserInputModeView::UserInputModeView(const int32_t windowIndex,
                                     const UserInputModeEnum::Enum inputMode)
: UserInputModeAbstract(inputMode),
m_browserWindowIndex(windowIndex)
{
    
}


/**
 * Destructor.
 */
UserInputModeView::~UserInputModeView()
{
    
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
UserInputModeView::processModelViewIdentification(BrainOpenGLViewportContent* viewportContent,
                                           BrainOpenGLWidget* openGLWidget,
                                           const int32_t mouseClickX,
                                           const int32_t mouseClickY)
{
    SelectionManager* selectionManager =
    openGLWidget->performIdentification(mouseClickX,
                                        mouseClickY,
                                        false);
    
    BrowserTabContent* btc = viewportContent->getBrowserTabContent();
    if (btc != NULL) {
       const int32_t tabIndex = btc->getTabNumber();
       GuiManager::get()->processIdentification(tabIndex,
                                                selectionManager,
                                                openGLWidget);
        
        /*
         * Keep the main window as the active window NOT the identification window.
         * This does not work correctly on Linux as the identication window
         * may hide behind the main window.  
         */
#ifdef CARET_OS_MACOSX
        openGLWidget->parentWidget()->activateWindow();
#endif
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
 * Called to update the input receiver for various events.
 */
void
UserInputModeView::update()
{
    
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeView::getCursor() const
{
    CursorEnum::Enum cursorOut(CursorEnum::CURSOR_DEFAULT);
    
    switch (m_mprCursorMode) {
        case VOLUME_MPR_CURSOR_MODE::INVALID:
            cursorOut = CursorEnum::CURSOR_DEFAULT;
            break;
        case VOLUME_MPR_CURSOR_MODE::ROTATE_SLICE:
            cursorOut = CursorEnum::CURSOR_ROTATION;
            break;
        case VOLUME_MPR_CURSOR_MODE::SCROLL_SLICE:
            /* cursorOut = CursorEnum::CURSOR_RESIZE_VERTICAL; */
            break;
        case VOLUME_MPR_CURSOR_MODE::SELECT_SLICE:
            cursorOut = CursorEnum::CURSOR_FOUR_ARROWS;
            break;
    }
    
    return cursorOut;
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
 * Process a mouse left double-click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftDoubleClick(const MouseEvent& mouseEvent)
{
    const bool allowDoubleClickToEditChartLabel = false;
    if (allowDoubleClickToEditChartLabel) {
        const int32_t mouseX = mouseEvent.getX();
        const int32_t mouseY = mouseEvent.getY();
        
        BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
        SelectionManager* idManager = openGLWidget->performIdentification(mouseX,
                                                                          mouseY,
                                                                          false);
        CaretAssert(idManager);
        SelectionItemChartTwoLabel* labelID = idManager->getChartTwoLabelIdentification();
        if (labelID->isValid()) {
            ChartTwoCartesianAxis* axis = labelID->getChartTwoCartesianAxis();
            ChartTwoOverlaySet* chartOverlaySet = labelID->getChartOverlaySet();
            if ((axis != NULL)
                && (chartOverlaySet != NULL)) {
                WuQDataEntryDialog newNameDialog("Axis Label",
                                                 openGLWidget);
                QLineEdit* lineEdit = newNameDialog.addLineEditWidget("Label");
                lineEdit->setText(chartOverlaySet->getAxisLabel(axis));
                if (newNameDialog.exec() == WuQDataEntryDialog::Accepted) {
                    const AString name = lineEdit->text().trimmed();
                    chartOverlaySet->setAxisLabel(axis,
                                                  name);
                    
                    /*
                     * Update graphics.
                     */
                    updateGraphics(mouseEvent);
                }
            }
        }
    }
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
    if (mouseEvent.getViewportContent() == NULL) {
        return;
    }
    processModelViewIdentification(mouseEvent.getViewportContent(),
                                   mouseEvent.getOpenGLWidget(),
                                   mouseEvent.getX(),
                                   mouseEvent.getY());
    
    SelectionManager* selectionManager = GuiManager::get()->getBrain()->getSelectionManager();
    
    SelectionItemChartTwoLineLayerVerticalNearest* layerSelection = selectionManager->getChartTwoLineLayerVerticalNearestIdentification();
    CaretAssert(layerSelection);
    
    if (layerSelection->isValid()) {
        processChartActiveLayerAction(ChartActiveLayerMode::SELECT,
                                      layerSelection->getChartTwoOverlay(),
                                      layerSelection->getLineSegmentIndex());
    }
    else if (layerSelection->isOutsideChartBounds()) {
        ChartTwoOverlay* invalidChartOverlay(NULL);
        int32_t invalidLineSegmentIndex(-1);
        processChartActiveLayerAction(ChartActiveLayerMode::DESELECT_ALL,
                                      invalidChartOverlay,
                                      invalidLineSegmentIndex);
    }
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
}

/**
 * Process a mouse left click with ctrl and shift keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftClickWithCtrlShift(const MouseEvent& mouseEvent)
{
    /*
     * Perform identification same as a left click
     */
    mouseLeftClick(mouseEvent);
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
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }

    bool allowRotationFlag(true);
    bool scrollVolumeSlicesFlag(false);
    if (browserTabContent->isVolumeSlicesDisplayed()) {
        bool mprFlag(false);
        switch (browserTabContent->getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                mprFlag = true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                allowRotationFlag      = false;
                scrollVolumeSlicesFlag = true;
                break;
        }
        if (mprFlag) {
            if (browserTabContent->getVolumeSliceViewPlane() != VolumeSliceViewPlaneEnum::ALL) {
                /*
                 * Scroll slice if viewing a single slice plane
                 */
                scrollVolumeSlicesFlag = true;
            }
            else {
                switch (m_mprCursorMode) {
                    case VOLUME_MPR_CURSOR_MODE::INVALID:
                        break;
                    case VOLUME_MPR_CURSOR_MODE::SCROLL_SLICE:
                        scrollVolumeSlicesFlag = true;
                        break;
                    case VOLUME_MPR_CURSOR_MODE::SELECT_SLICE:
                    {
                        SelectionManager* selectionManager(GuiManager::get()->getBrain()->getSelectionManager());
                        selectionManager->setAllSelectionsEnabled(false);
                        SelectionItemVoxel* voxelID(selectionManager->getVoxelIdentification());
                        voxelID->setEnabledForSelection(true);
                        mouseEvent.getOpenGLWidget()->performIdentification(mouseEvent.getX(),
                                                                            mouseEvent.getY(),
                                                                            false);
                        if (voxelID->isValid()) {
                            double xyzDouble[3] { 0.0, 0.0, 0.0 };
                            voxelID->getModelXYZ(xyzDouble);
                            float xyz[3] {
                                static_cast<float>(xyzDouble[0]),
                                static_cast<float>(xyzDouble[1]),
                                static_cast<float>(xyzDouble[2])
                            };
                            browserTabContent->selectVolumeSlicesAtCoordinate(xyz);
                        }
                        allowRotationFlag = false;
                        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_UPDATE_VOLUME_SLICE_INDICES_COORDS_TOOLBAR);
                    }
                        break;
                    case VOLUME_MPR_CURSOR_MODE::ROTATE_SLICE:
                        /*
                         * Will fall through to rotation
                         */
                        break;
                }
            }
        }
    }
    if (scrollVolumeSlicesFlag) {
        browserTabContent->applyMouseVolumeSliceIncrement(viewportContent,
                                                          mouseEvent.getPressedX(),
                                                          mouseEvent.getPressedY(),
                                                          mouseEvent.getDy());
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_UPDATE_VOLUME_SLICE_INDICES_COORDS_TOOLBAR);
    }
    else if (browserTabContent->isChartTwoDisplayed()) {
        const int32_t x1(mouseEvent.getPressedX());
        const int32_t y1(mouseEvent.getPressedY());
        const int32_t x2(mouseEvent.getX());
        const int32_t y2(mouseEvent.getY());
        
        Matrix4x4 m1, m2;
        int32_t chartViewport[4];
        if (viewportContent->getChartDataMatricesAndViewport(m1,
                                                             m2,
                                                             chartViewport)) {
            browserTabContent->applyChartTwoAxesBoundSelection(chartViewport,
                                                               x1, y1, x2, y2);
        }
        else {
            CaretLogSevere("Chart viewport is invalid");
        }
    }
    else if (browserTabContent->isHistologyDisplayed()) {
        bool modelXyzValidFlag(false);
        double modelXYZ[3];
        BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
        SelectionItemHistologyCoordinate* histologyID = openGLWidget->performIdentificationHistologyPlaneCoordinate(mouseEvent.getX(),
                                                                                                             mouseEvent.getY());
        CaretAssert(histologyID);
        if (histologyID->isValid()) {
            const HistologyCoordinate coordinate(histologyID->getCoordinate());
            const Vector3D planeXYZ(coordinate.getPlaneXYZ());
            modelXYZ[0] = planeXYZ[0];
            modelXYZ[1] = planeXYZ[1];
            modelXYZ[2] = planeXYZ[2];
            modelXyzValidFlag = true;
        }

        if (modelXyzValidFlag) {
            GraphicsRegionSelectionBox* box = browserTabContent->getMediaRegionSelectionBox();
            CaretAssert(box);
            
            if (mouseEvent.isFirstDragging()) {
                box->initialize(modelXYZ[0],
                                modelXYZ[1]);
            }
            else {
                box->update(modelXYZ[0],
                            modelXYZ[1]);
            }
        }
    }
    else if (browserTabContent->isMediaDisplayed()) {
        bool modelXyzValidFlag(false);
        double modelXYZ[3];
        BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
        switch (browserTabContent->getMediaDisplayCoordinateMode()) {
            case MediaDisplayCoordinateModeEnum::PIXEL:
            {
                SelectionItemMediaLogicalCoordinate* mediaID = openGLWidget->performIdentificationMediaLogicalCoordinate(mouseEvent.getX(),
                                                                                                                         mouseEvent.getY());
                CaretAssert(mediaID);
                if (mediaID->isValid()) {
                    mediaID->getModelXYZ(modelXYZ);
                    modelXyzValidFlag = true;
                }
            }
                break;
            case MediaDisplayCoordinateModeEnum::PLANE:
            {
                SelectionItemMediaPlaneCoordinate* mediaID = openGLWidget->performIdentificationMediaPlaneCoordinate(mouseEvent.getX(),
                                                                                                                     mouseEvent.getY());
                CaretAssert(mediaID);
                if (mediaID->isValid()) {
                    const Vector3D planeXYZ(mediaID->getPlaneCoordinate());
                    modelXYZ[0] = planeXYZ[0];
                    modelXYZ[1] = planeXYZ[1];
                    modelXYZ[2] = planeXYZ[2];
                    modelXyzValidFlag = true;
                }
            }
                break;
        }

        if (modelXyzValidFlag) {
            GraphicsRegionSelectionBox* box = browserTabContent->getMediaRegionSelectionBox();
            CaretAssert(box);
            
            if (mouseEvent.isFirstDragging()) {
                box->initialize(modelXYZ[0],
                                modelXYZ[1]);
            }
            else {
                box->update(modelXYZ[0],
                            modelXYZ[1]);
            }
        }
    }
    else if (allowRotationFlag) {
        browserTabContent->applyMouseRotation(viewportContent,
                                              mouseEvent.getPressedX(),
                                              mouseEvent.getPressedY(),
                                              mouseEvent.getX(),
                                              mouseEvent.getY(),
                                              mouseEvent.getDx(),
                                              mouseEvent.getDy());
    }
    
    /*
     * Update graphics.
     */
    updateGraphics(mouseEvent);
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
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
    int32_t modelViewport[4];
    viewportContent->getModelViewport(modelViewport);
    if (browserTabContent->isHistologyDisplayed()) {
        if (mouseEvent.isFirstDragging()) {
            m_histologyLeftDragWithCtrlModelXYZValidFlag = false;
            
            BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
            bool modelXyzValidFlag(false);
            double modelXYZ[3];
            SelectionItemHistologyCoordinate* histologyID = openGLWidget->performIdentificationHistologyPlaneCoordinate(mouseEvent.getPressedX(),
                                                                                                                             mouseEvent.getPressedY());
            CaretAssert(histologyID);
            if (histologyID->isValid()) {
                const HistologyCoordinate coordinate(histologyID->getCoordinate());
                const Vector3D planeXYZ(coordinate.getPlaneXYZ());
                modelXYZ[0] = planeXYZ[0];
                modelXYZ[1] = planeXYZ[1];
                modelXYZ[2] = planeXYZ[2];
                modelXyzValidFlag = true;
            }
            
            if (modelXyzValidFlag) {
                m_histologyLeftDragWithCtrlModelXYZ[0] = modelXYZ[0];
                m_histologyLeftDragWithCtrlModelXYZ[1] = modelXYZ[1];
                m_histologyLeftDragWithCtrlModelXYZ[2] = modelXYZ[2];
                m_histologyLeftDragWithCtrlModelXYZValidFlag = true;
            }
        }
        browserTabContent->applyHistologyMouseScaling(viewportContent,
                                                      mouseEvent.getPressedX(),
                                                      mouseEvent.getPressedY(),
                                                      mouseEvent.getDy(),
                                                      m_histologyLeftDragWithCtrlModelXYZ[0],
                                                      m_histologyLeftDragWithCtrlModelXYZ[1],
                                                      m_histologyLeftDragWithCtrlModelXYZValidFlag);
    }
    else if (browserTabContent->isMediaDisplayed()) {
        if (mouseEvent.isFirstDragging()) {
            m_mediaLeftDragWithCtrlModelXYZValidFlag = false;
            
            BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
            bool modelXyzValidFlag(false);
            double modelXYZ[3];
            switch (browserTabContent->getMediaDisplayCoordinateMode()) {
                case MediaDisplayCoordinateModeEnum::PIXEL:
                {
                    SelectionItemMediaLogicalCoordinate* mediaID = openGLWidget->performIdentificationMediaLogicalCoordinate(mouseEvent.getPressedX(),
                                                                                                            mouseEvent.getPressedY());
                    CaretAssert(mediaID);
                    if (mediaID->isValid()) {
                        mediaID->getModelXYZ(modelXYZ);
                        modelXyzValidFlag = true;
                    }
                }
                    break;
                case MediaDisplayCoordinateModeEnum::PLANE:
                {
                    SelectionItemMediaPlaneCoordinate* mediaID = openGLWidget->performIdentificationMediaPlaneCoordinate(mouseEvent.getPressedX(),
                                                                                                                             mouseEvent.getPressedY());
                    CaretAssert(mediaID);
                    if (mediaID->isValid()) {
                        const Vector3D planeXYZ(mediaID->getPlaneCoordinate());
                        modelXYZ[0] = planeXYZ[0];
                        modelXYZ[1] = planeXYZ[1];
                        modelXYZ[2] = planeXYZ[2];
                        modelXyzValidFlag = true;
                    }
                }
                    break;
            }

            if (modelXyzValidFlag) {
                m_mediaLeftDragWithCtrlModelXYZ[0] = modelXYZ[0];
                m_mediaLeftDragWithCtrlModelXYZ[1] = modelXYZ[1];
                m_mediaLeftDragWithCtrlModelXYZ[2] = modelXYZ[2];
                m_mediaLeftDragWithCtrlModelXYZValidFlag = true;
            }
        }
        browserTabContent->applyMediaMouseScaling(viewportContent,
                                                  mouseEvent.getPressedX(),
                                                  mouseEvent.getPressedY(),
                                                  mouseEvent.getDy(),
                                                  m_mediaLeftDragWithCtrlModelXYZ[0],
                                                  m_mediaLeftDragWithCtrlModelXYZ[1],
                                                  m_mediaLeftDragWithCtrlModelXYZValidFlag);
    }
    else {
        browserTabContent->applyMouseScaling(viewportContent,
                                             mouseEvent.getPressedX() - modelViewport[0],
                                             mouseEvent.getPressedY() - modelViewport[1],
                                             mouseEvent.getDx(),
                                             mouseEvent.getDy());
    }
    updateGraphics(mouseEvent);
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
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
    const float factor(viewportContent->getTranslationFactorForMousePanning());
    browserTabContent->applyMouseTranslation(viewportContent,
                                             mouseEvent.getPressedX(),
                                             mouseEvent.getPressedY(),
                                             mouseEvent.getX(),
                                             mouseEvent.getY(),
                                             static_cast<int32_t>(mouseEvent.getDx() * factor),
                                             static_cast<int32_t>(mouseEvent.getDy() * factor));
    updateGraphics(mouseEvent);
}

/**
 * Process a mouse left release event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftRelease(const MouseEvent& mouseEvent)
{
    m_mprCursorMode = VOLUME_MPR_CURSOR_MODE::INVALID;
    

    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
    if (browserTabContent->isChartTwoDisplayed()) {
        const int32_t x1(mouseEvent.getPressedX());
        const int32_t y1(mouseEvent.getPressedY());
        const int32_t x2(mouseEvent.getX());
        const int32_t y2(mouseEvent.getY());
        
        Matrix4x4 m1, m2;
        int32_t chartViewport[4];
        if (viewportContent->getChartDataMatricesAndViewport(m1,
                                                             m2,
                                                             chartViewport)) {
            browserTabContent->finalizeChartTwoAxesBoundSelection(chartViewport,
                                                                  x1, y1, x2, y2);
            updateGraphics(viewportContent);
        }
        else {
            CaretLogSevere("Chart viewport is invalid");
        }
    }
    else if (browserTabContent->isHistologyDisplayed()) {
        GraphicsRegionSelectionBox* selectionBox = browserTabContent->getMediaRegionSelectionBox();
        CaretAssert(selectionBox);
        
        int32_t viewport[4];
        viewportContent->getModelViewport(viewport);
        const GraphicsObjectToWindowTransform* transform = viewportContent->getHistologyGraphicsObjectToWindowTransform();
        
        const float vpMinX(viewport[0]);
        const float vpMaxX(viewport[0] + viewport[2]);
        const float vpMinY(viewport[1]);
        const float vpMaxY(viewport[1] + viewport[3]);
        const float vpZ(0.0);
        float bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
        transform->inverseTransformPoint(vpMinX, vpMinY, vpZ, bottomLeft);
        transform->inverseTransformPoint(vpMaxX, vpMinY, vpZ, bottomRight);
        transform->inverseTransformPoint(vpMaxX, vpMaxY, vpZ, topRight);
        transform->inverseTransformPoint(vpMinX, vpMaxY, vpZ, topLeft);
        
        BoundingBox windowBounds;
        windowBounds.set(bottomLeft, bottomRight, topRight, topLeft);
        
        std::array<float, 4> orthoBoundsLRBT(transform->getOrthoLRBT());
        BoundingBox orthoBounds;
        orthoBounds.set(orthoBoundsLRBT[0], orthoBoundsLRBT[1],
                        orthoBoundsLRBT[2], orthoBoundsLRBT[3],
                        vpZ, vpZ);
        
        switch (selectionBox->getStatus()) {
            case GraphicsRegionSelectionBox::Status::INVALID:
                break;
            case GraphicsRegionSelectionBox::Status::VALID:
            {
                ModelHistology* histologyModel = browserTabContent->getDisplayedHistologyModel();
                if (histologyModel != NULL) {
                    /*
                     * Zoom to selection region
                     */
                    browserTabContent->setHistologyViewToBounds(viewportContent,
                                                                &orthoBounds,
                                                                selectionBox);
                }
            }
                break;
        }
        
        selectionBox->setStatus(GraphicsRegionSelectionBox::Status::INVALID);
        
        /*
         * Update graphics.
         */
        updateGraphics(mouseEvent);
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else if (browserTabContent->isMediaDisplayed()) {
        GraphicsRegionSelectionBox* selectionBox = browserTabContent->getMediaRegionSelectionBox();
        CaretAssert(selectionBox);
        
        int32_t viewport[4];
        viewportContent->getModelViewport(viewport);
        const GraphicsObjectToWindowTransform* transform = viewportContent->getMediaGraphicsObjectToWindowTransform();
        
        const float vpMinX(viewport[0]);
        const float vpMaxX(viewport[0] + viewport[2]);
        const float vpMinY(viewport[1]);
        const float vpMaxY(viewport[1] + viewport[3]);
        const float vpZ(0.0);
        float bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
        transform->inverseTransformPoint(vpMinX, vpMinY, vpZ, bottomLeft);
        transform->inverseTransformPoint(vpMaxX, vpMinY, vpZ, bottomRight);
        transform->inverseTransformPoint(vpMaxX, vpMaxY, vpZ, topRight);
        transform->inverseTransformPoint(vpMinX, vpMaxY, vpZ, topLeft);
        
        BoundingBox windowBounds;
        windowBounds.set(bottomLeft, bottomRight, topRight, topLeft);
        
        std::array<float, 4> orthoBoundsLRBT(transform->getOrthoLRBT());
        BoundingBox orthoBounds;
        orthoBounds.set(orthoBoundsLRBT[0], orthoBoundsLRBT[1],
                        orthoBoundsLRBT[2], orthoBoundsLRBT[3],
                        vpZ, vpZ);
        
        switch (selectionBox->getStatus()) {
            case GraphicsRegionSelectionBox::Status::INVALID:
                break;
            case GraphicsRegionSelectionBox::Status::VALID:
            {
                ModelMedia* mediaModel = browserTabContent->getDisplayedMediaModel();
                if (mediaModel != NULL) {
                    /*
                     * Zoom to selection region
                     */
                    browserTabContent->setMediaViewToBounds(viewportContent,
                                                            &orthoBounds,
                                                            selectionBox);
                }
            }
                break;
        }
        
        selectionBox->setStatus(GraphicsRegionSelectionBox::Status::INVALID);

        /*
         * Update graphics.
         */
        updateGraphics(mouseEvent);
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

/**
 * Process a mouse left press event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseLeftPress(const MouseEvent& mouseEvent)
{
    m_mprCursorMode = getVolumeMprMouseMode(mouseEvent);
}

/**
 * @return MPR mouse mode based upon cursor position
 * @param mouseEvent
 *    A mouse event
 */
UserInputModeView::VOLUME_MPR_CURSOR_MODE
UserInputModeView::getVolumeMprMouseMode(const MouseEvent& mouseEvent)
{
    UserInputModeView::VOLUME_MPR_CURSOR_MODE mprModeOut(VOLUME_MPR_CURSOR_MODE::INVALID);
    
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    if (viewportContent == NULL) {
        return mprModeOut;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return mprModeOut;
    }
    
    if (browserTabContent->isVolumeMprDisplayed()) {
        BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
        SelectionItemVolumeMprCrosshair* crosshairID(openGLWidget->performIdentificationVolumeMprCrosshairs(mouseEvent.getX(), // mouseEvent.getPressedX(),
                                                                                                            mouseEvent.getY()));
        CaretAssert(crosshairID);
        if (crosshairID->isValid()) {
            if (crosshairID->isRotateAxisSelected()) {
                mprModeOut = VOLUME_MPR_CURSOR_MODE::ROTATE_SLICE;
            }
            else if (crosshairID->isSliceAxisSelected()) {
                mprModeOut = VOLUME_MPR_CURSOR_MODE::SELECT_SLICE;
            }
        }
        else {
            mprModeOut = VOLUME_MPR_CURSOR_MODE::SCROLL_SLICE;
        }
    }

    return mprModeOut;
}

/**
 * Process a mouse move with no buttons or keys down
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeView::mouseMove(const MouseEvent& mouseEvent)
{
    m_mprCursorMode = getVolumeMprMouseMode(mouseEvent);
}

/**
 * Process a gesture event (pinch zoom; or rotate)
 *
 * @param gestureEvent
 *     Gesture event information.
 */
void
UserInputModeView::gestureEvent(const GestureEvent& gestureEvent)
{
    BrainOpenGLViewportContent* viewportContent = gestureEvent.getViewportContent();
    if (viewportContent == NULL) {
        return;
    }
    
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    if (browserTabContent == NULL) {
        return;
    }
    
    switch (gestureEvent.getType()) {
        case GestureEvent::Type::PINCH:
        {
            float deltaY(gestureEvent.getValue());
            if (deltaY > 0.0) {
                float scaleFactor(0.0);
                if (deltaY > 1.0) {
                    scaleFactor = 2.0;
                }
                else if (deltaY < 1.0) {
                    scaleFactor = -2.0;
                }
                if (scaleFactor != 0.0) {
                    if (browserTabContent->isHistologyDisplayed()) {
                        const bool enableHistologyesturesFlag(false);
                        if (enableHistologyesturesFlag) {
                            BrainOpenGLWidget* openGLWidget = gestureEvent.getOpenGLWidget();
                            bool modelXyzValidFlag(false);
                            double modelXYZ[3];
                            SelectionItemHistologyCoordinate* histologyID = openGLWidget->performIdentificationHistologyPlaneCoordinate(gestureEvent.getStartCenterX(),
                                                                                                                                             gestureEvent.getStartCenterY());
                            CaretAssert(histologyID);
                            if (histologyID->isValid()) {
                                const HistologyCoordinate coordinate(histologyID->getCoordinate());
                                const Vector3D planeXYZ(coordinate.getPlaneXYZ());
                                modelXYZ[0] = planeXYZ[0];
                                modelXYZ[1] = planeXYZ[1];
                                modelXYZ[2] = planeXYZ[2];
                                modelXyzValidFlag = true;
                            }
                            if (modelXyzValidFlag) {
                                browserTabContent->applyHistologyMouseScaling(viewportContent,
                                                                              gestureEvent.getStartCenterX(),
                                                                              gestureEvent.getStartCenterX(),
                                                                              deltaY,
                                                                              modelXYZ[0],
                                                                              modelXYZ[1],
                                                                              true);
                            }
                        }
                    }
                    else if (browserTabContent->isMediaDisplayed()) {
                        const bool enableMediaGesturesFlag(false);
                        if (enableMediaGesturesFlag) {
                            BrainOpenGLWidget* openGLWidget = gestureEvent.getOpenGLWidget();
                            bool modelXyzValidFlag(false);
                            double modelXYZ[3];
                            switch (browserTabContent->getMediaDisplayCoordinateMode()) {
                                case MediaDisplayCoordinateModeEnum::PIXEL:
                                {
        
                                    SelectionItemMediaLogicalCoordinate* mediaID = openGLWidget->performIdentificationMediaLogicalCoordinate(gestureEvent.getStartCenterX(),
                                                                                                                                             gestureEvent.getStartCenterY());
                                    CaretAssert(mediaID);
                                    if (mediaID->isValid()) {
                                        mediaID->getModelXYZ(modelXYZ);
                                        modelXyzValidFlag = true;
                                    }
                                }
                                    break;
                                case MediaDisplayCoordinateModeEnum::PLANE:
                                {
                                    SelectionItemMediaPlaneCoordinate* mediaID = openGLWidget->performIdentificationMediaPlaneCoordinate(gestureEvent.getStartCenterX(),
                                                                                                                                         gestureEvent.getStartCenterY());
                                    CaretAssert(mediaID);
                                    if (mediaID->isValid()) {
                                        const Vector3D planeXYZ(mediaID->getPlaneCoordinate());
                                        modelXYZ[0] = planeXYZ[0];
                                        modelXYZ[1] = planeXYZ[1];
                                        modelXYZ[2] = planeXYZ[2];
                                        modelXyzValidFlag = true;
                                    }
                                    break;
                                }
                            }
                            if (modelXyzValidFlag) {
                                browserTabContent->applyMediaMouseScaling(viewportContent,
                                                                          gestureEvent.getStartCenterX(),
                                                                          gestureEvent.getStartCenterX(),
                                                                          deltaY,
                                                                          modelXYZ[0],
                                                                          modelXYZ[1],
                                                                          true);
                            }
                        }
                    }
                    else {
                        browserTabContent->applyMouseScaling(viewportContent,
                                                             gestureEvent.getStartCenterX(),
                                                             gestureEvent.getStartCenterX(),
                                                             0.0f,
                                                             scaleFactor);
                    }
                    updateGraphics(viewportContent);
                }
            }
        }
            break;
        case GestureEvent::Type::ROTATE:
            break;
    }
}

/**
 * Show a context menu (pop-up menu at mouse location)
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param menuPosition
 *     Point at which menu is displayed (passed to QMenu::exec())
 * @param openGLWidget
 *     OpenGL widget in which context menu is requested
 */
void
UserInputModeView::showContextMenu(const MouseEvent& mouseEvent,
                                   const QPoint& menuPosition,
                                   BrainOpenGLWidget* openGLWidget)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    BrowserTabContent* tabContent = viewportContent->getBrowserTabContent();
    if (tabContent == NULL) {
        return;
    }
    
    const int32_t mouseX = mouseEvent.getX();
    const int32_t mouseY = mouseEvent.getY();
    
    SelectionManager* idManager = openGLWidget->performIdentification(mouseX,
                                                                      mouseY,
                                                                      false);
    
    UserInputModeViewContextMenu contextMenu(mouseEvent,
                                             viewportContent,
                                             idManager,
                                             openGLWidget);
    contextMenu.exec(menuPosition);
}

/**
 * If this windows is yoked, issue an event to update other
 * windows that are using the same yoking.
 *
 * @param viewportContent
 *    Content of the viewport
 */
void
UserInputModeView::updateGraphics(const BrainOpenGLViewportContent* viewportContent)
{
    bool issuedYokeEvent = false;
    if (viewportContent != NULL) {
        BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
        const int32_t browserWindowIndex = viewportContent->getWindowIndex();
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(browserWindowIndex).getPointer());
        
        YokingGroupEnum::Enum brainYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
        YokingGroupEnum::Enum chartYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
        
        if (browserTabContent != NULL) {
            if (browserTabContent->isBrainModelYoked()) {
                brainYokingGroup = browserTabContent->getBrainModelYokingGroup();
                issuedYokeEvent = true;
            }
            if (browserTabContent->isChartModelYoked()) {
                chartYokingGroup = browserTabContent->getChartModelYokingGroup();
                issuedYokeEvent = true;
            }
            
            if (issuedYokeEvent) {
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                EventManager::get()->sendEvent(EventUpdateYokedWindows(brainYokingGroup,
                                                                       chartYokingGroup).getPointer());
            }
        }
    }
    
    /*
     * If not yoked, just need to update graphics.
     */
    if ( ! issuedYokeEvent) {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(viewportContent->getWindowIndex()).getPointer());
    }
}

/**
 * If this windows is yoked, issue an event to update other
 * windows that are using the same yoking.
 */
void
UserInputModeView::updateGraphics(const MouseEvent& mouseEvent)
{
    bool issuedYokeEvent = false;
    if (mouseEvent.getViewportContent() != NULL) {
        BrowserTabContent* browserTabContent = mouseEvent.getViewportContent()->getBrowserTabContent();
        const int32_t browserWindowIndex = mouseEvent.getBrowserWindowIndex();
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(browserWindowIndex).getPointer());
        
        YokingGroupEnum::Enum brainYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
        YokingGroupEnum::Enum chartYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
        
        if (browserTabContent != NULL) {
            if (browserTabContent->isBrainModelYoked()) {
                brainYokingGroup = browserTabContent->getBrainModelYokingGroup();
                issuedYokeEvent = true;
            }
            if (browserTabContent->isChartModelYoked()) {
                chartYokingGroup = browserTabContent->getChartModelYokingGroup();
                issuedYokeEvent = true;
            }
            
            if (issuedYokeEvent) {
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                EventManager::get()->sendEvent(EventUpdateYokedWindows(brainYokingGroup,
                                                                       chartYokingGroup).getPointer());
            }
        }
    }
    
    /*
     * If not yoked, just need to update graphics.
     */
    if ( ! issuedYokeEvent) {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(mouseEvent.getBrowserWindowIndex()).getPointer());
    }
}

/**
 * Process a key press event
 *
 * @param keyEvent
 *     Key event information.
 * @return
 *     True if the input process recognized the key event
 *     and the key event SHOULD NOT be propagated to parent
 *     widgets
 */
bool
UserInputModeView::keyPressEvent(const KeyEvent& keyEvent)
{
    bool keyWasProcessedFlag(false);
    
    bool decrementFlag(false);
    bool incrementFlag(false);
    const int32_t keyCode = keyEvent.getKeyCode();
    switch (keyCode) {
        case Qt::Key_Right:
            incrementFlag = true;
            break;
        case Qt::Key_Left:
            decrementFlag = true;
            break;
        case Qt::Key_Up:
            incrementFlag = true;
            break;
        case Qt::Key_Down:
            decrementFlag = true;
            break;
    }
    
    if (decrementFlag
        || incrementFlag) {
        std::array<int32_t, 2> mouseXY;
        if (keyEvent.getMouseXY(mouseXY)) {
            /*
             * Increment/decrement selected point in line layer chart.
             * Identification will fail if chart is not visible.
             */
            SelectionManager* selectionManager = keyEvent.getOpenGLWidget()->performIdentification(mouseXY[0],
                                                                                                   mouseXY[1],
                                                                                                   false);
            
            SelectionItemChartTwoLineLayerVerticalNearest* layerSelection = selectionManager->getChartTwoLineLayerVerticalNearestIdentification();
            CaretAssert(layerSelection);
            if (layerSelection->isValid()) {
                ChartTwoOverlay* invalidChartOverlay(NULL);
                int32_t invalidLineSegmentIndex(-1);
                if (incrementFlag) {
                    processChartActiveLayerAction(ChartActiveLayerMode::INCREMENT,
                                                  invalidChartOverlay,
                                                  invalidLineSegmentIndex);
                }
                else if (decrementFlag) {
                    processChartActiveLayerAction(ChartActiveLayerMode::DECREMENT,
                                                  invalidChartOverlay,
                                                  invalidLineSegmentIndex);
                }
                else {
                    CaretAssertMessage(0, "Invalid increment/decrement");
                }
            }
        }
    }
    
    return keyWasProcessedFlag;
}

/**
 * Process a chart active layer action
 *
 * @param chartActiveMode
 *    The mode
 * @param chartOverlay
 *    The given chart overlay
 * @param pointIndex
 *    Index of point selected
 */
void
UserInputModeView::processChartActiveLayerAction(const ChartActiveLayerMode chartActiveMode,
                                                 ChartTwoOverlay* chartOverlay,
                                                 const int32_t pointIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = NULL;
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent != NULL) {
        chartOverlaySet = browserTabContent->getChartTwoOverlaySet();
    }

    if (chartOverlaySet != NULL) {
        switch (chartActiveMode) {
            case ChartActiveLayerMode::DECREMENT:
                chartOverlaySet->incrementOverlayActiveLineChartPoint(-1);
                break;
            case ChartActiveLayerMode::DESELECT_ALL:
                chartOverlaySet->selectOverlayActiveLineChart(NULL,
                                                              -1);
                break;
            case ChartActiveLayerMode::INCREMENT:
                chartOverlaySet->incrementOverlayActiveLineChartPoint(1);
                break;
            case ChartActiveLayerMode::SELECT:
                chartOverlaySet->selectOverlayActiveLineChart(chartOverlay,
                                                              pointIndex);
                break;
        }
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addToolBox().getPointer());    
}

