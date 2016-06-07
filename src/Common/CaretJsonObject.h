#ifndef __CARET_JSON_OBJECT_H__
#define __CARET_JSON_OBJECT_H__

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

#include <map>

#include "CaretObject.h"



namespace caret {

    class CaretJsonObject : public CaretObject {
        
    public:
        CaretJsonObject(const AString& jsonText);
        
        virtual ~CaretJsonObject();

        int32_t count() const;
        
        bool empty() const;

        std::vector<AString> keys() const;
        
        bool hasKey(const AString& key) const;
        
        AString value(const AString& key) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        CaretJsonObject(const CaretJsonObject&);

        CaretJsonObject& operator=(const CaretJsonObject&);
        
        void processNameValueString(const AString& nameValueString);
        
        void trimLeadingTrailingDoubleQuotes(AString& text);
        
        std::map<AString, AString> m_data;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_JSON_OBJECT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_JSON_OBJECT_DECLARE__

} // namespace
#endif  //__CARET_JSON_OBJECT_H__
