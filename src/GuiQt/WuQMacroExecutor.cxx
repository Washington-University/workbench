
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
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"
#include "WuQMacroManager.h"
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
    QWidget* moveToWidget(NULL);
    while ((moveToWidget == NULL)
           && (moveToObject != NULL)) {
        moveToWidget = qobject_cast<QWidget*>(moveToObject);
        if (moveToWidget == NULL) {
            moveToObject = moveToObject->parent();
        }
    }
    
    if (moveToWidget == NULL) {
        return;
    }
    
    /*
     * Test visibility of widget.
     * Note: Cannot use isHidden(), see Qt documentation.
     */
    if ( ! moveToWidget->isVisible()) {
        return;
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
 * @param macroCommandToStartAt
 *     Macro command at which execution should begin.  If NULL, start
 *     with the first command in the macro
 * @param macroCommandToStopAfter
 *     Macro command that the executor may stop after, depending upon options
 * @param window
 *     Widget for parent
 * @param otherObjectParents
 *    Additional objects that are searched for objects contained
 *    in the macro commands
 * @param executorMonitor
 *    The executor monitor
 * @param executorOptions
 *    Executor options
 * @param errorMessageOut
 *    Output containing any error messages
 * @return
 *    True if the macro completed without errors, else false.
 */
bool
WuQMacroExecutor::runMacro(const WuQMacro* macro,
                           const WuQMacroCommand* macroCommandToStartAt,
                           const WuQMacroCommand* macroCommandToStopAfter,
                           QWidget* window,
                           std::vector<QObject*>& otherObjectParents,
                           const WuQMacroExecutorMonitor* executorMonitor,
                           const WuQMacroExecutorOptions* executorOptions,
                           QString& errorMessageOut) const
{
    const bool result = runMacroPrivate(macro,
                                        macroCommandToStartAt,
                                        macroCommandToStopAfter,
                                        window,
                                        otherObjectParents,
                                        executorMonitor,
                                        executorOptions,
                                        errorMessageOut);
    
    return result;
}

/**
 * Run the commands in the given macro.
 *
 * @param macro
 *    Macro that is run
 * @param macroCommandToStartAt
 *     Macro command at which execution should begin.  If NULL, start
 *     with the first command in the macro
 * @param macroCommandToStopAfter
 *     Macro command that the executor may stop after, depending upon options
 * @param window
 *     Widget for parent
 * @param otherObjectParents
 *    Additional objects that are searched for objects contained
 *    in the macro commands
 * @param executorMonitor
 *    The executor monitor
 * @param executorOptions
 *    Executor options
 * @param errorMessageOut
 *    Output containing any error messages
 * @return
 *    True if the macro completed without errors, else false.
 */
bool
WuQMacroExecutor::runMacroPrivate(const WuQMacro* macro,
                                  const WuQMacroCommand* macroCommandToStartAt,
                                  const WuQMacroCommand* macroCommandToStopAfter,
                                  QWidget* window,
                                  std::vector<QObject*>& otherObjectParents,
                                  const WuQMacroExecutorMonitor* executorMonitor,
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

    int32_t startCommandIndex(0);
    if (macroCommandToStartAt != NULL) {
        const int32_t commandIndex = macro->getIndexOfMacroCommand(macroCommandToStartAt);
        if (commandIndex < 0) {
            errorMessageOut = ("Macro command to start at not found in macro \""
                               + macroCommandToStartAt->getDescriptiveName()
                               + "\"");
            return false;
        }
        startCommandIndex = commandIndex;
    }
    
    const int32_t numberOfMacroCommands = macro->getNumberOfMacroCommands();
    for (int32_t i = startCommandIndex; i < numberOfMacroCommands; i++) {
        const WuQMacroCommand* mc = macro->getMacroCommandAtIndex(i);
        CaretAssert(mc);
        
        emit macroCommandStarting(macro,
                                  mc);
        
        const QString objectName(mc->getObjectName());
        
        bool requiresObjectFlag(false);
        switch (mc->getCommandType()) {
            case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
                break;
            case WuQMacroCommandTypeEnum::MOUSE:
                requiresObjectFlag = true;
                break;
            case WuQMacroCommandTypeEnum::WIDGET:
                requiresObjectFlag = true;
                break;
        }
        
        QObject* object = findObjectByName(objectName);
        if (requiresObjectFlag) {
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
        }
        
        bool allowDelayBeforeCommandFlag(false);
        emit macroCommandAboutToStart(window,
                                      mc,
                                      executorOptions,
                                      allowDelayBeforeCommandFlag);
        /*
         * May get stopped by user in macroCommandAboutToStart()
         */
        switch (executorMonitor->getMode()) {
            case WuQMacroExecutorMonitor::Mode::PAUSE:
                break;
            case WuQMacroExecutorMonitor::Mode::RUN:
                break;
            case WuQMacroExecutorMonitor::Mode::STOP:
                errorMessageOut = executorMonitor->getStoppedByUserMessage();
                return false;
                break;
        }

        if (executorOptions->isIgnoreDelaysAndDurations()) {
            allowDelayBeforeCommandFlag = false;
        }
        if (allowDelayBeforeCommandFlag) {
            performCommandDelay(mc);
        }
        
        QString commandErrorMessage;
        bool successFlag(false);
        switch (mc->getCommandType()) {
            case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
                successFlag = WuQMacroManager::instance()->executeCustomOperationMacroCommand(window,
                                                                                              executorMonitor,
                                                                                              executorOptions,
                                                                                              mc,
                                                                                              commandErrorMessage);

                break;
            case WuQMacroCommandTypeEnum::MOUSE:
            {
                CaretAssert(object);
                bool notFoundFlag(false);
                successFlag = runMouseCommand(mc,
                                              object,
                                              commandErrorMessage,
                                              notFoundFlag);
            }
                break;
            case WuQMacroCommandTypeEnum::WIDGET:
                CaretAssert(object);
                successFlag = runMacroCommand(window,
                                              executorMonitor,
                                              mc,
                                              object,
                                              commandErrorMessage);
                break;
        }
        
        if ( ! successFlag) {
            errorMessageOut.append(commandErrorMessage + "\n");
            if (m_runOptions.isStopOnError()) {
                return false;
            }
        }
        
        
        QGuiApplication::processEvents();
        if (m_stopFlag) {
            errorMessageOut = "OBSOLETE: Macro stopped at request of user";
            return false;
        }

        bool allowDelayAfterCommandFlag(false);
        emit macroCommandHasCompleted(window,
                                      mc,
                                      executorOptions,
                                      allowDelayAfterCommandFlag);
        
        QGuiApplication::processEvents();
        if (mc == macroCommandToStopAfter) {
            errorMessageOut = ("Macro stopped after "
                               + mc->getDescriptiveName());
            return false;
        }

        const bool stopFlag = executorMonitor->testForStop();
        if (stopFlag) {
            errorMessageOut = executorMonitor->getStoppedByUserMessage();
            return false;
        }
        
        if (executorOptions->isIgnoreDelaysAndDurations()) {
            allowDelayAfterCommandFlag = false;
        }
        if (allowDelayAfterCommandFlag) {
            performCommandDelay(mc);
        }
        
        QGuiApplication::processEvents();
    }
    
    macroCommandStarting(macro,
                         NULL);
    
    return (errorMessageOut.isEmpty());
}

/**
 * Perform delay for the given macro command
 *
 * @param mc
 *     The macro command
 */
void
WuQMacroExecutor::performCommandDelay(const WuQMacroCommand* mc) const
{
    if (mc->getCommandType() != WuQMacroCommandTypeEnum::MOUSE) {
        if (mc->getDelayInSeconds() > 0.0) {
            SystemUtilities::sleepSeconds(mc->getDelayInSeconds());
        }
    }
}

/**
 * Stop the macro that is executing
 */
void
WuQMacroExecutor::stopMacro()
{
    m_stopFlag = true;
}

/**
 * Run the commands in the given macro.
 *
 * @param parentWidget
 *    Parent widget for dialogs
 * @param executorMonitor
 *    The executor monitor
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *    Object on which command is run
 * @param errorMessageOut
 *    Output containing any error messages
 * @return
 *    True if the macro completed without errors, else false.
 */
bool
WuQMacroExecutor::runMacroCommand(QWidget* /*parentWidget*/,
                                  const WuQMacroExecutorMonitor* /*executorMonitor*/,
                                  const WuQMacroCommand* macroCommand,
                                  QObject* object,
                                  QString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    CaretAssert(macroCommand);
    CaretAssert(object);
    
    const WuQMacroWidgetTypeEnum::Enum classType = macroCommand->getWidgetType();
    
    QString objectErrorMessage;
    bool notFoundFlag(false);
    switch (classType) {
        case WuQMacroWidgetTypeEnum::ACTION:
            runActionCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::ACTION_CHECKABLE:
            runActionCheckableCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::ACTION_GROUP:
            runActionGroupCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::BUTTON_GROUP:
            runButtonGroupCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::CHECK_BOX:
            runCheckBoxCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::COMBO_BOX:
            runComboBoxCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::DOUBLE_SPIN_BOX:
            runDoubleSpinBoxCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case WuQMacroWidgetTypeEnum::LINE_EDIT:
            runLineEditCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::LIST_WIDGET:
            runListWidgetCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
             break;
        case WuQMacroWidgetTypeEnum::MENU:
            runMenuCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::PUSH_BUTTON:
            runPushButtonCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::PUSH_BUTTON_CHECKABLE:
            runPushButtonCheckableCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::RADIO_BUTTON:
            runRadioButtonCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::SLIDER:
            runSliderCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::SPIN_BOX:
            runSpinBoxCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::TAB_BAR:
            runTabBarCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::TAB_WIDGET:
            runTabWidgetCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::TOOL_BUTTON:
            runToolButtonCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
        case WuQMacroWidgetTypeEnum::TOOL_BUTTON_CHECKABLE:
            runToolButtonCheckableCommand(macroCommand, object, objectErrorMessage, notFoundFlag);
            break;
    }
    
    if (notFoundFlag) {
        errorMessageOut = ("ERROR: Unable to cast object named "
                           + object->objectName()
                           + " to "
                           + WuQMacroWidgetTypeEnum::toGuiName(classType));
        return false;
    }
    else if ( ! objectErrorMessage.isEmpty()) {
        errorMessageOut = objectErrorMessage;
        return false;
    }
    
    return true;
}

/**
 * Run a non-checkable QAction selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runActionCommand(const WuQMacroCommand* /*macroCommand*/,
                                   QObject* object,
                                   QString& /*errorMessageOut*/,
                                   bool& castFailureFlagOut) const
{
    QAction* action = qobject_cast<QAction*>(object);
    if (action != NULL) {
        moveMouseToWidget(action);
        
        /*
         * Always emit true for a non-checkable action
         */
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQActionSignal(action,
                                        true);
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a checkable QAction selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runActionCheckableCommand(const WuQMacroCommand* macroCommand,
                                            QObject* object,
                                            QString& /*errorMessageOut*/,
                                            bool& castFailureFlagOut) const
{
    QAction* action = qobject_cast<QAction*>(object);
    if (action != NULL) {
        moveMouseToWidget(action);
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::BOOLEAN);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQActionSignal(action,
                                        parameterOne->getValue().toBool());
    }
    else {
        castFailureFlagOut = true;
    }
}


/**
 * Run a action group selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runActionGroupCommand(const WuQMacroCommand* macroCommand,
                                        QObject* object,
                                        QString& errorMessageOut,
                                        bool& castFailureFlagOut) const
{
    QActionGroup* actionGroup = qobject_cast<QActionGroup*>(object);
    if (actionGroup != NULL) {
        moveMouseToWidget(actionGroup);
        
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);
        CaretAssert(parameterTwo);
        
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::STRING);
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);

        QAction* textAction(NULL);
        QAction* indexAction(NULL);
        QList<QAction*> actions = actionGroup->actions();
        for (int32_t i = 0; i < actions.size(); i++) {
            QAction* actionAtIndex = actions.at(i);
            if (actionAtIndex->text() == parameterOne->getValue().toString()) {
                textAction = actionAtIndex;
            }
            if (parameterTwo->getValue().toInt() == i) {
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
            errorMessageOut = ("For QActionGroup \""
                                  + object->objectName()
                                  + "\", unable to find action with text \""
                                  + parameterOne->getValue().toString()
                                  + "\" or index="
                                  + parameterTwo->getValue().toInt());
        }
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a button group selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runButtonGroupCommand(const WuQMacroCommand* macroCommand,
                                        QObject* object,
                                        QString& errorMessageOut,
                                        bool& castFailureFlagOut) const
{
    QButtonGroup* buttonGroup = qobject_cast<QButtonGroup*>(object);
    if (buttonGroup != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);
        CaretAssert(parameterTwo);

        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::STRING);
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);

        const QVariant dataValue    = parameterOne->getValue();
        const QVariant dataValueTwo = parameterTwo->getValue();
        
        QAbstractButton* textButton(NULL);
        QAbstractButton* indexButton(NULL);
        QList<QAbstractButton*> buttons = buttonGroup->buttons();
        for (int32_t i = 0; i < buttons.size(); i++) {
            QAbstractButton* buttonAtIndex = buttons.at(i);
            if (buttonAtIndex->text() == dataValue.toString()) {
                textButton = buttonAtIndex;
            }
            if (dataValueTwo.toInt() == i) {
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
            errorMessageOut = ("For QButtonGroup \""
                                  + object->objectName()
                                  + "\", unable to find button with text \""
                                  + dataValue.toString()
                                  + "\" or index="
                                  + dataValueTwo.toInt());
        }
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a check box selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runCheckBoxCommand(const WuQMacroCommand* macroCommand,
                                     QObject* object,
                                     QString& /*errorMessageOut*/,
                                     bool& castFailureFlagOut) const
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
    if (checkBox != NULL) {
        moveMouseToWidget(checkBox);
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::BOOLEAN);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQCheckBoxSignal(checkBox,
                                          parameterOne->getValue().toBool());
    }
    else {
        castFailureFlagOut = true;
    }
}


/**
 * Run a combo box selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runComboBoxCommand(const WuQMacroCommand* macroCommand,
                                     QObject* object,
                                     QString& errorMessageOut,
                                     bool& castFailureFlagOut) const
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(object);
    if (comboBox != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);
        CaretAssert(parameterTwo);

        const QVariant dataValue    = parameterOne->getValue();
        const QVariant dataValueTwo = parameterTwo->getValue();
        CaretAssert(parameterOne->getDataType()    == WuQMacroDataValueTypeEnum::STRING);
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        int textIndex(-1);
        int indexIndex(-1);
        for (int32_t i = 0; i < comboBox->count(); i++) {
            if (comboBox->itemText(i) == dataValue.toString()) {
                textIndex = i;
            }
            else if (i == dataValueTwo.toInt()) {
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
            errorMessageOut = ("For ComboBox \""
                                  + object->objectName()
                                  + "\", unable to find item with text \""
                                  + dataValue.toString()
                                  + "\" or index="
                                  + dataValueTwo.toInt());
        }
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a double spin box selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runDoubleSpinBoxCommand(const WuQMacroCommand* macroCommand,
                                          QObject* object,
                                          QString& /*errorMessageOut*/,
                                          bool& castFailureFlagOut) const
{
    QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(object);
    if (doubleSpinBox != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);

        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::FLOAT);
        const QVariant dataValue = parameterOne->getValue();

        moveMouseToWidget(doubleSpinBox);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQDoubleSpinBoxSignal(doubleSpinBox,
                                               dataValue.toDouble());
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a line edit selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runLineEditCommand(const WuQMacroCommand* macroCommand,
                                     QObject* object,
                                     QString& /*errorMessageOut*/,
                                     bool& castFailureFlagOut) const
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
    if (lineEdit != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);

        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::STRING);
        const QVariant dataValue = parameterOne->getValue();

        moveMouseToWidget(lineEdit);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQLineEditSignal(lineEdit,
                                          dataValue.toString());
    }
    else {
        castFailureFlagOut = true;
    }
}


/**
 * Run a list widget selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runListWidgetCommand(const WuQMacroCommand* macroCommand,
                                       QObject* object,
                                       QString& errorMessageOut,
                                       bool& castFailureFlagOut) const
{
    QListWidget* listWidget = qobject_cast<QListWidget*>(object);
    if (listWidget != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);
        CaretAssert(parameterTwo);
 
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::STRING);
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        const QVariant dataValue = parameterOne->getValue();
        const QVariant dataValueTwo = parameterTwo->getValue();
        
        QListWidgetItem* textItem(NULL);
        QListWidgetItem* indexItem(NULL);
        for (int32_t i = 0; i < listWidget->count(); i++) {
            QListWidgetItem* itemAtIndex = listWidget->item(i);
            if (itemAtIndex->text() == dataValue.toString()) {
                textItem = itemAtIndex;
            }
            if (dataValueTwo.toInt() == i) {
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
            errorMessageOut = ("For QListWidget \""
                                  + object->objectName()
                                  + "\", unable to find item with text \""
                                  + dataValue.toString()
                                  + "\" or index="
                                  + dataValueTwo.toInt());
        }
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a menu selection selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runMenuCommand(const WuQMacroCommand* macroCommand,
                                 QObject* object,
                                 QString& errorMessageOut,
                                 bool& castFailureFlagOut) const
{
    QMenu* menu = qobject_cast<QMenu*>(object);
    if (menu != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);

        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::STRING);
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        const QVariant dataValue = parameterOne->getValue();
        const QVariant dataValueTwo = parameterTwo->getValue();
        
        QAction* textAction(NULL);
        QAction* indexAction(NULL);
        QList<QAction*> actions = menu->actions();
        for (int32_t i = 0; i < actions.size(); i++) {
            QAction* actionAtIndex = actions.at(i);
            if (actionAtIndex->text() == dataValue.toString()) {
                textAction = actionAtIndex;
            }
            if (dataValueTwo.toInt() == i) {
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
            errorMessageOut = ("For QMenu \""
                                  + object->objectName()
                                  + "\", unable to find action with text \""
                                  + dataValue.toString()
                                  + "\" or index="
                                  + dataValueTwo.toInt());
        }
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a mouse command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 * @return
 *     True if mouse command was successful
 */
bool
WuQMacroExecutor::runMouseCommand(const WuQMacroCommand* macroCommand,
                                  QObject* object,
                                  QString& errorMessageOut,
                                  bool& /*castFailureFlagOut*/) const
{
    QWidget* widget = qobject_cast<QWidget*>(object);
    if (widget != NULL) {
        WuQMacroMouseEventWidgetInterface* mouseInterface = dynamic_cast<WuQMacroMouseEventWidgetInterface*>(widget);
        if (mouseInterface != NULL) {
            const QSize currentWidgetSize = widget->size();
            const WuQMacroMouseEventInfo* mouseEventInfo = macroCommand->getMouseEventInfo();
            CaretAssert(mouseEventInfo);
            
            const int32_t numXY = mouseEventInfo->getNumberOfLocalXY();
            for (int32_t i = 0; i < numXY; i++) {
                int32_t adjustedLocalX(0);
                int32_t adjustedLocalY(0);
                mouseEventInfo->getLocalPositionRescaledToWidgetSize(currentWidgetSize.width(),
                                                                     currentWidgetSize.height(),
                                                                     mouseEventInfo->getLocalX(i),
                                                                     mouseEventInfo->getLocalY(i),
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
                    case WuQMacroMouseEventTypeEnum::DOUBLE_CLICKED:
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
                QGuiApplication::processEvents();
            }
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
    
    return true;
}

/**
 * Run a pushbutton selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runPushButtonCommand(const WuQMacroCommand* macroCommand,
                                       QObject* object,
                                       QString& /*errorMessageOut*/,
                                       bool& castFailureFlagOut) const
{
    QPushButton* pushButton = qobject_cast<QPushButton*>(object);
    if (pushButton != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);

        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::NONE);
        const QVariant dataValue = parameterOne->getValue();
        
        moveMouseToWidget(pushButton);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQPushButtonSignal(pushButton,
                                            true);
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a checkable pushbutton selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runPushButtonCheckableCommand(const WuQMacroCommand* macroCommand,
                                                QObject* object,
                                                QString& /*errorMessageOut*/,
                                                bool& castFailureFlagOut) const
{
    QPushButton* pushButton = qobject_cast<QPushButton*>(object);
    if (pushButton != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::BOOLEAN);
        const QVariant dataValue = parameterOne->getValue();
        
        moveMouseToWidget(pushButton);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQPushButtonSignal(pushButton,
                                            dataValue.toBool());
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a radio button selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runRadioButtonCommand(const WuQMacroCommand* macroCommand,
                                        QObject* object,
                                        QString& /*errorMessageOut*/,
                                        bool& castFailureFlagOut) const
{
    QRadioButton* radioButton = qobject_cast<QRadioButton*>(object);
    if (radioButton != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        
        /* Type=NONE since radio button is ALWAYS TRUE */
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::NONE);
        const QVariant dataValue = parameterOne->getValue();

        moveMouseToWidget(radioButton);
        WuQMacroSignalEmitter signalEmitter;
        /* Note: Radio buttons are always exclusive so always use a true value */
        signalEmitter.emitQRadioButtonSignal(radioButton,
                                             true);
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a slider selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runSliderCommand(const WuQMacroCommand* macroCommand,
                                   QObject* object,
                                   QString& /*errorMessageOut*/,
                                   bool& castFailureFlagOut) const
{
    QSlider* slider = qobject_cast<QSlider*>(object);
    if (slider != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        const QVariant dataValue = parameterOne->getValue();
        
        moveMouseToWidget(slider);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQSliderSignal(slider,
                                        dataValue.toInt());
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a spin box selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runSpinBoxCommand(const WuQMacroCommand* macroCommand,
                                    QObject* object,
                                    QString& /*errorMessageOut*/,
                                    bool& castFailureFlagOut) const
{
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
    if (spinBox != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        const QVariant dataValue = parameterOne->getValue();

        moveMouseToWidget(spinBox);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQSpinBoxSignal(spinBox,
                                         dataValue.toInt());
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a tab bar selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runTabBarCommand(const WuQMacroCommand* macroCommand,
                                   QObject* object,
                                   QString& errorMessageOut,
                                   bool& castFailureFlagOut) const
{
    QTabBar* tabBar = qobject_cast<QTabBar*>(object);
    if (tabBar != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);
        
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        const QVariant dataValue = parameterOne->getValue();
        const WuQMacroDataValueTypeEnum::Enum dataValueType = parameterOne->getDataType();
        const QVariant dataValueTwo = parameterTwo->getValue();
        
        int32_t tabIndex = dataValueTwo.toInt();
        if (dataValueType == WuQMacroDataValueTypeEnum::STRING) {
            /*
             * Allow tab name to override tab index
             */
            const QString tabName = dataValue.toString();
            if ( ! tabName.isEmpty()) {
                for (int32_t i = 0; i < tabBar->count(); i++) {
                    if (tabName == tabBar->tabText(i)) {
                        tabIndex = i;
                        break;
                    }
                }
            }
        }
        if (tabBar->isTabEnabled(tabIndex)) {
            moveMouseToTabBarTab(tabBar, tabIndex);
            WuQMacroSignalEmitter signalEmitter;
            signalEmitter.emitQTabBarSignal(tabBar,
                                            tabIndex);
        }
        else {
            errorMessageOut = ("QTabWidget \""
                                  + object->objectName()
                                  + "\", tab \""
                                  + QString::number(tabIndex + 1)
                                  + "\" with text \""
                                  + tabBar->tabText(tabIndex)
                                  + "\" is disabled");
        }
        
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a tab widget selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runTabWidgetCommand(const WuQMacroCommand* macroCommand,
                                      QObject* object,
                                      QString& errorMessageOut,
                                      bool& castFailureFlagOut) const
{
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(object);
    if (tabWidget != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 1);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        const WuQMacroCommandParameter* parameterTwo = macroCommand->getParameterAtIndex(1);
        
        CaretAssert(parameterTwo->getDataType() == WuQMacroDataValueTypeEnum::INTEGER);
        const QVariant dataValue = parameterOne->getValue();
        const WuQMacroDataValueTypeEnum::Enum dataValueType = parameterOne->getDataType();
        const QVariant dataValueTwo = parameterTwo->getValue();
        
        QTabBar* tabBar = tabWidget->tabBar();
        CaretAssert(tabBar);
        int32_t tabIndex = dataValueTwo.toInt();
        if (dataValueType == WuQMacroDataValueTypeEnum::STRING) {
            /*
             * Allow tab name to override tab index
             */
            const QString tabName = dataValue.toString();
            if ( ! tabName.isEmpty()) {
                for (int32_t i = 0; i < tabWidget->count(); i++) {
                    if (tabName == tabWidget->tabText(i)) {
                        tabIndex = i;
                        break;
                    }
                }
            }
        }
        
        if (tabWidget->isTabEnabled(tabIndex)) {
            moveMouseToTabBarTab(tabBar,
                                 tabIndex);
            WuQMacroSignalEmitter signalEmitter;
            signalEmitter.emitQTabWidgetSignal(tabWidget,
                                               tabIndex);
        }
        else {
            errorMessageOut = ("QTabWidget \""
                                  + object->objectName()
                                  + "\", tab \""
                                  + QString::number(tabIndex + 1)
                                  + "\" with text \""
                                  + tabWidget->tabText(tabIndex)
                                  + "\" is disabled");
        }
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a toolbutton selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runToolButtonCommand(const WuQMacroCommand* macroCommand,
                                       QObject* object,
                                       QString& /*errorMessageOut*/,
                                       bool& castFailureFlagOut) const
{
    QToolButton* toolButton = qobject_cast<QToolButton*>(object);
    if (toolButton != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::NONE);
        const QVariant dataValue = parameterOne->getValue();
        
        moveMouseToWidget(toolButton);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQToolButtonSignal(toolButton,
                                            true);
    }
    else {
        castFailureFlagOut = true;
    }
}

/**
 * Run a checkable toolbutton selection command
 *
 * @param macroCommand
 *    Macro command that is run
 * @param object
 *     The object that is cast to specific object/widget
 * @param errorMessageOut
 *     Error message from execution
 * @param castFailureFlagOut
 *     Set to true if unable to cast object to widget type
 */
void
WuQMacroExecutor::runToolButtonCheckableCommand(const WuQMacroCommand* macroCommand,
                                                QObject* object,
                                                QString& /*errorMessageOut*/,
                                                bool& castFailureFlagOut) const
{
    QToolButton* toolButton = qobject_cast<QToolButton*>(object);
    if (toolButton != NULL) {
        CaretAssert(macroCommand->getNumberOfParameters() > 0);
        const WuQMacroCommandParameter* parameterOne = macroCommand->getParameterAtIndex(0);
        CaretAssert(parameterOne);
        
        CaretAssert(parameterOne->getDataType() == WuQMacroDataValueTypeEnum::BOOLEAN);
        const QVariant dataValue = parameterOne->getValue();
        
        moveMouseToWidget(toolButton);
        WuQMacroSignalEmitter signalEmitter;
        signalEmitter.emitQToolButtonSignal(toolButton,
                                            dataValue.toBool());
    }
    else {
        castFailureFlagOut = true;
    }
}
