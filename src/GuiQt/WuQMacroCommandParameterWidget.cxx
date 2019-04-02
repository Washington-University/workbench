
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_DECLARE__
#include "WuQMacroCommandParameterWidget.h"
#undef __WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QStackedWidget>
#include <QToolButton>

#include "CaretAssert.h"
#include "WbMacroCustomDataInfo.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroManager.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroCommandParameterWidget 
 * \brief Widget for display and editing a macro command parameter value
 * \ingroup GuiQt
 */

/**
 * Constructor for command parameter widgets
 *
 * @param index
 *    Index of the parameter
 * @param gridLayout
 *    Layout containing the widgets
 * @param parent
 *    The parent widget
 */
WuQMacroCommandParameterWidget::WuQMacroCommandParameterWidget(const int32_t index,
                                                               QGridLayout* gridLayout,
                                                               QWidget* parent)
: QObject(parent),
m_index(index)
{
    m_nameLabel = new QLabel();
    modifySizePolicy(m_nameLabel);
    
    m_booleanOnAction = new QAction("On");
    m_booleanOnAction->setCheckable(true);
    QObject::connect(m_booleanOnAction, &QAction::triggered,
                     this, &WuQMacroCommandParameterWidget::booleanOnActionTriggered);
    QToolButton* booleanOnToolButton = new QToolButton();
    booleanOnToolButton->setDefaultAction(m_booleanOnAction);
    
    m_booleanOffAction = new QAction("Off");
    m_booleanOffAction->setCheckable(true);
    QObject::connect(m_booleanOffAction, &QAction::triggered,
                     this, &WuQMacroCommandParameterWidget::booleanOffActionTriggered);
    QToolButton* booleanOffToolButton = new QToolButton();
    booleanOffToolButton->setDefaultAction(m_booleanOffAction);
    
    QActionGroup* booleanActionGroup = new QActionGroup(this);
    booleanActionGroup->addAction(m_booleanOffAction);
    booleanActionGroup->addAction(m_booleanOnAction);
    booleanActionGroup->setExclusive(true);
    
    m_comboBox      = new QComboBox();
    m_comboBox->setFixedHeight(m_comboBox->sizeHint().height());
    m_comboBox->setEditable(true);
    QObject::connect(m_comboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &WuQMacroCommandParameterWidget::comboBoxActivated);
    
    m_doubleSpinBox = new QDoubleSpinBox();
    m_doubleSpinBox->setFixedWidth(120);
    m_doubleSpinBox->setFixedHeight(m_doubleSpinBox->sizeHint().height());
    QObject::connect(m_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &WuQMacroCommandParameterWidget::doubleSpinBoxValueChanged);
    
    m_lineEdit      = new QLineEdit();
    m_lineEdit->setFixedHeight(m_lineEdit->sizeHint().height());
    QObject::connect(m_lineEdit, &QLineEdit::textEdited,
                     this, &WuQMacroCommandParameterWidget::lineEditTextEdited);
    
    m_spinBox       = new QSpinBox();
    m_spinBox->setFixedWidth(120);
    m_spinBox->setFixedHeight(m_spinBox->sizeHint().height());
    QObject::connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &WuQMacroCommandParameterWidget::spinBoxValueChanged);

    m_noValueWidget = new QWidget();
    m_noValueWidget->setFixedWidth(50);
    m_noValueWidget->setFixedHeight(10);
    
    m_booleanWidget = new QWidget();
    QHBoxLayout* booleanLayout = new QHBoxLayout(m_booleanWidget);
    booleanLayout->setContentsMargins(0, 0, 0, 0);
    booleanLayout->addWidget(booleanOnToolButton);
    booleanLayout->addWidget(booleanOffToolButton);
    booleanLayout->addStretch();
    m_booleanWidget->setFixedHeight(m_booleanWidget->sizeHint().height());

    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_booleanWidget);
    m_stackedWidget->addWidget(m_comboBox);
    m_stackedWidget->addWidget(m_doubleSpinBox);
    m_stackedWidget->addWidget(m_lineEdit);
    m_stackedWidget->addWidget(m_spinBox);
    m_stackedWidget->addWidget(m_noValueWidget);
    
    const int numWidgets = m_stackedWidget->count();
    for (int32_t i = 0; i < numWidgets; i++) {
        modifySizePolicy(m_stackedWidget->widget(i));
    }
    modifySizePolicy(m_stackedWidget);
    
    /*
     * Note: A QStackedWidget aligns its current widget at the top.
     * So, align the label at the top so that the label and widget
     * are approximately aligned
     */
    const int32_t row = gridLayout->rowCount();
    gridLayout->addWidget(m_nameLabel, row, 0, Qt::AlignTop);
    gridLayout->addWidget(m_stackedWidget, row, 1);
}

/**
 * Destructor.
 */
WuQMacroCommandParameterWidget::~WuQMacroCommandParameterWidget()
{
}

/**
 * Modify the size policy so that widget does not retain
 * size when hidden
 *
 * @param w
 *     The widget
 */
void
WuQMacroCommandParameterWidget::modifySizePolicy(QWidget* w)
{
    CaretAssert(w);
    QSizePolicy sp = w->sizePolicy();
    sp.setRetainSizeWhenHidden(false);
    w->setSizePolicy(sp);
}

/**
 * Update content of a command parameter
 *
 * @param windowIndex
 *     Index of window
 * @param macroCommand
 *     Macro command containing the parameter
 * @param parameter
 *     The parameter
 */
void
WuQMacroCommandParameterWidget::updateContent(int32_t windowIndex,
                                              WuQMacroCommand* macroCommand,
                                              WuQMacroCommandParameter* parameter)
{
    m_windowIndex  = windowIndex;
    m_macroCommand = macroCommand;
    m_parameter    = parameter;
    
    QWidget* activeWidget(m_noValueWidget);
    
    const bool validFlag(m_parameter != NULL);
    if (validFlag) {
        CaretAssert(m_macroCommand);
        CaretAssert(m_parameter);
        
        m_nameLabel->setText(m_parameter->getName());
        
        const QString customDataTypeName = parameter->getCustomDataType();
        const bool customDataFlag( ! customDataTypeName.isEmpty());
        
        switch (parameter->getDataType()) {
            case WuQMacroDataValueTypeEnum::AXIS:
            {
                m_comboBox->clear();
                m_comboBox->setEditable(false);
                
                std::vector<QString> axisValues { "X", "Y", "Z" };
                
                int32_t defaultIndex(-1);
                const QString value = m_parameter->getValue().toString();
                const int32_t numAxes = static_cast<int32_t>(axisValues.size());
                for (int32_t i = 0; i < numAxes; i++) {
                    CaretAssertVectorIndex(axisValues, i);
                    if (value == axisValues[i]) {
                        defaultIndex = i;
                    }
                    
                    m_comboBox->addItem(axisValues[i]);
                }
                
                if (defaultIndex < 0) {
                    if ( ! value.isEmpty()) {
                        defaultIndex = m_comboBox->count();
                        m_comboBox->addItem(value);
                    }
                }
                if (defaultIndex >= 0) {
                    m_comboBox->setCurrentIndex(defaultIndex);
                }
                
                activeWidget = m_comboBox;
            }
                break;
            case WuQMacroDataValueTypeEnum::BOOLEAN:
            {
                if (parameter->getValue().toBool()) {
                    m_booleanOnAction->setChecked(true);
                }
                else {
                    m_booleanOffAction->setChecked(true);
                }
                activeWidget = m_booleanWidget;
            }
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
            {
                std::array<float, 2> floatRange { -1.0e6, 1.0e6 };
                if (customDataFlag) {
                    WbMacroCustomDataInfo customDataInfo(WuQMacroDataValueTypeEnum::FLOAT);
                    WuQMacroManager::instance()->getCustomParameterDataInfo(windowIndex,
                                                                            macroCommand,
                                                                            parameter,
                                                                            customDataInfo);
                    floatRange = customDataInfo.getFloatRange();
                }
                
                QSignalBlocker blocker(m_doubleSpinBox);
                m_doubleSpinBox->setRange(floatRange[0], floatRange[1]);
                m_doubleSpinBox->setValue(m_parameter->getValue().toFloat());
                activeWidget = m_doubleSpinBox;
            }
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
            {
                std::array<int32_t, 2> intRange { -100000, 100000 };
                if (customDataFlag) {
                    WbMacroCustomDataInfo customDataInfo(WuQMacroDataValueTypeEnum::INTEGER);
                    WuQMacroManager::instance()->getCustomParameterDataInfo(windowIndex,
                                                                            macroCommand,
                                                                            parameter,
                                                                            customDataInfo);
                    intRange = customDataInfo.getIntegerRange();
                }
                
                QSignalBlocker blocker(m_spinBox);
                m_spinBox->setRange(intRange[0], intRange[1]);
                m_spinBox->setValue(m_parameter->getValue().toInt());
                activeWidget = m_spinBox;
            }
                break;
            case WuQMacroDataValueTypeEnum::INVALID:
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                break;
            case WuQMacroDataValueTypeEnum::NONE:
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                m_lineEdit->setText(m_parameter->getValue().toString());
                activeWidget = m_lineEdit;
                break;
            case WuQMacroDataValueTypeEnum::STRING_LIST:
            {
                m_comboBox->clear();
                
                std::vector<QString> stringValues;
                
                if (customDataFlag) {
                    WbMacroCustomDataInfo customDataInfo(WuQMacroDataValueTypeEnum::STRING_LIST);
                    WuQMacroManager::instance()->getCustomParameterDataInfo(windowIndex,
                                                                            macroCommand,
                                                                            parameter,
                                                                            customDataInfo);
                    stringValues = customDataInfo.getStringListValues();
                }
                
                int32_t defaultIndex(-1);
                const QString selectedValue = m_parameter->getValue().toString();
                
                const int32_t numValues = static_cast<int32_t>(stringValues.size());
                if (numValues > 0) {
                    for (int32_t i = 0; i < numValues; i++) {
                        CaretAssertVectorIndex(stringValues, i);
                        if (selectedValue == stringValues[i]) {
                            defaultIndex = i;
                        }
                        
                        m_comboBox->addItem(stringValues[i]);
                    }
                    
                    bool editableFlag(false);
                    if (editableFlag) {
                        m_comboBox->setEditable(true);
                        if (defaultIndex < 0) {
                            if ( ! selectedValue.isEmpty()) {
                                defaultIndex = m_comboBox->count();
                                m_comboBox->addItem(selectedValue);
                            }
                        }
                    }
                    else {
                        m_comboBox->setEditable(false);
                    }
                }
                else {
                    if ( ! selectedValue.isEmpty()) {
                        defaultIndex = m_comboBox->count();
                        m_comboBox->addItem(selectedValue);
                    }
                    m_comboBox->setEditable(true);
                }
                
                if (defaultIndex >= 0) {
                    m_comboBox->setCurrentIndex(defaultIndex);
                }
                
                activeWidget = m_comboBox;
            }
                break;
        }
    }

    m_stackedWidget->setCurrentWidget(activeWidget);
    
    m_nameLabel->setVisible(validFlag);
    m_stackedWidget->setVisible(validFlag);
    
//    std::cout << "Stacked " << m_index << " height " << m_stackedWidget->sizeHint().height() << std::endl;
//    for (int32_t i = 0; i < m_stackedWidget->count(); i++) {
//        QWidget* w = m_stackedWidget->widget(i);
//        std::cout << "   " << i << " height " << w->sizeHint().height() << " " << w->metaObject()->className() << std::endl;
//    }
}

/**
 * Called when boolean OFF action is triggered
 */
void
WuQMacroCommandParameterWidget::booleanOffActionTriggered(bool)
{
    CaretAssert(m_parameter);
    m_parameter->setValue(false);
    
    emit dataChanged(m_index);
}

/**
 * Called when boolean ON action is triggered
 */
void
WuQMacroCommandParameterWidget::booleanOnActionTriggered(bool)
{
    CaretAssert(m_parameter);
    m_parameter->setValue(true);
    
    emit dataChanged(m_index);
}

/**
 * Called when combo box is activated
 *
 * @param index
 *     index of item selecteed
 */
void
WuQMacroCommandParameterWidget::comboBoxActivated(int /*index*/)
{
    CaretAssert(m_parameter);
    m_parameter->setValue(m_comboBox->currentText());
    
    emit dataChanged(m_index);
}

/**
 * Called when value of double spin box is changed
 *
 * @param value
 *     The new value
 */
void
WuQMacroCommandParameterWidget::doubleSpinBoxValueChanged(double value)
{
    CaretAssert(m_parameter);
    m_parameter->setValue(static_cast<float>(value));
    
    emit dataChanged(m_index);
}

/**
 * Called when value of the line edit is changed
 *
 * @param text
 *     The new text value
 */
void
WuQMacroCommandParameterWidget::lineEditTextEdited(const QString& text)
{
    CaretAssert(m_parameter);
    m_parameter->setValue(text);
    
    emit dataChanged(m_index);
}

/**
 * Called when value of double spin box is changed
 *
 * @param value
 *     The new value
 */
void
WuQMacroCommandParameterWidget::spinBoxValueChanged(int value)
{
    CaretAssert(m_parameter);
    m_parameter->setValue(value);
    
    emit dataChanged(m_index);
}
