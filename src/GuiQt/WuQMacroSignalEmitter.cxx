
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
    checkBox->setChecked(checkBox);
    
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
 * Update a QLineEdit's text and cause emission
 * of its textChanged() signal
 *
 * @param lineEdit
 *    The QLineEdit
 * @param text
 *    New text
 */
void
WuQMacroSignalEmitter::emitQMenuTriggered(QMenu* menu,
                        const QString& text)
{
    CaretAssert(menu);
    
    QList<QAction*> actions = menu->actions();
    if (actions.isEmpty()) {
        CaretLogWarning("Unable to find QAction with text \""
                        + text
                        + "\" for menu "
                        + menu->objectName());
        return;
    }
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangeSignalMenuAction,
                     menu, &QMenu::triggered);
    
    emit valueChangeSignalMenuAction(actions.at(0));
    
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
    radioButton->setChecked(checked);
    
    QObject::connect(this, &WuQMacroSignalEmitter::valueChangedSignalBool,
                     radioButton, &QRadioButton::clicked);
    
    emit valueChangedSignalBool(checked);
    
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
