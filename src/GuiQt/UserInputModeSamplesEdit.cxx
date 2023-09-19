
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

#define __USER_INPUT_MODE_SAMPLES_EDIT_DECLARE__
#include "UserInputModeSamplesEdit.h"
#undef __USER_INPUT_MODE_SAMPLES_EDIT_DECLARE__

#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "Brain.h"
#include "BrowserWindowContent.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabCloseInToolBar.h"
#include "EventBrowserTabSelectInWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
//#include "UserInputModeSamplesEditContextMenu.h"
#include "WuQMessageBox.h"

using namespace caret;
    
/**
 * \class caret::UserInputModeSamplesEdit
 * \brief User Input mode for editing samples
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserIndexIndex
 *     Index of window
 */
UserInputModeSamplesEdit::UserInputModeSamplesEdit(const int32_t browserIndexIndex)
: UserInputModeAnnotations(UserInputModeEnum::Enum::SAMPLES_EDITING,
                           browserIndexIndex)
{
    
}

/**
 * Destructor.
 */
UserInputModeSamplesEdit::~UserInputModeSamplesEdit()
{
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
UserInputModeSamplesEdit::showContextMenu(const MouseEvent& mouseEvent,
                                          const QPoint& menuPosition,
                                          BrainOpenGLWidget* openGLWidget)
{
    UserInputModeAnnotations::showContextMenu(mouseEvent,
                                              menuPosition,
                                              openGLWidget);
//    BrainBrowserWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(getBrowserWindowIndex());
//    CaretAssert(window);
//    BrowserWindowContent* windowContent = window->getBrowerWindowContent();
//    CaretAssert(windowContent);
//
//    bool showMenuFlag(false);
//    switch (windowContent->getTileTabsConfigurationMode()) {
//        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
//            break;
//        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
//            break;
//        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
//            showMenuFlag = true;
//            break;
//    }
//    if ( ! showMenuFlag) {
//        return;
//    }
//
//    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
//    BrowserTabContent* tabContent = viewportContent->getBrowserTabContent();
//
//    /*
//     * Select any annotation that is under the mouse.
//     * There might not be an annotation under the
//     * mouse and that is okay.
//     */
//    const bool singleSelectionModeFlag(false);
//    processMouseSelectAnnotation(mouseEvent,
//                                 false,
//                                 singleSelectionModeFlag);
//
//    UserInputModeSamplesEditContextMenu contextMenu(this,
//                                                    mouseEvent,
//                                                    tabContent,
//                                                    openGLWidget);
//    if (contextMenu.actions().size() > 0) {
//        contextMenu.exec(menuPosition);
//    }
}

/**
 * Delete all selected tabs
 */
void
UserInputModeSamplesEdit::deleteSelectedAnnotations()
{
//    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
//    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(getBrowserWindowIndex());
//    if ( ! selectedAnnotations.empty()) {
//        std::vector<BrowserTabContent*> tabs;
//        for (auto ann : selectedAnnotations) {
//            AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(ann);
//            if (abt != NULL) {
//                tabs.push_back(abt->getBrowserTabContent());
//            }
//        }
//
//        if ( ! tabs.empty()) {
//            BrainBrowserWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(getBrowserWindowIndex());
//            CaretAssert(window);
//
//            QString msg("Close the selected tab(s)?");
//            if (WuQMessageBox::warningOkCancel(window,
//                                               msg)) {
//                for (auto t : tabs) {
//                    EventBrowserTabCloseInToolBar deleteEvent(t,
//                                                              t->getTabNumber());
//                    EventManager::get()->sendEvent(deleteEvent.getPointer());
//                }
//            }
//        }
///* Will eventually use an undo command when "reopen closed browser tab" is implemented
//            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
//            undoCommand->setModeDeleteAnnotations(selectedAnnotations);
//            AString errorMessage;
//            if ( !  annotationManager->applyCommand(getUserInputMode(),
//                                                    undoCommand,
//                                                    errorMessage)) {
//                WuQMessageBox::errorOk(m_annotationToolsWidget,
//                                       errorMessage);
//            }
// */
//    }
//
//    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Process a mouse left click for selection mode.
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param shiftKeyDownFlag
 *     True if shift key is down.
 * @param singleSelectionModeFlag
 *     If true, deselect any other annotations so that only the annotation under mouse is selected
 */
void
UserInputModeSamplesEdit::processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                                                const bool shiftKeyDownFlag,
                                                                const bool singleSelectionModeFlag)
{
    /*
     * Do normal selection processing
     */
    UserInputModeAnnotations::processMouseSelectAnnotation(mouseEvent,
                                                           shiftKeyDownFlag,
                                                           singleSelectionModeFlag);
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(getUserInputMode());
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(getBrowserWindowIndex());
    if (selectedAnnotations.size() == 1) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        AnnotationBrowserTab* tabAnn = dynamic_cast<AnnotationBrowserTab*>(selectedAnnotations[0]);
        if (tabAnn != NULL) {
            const int32_t browserTabIndex = tabAnn->getTabIndex();
            
//            EventBrowserTabSelectInWindow selectTabEvent(browserTabIndex);
//            EventManager::get()->sendEvent(selectTabEvent.getPointer());
        }
    }
}
