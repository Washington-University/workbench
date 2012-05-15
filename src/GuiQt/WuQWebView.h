#ifndef WUQWEBVIEW_H
#define WUQWEBVIEW_H

#include <QWebView>

class WuQWebView : public QWebView
{
    Q_OBJECT

    public:
        WuQWebView(QWidget *parent = 0);
    private slots:
        void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
};

#endif // WUQWEBVIEW_H