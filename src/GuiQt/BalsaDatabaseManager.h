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

#include <map>

#include "BalsaStudyInformation.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"

class QJsonValue;

namespace caret {

    class BalsaUserRoles;
    struct CaretHttpResponse;
    class SceneFile;
    
    class BalsaDatabaseManager : public CaretObject, public EventListenerInterface {
        
    public:
        BalsaDatabaseManager();
        
        virtual ~BalsaDatabaseManager();
        
        bool login(const AString& databaseURL,
                   const AString& username,
                   const AString& password,
                   AString& errorMessageOut);
        
        void logout();
        
        bool isStudyIDValid(const AString& studyID);
        
        bool getAllStudyInformation(std::vector<BalsaStudyInformation>& studyInformationOut,
                                    AString& errorMessageOut);
        
        bool getUserRoles(BalsaUserRoles& userRolesOut,
                          AString& errorMessageOut);
        
        bool getStudyExtractDirectoryPrefix(const AString& studyID,
                                            AString& extractDirectoryPrefixOut,
                                            AString& errorMessageOut);
        
        bool getStudyIDFromStudyTitle(const AString& studyTitle,
                                      AString& studyIdOut,
                                      AString& errorMessageOut);
        
        bool getSceneIDs(const int32_t numberOfSceneIDs,
                             std::vector<AString>& sceneIDsOut,
                             AString& errorMessageOut);
        
        bool updateSceneIDs(SceneFile* sceneFile,
                            AString& errorMessageOut);
        
        bool checkSceneIDs(SceneFile* sceneFile,
                           AString& errorMessageOut);
        
        bool processCheckSceneIdResponse(SceneFile* sceneFile,
                                         const AString& responseContent,
                                         AString& errorMessageOut);
        
        bool uploadZippedSceneFile(SceneFile* sceneFile,
                                   const AString& zipFileName,
                                   const AString& extractToDirectoryName,
                                   AString& errorMessageOut);
        
        static bool zipSceneAndDataFiles(const SceneFile* sceneFile,
                                         const AString& extractDirectory,
                                         const AString& zipFileName,
                                         AString& errorMessageOut);
        
        bool isStudyEditableByUser(const AString& studyID,
                                   AString& errorMessageOut);
        
        AString getInfoMessages() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

        AString getUploadSummaryMessage() const;
        
    private:
        class SceneFileIdentifiers {
        public:
            SceneFileIdentifiers(const bool debugFlag,
                                 const QJsonValue& jsonValue);
            
            bool isValid() const;
            
            AString getErrorMessage() const;
            
            const bool m_debugFlag;
            
            AString m_errorMessage;
            
            AString m_sceneFileName;
            
            std::map<int32_t, AString> m_sceneIndexAndIDsMap;
        };
        
        BalsaDatabaseManager(const BalsaDatabaseManager&);

        BalsaDatabaseManager& operator=(const BalsaDatabaseManager&);
        
        AString getJSessionIdCookie() const;
        
        AString addToUploadProgressMessage(const AString& message);
        
        bool loginToAvoidTimeout(AString& errorMessageOut);
        
        bool uploadFile(const AString& uploadURL,
                        const AString& fileName,
                        const AString& httpContentTypeName,
                        AString& responseContentOut,
                        AString& errorMessageOut);
        
        bool processUploadedFile(SceneFile* sceneFile,
                                 const AString& processUploadURL,
                                 const AString& httpContentTypeName,
                                 const bool updateSceneIDsFromResponseFlag,
                                 AString& errorMessageOut);
        
        bool requestStudyID(const AString& databaseURL,
                            const AString& studyTitle,
                            AString& studyIDOut,
                            AString& errorMessageOut);
        
        bool uploadFileWithCaretHttpManager(const AString& uploadURL,
                        const AString& fileName,
                        const AString& httpContentTypeName,
                        AString& responseContentOut,
                        AString& errorMessageOut);
        
        bool verifyUploadFileResponse(const std::map<AString, AString>& responseHeaders,
                                      const AString& responseContent,
                                      const int32_t responseHttpCode,
                                      const AString& responseErrorMessage,
                                      AString& errorMessageOut) const;
        
        AString getHeaderValue(const CaretHttpResponse& httpResponse,
                               const AString& headerName) const;
        
        bool updateSceneIdsFromProcessUploadResponse(SceneFile* sceneFile,
                                                     const AString& jsonContent,
                                                     AString& errorMessageOut);
        
        AString m_databaseURL;
        
        AString m_username;
        
        AString m_password;
        
        AString m_jSessionIdCookie;
        
        AString m_infoMessages;
        
        bool m_debugFlag;
        
        AString m_uploadProgressMessage;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_DATABASE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_MANAGER_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_MANAGER_H__
