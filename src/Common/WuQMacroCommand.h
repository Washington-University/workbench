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

#include <QStandardItem>
#include <QVariant>

#include "TracksModificationInterface.h"
#include "WuQMacroDataValueTypeEnum.h"
#include "WuQMacroClassTypeEnum.h"

class QObject;

namespace caret {

    class WuQMacroMouseEventInfo;
    
    class WuQMacroCommand : public QStandardItem, public TracksModificationInterface {
        
    public:
        
        WuQMacroCommand(const WuQMacroClassTypeEnum::Enum objectClassType,
                        const QString& objectName,
                        const QString& objectToolTip,
                        const QVariant dataValue,
                        const QVariant dataValueTwo);
        
        WuQMacroCommand(const WuQMacroClassTypeEnum::Enum objectClassType,
                        const QString& objectName);
        
        WuQMacroCommand(const QString& objectName,
                        WuQMacroMouseEventInfo* mouseEventInfo);
        
        virtual ~WuQMacroCommand();
        
        WuQMacroCommand(const WuQMacroCommand& obj);
        
        WuQMacroCommand& operator=(const WuQMacroCommand& obj);
        
        WuQMacroClassTypeEnum::Enum getClassType() const;
        
        QString getObjectName() const;
        
        QString getObjectToolTip() const;
        
        void setObjectToolTip(const QString& objectToolTip);
        
        WuQMacroDataValueTypeEnum::Enum getDataType() const;
        
        QVariant getDataValue() const;

        void setDataValue(const QVariant& dataValue);
        
        WuQMacroDataValueTypeEnum::Enum getDataTypeTwo() const;
        
        QVariant getDataValueTwo() const;
        
        void setDataValueTwo(const QVariant& dataValue);
        
        bool isDataValueTwoValid() const;
        
        WuQMacroMouseEventInfo* getMouseEventInfo();
        
        const WuQMacroMouseEventInfo* getMouseEventInfo() const;
        
        void setMouseEventInfo(WuQMacroMouseEventInfo* mouseEventInfo);
        
        bool isMouseEventMatch(const WuQMacroCommand* command) const;
        
        // ADD_NEW_METHODS_HERE

        virtual bool isModified() const override;
        
        virtual void clearModified() override;
        
        virtual void setModified() override;
        
        virtual AString toString() const;
        
        virtual int type() const override;
        
    private:
        void copyHelperWuQMacroCommand(const WuQMacroCommand& obj);
                
        WuQMacroClassTypeEnum::Enum m_classType;
        
        QString m_objectName;
        
        QVariant m_dataValue;
        
        WuQMacroDataValueTypeEnum::Enum m_dataType;
        
        QVariant m_dataValueTwo;
        
        WuQMacroDataValueTypeEnum::Enum m_dataTypeTwo;
        
        WuQMacroMouseEventInfo* m_macroMouseEvent;
        
        bool m_modifiedStatusFlag = false;
        
        bool m_dataValueTwoValidFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_COMMAND_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COMMAND_H__
