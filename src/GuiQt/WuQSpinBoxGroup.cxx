
/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

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

