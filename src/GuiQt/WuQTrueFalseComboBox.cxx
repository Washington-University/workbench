
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
: QObject(parent)
{
    this->createComboBox(trueText, 
                         falseText);
}

/**
 * Constructor.
 */
WuQTrueFalseComboBox::WuQTrueFalseComboBox(QObject* parent)
: QObject(parent)
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
