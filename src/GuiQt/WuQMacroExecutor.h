#ifndef __WU_Q_MACRO_EXECUTOR_H__
#define __WU_Q_MACRO_EXECUTOR_H__

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

#include "WuQMacroExecutorOptions.h"
class QTabBar;
class QWidget;

namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroExecutorOptions;
    
    class WuQMacroExecutor : public QObject {
        
        Q_OBJECT

    public:
        WuQMacroExecutor();
        
        virtual ~WuQMacroExecutor();
        
        WuQMacroExecutor(const WuQMacroExecutor&) = delete;

        WuQMacroExecutor& operator=(const WuQMacroExecutor&) = delete;
        
        bool runMacro(const WuQMacro* macro,
                      QWidget* window,
                      std::vector<QObject*>& otherObjectParents,
                      const WuQMacroExecutorOptions* executorOptions,
                      QString& errorMessageOut) const;
        
        void stopMacro();
        
        // ADD_NEW_METHODS_HERE
        
    private:
        void moveMouseToTabBarTab(QTabBar* tabBar,
                                  const int32_t tabIndex) const;

        void moveMouseToWidget(QObject* moveToObject,
                               const bool highlightFlag = true) const;

        void moveMouseToWidgetXY(QObject* moveToObject,
                                 const int x,
                                 const int y,
                                 const bool highlightFlag = true) const;
        
        void moveMouseToWidgetImplementation(QObject* moveToObject,
                                             const int x,
                                             const int y,
                                             const QRect* objectRect = NULL,
                                             const bool hightlightFlag = false) const;
        
        bool runMacroPrivate(const WuQMacro* macro,
                             QWidget* window,
                             std::vector<QObject*>& otherObjectParents,
                             const WuQMacroExecutorOptions* executorOptions,
                             QString& errorMessageOut) const;

        bool runMacroCommand(QWidget* parentWidget,
                             const WuQMacroCommand* macroCommand,
                             QObject* object,
                             QString& errorMessageOut) const;
        
        QObject* findObjectByName(const QString& objectName) const;
        
        void runActionCommand(const WuQMacroCommand* macroCommand,
                              QObject* object,
                              QString& errorMessageOut,
                              bool& castFailureFlagOut) const;
        
        void runActionCheckableCommand(const WuQMacroCommand* macroCommand,
                                       QObject* object,
                                       QString& errorMessageOut,
                                       bool& castFailureFlagOut) const;

        void runActionGroupCommand(const WuQMacroCommand* macroCommand,
                                   QObject* object,
                                   QString& errorMessageOut,
                                   bool& castFailureFlagOut) const;
        
        void runButtonGroupCommand(const WuQMacroCommand* macroCommand,
                                   QObject* object,
                                   QString& errorMessageOut,
                                   bool& castFailureFlagOut) const;
        
        void runCheckBoxCommand(const WuQMacroCommand* macroCommand,
                                QObject* object,
                                QString& errorMessageOut,
                                bool& castFailureFlagOut) const;
        
        void runComboBoxCommand(const WuQMacroCommand* macroCommand,
                                QObject* object,
                                QString& errorMessageOut,
                                bool& castFailureFlagOut) const;
        
        void runDoubleSpinBoxCommand(const WuQMacroCommand* macroCommand,
                                     QObject* object,
                                     QString& errorMessageOut,
                                     bool& castFailureFlagOut) const;
        
        void runLineEditCommand(const WuQMacroCommand* macroCommand,
                                QObject* object,
                                QString& errorMessageOut,
                                bool& castFailureFlagOut) const;
        
        void runListWidgetCommand(const WuQMacroCommand* macroCommand,
                                  QObject* object,
                                  QString& errorMessageOut,
                                  bool& castFailureFlagOut) const;
        
        void runMenuCommand(const WuQMacroCommand* macroCommand,
                            QObject* object,
                            QString& errorMessageOut,
                            bool& castFailureFlagOut) const;
        
        bool runMouseCommand(const WuQMacroCommand* macroCommand,
                             QObject* object,
                             QString& errorMessageOut,
                             bool& castFailureFlagOut) const;
        
        void runPushButtonCommand(const WuQMacroCommand* macroCommand,
                                  QObject* object,
                                  QString& errorMessageOut,
                                  bool& castFailureFlagOut) const;
        
        void runPushButtonCheckableCommand(const WuQMacroCommand* macroCommand,
                                           QObject* object,
                                           QString& errorMessageOut,
                                           bool& castFailureFlagOut) const;

        void runRadioButtonCommand(const WuQMacroCommand* macroCommand,
                                   QObject* object,
                                   QString& errorMessageOut,
                                   bool& castFailureFlagOut) const;
        
        void runSliderCommand(const WuQMacroCommand* macroCommand,
                              QObject* object,
                              QString& errorMessageOut,
                              bool& castFailureFlagOut) const;
        
        void runSpinBoxCommand(const WuQMacroCommand* macroCommand,
                               QObject* object,
                               QString& errorMessageOut,
                               bool& castFailureFlagOut) const;
        
        void runTabBarCommand(const WuQMacroCommand* macroCommand,
                              QObject* object,
                              QString& errorMessageOut,
                              bool& castFailureFlagOut) const;
        
        void runTabWidgetCommand(const WuQMacroCommand* macroCommand,
                                 QObject* object,
                                 QString& errorMessageOut,
                                 bool& castFailureFlagOut) const;
        
        void runToolButtonCommand(const WuQMacroCommand* macroCommand,
                                  QObject* object,
                                  QString& errorMessageOut,
                                  bool& castFailureFlagOut) const;
        
        void runToolButtonCheckableCommand(const WuQMacroCommand* macroCommand,
                                           QObject* object,
                                           QString& errorMessageOut,
                                           bool& castFailureFlagOut) const;

        mutable WuQMacroExecutorOptions m_runOptions;
        
        mutable std::vector<QObject*> m_parentObjects;
        
        bool m_stopFlag = false;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __WU_Q_MACRO_EXECUTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_EXECUTOR_DECLARE__
    
} // namespace
#endif  //__WU_Q_MACRO_EXECUTOR_H__
