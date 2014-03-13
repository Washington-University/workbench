
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __WU_Q_TRUE_FALSE_COMBO_BOX_DECLARE__
#include "WuQTrueFalseComboBox.h"
#undef __WU_Q_TRUE_FALSE_COMBO_BOX_DECLARE__

using namespace caret;


    
/**
 * \class caret::WuQTrueFalseComboBox 
 * \brief Combo box for true/false type values.
 */

/**
 * Constructor.
 */
WuQTrueFalseComboBox::WuQTrueFalseComboBox(const QString& trueText,
                                           const QString& falseText,
                                           QObject* parent)
: WuQWidget(parent)
{
    this->createComboBox(trueText, 
                         falseText);
}

/**
 * Constructor.
 */
WuQTrueFalseComboBox::WuQTrueFalseComboBox(QObject* parent)
: WuQWidget(parent)
{
    this->createComboBox("true", 
                         "false");
}

/**
 * Destructor.
 */
WuQTrueFalseComboBox::~WuQTrueFalseComboBox()
{
    
}

/**
 * Create the combo box.
 * @param trueText
 *    Text for 'true' value.
 * @param falseText
 *    Text for 'false' value.
 */
void 
WuQTrueFalseComboBox::createComboBox(const QString& trueText,
                                     const QString& falseText)
{
    this->comboBox = new QComboBox();
    this->comboBox->addItem(trueText);
    this->comboBox->addItem(falseText);
    QObject::connect(this->comboBox, SIGNAL(activated(int)),
                     this, SLOT(comboBoxValueChanged(int)));
}

/**
 * Called when value is changed.
 */
void 
WuQTrueFalseComboBox::comboBoxValueChanged(int indx)
{
    bool boolValue = (indx == 0);
    emit statusChanged(boolValue);
}

/**
 * @return The embedded widget.
 */
QWidget* 
WuQTrueFalseComboBox::getWidget()
{
    return this->comboBox;
}

/**
 * @return If true is selected.
 */
bool 
WuQTrueFalseComboBox::isTrue()
{
    const int indx = this->comboBox->currentIndex();
    const bool boolValue = (indx == 0);
    return boolValue;
}

/**
 * @return If false is selected.
 */
bool 
WuQTrueFalseComboBox::isFalse()
{
    const int indx = this->comboBox->currentIndex();
    const bool boolValue = (indx == 1);
    return boolValue;
}

/**
 * Set the new true/false status.
 * @parma status
 *    New status.
 */
void 
WuQTrueFalseComboBox::setStatus(const bool status)
{
    if (status) {
        this->comboBox->setCurrentIndex(0);
    }
    else {
        this->comboBox->setCurrentIndex(1);
    }
}
