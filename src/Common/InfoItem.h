#ifndef __INFO_ITEM_H__
#define __INFO_ITEM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#include "CaretObject.h"

namespace caret {

    class InfoItem : public CaretObject {
        
    public:
        static std::unique_ptr<InfoItem> makeItem(const AString& name,
                                                  const AString& value,
                                                  const AString& tooltip = "");
        
        InfoItem(const AString& name,
                 const AString& value,
                 const AString& tooltip = "");
        
        virtual ~InfoItem();
        
        InfoItem(const InfoItem& obj);

        InfoItem& operator=(const InfoItem& obj);
        
        AString getName() const;
        
        AString getValue() const;
        
        AString getToolTip() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperInfoItem(const InfoItem& obj);

        AString m_name;
        
        AString m_value;
        
        AString m_tooltip;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __INFO_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __INFO_ITEM_DECLARE__

} // namespace
#endif  //__INFO_ITEM_H__
