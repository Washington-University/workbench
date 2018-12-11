
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

#define __WU_Q_MACRO_SIGNAL_EMITTER_DECLARE__
#include "WuQMacroSignalEmitter.h"
#undef __WU_Q_MACRO_SIGNAL_EMITTER_DECLARE__

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

using namespace caret;

/**
 * \class caret::WuQMacroSignalEmitter 
 * \brief Causes the emission of a signal from a QObject instance
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 */
WuQMacroSignalEmitter::WuQMacroSignalEmitter()
: QObject()
{
    
}

/**
 * Destructor.
 */
WuQMacroSignalEmitter::~WuQMacroSignalEmitter()
{
}

/**
 * Update a QAction's checked status and cause emission
 * of its triggered() signal
 *
 * @param action
 *    The QAction
 * @param checked
 *    New checked status
 */
void
WuQMacroSignalEmitter::emitQActionSignal(QAction* action,
                                        const bool checked)
{
    CaretAssert(action);
    action->setChecked(checked);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalBool,
                     action, &QAction::triggered);
    
    emit valueChangedSignalBool(checked);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalBool, 0, 0);
}

/**
 * Update a QActionGroup's selected action and cause emission
 * of its triggered() signal
 *
 * @param actionGroup
 *    The QActionGroup
 * @param text
 *    Text of selected action
 */
void
WuQMacroSignalEmitter::emitActionGroupSignal(QActionGroup* actionGroup,
                                             const QString& text)
{
    CaretAssert(actionGroup);
    
    QList<QAction*> actions = actionGroup->actions();
    if (actions.isEmpty()) {
        CaretLogWarning("Menu "
                        + actionGroup->objectName()
                        + " does not contain any actions when trying to select menu item with text: "
                        + text);
        return;
    }
    
    QAction* actionSelected = NULL;
    const int32_t numActions = actions.size();
    for (int32_t i = 0; i < numActions; i++) {
        if (actions.at(i)->text() == text) {
            actionSelected = actions.at(i);
        }
    }
    
    if (actionSelected == NULL) {
        CaretLogWarning("Unable to find QAction with text \""
                        + text
                        + "\" for menu "
                        + actionGroup->objectName());
        return;
    }
    
    QSignalBlocker blocker(actionGroup);
    actionSelected->trigger();
    blocker.unblock();
    
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalActionGroupAction,
                     actionGroup, &QActionGroup::triggered);
    
    emit valueChangeSignalMenuAction(actionSelected);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalActionGroupAction, 0, 0);
}

/**
 * Update a QButtonGroup and cause emission
 * of its buttonClicked() signal
 *
 * @param buttonGroup
 *    The QButtonGroup
 * @param text
 *    Text of button
 */
void
WuQMacroSignalEmitter::emitQButtonGroupSignal(QButtonGroup* buttonGroup,
                                              const QString& text)
{
    CaretAssert(buttonGroup);
    
    QAbstractButton* buttonSelected = NULL;
    
    QList<QAbstractButton*> allButtons = buttonGroup->buttons();
    if (allButtons.isEmpty()) {
        CaretLogWarning("ButtonGroup "
                        + buttonGroup->objectName()
                        + " does not contain any buttons when trying to select button item with text: "
                        + text);
        return;
    }
    
    for (auto b : allButtons) {
        if (b->text() == text) {
            buttonSelected = b;
            break;
        }
    }
    if (buttonSelected == NULL) {
        CaretLogWarning("Unable to find QAbstactButton with text \""
                        + text
                        + "\" for button group "
                        + buttonGroup->objectName());
        return;
    }
    
    QSignalBlocker blocker(buttonGroup);
    buttonSelected->click();
    blocker.unblock();
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalAbstractButton,
                     buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked));
    
    emit valueChangedSignalAbstractButton(buttonSelected);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalBool, 0, 0);
}

/**
 * Update a QCheckBox's checked status and cause emission
 * of its clicked() signal
 *
 * @param checkBox
 *    The QCheckBox
 * @param checked
 *    New checked status
 */
void
WuQMacroSignalEmitter::emitQCheckBoxSignal(QCheckBox* checkBox,
                                           const bool checked)
{
    CaretAssert(checkBox);
    checkBox->setChecked(checked);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalBool,
                     checkBox, &QCheckBox::clicked);
    
    emit valueChangedSignalBool(checked);

    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalBool, 0, 0);
}

/**
 * Update a QComboBox's index and cause emission
 * of its triggered() signal
 *
 * @param comboBox
 *    The QComboBox
 * @param index
 *    New index
 */
void
WuQMacroSignalEmitter::emitQComboBoxSignal(QComboBox* comboBox,
                                           const int32_t index)
{
    CaretAssert(comboBox);
    comboBox->setCurrentIndex(index);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalInt,
                     comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated));
    
    emit valueChangedSignalInt(index);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalInt, 0, 0);
}

/**
 * Update a QDoubleSpinBox value and cause emission
 * of its valueChanged() signal
 *
 * @param doubleSpinBox
 *    The QDoubleSpinBox
 * @param value
 *    New value
 */
void
WuQMacroSignalEmitter::emitQDoubleSpinBoxSignal(QDoubleSpinBox* doubleSpinBox,
                                                const double value)
{
    CaretAssert(doubleSpinBox);
    QSignalBlocker blocker(doubleSpinBox);
    doubleSpinBox->setValue(value);
    blocker.unblock();
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalDouble,
                     doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged));
    
    emit valueChangedSignalDouble(value);

    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalDouble, 0, 0);
    
}

/**
 * Update a QListWidget's item and cause emission
 * of its itemActivated() signal
 *
 * @param lineEdit
 *    The QLineEdit
 * @param text
 *    New text
 */
void
WuQMacroSignalEmitter::emitQListWidgetSignal(QListWidget* listWidget,
                           const QString& text)
{
    CaretAssert(listWidget);
    
    QList<QListWidgetItem*> items = listWidget->findItems(text,
                                                          Qt::MatchExactly);
    if (items.isEmpty()) {
        CaretLogWarning("Unable to find QListWidgetItem with text \""
                        + text
                        + "\" for "
                        + listWidget->objectName());
        return;
    }
    
    QListWidgetItem* newItem = items.at(0);
    listWidget->setCurrentItem(newItem);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangeSignalListWidgetItem,
                     listWidget, &QListWidget::itemActivated);
    
    emit valueChangeSignalListWidgetItem(newItem);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangeSignalListWidgetItem, 0, 0);
}


/**
 * Update a QLineEdit's text and cause emission
 * of its textChanged() signal
 *
 * @param lineEdit
 *    The QLineEdit
 * @param text
 *    New text
 */
void
WuQMacroSignalEmitter::emitQLineEditSignal(QLineEdit* lineEdit,
                                           const QString& text)
{
    CaretAssert(lineEdit);
    lineEdit->setText(text);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalString,
                     lineEdit, &QLineEdit::textChanged);
    
    emit valueChangedSignalString(text);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalString, 0, 0);
}

/**
 * Select an item from a menu using the item's text
 *
 * @param lineEdit
 *    The QLineEdit
 * @param menuActionText
 *    Text of menu item for selection
 */
void
WuQMacroSignalEmitter::emitQMenuSignal(QMenu* menu,
                                       const QString& menuActionText)
{
    CaretAssert(menu);
    
    QList<QAction*> actions = menu->actions();
    if (actions.isEmpty()) {
        CaretLogWarning("Menu "
                        + menu->objectName()
                        + " does not contain any actions when trying to select menu item with text: "
                        + menuActionText);
        return;
    }
    
    QAction* actionSelected = NULL;
    const int32_t numActions = actions.size();
    for (int32_t i = 0; i < numActions; i++) {
        if (actions.at(i)->text() == menuActionText) {
            actionSelected = actions.at(i);
        }
    }

    if (actionSelected == NULL) {
        CaretLogWarning("Unable to find QAction with text \""
                        + menuActionText
                        + "\" for menu "
                        + menu->objectName());
        return;
    }
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangeSignalMenuAction,
                     menu, &QMenu::triggered);
    
    emit valueChangeSignalMenuAction(actionSelected);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangeSignalMenuAction, 0, 0);
}


/**
 * Update a QPushButton's checked status and cause emission
 * of its clicked() signal
 *
 * @param pushButton
 *    The QPushButton
 * @param checked
 *    New checked status
 */
void
WuQMacroSignalEmitter::emitQPushButtonSignal(QPushButton* pushButton,
                                             const bool checked)
{
    CaretAssert(pushButton);
    pushButton->setChecked(checked);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalBool,
                     pushButton, &QPushButton::clicked);
    
    emit valueChangedSignalBool(checked);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalBool, 0, 0);
}

/**
 * Update a RadioButton's checked status and cause emission
 * of its checked() signal
 *
 * @param radioButton
 *    The QRadioButton
 * @param checked
 *    New checked status
 */
void
WuQMacroSignalEmitter::emitQRadioButtonSignal(QRadioButton* radioButton,
                                              const bool checked)
{
    CaretAssert(radioButton);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalBool,
                     radioButton, &QRadioButton::clicked);
    
    const bool useClickFlag(true);
    if (useClickFlag) {
        QSignalBlocker blocker(radioButton);
        radioButton->setChecked( !checked);
        blocker.unblock();
        radioButton->click();
    }
    else {
        /*
         * This does not work for a radio button in a button group
         * as either click() or animateClick() is needed for the
         * button group to emit a signal
         */
        radioButton->setChecked(checked);
        emit valueChangedSignalBool(checked);
    }
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalBool, 0, 0);
}

/**
 * Update a QSlider's value and cause emission
 * of its valueChanged signal
 *
 * @param slider
 *    The QSlider
 * @param value
 *    New value
 */
void
WuQMacroSignalEmitter::emitQSliderSignal(QSlider* slider,
                                         const int32_t value)
{
    CaretAssert(slider);
    
    QSignalBlocker blocker(slider);
    slider->setValue(value);
    blocker.unblock();
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalInt,
                     slider, &QSlider::valueChanged);
    
    emit valueChangedSignalInt(value);

    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalInt, 0, 0);
}

/**
 * Update a spin box's value and cause emission
 * of its valueChanged signal
 *
 * @param spinBox
 *    The QSpinBox
 * @param value
 *    New value
 */
void
WuQMacroSignalEmitter::emitQSpinBoxSignal(QSpinBox* spinBox,
                                          const int32_t value)
{
    CaretAssert(spinBox);
    
    QSignalBlocker blocker(spinBox);
    spinBox->setValue(value);
    blocker.unblock();
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalInt,
                     spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged));
    
    emit valueChangedSignalInt(value);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalInt, 0, 0);
    
}

/**
 * Update a TabBar's index and cause emission
 * of its currentChanged() signal
 *
 * @param tabBar
 *    The QTabBar
 * @param index
 *    New index
 */
void
WuQMacroSignalEmitter::emitQTabBarSignal(QTabBar* tabBar,
                                         const int32_t index)
{
    CaretAssert(tabBar);
    
    tabBar->setCurrentIndex(index);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalInt,
                     tabBar, &QTabBar::currentChanged);
    
    emit valueChangedSignalInt(index);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalInt, 0, 0);
}

/**
 * Update a TabWidget's index and cause emission
 * of its currentChanged() signal
 *
 * @param tabWidget
 *    The QTabWidget
 * @param index
 *    New index
 */
void
WuQMacroSignalEmitter::emitQTabWidgetSignal(QTabWidget* tabWidget,
                                            const int32_t index)
{
    CaretAssert(tabWidget);
    
    tabWidget->setCurrentIndex(index);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalInt,
                     tabWidget, &QTabWidget::currentChanged);
    
    emit valueChangedSignalInt(index);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalInt, 0, 0);
}

/**
 * Update a QToolButton checked status and cause emission
 * of its checked() signal
 *
 * @param toolButton
 *    The QToolButton
 * @param checked
 *    New checked status
 */
void
WuQMacroSignalEmitter::emitQToolButtonSignal(QToolButton* toolButton,
                                             const bool checked)
{
    CaretAssert(toolButton);
    toolButton->setChecked(checked);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalBool,
                     toolButton, &QToolButton::clicked);
    
    emit valueChangedSignalBool(checked);
    
    QObject::disconnect(this, &WuQMacroSignalEmitter::valueChangedSignalBool, 0, 0);
}
