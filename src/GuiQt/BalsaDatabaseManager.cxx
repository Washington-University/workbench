
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
#include "EventManager.h"
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
 * @param username
 *     Name for login.
 * @param password
 *     Password for login.
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

