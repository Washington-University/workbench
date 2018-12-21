
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
 * Move the mouse to the tab bar's tab with the given tab index
 *
 * @param tabBar
 *     The tab bar
 * @param tabIndex
 *     Index of the tab
 */
void
WuQMacroExecutor::moveMouseToTabBarTab(QTabBar* tabBar,
                                       const int32_t tabIndex) const
{
    QRect tabRect = tabBar->tabRect(tabIndex);
    if (tabRect.isNull()) {
        moveMouseToWidget(tabBar);
    }
    else {
        moveMouseToWidgetImplementation(tabBar,
                                        -1,
                                        -1,
                                        &tabRect,
                                        true);
    }
}

/**
 * Move the mouse around the given widget
 *
 * @param moveToObject
 *     Object to where mouse should be moved
 * @param highlightFlag
 *     If true, highlight mouse location by moving mouse in 
 *     a circular orientation
 */
void
WuQMacroExecutor::moveMouseToWidget(QObject* moveToObject,
                                    const bool highlightFlag) const
{
    moveMouseToWidgetImplementation(moveToObject,
                                    -1,
                                    -1,
                                    NULL,
                                    highlightFlag);
}

/**
 * Move the mouse to and around the given widget
 * at the given X/Y
 *
 * @param moveToObject
 *     Object to where mouse should be moved
 * @param x
 *     Move to this X in widget
 * @param y
 *     Move to this Y in widget
 * @param highlightFlag
 *     If true, highlight mouse location by moving mouse in
 *     a circular orientation
 */
void
WuQMacroExecutor::moveMouseToWidgetXY(QObject* moveToObject,
                                      const int x,
                                      const int y,
                                      const bool highlightFlag) const
{
    moveMouseToWidgetImplementation(moveToObject,
                                    x,
                                    y,
                                    NULL,
                                    highlightFlag);
}


/**
 * Move the mouse around the given widget.  If the given X/Y are
 * non-negative, mouse is moved to that location instead of center
 * of widget
 *
 * @param moveToObject
 *     Object to where mouse should be moved
 * @param x
 *     Move to this X in widget
 * @param y
 *     Move to this Y in widget
 * @param objectRect
 *     Optional, if not NULL, rectangle of object used for positioning mouse
 * @param highlightFlag
 *     If true, highlight mouse location by moving mouse in
 *     a circular orientation
 */
void
WuQMacroExecutor::moveMouseToWidgetImplementation(QObject* moveToObject,
                                                  const int x,
                                                  const int y,
                                                  const QRect* objectRect,
                                                  const bool highlightFlag) const
{
    if ( ! m_runOptions.isShowMouseMovement()) {
        return;
    }
    CaretAssert(moveToObject);
    const QString objectName = moveToObject->objectName();
    
    /*
     * Object may not be a widget so find ancestor
     * that is a widget
     */
    bool usingParentFlag(false);
    QWidget* moveToWidget(NULL);
    while ((moveToWidget == NULL)
           && (moveToObject != NULL)) {
        moveToWidget = qobject_cast<QWidget*>(moveToObject);
        if (moveToWidget == NULL) {
            moveToObject = moveToObject->parent();
            usingParentFlag = true;
        }
    }
    
    if (moveToWidget == NULL) {
        return;
        
    }
    
    if (usingParentFlag) {
        std::cout << "For mouse, using parent " << moveToWidget->objectName()
        << " for object " << objectName << std::endl;
    }
    
    CaretAssert(moveToWidget);
    
    const QRect widgetRect = ((objectRect != NULL)
                              ? *objectRect
                              : moveToWidget->rect());
    QPoint widgetPoint = widgetRect.center();
    if ((x >= 0) && (y >= 0)) {
        widgetPoint.setX(x);
        widgetPoint.setY(y);
    }
    const QPoint windowPoint = moveToWidget->mapToGlobal(widgetPoint);
    QCursor::setPos(windowPoint);
    SystemUtilities::sleepSeconds(0.025);
    
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
 * Find an object, by name, in the parent objects
 *
 * @param objectName
 *     Name of object
 * @return
 *     Pointer to object with name or NULL if not found
 */
QObject*
WuQMacroExecutor::findObjectByName(const QString& objectName) const
{
    QObject* object(NULL);
    
    for (auto po : m_parentObjects) {
        object = po->findChild<QObject*>(objectName);
        if (object != NULL) {
            break;
        }
    }
    
    return object;
}

/**
 * Run the commands in the given macro.
 *
 * @param macro
 *    Macro that is run
 * @param topLevelObject
 *     Top level object for finding children objects
 * @param otherObjectParents
 *    Additional objects that are searched for objects contained
 *    in the macro commands
 * @param executorOptions
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
                           const WuQMacroExecutorOptions* executorOptions,
                           QString& errorMessageOut) const
{
    CaretAssert(macro);
    CaretAssert(executorOptions);
    
    m_parentObjects.clear();
    m_parentObjects.push_back(window);
    m_parentObjects.insert(m_parentObjects.end(),
                           otherObjectParents.begin(), otherObjectParents.end());
    
    m_runOptions = *executorOptions;
    
    errorMessageOut.clear();
    
    const int32_t numberOfMacroCommands = macro->getNumberOfMacroCommands();
    for (int32_t i = 0; i < numberOfMacroCommands; i++) {
        const WuQMacroCommand* mc = macro->getMacroCommandAtIndex(i);
        CaretAssert(mc);
        
        const QString objectName(mc->getObjectName());
        QObject* object = findObjectByName(objectName);
        if (object == NULL) {
            errorMessageOut.append("Unable to find object named "
                                   + objectName
                                   + "\n");
            if (m_runOptions.isStopOnError()) {
                return false;
            }
            continue;
        }

        if (object->signalsBlocked()) {
            errorMessageOut.append("Object named "
                                   + objectName
                                   + " has signals blocked");
            if (m_runOptions.isStopOnError()) {
                return false;
            }
            continue;
        }
        
        QString commandErrorMessage;
        if ( ! runMacroCommand(mc,
                               object,
                               commandErrorMessage)) {
            errorMessageOut.append(commandErrorMessage + "\n");
            if (m_runOptions.isStopOnError()) {
                return false;
            }
        }
        
        QGuiApplication::processEvents();
        if (mc->getClassType() != WuQMacroClassTypeEnum::MOUSE_USER_EVENT) {
            if (m_runOptions.getSecondsDelayBetweenCommands() > 0.0) {
                SystemUtilities::sleepSeconds(m_runOptions.getSecondsDelayBetweenCommands());
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
    
    QString objectErrorMessage;
    bool notFoundFlag(false);
    switch (classType) {
        case WuQMacroClassTypeEnum::ACTION:
        {
            QAction* action = qobject_cast<QAction*>(object);
            if (action != NULL) {
                moveMouseToWidget(action);
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
                moveMouseToWidget(actionGroup);
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
                    objectErrorMessage = ("For QActionGroup \""
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
                    moveMouseToWidget(textButton);
                    signalEmitter.emitQButtonGroupSignal(buttonGroup,
                                                        textButton->text());
                }
                else if (indexButton != NULL) {
                    moveMouseToWidget(indexButton);
                    signalEmitter.emitQButtonGroupSignal(buttonGroup,
                                                        indexButton->text());
                }
                else {
                    objectErrorMessage = ("For QButtonGroup \""
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
                moveMouseToWidget(checkBox);
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
                moveMouseToWidget(comboBox);
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
                    objectErrorMessage = ("For ComboBox \""
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
                moveMouseToWidget(doubleSpinBox);
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
                moveMouseToWidget(lineEdit);
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

                moveMouseToWidget(listWidget);
                
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
                    objectErrorMessage = ("For QListWidget \""
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
                
                moveMouseToWidget(menu);
                
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
                    objectErrorMessage = ("For QMenu \""
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
                    
                    moveMouseToWidgetXY(widget,
                                        adjustedLocalX,
                                        adjustedLocalY,
                                        false);
                    
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
                moveMouseToWidget(pushButton);
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
                moveMouseToWidget(radioButton);
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
                moveMouseToWidget(slider);
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
                moveMouseToWidget(spinBox);
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
                const int32_t tabIndex = dataValue.toInt();
                if (tabBar->isTabEnabled(tabIndex)) {
                    moveMouseToTabBarTab(tabBar, tabIndex);
                    WuQMacroSignalEmitter signalEmitter;
                    signalEmitter.emitQTabBarSignal(tabBar,
                                                    tabIndex);
                }
                else {
                    objectErrorMessage = ("QTabWidget \""
                                          + object->objectName()
                                          + "\", tab \""
                                          + QString::number(tabIndex + 1)
                                          + "\" with text \""
                                          + tabBar->tabText(tabIndex)
                                          + "\" is disabled");
                }
                
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
                QTabBar* tabBar = tabWidget->tabBar();
                CaretAssert(tabBar);
                const int32_t tabIndex = dataValue.toInt();
                if (tabWidget->isTabEnabled(tabIndex)) {
                    moveMouseToTabBarTab(tabBar,
                                         tabIndex);
                    WuQMacroSignalEmitter signalEmitter;
                    signalEmitter.emitQTabWidgetSignal(tabWidget,
                                                       tabIndex);
                }
                else {
                    objectErrorMessage = ("QTabWidget \""
                                          + object->objectName()
                                          + "\", tab \""
                                          + QString::number(tabIndex + 1)
                                          + "\" with text \""
                                          + tabWidget->tabText(tabIndex)
                                          + "\" is disabled");
                }
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
                moveMouseToWidget(toolButton);
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
    else if ( ! objectErrorMessage.isEmpty()) {
        errorMessageOut = objectErrorMessage;
        return false;
    }
    
    return true;
}


