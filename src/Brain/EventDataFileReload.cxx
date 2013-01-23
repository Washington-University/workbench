
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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


