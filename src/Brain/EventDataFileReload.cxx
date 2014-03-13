
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

#define __EVENT_DATA_FILE_RELOAD_DECLARE__
#include "EventDataFileReload.h"
#undef __EVENT_DATA_FILE_RELOAD_DECLARE__

#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDataFileReload 
 * \brief Event for reloading a Caret Data File.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * Note: If reload fails, the caretDataFile WILL BE DELETED and the pointer
 * must no longer be deferenced.
 *
 * @param brain
 *    Brain into which file is reloaded.
 * @param caretDataFile
 *    Caret data file that is reloaded.
 */
EventDataFileReload::EventDataFileReload(Brain* brain,
                                         CaretDataFile* caretDataFile)
: Event(EventTypeEnum::EVENT_DATA_FILE_RELOAD),
m_brain(brain),
m_caretDataFile(caretDataFile)
{
    
}

/**
 * Destructor.
 */
EventDataFileReload::~EventDataFileReload()
{
    
}

/**
 * @return Brain into which file is loaded.
 */
Brain*
EventDataFileReload::getBrain()
{
    return this->m_brain;
}

/**
 * @return The Caret Data File that will be reloaded.
 */
CaretDataFile*
EventDataFileReload::getCaretDataFile()
{
    return m_caretDataFile;
}

/**
 * @return true if there was an error reloading the file, else false.
 */
bool
EventDataFileReload::isError() const
{
    const bool errorFlag = (m_errorMessage.isEmpty() == false);
    return errorFlag;
}

/**
 * @return The error message.
 */
AString
EventDataFileReload::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * Set there error message describing reloading error.
 *
 * @param errorMessage
 *    Message describing the error.
 */
void
EventDataFileReload::setErrorMessage(const AString& errorMessage)
{
    m_errorMessage = errorMessage;
}

/**
 * @return The username.
 */
AString
EventDataFileReload::getUsername() const
{
    return m_username;
}

/**
 * @return The password.
 */
AString
EventDataFileReload::getPassword() const
{
    return m_password;
}

/**
 * Set the username and password.
 *
 * @param username
 *     Name of user account.
 * @param password
 *     Password of user account.
 */
void
EventDataFileReload::setUsernameAndPassword(const AString& username,
                                            const AString& password)
{
    m_username = username;
    m_password = password;
}


