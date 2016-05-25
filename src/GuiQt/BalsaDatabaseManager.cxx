
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
#include "CommandOperationManager.h"
#include "EventManager.h"
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
BalsaDatabaseManager::login(const AString& username,
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
    loginRequest.m_method = CaretHttpManager::POST;
    loginRequest.m_url    = "https://balsa.wustl.edu/j_spring_security_check";
//    loginRequest.m_url    = "https://balsa.wustl.edu/j_spring_security_check/login/auth";
//    loginRequest.m_url    = "https://balsa.wustl.edu/login/auth";
    //    loginRequest.m_headers.push_back(std::make_pair("Authorization", "Basic"));
    //    loginRequest.m_headers.push_back(std::make_pair("Connection", "Keep-Alive"));
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
        return true;
    }
    
    errorMessageOut = ("Login appears to have failed.\n"
                       "HTTP Code: " + AString::number(loginResponse.m_responseCode));

    return false;
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
    /*
     * Create parameters for running zip scene file command.
     * Need to use strdup() since QString::toAscii() returns
     * QByteArray instance that will go out of scope.  Use
     * strdup() for all parameters since "free" is later
     * used to free the memory allocated by strdup().
     */
    std::vector<char*> argvVector;
    argvVector.push_back(strdup("wb_command_in_wb_view"));
    argvVector.push_back(strdup(OperationZipSceneFile::getCommandSwitch().toAscii().constData()));
    argvVector.push_back(strdup(sceneFileName.toAscii().constData()));
    argvVector.push_back(strdup(extractToDirectoryName.toAscii().constData()));
    argvVector.push_back(strdup(zipFileName.toAscii().constData()));
    if ( ! baseDirectoryName.isEmpty()) {
        argvVector.push_back(strdup("-base-dir"));
        argvVector.push_back(strdup(baseDirectoryName.toAscii().constData()));
    }
    
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
    
    /*
     * Free memory from use of strdup().
     */
    for (std::vector<char*>::iterator charIter = argvVector.begin();
         charIter != argvVector.end();
         charIter++) {
        std::free(*charIter);
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

