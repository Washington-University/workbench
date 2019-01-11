#ifndef __WU_Q_MACRO_H__
#define __WU_Q_MACRO_H__

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

#include "TracksModificationInterface.h"
#include "WuQMacroShortCutKeyEnum.h"

namespace caret {

    class WuQMacroCommand;
    
    class WuQMacro : public QStandardItem, public TracksModificationInterface {
        
    public:
        WuQMacro();
        
        virtual ~WuQMacro();
        
        WuQMacro(const WuQMacro& obj);
        
        WuQMacro& operator=(const WuQMacro& obj);
        
        void appendMacroCommand(WuQMacroCommand* macroCommand);
        
        void insertMacroCommandAtIndex(const int32_t index,
                                       WuQMacroCommand* macroCommand);
        
        int32_t getNumberOfMacroCommands() const;
        
        const WuQMacroCommand* getMacroCommandAtIndex(const int32_t index) const;
        
        WuQMacroCommand* getMacroCommandAtIndex(const int32_t index);
        
        int32_t getIndexOfMacroCommand(const WuQMacroCommand* macroCommand) const;
        
        void deleteMacroCommandAtIndex(const int32_t index);
        
        void deleteMacroCommand(WuQMacroCommand* macroCommand);
        
        void moveMacroCommandDown(WuQMacroCommand* macroCommand);
        
        void moveMacroCommandUp(WuQMacroCommand* macroCommand);
        
        QString getUniqueIdentifier() const;
        
        void setUniqueIdentifier(const QString& uniqueIdentifier);
        
        QString getName() const;
        
        void setName(const QString& name);
        
        QString getDescription() const;
        
        void setDescription(const QString& description);
        
        WuQMacroShortCutKeyEnum::Enum getShortCutKey() const;
        
        void setShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey);
        
        virtual AString toString() const;

        virtual bool isModified() const override;
        
        virtual void clearModified() override;
        
        virtual void setModified() override;
        
        virtual int type() const override;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void copyHelperWuQMacro(const WuQMacro& obj);
        
        void clearCommands();
        
        QString m_uniqueIdentifier;
        
        QString m_description;
        
        WuQMacroShortCutKeyEnum::Enum m_shortCutKey = WuQMacroShortCutKeyEnum::Key_None;
        
        bool m_modifiedStatusFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_H__
