
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

#include <iostream>

#define __BALSA_DATABASE_MANAGER_DECLARE__
#include "BalsaDatabaseManager.h"
#undef __BALSA_DATABASE_MANAGER_DECLARE__

#include <QJsonDocument>

#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretJsonObject.h"
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
 * @param loginURL
 *     URL for logging in.
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
BalsaDatabaseManager::login(const AString& loginURL,
                            const AString& username,
                            const AString& password,
                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    m_username = username.trimmed();
    m_password = password.trimmed();
    m_jSessionIdCookie = "";
    
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
            return false;
        }
        
        return true;
    }
    
    loginResponse.m_body.push_back('\0');
    const AString responseContent(&loginResponse.m_body[0]);
    errorMessageOut = ("Login has failed.\n"
                       "HTTP Code: " + AString::number(loginResponse.m_responseCode)
                       + " Content: " + responseContent);

    return false;
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
    
//    std::cout << std::endl << "RESPONSE CONTENT: " << responseContent << std::endl << std::endl;
//    
//    QJsonDocument jsonDocument = QJsonDocument::fromRawData(responseContent.toLatin1().constData(),
//                                                            responseContent.length());
    
    CaretJsonObject json(responseContent);
    
    if (responseHttpCode != 200) {
        AString msg = json.value("statusText");
        if ( ! msg.isEmpty()) {
            contentErrorMessage.insert(0, msg + "\n");
        }
        
        errorMessageOut = ("Upload failed.  Http Code="
                           + AString::number(responseHttpCode)
                           + "\n"
                           "   " + contentErrorMessage);
        return false;
    }
    
    if ( ! haveJsonResponseContentFlag) {
        errorMessageOut = (contentErrorMessage);
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
    
    AString baseDirectoryName;
    if ( ! sceneFile->getBalsaBaseDirectory().isEmpty()) {
        /* validate ? */
        baseDirectoryName = sceneFile->getBalsaBaseDirectory();
    }
    
    bool successFlag = false;
    try {
        OperationZipSceneFile::createZipFile(sceneFileName,
                                             extractToDirectoryName,
                                             zipFileName,
                                             baseDirectoryName,
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
    
    CaretHttpRequest uploadRequest;
    uploadRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    uploadRequest.m_url    = studyIdURL;
    uploadRequest.m_headers.insert(std::make_pair("Content-Type",
                                                  "application/x-www-form-urlencoded"));
    uploadRequest.m_headers.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    uploadRequest.m_arguments.push_back(std::make_pair("title",
                                                  studyTitle));
    
    CaretHttpResponse idResponse;
    CaretHttpManager::httpRequest(uploadRequest, idResponse);
    
    if (m_debugFlag) {
        std::cout << "Request study ID response Code: " << idResponse.m_responseCode << std::endl;
    }
    
    AString locationFromHeader;
    for (std::map<AString, AString>::iterator mapIter = idResponse.m_headers.begin();
         mapIter != idResponse.m_headers.end();
         mapIter++) {
        if (m_debugFlag) {
            std::cout << "   Request study ID Response Header: " << qPrintable(mapIter->first)
            << " -> " << qPrintable(mapIter->second) << std::endl;
        }
        if (mapIter->first.toLower() == "location") {
            locationFromHeader = mapIter->second;
        }
    }
    
    idResponse.m_body.push_back('\0');
    AString responseContent(&idResponse.m_body[0]);
    
    if (m_debugFlag) {
        std::cout << "Request Study ID reply body:" << responseContent << std::endl;
    }
    
    CaretLogFine("Request Study ID to BALSA reply body: "
                 + responseContent);
    
    if (idResponse.m_responseCode != 200) {
        errorMessageOut = ("Requesting study ID failed HTTP code: "
                           + QString::number(idResponse.m_responseCode)
                           + "\n"
                           "Could be due failure to agree to data user terms.");
        
        return false;
    }
    
    if ( ! locationFromHeader.isEmpty()) {
        const int lastSlashIndex = locationFromHeader.lastIndexOf('/');
        if (lastSlashIndex >= 0) {
            studyIDOut = locationFromHeader.mid(lastSlashIndex + 1);
            if (studyIDOut.isNull()
                || studyIDOut.isEmpty()) {
                errorMessageOut = ("Received 'Location' from BALSA but unable to find study ID in the URL: "
                                   + locationFromHeader);
                return false;
            }
            
            /*
             * Have the Study ID
             */
            return true;
        }
        else {
            errorMessageOut = ("Received 'Location' from BALSA but unable to find study ID in the URL: "
                               + locationFromHeader);
        }
    }
    else {
        if (responseContent.indexOf("must be unique") >= 0) {
            errorMessageOut = ("A study with the title \""
                               + studyTitle
                               + "\" already exists and duplicate study titles are not allowed in the BALSA database.");
        }
        else {
            errorMessageOut = "Unable to find 'Location' in response from BALSA.";
        }
    }
    
    return false;
}

/**
 * Login to BALSA and request a study ID from a study title.
 *
 * @param databaseURL
 *     URL of the database.
 * @param username
 *     Username for logging in.
 * @param password
 *     Password for logging in.
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
BalsaDatabaseManager::getStudyIDFromStudyTitle(const AString& databaseURL,
                                               const AString& username,
                                               const AString& password,
                                               const AString& studyTitle,
                                               AString& studyIdOut,
                                               AString& errorMessageOut)
{
    if (studyTitle.isEmpty()) {
        errorMessageOut = "The study title is empty.";
        return false;
    }
    
    /*
     * Login
     */
    const AString loginURL(databaseURL
                           + "/j_spring_security_check");
    if ( ! login(loginURL,
                 username,
                 password,
                 errorMessageOut)) {
        
        return false;
    }
    
    CaretLogInfo("SESSION ID from BALSA Login: "
                 + getJSessionIdCookie());
    
    if ( ! requestStudyID(databaseURL,
                          studyTitle,
                          studyIdOut,
                          errorMessageOut)) {
        return false;
    }
    
    return true;
}

/**
 * Login to BALSA, zip the scene and data files, and upload the
 * ZIP file to BALSA.
 *
 * @param databaseURL
 *     URL of the database.
 * @param username
 *     Username for logging in.
 * @param password
 *     Password for logging in.
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
BalsaDatabaseManager::uploadZippedSceneFile(const AString& databaseURL,
                           const AString& username,
                           const AString& password,
                           const SceneFile* sceneFile,
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
    
    /*
     * Login
     */
    const AString loginURL(databaseURL
                           + "/j_spring_security_check");
    if ( ! login(loginURL,
                 username,
                 password,
                 errorMessageOut)) {
        
        return false;
    }
    
    CaretLogInfo("SESSION ID from BALSA Login: "
                 + getJSessionIdCookie());
    
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
    const AString uploadURL(databaseURL
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
        
        const AString processUploadURL(databaseURL
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


