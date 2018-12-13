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

#include "WuQMacroClassTypeEnum.h"

class QAbstractButton;
class QAction;
class QListWidgetItem;

namespace caret {

    class WuQMacroManager;

    class WuQMacroSignalWatcher : public QObject {
        
        Q_OBJECT

    public:
        static WuQMacroSignalWatcher* newInstance(WuQMacroManager* parentMacroManager,
                                                  QObject* object,
                                                  const QString& toolTipTextOverride,
                                                  QString& errorMessageOut);
        
        virtual ~WuQMacroSignalWatcher();
        
        WuQMacroSignalWatcher(const WuQMacroSignalWatcher&) = delete;

        WuQMacroSignalWatcher& operator=(const WuQMacroSignalWatcher&) = delete;

        QString toString() const;
        
        QString getToolTip() const;
        
    private slots:
        void actionTriggered(bool);
        
        void actionGroupTriggered(QAction* action);
        
        void buttonGroupButtonClicked(QAbstractButton* button);
        
        void checkBoxClicked(bool);
        
        void comboBoxActivated(int);
        
        void doubleSpinBoxValueChanged(double);
        
        void lineEditTextEdited(const QString&);
        
        void listWidgetItemActivated(QListWidgetItem*);
        
        void menuTriggered(QAction* action);
        
        void pushButtonClicked(bool);
        
        void radioButtonClicked(bool);
        
        void sliderValueChanged(int);
        
        void spinBoxValueChanged(int);
        
        void tabBarCurrentChanged(int);
        
        void tabWidgetCurrentChanged(int);
        
        void toolButtonClicked(bool);

        void objectWasDestroyed(QObject* obj);
        
        void objectNameWasChanged(const QString& name);
        
        // ADD_NEW_METHODS_HERE

    private:
        WuQMacroSignalWatcher(WuQMacroManager* parentMacroManager,
                              QObject* object,
                              const WuQMacroClassTypeEnum::Enum objectType,
                              const QString& toolTipTextOverride);
        
        WuQMacroManager* m_parentMacroManager;
        
        void createAndSendMacroCommand(const QVariant& value,
                                       const QVariant& valueTwo);
        
        QObject* m_object;
        
        const WuQMacroClassTypeEnum::Enum m_objectType;
        
        const QString m_objectName;
        
        QString m_toolTipText;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_SIGNAL_WATCHER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_SIGNAL_WATCHER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_SIGNAL_WATCHER_H__
