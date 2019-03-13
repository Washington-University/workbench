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

#include <QMutex>
#include <QObject>

#include "WuQMacroExecutor.h"
#include "WuQMacroModeEnum.h"
#include "WuQMacroShortCutKeyEnum.h"

class QKeyEvent;
class QMainWindow;
class QMouseEvent;
class QWidget;

namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroCommandParameter;
    class WuQMacroCustomOperationManagerInterface;
    class WuQMacroDialog;
    class WuQMacroExecutor;
    class WuQMacroExecutorMonitor;
    class WuQMacroExecutorOptions;
    class WuQMacroGroup;
    class WuQMacroHelperInterface;
    class WuQMacroSignalWatcher;
    
    class WuQMacroManager : public QObject {
        Q_OBJECT
        
    public:
        static WuQMacroManager* instance();
        
        virtual ~WuQMacroManager();
        
        WuQMacroManager(const WuQMacroManager&) = delete;

        WuQMacroManager& operator=(const WuQMacroManager&) = delete;
        
        void setMacroHelper(WuQMacroHelperInterface* macroHelper);
        
        void setCustomCommandManager(WuQMacroCustomOperationManagerInterface* customCommandManager);
        
        QString getName() const;

        bool addMacroSupportToObject(QObject* object,
                                     const QString& descriptiveName);
        
        bool addMacroSupportToObjectWithToolTip(QObject* object,
                                                const QString& descriptiveName,
                                                const QString& toolTipOverride);
        
        bool addMacroCommandToRecording(WuQMacroCommand* macroCommand);
        
        bool addMouseEventToRecording(QWidget* widget,
                                      const QString& descriptiveName,
                                      const QMouseEvent* me);
        
        WuQMacroModeEnum::Enum getMode() const;
        
        void setMode(const WuQMacroModeEnum::Enum mode);
        
        bool isModeRecording() const;
        
        std::vector<WuQMacroGroup*> getActiveMacroGroups() const;
        
        std::vector<const WuQMacroGroup*> getAllMacroGroups() const;        

        void startRecordingNewMacro(QWidget* parent);
        
        void stopRecordingNewMacro();
        
        void showMacrosDialog(QWidget* parent);
        
        std::vector<QWidget*> getNonModalDialogs();
        
        WuQMacroExecutorOptions* getExecutorOptions();
        
        const WuQMacroExecutorOptions* getExecutorOptions() const;
        
        const WuQMacroExecutorMonitor* getMacroExecutorMonitor() const;

        bool deleteMacro(QWidget* parent,
                         WuQMacroGroup* macroGroup,
                         WuQMacro* macro);
        
        bool deleteMacroCommand(QWidget* parent,
                                WuQMacroGroup* macroGroup,
                                WuQMacro* macro,
                                WuQMacroCommand* macroCommand);
        
        bool importMacros(QWidget* parent,
                          WuQMacroGroup* appendToMacroGroup);
        
        bool exportMacros(QWidget* parent,
                          WuQMacroGroup* macroGroup,
                          WuQMacro* macro);
        
        void runMacro(QWidget* window,
                      const WuQMacro* macro,
                      const WuQMacroCommand* macroCommandToStopAfter);
        
        void stopMacro();
        
        void pauseContinueMacro();
        
        bool runMacroWithShortCutKeyEvent(QWidget* window,
                                          const QKeyEvent* keyEvent);
        
        void printSupportedWidgetsToTerminal();
        
        void printToLevelWidgetsToTerminal();
        
        void addParentObject(QObject* parentObject);
        
        QString getToolTipForObjectName(const QString& objectName) const;
        
        WuQMacro* getMacroWithShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey) const;
        
        static QString getShortCutKeysMask();
        
        void macroWasModified(WuQMacro* macro);
        
        bool editCustomDataValueParameter(QWidget* parent,
                                          WuQMacroCommand* macroCommand,
                                          WuQMacroCommandParameter* parameter);

        bool executeCustomOperationMacroCommand(QWidget* parent,
                                                const WuQMacroExecutorMonitor* executorMonitor,
                                                const WuQMacroExecutorOptions* executorOptions,
                                                const WuQMacroCommand* macroCommand,
                                                QString& errorMessageOut);
        
        virtual std::vector<WuQMacroCommand*> getAllCustomOperationMacroCommands();

        virtual std::vector<QString> getNamesOfCustomOperationMacroCommands();
        
        virtual WuQMacroCommand* newInstanceOfCustomOperationMacroCommand(const QString& macroCommandName,
                                                                          QString& errorMessageOut);
        
        std::vector<QString> getMainWindowIdentifiers();
        
        QMainWindow* getMainWindowWithIdentifier(const QString& identifier);

        std::vector<WuQMacroSignalWatcher*> getAllWidgetSignalWatchers(const bool enabledItemsOnly) const;
        
        std::vector<QString> getAllWidgetSignalWatcherNames(const bool enabledItemsOnly) const;
        
        WuQMacroSignalWatcher* getWidgetSignalWatcherWithName(const QString& name);
        
        // ADD_NEW_METHODS_HERE
        
    public slots:
        void updateNonModalDialogs();
        
        void macroCommandCompletedExecution(QWidget* window,
                                            const WuQMacroCommand* command,
                                            const WuQMacroExecutorOptions* executorOptions,
                                            bool& allowDelayFlagOut);
        
        void macroCommandStartingExecution(QWidget* window,
                                           const WuQMacroCommand* command,
                                           const WuQMacroExecutorOptions* executorOptions,
                                           bool& allowDelayFlagOut);
        
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
        
        std::unique_ptr<WuQMacroExecutorOptions> m_executorOptions;
        
        WuQMacroHelperInterface* m_macroHelper = NULL;
        
        WuQMacroCustomOperationManagerInterface* m_customCommandManager = NULL;
        
        WuQMacroExecutor* m_macroExecutor = NULL;
        
        WuQMacroExecutorMonitor* m_macroExecutorMonitor = NULL;
        
        QMutex m_macroExecutorMutex;
        
        static QString s_importExportMacroFileDirectory;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_MANAGER_DECLARE__
    WuQMacroManager* WuQMacroManager::s_singletonMacroManager = NULL;
    QString WuQMacroManager::s_importExportMacroFileDirectory;
#endif // __WU_Q_MACRO_MANAGER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_MANAGER_H__
