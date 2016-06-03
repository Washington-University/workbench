#ifndef __HTTP_COMMUNICATOR_RESULT_H__
#define __HTTP_COMMUNICATOR_RESULT_H__

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

#include <map>

#include "CaretObject.h"



namespace caret {

    class HttpCommunicatorResult : public CaretObject {
        
    public:
        HttpCommunicatorResult();
        
        HttpCommunicatorResult(const int32_t httpCode,
                               const std::map<AString, AString>& headers,
                               const AString& content);
        
        virtual ~HttpCommunicatorResult();
        
        bool isValid() const;
        
        int32_t getHttpCode() const;
        
        std::map<AString, AString> getHeaders() const;
        
        AString getContent() const;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        HttpCommunicatorResult(const HttpCommunicatorResult& obj);
        
        HttpCommunicatorResult& operator=(const HttpCommunicatorResult& obj);
        
        int32_t m_httpCode;
        
        std::map<AString, AString> m_headers;
        
        AString m_content;
        
        const bool m_valid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HTTP_COMMUNICATOR_RESULT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HTTP_COMMUNICATOR_RESULT_DECLARE__

} // namespace
#endif  //__HTTP_COMMUNICATOR_RESULT_H__
