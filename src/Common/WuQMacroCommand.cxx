
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

#define __WU_Q_MACRO_COMMAND_DECLARE__
#include "WuQMacroCommand.h"
#undef __WU_Q_MACRO_COMMAND_DECLARE__

//#include <QAction>
//#include <QApplication>
//#include <QCheckBox>
//#include <QComboBox>
//#include <QCursor>
//#include <QDoubleSpinBox>
//#include <QLineEdit>
//#include <QListWidget>
//#include <QMenu>
//#include <QPushButton>
//#include <QRadioButton>
//#include <QSlider>
//#include <QSpinBox>
//#include <QTabBar>
//#include <QTabWidget>
//#include <QToolButton>

#include "CaretAssert.h"
//#include "WuQMacroSignalEmitter.h"

using namespace caret;

/**
 * \class caret::WuQMacroCommand 
 * \brief Issues a QObject's signal so that its slots execute
 * \ingroup WuQMacro
 */

/**
 * Constructor.
 *
 * @param objectType
 *    Type of object
 * @param objectName
 *    Name of object
 * @param objectToolTip
 *    Tooltip of object
 * @param value
 *    Value for the command.
 */
WuQMacroCommand::WuQMacroCommand(const WuQMacroObjectTypeEnum::Enum objectType,
                                 const QString objectName,
                                 const QString objectToolTip,
                                 const QVariant value)
: CaretObjectTracksModification(),
m_objectType(objectType),
m_objectName(objectName),
m_objectToolTip(objectToolTip),
m_value(value)
{
}

/**
 * Destructor.
 */
WuQMacroCommand::~WuQMacroCommand()
{
}

/**
 * @return The object' type
 */
WuQMacroObjectTypeEnum::Enum
WuQMacroCommand::getObjectType() const
{
    return m_objectType;
}

/**
 * @return The object's name
 */
QString
WuQMacroCommand::getObjectName() const
{
    return m_objectName;
}

/**
 * @return The object's tooltip
 */
QString
WuQMacroCommand::getObjectToolTip() const
{
    return m_objectToolTip;
}

/**
 * @return The object's value.
 */
QVariant
WuQMacroCommand::getObjectValue() const
{
    return m_value;
}

///**
// * Run the command.
// *
// * @param object
// *    The object that must be cast to a specific type
// * @param errorMessageOut
// *    Output containing any error messages
// * @return
// *    True if the macro completed without errors, else false.
// */
//bool
//WuQMacroCommand::runMacro(QObject* object,
//                          QString& errorMessageOut) const
//{
//    errorMessageOut.clear();
//    
//    bool notFoundFlag(false);
//    switch (m_objectType) {
//        case WuQMacroObjectTypeEnum::ACTION:
//        {
//            QAction* action = qobject_cast<QAction*>(object);
//            if (action != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQActionSignal(action,
//                                                m_value.toBool());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::CHECK_BOX:
//        {
//            QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
//            if (checkBox != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQCheckBoxSignal(checkBox,
//                                                  m_value.toBool());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::COMBO_BOX:
//        {
//            QComboBox* comboBox = qobject_cast<QComboBox*>(object);
//            if (comboBox != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQComboBoxSignal(comboBox,
//                                                  m_value.toInt());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::DOUBLE_SPIN_BOX:
//        {
//            QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(object);
//            if (doubleSpinBox != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQDoubleSpinBoxSignal(doubleSpinBox,
//                                                       m_value.toDouble());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::INVALID:
//            CaretAssert(0);
//            break;
//        case WuQMacroObjectTypeEnum::LINE_EDIT:
//        {
//            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
//            if (lineEdit != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQLineEditSignal(lineEdit,
//                                                  m_value.toString());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::LIST_WIDGET:
//        {
//            QListWidget* listWidget = qobject_cast<QListWidget*>(object);
//            if (listWidget != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQListWidgetSignal(listWidget,
//                                                    m_value.toString());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::MENU:
//        {
//            QMenu* menu = qobject_cast<QMenu*>(object);
//            if (menu != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQMenuTriggered(menu,
//                                                 m_value.toString());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::MOUSE_USER_EVENT:
//            CaretAssertToDoFatal();
//            break;
//        case WuQMacroObjectTypeEnum::PUSH_BUTTON:
//        {
//            QPushButton* pushButton = qobject_cast<QPushButton*>(object);
//            if (pushButton != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQPushButtonSignal(pushButton,
//                                                    m_value.toBool());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::RADIO_BUTTON:
//        {
//            QRadioButton* radioButton = qobject_cast<QRadioButton*>(object);
//            if (radioButton != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQRadioButtonSignal(radioButton,
//                                                     m_value.toBool());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::SLIDER:
//        {
//            QSlider* slider = qobject_cast<QSlider*>(object);
//            if (slider != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQSliderSignal(slider,
//                                                m_value.toInt());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::SPIN_BOX:
//        {
//            QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
//            if (spinBox != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQSpinBoxSignal(spinBox,
//                                                 m_value.toInt());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::TAB_BAR:
//        {
//            QTabBar* tabBar = qobject_cast<QTabBar*>(object);
//            if (tabBar != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQTabBarSignal(tabBar,
//                                                m_value.toInt());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::TAB_WIDGET:
//        {
//            QTabWidget* tabWidget = qobject_cast<QTabWidget*>(object);
//            if (tabWidget != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQTabWidgetSignal(tabWidget,
//                                                   m_value.toInt());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//        case WuQMacroObjectTypeEnum::TOOL_BUTTON:
//        {
//            QToolButton* toolButton = qobject_cast<QToolButton*>(object);
//            if (toolButton != NULL) {
//                WuQMacroSignalEmitter signalEmitter;
//                signalEmitter.emitQToolButtonSignal(toolButton,
//                                                    m_value.toBool());
//            }
//            else {
//                notFoundFlag = true;
//            }
//        }
//            break;
//    }
//    
//    if (notFoundFlag) {
//        errorMessageOut = ("ERROR: Unable to cast object named "
//                           + m_objectName
//                           + " to "
//                           + WuQMacroObjectTypeEnum::toGuiName(m_objectType));
//        return false;
//    }
//    
//    return true;
//}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
WuQMacroCommand::toString() const
{
    QString s("WuQMacroCommand name=%1, type=%2, tooltip=%3, value=%4");
    s = s.arg(m_objectName
              ).arg(WuQMacroObjectTypeEnum::toGuiName(m_objectType)
                    ).arg(m_objectToolTip
                    ).arg(m_value.toString());
    return s;
}

