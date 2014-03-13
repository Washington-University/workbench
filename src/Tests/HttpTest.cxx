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
#include "HttpTest.h"
#include "CaretHttpManager.h"

using namespace caret;

HttpTest::HttpTest(const AString& identifier) : TestInterface(identifier)
{
}

void HttpTest::execute()
{
    CaretHttpManager* myMgr = CaretHttpManager::getHttpManager();
    CaretHttpRequest myReq;
    myReq.m_url = "http://www.google.com/";
    myReq.m_method = CaretHttpManager::GET;
    CaretHttpResponse myResp;
    myMgr->httpRequest(myReq, myResp);
    if (!myResp.m_ok)
    {
        setFailed(AString("retrieving google.com failed, code ") + AString::number(myResp.m_responseCode));
    }
    if (myResp.m_ok && myResp.m_responseCode != 200)
    {
        setFailed(AString("response said OK, but status code is ") + AString::number(myResp.m_responseCode));
    }
}
