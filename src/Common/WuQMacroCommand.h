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
#include "WuQMacroCommandTypeEnum.h"
#include "WuQMacroDataValueTypeEnum.h"
#include "WuQMacroWidgetTypeEnum.h"

class QObject;

namespace caret {

    class WuQMacroCommandParameter;
    class WuQMacroMouseEventInfo;
    
    class WuQMacroCommand : public QStandardItem, public TracksModificationInterface {
        
    public:
        
        static WuQMacroCommand* newInstanceCustomCommand(const QString& customOperationName,
                                                         const int32_t version,
                                                         const QString& objectName,
                                                         const QString& objectDescriptiveName,
                                                         const QString& objectToolTip,
                                                         const float delay,
                                                         QString& errorMessageOut);
        
        static WuQMacroCommand* newInstanceMouseCommand(WuQMacroMouseEventInfo* mouseEventInfo,
                                                        const int32_t version,
                                                        const QString& objectName,
                                                        const QString& objectDescriptiveName,
                                                        const QString& objectToolTip,
                                                        const float delay,
                                                        QString& errorMessageOut);
        
        static WuQMacroCommand* newInstanceWidgetCommand(const WuQMacroWidgetTypeEnum::Enum widgetType,
                                                         const int32_t version,
                                                         const QString& objectName,
                                                         const QString& objectDescriptiveName,
                                                         const QString& objectToolTip,
                                                         const float delay,
                                                         QString& errorMessageOut);
        
        WuQMacroCommand(const WuQMacroCommandTypeEnum::Enum commandType,
                        const QString& customOperationTypeName,
                        WuQMacroMouseEventInfo* mouseEventInfo,
                        const WuQMacroWidgetTypeEnum::Enum widgetType,
                        const int32_t version,
                        const QString& objectName,
                        const QString& objectDescriptiveName,
                        const QString& objectToolTip,
                        const float delayValue);
        
        virtual ~WuQMacroCommand();
        
        WuQMacroCommand(const WuQMacroCommand& obj);
        
        WuQMacroCommand& operator=(const WuQMacroCommand& obj);
        
        WuQMacroCommandTypeEnum::Enum getCommandType() const;
        
        WuQMacroWidgetTypeEnum::Enum getWidgetType() const;
        
        int32_t getVersion() const;
        
        QString getObjectName() const;
        
        QString getDescriptiveName() const;
        
        QString getObjectToolTip() const;
        
        void setObjectToolTip(const QString& objectToolTip);
        
        void addParameter(WuQMacroCommandParameter* parameter);
        
        void addParameter(const WuQMacroDataValueTypeEnum::Enum dataType,
                          const QString& name,
                          const QVariant& value);
        
        int32_t getNumberOfParameters() const;
        
        WuQMacroCommandParameter* getParameterAtIndex(const int32_t);
        
        const WuQMacroCommandParameter* getParameterAtIndex(const int32_t) const;
        
        int32_t getIndexOfParameter(const WuQMacroCommandParameter* parameter) const;
        
        WuQMacroMouseEventInfo* getMouseEventInfo();
        
        const WuQMacroMouseEventInfo* getMouseEventInfo() const;
        
        void setMouseEventInfo(WuQMacroMouseEventInfo* mouseEventInfo);
        
        bool isMouseEventMatch(const WuQMacroCommand* command) const;
        
        float getDelayInSeconds() const;
        
        void setDelayInSeconds(const float seconds);
        
        QString getCustomOperationTypeName() const;
        
        void setCustomOperationTypeName(const QString& customOperationTypeName);
        
        // ADD_NEW_METHODS_HERE

        virtual bool isModified() const override;
        
        virtual void clearModified() override;
        
        virtual void setModified() override;
        
        virtual AString toString() const;
        
        virtual int type() const override;
        
    private:
        void copyHelperWuQMacroCommand(const WuQMacroCommand& obj);
        
        void updateTitle();
        
        void removeAllParameters();
        
        WuQMacroCommandTypeEnum::Enum m_commandType;
        
        QString m_customOperationTypeName;
        
        WuQMacroMouseEventInfo* m_macroMouseEvent;
        
        WuQMacroWidgetTypeEnum::Enum m_widgetType;
        
        int32_t m_version;
        
        QString m_objectName;
        
        QString m_descriptiveName;
        
        float m_delayInSeconds = 1.0f;
        
        std::vector<WuQMacroCommandParameter*> m_parameters;
        
        bool m_modifiedStatusFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_COMMAND_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COMMAND_H__
