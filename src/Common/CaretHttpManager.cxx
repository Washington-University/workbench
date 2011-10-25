#include "CaretHttpManager.h"
#include "CaretAssert.h"

using namespace caret;
using namespace std;

CaretHttpManager* CaretHttpManager::m_singleton = NULL;

CaretHttpManager::CaretHttpManager()
{
}

CaretHttpManager* CaretHttpManager::getUrlManager()
{
    if (m_singleton == NULL)
    {
        m_singleton = new CaretHttpManager();
    }
    return m_singleton;
}

void CaretHttpManager::deleteUrlManager()
{
    if (m_singleton != NULL)
    {
        delete m_singleton;
    }
}

QNetworkAccessManager* CaretHttpManager::getQNetManager()
{
    return &(getUrlManager()->m_netMgr);
}

void CaretHttpManager::httpRequest(const CaretHttpRequest &request, CaretHttpResponse &reply)
{
    QEventLoop myLoop;
    QNetworkRequest myRequest;
    QNetworkReply* myReply;
    QUrl myParams, myUrl;
    switch (request.m_method)
    {
    case POST:
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myParams.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        myRequest.setUrl(QUrl::fromUserInput(request.m_url));
        myReply = getUrlManager()->getQNetManager()->post(myRequest, myParams.toEncoded());
        break;
    case GET:
        myUrl = QUrl::fromUserInput(request.m_url);
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setUrl(myUrl);
        myReply = getUrlManager()->getQNetManager()->get(myRequest);
        break;
    case HEAD:
        myUrl = QUrl::fromUserInput(request.m_url);
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setUrl(myUrl);
        myReply = getUrlManager()->getQNetManager()->head(myRequest);
        break;
    default:
        CaretAssertMessage(false, "Unrecognized http request method");
    };
    QObject::connect(myReply, SIGNAL(finished()), &myLoop, SLOT(quit()));
    myLoop.exec();
    reply.m_method = request.m_method;
    reply.m_ok = false;
    reply.m_responseCode = myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply.m_responseCode == 200)
    {
        reply.m_ok = true;
    }
    QByteArray myBody = myReply->readAll();
    int64_t mySize = myBody.size();
    reply.m_body.resize(mySize);
    for (int64_t i = 0; i < mySize; ++i)
    {
        reply.m_body[i] = myBody[(int)i];//because QByteArray apparently just uses int - hope we won't need to transfer 2GB on a system that uses int32 for this
    }
}
