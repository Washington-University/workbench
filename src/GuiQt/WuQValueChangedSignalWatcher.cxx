
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

#define __WU_Q_VALUE_CHANGED_SIGNAL_WATCHER_DECLARE__
#include "WuQValueChangedSignalWatcher.h"
#undef __WU_Q_VALUE_CHANGED_SIGNAL_WATCHER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
using namespace caret;

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
    
#include "CaretColorEnumComboBox.h"

/**
 * \class caret::WuQValueChangedSignalWatcher 
 * \brief Simplifies connecting many value changed signals to one slot
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *  The parent object
 */
WuQValueChangedSignalWatcher::WuQValueChangedSignalWatcher(QObject* parent)
: QObject(parent)
{
    
}

/**
 * Destructor.
 */
WuQValueChangedSignalWatcher::~WuQValueChangedSignalWatcher()
{
}

/**
 * Watch for value changed signal from the given object.  If the object is not
 * supported a message is logged.
 *
 * @param object
 *  Object whose signal are watched for value changed
 */
void
WuQValueChangedSignalWatcher::addObject(QObject* object)
{
    CaretAssert(object);
    
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(object);
    if (checkBox != NULL) {
        QObject::connect(checkBox, &QCheckBox::clicked,
                         this, [=](bool) { emit valueChanged(); });
        return;
    }
    
    QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(object);
    if (doubleSpinBox != NULL) {
        QObject::connect(doubleSpinBox,  QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                         this, [=](double) { emit valueChanged(); });
        return;
    }
    
    CaretColorEnumComboBox* caretColorComboBox = qobject_cast<CaretColorEnumComboBox*>(object);
    if (caretColorComboBox != NULL) {
        QObject::connect(caretColorComboBox, &CaretColorEnumComboBox::colorSelected,
                         this, [=](const CaretColorEnum::Enum) { emit valueChanged(); });
        return;
    }

    /* must be after CaretColorEnumComboBox as it is child of QComboBox */
    QComboBox* comboBox = qobject_cast<QComboBox*>(object);
    if (caretColorComboBox != NULL) {
        QObject::connect(comboBox, QOverload<int>::of(&QComboBox::activated),
                         this, [=](int) { emit valueChanged(); });
        return;
    }
    
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
    if (doubleSpinBox != NULL) {
        QObject::connect(spinBox,  QOverload<int>::of(&QSpinBox::valueChanged),
                         this, [=](int) { emit valueChanged(); });
        return;
    }

    const QString msg("Object of class "
                      + QString(object->metaObject()->className())
                      + " not supported by WuQValueChangedSignalWatcher");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
}

