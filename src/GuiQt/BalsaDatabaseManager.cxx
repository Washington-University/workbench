
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

#include <algorithm>
#include <iostream>

#define __BALSA_DATABASE_MANAGER_DECLARE__
#include "BalsaDatabaseManager.h"
#undef __BALSA_DATABASE_MANAGER_DECLARE__

#include <QDateTime>
#include <QJsonDocument>

#include "ApplicationInformation.h"
#include "BalsaUserRoles.h"
#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "CommandOperationManager.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "FileInformation.h"
#include "JsonHelper.h"
#include "OperationZipSceneFile.h"
#include "ProgramParameters.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneInfo.h"

using namespace caret;


    
/**
 * \class caret::BalsaDatabaseManager 
 * \brief Manages connection with BALSA Database.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BalsaDatabaseManager::BalsaDatabaseManager()
: CaretObject()
{
    m_debugFlag = false;
    logout();
    //EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
BalsaDatabaseManager::~BalsaDatabaseManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BalsaDatabaseManager::toString() const
{
    return "BalsaDatabaseManager";
}

/**
 * Login to the BALSA Database.
 *
 * @param databaseURL
 *     URL of the database for login
 * @param username
 *     Name for login.
 * @param password
 *     Password for login.
 * @param errorMessageOut
 *     Contains error information if login failed.
 * @return
 *     True if login is successful, else false.
 */
bool
BalsaDatabaseManager::login(const AString& databaseURL,
                            const AString& username,
                            const AString& password,
                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    m_databaseURL = databaseURL;
    m_username = username.trimmed();
    m_password = password.trimmed();
    m_jSessionIdCookie = "";
    
    const AString loginURL(databaseURL + "/j_spring_security_check");
    
    CaretHttpRequest loginRequest;
    loginRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    loginRequest.m_url    = loginURL;
    loginRequest.m_arguments.push_back(std::make_pair("j_username",
                                                      m_username));
    loginRequest.m_arguments.push_back(std::make_pair("j_password",
                                                      m_password));
    
    CaretHttpResponse loginResponse;
    CaretHttpManager::httpRequest(loginRequest, loginResponse);
    
    if (m_debugFlag) {
        std::cout << "Response Code: " << loginResponse.m_responseCode << std::endl;
    }
    
    AString setCookieErrorMessage = "Did not find 'set-cookie' in response headers.";
    for (std::map<AString, AString>::iterator mapIter = loginResponse.m_headers.begin();
         mapIter != loginResponse.m_headers.end();
         mapIter++) {
        
        if (mapIter->first.toLower() == "set-cookie") {
            const AString value = mapIter->second;
            const int equalPos = value.indexOf("=");
            const int semiPos  = value.indexOf(";");
            if ((equalPos >= 0)
                && (semiPos > (equalPos + 1))) {
                const int offset = equalPos + 1;
                const int length = semiPos - offset;
                m_jSessionIdCookie = value.mid(offset,
                                               length);
            }
            else {
                setCookieErrorMessage = ("set-cookie invalid.  Value='"
                                         + value
                                         + "'.");
            }
        }
        if (m_debugFlag) {
            std::cout << "   Response Header: " << qPrintable(mapIter->first)
                << " -> " << qPrintable(mapIter->second) << std::endl;
        }
    }
    
    if (m_debugFlag) {
        std::cout << "SessionID: " << qPrintable(m_jSessionIdCookie) << std::endl;
    }
    
    if (loginResponse.m_responseCode == 200) {
        if (m_jSessionIdCookie.isEmpty()) {
            errorMessageOut = ("Login was successful but BALSA failed to provide a Session ID.\n"
                               + setCookieErrorMessage);
            logout();
            return false;
        }
        
        return true;
    }
    else if (loginResponse.m_responseCode == 302) {
        errorMessageOut = ("Login failed.<p>"
                           "If you are unable to login, helpful links are located on the right side of the username and password boxes:"
                           "<ul>"
                           "<li><i>Register</i>:  Click to create a BALSA account"
                           "<li><i>Forgot Username</i>:  Click to get your username by entering your email address"
                           "<li><i>Forgot Password</i>:  Click to reset your password by entering your username"
                           "</ul>");
        logout();
        return false;
    }
    
    loginResponse.m_body.push_back('\0');
    const AString responseContent(&loginResponse.m_body[0]);
    errorMessageOut = ("Login has failed.\n"
                       "HTTP Code: " + AString::number(loginResponse.m_responseCode)
                       + " Content: " + responseContent);
    if (loginResponse.m_responseCode < 0) {
        errorMessageOut.appendWithNewLine(loginResponse.m_errorMessage);
    }
    logout();

    return false;
}

/**
 * Logout of the database
 */
void
BalsaDatabaseManager::logout()
{
    m_databaseURL.clear();
    m_username.clear();
    m_password.clear();
    m_jSessionIdCookie.clear();
}


/**
 * Upload file to the BALSA Database.
 *
 * @param uploadURL
 *     URL for uploading file.
 * @param fileName
 *     Name of file.
 * @param httpContentTypeName
 *     Type of content for upload (eg: application/zip, see http://www.freeformatter.com/mime-types-list.html)
 * @param responseContentOut
 *     If successful, contains the response content received after successful upload.
 * @param errorMessageOut
 *     Contains error information if upload failed.
 * @return
 *     True if upload is successful, else false.
 */
bool
BalsaDatabaseManager::uploadFile(const AString& uploadURL,
                                 const AString& fileName,
                                 const AString& httpContentTypeName,
                                 AString& responseContentOut,
                                 AString& errorMessageOut)
{
    responseContentOut.clear();
    errorMessageOut.clear();
    
    const bool successFlag = uploadFileWithCaretHttpManager(uploadURL,
                                                            fileName,
                                                            httpContentTypeName,
                                                            responseContentOut,
                                                            errorMessageOut);
    return successFlag;
}

/**
 * Upload file to the BALSA Database.
 *
 * @param uploadURL
 *     URL for uploading file.
 * @param fileName
 *     Name of file.
 * @param httpContentTypeName
 *     Type of content for upload (eg: application/zip, see http://www.freeformatter.com/mime-types-list.html)
 * @param responseContentOut
 *     If successful, contains the response content received after successful upload.
 * @param errorMessageOut
 *     Contains error information if upload failed.
 * @return
 *     True if upload is successful, else false.
 */
bool
BalsaDatabaseManager::uploadFileWithCaretHttpManager(const AString& uploadURL,
                                 const AString& fileName,
                                 const AString& httpContentTypeName,
                                 AString& responseContentOut,
                                 AString& errorMessageOut)
{
    responseContentOut.clear();
    errorMessageOut.clear();
    
    if (httpContentTypeName.isEmpty()) {
        errorMessageOut = ("Content Type Name is empty.  "
                           "See http://www.freeformatter.com/mime-types-list.html for examples.");
        return false;
    }
    
    FileInformation fileInfo(fileName);
    if ( ! fileInfo.exists()) {
        errorMessageOut = (fileName
                           + " does not exist.");
        return false;
    }
    const int64_t fileSize = fileInfo.size();
    if (fileSize <= 0) {
        errorMessageOut = (fileName
                           + " does not contain any data (size=0)");
        return false;
    }
    
    /*
     * Upload file name must be name of file without path
     */
    const AString uploadFileName(fileInfo.getFileName());
    const AString fileSizeString(AString::number(fileSize));
    
    CaretHttpRequest uploadRequest;
    uploadRequest.m_method = CaretHttpManager::POST_FILE;
    uploadRequest.m_url    = uploadURL;
    uploadRequest.m_uploadFileName = fileName;
    uploadRequest.m_headers.insert(std::make_pair("Content-Type",
                                                  httpContentTypeName));
    uploadRequest.m_headers.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    uploadRequest.m_headers.insert(std::make_pair("X-File-Name",
                                                  uploadFileName));
    uploadRequest.m_headers.insert(std::make_pair("X-File-Size",
                                                  fileSizeString));
    
    
    
    CaretHttpResponse uploadResponse;
    CaretHttpManager::httpRequest(uploadRequest, uploadResponse);

    uploadResponse.m_body.push_back('\0');
    responseContentOut.append(&uploadResponse.m_body[0]);

    
    return verifyUploadFileResponse(uploadResponse.m_headers,
                                    responseContentOut,
                                    uploadResponse.m_responseCode,
                                    uploadResponse.m_errorMessage,
                                    errorMessageOut);
}

/**
 * Process the response from file upload.  Content should be JSON.
 *
 * @param responseHeaders
 *     Headers from the response.
 * @param responseContent
 *     Content from the response.
 * @param responseHttpCode
 *     HTTP code from response.
 * @param responseErrorMessage
 *
 * @param errorMessageOut
 *     Contains description of error.
 * @return
 *     True if response shows upload was successful, else false.
 */
bool
BalsaDatabaseManager::verifyUploadFileResponse(const std::map<AString, AString>& responseHeaders,
                                               const AString& responseContent,
                                               const int32_t responseHttpCode,
                                               const AString& responseErrorMessage,
                                               AString& errorMessageOut) const
{
    if (responseHttpCode != 200) {
        if (responseHttpCode == 403) {
            errorMessageOut = ("Upload failed.  (Http Code="
                               + AString::number(responseHttpCode)
                               + ").\n\n"
                               "Either you do not have ownership/permission to edit the study or "
                               "the study has been submitted for curation.\n\n"
                               "Use your web browser to login to BALSA to view the study and "
                               "check its permissions.  If the the study has been submitted for "
                               "curation, there will be a 'return for revision' option on the study.  "
                               "Selection of 'return for 'revision' will allow you to upload your data.");
        }
        else {
            errorMessageOut = ("Upload failed.  Http Code="
                               + AString::number(responseHttpCode)
                               + ".\n"
                               + responseErrorMessage);
//            if (responseHttpCode < 0) {
//                const QString s(responseContent.isEmpty()
//                                ? "Response content is empty !!!"
//                                : ("Response content:\n"
//                                   + responseContent));
//                CaretLogSevere("Invalid http response="
//                               + AString::number(responseHttpCode)
//                               + "\n"
//                               + s
//                               + "\n");
//            }
        }
        return false;
    }
    
    if (responseHeaders.empty()) {
        if (m_debugFlag) std::cout << "Response headers from upload are empty." << std::endl;
    }
    
    bool haveContentTypeFlag = false;
    AString contentTypeString;
    for (std::map<AString, AString>::const_iterator iter = responseHeaders.begin();
         iter != responseHeaders.end();
         iter++) {
        if (m_debugFlag) std::cout << "Response Header: " << iter->first << " -> " << iter->second << std::endl;
        
        if (iter->first.toLower() == "content-type") {
            haveContentTypeFlag = true;
            contentTypeString = iter->second;
        }
    }
    
    AString contentErrorMessage;
    bool haveJsonResponseContentFlag = false;
    if (haveContentTypeFlag) {
        if (contentTypeString.startsWith("application/json;")) {
            haveJsonResponseContentFlag = true;
        }
        else {
            contentErrorMessage = ("Content-Type received from file upload is not JSON but is "
                                   + contentTypeString
                                   + "\n");
        }
    }
    else {
        contentErrorMessage = "No Content-Type header received from file upload.\n";
    }
    
    if (m_debugFlag) {
        std::cout << std::endl << "RESPONSE CONTENT: " << responseContent << std::endl << std::endl;
    }
    
    
    
    AString statusText;
    if (haveJsonResponseContentFlag) {
        if (responseContent.isEmpty()) {
            errorMessageOut = "Process upload response content is empty.";
            return false;
        }
        
        QJsonParseError jsonError;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toUtf8(),
                                                             &jsonError);
        if (jsonDocument.isNull()) {
            errorMessageOut = ("Process upload response failed.  Failed to parse JSON, error:"
                               + jsonError.errorString()
                               + " Offset="
                               + AString::number(jsonError.offset)
                               + "\n\n"
                               "Response Content: "
                               + responseContent);
            return false;
        }
        
        QByteArray json = jsonDocument.toJson();
        if (m_debugFlag) {
            std::cout << "Array?: " << jsonDocument.isArray() << std::endl;
            std::cout << "Empty?: " << jsonDocument.isEmpty() << std::endl;
            std::cout << "NULL?: " << jsonDocument.isNull() << std::endl;
            std::cout << "Object?: " << jsonDocument.isObject() << std::endl;
            std::cout << "JSON length: " << json.size() << std::endl;
            AString str(json);
            std::cout << ("Formatted JSON:\n" + str) << std::endl;
        }
        
        if ( ! jsonDocument.isObject()) {
            errorMessageOut  = ("Process upload response failed.  JSON content is not an object."
                                "JSON is displayed in terminal if logging level is warning or higher.");
            CaretLogWarning("Study ID JSON is not Object\n"
                            + AString(jsonDocument.toJson()));
            return false;
        }
        
        QJsonObject responseObject = jsonDocument.object();
        const QJsonValue statusValue = responseObject.value("statusText");
        if (statusValue.type() == QJsonValue::String) {
            statusText = statusValue.toString();
        }
        if (m_debugFlag) {
            std::cout << "Status Text: " << statusText << std::endl;
        }
    }
    else {
        errorMessageOut = contentErrorMessage;
        return false;
    }
    
    return true;
}

/**
 * Zip a scene file and its data files.
 *
 * @param sceneFile
 *     Scene file that is zipped.
 * @param extractDirectory
 *     Directory into which files are extracted.
 * @param zipFileName
 *     Name for the ZIP file.
 * @param errorMessageOut
 *     Contains error information if zipping failed.
 * @return
 *     True if zipping is successful, else false.
 */
bool
BalsaDatabaseManager::zipSceneAndDataFiles(const SceneFile* sceneFile,
                                           const AString& extractDirectory,
                                           const AString& zipFileName,
                                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (sceneFile == NULL) {
        errorMessageOut = "Scene file is invalid.";
        return false;
    }
    const QString sceneFileName = sceneFile->getFileName();
    if (sceneFileName.isEmpty()) {
        errorMessageOut = "Scene File does not have a name.";
        return false;
    }
    
    const AString extractToDirectoryName = extractDirectory;
    if (extractToDirectoryName.isEmpty()) {
        errorMessageOut = "Extract to directory is empty.";
        return false;
    }
    
    if (zipFileName.isEmpty()) {
        errorMessageOut =  "Zip File name is empty";
        return false;
    }
    
    AString basePathName;
    
    switch (sceneFile->getBasePathType()) {
        case SceneFileBasePathTypeEnum::AUTOMATIC:
        {
            std::vector<AString> missingFileNames;
            AString errorMessage;
            const bool validBasePathFlag = sceneFile->findBaseDirectoryForDataFiles(basePathName,
                                                                                    missingFileNames,
                                                                                    errorMessage);
            if ( ! validBasePathFlag) {
                errorMessageOut.appendWithNewLine("Automatic Base Path Failed: "
                                                  + errorMessage);
                return false;
            }
            
            if ( ! FileInformation(basePathName).exists()) {
                errorMessageOut = ("AUTOMATIC base path mode produced an invalid base path (directory does not exist).  "
                                   ". \""
                                   + basePathName
                                   + "\".");
                return false;
            }
        }
            break;
        case SceneFileBasePathTypeEnum::CUSTOM:
            basePathName = sceneFile->getBalsaCustomBaseDirectory();
            if (basePathName.isEmpty()) {
                errorMessageOut = ("CUSTOM base path is empty");
                return false;
            }
            if ( ! FileInformation(basePathName).exists()) {
                errorMessageOut = ("CUSTOM base path is invalid (directory does not exist): \""
                                   + basePathName
                                   + "\"");
                return false;
            }
            break;
    }
    
    bool successFlag = false;
    try {
        OperationZipSceneFile::createZipFile(NULL,
                                             sceneFileName,
                                             extractToDirectoryName,
                                             zipFileName,
                                             basePathName,
                                             OperationZipSceneFile::PROGRESS_GUI_EVENT);
        successFlag = true;
    }
    catch (const CaretException& e) {
        AString moreInfo("If there was an error opening or writing to the ZIP file, "
                         "it may caused by incorrect permissions or insufficient space in the "
                         "temporary directory used for creating the ZIP file.  Use the ZIP File Directory Custom option "
                         "in the Advanced Tab to choose an alternative directory.");
        errorMessageOut = ("Error: "
                           + e.whatString()
                           + "\n\n"
                           + moreInfo);
    }
    
    return successFlag;
}

/**
 * @return The JSESSIONID Cookie value (empty if not valid).
 */
AString
BalsaDatabaseManager::getJSessionIdCookie() const
{
    return m_jSessionIdCookie;
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
BalsaDatabaseManager::receiveEvent(Event* /*event*/)
{
    //    if (event->getEventType() == EventTypeEnum::) {
    //        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
    //        CaretAssert(eventName);
    //
    //        event->setEventProcessed();
    //    }
}

/**
 * Request BALSA database process the file that was uploaded.
 *
 * @param sceneFile
 *     Scene file that was uploaded.
 * @param processUploadURL
 *     URL for uploading file.
 * @param httpContentTypeName
 *     Type of content for upload (eg: application/zip, see http://www.freeformatter.com/mime-types-list.html)
 * @param updateSceneIDsFromResponseFlag
 *     If true, update the Scene IDs using those in the response content.
 * @param errorMessageOut
 *     Contains error information if upload failed.
 * @return
 *     True if upload is successful, else false.
 */
bool
BalsaDatabaseManager::processUploadedFile(SceneFile* sceneFile,
                                          const AString& processUploadURL,
                                          const AString& httpContentTypeName,
                                          const bool updateSceneIDsFromResponseFlag,
                                          AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (httpContentTypeName.isEmpty()) {
        errorMessageOut = ("Content Type Name is empty.  "
                           "See http://www.freeformatter.com/mime-types-list.html for examples.");
        return false;
    }
    
    CaretHttpRequest uploadRequest;
    uploadRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    uploadRequest.m_url    = processUploadURL;
    uploadRequest.m_headers.insert(std::make_pair("Content-Type",
                                                  httpContentTypeName));
    uploadRequest.m_headers.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    uploadRequest.m_arguments.push_back(std::make_pair("type",
                                                      "json"));
    
    CaretHttpResponse uploadResponse;
    CaretHttpManager::httpRequest(uploadRequest, uploadResponse);
    
    if (m_debugFlag) {
        std::cout << "Process upload response Code: " << uploadResponse.m_responseCode << std::endl;
    }
    
    if (uploadResponse.m_responseCode != 200) {
        errorMessageOut = ("Process Upload failed code: "
                           + QString::number(uploadResponse.m_responseCode)
                           + ".  ");
        errorMessageOut.appendWithNewLine(uploadResponse.m_errorMessage);
        
        uploadResponse.m_body.push_back('\0');
        AString responseContent(&uploadResponse.m_body[0]);
        if (m_debugFlag) {
            std::cout << errorMessageOut << std::endl;
            std::cout << "Process Upload FAILED to BALSA reply body: " << responseContent << std::endl;
        }
        else {
            CaretLogFine("Process Upload FAILED to BALSA reply error message in GUI: "
                         + errorMessageOut
                         + "Process Upload FAILED to BALSA reply body: "
                         + responseContent);
        }
        return false;
    }

    /*
     * As of 24 Oct 2017, Scene IDs are updated prior to uploading scene fiel
     */
    if (updateSceneIDsFromResponseFlag) {
        for (std::map<AString, AString>::iterator mapIter = uploadResponse.m_headers.begin();
             mapIter != uploadResponse.m_headers.end();
             mapIter++) {
            if (m_debugFlag) {
                std::cout << "   Process Upload Response Header: " << qPrintable(mapIter->first)
                << " -> " << qPrintable(mapIter->second) << std::endl;
            }
        }
        
        uploadResponse.m_body.push_back('\0');
        AString responseContent(&uploadResponse.m_body[0]);
        if (m_debugFlag) {
            std::cout << "Process Upload to BALSA reply body: " << responseContent << std::endl;
        }
        else {
            CaretLogFine("Process Upload to BALSA reply body: "
                         + responseContent);
        }
        
        
        AString contentType = getHeaderValue(uploadResponse, "content-type");
        if (contentType.isNull()) {
            errorMessageOut = ("Process Upload Content type returned by BALSA is unknown");
            return false;
        }
        
        if ( ! contentType.toLower().startsWith("application/json")) {
            errorMessageOut = ("Process Upload Content type return by BALSA should be JSON format but is "
                               + contentType
                               + "Scene File was uploaded but Scene IDs will not be updated");
            return false;
        }
        
        if ( ! updateSceneIdsFromProcessUploadResponse(sceneFile,
                                                       responseContent,
                                                       errorMessageOut)) {
            return false;
        }
    }
    
    return true;
}

/**
 * Get the Scene IDs from the JSON content returned by
 * the process upload command and update the scene file.
 *
 * @param sceneFile
 *     Scene file that was uploaded.
 * @param jsonContent
 *     The JSON content.
 * @param errorMessage
 *     Contains error information if there is a problem.
 * @return
 *     true if successful, else false.
 */
bool
BalsaDatabaseManager::updateSceneIdsFromProcessUploadResponse(SceneFile* sceneFile,
                                                              const AString& jsonContent,
                                                              AString& errorMessageOut)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonContent.toUtf8(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Failed to parse JSON response from procssess upload, error:"
                           + jsonError.errorString());
        return false;
    }
    if ( ! jsonDocument.isArray()) {
        errorMessageOut = ("Unrecognized format of JSON response from process upload (not an array)");
        return false;
    }
    const AString mySceneFileName = sceneFile->getFileNameNoPath();
    
    QJsonArray jsonArray = jsonDocument.array();
    const int32_t numFileAndSceneIds = jsonArray.count();
    if (numFileAndSceneIds <= 0) {
        errorMessageOut = ("No Scene IDs were returned by BALSA.  Content returned \""
                           + JsonHelper::jsonArrayToString(jsonArray)
                           + "\"");
        return false;
    }
    for (int32_t j = 0; j < numFileAndSceneIds; j++) {
        SceneFileIdentifiers sceneFileIDs(m_debugFlag,
                                          jsonArray.at(j));
        if ( ! sceneFileIDs.isValid()) {
            errorMessageOut = ("Scene File IDs error: "
                               + sceneFileIDs.getErrorMessage());
            return false;
        }
        
        if (sceneFileIDs.m_sceneFileName == mySceneFileName) {
            for (const auto& indexAndID : sceneFileIDs.m_sceneIndexAndIDsMap) {
                const int32_t sceneIndex = indexAndID.first;
                const AString sceneID    = indexAndID.second;
                
                if ((sceneIndex >= 0)
                    && (sceneIndex < sceneFile->getNumberOfScenes())) {
                    Scene* scene = sceneFile->getSceneAtIndex(sceneIndex);
                    CaretAssert(scene);
                    scene->setBalsaSceneID(sceneID);
                }
                else {
                    errorMessageOut = ("Invalid Scene Index from BALSA Scene IDs: "
                                       + AString::number(sceneIndex));
                    return false;
                }
            }
        }
    }
    
    return true;
}

/**
 * Get the value for the given header name using
 * a case-insensitive string comparison.
 *
 * @param httpResponse
 *     The HTTP response.
 * @param headerName
 *     The header name.
 * @return
 *     Value for header name.  If the header name is
 *     not found the returned string will be NULL
 *     (test with .isNull()).
 */
AString
BalsaDatabaseManager::getHeaderValue(const CaretHttpResponse& httpResponse,
                                     const AString& headerName) const
{
    /*
     * NULL string
     */
    AString value;
    CaretAssert(value.isNull());
    
    const QString headerNameLower(headerName.toLower());
    
    
    for (auto iter : httpResponse.m_headers) {
        if (iter.first.toLower() == headerNameLower) {
            value = iter.second;
            break;
        }
    }
    
    return value;
}

/**
 * Add Scene IDs to scenes in the given scene file that are missing Scene IDs
 *
 * @param sceneFile
 *     The scene file.
 * @param errorMessageOut
 *     Output with error message
 * @return
 *     True if success, else false.
 */
bool
BalsaDatabaseManager::updateSceneIDs(SceneFile* sceneFile,
                                     AString& errorMessageOut)
{
    CaretAssert(sceneFile);
    
    std::vector<Scene*> scenesMissingID;
    
    const int32_t numScenes = sceneFile->getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        Scene* scene = sceneFile->getSceneAtIndex(i);
        CaretAssert(scene);
        if (scene->getBalsaSceneID().trimmed().isEmpty()) {
            scenesMissingID.push_back(scene);
        }
    }
    
    const int32_t numberOfIDs = static_cast<int32_t>(scenesMissingID.size());
    if (numberOfIDs > 0) {
        std::vector<AString> sceneIDs;
        if ( ! getSceneIDs(numberOfIDs,
                           sceneIDs,
                           errorMessageOut)) {
            return false;
        }
        
        CaretAssert(scenesMissingID.size() == sceneIDs.size());
        
        for (int32_t i = 0; i < numberOfIDs; i++) {
            scenesMissingID[i]->setBalsaSceneID(sceneIDs[i]);
        }
    }
    
    return true;
}

/**
 * Check scene IDs
 * @param sceneFile
 *    The scene file
 * @param errorMessageOut
 *    Output error message
 * @return
 *    True if successful, else false.
 */
bool
BalsaDatabaseManager::checkSceneIDs(SceneFile* sceneFile,
                                    AString& errorMessageOut)
{
    CaretAssert(sceneFile);
    errorMessageOut.clear();

    const AString studyID(sceneFile->getBalsaStudyID());
    if (studyID.isEmpty()) {
        errorMessageOut = "Study ID is invalid (empty)";
        return false;
    }
    
    AString sceneIdList;
    const int32_t numScenes(sceneFile->getNumberOfScenes());
    for (int32_t i = 0; i < numScenes; i++) {
        const Scene* scene(sceneFile->getSceneAtIndex(i));
        CaretAssert(scene);
        const AString sceneID(scene->getBalsaSceneID());
        if ( ! sceneID.isEmpty()) {
            if ( ! sceneIdList.isEmpty()) {
                sceneIdList.append(",");
            }
            sceneIdList.append(sceneID);
        }
    }
    if (sceneIdList.isEmpty()) {
        /*
         * There are no scene IDs and that is OK
         */
        return true;
    }
    
    const AString checkURL(m_databaseURL
                           + "/study/checkScenes/"
                           + studyID
                           + "?sceneList="
                           + sceneIdList);
    if (m_debugFlag) {
        std::cout << "Check Scenes URL: " << checkURL << std::endl;
    }

    errorMessageOut.clear();
    
    CaretHttpRequest caretRequest;
    caretRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    caretRequest.m_url    = checkURL;
    caretRequest.m_headers.insert(std::make_pair("Content-Type",
                                                 "application/x-www-form-urlencoded"));
    caretRequest.m_headers.insert(std::make_pair("Cookie",
                                                 getJSessionIdCookie()));
    caretRequest.m_arguments.push_back(std::make_pair("type",
                                                      "json"));
    
    CaretHttpResponse response;
    CaretHttpManager::httpRequest(caretRequest, response);
    
    if (m_debugFlag) {
        std::cout << "Check Scene IDs response Code: " << response.m_responseCode << std::endl;
    }
    
    if (response.m_responseCode != 200) {
        errorMessageOut = ("Check Scene IDs failed with HTTP code="
                           + AString::number(response.m_responseCode)
                           + ".  This error may be caused by failure to agree to data use terms.  ");
        errorMessageOut.appendWithNewLine(response.m_errorMessage);
        return false;
    }
    
    response.m_body.push_back('\0');
    const AString responseContent(&response.m_body[0]);
    if (m_debugFlag) {
        std::cout << "Check Scene IDs body:\n" << responseContent << std::endl << std::endl;
    }

    if ( ! processCheckSceneIdResponse(sceneFile,
                                       responseContent,
                                       errorMessageOut)) {
        return false;
    }
    
    
    return true;
}

/**
 * Process the content from checking scene IDs
 * @param sceneFile
 *    The scene file
 * @param responseContent
 *    The content
 * @param errorMessageOut
 *    Output error message
 * @return
 *    True if successful, else false.
 */
bool
BalsaDatabaseManager::processCheckSceneIdResponse(SceneFile* sceneFile,
                                                  const AString& responseContent,
                                                  AString& errorMessageOut)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toUtf8(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Checking Scene IDs failed.  Failed to parse JSON, error:"
                           + jsonError.errorString()
                           + " Offset="
                           + AString::number(jsonError.offset)
                           + "\n"
                           + responseContent);
        return false;
    }
    
    QByteArray json = jsonDocument.toJson();
    if ( ! jsonDocument.isObject()) {
        errorMessageOut  = ("Checking Scene IDs failed.  JSON content is not an object."
                           "  \n"
                            + responseContent);
        return false;
    }

    bool validFlag(false);
    bool saveSceneFileFlag(false);
    
    const QJsonObject object(jsonDocument.object());
    const QStringList keys(object.keys());

    AString updatedSceneNames;
    
    const QString badSceneIdsName("badSceneIds");
    const QString newSceneIdsName("newSceneIds");
    if (object.contains(badSceneIdsName) && object.value(badSceneIdsName).isArray()) {
        if (object.contains(newSceneIdsName) && object.value(newSceneIdsName).isArray()) {
            const QJsonArray badSceneIdsArray(object.value(badSceneIdsName).toArray());
            const QJsonArray newSceneIdsArray(object.value(newSceneIdsName).toArray());
            const int32_t numIDs(badSceneIdsArray.count());
            
            if (numIDs == newSceneIdsArray.count()) {
                /*
                 * If both arrays contain zero elements, all scene
                 * IDs are valid
                 */
                if (numIDs == 0) {
                    return true;
                }

                validFlag = true;
                for (int32_t i = 0; i < numIDs; i++) {
                    if (badSceneIdsArray.at(i).isString()
                        && (newSceneIdsArray.at(i).isString())) {
                        const AString badSceneID(badSceneIdsArray.at(i).toString());
                        Scene* scene(sceneFile->getSceneWithSceneID(badSceneID));
                        if (scene != NULL) {
                            const AString newSceneID(newSceneIdsArray.at(i).toString());
                            scene->getSceneInfo()->setBalsaSceneID(newSceneID);
                            saveSceneFileFlag = true;
                            
                            updatedSceneNames.appendWithNewLine("   " + scene->getName());
                        }
                        else {
                            errorMessageOut = ("Program Error: Scene not found with bad Scene ID: "
                                               + badSceneID);
                            return false;
                        }
                    }
                }
            }
        }
    }

    if (validFlag) {
        if (saveSceneFileFlag) {
            try {
                sceneFile->writeFile(sceneFile->getFileName());
                
                if ( ! updatedSceneNames.isEmpty()) {
                    m_infoMessages.appendWithNewLine("These scenes had their Scene IDs replaced (IDs used by other scenes in BALSA):");
                    m_infoMessages.appendWithNewLine(updatedSceneNames);
                }
            }
            catch (const DataFileException& dfe) {
                errorMessageOut = ("Error writing scene file after replacing bad Scene IDs: "
                                   + dfe.whatString());
                return false;
            }
        }
    }
    
    if ( ! validFlag) {
        errorMessageOut = ("Response from BALSA for replacing Bad Scene IDs invalid: "
                           + responseContent);
    }
    
    return validFlag;
}

/**
 * Request the given number of scene IDs
 *
 * @param numberOfSceneIDs
 *     Number of scene IDs requested
 * @param sceneIDsOut
 *     Output with requested number of Scene IDs
 * @param errorMessageOut
 *     Output with error message
 * @return
 *     True if success, else false.
 */
bool
BalsaDatabaseManager::getSceneIDs(const int32_t numberOfSceneIDs,
                                  std::vector<AString>& sceneIDsOut,
                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    sceneIDsOut.clear();
    
    if (numberOfSceneIDs <= 0) {
        errorMessageOut = "Zero or fewer Study IDs requested";
        return false;
    }
    
    const AString studyIdURL(m_databaseURL
                             + "/scene/newIds/"
                             + AString::number(numberOfSceneIDs));
    
    errorMessageOut.clear();
    
    CaretHttpRequest caretRequest;
    caretRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    caretRequest.m_url    = studyIdURL;
    caretRequest.m_headers.insert(std::make_pair("Content-Type",
                                                 "application/x-www-form-urlencoded"));
    caretRequest.m_headers.insert(std::make_pair("Cookie",
                                                 getJSessionIdCookie()));
    caretRequest.m_arguments.push_back(std::make_pair("type",
                                                      "json"));
    
    CaretHttpResponse response;
    CaretHttpManager::httpRequest(caretRequest, response);
    
    if (m_debugFlag) {
        std::cout << "Request Scene IDs response Code: " << response.m_responseCode << std::endl;
    }
    
    if (response.m_responseCode != 200) {
        errorMessageOut = ("Requesting Scene IDs failed with HTTP code="
                           + AString::number(response.m_responseCode)
                           + ".  This error may be caused by failure to agree to data use terms.  ");
        errorMessageOut.appendWithNewLine(response.m_errorMessage);
        return false;
    }
    
    response.m_body.push_back('\0');
    AString responseContent(&response.m_body[0]);
    
    if (m_debugFlag) {
        std::cout << "Request Scene IDs body:\n" << responseContent << std::endl << std::endl;
    }
    
    AString contentType = getHeaderValue(response, "content-type");
    if (contentType.isNull()) {
        errorMessageOut = ("Requesting Scene IDs failed.  Content type returned by BALSA is unknown");
        return false;
    }
    
    if (contentType.toLower().startsWith("text/html")) {
        /*
         * Response from BALSA has Scene IDs separated by '<br>'
         * Example: vp5B<br>Z02A<BR>
         */
#if QT_VERSION >= 0x060000
        QStringList sl = responseContent.split("<br>",
                                               Qt::SkipEmptyParts,
                                               Qt::CaseInsensitive);
#else
        QStringList sl = responseContent.split("<br>",
                                               QString::SkipEmptyParts,
                                               Qt::CaseInsensitive);
#endif
        const int32_t receivedCount = sl.count();
        if (receivedCount != numberOfSceneIDs) {
            errorMessageOut = ("Requested "
                               + AString::number(numberOfSceneIDs)
                               + " but received "
                               + AString::number(receivedCount)
                               + " IDs");
            return false;
        }
        for (int32_t i = 0; i < receivedCount; i++) {
            sceneIDsOut.push_back(sl.at(i));
        }
    }
    else if (contentType.toLower().startsWith("application/json")) {
        errorMessageOut = ("Requesting Scene IDS failed.  Content type returned by BALSA is JSON but support for JSON has not been implemented in Workbench");
        return false;
    }
    else {
        errorMessageOut = ("Requesting Scene IDS failed.  Content type returned by BALSA should be either text/html or JSON format but is "
                           + contentType);
        return false;
    }
    
    return true;
}


/**
 * Get the study ID from a study Title
 *
 * @param databaseURL
 *     URL for database
 * @param studyTitle
 *     Title of the study
 * @param studyIDOut
 *     Output with the study ID
 * @param errorMessageOut
 *     Output with error message
 * @return 
 *     True if success, else false.
 */
bool
BalsaDatabaseManager::requestStudyID(const AString& databaseURL,
                                     const AString& studyTitle,
                                     AString& studyIDOut,
                                     AString& errorMessageOut)
{
    const AString studyIdURL(databaseURL
                             + "/study/save");

    errorMessageOut.clear();
    
    CaretHttpRequest caretRequest;
    caretRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    caretRequest.m_url    = studyIdURL;
    caretRequest.m_headers.insert(std::make_pair("Content-Type",
                                                  "application/x-www-form-urlencoded"));
    caretRequest.m_headers.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    caretRequest.m_arguments.push_back(std::make_pair("title",
                                                  studyTitle));
    caretRequest.m_arguments.push_back(std::make_pair("type",
                                                      "json"));
    
    CaretHttpResponse studyResponse;
    CaretHttpManager::httpRequest(caretRequest, studyResponse);
    
    if (m_debugFlag) {
        std::cout << "Request study ID response Code: " << studyResponse.m_responseCode << std::endl;
    }
    
    if (studyResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting study ID failed with HTTP code="
                           + AString::number(studyResponse.m_responseCode)
                           + ".  This error may be caused by failure to agree to data use terms.  ");
        errorMessageOut.appendWithNewLine(studyResponse.m_errorMessage);
        return false;
    }
    
    AString contentType = getHeaderValue(studyResponse, "content-type");
    if (contentType.isNull()) {
        errorMessageOut = ("Requesting study ID failed.  Content type returned by BALSA is unknown");
        return false;
    }
    
    if ( ! contentType.toLower().startsWith("application/json")) {
        errorMessageOut = ("Requesting study ID failed.  Content type return by BALSA should be JSON format but is "
                           + contentType);
        return false;
    }

    studyResponse.m_body.push_back('\0');
    AString responseContent(&studyResponse.m_body[0]);
    
    if (m_debugFlag) {
        std::cout << "Request study ID body:\n" << responseContent << std::endl << std::endl;
    }
    
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toUtf8(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Requesting study ID failed.  Failed to parse JSON, error:"
                           + jsonError.errorString()
                           + " Offset="
                           + AString::number(jsonError.offset));
        return false;
    }
    
    QByteArray json = jsonDocument.toJson();
    if (m_debugFlag) {
        std::cout << "Array?: " << jsonDocument.isArray() << std::endl;
        std::cout << "Empty?: " << jsonDocument.isEmpty() << std::endl;
        std::cout << "NULL?: " << jsonDocument.isNull() << std::endl;
        std::cout << "Object?: " << jsonDocument.isObject() << std::endl;
        std::cout << "JSON length: " << json.size() << std::endl;
        AString str(json);
        std::cout << ("Formatted JSON:\n" + str) << std::endl;
    }
    
    if ( ! jsonDocument.isObject()) {
        errorMessageOut  = ("Requesting study ID failed.  JSON content is not an object."
                            "JSON is displayed in terminal if logging level is warning or higher.");
        CaretLogWarning("Study ID JSON is not Object\n"
                        + AString(jsonDocument.toJson()));
        return false;
    }
    
    BalsaStudyInformation bsi(jsonDocument.object());
    if ( ! bsi.getStudyID().isEmpty()) {
        studyIDOut = bsi.getStudyID();
        return true;
    }
    
    return false;
}

/**
 * Login to BALSA and request a study ID from a study title.
 *
 * @param studyTitle
 *     The study's titlee.
 * @param studyIdOut
 *     Output containing the study ID.
 * @param errorMessageOut
 *     Contains description of any error(s).
 * @return
 *     True if processing was successful, else false.
 */
bool
BalsaDatabaseManager::getStudyIDFromStudyTitle(const AString& studyTitle,
                                               AString& studyIdOut,
                                               AString& errorMessageOut)
{
    if (studyTitle.isEmpty()) {
        errorMessageOut = "The study title is empty.";
        return false;
    }
    
    if ( ! requestStudyID(m_databaseURL,
                          studyTitle,
                          studyIdOut,
                          errorMessageOut)) {
        return false;
    }
    
    return true;
}

/**
 * Get the User's Roles.
 *
 * @param userRolesOut
 *     Output with user's roles.
 * @param errorMessageOut
 *     Contains description of any error(s).
 * @return
 *     True if processing was successful, else false.
 */
bool
BalsaDatabaseManager::getUserRoles(BalsaUserRoles& userRolesOut,
                                   AString& errorMessageOut)
{
    userRolesOut.resetToAllInvalid();
    
    const AString studyIdURL(m_databaseURL
                             + "/user/roles");
    
    errorMessageOut.clear();
    
    CaretHttpRequest caretRequest;
    caretRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    caretRequest.m_url    = studyIdURL;
    caretRequest.m_headers.insert(std::make_pair("Content-Type",
                                                 "application/x-www-form-urlencoded"));
    caretRequest.m_headers.insert(std::make_pair("Cookie",
                                                 getJSessionIdCookie()));
    caretRequest.m_arguments.push_back(std::make_pair("type",
                                                      "json"));
    
    CaretHttpResponse studyResponse;
    CaretHttpManager::httpRequest(caretRequest, studyResponse);
    
    if (m_debugFlag) {
        std::cout << "Request roles response Code: " << studyResponse.m_responseCode << std::endl;
    }
    
    if (studyResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting roles failed with HTTP code="
                           + AString::number(studyResponse.m_responseCode)
                           + ".  ");
        errorMessageOut.appendWithNewLine(studyResponse.m_errorMessage);
        return false;
    }
    
    AString contentType = getHeaderValue(studyResponse, "content-type");
    if (contentType.isNull()) {
        errorMessageOut = ("Requesting roles failed.  Content type returned by BALSA is unknown");
        return false;
    }
    
    if ( ! contentType.toLower().startsWith("application/json")) {
        errorMessageOut = ("Requesting rolesfailed.  Content type return by BALSA should be JSON format but is "
                           + contentType);
        return false;
    }
    
    studyResponse.m_body.push_back('\0');
    AString responseContent(&studyResponse.m_body[0]);
    
    if (m_debugFlag) {
        std::cout << "Request roles body:\n" << responseContent << std::endl << std::endl;
    }
    
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toUtf8(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Requesting roles failed.  Failed to parse JSON, error:"
                           + jsonError.errorString()
                           + " Offset="
                           + AString::number(jsonError.offset));
        return false;
    }
    
    QByteArray json = jsonDocument.toJson();
    if (m_debugFlag) {
        std::cout << "Array?: " << jsonDocument.isArray() << std::endl;
        std::cout << "Empty?: " << jsonDocument.isEmpty() << std::endl;
        std::cout << "NULL?: " << jsonDocument.isNull() << std::endl;
        std::cout << "Object?: " << jsonDocument.isObject() << std::endl;
        std::cout << "JSON length: " << json.size() << std::endl;
        AString str(json);
        std::cout << ("Formatted JSON:\n" + str) << std::endl;
    }
    
    if ( ! jsonDocument.isArray()) {
        errorMessageOut  = ("Requesting roles failed.  JSON content is not an array."
                            "JSON is displayed in terminal if logging level is warning or higher.");
        CaretLogWarning("Roles JSON is not Array\n"
                        + AString(jsonDocument.toJson()));
        return false;
    }
    
    userRolesOut.parseJson(jsonDocument.array());
    
    if (m_debugFlag) {
        std::cout << "**** Roles: " << userRolesOut.toString() << std::endl;
    }
    
    return userRolesOut.isValid();
}

/**
 * Get the Extract Directory Prefix for the given study.
 *
 * @param studyID
 *     The Study ID.
 * @param extractDirectoryPrefixOut
 *     Output containing the directory prefix from BALSA.
 * @param errorMessageOut
 *     Contains description of any error(s).
 * @return
 *     True if processing was successful, else false.
 */
bool
BalsaDatabaseManager::getStudyExtractDirectoryPrefix(const AString& studyID,
                                                     AString& extractDirectoryPrefixOut,
                                                     AString& errorMessageOut)
{
    const AString studyIdURL(m_databaseURL
                             + "/study/extractdir/"
                             + studyID.trimmed());
    
    extractDirectoryPrefixOut.clear();
    errorMessageOut.clear();
    
    CaretHttpRequest caretRequest;
    caretRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    caretRequest.m_url    = studyIdURL;
    caretRequest.m_headers.insert(std::make_pair("Content-Type",
                                                 "application/x-www-form-urlencoded"));
    caretRequest.m_headers.insert(std::make_pair("Cookie",
                                                 getJSessionIdCookie()));
    caretRequest.m_arguments.push_back(std::make_pair("type",
                                                      "utf-8"));
    
    CaretHttpResponse studyResponse;
    CaretHttpManager::httpRequest(caretRequest, studyResponse);
    
    if (m_debugFlag) {
        std::cout << "Request extract directory response Code: " << studyResponse.m_responseCode << std::endl;
    }
    
    if (studyResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting extract directory failed with HTTP code="
                           + AString::number(studyResponse.m_responseCode)
                           + ".  ");
        errorMessageOut.appendWithNewLine(studyResponse.m_errorMessage);
        return false;
    }
    
    AString contentType = getHeaderValue(studyResponse, "content-type");
    if (contentType.isNull()) {
        errorMessageOut = ("Requesting extract directory failed.  Content type returned by BALSA is unknown");
        return false;
    }
    
    studyResponse.m_body.push_back('\0');
    AString responseContent(&studyResponse.m_body[0]);
    
    if (m_debugFlag) {
        std::cout << "Request extract directory body:\n" << responseContent << std::endl << std::endl;
    }
    
    if ( ! contentType.toLower().startsWith("text/html;charset=utf-8")) {
        errorMessageOut = ("Requesting exxtract directory failed.  Content type return by BALSA should be text/html;charset=utf-8 format but is "
                           + contentType);
        return false;
    }
    
    extractDirectoryPrefixOut = responseContent;
    
    /*
     * "null" is returned by BALSA when the directory is empty
     */
    if (extractDirectoryPrefixOut == "null") {
        extractDirectoryPrefixOut.clear();
    }

    return true;
}


/**
 * Request study information.
 *
 * @param studyInformationOut
 *     Output containing information for all of the user's studies.
 * @param errorMessageOut
 *     Contains description of any error(s).
 * @return
 *     True if processing was successful, else false.
 */
bool
BalsaDatabaseManager::getAllStudyInformation(std::vector<BalsaStudyInformation>& studyInformationOut,
                                             AString& errorMessageOut)
{
    studyInformationOut.clear();
    errorMessageOut = "";
    
    const AString studyIdURL(m_databaseURL
                             + "/study/mine");
    
    errorMessageOut.clear();
    
    CaretHttpRequest caretRequest;
    caretRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    caretRequest.m_url    = studyIdURL;
    caretRequest.m_headers.insert(std::make_pair("Content-Type",
                                                  "application/x-www-form-urlencoded"));
    caretRequest.m_headers.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    caretRequest.m_arguments.push_back(std::make_pair("type",
                                                       "json"));
    
    CaretHttpResponse idResponse;
    CaretHttpManager::httpRequest(caretRequest, idResponse);
    
    idResponse.m_body.push_back('\0');
    AString responseContent(&idResponse.m_body[0]);
    CaretLogFine("Get All Study Information Response from "
                 + studyIdURL
                 + ", Response Code="
                 + AString::number(idResponse.m_responseCode)
                 + "\nContent:\n"
                 + responseContent);
    
    if (m_debugFlag) {
        std::cout << "Request all studies response Code: " << idResponse.m_responseCode << std::endl;
    }
    
    if (idResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting all study information failed with HTTP code="
                           + AString::number(idResponse.m_responseCode)
                           + ".  This error may be caused by failure to agree to data use terms.  ");
        errorMessageOut.appendWithNewLine(idResponse.m_errorMessage);
        return false;
    }
    
    AString contentType = getHeaderValue(idResponse, "content-type");
    if (contentType.isNull()) {
        errorMessageOut = ("Requesting all study information failed.  Content type returned by BALSA is unknown");
        return false;
    }
    
    if ( ! contentType.toLower().startsWith("application/json")) {
        errorMessageOut = ("Requesting all study information failed.  Content type return by BALSA should be JSON format but is "
                           + contentType);
        return false;
    }
    
    
    if (m_debugFlag) {
        std::cout << "Request all studies reply body:\n" << responseContent << std::endl << std::endl;
    }
    
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toUtf8(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Requesting all study information failed.  Failed to parse JSON, error:"
                           + jsonError.errorString()
                           + " Offset="
                           + AString::number(jsonError.offset));
        return false;
    }
    QByteArray json = jsonDocument.toJson();
    
    if (m_debugFlag) {
        std::cout << "Array?: " << jsonDocument.isArray() << std::endl;
        std::cout << "Empty?: " << jsonDocument.isEmpty() << std::endl;
        std::cout << "NULL?: " << jsonDocument.isNull() << std::endl;
        std::cout << "Object?: " << jsonDocument.isObject() << std::endl;
        std::cout << "JSON length: " << json.size() << std::endl;
        AString str(json);
        std::cout << ("Formatted JSON:\n" + str) << std::endl;
    }

    if ( ! jsonDocument.isArray()) {
        errorMessageOut  = ("Requesting all study information failed.  JSON content is not an array."
                            "JSON is displayed in terminal if logging level is warning or higher.");
        CaretLogWarning("Study Information JSON is not Array\n"
                        + AString(jsonDocument.toJson()));
        return false;
    }
    
    QJsonArray jsonArray = jsonDocument.array();
    for (QJsonArray::iterator iter = jsonArray.begin();
         iter != jsonArray.end();
         iter++) {
        QJsonValue jsonValue = *iter;
        if ( ! jsonValue.isNull()) {
            if (jsonValue.isObject()) {
                QJsonObject studyInfo = jsonValue.toObject();
                
                BalsaStudyInformation bsi(studyInfo);
                if ( ! bsi.isEmpty()) {
                    studyInformationOut.push_back(bsi);
                }
            }
        }
    }

    if ( ! studyInformationOut.empty()) {
        std::sort(studyInformationOut.begin(),
                  studyInformationOut.end());
    }
    
    return true;
}

/**
 * @return Is the given Study ID valid (in BALSA)?
 *
 * @param studyID
 *     The study ID.
 */
bool
BalsaDatabaseManager::isStudyIDValid(const AString& studyID)
{
    std::vector<BalsaStudyInformation> studyInformation;
    AString errorMessage;
    
    if (getAllStudyInformation(studyInformation,
                               errorMessage)) {
        for (const auto& info : studyInformation) {
            if (info.getStudyID() == studyID) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @return Is the study editable by the user?
 * 
 * @param studyID
 *     The study ID.
 * @param errorMessageOut
 *      Description of error if false is returned.
 */
bool
BalsaDatabaseManager::isStudyEditableByUser(const AString& studyID,
                                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    std::vector<BalsaStudyInformation> studyInformation;
    AString errorMessage;
    
    if (getAllStudyInformation(studyInformation,
                               errorMessage)) {
        for (const auto& info : studyInformation) {
            if (info.getStudyID() == studyID) {
                if (info.isEditable()) {
                    return true;
                }
                else {
                    errorMessageOut = ("The study (ID="
                                       + studyID
                                       + ") is not editable.");
                    return false;
                }
            }
        }
        
        errorMessageOut = ("You are not the owner of the study (ID="
                           + studyID
                           + ").");
        return false;
    }
    else {
        errorMessageOut = errorMessage;
    }
    
    return false;
}


/**
 * Login to BALSA, zip the scene and data files, and upload the
 * ZIP file to BALSA.
 *
 * @param sceneFile
 *     Name of scene file.
 * @param zipFileName
 *     Name for ZIP file.
 * @param extractToDirectoryName
 *     Directory for extraction of ZIP file.
 * @param errorMessageOut
 *     Contains description of any error(s).
 * @return
 *     True if processing was successful, else false.
 */
bool
BalsaDatabaseManager::uploadZippedSceneFile(SceneFile* sceneFile,
                                            const AString& zipFileName,
                                            const AString& extractToDirectoryName,
                                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    m_infoMessages.clear();
    
    /*
     * Check for errors
     */
    if (sceneFile == NULL) {
        errorMessageOut = "Scene file is invalid.";
        return false;
    }
    const AString sceneFileName = sceneFile->getFileName();
    if (sceneFileName.isEmpty()) {
        errorMessageOut = "Scene file does not have a name.";
        return false;
    }
    if (sceneFile->getNumberOfScenes() <= 0) {
        errorMessageOut = "Scene file does not contain any scenes.";
        return false;
    }
    
    if (zipFileName.isEmpty()) {
        errorMessageOut = "Zip file does not have a name.";
        return false;
    }
    if ( ! zipFileName.endsWith(".zip")) {
        errorMessageOut = "Zip file name must end with \".zip\"";
        return false;
    }
    
    if (extractToDirectoryName.isEmpty()) {
        errorMessageOut = "The extract directory name is empty.";
        return false;
    }
    
    CaretLogFine("Uploading to BALSA with Version " + ApplicationInformation().getVersion());

    enum ProgressEnum {
        PROGRESS_NONE,
        PROGRESS_LOGIN,
        PROGRESS_CHECK_SCENE_IDS,
        PROGRESS_ZIPPING,
        PROGRESS_UPLOAD,
        PROGRESS_PROCESS_UPLOAD,
        PROGRESS_DONE
    };
    
    EventProgressUpdate progressUpdate(PROGRESS_NONE,
                                       PROGRESS_DONE,
                                       PROGRESS_LOGIN,
                                       "Logging in...");
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    
    
    progressUpdate.setProgress(PROGRESS_CHECK_SCENE_IDS, addToUploadProgressMessage("Checking Scene IDs"));
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    if ( ! checkSceneIDs(sceneFile,
                         errorMessageOut)) {
        return false;
    }
    
    progressUpdate.setProgress(PROGRESS_ZIPPING, addToUploadProgressMessage("Zipping Scene and Data Files"));
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    
    /*
     * Zip the scene file and its data files
     */
    if ( ! zipSceneAndDataFiles(sceneFile,
                                extractToDirectoryName,
                                zipFileName,
                                errorMessageOut)) {
        return false;
    }
    
    if (m_debugFlag) std::cout << "Zip file " << zipFileName << " has been created " << std::endl;
    
    progressUpdate.setProgress(PROGRESS_UPLOAD, addToUploadProgressMessage("Uploading zip file"));
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    
    /*
     * Upload the ZIP file
     */
    AString uploadResultText;
    const AString uploadURL(m_databaseURL
                            + "/study/handleUpload/"
                            + sceneFile->getBalsaStudyID());
    const bool uploadSuccessFlag = uploadFile(uploadURL,
                                              zipFileName,
                                              "application/zip",
                                              uploadResultText,
                                              errorMessageOut);
    if (m_debugFlag) std::cout << "Output of uploading zip file: " << uploadResultText << std::endl;
    
    
    if ( ! uploadSuccessFlag) {
        return false;
    }
    
    const bool doProcessUploadFlag = true;
    if (doProcessUploadFlag) {
        /*
         * Process the uploaded file
         */
        progressUpdate.setProgress(PROGRESS_PROCESS_UPLOAD, 
                                   addToUploadProgressMessage("Processing uploaded zip file (this step may take a long time)"));
        EventManager::get()->sendEvent(progressUpdate.getPointer());
        
        const AString processUploadURL(m_databaseURL
                                       + "/study/processUpload/"
                                       + sceneFile->getBalsaStudyID());
        const bool processUploadSuccessFlag = processUploadedFile(sceneFile,
                                                                  processUploadURL,
                                                                  "application/x-www-form-urlencoded",
                                                                  false, // do not look for Scene IDs in response content
                                                                  errorMessageOut);
        
        if (m_debugFlag) std::cout << "Result of processing the uploaded ZIP file" << AString::fromBool(processUploadSuccessFlag) << std::endl;
        if ( ! processUploadSuccessFlag) {
            return false;
        }
    }
    else {
        CaretLogSevere("PROCESSING OF FILE UPLOADED TO BALSA IS DISABLED");
    }

    if (QFile::exists(zipFileName)) {
        if ( ! QFile::remove(zipFileName)) {
            CaretLogWarning("Unable to delete Zip file after uploading: "
                            + zipFileName);
        }
    }
    
    progressUpdate.setProgress(PROGRESS_DONE, addToUploadProgressMessage("Finished."));
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    
    return true;
}

/**
 * Constructor that parses JSON containing scene file name and identifiers.
 *
 * Example: ["FileName.scene",[[1,"55XX"],[0,"n51z"]]]
 *
 * @param debugFlag
 *     Debugging flag
 * @param jsonValue
 *     The JSON value containing the array.
 */
BalsaDatabaseManager::SceneFileIdentifiers::SceneFileIdentifiers(const bool debugFlag,
                                                                 const QJsonValue& jsonValue)
: m_debugFlag(debugFlag)
{
    if ( ! jsonValue.isArray()) {
        m_errorMessage = ("Scene File Name/Identifier is not JSON array \""
                          + JsonHelper::jsonValueToString(jsonValue)
                          + "\"");
        return;
    }
    
    if (m_debugFlag) {
        std::cout << "Scene IDs Content: " << JsonHelper::jsonValueToString(jsonValue) << std::endl;
    }

    
    const QJsonArray jsonArray = jsonValue.toArray();
    if (jsonArray.count() != 2) {
        m_errorMessage = ("Scene File Name/Identifier array should have two elements \""
                          + JsonHelper::jsonValueToString(jsonValue)
                          + "\"");
        return;
    }
    
    const QJsonValue sceneFileNameJsonValue = jsonArray.at(0);
    if ( ! sceneFileNameJsonValue.isString()) {
        m_errorMessage = ("Scene File Name is not a string \""
                          + JsonHelper::jsonValueToString(sceneFileNameJsonValue)
                          + "\"");
        return;
    }
    m_sceneFileName = sceneFileNameJsonValue.toString();
    
    const QJsonValue sceneIDsJsonValue = jsonArray.at(1);
    if ( ! sceneIDsJsonValue.isArray()) {
        m_errorMessage = ("Scene IDs is not an array \""
                          + JsonHelper::jsonValueToString(sceneFileNameJsonValue)
                          + "\"");
        
        return;
    }
    
    const QJsonArray sceneIDsArray = sceneIDsJsonValue.toArray();
    const int numSceneIDs = sceneIDsArray.count();
    for (int32_t i = 0; i < numSceneIDs; i++) {
        const QJsonValue sceneIDJsonElementValue = sceneIDsArray.at(i);
        if (m_debugFlag) {
            std::cout << "Scene Index/ID: " << i << " " << JsonHelper::jsonValueToString(sceneIDJsonElementValue) << std::endl;
        }
        
        if ( ! sceneIDJsonElementValue.isArray()) {
            m_errorMessage = ("Scene Index/ID is not an array \""
                              + JsonHelper::jsonValueToString(sceneIDJsonElementValue)
                              + "\"");
            
            return;
        }
        
        const QJsonArray sceneIDJsonArray = sceneIDJsonElementValue.toArray();
        if (sceneIDJsonArray.count() != 2) {
            m_errorMessage = ("Scene Index/ID array should have two elements \""
                              + JsonHelper::jsonValueToString(sceneIDJsonArray)
                              + "\"");
            return;
        }
        
        const QJsonValue sceneIndexValue = sceneIDJsonArray.at(0);
        if ( ! sceneIndexValue.isDouble()) {
            m_errorMessage = ("Scene Index (first element) is not a number \""
                              + JsonHelper::jsonValueToString(sceneIndexValue)
                              + "\"");
            return;
        }
        
        const QJsonValue sceneIDValue = sceneIDJsonArray.at(1);
        if ( ! sceneIDValue.isString()) {
            m_errorMessage = ("Scene ID (second element) is not a string \""
                              + JsonHelper::jsonValueToString(sceneIDJsonArray)
                              + "\"");
            return;
        }
        
        const int32_t invalidIndex = -1;
        const int32_t sceneIndex = sceneIndexValue.toInt(invalidIndex);
        if (sceneIndex == invalidIndex) {
            m_errorMessage = ("Scene Index (first element) is not an integer \""
                              + JsonHelper::jsonValueToString(sceneIndexValue)
                              + "\"");
            return;
        }
        
        const AString sceneID = sceneIDValue.toString();
        
        m_sceneIndexAndIDsMap.insert(std::make_pair(sceneIndex,
                                                    sceneID));
    }
}

/**
 * @return The upload progress message containing a summary of the upload progress
 */
AString
BalsaDatabaseManager::getUploadSummaryMessage() const
{
    return m_uploadProgressMessage;
}

/**
 * Add a message to the progress message which gets followed by the current time
 * @param message
 *    New message appended to progress message
 * @return
 *    Updated progress message
 */
AString
BalsaDatabaseManager::addToUploadProgressMessage(const AString& message)
{
    if ( ! message.isEmpty()) {
        /*
         * Time followed by timezone
         */
        const AString timeString(QDateTime::currentDateTime().toString("HH:mm:ss t"));
        m_uploadProgressMessage.appendWithNewLine(message
                                                  + " "
                                                  + timeString);
    }
    
    return m_uploadProgressMessage;
}

/**
 * @return True if parsing was valid.
 */
bool
BalsaDatabaseManager::SceneFileIdentifiers::isValid() const
{
    return (m_errorMessage.isEmpty());
}

/**
 * @return Error message describing parsing problem.
 */
AString
BalsaDatabaseManager::SceneFileIdentifiers::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * @return Information messages from uploading
 */
AString
BalsaDatabaseManager::getInfoMessages() const
{
    return m_infoMessages;
}

