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

#include <QStandardItemModel>

#include "CaretObjectTracksModification.h"
#include "TracksModificationInterface.h"
#include "WuQMacroShortCutKeyEnum.h"

namespace caret {

    class WuQMacro;
    
    class WuQMacroGroup : public QStandardItemModel, public TracksModificationInterface {
        
    public:
        WuQMacroGroup(const QString& groupName);
        
        virtual ~WuQMacroGroup();
        
        QString getName() const;

        void setName(const QString& name);
        
        QString getUniqueIdentifier() const;
        
        void setUniqueIdentifier(const QString& uniqueIdentifier);
        
        void addMacro(WuQMacro* macro);
        
        void insertMacroAtIndex(const int32_t index,
                                WuQMacro* macro);
        
        void appendMacroGroup(const WuQMacroGroup* macroGroup);
        
        int32_t getNumberOfMacros() const;
        
        WuQMacro* getMacroByName(const QString& name);
        
        const WuQMacro* getMacroByName(const QString& name) const;
        
        WuQMacro* getMacroAtIndex(const int32_t index);
        
        const WuQMacro* getMacroAtIndex(const int32_t index) const;
        
        int32_t getIndexOfMacro(const WuQMacro* macro);
        
        WuQMacro* getMacroWithUniqueIdentifier(const QString& uniqueIdentifier);
        
        const WuQMacro* getMacroWithUniqueIdentifier(const QString& uniqueIdentifier) const;

        WuQMacro* getMacroWithShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey);
        
        const WuQMacro* getMacroWithShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey) const;

        bool containsMacro(const WuQMacro* macro);
        
        void deleteMacro(const WuQMacro* macro);
        
        void deleteMacroAtIndex(const int32_t index);
        
        void moveMacroDown(WuQMacro* macro);
        
        void moveMacroUp(WuQMacro* macro);
        
        virtual bool isModified() const override;
        
        virtual void clearModified() override;
        
        virtual void setModified() override;
        
        bool readXmlFromStringOld(const QString& xmlString,
                               QString& errorMessageOut,
                               QString& nonFatalWarningMessageOut);
        
        bool writeXmlToString(QString& xmlString,
                              QString& errorMessageOut);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void clearPrivate();
        
        QString m_uniqueIdentifier;
        
        QString m_name;
        
        bool m_modifiedStatusFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_GROUP_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_GROUP_H__
