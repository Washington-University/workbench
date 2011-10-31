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
/*LICENSE_END*/

#include "CaretHttpManager.h"
#include "CaretAssert.h"
#include "NetworkException.h"
#include <QNetworkRequest>

using namespace caret;
using namespace std;

CaretHttpManager* CaretHttpManager::m_singleton = NULL;

CaretHttpManager::CaretHttpManager() : QObject()
{
}

CaretHttpManager* CaretHttpManager::getHttpManager()
{
    if (m_singleton == NULL)
    {
        m_singleton = new CaretHttpManager();
    }
    return m_singleton;
}

void CaretHttpManager::deleteHttpManager()
{
    if (m_singleton != NULL)
    {
        delete m_singleton;
    }
}

QNetworkAccessManager* CaretHttpManager::getQNetManager()
{
    return &(getHttpManager()->m_netMgr);
}

void CaretHttpManager::httpRequest(const CaretHttpRequest &request, CaretHttpResponse &response)
{
    QEventLoop myLoop;
    QNetworkRequest myRequest;
    myRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    QNetworkReply* myReply;
    QUrl myUrl = QUrl::fromUserInput(request.m_url);
    for (int32_t i = 0; i < (int32_t)request.m_queries.size(); ++i)
    {
        myUrl.addQueryItem(request.m_queries[i].first, request.m_queries[i].second);
    }
    CaretHttpManager* myCaretMgr = getHttpManager();
    QNetworkAccessManager* myQNetMgr = &(myCaretMgr->m_netMgr);
    bool first = true;
    QByteArray postData;
    switch (request.m_method)
    {
    case POST:
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            if (!first) postData += "&";
            if (request.m_arguments[i].second == "")
            {
                postData += request.m_arguments[i].first;
            } else {
                postData += request.m_arguments[i].first + "=" + request.m_arguments[i].second;
            }
            first = false;
        }
        myRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        myRequest.setUrl(myUrl);
        if (request.m_url == myCaretMgr->m_authURL)
        {
            if (!first) postData += "&";
            QString unencoded = myCaretMgr->m_authUser + ":" + myCaretMgr->m_authPass;
            myRequest.setRawHeader("Authorization", "Basic " + unencoded.toLocal8Bit().toBase64());
            //postData += "Authorization=Basic%20" + unencoded.toLocal8Bit().toBase64();
        }
        myReply = myQNetMgr->post(myRequest, postData);
        break;
    case GET:
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setUrl(myUrl);
        myReply = myQNetMgr->get(myRequest);
        break;
    case HEAD:
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setUrl(myUrl);
        myReply = myQNetMgr->head(myRequest);
        break;
    default:
        CaretAssertMessage(false, "Unrecognized http request method");
    };
    QObject::connect(myReply, SIGNAL(sslErrors(QList<QSslError>)), &myLoop, SLOT(quit()));
    QObject::connect(myQNetMgr, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), myCaretMgr, SLOT(authenticationCallback(QNetworkReply*,QAuthenticator*)));
    QObject::connect(myReply, SIGNAL(finished()), &myLoop, SLOT(quit()));//this is safe, because nothing will hand this thread events except queued through this thread's event mechanism
    myLoop.exec();//so, they can only be delivered after myLoop.exec() starts
    response.m_method = request.m_method;
    response.m_ok = false;
    response.m_responseCode = -1;
    response.m_responseCode = myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (response.m_responseCode == 200)
    {
        response.m_ok = true;
    }
    QByteArray myBody = myReply->readAll();
    int64_t mySize = myBody.size();
    response.m_body.reserve(mySize + 1);//make room for the null terminator that will sometimes be added to the end
    response.m_body.resize(mySize);//but don't set size to include it
    for (int64_t i = 0; i < mySize; ++i)
    {
        response.m_body[i] = myBody[(int)i];//because QByteArray apparently just uses int - hope we won't need to transfer 2GB on a system that uses int32 for this
    }
    delete myReply;
}

void CaretHttpManager::setAuthentication(const AString& url, const AString& user, const AString& password)
{
    CaretHttpManager* myMgr = getHttpManager();
    myMgr->m_authURL = url;//don't give the auth to a url that isn't the url intended to log into
    myMgr->m_authUser = user;
    myMgr->m_authPass = password;
}

void CaretHttpManager::authenticationCallback(QNetworkReply* reply, QAuthenticator* authenticator)
{
    if (reply->url() != QUrl::fromUserInput(m_authURL))//note: a redirect will cause this to break, this is ON PURPOSE so that auth isn't sent to a redirect
    {
        throw NetworkException("Authentication requested from different URL than authentication set for");
    }
    authenticator->setUser(m_authUser);
    authenticator->setPassword(m_authPass);
    m_authURL = "";
    m_authUser = "";
    m_authPass = "";
}
