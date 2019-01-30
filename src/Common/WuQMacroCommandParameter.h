#ifndef __WU_Q_MACRO_COMMAND_PARAMETER_H__
#define __WU_Q_MACRO_COMMAND_PARAMETER_H__

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


#include <QVariant>
#include <memory>

#include "CaretObjectTracksModification.h"
#include "WuQMacroDataValueTypeEnum.h"


namespace caret {

    class WuQMacroCommandParameter : public CaretObjectTracksModification {
        
    public:
        WuQMacroCommandParameter(const WuQMacroDataValueTypeEnum::Enum dataType,
                                 const QString& name,
                                 const QVariant& value);
        
        WuQMacroCommandParameter();
        
        virtual ~WuQMacroCommandParameter();
        
        WuQMacroCommandParameter(const WuQMacroCommandParameter& obj);

        WuQMacroCommandParameter& operator=(const WuQMacroCommandParameter& obj);

        WuQMacroDataValueTypeEnum::Enum getDataType() const;
        
        QString getName() const;
        
        QVariant getValue() const;
        
        void setValue(const QVariant& value);
        

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperWuQMacroCommandParameter(const WuQMacroCommandParameter& obj);

        WuQMacroDataValueTypeEnum::Enum m_dataType = WuQMacroDataValueTypeEnum::INVALID;
        
        QString m_name;
        
        QVariant m_value;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_COMMAND_PARAMETER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_COMMAND_PARAMETER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COMMAND_PARAMETER_H__
