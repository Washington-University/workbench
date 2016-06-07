
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

#define __HTTP_COMMUNICATOR_DECLARE__
#include "HttpCommunicator.h"
#undef __HTTP_COMMUNICATOR_DECLARE__

#include <QApplication>
#include <QNetworkAccessManager>

#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "HttpCommunicatorProgress.h"
#include "HttpCommunicatorResult.h"

using namespace caret;


    
/**
 * \class caret::HttpCommunicator 
 * \brief I/O with HTTP
 * \ingroup Common
 *
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent of this instance.
 */
HttpCommunicator::HttpCommunicator(QObject* parent,
                                             const Mode mode,
                                             const AString& urlName,
                                             const std::map<AString, AString>& requestHeaders)
: QObject(parent),
m_mode(mode),
m_urlName(urlName),
m_requestHeaders(requestHeaders)
{
    QNetworkAccessManager* networkManager = CaretHttpManager::getQNetManager();
    CaretAssert(networkManager);

    m_networkRequest.grabNew(NULL);
    m_networkReply.grabNew(NULL);
    m_result.grabNew(new HttpCommunicatorResult());
    m_finishedFlag = false;
    m_finishedMutex.unlock();
}

/**
 * Destructor.
 */
HttpCommunicator::~HttpCommunicator()
{
}

/**
 * Create a new instance for uploading a file.
 */
HttpCommunicator*
HttpCommunicator::newInstancePostFile(QObject* parent,
                                                  const AString& urlName,
                                                  const AString& filename,
                                                  const std::map<AString, AString>& headers)
{
    HttpCommunicator* instance = new HttpCommunicator(parent,
                                                                MODE_POST_FILE,
                                                                urlName,
                                                                headers);
    
    instance->m_postFileInfo.m_filename = filename;
    
    return instance;
}

void
HttpCommunicator::runUntilDone()
{
    QNetworkAccessManager* networkManager = CaretHttpManager::getQNetManager();
    CaretAssert(networkManager);
    
    QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(slotNetworkAccessManagerFinished(QNetworkReply*)));
    
    m_networkRequest.grabNew(new QNetworkRequest(m_urlName));
    for (std::map<AString, AString>::const_iterator headerIter = m_requestHeaders.begin();
         headerIter != m_requestHeaders.end();
         headerIter++) {
        m_networkRequest->setRawHeader(headerIter->first.toLatin1(),
                                       headerIter->second.toLatin1());
    }
    
    m_postFileInfo.m_file = new QFile(m_postFileInfo.m_filename);
    if (m_postFileInfo.m_file->open(QFile::ReadOnly)) {
        m_networkReply.grabNew(networkManager->post(*m_networkRequest,
                                                    m_postFileInfo.m_file));

        QObject::connect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
                         this, SLOT(slotNetworkReplyError(QNetworkReply::NetworkError)));
        QObject::connect(m_networkReply, SIGNAL(finished()),
                         this, SLOT(slotNetworkReplyFinished()));
        QObject::connect(m_networkReply, SIGNAL(sslErrors(const QList<QSslError>&)),
                         this, SLOT(slotNetworkReplySslErrors(const QList<QSslError>&)));
        QObject::connect(m_networkReply, SIGNAL(uploadProgress(qint64, qint64)),
                         this, SLOT(slotNetworkReplyUploadProgress(qint64, qint64)));
    }
    
    while ( ! isFinished()) {
        QApplication::processEvents();
    }
    
//    int32_t httpCode = -1;
//    AString content;
//    getResult(httpCode, content);
//    std::cout << "Http Code: " << httpCode << std::endl;
//    std::cout << "Content: " << qPrintable(content) << std::endl;
}

/**
 * @return The result of the operation.
 */
const
HttpCommunicatorResult*
HttpCommunicator::getResult() const
{
    return m_result;
}

void HttpCommunicator::slotNetworkAccessManagerFinished(QNetworkReply* reply)
{
    CaretAssert(reply);
    
    int32_t httpCode = -1;
    std::map<AString, AString> headers;
    AString contentString;

//    if (isFinished()) {
        const QVariant httpCodeVariant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if ( ! httpCodeVariant.isNull()) {
            httpCode = httpCodeVariant.toInt();
            QByteArray content = m_networkReply->readAll();
            if ( ! content.isEmpty()) {
                content.push_back('\0');
            }
            contentString = AString(content);
            
            QList<QNetworkReply::RawHeaderPair> headerList = m_networkReply->rawHeaderPairs();
            QListIterator<QNetworkReply::RawHeaderPair> headerIter(headerList);
            while (headerIter.hasNext()) {
                QNetworkReply::RawHeaderPair pair = headerIter.next();
                AString name(pair.first);
                AString value (pair.second);
                headers.insert(std::make_pair(name, value));
            }
            m_result.grabNew(new HttpCommunicatorResult(httpCode,
                                                        headers,
                                                        contentString));
        }
//    }

//    std::cout << "Http Code: " << httpCode << std::endl;
//    std::cout << "Content: " << qPrintable(contentString) << std::endl;
}

void HttpCommunicator::slotNetworkAccessManagerSslErrors(QNetworkReply* reply,
                                                              const QList<QSslError>& errors)
{
    QListIterator<QSslError> listIter(errors);
    while (listIter.hasNext()) {
        const QSslError e = listIter.next();
        CaretLogSevere("Network Access Manager SSL ERROR: " + e.errorString());
    }
    setFinished(true);
}


void
HttpCommunicator::slotNetworkReplyError(QNetworkReply::NetworkError code)
{
//    std::cout << "Network Reply Network Error Code=" << code << " message: " << qPrintable(m_networkReply->errorString()) << std::endl;
    setFinished(true);
}

void
HttpCommunicator::slotNetworkReplyFinished()
{
//    std::cout << "Network Reply Finished" << std::endl;
    setFinished(true);
}

void
HttpCommunicator::slotNetworkReplySslErrors(const QList<QSslError>& errors)
{
    QListIterator<QSslError> listIter(errors);
    while (listIter.hasNext()) {
        const QSslError e = listIter.next();
        CaretLogSevere("Network Reply SSL ERROR: " + e.errorString());
    }
    setFinished(true);
}

void
HttpCommunicator::slotNetworkReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
//    std::cout << "Network Reply Upload progress: " << bytesSent << " of " << bytesTotal << std::endl;
    
    HttpCommunicatorProgress progress(this,
                                      HttpCommunicatorProgress::STATUS_IN_PROGRESS,
                                      "In Progress",
                                      0,
                                      bytesTotal,
                                      bytesSent);
    emit progressReport(progress);
    
    if (progress.isCancelled()) {
        m_networkReply->abort();
    }
    
    //SystemUtilities::sleepSeconds(0.05);
}

/**
 * see if the download is complete.
 */
bool
HttpCommunicator::isFinished()
{
    bool finishedFlag = false;
    m_finishedMutex.lock();
    finishedFlag = m_finishedFlag;
    m_finishedMutex.unlock();
    return finishedFlag;
}

/**
 * set the download complete.
 */
void
HttpCommunicator::setFinished(const bool finished)
{
    m_finishedMutex.lock();
    m_finishedFlag = finished;
    m_finishedMutex.unlock();
}
