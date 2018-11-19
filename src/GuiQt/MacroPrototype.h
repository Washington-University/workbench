#ifndef __WU_Q_OBJECT_H__
#define __WU_Q_OBJECT_H__

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


#include <map>
#include <memory>

#include <QMenu>
#include <QMouseEvent>
#include <QObject>

#include "AString.h"

class QDoubleSpinBox;
class QComboBox;
class QMainWindow;
class QSpinBox;

namespace caret {

    class WidgetWatcher;
    
    enum WidgetWatcherType {
        INVALID,
        ACTION,
        CHECK_BOX,
        COMBO_BOX,
        DOUBLE_SPIN_BOX,
        MOUSE_EVENT,
        SPIN_BOX,
        TOOL_BUTTON,
    };
    
    /* ========================================================================= */

    class WuQObject : public QObject {
        
        Q_OBJECT

    public:
        WuQObject(const WuQObject&) = delete;

        WuQObject& operator=(const WuQObject&) = delete;

        static void watchObjectForMacroRecording(QObject* sender);
        
        static QMetaObject::Connection connect(QObject *sender,
                                               const char *signal,
                                               const QObject *receiver,
                                               const char *method,
                                               Qt::ConnectionType type = Qt::AutoConnection);
        
//        typedef void (QObject::*QObjectBoolPointerToMemberFunction)(bool);
//        
//        static QMetaObject::Connection connectFP(QObject *sender,
//                                               QObjectBoolPointerToMemberFunction signal,
//                                               QObject* receiver,
//                                               QObjectBoolPointerToMemberFunction method,
//                                               Qt::ConnectionType type = Qt::AutoConnection);

        // ADD_NEW_METHODS_HERE

    private:
        WuQObject();
        
        virtual ~WuQObject();
        
        static void watchObjectForMacroRecordingPrivate(QObject* sender,
                                                        const char* signal);
        
//        static WidgetWatcher* createWidgetWatcher(QObject* object);
        
        // ADD_NEW_MEMBERS_HERE

        static std::map<QString, WidgetWatcher*> s_widgetWatchers;
    };
    
    /* ========================================================================= */
    
    class MacroMenu : public QMenu {
        Q_OBJECT
        
    public:
        MacroMenu(QMainWindow* mainWindowParent);
        
    private slots:
        void macroMenuAboutToShow();
        void processMacroRecord();
        void processMacroRun();
        void processMacroErase();
        void processMacroLoad();
        void processMacroSave();
        
    private:
        QMainWindow* m_mainWindowParent;
        QAction* m_macroEraseAction;
        QAction* m_macroRecordAction;
        QAction* m_macroRunAction;
        QAction* m_macroLoadAction;
        QAction* m_macroSaveAction;
    };
    
    /* ========================================================================= */
    
    class MacroEventMouseInfoWidgetInterface {
    public:
        MacroEventMouseInfoWidgetInterface() { }
        
        virtual QSize getSizeOfWidget() const = 0;
        
        virtual void processMouseEvent(QMouseEvent*) = 0;
    };
    
    /* ========================================================================= */
    
    class MacroEventMouseInfo {
    public:
        MacroEventMouseInfo();
        
        MacroEventMouseInfo(QMouseEvent* me,
                            const int32_t widgetWidth,
                            const int32_t widgetHeight);
        
        QPointF getLocalPosRescaledToSize(const QSize& widgetSize) const;
        
        QString toString() const;
        
        bool fromString(const QString& s);
        
        QEvent::Type m_type;
        QPointF m_localPos;
        Qt::MouseButton m_button;
        Qt::MouseButtons m_buttons;
        Qt::KeyboardModifiers m_modifiers;
        int32_t m_widgetWidth;
        int32_t m_widgetHeight;
    };
    
    /* ========================================================================= */
    
    class MacroEvent {
    public:
        enum class ValueType {
            BOOLEAN,
            DOUBLE,
            INTEGER,
            MOUSE
        };
        
        MacroEvent(const WidgetWatcherType widgetType,
                   const QString& widgetName);
        
        MacroEvent(QMouseEvent* me,
                   const QString& widgetName,
                   const int32_t widgetWidth,
                   const int32_t widgetHeight);
        
        MacroEvent();
        
        QString toString() const;
        
        bool fromString(const QString& s);
        
        WidgetWatcherType m_widgetType = WidgetWatcherType::INVALID;
        QString m_widgetName;
        ValueType m_valueType = ValueType::INTEGER;
        
        bool m_boolValue = false;
        int m_intValue = 0;
        double m_doubleValue = 0.0;
        
        MacroEventMouseInfo m_mouseEventInfo;
    };
    
    /* ========================================================================= */
    
    class MacroActionEmitter : public QObject {
        Q_OBJECT
        
    public:
        MacroActionEmitter(QAction* action);
        
        void setActionChecked(bool checked);
        
    signals:
        void actionCheckedSignal(bool);
        
    private:
        QAction* m_action;
        
    };
    
    /* ========================================================================= */
    
    class MacroComboBoxEmitter : public QObject {
        Q_OBJECT
        
    public:
        MacroComboBoxEmitter(QComboBox* comboBox);
        
        void setComboBoxIndex(const int index);
        
    signals:
        void valueChangedSignal(int);
        
    private:
        QComboBox* m_comboBox;
    
    };
    
    /* ========================================================================= */
    
//    class MacroSpinBoxEmitter : public QObject {
//        Q_OBJECT
//        
//    public:
//        MacroSpinBoxEmitter(QSpinBox* spinBox);
//        
//        void setSpinBoxValue(const int value);
//        
////    signals:
////        int valueChangedSignal(int);
//        
//    private:
//        QSpinBox* m_spinBox;
//        
//    };
    
    /* ========================================================================= */
    
    class MacroDoubleSpinBoxEmitter : public QObject {
        Q_OBJECT
        
    public:
        MacroDoubleSpinBoxEmitter(QDoubleSpinBox* doubleSpinBox);
        
        void setDoubleSpinBoxValue(const double value);
        
    signals:
        void valueChangedSignal(double);
        
    private:
        QDoubleSpinBox* m_doubleSpinBox;
        
    };
    
    /* ========================================================================= */
    
    class MacroManager {
    public:
        enum class Mode {
            OFF,
            RECORDING,
            RUNNING
        };
        
        static MacroManager* get();
        
        MacroManager();
        
        ~MacroManager();
        
        void addActionTriggeredToMacro(const QString& name,
                                       const bool checkedStatus);
        
        void addCheckBoxClickedToMacro(const QString& name,
                                       const bool checkedStatus);
        
        void addComboBoxActivatedToMacro(const QString& name,
                                         const int index);
        
        void addSpinBoxValueChangedToMacro(const QString& name,
                                           const int value);
        
        void addDoubleSpinBoxValueChangedToMacro(const QString& name,
                                                 const double value);
        
        void addToolButtonClickedToMacro(const QString& name,
                                         const bool checkedStatus);
        
        void recordMouseEvent(QWidget* widget,
                              QMouseEvent* me);
        
        bool isModeRecording() const;
        
        bool runMacro(QObject* window,
                      QString& errorMessageOut);
        
        Mode getMode() const;
        
        void eraseMacro();
        
        void setMode(const Mode mode);
        
        void moveMouse(QWidget* widget,
                       const bool highlightFlag);
        
        void saveMacroToFile(const AString& filename);
        
        void readMacroFromFile(const AString& filename);
        
        std::vector<MacroEvent*> m_macroEvents;
        
        static MacroManager* s_macroManager;
        
        Mode m_mode = Mode::OFF;
    };
    
    /* ========================================================================= */
    
    class WidgetWatcher : public QObject {
        
        Q_OBJECT

    public:
        static WidgetWatcher* newInstance(QObject* object,
                                          const QString& signalName,
                                          QString& errorMessageOut);
        
        WidgetWatcher(const WidgetWatcherType widgetType,
                      QObject* object);
        
        const WidgetWatcherType m_widgetType;
        QString m_name;
//        QObject* m_widget = NULL;
        
    public slots:
        void actionTriggered(bool);
        
        void checkBoxClicked(bool);
        
        void comboBoxActivated(int);
        
        void spinBoxValueChanged(int);
        
        void doubleSpinBoxValueChanged(double);
        
        void toolButtonClicked(bool);
        
    private:
        static WidgetWatcher* unsupportedMessage(QObject* object,
                                                 const QString& signalName,
                                                 QString& errorMessageOut);
        
    };
    
#ifdef __WU_Q_OBJECT_DECLARE__
    MacroManager* MacroManager::s_macroManager = NULL;
    std::map<QString, WidgetWatcher*> WuQObject::s_widgetWatchers;
#endif // __WU_Q_OBJECT_DECLARE__

} // namespace
#endif  //__WU_Q_OBJECT_H__
