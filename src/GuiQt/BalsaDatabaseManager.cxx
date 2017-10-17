
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

#include <QJsonDocument>

#include "BalsaUserRoles.h"
#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "CommandOperationManager.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "FileInformation.h"
#include "OperationZipSceneFile.h"
#include "ProgramParameters.h"
#include "SceneFile.h"

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
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
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
    
    for (std::map<AString, AString>::iterator mapIter = loginResponse.m_headers.begin();
         mapIter != loginResponse.m_headers.end();
         mapIter++) {
        
        if (mapIter->first == "Set-Cookie") {
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
            errorMessageOut = ("Login was successful but BALSA failed to provide a Session ID.");
            logout();
            return false;
        }
        
        return true;
    }
    
    loginResponse.m_body.push_back('\0');
    const AString responseContent(&loginResponse.m_body[0]);
    errorMessageOut = ("Login has failed.\n"
                       "HTTP Code: " + AString::number(loginResponse.m_responseCode)
                       + " Content: " + responseContent);
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

    
    return processUploadResponse(uploadResponse.m_headers,
                                 responseContentOut,
                                 uploadResponse.m_responseCode,
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
 * @param errorMessageOut
 *     Contains description of error.
 * @return
 *     True if response shows upload was successful, else false.
 */
bool
BalsaDatabaseManager::processUploadResponse(const std::map<AString, AString>& responseHeaders,
                                            const AString& responseContent,
                                            const int32_t responseHttpCode,
                                            AString& errorMessageOut) const
{
    if (responseHeaders.empty()) {
        if (m_debugFlag) std::cout << "Response headers from upload are empty." << std::endl;
    }
    
    bool haveContentTypeFlag = false;
    AString contentTypeString;
    for (std::map<AString, AString>::const_iterator iter = responseHeaders.begin();
         iter != responseHeaders.end();
         iter++) {
        if (m_debugFlag) std::cout << "Response Header: " << iter->first << " -> " << iter->second << std::endl;
        
        if (iter->first == "Content-Type") {
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
    {
        QJsonParseError jsonError;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toLatin1(),
                                                             &jsonError);
        if (jsonDocument.isNull()) {
            errorMessageOut = ("Process upload response failed.  Failed to parse JSON, error:"
                               + jsonError.errorString());
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
    
    if (responseHttpCode != 200) {
        if ( ! statusText.isEmpty()) {
            contentErrorMessage.insert(0, statusText + "\n");
        }
        
        errorMessageOut = ("Upload failed.  Http Code="
                           + AString::number(responseHttpCode)
                           + "\n"
                           "   " + contentErrorMessage);
        return false;
    }
    
    if ( ! haveJsonResponseContentFlag) {
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
    
    AString basePathName = sceneFile->getBalsaBaseDirectory();
    
    switch (sceneFile->getBasePathType()) {
        case SceneFileBasePathTypeEnum::AUTOMATIC:
            basePathName = sceneFile->findBaseDirectoryForDataFiles();
            if ( ! FileInformation(basePathName).exists()) {
                errorMessageOut = ("AUTOMATIC base path mode produced an invalid base path (directory does not exist): \""
                                   + basePathName
                                   + "\"");
                return false;
            }
            break;
        case SceneFileBasePathTypeEnum::CUSTOM:
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
        OperationZipSceneFile::createZipFile(sceneFileName,
                                             extractToDirectoryName,
                                             zipFileName,
                                             basePathName,
                                             OperationZipSceneFile::PROGRESS_GUI_EVENT,
                                             NULL);
        successFlag = true;
    }
    catch (const CaretException& e) {
        errorMessageOut = e.whatString();
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
 * @param processUploadURL
 *     URL for uploading file.
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
BalsaDatabaseManager::processUploadedFile(const AString& processUploadURL,
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
    
    CaretHttpRequest uploadRequest;
    uploadRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    uploadRequest.m_url    = processUploadURL;
    uploadRequest.m_headers.insert(std::make_pair("Content-Type",
                                                  httpContentTypeName));
    uploadRequest.m_headers.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    
    CaretHttpResponse uploadResponse;
    CaretHttpManager::httpRequest(uploadRequest, uploadResponse);
    
    if (m_debugFlag) {
        std::cout << "Process upload response Code: " << uploadResponse.m_responseCode << std::endl;
    }
    
    for (std::map<AString, AString>::iterator mapIter = uploadResponse.m_headers.begin();
         mapIter != uploadResponse.m_headers.end();
         mapIter++) {
        if (m_debugFlag) {
            std::cout << "   Process Upload Response Header: " << qPrintable(mapIter->first)
            << " -> " << qPrintable(mapIter->second) << std::endl;
        }
    }
    
    uploadResponse.m_body.push_back('\0');
    responseContentOut.append(&uploadResponse.m_body[0]);
    CaretLogFine("Process Upload to BALSA reply body: "
                 + responseContentOut);
    
    if (uploadResponse.m_responseCode == 200) {
        return true;
    }
    
    errorMessageOut = ("Process Upload failed code: "
                       + QString::number(uploadResponse.m_responseCode)
                       + "\n"
                       + responseContentOut);
    
    return false;
}

/**
 * Get the value for the given header name using
 * a case-insensitive string comparison.
 *
 * @param headerName
 *     The header name.
 * @return
 *     Value for header name.  If the header name is
 *     not found the returned string will be NULL
 *     (test with .isNull()).
 */
static AString getHeaderValue(const CaretHttpResponse& httpResponse,
                              const AString& headerName)
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
                           + ".  This error may be caused by failure to agree to data use terms.");
        return false;
    }
    
    AString contentType = getHeaderValue(studyResponse, "Content-Type");
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
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toLatin1(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Requesting study ID failed.  Failed to parse JSON, error:"
                           + jsonError.errorString());
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

///**
// * Login to BALSA and request a study ID from a study title.
// *
// * @param studyTitle
// *     The study's title.
// * @param studyIdOut
// *     Output containing the study ID.
// * @param errorMessageOut
// *     Contains description of any error(s).
// * @return
// *     True if processing was successful, else false.
// */
//bool
//BalsaDatabaseManager::getUserRoles(AString& roleNamesOut,
//                                   AString& errorMessageOut)
//{
//    if ( ! requestUserRoles(m_databaseURL,
//                          roleNamesOut,
//                          errorMessageOut)) {
//        return false;
//    }
//    
//    return true;
//}
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
        std::cout << "Request rolese response Code: " << studyResponse.m_responseCode << std::endl;
    }
    
    if (studyResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting roles failed with HTTP code="
                           + AString::number(studyResponse.m_responseCode));
        return false;
    }
    
    AString contentType = getHeaderValue(studyResponse, "Content-Type");
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
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toLatin1(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Requesting roles failed.  Failed to parse JSON, error:"
                           + jsonError.errorString());
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
    
    if (m_debugFlag) {
        std::cout << "Request all studies response Code: " << idResponse.m_responseCode << std::endl;
    }
    
    if (idResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting all study information failed with HTTP code="
                           + AString::number(idResponse.m_responseCode)
                           + ".  This error may be caused by failure to agree to data use terms.");
        return false;
    }
    
    AString contentType = getHeaderValue(idResponse, "Content-Type");
    if (contentType.isNull()) {
        errorMessageOut = ("Requesting all study information failed.  Content type returned by BALSA is unknown");
        return false;
    }
    
    if ( ! contentType.toLower().startsWith("application/json")) {
        errorMessageOut = ("Requesting all study information failed.  Content type return by BALSA should be JSON format but is "
                           + contentType);
        return false;
    }
    
    idResponse.m_body.push_back('\0');
    AString responseContent(&idResponse.m_body[0]);
    
    if (m_debugFlag) {
        std::cout << "Request all studies reply body:\n" << responseContent << std::endl << std::endl;
    }
    
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseContent.toLatin1(),
                                                         &jsonError);
    if (jsonDocument.isNull()) {
        errorMessageOut = ("Requesting all study information failed.  Failed to parse JSON, error:"
                           + jsonError.errorString());
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
    
    if (studyInformationOut.empty()) {
        errorMessageOut = "No study information was found.";
        return false;
    }
    
    std::sort(studyInformationOut.begin(),
              studyInformationOut.end());
    
    return true;
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
BalsaDatabaseManager::uploadZippedSceneFile(const SceneFile* sceneFile,
                                            const AString& zipFileName,
                                            const AString& extractToDirectoryName,
                                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
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
    
    enum ProgressEnum {
        PROGRESS_NONE,
        PROGRESS_LOGIN,
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
    
    progressUpdate.setProgress(PROGRESS_ZIPPING, "Zipping Scene and Data Files");
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
    
    if (m_debugFlag) std::cout << "Zip file has been created " << std::endl;
    
    progressUpdate.setProgress(PROGRESS_UPLOAD, "Uploading zip file");
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
        progressUpdate.setProgress(PROGRESS_PROCESS_UPLOAD, "Processing uploaded zip file");
        EventManager::get()->sendEvent(progressUpdate.getPointer());
        
        const AString processUploadURL(m_databaseURL
                                       + "/study/processUpload/"
                                       + sceneFile->getBalsaStudyID());
        AString processUploadResultText;
        const bool processUploadSuccessFlag = processUploadedFile(processUploadURL,
                                                                  "application/x-www-form-urlencoded",
                                                                  processUploadResultText,
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
    
    progressUpdate.setProgress(PROGRESS_DONE, "Finished.");
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    return true;
}


