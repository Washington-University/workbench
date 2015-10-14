
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
    
    this->spinBox = qobject_cast<QSpinBox*>(abstractSpinBox);
    if (this->spinBox != NULL) {
        QObject::connect(this->spinBox, SIGNAL(valueChanged(int)),
                         this, SLOT(valueChangedSlot(int)));
    }
    this->doubleSpinBox = qobject_cast<QDoubleSpinBox*>(abstractSpinBox);
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

