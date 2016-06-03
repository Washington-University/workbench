#ifndef __BALSA_DATABASE_MANAGER_H__
#define __BALSA_DATABASE_MANAGER_H__

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

#include <QObject>

#include "CaretObject.h"

#include "EventListenerInterface.h"
#include "HttpCommunicatorProgress.h"

namespace caret {

    class SceneFile;
    
    class BalsaDatabaseManager : public QObject, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BalsaDatabaseManager(QObject* parent);
        
        virtual ~BalsaDatabaseManager();
        
        bool uploadZippedSceneFile(const AString& databaseURL,
                   const AString& username,
                   const AString& password,
                   const SceneFile* sceneFile,
                   const AString& zipFileName,
                   const AString& extractToDirectoryName,
                   AString& errorMessageOut);
        
        bool login(const AString& loginURL,
                   const AString& username,
                   const AString& password,
                   AString& errorMessageOut);

        AString getJSessionIdCookie() const;
        
        bool uploadFile(const AString& uploadURL,
                        const AString& fileName,
                        const AString& httpContentTypeName,
                        AString& responseContentOut,
                        AString& errorMessageOut);
        
        bool processUploadedFile(const AString& processUploadURL,
                                 const AString& httpContentTypeName,
                                 AString& responseContentOut,
                                 AString& errorMessageOut);
        
        bool zipSceneAndDataFiles(const SceneFile* sceneFile,
                                  const AString& extractDirectory,
                                  const AString& zipFileName,
                                  AString& errorMessageOut);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

    signals:
        void reportProgress(const HttpCommunicatorProgress& progress);
        
    private slots:
        void receiveProgress(const HttpCommunicatorProgress& progress);
        
    private:
        BalsaDatabaseManager(const BalsaDatabaseManager&);

        BalsaDatabaseManager& operator=(const BalsaDatabaseManager&);
        
        bool uploadFileWithCaretHttpManager(const AString& uploadURL,
                        const AString& fileName,
                        const AString& httpContentTypeName,
                        AString& responseContentOut,
                        AString& errorMessageOut);
        
        bool uploadFileWithHttpCommunicator(const AString& uploadURL,
                                            const AString& fileName,
                                            const AString& httpContentTypeName,
                                            AString& responseContentOut,
                                            AString& errorMessageOut);
        
        AString m_username;
        
        AString m_password;
        
        AString m_jSessionIdCookie;
        
        bool m_debugFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_DATABASE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_MANAGER_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_MANAGER_H__
