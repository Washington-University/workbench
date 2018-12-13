#ifndef __WU_Q_MACRO_MANAGER_H__
#define __WU_Q_MACRO_MANAGER_H__

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

#include <QObject>

#include "WuQMacroExecutor.h"
#include "WuQMacroModeEnum.h"

class QMouseEvent;
class QWidget;

namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroDialog;
    class WuQMacroGroup;
    class WuQMacroSignalWatcher;
    
    class WuQMacroManager : public QObject {
        Q_OBJECT
        
    public:
        static WuQMacroManager* instance();
        
        virtual ~WuQMacroManager();
        
        WuQMacroManager(const WuQMacroManager&) = delete;

        WuQMacroManager& operator=(const WuQMacroManager&) = delete;
        
        QString getName() const;

        bool addMacroSupportToObject(QObject* object);
        
        bool addMacroSupportToObjectWithToolTip(QObject* object,
                                                const QString& toolTipOverride);
        
        bool addMacroCommandToRecording(WuQMacroCommand* macroCommand);
        
        bool addMouseEventToRecording(QWidget* widget,
                                      const QMouseEvent* me);
        
        WuQMacroModeEnum::Enum getMode() const;
        
        void setMode(const WuQMacroModeEnum::Enum mode);
        
        bool isModeRecording() const;
        
        std::vector<WuQMacroGroup*> getMacroGroups() const;
        
        void startRecordingNewMacro(QWidget* parent);
        
        void stopRecordingNewMacro();
        
        void showMacrosDialog(QWidget* parent);
        
        std::vector<QWidget*> getNonModalDialogs();
        
        void updateNonModalDialogs();
        
        bool editMacroAttributes(QWidget* parent,
                                 WuQMacro* macro);
        
        bool deleteMacro(QWidget* parent,
                         WuQMacroGroup* macroGroup,
                         WuQMacro* macro);
        
        bool editMacroCommands(QWidget* parent,
                               WuQMacro* macro);
        
        bool importMacros(QWidget* parent,
                          WuQMacroGroup* appendToMacroGroup);
        
        bool exportMacros(QWidget* parent,
                          WuQMacroGroup* macroGroup,
                          WuQMacro* macro);
        
        void runMacro(QWidget* window,
                      const WuQMacroExecutor::RunOptions& runOptions,
                      const WuQMacro* macro);
        
        void printSupportedWidgetsToTerminal();
        
        void addParentObject(QObject* parentObject);
        
        QString getToolTipForObjectName(const QString& objectName) const;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        WuQMacroManager(const QString& name,
                        QObject* parent = NULL);
        
        std::vector<QObject*> m_parentObjects;
        
        const QString m_name;
        
        WuQMacroModeEnum::Enum m_mode = WuQMacroModeEnum::OFF;
        
        std::map<QString, WuQMacroSignalWatcher*> m_signalWatchers;
        
        WuQMacro* m_macroBeingRecorded = NULL;
        
        WuQMacroDialog* m_macrosDialog = NULL;
        
        static WuQMacroManager* s_singletonMacroManager;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_MANAGER_DECLARE__
    WuQMacroManager* WuQMacroManager::s_singletonMacroManager = NULL;
#endif // __WU_Q_MACRO_MANAGER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_MANAGER_H__
