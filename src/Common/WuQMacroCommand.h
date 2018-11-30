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
#include "WuQMacroObjectTypeEnum.h"

class QObject;

namespace caret {

    class WuQMacroCommand : public CaretObjectTracksModification {
        
    public:
        
        WuQMacroCommand(const WuQMacroObjectTypeEnum::Enum objectType,
                        const QString objectName,
                        const QString objectToolTip,
                        const QVariant value);
        
        virtual ~WuQMacroCommand();
        
        WuQMacroCommand(const WuQMacroCommand&) = delete;

        WuQMacroCommand& operator=(const WuQMacroCommand&) = delete;
        
        WuQMacroObjectTypeEnum::Enum getObjectType() const;
        
        QString getObjectName() const;
        
        QString getObjectToolTip() const;
        
        QVariant getObjectValue() const;

//        bool runMacro(QObject* object,
//                      QString& errorMessageOut) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        
        const WuQMacroObjectTypeEnum::Enum m_objectType;
        
        const QString m_objectName;
        
        const QString m_objectToolTip;
        
        const QVariant m_value;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_COMMAND_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COMMAND_H__
