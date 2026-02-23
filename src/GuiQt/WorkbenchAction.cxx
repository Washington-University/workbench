
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __WORKBENCH_ACTION_DECLARE__
#include "WorkbenchAction.h"
#undef __WORKBENCH_ACTION_DECLARE__

#include <QEvent>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventDarkLightColorSchemeModeChanged.h"
#include "EventDarkLightColorSchemeModeGet.h"
#include "EventManager.h"
#include "WorkbenchIconTypeLoader.h"

using namespace caret;
    
/**
 * \class caret::WorkbenchAction 
 * \brief Extends QAction to work with light/dark mode for the icon
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param iconType
 *    Type of icon for tool button
 * @param parent
 *    Parent widget.  Required to ensure that this gets destroyed to event listeners removed
 */
WorkbenchAction::WorkbenchAction(const WorkbenchIconTypeEnum::Enum iconType,
                                 QObject* parent)
: QAction(parent),
m_iconType(iconType)
{
    static bool firstFlag(true);
    if (firstFlag) {
        firstFlag = false;
    }

    if (isColorIcon(iconType)) {
        /*
         * Color icons are the same for both light and dark
         */
        m_darkPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                      GuiDarkLightColorSchemeModeEnum::LIGHT);
    }
    else {
        m_darkPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                      GuiDarkLightColorSchemeModeEnum::DARK);
    }
    m_lightPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                  GuiDarkLightColorSchemeModeEnum::LIGHT);

    updateForDarkLightColorScheme(getCurrentDarkLightColorSchemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_CHANGED);
    
    s_allWorkbenchActions.insert(this);
}

/**
 * Destructor.
 */
WorkbenchAction::~WorkbenchAction()
{
    EventManager::get()->removeAllEventsFromListener(this);
    s_allWorkbenchActions.erase(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
WorkbenchAction::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_CHANGED) {
        EventDarkLightColorSchemeModeChanged* colorSchemeChangedEvent(dynamic_cast<EventDarkLightColorSchemeModeChanged*>(event));
        CaretAssert(colorSchemeChangedEvent);
        
        const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode(colorSchemeChangedEvent->getDarkLightColorSchemeMode());
        updateForDarkLightColorScheme(darkLightColorSchemeMode);
    }
}

/**
 * Update the button for the given dark / light color scheme
 *
 * @param darkLightColorSchemeMode
 *    The dark / light color scheme for the button
 */
void
WorkbenchAction::updateForDarkLightColorScheme(const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode)
{
    switch (darkLightColorSchemeMode) {
        case GuiDarkLightColorSchemeModeEnum::SYSTEM:
            CaretAssert(0);
            setIcon(m_lightPixmap);
            break;
        case GuiDarkLightColorSchemeModeEnum::DARK:
            setIcon(m_darkPixmap);
            break;
        case GuiDarkLightColorSchemeModeEnum::LIGHT:
            setIcon(m_lightPixmap);
            break;
    }
}

/**
 * @return The current dark/light color scheme
 */
GuiDarkLightColorSchemeModeEnum::Enum
WorkbenchAction::getCurrentDarkLightColorSchemeMode() const
{
    EventDarkLightColorSchemeModeGet colorSchemeGetEvent;
    EventManager::get()->sendEvent(colorSchemeGetEvent.getPointer());
    return colorSchemeGetEvent.getDarkLightColorSchemeMode();
}

void
WorkbenchAction::printLeftoverWorkbenchActions()
{
    if (s_allWorkbenchActions.size() == 0) {
        std::cout << "All Workbench Actions were deleted" << std::endl;
    }
    for (WorkbenchAction* wa : s_allWorkbenchActions) {
        std::cout << WorkbenchIconTypeEnum::toName(wa->m_iconType) << std::endl;
    }
}

/**
 * @return True if the icon is a color icon and the same for light and dark
 * @param iconType
 *    The icon type
 */
bool
WorkbenchAction::isColorIcon(const WorkbenchIconTypeEnum::Enum iconType)
{
    bool colorFlag(false);
    
    switch (iconType) {
        case WorkbenchIconTypeEnum::NO_ICON:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_DELETE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_DOWN:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_UP:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_ARROW:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_BOX:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_IMAGE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_LINE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_MARKER:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_OVAL:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYHEDRON:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYGON:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYLINE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_TEXT:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_CHART:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_HISTOLOGY:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_STEREOTAXIC:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_SURFACE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_TAB:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_WINDOW:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ORIENTATION_HORIZONTAL:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ORIENTATION_VERTICAL:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_CENTER:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_LEFT:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_BOTTOM:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_MIDDLE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_TOP:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_ANTERIOR:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_DORSAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT_LATERAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT_MEDIAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_POSTERIOR:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REDO:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REGION:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_RIGHT:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_RIGHT_LATERAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_RIGHT_MEDIAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_UNDO:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_VENTRAL:
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_COLOR_BAR:
            colorFlag = true;
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_CONSTRUCT:
            colorFlag = true;
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_WRENCH:
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_DIALOG_FAVORITE_FILLED:
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_DIALOG_FAVORITE_OUTLINE:
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_DIALOG_FORGET_BLACK:
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_DIALOG_FORGET_RED:
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_DIALOG_SHARE:
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_HCP_IMAGE:
            colorFlag = true;
            break;
        case WorkbenchIconTypeEnum::RECENT_FILES_X_IMAGE:
            break;
        case WorkbenchIconTypeEnum::SCENE_DIALOG_CAUTION:
            colorFlag = true;
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_DELETE:
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_LOAD:
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_OPTIONS:
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_RELOAD:
            break;
        case WorkbenchIconTypeEnum::TABBAR_DATA_TOOLTIPS:
            break;
        case WorkbenchIconTypeEnum::TABBAR_FEATURES:
            break;
        case WorkbenchIconTypeEnum::TABBAR_HELP:
            break;
        case WorkbenchIconTypeEnum::TABBAR_IDENTIFY_BRAINORDINATE:
            break;
        case WorkbenchIconTypeEnum::TABBAR_INFORMATION:
            break;
        case WorkbenchIconTypeEnum::TABBAR_MACROS_SCROLL:
            break;
        case WorkbenchIconTypeEnum::TABBAR_MOVIE:
            break;
        case WorkbenchIconTypeEnum::TABBAR_OVERLAYS:
            break;
        case WorkbenchIconTypeEnum::TABBAR_SCENES_CLAP_BOARD:
            break;
        case WorkbenchIconTypeEnum::TABBAR_TOOLBAR:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_LIGHT_BULB:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_RELOCK_ARROW:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_RULER:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_SCISSORS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_AXIAL:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIRS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL:
            break;
    }

    return colorFlag;
}

