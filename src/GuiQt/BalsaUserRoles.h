#ifndef __BALSA_USER_ROLES_H__
#define __BALSA_USER_ROLES_H__

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


#include "CaretObject.h"

class QJsonArray;

namespace caret {

    class BalsaUserRoles : public CaretObject {
        
    public:
        BalsaUserRoles(const QJsonArray& jsonArray);
        
        BalsaUserRoles();
        
        virtual ~BalsaUserRoles();
        
        BalsaUserRoles(const BalsaUserRoles& obj);

        BalsaUserRoles& operator=(const BalsaUserRoles& obj);
        
        void resetToAllInvalid();
        
        bool parseJson(const QJsonArray& jsonArray);
        
        bool isEmpty() const;
        
        bool isAdmin() const;
        
        bool isCurator() const;
        
        bool isSubmitter() const;
        
        bool isUser() const;
        
        bool isValid() const;
        
        // ADD_NEW_METHODS_HERE

        AString getRolesForDisplayInGUI() const;
        
        virtual AString toString() const;
        
    private:
        void copyHelperBalsaUserRoles(const BalsaUserRoles& obj);

        bool m_adminFlag = false;
        
        bool m_curatorFlag = false;
        
        bool m_submitterFlag = false;
        
        bool m_userFlag = false;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_USER_ROLES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_USER_ROLES_DECLARE__

} // namespace
#endif  //__BALSA_USER_ROLES_H__
