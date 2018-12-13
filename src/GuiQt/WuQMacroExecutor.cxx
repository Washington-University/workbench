
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
#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
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
#include "CaretLogger.h"
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
 *    The window from which macro was launched and is searched for 
 *    objects contained in the macro commands
 * @param 
 *    Additional objects that are searched for objects contained
 *    in the macro commands
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
                           std::vector<QObject*>& otherObjectParents,
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
            object = qApp->findChild<QObject*>(objectName);
            if (object != NULL) {
                CaretLogWarning("Found "
                                + objectName
                                + " in the app");
            }
            else {
                for (auto other : otherObjectParents) {
                    object = other->findChild<QObject*>(objectName);
                    if (object != NULL) {
                        break;
                    }
                }
                    const bool extraFindFlag(false);
                    if (extraFindFlag) {
                        static bool firstTime = true;
                        if (firstTime) {
                            QWidgetList topLevelWs = QApplication::topLevelWidgets();
                            std::cout << "Top level widget count: " << topLevelWs.size() << std::endl;
                            for (QObject* w : topLevelWs) {
                                if (w->objectName() == objectName) {
                                    std::cout << objectName << " is a top level widget" << std::endl;
                                    break;
                                }
                                object = w->findChild<QObject*>(objectName);
                                if (object != NULL) {
                                    QString parentName;
                                    QObject* parent = object->parent();
                                    if (parent != NULL) {
                                        parentName = parent->objectName();
                                    }
                                    std::cout << "Found object in top level widget: \"" << parentName << "\"" << std::endl;
                                    break;
                                }
                            }
                        }
                    }
                
                if (object == NULL) {
                    errorMessageOut.append("Unable to find object named "
                                           + objectName
                                           + "\n");
                    if (options.m_stopOnErrorFlag) {
                        return false;
                    }
                    continue;
                }
            }
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
        
        const WuQMacroClassTypeEnum::Enum classType = mc->getClassType();
        const bool mouseEventFlag = (classType == WuQMacroClassTypeEnum::MOUSE_USER_EVENT);
        
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
    
    const WuQMacroClassTypeEnum::Enum classType = macroCommand->getClassType();
    const WuQMacroDataValueTypeEnum::Enum dataValueType = macroCommand->getDataType();
    const QVariant dataValue = macroCommand->getDataValue();
    const WuQMacroDataValueTypeEnum::Enum dataValueTypeTwo = macroCommand->getDataTypeTwo();
    const QVariant dataValueTwo = macroCommand->getDataValueTwo();
    
    QString valueNotFoundErrorMessage;
    bool notFoundFlag(false);
    switch (classType) {
        case WuQMacroClassTypeEnum::ACTION:
        {
            QAction* action = qobject_cast<QAction*>(object);
            if (action != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQActionSignal(action,
                                                dataValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::ACTION_GROUP:
        {
            QActionGroup* actionGroup = qobject_cast<QActionGroup*>(object);
            if (actionGroup != NULL) {
                CaretAssert(dataValueType    == WuQMacroDataValueTypeEnum::INTEGER);
                CaretAssert(dataValueTypeTwo == WuQMacroDataValueTypeEnum::STRING);
                
                QAction* textAction(NULL);
                QAction* indexAction(NULL);
                QList<QAction*> actions = actionGroup->actions();
                for (int32_t i = 0; i < actions.size(); i++) {
                    QAction* actionAtIndex = actions.at(i);
                    if (actionAtIndex->text() == dataValueTwo.toString()) {
                        textAction = actionAtIndex;
                    }
                    if (dataValue.toInt() == i) {
                        indexAction = actionAtIndex;
                    }
                }
                
                WuQMacroSignalEmitter signalEmitter;
                if (textAction != NULL) {
                    signalEmitter.emitActionGroupSignal(actionGroup,
                                                        textAction->text());
                }
                else if (indexAction != NULL) {
                    signalEmitter.emitActionGroupSignal(actionGroup,
                                                        indexAction->text());
                }
                else {
                    valueNotFoundErrorMessage = ("For QActionGroup \""
                                                 + object->objectName()
                                                 + "\", unable to find action with text \""
                                                 + dataValueTwo.toString()
                                                 + "\" or index="
                                                 + dataValue.toInt());
                }
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::BUTTON_GROUP:
        {
            QButtonGroup* buttonGroup = qobject_cast<QButtonGroup*>(object);
            if (buttonGroup != NULL) {
                CaretAssert(dataValueType    == WuQMacroDataValueTypeEnum::INTEGER);
                CaretAssert(dataValueTypeTwo == WuQMacroDataValueTypeEnum::STRING);
                
                QAbstractButton* textButton(NULL);
                QAbstractButton* indexButton(NULL);
                QList<QAbstractButton*> buttons = buttonGroup->buttons();
                for (int32_t i = 0; i < buttons.size(); i++) {
                    QAbstractButton* buttonAtIndex = buttons.at(i);
                    if (buttonAtIndex->text() == dataValueTwo.toString()) {
                        textButton = buttonAtIndex;
                    }
                    if (dataValue.toInt() == i) {
                        indexButton = buttonAtIndex;
                    }
                }
                
                WuQMacroSignalEmitter signalEmitter;
                if (textButton != NULL) {
                    signalEmitter.emitQButtonGroupSignal(buttonGroup,
                                                        textButton->text());
                }
                else if (indexButton != NULL) {
                    signalEmitter.emitQButtonGroupSignal(buttonGroup,
                                                        indexButton->text());
                }
                else {
                    valueNotFoundErrorMessage = ("For QButtonGroup \""
                                                 + object->objectName()
                                                 + "\", unable to find button with text \""
                                                 + dataValueTwo.toString()
                                                 + "\" or index="
                                                 + dataValue.toInt());
                }
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::CHECK_BOX:
        {
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
            if (checkBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQCheckBoxSignal(checkBox,
                                                  dataValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::COMBO_BOX:
        {
            QComboBox* comboBox = qobject_cast<QComboBox*>(object);
            if (comboBox != NULL) {
                CaretAssert(dataValueType    == WuQMacroDataValueTypeEnum::INTEGER);
                CaretAssert(dataValueTypeTwo == WuQMacroDataValueTypeEnum::STRING);
                int textIndex(-1);
                int indexIndex(-1);
                for (int32_t i = 0; i < comboBox->count(); i++) {
                    if (comboBox->itemText(i) == dataValueTwo.toString()) {
                        textIndex = i;
                    }
                    else if (i == dataValue.toInt()) {
                        indexIndex = i;
                    }
                }
                WuQMacroSignalEmitter signalEmitter;
                if (textIndex >= 0) {
                    signalEmitter.emitQComboBoxSignal(comboBox,
                                                      textIndex);
                }
                else if (indexIndex >= 0) {
                    signalEmitter.emitQComboBoxSignal(comboBox,
                                                      indexIndex);
                }
                else {
                    valueNotFoundErrorMessage = ("For ComboBox \""
                                                 + object->objectName()
                                                 + "\", unable to find item with text \""
                                                 + dataValueTwo.toString()
                                                 + "\" or index="
                                                 + dataValue.toInt());
                }
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::DOUBLE_SPIN_BOX:
        {
            QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(object);
            if (doubleSpinBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::FLOAT);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQDoubleSpinBoxSignal(doubleSpinBox,
                                                       dataValue.toDouble());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case WuQMacroClassTypeEnum::LINE_EDIT:
        {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
            if (lineEdit != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::STRING);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQLineEditSignal(lineEdit,
                                                  dataValue.toString());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::LIST_WIDGET:
        {
            QListWidget* listWidget = qobject_cast<QListWidget*>(object);
            if (listWidget != NULL) {
                CaretAssert(dataValueType    == WuQMacroDataValueTypeEnum::INTEGER);
                CaretAssert(dataValueTypeTwo == WuQMacroDataValueTypeEnum::STRING);
                
                QListWidgetItem* textItem(NULL);
                QListWidgetItem* indexItem(NULL);
                for (int32_t i = 0; i < listWidget->count(); i++) {
                    QListWidgetItem* itemAtIndex = listWidget->item(i);
                    if (itemAtIndex->text() == dataValueTwo.toString()) {
                        textItem = itemAtIndex;
                    }
                    if (dataValue.toInt() == i) {
                        indexItem = itemAtIndex;
                    }
                }
                
                WuQMacroSignalEmitter signalEmitter;
                if (textItem != NULL) {
                    signalEmitter.emitQListWidgetSignal(listWidget,
                                                        textItem->text());
                }
                else if (indexItem != NULL) {
                    signalEmitter.emitQListWidgetSignal(listWidget,
                                                        indexItem->text());
                }
                else {
                    valueNotFoundErrorMessage = ("For QListWidget \""
                                                 + object->objectName()
                                                 + "\", unable to find item with text \""
                                                 + dataValueTwo.toString()
                                                 + "\" or index="
                                                 + dataValue.toInt());
                }
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::MENU:
        {
            QMenu* menu = qobject_cast<QMenu*>(object);
            if (menu != NULL) {
                CaretAssert(dataValueType    == WuQMacroDataValueTypeEnum::INTEGER);
                CaretAssert(dataValueTypeTwo == WuQMacroDataValueTypeEnum::STRING);
                
                QAction* textAction(NULL);
                QAction* indexAction(NULL);
                QList<QAction*> actions = menu->actions();
                for (int32_t i = 0; i < actions.size(); i++) {
                    QAction* actionAtIndex = actions.at(i);
                    if (actionAtIndex->text() == dataValueTwo.toString()) {
                        textAction = actionAtIndex;
                    }
                    if (dataValue.toInt() == i) {
                        indexAction = actionAtIndex;
                    }
                }
                
                WuQMacroSignalEmitter signalEmitter;
                if (textAction != NULL) {
                    signalEmitter.emitQMenuSignal(menu,
                                                  textAction->text());
                }
                else if (indexAction != NULL) {
                    signalEmitter.emitQMenuSignal(menu,
                                                  indexAction->text());
                }
                else {
                    valueNotFoundErrorMessage = ("For QMenu \""
                                                 + object->objectName()
                                                 + "\", unable to find action with text \""
                                                 + dataValueTwo.toString()
                                                 + "\" or index="
                                                 + dataValue.toInt());
                }
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::MOUSE_USER_EVENT:
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
        case WuQMacroClassTypeEnum::PUSH_BUTTON:
        {
            QPushButton* pushButton = qobject_cast<QPushButton*>(object);
            if (pushButton != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQPushButtonSignal(pushButton,
                                                    dataValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::RADIO_BUTTON:
        {
            QRadioButton* radioButton = qobject_cast<QRadioButton*>(object);
            if (radioButton != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQRadioButtonSignal(radioButton,
                                                     dataValue.toBool());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::SLIDER:
        {
            QSlider* slider = qobject_cast<QSlider*>(object);
            if (slider != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQSliderSignal(slider,
                                                dataValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::SPIN_BOX:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
            if (spinBox != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQSpinBoxSignal(spinBox,
                                                 dataValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::TAB_BAR:
        {
            QTabBar* tabBar = qobject_cast<QTabBar*>(object);
            if (tabBar != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQTabBarSignal(tabBar,
                                                dataValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::TAB_WIDGET:
        {
            QTabWidget* tabWidget = qobject_cast<QTabWidget*>(object);
            if (tabWidget != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::INTEGER);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQTabWidgetSignal(tabWidget,
                                                   dataValue.toInt());
            }
            else {
                notFoundFlag = true;
            }
        }
            break;
        case WuQMacroClassTypeEnum::TOOL_BUTTON:
        {
            QToolButton* toolButton = qobject_cast<QToolButton*>(object);
            if (toolButton != NULL) {
                CaretAssert(dataValueType == WuQMacroDataValueTypeEnum::BOOLEAN);
                WuQMacroSignalEmitter signalEmitter;
                signalEmitter.emitQToolButtonSignal(toolButton,
                                                    dataValue.toBool());
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
                           + WuQMacroClassTypeEnum::toGuiName(classType));
        return false;
    }
    else if ( ! valueNotFoundErrorMessage.isEmpty()) {
        errorMessageOut = valueNotFoundErrorMessage;
        return false;
    }
    
    return true;
}


