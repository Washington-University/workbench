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

#include "Brain.h"
#include "EventUpdateInformationWindows.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "IdentifiedItemBase.h"

using namespace caret;
/**
 * \class caret::EventUpdateInformationWindows 
 * \brief Event for updating Information Windows
 * \ingroup GuiQt
 */

/**
 * Construct an event for update of information window with
 * content from the identification manager.  The given
 * text will be added to the information manager prior to 
 * update of information window.
 *
 * @param informationText
 *   Next text that will be added to the information manager.
 */
EventUpdateInformationWindows::EventUpdateInformationWindows(const AString& informationText)
: Event(EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS)
{
    if ( ! informationText.isEmpty()) {
        IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
        CaretAssert(idManager);
        idManager->addIdentifiedItem(new IdentifiedItemBase(informationText,
                                                            informationText));
    }
    
    m_important = true;
}

/**
 * Construct an event for update of information window with
 * content from the identification manager.
 */
EventUpdateInformationWindows::EventUpdateInformationWindows()
: Event(EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS)
{
    m_important = true;
}
/**
 *  Destructor.
 */
EventUpdateInformationWindows::~EventUpdateInformationWindows()
{
    
}

/**
 * Set the message as not important so that the toolbox
 * does NOT switch to the information panel.
 */
EventUpdateInformationWindows*
EventUpdateInformationWindows::setNotImportant()
{
    m_important = false;
    return this;
}

/**
 * @return Is this message important.  If so,
 * the Toolbox will switch to the information 
 * display.
 */
bool 
EventUpdateInformationWindows::isImportant() const
{
    return m_important;
}
