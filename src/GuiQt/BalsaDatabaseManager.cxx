
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

#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretJsonObject.h"
#include "CaretLogger.h"
#include "CommandOperationManager.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "FileInformation.h"
#include "HttpCommunicator.h"
#include "HttpCommunicatorResult.h"
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
BalsaDatabaseManager::BalsaDatabaseManager(QObject* parent)
: QObject(parent)
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
    
    /*
     --location \
     --dump-header $headers_file \
     --verbose \
     --cookie-jar cookies.txt \
     --header "Authorization: Basic" \
     --header "Connection: Keep-Alive" \
     --data "j_username=XXbalsaTest&j_password=XX%402password" \
     'https://balsa.wustl.edu/j_spring_security_check'
     */
    
    //    CaretHttpManager::setAuthentication("https://balsa.wustl.edu",
    //                                        m_usernameLineEdit->text().trimmed(),
    //                                        m_passwordLineEdit->text().trimmed());
    
    CaretHttpRequest loginRequest;
    loginRequest.m_method = CaretHttpManager::POST_ARGUMENTS;
    loginRequest.m_url    = "https://balsa.wustl.edu/j_spring_security_check";
//    loginRequest.m_url    = "https://balsa.wustl.edu/j_spring_security_check/login/auth";
//    loginRequest.m_url    = "https://balsa.wustl.edu/login/auth";
    //    loginRequest.m_headers.push_back(std::make_pair("Authorization", "Basic"));
    //    loginRequest.m_headers.push_back(std::make_pair("Connection", "Keep-Alive"));
    loginRequest.m_url = loginURL;
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
    
    errorMessageOut = ("Login has failed.\n"
                       "HTTP Code: " + AString::number(loginResponse.m_responseCode));

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
    
    bool successFlag = false;
    
    const bool useCaretHttpFlag = false;
    if (useCaretHttpFlag) {
        successFlag = uploadFileWithCaretHttpManager(uploadURL,
                                                     fileName,
                                                     httpContentTypeName,
                                                     responseContentOut,
                                                     errorMessageOut);
    }
    else {
        successFlag = uploadFileWithHttpCommunicator(uploadURL,
                                                     fileName,
                                                     httpContentTypeName,
                                                     responseContentOut,
                                                     errorMessageOut);
    }
    
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
    
    if (m_debugFlag) {
        std::cout << "Upload response Code: " << uploadResponse.m_responseCode << std::endl;
    }
    
    for (std::map<AString, AString>::iterator mapIter = uploadResponse.m_headers.begin();
         mapIter != uploadResponse.m_headers.end();
         mapIter++) {
        if (m_debugFlag) {
            std::cout << "   Response Header: " << qPrintable(mapIter->first)
            << " -> " << qPrintable(mapIter->second) << std::endl;
        }
    }
    
    uploadResponse.m_body.push_back('\0');
    responseContentOut.append(&uploadResponse.m_body[0]);
    //QString bodyString(&uploadResponse.m_body[0]);
    CaretLogInfo("Upload file to BALSA reply body: "
                 + responseContentOut);
    
    if (uploadResponse.m_responseCode == 200) {
        return true;
    }
    
    errorMessageOut = ("Upload failed code: "
                       + QString::number(uploadResponse.m_responseCode)
                       + "\n"
                       + responseContentOut);
    
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
BalsaDatabaseManager::uploadFileWithHttpCommunicator(const AString& uploadURL,
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
    
    std::map<AString, AString> requestHeaders;
    requestHeaders.insert(std::make_pair("Content-Type",
                                                  httpContentTypeName));
    requestHeaders.insert(std::make_pair("Cookie",
                                                  getJSessionIdCookie()));
    requestHeaders.insert(std::make_pair("X-File-Name",
                                                  uploadFileName));
    requestHeaders.insert(std::make_pair("X-File-Size",
                                                  fileSizeString));
    
    
    HttpCommunicator* httpComm = HttpCommunicator::newInstancePostFile(NULL,
                                                                       uploadURL,
                                                                       fileName,
                                                                       requestHeaders);
    QObject::connect(httpComm, SIGNAL(progressReport(const HttpCommunicatorProgress&)),
                     this, SLOT(receiveProgress(const HttpCommunicatorProgress&)));
    
    httpComm->runUntilDone();
    
    const HttpCommunicatorResult* result = httpComm->getResult();
    
    const int32_t httpCode = result->getHttpCode();
    responseContentOut = result->getContent();
    
    std::map<AString, AString> responseHeaders = result->getHeaders();
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
    
    CaretJsonObject json(responseContentOut);
    
    if (httpCode != 200) {
        AString msg = json.value("statusText");
        if ( ! msg.isEmpty()) {
            contentErrorMessage.insert(0, msg + "\n");
        }
        
        errorMessageOut = ("Upload failed.  Http Code="
                           + AString::number(httpCode)
                           + "\n"
                           "   " + contentErrorMessage);
        return false;
    }
    
    if ( ! haveJsonResponseContentFlag) {
        errorMessageOut = (contentErrorMessage);
        return false;
    }
    
    return true;
    
//    if (m_debugFlag) {
//        std::cout << "Upload response Code: " << uploadResponse.m_responseCode << std::endl;
//    }
//    
//    for (std::map<AString, AString>::iterator mapIter = uploadResponse.m_headers.begin();
//         mapIter != uploadResponse.m_headers.end();
//         mapIter++) {
//        if (m_debugFlag) {
//            std::cout << "   Response Header: " << qPrintable(mapIter->first)
//            << " -> " << qPrintable(mapIter->second) << std::endl;
//        }
//    }
//    
//    uploadResponse.m_body.push_back('\0');
//    responseContentOut.append(&uploadResponse.m_body[0]);
//    //QString bodyString(&uploadResponse.m_body[0]);
//    CaretLogInfo("Upload file to BALSA reply body: "
//                 + responseContentOut);
//    
//    if (uploadResponse.m_responseCode == 200) {
//        return true;
//    }
//    
//    errorMessageOut = ("Upload failed code: "
//                       + QString::number(uploadResponse.m_responseCode)
//                       + "\n"
//                       + responseContentOut);
    
   // NEED TO CHECK FOR SUCCESS AND GET REPLY CODE AND REPLY CONTENT
    
//    return false;
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
    if ( ! sceneFile->getBaseDirectory().isEmpty()) {
        /* validate ? */
        baseDirectoryName = sceneFile->getBaseDirectory();
    }
//    /*
//     * Create parameters for running zip scene file command.
//     * Need to use strdup() since QString::toAscii() returns
//     * QByteArray instance that will go out of scope.  Use
//     * strdup() for all parameters since "free" is later
//     * used to free the memory allocated by strdup().
//     */
//    std::vector<char*> argvVector;
//    argvVector.push_back(strdup("wb_command_in_wb_view"));
//    argvVector.push_back(strdup(OperationZipSceneFile::getCommandSwitch().toAscii().constData()));
//    argvVector.push_back(strdup(sceneFileName.toAscii().constData()));
//    argvVector.push_back(strdup(extractToDirectoryName.toAscii().constData()));
//    argvVector.push_back(strdup(zipFileName.toAscii().constData()));
//    if ( ! baseDirectoryName.isEmpty()) {
//        argvVector.push_back(strdup("-base-dir"));
//        argvVector.push_back(strdup(baseDirectoryName.toAscii().constData()));
//    }
    
    //    for (uint32_t i = 0; i < argvVector.size(); i++) {
    //        std::cout << "Zip Scene File Param " << i << ": " << argvVector[i] << std::endl;
    //    }
    
    bool successFlag = false;
    try {
        OperationZipSceneFile::createZipFile(sceneFileName,
                                             extractToDirectoryName,
                                             zipFileName,
                                             baseDirectoryName,
                                             OperationZipSceneFile::PROGRESS_GUI_EVENT,
                                             NULL);
//        CommandOperationManager* cmdMgr = CommandOperationManager::getCommandOperationManager();
//        ProgramParameters progParams(argvVector.size(),
//                                     &argvVector[0]);
//        cmdMgr->runCommand(progParams);
        
        successFlag = true;
    }
    catch (const CaretException& e) {
        errorMessageOut = e.whatString();
    }
    
//    /*
//     * Free memory from use of strdup().
//     */
//    for (std::vector<char*>::iterator charIter = argvVector.begin();
//         charIter != argvVector.end();
//         charIter++) {
//        std::free(*charIter);
//    }
    
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
    //QString bodyString(&uploadResponse.m_body[0]);
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

void
BalsaDatabaseManager::receiveProgress(const HttpCommunicatorProgress& progress)
{
    if (m_debugFlag) std::cout << "Progress Received: " << progress.getProgressValue() << " of " << progress.getProgressMaximum() << std::endl;
    emit reportProgress(progress);
    
//    EventProgressUpdate progressUpdate;
//    progressUpdate.setProgress(-1, ("Uploading "
//                                    + QString::number(progress.getProgressValue())
//                                    + " of "
//                                    + QString::number(progress.getProgressMaximum())));
//    EventManager::get()->sendEvent(progressUpdate.getPointer());

}

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
    
    EventProgressUpdate progressUpdate(0,
                                       5,
                                       0,
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
    
    progressUpdate.setProgress(1, "Zipping Scene and Data Files");
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
    
    progressUpdate.setProgress(2, "Uploading zip file");
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
    progressUpdate.setProgress(3, "Zip files has been uploaded");
    EventManager::get()->sendEvent(progressUpdate.getPointer());
    
    const bool doProcessUploadFlag = true;
    if (doProcessUploadFlag) {
        /*
         * Process the uploaded file
         */
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
        
        progressUpdate.setProgress(4, "Zip files has been processed after uploading.");
        EventManager::get()->sendEvent(progressUpdate.getPointer());
    }
    else {
        CaretLogSevere("PROCESSING OF FILE UPLOADED TO BALSA IS DISABLED");
    }

    return true;
}


