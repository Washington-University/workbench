#ifndef __HTTP_COMMUNICATOR_H__
#define __HTTP_COMMUNICATOR_H__

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

#include <QNetworkReply>
#include <QObject>

#include "AString.h"
#include "CaretPointer.h"

class QFile;
class QNetworkAccessManager;
class QNetworkRequest;

namespace caret {

    class HttpCommunicatorProgress;
    class HttpCommunicatorResult;
    
    class HttpCommunicator : public QObject {
        
        Q_OBJECT

    public:
        static HttpCommunicator* newInstancePostFile(QObject* parent,
                                                          const AString& urlName,
                                                          const AString& filename,
                                                          const std::map<AString, AString>& headers);
        
        void runUntilDone();
        
        const HttpCommunicatorResult* getResult() const;
        
        virtual ~HttpCommunicator();
        

        // ADD_NEW_METHODS_HERE

    signals:
        void progressReport(const HttpCommunicatorProgress& progress);
        
    private slots:
        
        void slotNetworkAccessManagerFinished(QNetworkReply* reply);
        
        void slotNetworkAccessManagerSslErrors(QNetworkReply* reply,
                                               const QList<QSslError>& errors);
        
        void slotNetworkReplyError(QNetworkReply::NetworkError code);

        void slotNetworkReplyFinished();
        
        void slotNetworkReplySslErrors(const QList<QSslError>& errors);
        
        void slotNetworkReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal);
        
    private:
        enum Mode {
            MODE_POST_FILE
        };
        
        class PostFileInfo {
        public:
            PostFileInfo() {
                m_file = NULL;
            }
            
            QFile* m_file;
            
            AString m_filename;
        };
        
        HttpCommunicator(QObject* parent,
                              const Mode mode,
                              const AString& urlName,
                              const std::map<AString, AString>& requestHeaders);
        
        HttpCommunicator(const HttpCommunicator&);

        HttpCommunicator& operator=(const HttpCommunicator&);
        
        bool isFinished();
        
        void setFinished(const bool finished);
        
        const Mode m_mode;
        
        const AString m_urlName;
        
        const std::map<AString, AString> m_requestHeaders;
        
        PostFileInfo m_postFileInfo;
        
        QNetworkAccessManager* m_networkManager;
        
        CaretPointer<QNetworkReply> m_networkReply;
        
        CaretPointer<QNetworkRequest> m_networkRequest;
        
        CaretPointer<HttpCommunicatorResult> m_result;
        
        bool m_finishedFlag;
        
        QMutex m_finishedMutex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HTTP_COMMUNICATOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HTTP_COMMUNICATOR_DECLARE__

} // namespace
#endif  //__HTTP_COMMUNICATOR_H__
