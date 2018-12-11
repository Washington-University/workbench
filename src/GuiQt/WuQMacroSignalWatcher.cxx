
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

#define __WU_Q_MACRO_SIGNAL_WATCHER_DECLARE__
#include "WuQMacroSignalWatcher.h"
#undef __WU_Q_MACRO_SIGNAL_WATCHER_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacroCommand.h"
#include "WuQMacroManager.h"

using namespace caret;

/**
 * \class caret::WuQMacroSignalWatcher 
 * \brief Watches a QObject instance to observe its "value changed" signal
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param parentMacroManager
 *     Parent macro manager.
 * @param object
 *     Object that is watched for a "value changed" signal
 * @param objectType
 *     The type of the object.
 * @param toolTipTextOverride
 *     Used to override tool tip or for when object does not
 *     support a tool tip.
 */
WuQMacroSignalWatcher::WuQMacroSignalWatcher(WuQMacroManager* parentMacroManager,
                                             QObject* object,
                                             const WuQMacroObjectTypeEnum::Enum objectType,
                                             const QString& toolTipTextOverride)
: QObject(),
m_parentMacroManager(parentMacroManager),
m_objectType(objectType),
m_objectName(object->objectName())
{
    CaretAssert(m_parentMacroManager);
    CaretAssert(object);
    
    QWidget* widget = qobject_cast<QWidget*>(object);
    if (widget != NULL) {
        m_toolTipText = widget->toolTip();
    }
    
    switch (m_objectType) {
        case WuQMacroObjectTypeEnum::ACTION:
        {
            QAction* action = qobject_cast<QAction*>(object);
            CaretAssert(action);
            QObject::connect(action, &QAction::triggered,
                             this, &WuQMacroSignalWatcher::actionTriggered);
            m_toolTipText = action->toolTip();
        }
            break;
        case WuQMacroObjectTypeEnum::ACTION_GROUP:
        {
            QActionGroup* actionGroup = qobject_cast<QActionGroup*>(object);
            CaretAssert(actionGroup);
            QObject::connect(actionGroup, &QActionGroup::triggered,
                             this, &WuQMacroSignalWatcher::actionGroupTriggered);
        }
            break;
        case WuQMacroObjectTypeEnum::BUTTON_GROUP:
        {
            QButtonGroup* buttonGroup = qobject_cast<QButtonGroup*>(object);
            CaretAssert(buttonGroup);
            QObject::connect(buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
                             this, &WuQMacroSignalWatcher::buttonGroupButtonClicked);
        }
            break;
        case WuQMacroObjectTypeEnum::CHECK_BOX:
        {
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
            CaretAssert(checkBox);
            QObject::connect(checkBox, &QCheckBox::clicked,
                             this, &WuQMacroSignalWatcher::checkBoxClicked);
        }
            break;
        case WuQMacroObjectTypeEnum::COMBO_BOX:
        {
            QComboBox* comboBox = qobject_cast<QComboBox*>(object);
            CaretAssert(comboBox);
            QObject::connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                             this, &WuQMacroSignalWatcher::comboBoxActivated);
        }
            break;
        case WuQMacroObjectTypeEnum::DOUBLE_SPIN_BOX:
        {
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(object);
            CaretAssert(spinBox);
            QObject::connect(spinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             this, &WuQMacroSignalWatcher::doubleSpinBoxValueChanged);
        }
            break;
        case WuQMacroObjectTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case WuQMacroObjectTypeEnum::LINE_EDIT:
        {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
            CaretAssert(lineEdit);
            QObject::connect(lineEdit, &QLineEdit::textEdited,
                             this, &WuQMacroSignalWatcher::lineEditTextEdited);
        }
            break;
        case WuQMacroObjectTypeEnum::LIST_WIDGET:
        {
            QListWidget* listWidget = qobject_cast<QListWidget*>(object);
            CaretAssert(listWidget);
            QObject::connect(listWidget, &QListWidget::itemActivated,
                             this, &WuQMacroSignalWatcher::listWidgetItemActivated);
        }
            break;
        case WuQMacroObjectTypeEnum::MENU:
        {
            QMenu* menu = qobject_cast<QMenu*>(object);
            CaretAssert(menu);
            QObject::connect(menu, &QMenu::triggered,
                             this, &WuQMacroSignalWatcher::menuTriggered);
        }
            break;
        case WuQMacroObjectTypeEnum::MOUSE_USER_EVENT:
            CaretAssertToDoFatal();
            break;
        case WuQMacroObjectTypeEnum::PUSH_BUTTON:
        {
            QPushButton* pushButton = qobject_cast<QPushButton*>(object);
            CaretAssert(pushButton);
            QObject::connect(pushButton, &QPushButton::clicked,
                             this, &WuQMacroSignalWatcher::pushButtonClicked);
        }
            break;
        case WuQMacroObjectTypeEnum::RADIO_BUTTON:
        {
            QRadioButton* radioButton = qobject_cast<QRadioButton*>(object);
            CaretAssert(radioButton);
            QObject::connect(radioButton, &QRadioButton::clicked,
                             this, &WuQMacroSignalWatcher::radioButtonClicked);
        }
            break;
        case WuQMacroObjectTypeEnum::SLIDER:
        {
            QSlider* slider = qobject_cast<QSlider*>(object);
            CaretAssert(slider);
            QObject::connect(slider, &QSlider::valueChanged,
                             this, &WuQMacroSignalWatcher::sliderValueChanged);
        }
            break;
        case WuQMacroObjectTypeEnum::SPIN_BOX:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
            CaretAssert(spinBox);
            QObject::connect(spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                             this, &WuQMacroSignalWatcher::spinBoxValueChanged);
        }
            break;
        case WuQMacroObjectTypeEnum::TAB_BAR:
        {
            QTabBar* tabBar = qobject_cast<QTabBar*>(object);
            CaretAssert(tabBar);
            QObject::connect(tabBar, &QTabBar::currentChanged,
                             this, &WuQMacroSignalWatcher::tabBarCurrentChanged);
        }
            break;
        case WuQMacroObjectTypeEnum::TAB_WIDGET:
        {
            QTabWidget* tabWidget = qobject_cast<QTabWidget*>(object);
            CaretAssert(tabWidget);
            QObject::connect(tabWidget, &QTabWidget::currentChanged,
                             this, &WuQMacroSignalWatcher::tabWidgetCurrentChanged);
        }
            break;
        case WuQMacroObjectTypeEnum::TOOL_BUTTON:
        {
            QToolButton* toolButton = qobject_cast<QToolButton*>(object);
            CaretAssert(toolButton);
            QObject::connect(toolButton, &QCheckBox::clicked,
                             this, &WuQMacroSignalWatcher::toolButtonClicked);
        }
            break;
    }
    
    /*
     * Override the tool tip text
     */
    if ( ! toolTipTextOverride.isEmpty()) {
        m_toolTipText = toolTipTextOverride;
    }
    
    QObject::connect(object, &QObject::destroyed,
                     this, &WuQMacroSignalWatcher::objectWasDestroyed);
    QObject::connect(object, &QObject::objectNameChanged,
                     this, &WuQMacroSignalWatcher::objectNameWasChanged);
}

/**
 * Destructor.
 */
WuQMacroSignalWatcher::~WuQMacroSignalWatcher()
{
}

/**
 * Called if the object whose signal is being monitored is destroyed
 *
 * @obj
 *    Pointer to object that was destroyed
 */
void
WuQMacroSignalWatcher::objectWasDestroyed(QObject* /*obj*/)
{
    /*
     * Log object destroyed only when NOT debug
     */
#ifndef NDEBUG
    CaretLogWarning("Object was destroyed: "
                    + m_objectName);
#endif
}

/**
 * Called if the object whose signal is being monitored 
 * has its name changed
 *
 * @name
 *    New name
 */
void
WuQMacroSignalWatcher::objectNameWasChanged(const QString& name)
{
    std::cout << "Object name changed from "
    << m_objectName << " to " << name << std::endl;
}

/**
 * Create an new instance of a widget signal watcher for
 * the given object.
 *
 * @param parentMacroManager
 *     Parent macro manager.
 * @param object
 *     Object that will have a widget signal watcher.
 * @param toolTipTextOverride
 *     Used to override tool tip or for when object does not
 *     support a tool tip.
 * @param errorMessageOut
 *     Output containing error information if failure.
 * @return
 *     Pointer to widget watcher or NULL if there was an error.
 */
WuQMacroSignalWatcher*
WuQMacroSignalWatcher::newInstance(WuQMacroManager* parentMacroManager,
                                   QObject* object,
                                   const QString& toolTipTextOverride,
                                   QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    QString objectClassName = object->metaObject()->className();
    
    bool validFlag(false);
    WuQMacroObjectTypeEnum::Enum objectType = WuQMacroObjectTypeEnum::fromGuiName(objectClassName,
                                                                                  &validFlag);

    if ((objectType == WuQMacroObjectTypeEnum::INVALID)
        || ( ! validFlag)) {
        errorMessageOut = ("Widget named \""
                           + object->objectName()
                           + "\" of class \""
                           + object->metaObject()->className()
                           + "\" is not supported for macros");
        return NULL;
    }
    
    WuQMacroSignalWatcher* ww = new WuQMacroSignalWatcher(parentMacroManager,
                                                          object,
                                                          objectType,
                                                          toolTipTextOverride);
    return ww;
}

/**
 * If recording mode is enabled, create and send a macro command
 * to the macro manager.
 *
 * @param value
 *     QVariant containing the data value.
 */
void
WuQMacroSignalWatcher::createAndSendMacroCommand(const QVariant value)
{
    if (m_parentMacroManager->isModeRecording()) {
        WuQMacroCommand* mc = new WuQMacroCommand(m_objectType,
                                                  m_objectName,
                                                  m_toolTipText,
                                                  value);
        if ( ! m_parentMacroManager->addMacroCommandToRecording(mc)) {
            delete mc;
        }
    }
}

/**
 * Called when a action group has an item triggered
 *
 * @param action
 *     ActionGroup action that was triggered
 */
void
WuQMacroSignalWatcher::actionGroupTriggered(QAction* action)
{
    const QString text((action != NULL)
                       ? action->text()
                       : "");
    createAndSendMacroCommand(text);
}


/**
 * Called when an action is triggered
 *
 * @param checked
 *     New checked status
 */
void
WuQMacroSignalWatcher::actionTriggered(bool checked)
{
    createAndSendMacroCommand(checked);
}

/**
 * Called when a button group button is clicked
 *
 * @param button
 *     Button that was clicked
 */
void
WuQMacroSignalWatcher::buttonGroupButtonClicked(QAbstractButton* button)
{
    const QString text((button != NULL)
                       ? button->text()
                       : "");
    createAndSendMacroCommand(text);
}


/**
 * Called when a check box is clicked
 *
 * @param checked
 *     New checked status
 */
void
WuQMacroSignalWatcher::checkBoxClicked(bool checked)
{
    createAndSendMacroCommand(checked);
}

/**
 * Called when a combo box is activated
 *
 * @param index
 *     Index of activated item
 */
void
WuQMacroSignalWatcher::comboBoxActivated(int index)
{
    createAndSendMacroCommand(index);
}

/**
 * Called when a spin box value is changed
 *
 * @param value
 *     New value in double spin box
 */
void
WuQMacroSignalWatcher::doubleSpinBoxValueChanged(double value)
{
    createAndSendMacroCommand(value);
}

/**
 * Called when a line edit has text edited
 *
 * @param text
 *     New value of text in the line edit
 */
void
WuQMacroSignalWatcher::lineEditTextEdited(const QString& text)
{
    createAndSendMacroCommand(text);
}

/**
 * Called when a list widget item is activated
 *
 * @param item
 *     List widget item that was selected
 */
void
WuQMacroSignalWatcher::listWidgetItemActivated(QListWidgetItem* item)
{
    const QString text((item != NULL)
                       ? item->text()
                       : "");
    createAndSendMacroCommand(text);
}

/**
 * Called when a menu has an item triggered
 *
 * @param action
 *     Menu action that was triggered
 */
void
WuQMacroSignalWatcher::menuTriggered(QAction* action)
{
    const QString text((action != NULL)
                       ? action->text()
                       : "");
    createAndSendMacroCommand(text);
}

/**
 * Called when a
 *
 * @param checked
 *     New checked status
 */
void
WuQMacroSignalWatcher::pushButtonClicked(bool checked)
{
    createAndSendMacroCommand(checked);
}

/**
 * Called when a radio button is clicked
 *
 * @param checked
 *     New checked status
 */
void
WuQMacroSignalWatcher::radioButtonClicked(bool checked)
{
    createAndSendMacroCommand(checked);
}

/**
 * Called when a slider value is changed
 *
 * @param value
 *     New value
 */
void
WuQMacroSignalWatcher::sliderValueChanged(int value)
{
    createAndSendMacroCommand(value);
}

/**
 * Called when a spin box value is changed
 *
 * @param value
 *     New value
 */
void
WuQMacroSignalWatcher::spinBoxValueChanged(int value)
{
    createAndSendMacroCommand(value);
}

/**
 * Called when a tab bar current tab is changed
 *
 * @param index
 *     Index of the new selected tab
 */
void
WuQMacroSignalWatcher::tabBarCurrentChanged(int index)
{
    createAndSendMacroCommand(index);
}

/**
 * Called when a tab widget current tab is changed
 *
 * @param index
 *     Index of the new selected tab
 */
void
WuQMacroSignalWatcher::tabWidgetCurrentChanged(int index)
{
    createAndSendMacroCommand(index);
}

/**
 * Called when a tool button is clicked
 *
 * @param checked
 *     New check status
 */
void
WuQMacroSignalWatcher::toolButtonClicked(bool checked)
{
    createAndSendMacroCommand(checked);
}

/**
 * @return String containing description of this signal watcher
 */
QString
WuQMacroSignalWatcher::toString() const
{
    QString s(m_objectName
              + " type="
              + WuQMacroObjectTypeEnum::toGuiName(m_objectType));
    return s;
}


