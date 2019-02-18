#ifndef __WU_Q_MACRO_SIGNAL_WATCHER_H__
#define __WU_Q_MACRO_SIGNAL_WATCHER_H__

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

#include "WuQMacroWidgetTypeEnum.h"

class QAbstractButton;
class QAction;
class QListWidgetItem;

namespace caret {

    class WuQMacroCommand;
    class WuQMacroCommandParameter;
    class WuQMacroManager;

    class WuQMacroSignalWatcher : public QObject {
        
        Q_OBJECT

    public:
        static WuQMacroSignalWatcher* newInstance(WuQMacroManager* parentMacroManager,
                                                  QObject* object,
                                                  const QString& descriptiveName,
                                                  const QString& toolTipTextOverride,
                                                  QString& errorMessageOut);
        
        virtual ~WuQMacroSignalWatcher();
        
        WuQMacroSignalWatcher(const WuQMacroSignalWatcher&) = delete;

        WuQMacroSignalWatcher& operator=(const WuQMacroSignalWatcher&) = delete;

        WuQMacroCommand* createMacroCommandWithDefaultParameters(QString& errorMessageOut) const;
        
        QString getObjectName() const;
        
        QString toString() const;
        
        QString getToolTip() const;
        
    private slots:
        void actionTriggered(bool);
        
        void actionCheckableTriggered(bool);
        
        void actionGroupTriggered(QAction* action);
        
        void buttonGroupButtonClicked(QAbstractButton* button);
        
        void checkBoxClicked(bool);
        
        void comboBoxActivated(int);
        
        void doubleSpinBoxValueChanged(double);
        
        void lineEditTextEdited(const QString&);
        
        void listWidgetItemActivated(QListWidgetItem*);
        
        void menuTriggered(QAction* action);
        
        void pushButtonClicked(bool);
        
        void pushButtonCheckableClicked(bool);
        
        void radioButtonClicked(bool);
        
        void sliderValueChanged(int);
        
        void spinBoxValueChanged(int);
        
        void tabBarCurrentChanged(int);
        
        void tabWidgetCurrentChanged(int);
        
        void toolButtonClicked(bool);

        void toolButtonCheckableClicked(bool);
        
        void objectWasDestroyed(QObject* obj);
        
        void objectNameWasChanged(const QString& name);
        
        // ADD_NEW_METHODS_HERE

    private:
        WuQMacroSignalWatcher(WuQMacroManager* parentMacroManager,
                              QObject* object,
                              const WuQMacroWidgetTypeEnum::Enum objectType,
                              const QString& descriptiveName,
                              const QString& toolTipTextOverride);
        
        WuQMacroManager* m_parentMacroManager;
        
        void createAndSendMacroCommand(std::vector<WuQMacroCommandParameter*>& parameters);
        
        std::vector<WuQMacroCommandParameter*> getCopyOfObjectParameters() const;
        
        QObject* m_object;
        
        const WuQMacroWidgetTypeEnum::Enum m_objectType;
        
        const QString m_descriptiveName;
        
        const QString m_objectName;
        
        QString m_toolTipText;
        
        std::vector<WuQMacroCommandParameter*> m_objectParameters;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_SIGNAL_WATCHER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_SIGNAL_WATCHER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_SIGNAL_WATCHER_H__
