
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
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
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
}
