
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __HTTP_COMMUNICATOR_PROGRESS_DECLARE__
#include "HttpCommunicatorProgress.h"
#undef __HTTP_COMMUNICATOR_PROGRESS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::HttpCommunicatorProgress 
 * \brief Contains information about HTTP progress
 * \ingroup Common
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
HttpCommunicatorProgress::HttpCommunicatorProgress(HttpCommunicator* httpCommunicator,
                                                   const Status status,
                                                   const AString& progressMessage,
                                                   const int32_t& progressMinimum,
                                                   const int32_t& progressMaximum,
                                                   const int32_t& progressValue)
: CaretObject(),
m_httpCommunicator(httpCommunicator),
m_status(status),
m_progressMessage(progressMessage),
m_progressMinimum(progressMinimum),
m_progressMaximum(progressMaximum),
m_progressValue(progressValue),
m_cancelled(false)
{
    
}

/**
 * Destructor.
 */
HttpCommunicatorProgress::~HttpCommunicatorProgress()
{
}

HttpCommunicator*
HttpCommunicatorProgress::getHttpCommunicator()
{
    return m_httpCommunicator;
}

HttpCommunicatorProgress::Status
HttpCommunicatorProgress::getStatus() const
{
    return m_status;
}

AString
HttpCommunicatorProgress::getProgressMessage() const
{
    return m_progressMessage;
}

int32_t
HttpCommunicatorProgress::getProgressMinimum() const
{
    return m_progressMinimum;
}

int32_t
HttpCommunicatorProgress::getProgressMaximum() const
{
    return m_progressMaximum;
}

int32_t
HttpCommunicatorProgress::getProgressValue() const
{
    return m_progressValue;
}

bool
HttpCommunicatorProgress::isCancelled() const
{
    return m_cancelled;
}

void HttpCommunicatorProgress::setCancelled(const bool cancelled)
{
    m_cancelled = cancelled;
}


