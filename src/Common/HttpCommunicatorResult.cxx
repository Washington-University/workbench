
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

#define __HTTP_COMMUNICATOR_RESULT_DECLARE__
#include "HttpCommunicatorResult.h"
#undef __HTTP_COMMUNICATOR_RESULT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::HttpCommunicatorResult 
 * \brief Result from request to HttpCommunicator.
 * \ingroup Common
 */

/**
 * Constructor a invalid result.
 */
HttpCommunicatorResult::HttpCommunicatorResult()
: m_httpCode(-1),
m_valid(false)
{
    
}


/**
 * Constructor a valid result.
 *
 * @param httpCode
 *    The HTTP code.
 * @param headers
 *    The headers.
 * @param content
 *    The content.
 */
HttpCommunicatorResult::HttpCommunicatorResult(const int32_t httpCode,
                                               const std::map<AString, AString>& headers,
                                               const AString& content)
: CaretObject(),
m_httpCode(httpCode),
m_headers(headers),
m_content(content),
m_valid(true)
{
    
}

/**
 * Destructor.
 */
HttpCommunicatorResult::~HttpCommunicatorResult()
{
}

/**
 * @return Is result valid?
 */
bool
HttpCommunicatorResult::isValid() const
{
    return m_valid;
}


/**
 * @return The HTTP code.
 */
int32_t
HttpCommunicatorResult::getHttpCode() const
{
    return m_httpCode;
}

/**
 * @return The headers.
 */
std::map<AString, AString>
HttpCommunicatorResult::getHeaders() const
{
    return m_headers;
}

/**
 * @return The content.
 */
AString
HttpCommunicatorResult::getContent() const
{
    return m_content;
}
