
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __BALSA_USER_ROLES_DECLARE__
#include "BalsaUserRoles.h"
#undef __BALSA_USER_ROLES_DECLARE__

#include <QJsonArray>

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::BalsaUserRoles 
 * \brief Information about a BALSA User's Roles
 * \ingroup GuiQt
 */

/**
 * Constructor with all roles invalid;
 */
BalsaUserRoles::BalsaUserRoles()
: CaretObject()
{
    resetToAllInvalid();
}

/**
 * Constructor that creates BALSA user roles from a JSON array.
 *
 * @param jsonArray
 *     The JSON array.
 */
BalsaUserRoles::BalsaUserRoles(const QJsonArray& jsonArray)
{
    resetToAllInvalid();
    parseJson(jsonArray);
}

/**
 * Destructor.
 */
BalsaUserRoles::~BalsaUserRoles()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BalsaUserRoles::BalsaUserRoles(const BalsaUserRoles& obj)
: CaretObject(obj)
{
    this->copyHelperBalsaUserRoles(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BalsaUserRoles&
BalsaUserRoles::operator=(const BalsaUserRoles& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBalsaUserRoles(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BalsaUserRoles::copyHelperBalsaUserRoles(const BalsaUserRoles& obj)
{
    m_validFlag     = obj.m_validFlag;
    m_adminFlag     = obj.m_adminFlag;
    m_curatorFlag   = obj.m_curatorFlag;
    m_submitterFlag = obj.m_submitterFlag;
    m_userFlag      = obj.m_userFlag;
}

/**
 * Reset the instance to all invalid roles.
 */
void
BalsaUserRoles::resetToAllInvalid()
{
    m_validFlag     = false;
    m_adminFlag     = false;
    m_curatorFlag   = false;
    m_submitterFlag = false;
    m_userFlag      = false;
}

/**
 * Parse JSON to find roles.
 *
 * @param jsonArray
 *     JSON array with roles.
 * @return
 *     True if data was parsed, else false.
 */
bool
BalsaUserRoles::parseJson(const QJsonArray& jsonArray)
{
    resetToAllInvalid();
    
    for (QJsonArray::const_iterator roleIter = jsonArray.begin();
         roleIter != jsonArray.end();
         roleIter++) {
        QJsonValue value = *roleIter;
        if (value.isString()) {
            const QString roleName = value.toString().trimmed();
            if (roleName == "ROLE_ADMIN") {
                m_adminFlag = true;
            }
            else if (roleName == "ROLE_CURATOR") {
                m_curatorFlag = true;
            }
            else if (roleName == "ROLE_SUBMITTER") {
                m_submitterFlag = true;
            }
            else if (roleName == "ROLE_USER") {
                m_userFlag = true;
            }
            else {
                CaretLogInfo("Unrecognized BALSA role found \""
                             + roleName
                             + "\"");
            }
        }
        
        m_validFlag = true;
    }
    
    /*
     * Some roles imply other roles
     */
    if (m_curatorFlag) {
        m_submitterFlag = true;
    }
    
    return isValid();
}

/**
 * @return True if has admin role
 */
bool
BalsaUserRoles::isAdmin() const
{
    return m_adminFlag;
}

/**
 * @return True if has admin role
 */
bool
BalsaUserRoles::isCurator() const
{
    return m_curatorFlag;
}

/**
 * @return True if has submitter role
 */
bool
BalsaUserRoles::isSubmitter() const
{
    return m_submitterFlag;
}

/**
 * @return True if has user role
 */
bool
BalsaUserRoles::isUser() const
{
    return m_userFlag;
}

/**
 * Is the data valid (has parsed successfully)
 */
bool
BalsaUserRoles::isValid() const
{
    return m_validFlag;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BalsaUserRoles::toString() const
{
    AString msg("BalsaUserRoles: "
                + AString("Data Valid - " + AString(m_validFlag ? "YES: " : "NO: "))
                + AString("Admin - "      + AString(m_adminFlag ? "YES: " : "NO: "))
                + AString("Curator - "    + AString(m_curatorFlag ? "YES: " : "NO: "))
                + AString("Submitter - "  + AString(m_submitterFlag ? "YES: " : "NO: "))
                + AString("User - "       + AString(m_userFlag ? "YES: " : "NO: ")));
    return msg;
}

