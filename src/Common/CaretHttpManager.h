#ifndef __CARET_HTTP_MANAGER_H__
#define __CARET_HTTP_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <QtNetwork>
#include <vector>
#include "stdint.h"
#include "AString.h"

namespace caret {

    class CaretHttpRequest;
    class CaretHttpResponse;

    class CaretHttpManager
    {
        QNetworkAccessManager m_netMgr;
        CaretHttpManager();
        static CaretHttpManager* m_singleton;
    public:
        enum Method
        {
            GET,
            POST,
            HEAD
        };
        static CaretHttpManager* getHttpManager();
        static void deleteHttpManager();
        static void httpRequest(const CaretHttpRequest& request, CaretHttpResponse& response);
        static QNetworkAccessManager* getQNetManager();
    };

    struct CaretHttpResponse
    {
        CaretHttpManager::Method m_method;
        std::vector<char> m_body;
        bool m_ok;
        int32_t m_responseCode;
    };

    struct CaretHttpRequest
    {
        CaretHttpManager::Method m_method;
        AString m_url;
        std::vector<std::pair<AString, AString> > m_arguments;
    };

}

#endif // __CARET_HTTP_MANAGER_H__
