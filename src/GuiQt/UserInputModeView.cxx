
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
#include "ChartTwoOverlaySet.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUpdateYokedWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MouseEvent.h"
#include "SelectionItemChartTwoLabel.h"
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
 */
UserInputModeView::UserInputModeView()
: UserInputModeAbstract(UserInputModeEnum::VIEW)
{
    
}

/**
 * Constructor for subclasses.
 *
 * @param inputMode
 *    Subclass' input mode.
 */
UserInputModeView::UserInputModeView(const UserInputModeEnum::Enum inputMode)
: UserInputModeAbstract(inputMode)
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
         * Keep the main window as the active window NOT the identification window
         */
        openGLWidget->parentWidget()->activateWindow();
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
    
    browserTabContent->applyMouseScaling(mouseEvent.getDx(), mouseEvent.getDy());
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
    
    UserInputModeViewContextMenu contextMenu(viewportContent,
                                             idManager,
                                             openGLWidget);
    contextMenu.exec(menuPosition);
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

