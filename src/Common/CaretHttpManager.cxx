#include "CaretHttpManager.h"
#include "CaretAssert.h"

using namespace caret;
using namespace std;

CaretHttpManager* CaretHttpManager::m_singleton = NULL;

CaretHttpManager::CaretHttpManager()
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
        myReply = getHttpManager()->getQNetManager()->post(myRequest, myParams.toEncoded());
        break;
    case GET:
        myUrl = QUrl::fromUserInput(request.m_url);
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setUrl(myUrl);
        myReply = getHttpManager()->getQNetManager()->get(myRequest);
        break;
    case HEAD:
        myUrl = QUrl::fromUserInput(request.m_url);
        for (int32_t i = 0; i < (int32_t)request.m_arguments.size(); ++i)
        {
            myUrl.addQueryItem(request.m_arguments[i].first, request.m_arguments[i].second);
        }
        myRequest.setUrl(myUrl);
        myReply = getHttpManager()->getQNetManager()->head(myRequest);
        break;
    default:
        CaretAssertMessage(false, "Unrecognized http request method");
    };
    QObject::connect(myReply, SIGNAL(finished()), &myLoop, SLOT(quit()));
    myLoop.exec();//this is safe, because nothing will hand this thread events except queued through this thread's event mechanism
    response.m_method = request.m_method;//so, they can only be delivered after myLoop.exec() starts
    response.m_ok = false;
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
}
