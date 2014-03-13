
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

#define __WU_Q_SPIN_BOX_GROUP_DECLARE__
#include "WuQSpinBoxGroup.h"
#undef __WU_Q_SPIN_BOX_GROUP_DECLARE__

#include <QDoubleSpinBox>
#include <QSpinBox>

using namespace caret;


    
/**
 * \class caret::WuQSpinBoxGroup 
 * \brief Container that organizes a group of spin boxes. 
 *
 * Container that organizes a group of spin boxes.
 */
/**
 * Constructor.
 * @param parent
 *   Parent of this container.
 */
WuQSpinBoxGroup::WuQSpinBoxGroup(QObject* parent)
: QObject(parent)
{
    
}

/**
 * Destructor.
 */
WuQSpinBoxGroup::~WuQSpinBoxGroup()
{
    for (std::vector<SpinBoxReceiver*>::iterator iter =
         this->spinBoxReceivers.begin();
         iter != this->spinBoxReceivers.end();
         iter++) {
        delete *iter;
    }
    this->spinBoxReceivers.clear();
}

/**
 * Add a spin box to this spin box group.
 * @param abstractSpinBox
 *    Spin box that is added.
 */
void 
WuQSpinBoxGroup::addSpinBox(QAbstractSpinBox* abstractSpinBox)
{
    SpinBoxReceiver* sbr = new SpinBoxReceiver(this,
                                               abstractSpinBox,
                                               this->spinBoxReceivers.size());
    this->spinBoxReceivers.push_back(sbr);    
}

/**
 * Called when double spin box value is changed.
 * @param doubleSpinBox
 *    Double spin box that had its value changed.
 * @param d
 *    New value.
 */
void 
WuQSpinBoxGroup::doubleSpinBoxChangeReceiver(QDoubleSpinBox* doubleSpinBox,
                                 const double d)
{
    emit doubleSpinBoxValueChanged(doubleSpinBox, d);
}

/**
 * Called when spin box value is changed.
 * @param spinBox
 *    Spin box that had its value changed.
 * @param i
 *    New value.
 */
void 
WuQSpinBoxGroup::spinBoxChangeReceiver(QSpinBox* spinBox,
                           const int i)
{
    emit spinBoxValueChanged(spinBox, i);
}
//------------------------------------------------------------

SpinBoxReceiver::SpinBoxReceiver(WuQSpinBoxGroup* spinBoxGroup,
                                 QAbstractSpinBox* abstractSpinBox,
                                 const int spinBoxIndex)
{
    this->spinBoxGroup = spinBoxGroup;
    this->spinBoxIndex = spinBoxIndex;
    
    this->spinBox = dynamic_cast<QSpinBox*>(abstractSpinBox);
    if (this->spinBox != NULL) {
        QObject::connect(this->spinBox, SIGNAL(valueChanged(int)),
                         this, SLOT(valueChangedSlot(int)));
    }
    this->doubleSpinBox = dynamic_cast<QDoubleSpinBox*>(abstractSpinBox);
    if (this->doubleSpinBox != NULL) {
        QObject::connect(this->doubleSpinBox, SIGNAL(valueChanged(double)),
                         this, SLOT(valueChangedSlot(double)));
    }
}

SpinBoxReceiver::~SpinBoxReceiver()
{
    
}

void 
SpinBoxReceiver::valueChangedSlot(int i)
{
    this->spinBoxGroup->spinBoxChangeReceiver(this->spinBox,
                                              i);
}

void 
SpinBoxReceiver::valueChangedSlot(double d)
{
    this->spinBoxGroup->doubleSpinBoxChangeReceiver(this->doubleSpinBox,
                                                    d);
}

