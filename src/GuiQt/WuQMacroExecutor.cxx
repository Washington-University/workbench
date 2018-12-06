
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

#define __WU_Q_MACRO_EXECUTOR_DECLARE__
#include "WuQMacroExecutor.h"
#undef __WU_Q_MACRO_EXECUTOR_DECLARE__

#include <cmath>

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>

#include "CaretAssert.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroMouseEventWidgetInterface.h"
#include "WuQMacroSignalEmitter.h"

using namespace caret;

/**
 * \class caret::WuQMacroExecutor 
 * \brief Executes a macro
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 */
WuQMacroExecutor::WuQMacroExecutor()
: QObject()
{
    
}

/**
 * Destructor.
 */
WuQMacroExecutor::~WuQMacroExecutor()
{
}

/**
 * Move the mouse around the given widget
 *
 * @param widget
 *     Widget to where mouse should be moved
 * @param highlightFlag
 *     Highlights widget by having mouse circle around widget center
 */
void
WuQMacroExecutor::moveMouse(QWidget* widget,
                            const bool highlightFlag) const
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

/**
 * Run the commands in the given macro.
 *
 * @param macro
 *    Macro that is run
 * @param window
 *    The window from which macro was launched
 * @param options
 *    Executor options
 * @param errorMessageOut
 *    Output containing any error messages
 * @return
 *    True if the macro completed without errors, else false.
 */
bool
WuQMacroExecutor::runMacro(const WuQMacro* macro,
                           QObject* window,
                           const RunOptions& options,
                           QString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    const int32_t numberOfMacroCommands = macro->getNumberOfMacroCommands();
    for (int32_t i = 0; i < numberOfMacroCommands; i++) {
        const WuQMacroCommand* mc = macro->getMacroCommandAtIndex(i);
        CaretAssert(mc);
        
        const QString objectName(mc->getObjectName());
        QObject* object = window->findChild<QObject*>(objectName);
        if (object == NULL) {
            errorMessageOut.append("Unable to find object named "
                                   + objectName
                                   + "\n");
            if (options.m_stopOnErrorFlag) {
                return false;
            }
            continue;
        }
        
        if (object->signalsBlocked()) {
            errorMessageOut.append("Object named "
                                   + objectName
                                   + " has signals blocked");
            if (options.m_stopOnErrorFlag) {
                return false;
            }
            continue;
        }
        
        QWidget* widgetToMoveMouse = qobject_cast<QWidget*>(object);
        
        if (widgetToMoveMouse == NULL) {
            QObject* object = window->findChild<QObject*>(objectName);
            if (object != NULL) {
                QObject* parent = object->parent();
                if (parent != NULL) {
                    widgetToMoveMouse = qobject_cast<QWidget*>(parent);
                }
            }
        }
        
        const WuQMacroObjectTypeEnum::Enum objectType = mc->getObjectType();
        const bool mouseEventFlag = (objectType == WuQMacroObjectTypeEnum::MOUSE_USER_EVENT);
        
        if (options.m_showMouseMovementFlag) {
            if (widgetToMoveMouse != NULL) {
                if ( ! mouseEventFlag) {
                    const bool highlightFlag = ( ! mouseEventFlag);
                    moveMouse(widgetToMoveMouse,
                              highlightFlag);
                }
            }
        }
        
        QString commandErrorMessage;
        if ( ! runMacroCommand(mc,
                               object,
                               commandErrorMessage)) {
            errorMessageOut.append(commandErrorMessage + "\n");
            if (options.m_stopOnErrorFlag) {
                return false;
            }
        }
        
        QGuiApplication::processEvents();
        if ( ! mouseEventFlag) {
            if (options.m_secondsDelayBetweenCommands > 0.0) {
                SystemUtilities::sleepSeconds(options.m_secondsDelayBetweenCommands);
            }
        }
        
        QGuiApplication::processEvents();
    }
    
    return (errorMessageOut.isEmpty());
}

/**
 * Run the commands in the given macro.
 *
 * @param macro
 *    Macro that is run
 * @param window
 *    The window from which macro was launched
 * @param stopOnErrorFlag
 *    If true, stop running the commands if there is an error
 * @param errorMessageOut
 *    Output containing any error messages
 * @return
 *    True if the macro completed without errors, else false.
 */
bool
WuQMacroExecutor::runMacroCommand(const WuQMacroCommand* macroCommand,
                                  QObject* object,
                                  QString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    CaretAssert(macroCommand);
    CaretAssert(object);
    
    const WuQMacroObjectTypeEnum::Enum objectType = macroCommand->getObjectType();
    const WuQMacroDataValueTypeEnum::Enum dataValueType = macroCommand->getObjectDataValueType();
    const QVariant objectValue = macroCommand->getObjectValue();
    
    bool notFoundFlag(false);
    switch (objectType) {
        case WuQMacroObjectTypeEnum::ACTION:
        {
            QAction* action = qobject_cast<QAction*>(object);
            if (action != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQActionSignal(action,
                                                objectValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::CHECK_BOX:
        {
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
            if (checkBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQCheckBoxSignal(checkBox,
                                                  objectValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::COMBO_BOX:
        {
            QComboBox* comboBox = qobject_cast<QComboBox*>(object);
            if (comboBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQComboBoxSignal(comboBox,
                                                  objectValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::DOUBLE_SPIN_BOX:
        {
            QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(object);
            if (doubleSpinBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::FLOAT);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQDoubleSpinBoxSignal(doubleSpinBox,
                                                       objectValue.toDouble());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case WuQMacroObjectTypeEnum::LINE_EDIT:
        {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
            if (lineEdit != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::STRING);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQLineEditSignal(lineEdit,
                                                  objectValue.toString());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::LIST_WIDGET:
        {
            QListWidget* listWidget = qobject_cast<QListWidget*>(object);
            if (listWidget != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::STRING);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQListWidgetSignal(listWidget,
                                                    objectValue.toString());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::MENU:
        {
            QMenu* menu = qobject_cast<QMenu*>(object);
            if (menu != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::STRING);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQMenuTriggered(menu,
                                                 objectValue.toString());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::MOUSE_USER_EVENT:
        {
            QWidget* widget = qobject_cast<QWidget*>(object);
            if (widget != NULL) {
                WuQMacroMouseEventWidgetInterface* mouseInterface = dynamic_cast<WuQMacroMouseEventWidgetInterface*>(widget);
                if (mouseInterface != NULL) {
                    const QSize currentWidgetSize = widget->size();
                    const WuQMacroMouseEventInfo* mouseEventInfo = macroCommand->getMouseEventInfo();
                    CaretAssert(mouseEventInfo);
                    
                    int32_t adjustedLocalX(0);
                    int32_t adjustedLocalY(0);
                    mouseEventInfo->getLocalPositionRescaledToWidgetSize(currentWidgetSize.width(),
                                                                         currentWidgetSize.height(),
                                                                         adjustedLocalX,
                                                                         adjustedLocalY);
                    
                    QEvent::Type qtEventType = QEvent::None;
                    switch (mouseEventInfo->getMouseEventType()) {
                        case WuQMacroMouseEventTypeEnum::BUTTON_PRESS:
                            qtEventType = QEvent::MouseButtonPress;
                            break;
                        case WuQMacroMouseEventTypeEnum::BUTTON_RELEASE:
                            qtEventType = QEvent::MouseButtonRelease;
                            break;
                        case WuQMacroMouseEventTypeEnum::DOUBLE_CLICK:
                            qtEventType = QEvent::MouseButtonDblClick;
                            break;
                        case WuQMacroMouseEventTypeEnum::MOVE:
                            qtEventType = QEvent::MouseMove;
                            break;
                    }
                    
                    QMouseEvent qtMouseEvent(qtEventType,
                                             QPointF(adjustedLocalX,
                                                     adjustedLocalY),
                                             static_cast<Qt::MouseButton>(mouseEventInfo->getMouseButton()),
                                             static_cast<Qt::MouseButtons>(mouseEventInfo->getMouseButtonsMask()),
                                             static_cast<Qt::KeyboardModifiers>(mouseEventInfo->getKeyboardModifiersMask()));
                    mouseInterface->processMouseEventFromMacro(&qtMouseEvent);
                }
                else {
                    errorMessageOut = ("ERROR: Unable to cast object named "
                                       + object->objectName()
                                       + " to WuQMacroMouseEventWidgetInterface");
                    return false;
                }
            }
            else {
                errorMessageOut = ("ERROR: Unable to cast object named "
                                   + object->objectName()
                                   + " to QWidget");
                return false;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::PUSH_BUTTON:
        {
            QPushButton* pushButton = qobject_cast<QPushButton*>(object);
            if (pushButton != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQPushButtonSignal(pushButton,
                                                    objectValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::RADIO_BUTTON:
        {
            QRadioButton* radioButton = qobject_cast<QRadioButton*>(object);
            if (radioButton != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQRadioButtonSignal(radioButton,
                                                     objectValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::SLIDER:
        {
            QSlider* slider = qobject_cast<QSlider*>(object);
            if (slider != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQSliderSignal(slider,
                                                objectValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::SPIN_BOX:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
            if (spinBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQSpinBoxSignal(spinBox,
                                                 objectValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::TAB_BAR:
        {
            QTabBar* tabBar = qobject_cast<QTabBar*>(object);
            if (tabBar != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQTabBarSignal(tabBar,
                                                objectValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::TAB_WIDGET:
        {
            QTabWidget* tabWidget = qobject_cast<QTabWidget*>(object);
            if (tabWidget != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQTabWidgetSignal(tabWidget,
                                                   objectValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroObjectTypeEnum::TOOL_BUTTON:
        {
            QToolButton* toolButton = qobject_cast<QToolButton*>(object);
            if (toolButton != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQToolButtonSignal(toolButton,
                                                    objectValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
    }
    
    if (notFoundFlag) {
        errorMessageOut = ("ERROR: Unable to cast object named "
                           + object->objectName()
                           + " to "
                           + WuQMacroObjectTypeEnum::toGuiName(objectType));
        return false;
    }
    
    return true;
}


