
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

#include <cmath>

#define __WU_Q_OBJECT_DECLARE__
#include "MacroPrototype.h"
#undef __WU_Q_OBJECT_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QGuiApplication>
#include <QMainWindow>
#include <QSpinBox>
#include <QTextStream>
#include <QToolButton>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQObject 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup GuiQt
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
WuQObject::WuQObject()
: QObject()
{
    
}

/**
 * Destructor.
 */
WuQObject::~WuQObject()
{
}

QMetaObject::Connection
WuQObject::connect(QObject *sender,
                   const char *signal,
                   const QObject *receiver,
                   const char *method,
                   Qt::ConnectionType type)
{
    CaretAssert(sender);
    const QString name = sender->objectName();
    if (name.isEmpty()) {
        CaretLogWarning("Object name is empty, will be ignored for macros");
    }
    else {
        
        auto existingWatcher = s_widgetWatchers.find(name);
        if (existingWatcher != s_widgetWatchers.end()) {
            CaretLogWarning("Widget named \""
                            + name
                            + "\" has already been connected for macros");
        }
        else {
            AString errorMessage;
            WidgetWatcher* widgetWatcher = WidgetWatcher::newInstance(sender,
                                                                      signal,
                                                                      errorMessage);
            if (widgetWatcher != NULL) {
                s_widgetWatchers.insert(std::make_pair(name,
                                                       widgetWatcher));
            }
            else {
                CaretLogWarning(errorMessage);
            }
        }
    }
    
    QMetaObject::Connection connection = QObject::connect(sender,
                                                          signal,
                                                          receiver,
                                                          method,
                                                          type);
    const bool validConnection(connection);
    CaretAssert(validConnection);
    
    return connection;
}

void
WuQObject::watchObjectForMacroRecording(QObject* sender)
{
    CaretAssert(sender);
    
    QAction* action = qobject_cast<QAction*>(sender);
    if (action != NULL) {
        watchObjectForMacroRecordingPrivate(action,
                                            SIGNAL(triggered(bool)));
        return;
    }
    
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender);
    if (comboBox != NULL) {
        watchObjectForMacroRecordingPrivate(comboBox,
                                            SIGNAL(activated(int)));
        return;
    }
    
    CaretLogWarning("Widget type with name "
                    + sender->objectName()
                    + " of class "
                    + QString(sender->metaObject()->className())
                    + " not supported in watchObjectForMacroRecording()");
}

void
WuQObject::watchObjectForMacroRecordingPrivate(QObject* sender,
                                               const char* signal)
{
    CaretAssert(sender);
    const QString name = sender->objectName();
    if (name.isEmpty()) {
        CaretLogWarning("Object name is empty, will be ignored for macros");
    }
    else {
        
        auto existingWatcher = s_widgetWatchers.find(name);
        if (existingWatcher != s_widgetWatchers.end()) {
            CaretLogWarning("Widget named \""
                            + name
                            + "\" has already been connected for macros");
        }
        else {
            AString errorMessage;
            WidgetWatcher* widgetWatcher = WidgetWatcher::newInstance(sender,
                                                                      signal,
                                                                      errorMessage);
            if (widgetWatcher != NULL) {
                s_widgetWatchers.insert(std::make_pair(name,
                                                       widgetWatcher));
            }
            else {
                CaretLogWarning(errorMessage);
            }
        }
    }
}

//QMetaObject::Connection
//WuQObject::connectFP(QObject *sender,
//                   QObjectBoolPointerToMemberFunction signal,
//                   QObject* receiver,
//                   QObjectBoolPointerToMemberFunction method,
//                   Qt::ConnectionType type)
//{
////    QMetaObject::Connection connection;
//    QMetaObject::Connection connection = QObject::connect(sender,
//                                                          signal,
//                                                          receiver,
//                                                          method,
//                                                          type);
//    const bool validConnection(connection);
//    CaretAssert(validConnection);
//    
//    return connection;
//    
//}

/* ========================================================================= */

/**
 * @return Create and return the macro menu.
 */
MacroMenu::MacroMenu(QMainWindow* mainWindowParent)
: QMenu(mainWindowParent),
m_mainWindowParent(mainWindowParent)
{
    CaretAssert(m_mainWindowParent);
    
    m_macroEraseAction = WuQtUtilities::createAction("Erase",
                                                     "Erase the Macro",
                                                     this,
                                                     this,
                                                     SLOT(processMacroErase()));
    
    m_macroRecordAction = WuQtUtilities::createAction("Record",
                                                      "Record a Macro",
                                                      this,
                                                      this,
                                                      SLOT(processMacroRecord()));
    m_macroRecordAction->setCheckable(true);
    
    m_macroRunAction = WuQtUtilities::createAction("Run",
                                                   "Run a Macro",
                                                   this,
                                                   this,
                                                   SLOT(processMacroRun()));
    
    m_macroLoadAction = WuQtUtilities::createAction("Load from File...",
                                                   "Load a Macro",
                                                   this,
                                                   this,
                                                   SLOT(processMacroLoad()));
    
    m_macroSaveAction = WuQtUtilities::createAction("Save to File...",
                                                   "Save a Macro",
                                                   this,
                                                   this,
                                                   SLOT(processMacroSave()));
    
    setTitle("Macro");
    QObject::connect(this, SIGNAL(aboutToShow()),
                     this, SLOT(macroMenuAboutToShow()));
    addAction(m_macroEraseAction);
    addAction(m_macroRecordAction);
    addAction(m_macroRunAction);
    addSeparator();
    addAction(m_macroLoadAction);
    addAction(m_macroSaveAction);
}

/**
 * Called when macro menu is about to show.
 */
void
MacroMenu::macroMenuAboutToShow()
{
    bool enableEraseFlag(false);
    bool enableRecordFlag(true);
    bool enableRunFlag(true);
    bool recordingFlag(false);
    bool saveFlag(false);
    bool loadFlag(false);
    switch (MacroManager::get()->getMode()) {
        case MacroManager::Mode::OFF:
            enableEraseFlag = true;
            saveFlag = true;
            loadFlag = true;
            break;
        case MacroManager::Mode::RECORDING:
            enableRunFlag = false;
            recordingFlag = true;
            break;
        case MacroManager::Mode::RUNNING:
            enableRecordFlag = false;
            break;
    }
    
    m_macroEraseAction->setEnabled(enableEraseFlag);
    m_macroRecordAction->setEnabled(enableRecordFlag);
    m_macroRecordAction->setChecked(recordingFlag);
    m_macroRunAction->setEnabled(enableRunFlag);
    m_macroLoadAction->setEnabled(loadFlag);
    m_macroSaveAction->setEnabled(saveFlag);
}

/*
 * Called to erase the current macro
 */
void
MacroMenu::processMacroErase()
{
    MacroManager::get()->eraseMacro();
}

/**
 * Called to record a macro
 */
void
MacroMenu::processMacroRecord()
{
    switch (MacroManager::get()->getMode()) {
        case MacroManager::Mode::OFF:
            MacroManager::get()->setMode(MacroManager::Mode::RECORDING);
            break;
        case MacroManager::Mode::RECORDING:
            MacroManager::get()->setMode(MacroManager::Mode::OFF);
            break;
        case MacroManager::Mode::RUNNING:
            break;
    }
}

/**
 * Called to run a macro
 */
void
MacroMenu::processMacroRun()
{
    switch (MacroManager::get()->getMode()) {
        case MacroManager::Mode::OFF:
        {
            AString errorMessage;
            if ( ! MacroManager::get()->runMacro(m_mainWindowParent,
                                                 errorMessage)) {
                WuQMessageBox::errorOk(m_mainWindowParent, errorMessage);
            }
        }
            break;
        case MacroManager::Mode::RECORDING:
            break;
        case MacroManager::Mode::RUNNING:
            MacroManager::get()->setMode(MacroManager::Mode::OFF);
            break;
    }
}
/**
 * Called to load a macro
 */
void
MacroMenu::processMacroLoad()
{
    const QString filename = QFileDialog::getOpenFileName(m_mainWindowParent,
                                                          "Open Macro File",
                                                          "",
                                                          "Macro File (*.wb_macro)");
    if ( ! filename.isNull()) {
        MacroManager::get()->readMacroFromFile(filename);
    }
}

/**
 * Called to save a macro
 */
void
MacroMenu::processMacroSave()
{
    const QString filename = QFileDialog::getSaveFileName(m_mainWindowParent,
                                                          "Save Macro File",
                                                          "",
                                                          "Macro File (*.wb_macro)");
    if ( ! filename.isNull()) {
        MacroManager::get()->saveMacroToFile(filename);
    }
}

/* ========================================================================= */

MacroEventMouseInfo::MacroEventMouseInfo()
{
    
}

MacroEventMouseInfo::MacroEventMouseInfo(QMouseEvent* me,
                                         const int32_t widgetWidth,
                                         const int32_t widgetHeight)
{
    m_type = me->type();
    m_localPos = me->localPos();
    m_button = me->button();
    m_buttons = me->buttons();
    m_modifiers = me->modifiers();
    m_widgetWidth = widgetWidth;
    m_widgetHeight = widgetHeight;
}

/*
 * Widget may be a different size than it was when the event was recorded
 */
QPointF
MacroEventMouseInfo::getLocalPosRescaledToSize(const QSize& widgetSize) const
{
    QPointF pointOut = m_localPos;
    
    const int32_t newWidth = widgetSize.width();
    const int32_t newHeight = widgetSize.height();
    
    if ((newWidth != m_widgetWidth)
        || (newHeight != m_widgetHeight)) {
        const float normalizedWidth = (static_cast<float>(m_localPos.x())
                                       / static_cast<float>(m_widgetWidth));
        const float normalizedHeight = (static_cast<float>(m_localPos.y())
                                        / static_cast<float>(m_widgetHeight));
        
        pointOut.setX(newWidth * normalizedWidth);
        pointOut.setY(newHeight * normalizedHeight);
    }
    
    return pointOut;
}

QString
MacroEventMouseInfo::toString() const
{
    const QString separator("*");
    QString s(QString::number((int)m_type)
              + separator
              + QString::number(m_localPos.x())
              + separator
              + QString::number(m_localPos.y())
              + separator
              + QString::number((int)m_button)
              + separator
              + QString::number((int)m_buttons)
              + separator
              + QString::number((int)m_modifiers)
              + separator
              + QString::number(m_widgetWidth)
              + separator
              + QString::number(m_widgetHeight));
    
    return s;
}

bool
MacroEventMouseInfo::fromString(const QString& s)
{
    const QString separator("*");
    QStringList components = s.split(separator);
    if (components.size() == 8) {
        m_type = static_cast<QEvent::Type>(components[0].toInt());
        m_localPos.setX(components[1].toFloat());
        m_localPos.setY(components[2].toFloat());
        m_button = static_cast<Qt::MouseButton>(components[3].toInt());
        m_buttons = static_cast<Qt::MouseButtons>(components[4].toInt());
        m_modifiers = static_cast<Qt::KeyboardModifiers>(components[5].toInt());
        m_widgetWidth = components[6].toInt();
        m_widgetHeight = components[7].toInt();
    }
    else {
        CaretLogWarning("Invalid value string for mouse info: "
                        + s);
        return false;
    }
    return true;
}

/* ========================================================================= */

MacroEvent::MacroEvent(const WidgetWatcherType widgetType,
                       const QString& widgetName)
: m_widgetType(widgetType),
m_widgetName(widgetName)
{
    m_valueType = ValueType::BOOLEAN;
    switch (m_widgetType) {
        case ACTION:
            m_valueType = ValueType::BOOLEAN;
            break;
        case CHECK_BOX:
            m_valueType = ValueType::BOOLEAN;
            break;
        case COMBO_BOX:
            m_valueType = ValueType::INTEGER;
            break;
        case DOUBLE_SPIN_BOX:
            m_valueType = ValueType::DOUBLE;
            break;
        case INVALID:
            break;
        case MOUSE_EVENT:
            CaretAssert(0);
            break;
        case SPIN_BOX:
            m_valueType = ValueType::INTEGER;
            break;
        case TOOL_BUTTON:
            m_valueType = ValueType::BOOLEAN;
            break;
    }
}

MacroEvent::MacroEvent(QMouseEvent* me,
                       const QString& widgetName,
                       const int32_t widgetWidth,
                       const int32_t widgetHeight)
: m_widgetType(WidgetWatcherType::MOUSE_EVENT),
m_widgetName(widgetName)
{
    m_valueType = ValueType::MOUSE;
    
    m_mouseEventInfo = MacroEventMouseInfo(me,
                                           widgetWidth,
                                           widgetHeight);
}

MacroEvent::MacroEvent()
{
    
}

QString
MacroEvent::toString() const
{
    AString widgetTypeString("INVALID");
    switch (m_widgetType) {
        case ACTION:
            widgetTypeString = "ACTION";
            break;
        case CHECK_BOX:
            widgetTypeString = "CHECK_BOX";
            break;
        case COMBO_BOX:
            widgetTypeString = "COMBO_BOX";
            break;
        case DOUBLE_SPIN_BOX:
            widgetTypeString = "DOUBLE_SPIN_BOX";
            break;
        case INVALID:
            break;
        case MOUSE_EVENT:
            widgetTypeString = "MOUSE";
            break;
        case SPIN_BOX:
            widgetTypeString = "SPIN_BOX";
            break;
        case TOOL_BUTTON:
            widgetTypeString = "TOOL_BUTTON";
            break;
    }
    
    AString valueString;
    switch (m_valueType) {
        case ValueType::BOOLEAN:
            valueString = AString::fromBool(m_boolValue);
            break;
        case ValueType::INTEGER:
            valueString = AString::number(m_intValue);
            break;
        case ValueType::DOUBLE:
            valueString = AString::number(m_doubleValue);
            break;
        case ValueType::MOUSE:
            valueString = m_mouseEventInfo.toString();
            break;
    }
    
    const QString sep(":");
    QString s(m_widgetName
              + sep
              + widgetTypeString
              + sep
              + valueString);
    
    return s;
}

bool
MacroEvent::fromString(const QString& s)
{
    bool validFlag = false;
    const QString sep(":");
    QStringList sl(s.split(sep));
    
    if (sl.length() == 3) {
        validFlag = true;
        const QString widgetName(sl.at(0));
        const QString widgetTypeName(sl.at(1));
        const AString valueString(sl.at(2));
        
        WidgetWatcherType widgetType = WidgetWatcherType::INVALID;
        if (widgetTypeName == "ACTION") {
            widgetType = WidgetWatcherType::ACTION;
        }
        else if (widgetTypeName == "CHECK_BOX") {
            widgetType = WidgetWatcherType::CHECK_BOX;
        }
        else if (widgetTypeName == "COMBO_BOX") {
            widgetType = WidgetWatcherType::COMBO_BOX;
        }
        else if (widgetTypeName == "DOUBLE_SPIN_BOX") {
            widgetType = WidgetWatcherType::DOUBLE_SPIN_BOX;
        }
        else if (widgetTypeName == "MOUSE") {
            widgetType = WidgetWatcherType::MOUSE_EVENT;
        }
        else if (widgetTypeName == "SPIN_BOX") {
            widgetType = WidgetWatcherType::SPIN_BOX;
        }
        else if (widgetTypeName == "TOOL_BUTTON") {
            widgetType = WidgetWatcherType::TOOL_BUTTON;
        }
        else {
            validFlag = false;
            CaretLogSevere("Invalid widget type: " + widgetTypeName);
        }
        
        if (validFlag) {
            m_widgetName = widgetName;
            m_widgetType = widgetType;
            if (widgetType == WidgetWatcherType::MOUSE_EVENT) {
                validFlag = m_mouseEventInfo.fromString(valueString);
            }
            else {
                m_boolValue = valueString.toBool();
                m_intValue  = valueString.toInt();
                m_doubleValue = valueString.toDouble();
            }
        }
    }
    else {
        CaretLogSevere("Invalid string for reading macro " + s);
    }
    
    return validFlag;
}

/* ========================================================================= */

MacroManager::MacroManager()
{
    
}

MacroManager::~MacroManager()
{
    eraseMacro();
}

MacroManager*
MacroManager::get()
{
    if (s_macroManager == NULL) {
        s_macroManager = new MacroManager();
    }
    
    CaretAssert(s_macroManager);
    return s_macroManager;
}

void
MacroManager::addActionTriggeredToMacro(const QString& name,
                                        const bool checkedStatus)
{
    MacroEvent* macroEvent = new MacroEvent(WidgetWatcherType::ACTION,
                                            name);
    macroEvent->m_boolValue = checkedStatus;
    m_macroEvents.push_back(macroEvent);
}


void
MacroManager::addCheckBoxClickedToMacro(const QString& name,
                                        const bool checkedStatus)
{
    MacroEvent* macroEvent = new MacroEvent(WidgetWatcherType::CHECK_BOX,
                                            name);
    macroEvent->m_boolValue = checkedStatus;
    m_macroEvents.push_back(macroEvent);
}

void
MacroManager::addComboBoxActivatedToMacro(const QString& name,
                                          const int index)
{
    MacroEvent* macroEvent = new MacroEvent(WidgetWatcherType::COMBO_BOX,
                                            name);
    macroEvent->m_intValue = index;
    m_macroEvents.push_back(macroEvent);
}

void
MacroManager::addSpinBoxValueChangedToMacro(const QString& name,
                                            const int value)
{
    MacroEvent* macroEvent = new MacroEvent(WidgetWatcherType::SPIN_BOX,
                                            name);
    macroEvent->m_intValue = value;
    m_macroEvents.push_back(macroEvent);
}

void
MacroManager::addDoubleSpinBoxValueChangedToMacro(const QString& name,
                                            const double value)
{
    MacroEvent* macroEvent = new MacroEvent(WidgetWatcherType::DOUBLE_SPIN_BOX,
                                            name);
    macroEvent->m_doubleValue = value;
    m_macroEvents.push_back(macroEvent);
}

void
MacroManager::addToolButtonClickedToMacro(const QString& name,
                                        const bool checkedStatus)
{
    MacroEvent* macroEvent = new MacroEvent(WidgetWatcherType::TOOL_BUTTON,
                                            name);
    macroEvent->m_boolValue = checkedStatus;
    m_macroEvents.push_back(macroEvent);
}

void
MacroManager::recordMouseEvent(QWidget* widget,
                               QMouseEvent* me)
{
    CaretAssert(widget);
    
    if (isModeRecording()) {
        const QString name(widget->objectName());
        if (name.isEmpty()) {
            CaretLogWarning("recordMouseEvent() widget name is empty.");
            return;
        }
        
        MacroEvent* macroEvent = new MacroEvent(me,
                                                name,
                                                widget->width(),
                                                widget->height());
        m_macroEvents.push_back(macroEvent);
    }
}

void
MacroManager::moveMouse(QWidget* widget,
                        const bool highlightFlag)
{
    CaretAssert(widget);
    
    const QPoint widgetCenter = widget->rect().center();
    const QPoint windowPoint = widget->mapToGlobal(widgetCenter);
    
    if (highlightFlag) {
        const float radius = 15.0;
        for (float angle = 0.0; angle < 6.28; angle += 0.314) {
            const float x = windowPoint.x() + (std::cos(angle) * radius);
            const float y = windowPoint.y() + (std::sin(angle) * radius);
            QCursor::setPos(x, y);
            SystemUtilities::sleepSeconds(0.025);
        }
    }
    
    QCursor::setPos(windowPoint);
}


bool
MacroManager::runMacro(QObject* window,
                       QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    switch (m_mode) {
        case MacroManager::Mode::OFF:
            break;
        case MacroManager::Mode::RECORDING:
            errorMessageOut = "Recording is on, turn off recording, then run";
            return false;
            break;
        case MacroManager::Mode::RUNNING:
            CaretAssert(0);
            break;
    }
    
    m_mode = Mode::RUNNING;
    
    for (auto me : m_macroEvents) {
        CaretAssert(me);
        
        const QString widgetName(me->m_widgetName);
        QWidget* widgetToMoveMouse = window->findChild<QWidget*>(widgetName);
        
        if (widgetToMoveMouse == NULL) {
            QObject* object = window->findChild<QObject*>(widgetName);
            if (object != NULL) {
                QObject* parent = object->parent();
                if (parent != NULL) {
                    widgetToMoveMouse = qobject_cast<QWidget*>(parent);
                }
            }
        }
        
        const bool mouseEventFlag = (me->m_widgetType == WidgetWatcherType::MOUSE_EVENT);
        if (widgetToMoveMouse != NULL) {
            if ( ! mouseEventFlag) {
                const bool highlightFlag = ( ! mouseEventFlag);
                moveMouse(widgetToMoveMouse,
                          highlightFlag);
            }
        }
        
        switch (me->m_widgetType) {
            case WidgetWatcherType::INVALID:
                CaretAssert(0);
                break;
            case ACTION:
            {
                QAction* action = window->findChild<QAction*>(me->m_widgetName);
                if (action != NULL) {
                    MacroActionEmitter actionUpdater(action);
                    actionUpdater.setActionChecked(me->m_boolValue);
                }
                else {
                    CaretLogWarning("ERROR: Unable to find action named "
                                    + me->m_widgetName);
                }
            }
                break;
            case WidgetWatcherType::CHECK_BOX:
            {
                QCheckBox* checkBox = window->findChild<QCheckBox*>(me->m_widgetName);
                if (checkBox != NULL) {
                    checkBox->setChecked( ! me->m_boolValue);
                    checkBox->animateClick(100); // milliseconds
                }
                else {
                    CaretLogWarning("ERROR: Unable to find checkbox named "
                                    + me->m_widgetName);
                }
            }
                break;
            case WidgetWatcherType::COMBO_BOX:
            {
                QComboBox* comboBox = window->findChild<QComboBox*>(me->m_widgetName);
                if (comboBox != NULL) {
                    //comboBox->setCurrentIndex(me->m_intValue);
                    MacroComboBoxEmitter comboBoxUpdater(comboBox);
                    comboBoxUpdater.setComboBoxIndex(me->m_intValue);
                }
                else {
                    CaretLogWarning("ERROR: Unabe to find combo box named "
                                    + me->m_widgetName);
                }
            }
                break;
            case WidgetWatcherType::DOUBLE_SPIN_BOX:
            {
                QList<QDoubleSpinBox*> boxes = window->findChildren<QDoubleSpinBox*>(me->m_widgetName);
                QDoubleSpinBox* doubleSpinBox = window->findChild<QDoubleSpinBox*>(me->m_widgetName);
                if (doubleSpinBox != NULL) {

                    MacroDoubleSpinBoxEmitter doubleSpinBoxUpdater(doubleSpinBox);
                    doubleSpinBoxUpdater.setDoubleSpinBoxValue(me->m_doubleValue);
                }
                else {
                    CaretLogWarning("ERROR: Unabe to find double spin box named "
                                    + me->m_widgetName);
                }
            }
                break;
            case WidgetWatcherType::MOUSE_EVENT:
            {
                QWidget* widget = window->findChild<QWidget*>(me->m_widgetName);
                if (widget != NULL) {
                    MacroEventMouseInfoWidgetInterface* mouseInterface = dynamic_cast<MacroEventMouseInfoWidgetInterface*>(widget);
                    if (mouseInterface != NULL) {
                        const QSize currentWidgetSize = mouseInterface->getSizeOfWidget();
                        const MacroEventMouseInfo& memi = me->m_mouseEventInfo;
                        const QPointF adjustedLocalPos = memi.getLocalPosRescaledToSize(currentWidgetSize);
                        
                        QMouseEvent mouseEvent(memi.m_type,
                                               adjustedLocalPos, //memi.m_localPos,
                                               memi.m_button,
                                               memi.m_buttons,
                                               memi.m_modifiers);
                        mouseInterface->processMouseEvent(&mouseEvent);
                        
//                        std::cout << "Original x/y: " << memi.m_localPos.x()
//                        << ", " << memi.m_localPos.y() << std::endl;
//                        std::cout << "   Updated x/y: " << adjustedLocalPos.x()
//                        << ", " << adjustedLocalPos.y() << std::endl;
                        
//                        QEvent* event = &mouseEvent;
//                        widget->event(event);
//                        switch (memi.m_type) {
//                            case QEvent::MouseButtonDblClick:
//                                break;
//                            case QEvent::MouseButtonPress:
//                                break;
//                            case QEvent::MouseButtonRelease:
//                                break;
//                            case QEvent::MouseMove:
//                                break;
//                            default:
//                                CaretAssert(0);
//                                break;
//                        }
                    }
                    else {
                        CaretLogWarning("ERROR: widget named "
                                        + me->m_widgetName
                                        + " does not cast to MacroEventMouseInfoWidgetInterface");
                    }
                }
                else {
                    CaretLogWarning("ERROR: Unabe to widget named "
                                    + me->m_widgetName);
                }
            }
                break;
            case WidgetWatcherType::SPIN_BOX:
            {
                QSpinBox* spinBox = window->findChild<QSpinBox*>(me->m_widgetName);
                if (spinBox != NULL) {
                    /*
                     * Calling QSpinBox::setValue() results in emission of 
                     * the QSpinBox::valueChanged() signal.
                     */
                    spinBox->setValue(me->m_intValue);
                }
                else {
                    CaretLogWarning("ERROR: Unabe to find spin box named "
                                    + me->m_widgetName);
                }
            }
                break;
            case WidgetWatcherType::TOOL_BUTTON:
            {
                QToolButton* toolButton = window->findChild<QToolButton*>(me->m_widgetName);
                if (toolButton != NULL) {
                    toolButton->setChecked( ! me->m_boolValue);
                    toolButton->animateClick(1000); // 1000 milliseconds
                }
                else {
                    CaretLogWarning("ERROR: Unable to find toolbutton named "
                                    + me->m_widgetName);
                }
            }
                break;
        }
        
        QGuiApplication::processEvents();
        if ( ! mouseEventFlag) {
            SystemUtilities::sleepSeconds(1);
        }
        QGuiApplication::processEvents();
    }
    
    m_mode = Mode::OFF;
    
    return (errorMessageOut.isEmpty());
}

void
MacroManager::eraseMacro()
{
    for (auto me : m_macroEvents) {
        CaretAssert(me);
        delete me;
    }
    m_macroEvents.clear();
}


MacroManager::Mode
MacroManager::getMode() const
{
    return m_mode;
}

bool
MacroManager::isModeRecording() const
{
    switch (m_mode) {
        case Mode::OFF:
            break;
        case Mode::RECORDING:
            return true;
            break;
        case Mode::RUNNING:
            break;
    }
    
    return false;
}

void
MacroManager::setMode(const Mode mode)
{
    m_mode = mode;
}

void
MacroManager::saveMacroToFile(const AString& filename)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly)) {
        QTextStream stream(&file);
        for (auto me : m_macroEvents) {
            stream << me->toString() << "\n";
        }
        file.close();
    }
    else {
         CaretLogSevere("Unable to open for writing: "
                        + filename);
    }
}

void
MacroManager::readMacroFromFile(const AString& filename)
{
    eraseMacro();
    
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        
        QString fileText(stream.readAll());
        QStringListIterator lineIter(fileText.split("\n"));
        while (lineIter.hasNext()) {
            const QString text = lineIter.next().trimmed();
            if ( ! text.isEmpty()) {
                MacroEvent* me = new MacroEvent();
                const bool valid = me->fromString(text);
                if (valid) {
                    m_macroEvents.push_back(me);
                }
                else {
                    CaretLogWarning("Invalid line in macro file: "
                                    + text);
                    delete me;
                }
            }
        }
        file.close();
    }
    else {
        CaretLogSevere("Unable to open for writing: "
                       + filename);
    }
}

/* ========================================================================= */

MacroActionEmitter::MacroActionEmitter(QAction* action)
: m_action(action)
{
    CaretAssert(m_action);
    /*
     * Need to cause emission of activated signale
     */
    QObject::connect(this, SIGNAL(actionCheckedSignal(bool)),
                     m_action, SIGNAL(triggered(bool)));
}

void
MacroActionEmitter::setActionChecked(bool checked)
{
//    QSignalBlocker blocker(m_action);
    m_action->setChecked(checked);
    emit actionCheckedSignal(checked);
}


MacroComboBoxEmitter::MacroComboBoxEmitter(QComboBox* comboBox)
: m_comboBox(comboBox)
{
    CaretAssert(comboBox);
    
    /*
     * Need to cause emission of activated signale
     */
    QObject::connect(this, SIGNAL(valueChangedSignal(int)),
                     m_comboBox, SIGNAL(activated(int)));
}

void
MacroComboBoxEmitter::setComboBoxIndex(const int index)
{
    m_comboBox->showPopup();
    m_comboBox->setCurrentIndex(index);
    m_comboBox->hidePopup();
    emit valueChangedSignal(index);
}


//MacroSpinBoxEmitter::MacroSpinBoxEmitter(QSpinBox* spinBox)
//: m_spinBox(spinBox)
//{
//    CaretAssert(spinBox);
//    
////    /*
////     * Need to cause emission of activated signale
////     */
////    QObject::connect(this, SIGNAL(valueChangedSignal(int)),
////                     m_spinBox, SIGNAL(valueChanged(int)));
//}
//
//void
//MacroSpinBoxEmitter::setSpinBoxValue(const int value)
//{
//    /*
//     * With a spin box, calling setValue() results in the valueChanged() 
//     * signal being emitted
//     */
//    m_spinBox->setValue(value);
//    
//}

MacroDoubleSpinBoxEmitter::MacroDoubleSpinBoxEmitter(QDoubleSpinBox* doubleSpinBox)
: m_doubleSpinBox(doubleSpinBox)
{
    CaretAssert(doubleSpinBox);
    
    QObject::connect(this, SIGNAL(valueChangedSignal(double)),
                     m_doubleSpinBox, SIGNAL(valueChanged(double)));
}

void
MacroDoubleSpinBoxEmitter::setDoubleSpinBoxValue(const double value)
{
    QSignalBlocker blocker(m_doubleSpinBox);
    m_doubleSpinBox->setValue(value);
    blocker.unblock();

    emit valueChangedSignal(value);
}

/* ========================================================================= */

WidgetWatcher*
WidgetWatcher::unsupportedMessage(QObject* object,
                                  const QString& signalName,
                                  QString& errorMessageOut)
{
    errorMessageOut = ("Unsupported signal \""
                       + signalName
                       + " for named \""
                       + object->objectName()
                       + "\"");

    return NULL;
}

WidgetWatcher*
WidgetWatcher::newInstance(QObject* object,
                           const QString& signalName,
                           QString& errorMessageOut)
{
    CaretAssert(object);
    errorMessageOut.clear();
    
    QAction* action = qobject_cast<QAction*>(object);
    if (action != NULL) {
        if (signalName.contains("triggered(bool)")) {
            WidgetWatcher* ww = new WidgetWatcher(WidgetWatcherType::ACTION,
                                                  action);
            return ww;
        }
        else {
            return unsupportedMessage(object,
                                      signalName,
                                      errorMessageOut);
        }
    }
    
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
    if (checkBox != NULL) {
        if (signalName.contains("clicked(bool)")) {
            WidgetWatcher* ww = new WidgetWatcher(WidgetWatcherType::CHECK_BOX,
                                                  checkBox);
            return ww;
        }
        else {
            errorMessageOut = ("Unsupported signal \""
                               + signalName
                               + " for named \""
                               + object->objectName()
                               + "\"");
            return NULL;
        }
    }
    
    QComboBox* comboBox = qobject_cast<QComboBox*>(object);
    if (comboBox != NULL) {
        if (signalName.contains("activated(int)")) {
            WidgetWatcher* ww = new WidgetWatcher(WidgetWatcherType::COMBO_BOX,
                                                  comboBox);
            return ww;
        }
        else {
            errorMessageOut = ("Unsupported signal \""
                               + signalName
                               + " for named \""
                               + object->objectName()
                               + "\"");
            return NULL;
        }
    }
    
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
    if (spinBox != NULL) {
        if (signalName.contains("valueChanged(int)")) {
            WidgetWatcher* ww = new WidgetWatcher(WidgetWatcherType::SPIN_BOX,
                                                  spinBox);
            return ww;
        }
        else {
            errorMessageOut = ("Unsupported signal \""
                               + signalName
                               + " for named \""
                               + object->objectName()
                               + "\"");
            return NULL;
        }
    }
    
    QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(object);
    if (doubleSpinBox != NULL) {
        if (signalName.contains("valueChanged(double)")) {
            WidgetWatcher* ww = new WidgetWatcher(WidgetWatcherType::DOUBLE_SPIN_BOX,
                                                  doubleSpinBox);
            return ww;
        }
        else {
            errorMessageOut = ("Unsupported signal \""
                               + signalName
                               + " for named \""
                               + object->objectName()
                               + "\"");
            return NULL;
        }
    }
    
    QToolButton* toolButton = qobject_cast<QToolButton*>(object);
    if (toolButton != NULL) {
        if (signalName.contains("clicked(bool)")) {
            WidgetWatcher* ww = new WidgetWatcher(WidgetWatcherType::TOOL_BUTTON,
                                                  toolButton);
            return ww;
        }
        else {
            errorMessageOut = ("Unsupported signal \""
                               + signalName
                               + " for named \""
                               + object->objectName()
                               + "\"");
            return NULL;
        }
    }
    
    errorMessageOut = ("Widget named \""
                       + object->objectName()
                       + "\" of class \""
                       + object->metaObject()->className()
                       + "\" is not supported for macros");
    
    return NULL;
}


WidgetWatcher::WidgetWatcher(const WidgetWatcherType widgetType,
                             QObject* object)
: m_widgetType(widgetType)
{
    CaretAssert(object);
    m_name = object->objectName();
    
    switch (m_widgetType) {
        case INVALID:
            break;
        case ACTION:
        {
            QAction* action = qobject_cast<QAction*>(object);
            CaretAssert(action);
            QObject::connect(action, &QAction::triggered,
                             this, &WidgetWatcher::actionTriggered);
        }
            break;
        case CHECK_BOX:
        {
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
            CaretAssert(checkBox);
            QObject::connect(checkBox, &QCheckBox::clicked,
                             this, &WidgetWatcher::checkBoxClicked);
        }
            break;
        case COMBO_BOX:
        {
            QComboBox* comboBox = qobject_cast<QComboBox*>(object);
            CaretAssert(comboBox);
            QObject::connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                             this, &WidgetWatcher::comboBoxActivated);
        }
            break;
        case DOUBLE_SPIN_BOX:
        {
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(object);
            CaretAssert(spinBox);
            QObject::connect(spinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             this, &WidgetWatcher::doubleSpinBoxValueChanged);
        }
            break;
        case MOUSE_EVENT:
            break;
        case SPIN_BOX:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
            CaretAssert(spinBox);
            QObject::connect(spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                             this, &WidgetWatcher::spinBoxValueChanged);
        }
            break;
        case TOOL_BUTTON:
        {
            QToolButton* toolButton = qobject_cast<QToolButton*>(object);
            CaretAssert(toolButton);
            QObject::connect(toolButton, &QCheckBox::clicked,
                             this, &WidgetWatcher::toolButtonClicked);
        }
            break;
    }
}

void
WidgetWatcher::actionTriggered(bool checked)
{
    if (MacroManager::get()->isModeRecording()) {
        MacroManager::get()->addActionTriggeredToMacro(m_name,
                                                       checked);
    }
}

void
WidgetWatcher::checkBoxClicked(bool checked)
{
    if (MacroManager::get()->isModeRecording()) {
        MacroManager::get()->addCheckBoxClickedToMacro(m_name,
                                                       checked);
    }
}

void
WidgetWatcher::comboBoxActivated(int index)
{
    if (MacroManager::get()->isModeRecording()) {
        MacroManager::get()->addComboBoxActivatedToMacro(m_name,
                                                         index);
    }
}

void
WidgetWatcher::spinBoxValueChanged(int value)
{
    if (MacroManager::get()->isModeRecording()) {
        MacroManager::get()->addSpinBoxValueChangedToMacro(m_name,
                                                           value);
    }
}

void
WidgetWatcher::doubleSpinBoxValueChanged(double value)
{
    if (MacroManager::get()->isModeRecording()) {
        MacroManager::get()->addDoubleSpinBoxValueChangedToMacro(m_name,
                                                                 value);
    }
}

void
WidgetWatcher::toolButtonClicked(bool checked)
{
    if (MacroManager::get()->isModeRecording()) {
        MacroManager::get()->addToolButtonClickedToMacro(m_name,
                                                         checked);
    }
}


