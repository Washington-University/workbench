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

#include "CaretHttpManager.h"
#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CaretLogger.h"
#include "NetworkException.h"
#include <QNetworkRequest>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif // QT_VERSION

using namespace caret;
using namespace std;

CaretHttpManager* CaretHttpManager::m_singleton = NULL;

CaretHttpManager::CaretHttpManager() : QObject()
{
    connect(&m_netMgr,
        SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
        this,
        SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));
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

static QString
caretHttpRequestToName(const CaretHttpRequest &caretHttpRequest)
{
    QString name("Unknown");
    
    switch (caretHttpRequest.m_method) {
        case CaretHttpManager::GET:
            name = "GET";
            break;
        case CaretHttpManager::HEAD:
            name = "HEAD";
            break;
        case CaretHttpManager::POST_ARGUMENTS:
            name = "POST ARGUMENTS";
            break;
        case CaretHttpManager::POST_FILE:
            name = "POST FILE";
            break;
    }
    return name;
}

static void
logHeadersFromRequest(const QNetworkRequest& request,
                      const CaretHttpRequest &caretHttpRequest)
{
    AString infoText;
    infoText.appendWithNewLine("Request " + caretHttpRequestToName(caretHttpRequest) + ": " + request.url().toString());
    infoText.appendWithNewLine("    Headers: ");

    QList<QByteArray> readHeaderList = request.rawHeaderList();
    const int numItems = readHeaderList.size();
    if (numItems > 0) {
        for (int32_t i = 0; i < numItems; i++) {
            const QByteArray headerName = readHeaderList.at(i);
            if ( ! headerName.isEmpty()) {
                const QByteArray headerValue = request.rawHeader(headerName);
                infoText.appendWithNewLine("        Name: " + QString(headerName));
                infoText.appendWithNewLine("        Value: " + QString(headerValue));
            }
        }
    }
    else {
        infoText.appendWithNewLine("        Contains no headers");
    }
    
    CaretLogFine(infoText);
}

void
CaretHttpManager::getHeaders(const QNetworkReply& reply,
                             std::map<AString, AString>& headersOut)
{
    QList<QByteArray> readHeaderList = reply.rawHeaderList();
    const int numItems = readHeaderList.size();
    if (numItems > 0) {
        for (int32_t i = 0; i < numItems; i++) {
            const QByteArray headerName = readHeaderList.at(i);
            if ( ! headerName.isEmpty()) {
                const QByteArray headerValue = reply.rawHeader(headerName);
                headersOut.insert(make_pair(QString(headerName), QString(headerValue)));
            }
        }
    }
}

static void
logHeadersFromReply(const QNetworkReply& reply,
                    const CaretHttpRequest &caretHttpRequest,
                    const CaretHttpResponse &caretHttpResponse)
{
    AString infoText;
    infoText.appendWithNewLine("Reply " + caretHttpRequestToName(caretHttpRequest) + " URL (" + QString::number(caretHttpResponse.m_responseCode) + ") Header: ");
    if ( ! caretHttpResponse.m_responseCodeValid) {
        infoText.appendWithNewLine("RESPONSE CODE IS NOT VALID.");
    }
    const QNetworkReply::NetworkError networkErrorCode = reply.error();
    if (networkErrorCode != QNetworkReply::NoError) {
        infoText.appendWithNewLine("Network Error Code (See QNetworkReply::NetworkError for description): "
                                   + QString::number(static_cast<int>(networkErrorCode)));
    }
    QList<QByteArray> readHeaderList = reply.rawHeaderList();
    const int numItems = readHeaderList.size();
    if (numItems > 0) {
        for (int32_t i = 0; i < numItems; i++) {
            const QByteArray headerName = readHeaderList.at(i);
            if ( ! headerName.isEmpty()) {
                const QByteArray headerValue = reply.rawHeader(headerName);
                infoText.appendWithNewLine("      Name: " + QString(headerName));
                infoText.appendWithNewLine("      Value: " + QString(headerValue));
            }
        }
    }
    else {
        infoText.appendWithNewLine("    Contains no headers");
    }
    
    CaretLogFine(infoText);
}

void CaretHttpManager::httpRequest(const CaretHttpRequest &request, CaretHttpResponse &response)
{
//    /*
//     * Clear headers from response here only.
//     * For logging into Spring, it always redirects and the 
//     * JSESSIONID is provided in the first reply but not
//     * in the reply from the redirect.
//     */
//    response.m_headers.clear();

    /*
     * Code for redirection is from the Qt HTTP example httpwindow.cpp. 
     */
    httpRequestPrivate(request,
                       response);
    if (response.m_responseCode == 302) {
        if (response.m_redirectionUrlValid) {
            CaretHttpRequest redirectedRequest = request;
            redirectedRequest.m_url = response.m_redirectionUrl.toString();

            CaretLogFine("Received and processing redirection request from "
                           + request.m_url
                           + " to "
                           + redirectedRequest.m_url);
            
            CaretHttpResponse redirectedResponse;
            redirectedResponse.m_headers = response.m_headers; // copy headers from first attempt (may have JSESSIONID)
            httpRequestPrivate(redirectedRequest,
                               redirectedResponse);
            /* need header from orginal and redirected response */
            std::map<AString,AString> allHeaders = response.m_headers;
            allHeaders.insert(redirectedRequest.m_headers.begin(),
                                redirectedRequest.m_headers.end());
            response = redirectedResponse;
            response.m_headers = allHeaders;
            if (redirectedResponse.m_body.size() > 0) {
                redirectedResponse.m_body.push_back(0);
                QString str(&redirectedResponse.m_body[0]);
                CaretLogFine("Redirected response content: " + str);
                //std::cout << "Redirected response content: " << qPrintable(str) << std::endl;
            }
        }
    }
}

void CaretHttpManager::httpRequestPrivate(const CaretHttpRequest &request, CaretHttpResponse &response)
{
    QEventLoop myLoop;
    QNetworkRequest myRequest;
    myRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    CaretHttpManager* myCaretMgr = getHttpManager();
    AString myServerString = getServerString(request.m_url);
    bool have_auth = false;
    for (int i = 0; i < (int)myCaretMgr->m_authList.size(); ++i)
    {
        if (myServerString == myCaretMgr->m_authList[i].m_serverString)
        {
            QString unencoded = myCaretMgr->m_authList[i].m_user + ":" + myCaretMgr->m_authList[i].m_pass;
            myRequest.setRawHeader("Authorization", "Basic " + unencoded.toLocal8Bit().toBase64());
            CaretLogFine("Found auth for URL " + request.m_url);
            have_auth = true;
            break;
        }
    }
    if (!have_auth)
    {
        CaretLogFine("NO AUTH FOUND for URL " + request.m_url);
    }
    QNetworkReply* myReply = NULL;
/*
 * QUrl::addQueryItem() deprecated in Qt5: http://wiki.qt.io/Transition_from_Qt_4.x_to_Qt5
 */
    QUrl myUrl = QUrl::fromUserInput(request.m_url);
#if QT_VERSION >= 0x050000
    QUrlQuery myUrlQuery(QUrl::fromUserInput(request.m_url));
    for (int32_t i = 0; i < (int32_t)request.m_queries.size(); ++i)
    {
        myUrlQuery.addQueryItem(request.m_queries[i].first, request.m_queries[i].second);
    }
    myUrl.setQuery(myUrlQuery);
#else // QT_VERSION
    for (int32_t i = 0; i < (int32_t)request.m_queries.size(); ++i)
    {
        myUrl.addQueryItem(request.m_queries[i].first, request.m_queries[i].second);
    }
#endif // QT_VERSION
    QNetworkAccessManager* myQNetMgr = &(myCaretMgr->m_netMgr);
    bool first = true;
    QByteArray postData;
    CaretPointer<QFile> postUploadFile; // file needs to remain in scope until upload finished
    switch (request.m_method)
    {
    case POST_ARGUMENTS:
        {
            for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
            {
                if (!first) postData += "&";
                if (request.m_arguments[i].second == "")
                {
                    postData += request.m_arguments[i].first;
                } else {
                    //postData += request.m_arguments[i].first + "=" + request.m_arguments[i].second;
                    postData += QUrl::toPercentEncoding(request.m_arguments[i].first) + "=" + QUrl::toPercentEncoding(request.m_arguments[i].second);
                }
                first = false;
            }
            myRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            for (std::map<AString,AString>::const_iterator headerIter = request.m_headers.begin();
                 headerIter != request.m_headers.end();
                 headerIter++) {
                myRequest.setRawHeader(headerIter->first.toLatin1(), headerIter->second.toLatin1());
            }
#if QT_VERSION >= 0x050000
            //myUrl.setQuery(myUrlQuery);
#endif // QT_VERSION
            myRequest.setUrl(myUrl);
            myReply = myQNetMgr->post(myRequest, postData);
            CaretLogFine("POST ARGUMENTS URL: " + myUrl.toString());
        }
        break;
    case POST_FILE:
        {
            postUploadFile.grabNew(new QFile(request.m_uploadFileName));
            if (postUploadFile->open(QFile::ReadOnly)) {
                //myRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                for (std::map<AString,AString>::const_iterator headerIter = request.m_headers.begin();
                     headerIter != request.m_headers.end();
                     headerIter++) {
                    myRequest.setRawHeader(headerIter->first.toLatin1(), headerIter->second.toLatin1());
//                    std::cout << "POST FILE header: " << qPrintable(headerIter->first)
//                    << ": " << qPrintable(headerIter->second) << std::endl;
                }
#if QT_VERSION >= 0x050000
                myUrl.setQuery(myUrlQuery);
#endif // QT_VERSION
                myRequest.setUrl(myUrl);
                myReply = myQNetMgr->post(myRequest, postUploadFile);
                CaretLogFine("POST FILE URL: " + myUrl.toString());
            }
            else {
                
            }
        }
        break;
    case GET:
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
#if QT_VERSION >= 0x050000
            myUrlQuery.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
#else // QT_VERSION
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
#endif // QT_VERSION
        }
#if QT_VERSION >= 0x050000
        myUrl.setQuery(myUrlQuery);
#endif // QT_VERSION
        myRequest.setUrl(myUrl);
        CaretLogFine("GET URL: " + myUrl.toString());
        myReply = myQNetMgr->get(myRequest);
        break;
    case HEAD:
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
#if QT_VERSION >= 0x050000
            myUrlQuery.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
#else // QT_VERSION
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
#endif // QT_VERSION
        }
#if QT_VERSION >= 0x050000
        myUrl.setQuery(myUrlQuery);
#endif // QT_VERSION
        myRequest.setUrl(myUrl);
        CaretLogFine("HEAD URL: " + myUrl.toString());
        myReply = myQNetMgr->head(myRequest);
        break;
    };
    //QObject::connect(myReply, SIGNAL(sslErrors(QList<QSslError>)), &myLoop, SLOT(quit()));
    //QObject::connect(myQNetMgr, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), myCaretMgr, SLOT(authenticationCallback(QNetworkReply*,QAuthenticator*)));
    QObject::connect(myReply, SIGNAL(finished()), &myLoop, SLOT(quit()));//this is safe, because nothing will hand this thread events except queued through this thread's event mechanism
    /*QObject::connect(myReply,
        SIGNAL(sslErrors(const QList<QSslError> & )),
        CaretHttpManager::getHttpManager(),
        SLOT(handleSslErrors(const QList<QSslError> & )));//*/
    myLoop.exec();//so, they can only be delivered after myLoop.exec() starts
    response.m_method = request.m_method;
    response.m_ok = false;
    response.m_redirectionUrlValid = false;
    response.m_responseCode = -1;
    response.m_responseCodeValid = false;
    response.m_headers.clear();
    const QVariant responseCodeVariant = myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if ( ! responseCodeVariant.isNull()) {
        response.m_responseCode = myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        response.m_responseCodeValid = true;
    }
    if (response.m_responseCode == 200)
    {
        response.m_ok = true;
    }
    else {
        QVariant redirectionTarget = myReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if ( ! redirectionTarget.isNull()) {
            response.m_redirectionUrl = myUrl.resolved(redirectionTarget.toUrl());
            if (response.m_redirectionUrl.isValid()) {
                response.m_redirectionUrlValid = true;
            }
        }
    }
    
    getHeaders(*myReply, response.m_headers);
    
    const bool showHeaderValues = false;
    if (showHeaderValues
        || (response.m_responseCode != 200)) {
        logHeadersFromRequest(myRequest,
                              request);
        logHeadersFromReply(*myReply,
                            request,
                            response);
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
    CaretHttpManager* myCaretMgr = getHttpManager();
    AString myServerString = getServerString(url);
    CaretLogFine("Setting auth for server " + myServerString);
    for (int i = 0; i < (int)myCaretMgr->m_authList.size(); ++i)
    {
        if (myServerString == myCaretMgr->m_authList[i].m_serverString)
        {//for the moment, only allow one auth token per server in one instance of caret, so replace
            myCaretMgr->m_authList[i].m_user = user;
            myCaretMgr->m_authList[i].m_pass = password;
            return;
        }
    }//not found, need to add
    AuthEntry myAuth;
    myAuth.m_serverString = myServerString;
    myAuth.m_user = user;
    myAuth.m_pass = password;
    myCaretMgr->m_authList.push_back(myAuth);
}

void CaretHttpManager::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &/*errors*/)
{
    /*qDebug() << "handleSslErrors: ";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error: " << e;
    }*/

    reply->ignoreSslErrors();
}

/*void CaretHttpManager::authenticationCallback(QNetworkReply* reply, QAuthenticator* authenticator)
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
}//*/ //currently not used, because callback doesn't work for the way xnat is set up, and doesn't fit well with synchronous requests

AString CaretHttpManager::getServerString(const AString& url)
{
    QUrl fullURL = QUrl::fromUserInput(url);
    AString ret = fullURL.toEncoded(QUrl::RemovePath | QUrl::StripTrailingSlash | QUrl::RemoveQuery | QUrl::RemoveUserInfo);
    return ret;
}
