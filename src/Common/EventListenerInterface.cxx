
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include "EventListenerInterface.h"

#include "CaretAssert.h"
#include "EventManager.h"
using namespace caret;


    
/**
 * \class caret::EventListenerInterface2 
 * \brief Interface for classes that listen for events
 * \ingroup Common
 */

/**
 * Constructor.
 */
EventListenerInterface::EventListenerInterface()
{
    
}

/**
 * Destructor.
 */
EventListenerInterface::~EventListenerInterface()
{
    EventManager::get()->verifyAllListenersRemoved(this);
}

