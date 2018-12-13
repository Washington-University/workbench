#ifndef __WU_Q_MACRO_COMMAND_H__
#define __WU_Q_MACRO_COMMAND_H__

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

#include <QVariant>

#include "CaretObjectTracksModification.h"
#include "WuQMacroDataValueTypeEnum.h"
#include "WuQMacroClassTypeEnum.h"

class QObject;

namespace caret {

    class WuQMacroMouseEventInfo;
    
    class WuQMacroCommand : public CaretObjectTracksModification {
        
    public:
        
        WuQMacroCommand(const WuQMacroClassTypeEnum::Enum objectClassType,
                        const QString& objectClassName,
                        const QVariant dataValue,
                        const QVariant dataValueTwo);
        
        WuQMacroCommand(const QString& objectClassName,
                        WuQMacroMouseEventInfo* mouseEventInfo);
        
        virtual ~WuQMacroCommand();
        
        WuQMacroCommand(const WuQMacroCommand& obj);
        
        WuQMacroCommand& operator=(const WuQMacroCommand& obj);
        
        WuQMacroClassTypeEnum::Enum getClassType() const;
        
        QString getObjectName() const;
        
        WuQMacroDataValueTypeEnum::Enum getDataType() const;
        
        QVariant getDataValue() const;

        WuQMacroDataValueTypeEnum::Enum getDataTypeTwo() const;
        
        QVariant getDataValueTwo() const;
        
        const WuQMacroMouseEventInfo* getMouseEventInfo() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperWuQMacroCommand(const WuQMacroCommand& obj);
                
        WuQMacroClassTypeEnum::Enum m_classType;
        
        QString m_objectName;
        
        QVariant m_dataValue;
        
        WuQMacroDataValueTypeEnum::Enum m_dataType;
        
        QVariant m_dataValueTwo;
        
        WuQMacroDataValueTypeEnum::Enum m_dataTypeTwo;
        
        WuQMacroMouseEventInfo* m_macroMouseEvent;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_COMMAND_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COMMAND_H__
