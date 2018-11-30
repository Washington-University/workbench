#ifndef __WU_Q_MACRO_GROUP_H__
#define __WU_Q_MACRO_GROUP_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include "CaretObjectTracksModification.h"



namespace caret {

    class WuQMacro;
    
    class WuQMacroGroup : public CaretObjectTracksModification {
        
    public:
        WuQMacroGroup(const QString& groupName);
        
        virtual ~WuQMacroGroup();
        
        WuQMacroGroup(const WuQMacroGroup&) = delete;

        WuQMacroGroup& operator=(const WuQMacroGroup&) = delete;

        QString getGroupName() const;

        void setGroupName(const QString& groupName);
        
        void addMacro(WuQMacro* macro);
        
        int32_t getNumberOfMacros() const;
        
        WuQMacro* getMacroByName(const QString& name);
        
        const WuQMacro* getMacroByName(const QString& name) const;
        
        WuQMacro* getMacroAtIndex(const int32_t index);
        
        const WuQMacro* getMacroAtIndex(const int32_t index) const;
        
        void deleteMacro(const WuQMacro* macro);
        
        void deleteMacroAtIndex(const int32_t index);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        QString m_groupName;
        
        std::vector<WuQMacro*> m_macros;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_GROUP_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_GROUP_H__
