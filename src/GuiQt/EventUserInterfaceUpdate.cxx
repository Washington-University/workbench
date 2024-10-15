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

#include "EventUserInterfaceUpdate.h"

using namespace caret;

/**
 * \class caret::EventUserInterfaceUpdate 
 * \brief Event for updating the user-interface.
 * \ingroup GuiQt
 *
 * After the user executes an action, there is often the
 * need to update the user-interface.  Options are available
 * to limit the update for specific types of data.  However,
 * some receivers will update regardless of any specific
 * update type requests.  In addition, the update can
 * be targeted to a specific window.
 */

/**
 * Constructor.  By default, everything is updated.
 * The 'add...()' methods can be used to limit the 
 * types of data that get updated.
 */
EventUserInterfaceUpdate::EventUserInterfaceUpdate()
: Event(EventTypeEnum::EVENT_USER_INTERFACE_UPDATE)
{
    m_windowIndex = -1;
    m_isFirstUpdateType = true;
    this->setAll(true);
}

/*
 * Destructor.
 */
EventUserInterfaceUpdate::~EventUserInterfaceUpdate()
{
    
}

/**
 * @return Is the update for the given window?
 *
 * @param windowIndex
 *     Index of window.
 */
bool 
EventUserInterfaceUpdate::isUpdateForWindow(const int32_t windowIndex) const
{
    if (m_windowIndex < 0) {
        return true;
    }
    else if (m_windowIndex == windowIndex) {
        return true;
    }
    return false;
}

/**
 * Set the update so that it only updates a specific window.
 * 
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::setWindowIndex(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
    return *this;
}

/**
 * Set the status of all update types.
 * @param new selection status.
 */
void 
EventUserInterfaceUpdate::setAll(bool selected)
{
    m_borderUpdate       = selected;
    m_connectivityUpdate = selected;
    m_fociUpdate         = selected;
    m_surfaceUpdate      = selected;
    m_tabUpdate          = selected;
    m_toolBarUpdate      = selected;
    m_toolBoxUpdate      = selected;
}

/**
 * Add borders for update (borders have changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addBorder()
{
    addInitialization();
    m_borderUpdate = true;
    return *this;
}

/**
 * Add connectivity for update (connectivity has changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addConnectivity()
{
    addInitialization();
    m_connectivityUpdate = true;
    return *this;
}

/**
 * Add foci for update (foci have changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addFoci()
{
    addInitialization();
    m_fociUpdate = true;
    return *this;
}

/**
 * Add surface for update (surfaces have changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addSurface()
{
    addInitialization();
    m_surfaceUpdate = true;
    return *this;
}

/**
 * Add browser tabs for update (browser tabs have changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addTab()
{
    addInitialization();
    m_tabUpdate = true;
    return *this;
}

/**
 * Add toolbar for update (toolbar data has changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addToolBar()
{
    addInitialization();
    m_toolBarUpdate = true;
    return *this;
}

/**
 * Add toolbox for update (toolbox data has changed).
 * 
 * Note the first call to an 'add...()' method will
 * set all other updates to off.
 *
 * @return A reference to the instance so that the
 * request update calls can be chained.
 */
EventUserInterfaceUpdate& 
EventUserInterfaceUpdate::addToolBox()
{
    addInitialization();
    m_toolBoxUpdate = true;
    return *this;
}

/**
 * This is called by any of the 'add...() methods
 * and, if this is the first time called after
 * creation of an instance, it will turn off all
 * update types so that the 'add...()' only has
 * to turn on its update type.
 */
void 
EventUserInterfaceUpdate::addInitialization()
{
    if (m_isFirstUpdateType) {
        setAll(false);
        m_isFirstUpdateType = false;
    }
}


/**
 * @return Is border update requested.
 */
bool 
EventUserInterfaceUpdate::isBorderUpdate() const
{
    return m_borderUpdate;
}

/**
 * @return Is connectivity update requested.
 */
bool 
EventUserInterfaceUpdate::isConnectivityUpdate() const
{
    return m_connectivityUpdate;
}

/**
 * @return Is foci update requested.
 */
bool 
EventUserInterfaceUpdate::isFociUpdate() const
{
    return m_fociUpdate;
}

/**
 * @return Is surface update requested.
 */
bool 
EventUserInterfaceUpdate::isSurfaceUpdate() const
{
    return m_surfaceUpdate;
}

/**
 * @return Is tab update requested (browser tabs have changed)
 */
bool 
EventUserInterfaceUpdate::isTabUpdate() const
{
    return m_tabUpdate;
}

/**
 * @return Is toolbar update requested.
 */
bool 
EventUserInterfaceUpdate::isToolBarUpdate() const
{
    return m_toolBarUpdate;
}

/**
 * @return Is toolbox update requested.
 */
bool 
EventUserInterfaceUpdate::isToolBoxUpdate() const
{
    return m_toolBoxUpdate;
}


