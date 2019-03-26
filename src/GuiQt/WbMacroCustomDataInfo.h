#ifndef __WB_MACRO_CUSTOM_DATA_INFO_H__
#define __WB_MACRO_CUSTOM_DATA_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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


#include <array>
#include <memory>

#include "CaretObject.h"
#include "WuQMacroDataValueTypeEnum.h"


namespace caret {

    class WbMacroCustomDataInfo : public CaretObject {
        
    public:
        WbMacroCustomDataInfo(const WuQMacroDataValueTypeEnum::Enum dataType);
        
        virtual ~WbMacroCustomDataInfo();
        
        WbMacroCustomDataInfo(const WbMacroCustomDataInfo& obj);

        WbMacroCustomDataInfo& operator=(const WbMacroCustomDataInfo& obj);

        WuQMacroDataValueTypeEnum::Enum getDataType() const;
        
        std::array<float, 2> getFloatRange() const;
        
        void setFloatRange(const std::array<float, 2>& range);
        
        std::array<int32_t, 2> getIntegerRange() const;

        void setIntegerRange(const std::array<int32_t, 2>& range);
        
        std::vector<QString> getStringListValues() const;
        
        void setStringListValues(const std::vector<QString>& values);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void copyHelperWbMacroCustomDataInfo(const WbMacroCustomDataInfo& obj);

        WuQMacroDataValueTypeEnum::Enum m_dataType;
        
        std::array<float, 2> m_floatRange;
        
        std::array<int32_t, 2> m_integerRange;
        
        std::vector<QString> m_stringListValues;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WB_MACRO_CUSTOM_DATA_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_CUSTOM_DATA_INFO_DECLARE__

} // namespace
#endif  //__WB_MACRO_CUSTOM_DATA_INFO_H__
