#include "WuQWebView.h"
#include <QNetworkReply>
#include <QtDebug>
#include <QSslError>

WuQWebView::WuQWebView(QWidget *parent) :
    QWebView(parent)
{
    connect(page()->networkAccessManager(),
            SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
            this,
            SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));  
}

void WuQWebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &/*errors*/)
{
    /*qDebug() << "handleSslErrors: ";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error: " << e;
    }*/

    reply->ignoreSslErrors();
}
