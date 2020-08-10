
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
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUpdateYokedWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GestureEvent.h"
#include "GuiManager.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "SelectionItemChartTwoLabel.h"
#include "SelectionItemChartTwoLineLayerVerticalNearest.h"
#include "SelectionManager.h"
#include "UserInputModeViewContextMenu.h"
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
    
    return CursorEnum::CURSOR_DEFAULT;
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

    bool scrollVolumeSlicesFlag(false);
    if (browserTabContent->isVolumeSlicesDisplayed()) {
        switch (browserTabContent->getSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                scrollVolumeSlicesFlag = true;
                break;
        }
    }
    if (scrollVolumeSlicesFlag) {
        browserTabContent->applyMouseVolumeSliceIncrement(viewportContent,
                                                          mouseEvent.getPressedX(),
                                                          mouseEvent.getPressedY(),
                                                          mouseEvent.getDy());
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_UPDATE_VOLUME_SLICE_INDICES_COORDS_TOOLBAR);
    }
    else {
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
    
    browserTabContent->applyMouseScaling(viewportContent,
                                         mouseEvent.getPressedX(),
                                         mouseEvent.getPressedY(),
                                         mouseEvent.getDx(),
                                         mouseEvent.getDy());
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
    
    browserTabContent->applyMouseTranslation(viewportContent,
                                             mouseEvent.getPressedX(),
                                             mouseEvent.getPressedY(),
                                             mouseEvent.getDx(),
                                             mouseEvent.getDy());
    updateGraphics(mouseEvent);
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
                    browserTabContent->applyMouseScaling(viewportContent,
                                                         gestureEvent.getStartCenterX(),
                                                         gestureEvent.getStartCenterY(),
                                                         0.0f,
                                                         scaleFactor);
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

