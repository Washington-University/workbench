
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

#define __EVENT_SCENE_ACTIVE_DECLARE__
#include "EventSceneActive.h"
#undef __EVENT_SCENE_ACTIVE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventSceneActive 
 * \brief Event to get/set the active scene
 * \ingroup Brain
 *
 * Event to get/set the active scene.
 * The "active scene" is the scene that was last loaded
 * or created.  If a spec file is loaded or the scene file
 * containing the scene is closed, the "active scene"
 * becomes invalid.
 */

/**
 * Constructor.
 *
 * @param mode
 *     The mode of this event
 */
EventSceneActive::EventSceneActive(const Mode mode)
: Event(EventTypeEnum::EVENT_SCENE_ACTIVE),
m_mode(mode)
{
    
}

/**
 * Destructor.
 */
EventSceneActive::~EventSceneActive()
{
}

/**
 * @return The mode.
 */
EventSceneActive::Mode
EventSceneActive::getMode() const
{
    return m_mode;
}
/**
 * @return The scene
 */
Scene*
EventSceneActive::getScene() const
{
    return m_scene;
}

/**
 * Set the scene
 *    New value for active scene.
 */
void
EventSceneActive::setScene(Scene* scene)
{
    m_scene = scene;
}

