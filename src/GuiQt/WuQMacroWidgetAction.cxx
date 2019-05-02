
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

#define __WU_Q_MACRO_WIDGET_ACTION_DECLARE__
#include "WuQMacroWidgetAction.h"
#undef __WU_Q_MACRO_WIDGET_ACTION_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
using namespace caret;


    
/**
 * \class caret::WuQMacroWidgetAction 
 * \brief Like QWidgetAction but for use with widgets monitored by macro system but in dialogs
 * \ingroup GuiQt
 *
 * Widgets in modal dialogs exist only while the dialog is active.  Widgets in non-modal
 * dialogs do not exist until the dialog is created.  This presents a problem
 * when the macro executor wants to trigger the widget's signal but the widget does not
 * exist.  This class is used by the macro system and an instance contains a signal corresponding
 * to a widget in a modal dialog.
 */


/**
 * Constructor.
 *
 * @param widgetType
 *     Type of widget
 * @param objectName
 *     Name of this object
 * @param objectToolTip
 *     Tooltip for object
 * @param parent
 *     Parent for object
 */
WuQMacroWidgetAction::WuQMacroWidgetAction(const WidgetType widgetType,
                                           const QString& objectName,
                                           const QString& objectToolTip,
                                           QObject* parent)
: QObject(parent),
m_widgetType(widgetType),
m_toolTip(objectToolTip)
{
    setObjectName(objectName);
    
    QObject::connect(this, &WuQMacroWidgetAction::valueChanged,
                     this, &WuQMacroWidgetAction::setValuePrivate);
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
WuQMacroWidgetAction::~WuQMacroWidgetAction()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return The type of data value
 */
WuQMacroWidgetAction::WidgetType
WuQMacroWidgetAction::getWidgetType() const
{
    return m_widgetType;
}

/**
 * @return A widget the represents the action with the given parent.
 * Caller is responsibled for deleting the widget and must call
 * releaseWidget() prior to deleting the widget.
 *
 * @param Parent for the returned widget
 */
QWidget*
WuQMacroWidgetAction::requestWidget(QWidget* parent)
{
    QWidget* w(NULL);
    
    /*
     * Create widget and attach to signal
     */
    switch (m_widgetType) {
        case WidgetType::CHECK_BOX_BOOLEAN:
        {
            QCheckBox* cb = new QCheckBox(parent);
            QObject::connect(cb, &QCheckBox::clicked,
                             this, [=](bool checked) { emit valueChanged(checked); });
            w = cb;
        }
            break;
        case WidgetType::COMBO_BOX_STRING_LIST:
        {
            QComboBox* cb = new QComboBox(parent);
            QObject::connect(cb, QOverload<const QString&>::of(&QComboBox::activated),
                             this, [=](const QString& value) { emit valueChanged(value); } );
            w = cb;
        }
            break;
        case WidgetType::LINE_EDIT_STRING:
        {
            QLineEdit* le = new QLineEdit(parent);
            QObject::connect(le, QOverload<const QString&>::of(&QLineEdit::textEdited),
                             this, [=](const QString& value) { emit valueChanged(value); } );
            w = le;
        }
            break;
        case WidgetType::SPIN_BOX_FLOAT:
        {
            QDoubleSpinBox* sb = new QDoubleSpinBox(parent);
            QObject::connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                             this, [=](const double value) { emit valueChanged(static_cast<float>(value)); } );
            w = sb;
        }
            break;
        case WidgetType::SPIN_BOX_INTEGER:
        {
            QSpinBox* sb = new QSpinBox(parent);
            QObject::connect(sb, QOverload<int>::of(&QSpinBox::valueChanged),
                             this, [=](const int value) { emit valueChanged(value); } );
            w = sb;
        }
            break;
    }
    
    CaretAssert(w);
    
    w->setToolTip(m_toolTip);
    w->setObjectName(objectName()
                     + ":"
                     + w->metaObject()->className());
    
    m_widgets.insert(w);
    
    updateWidgetWithModelValue(w);
    
    return w;
}

/**
 * Release the specified widget. Any signals are disconnected
 * and this widget is no longer updated.  The caller is
 * responsible for deleting the widget.
 *
 * It is okay to call this method with a widget
 * that belongs to another macro widget action
 * and in this case no action is taken.
 *
 * @return
 *     True if the widget was monitored by this
 *     macro widget action, otherwise false.
 */
bool
WuQMacroWidgetAction::releaseWidget(QWidget* widget)
{
    CaretAssert(widget);
    
    if (m_widgets.find(widget) != m_widgets.end()) {
        /*
         * Disconnect all signals in widget from 'this'
         */
        QObject::disconnect(widget, 0,
                            this, 0);
        
        m_widgets.erase(widget);
        
        std::cout << "Released widget: " << widget->objectName() << std::endl;
        return true;
    }

    /*
     * If here, widget was not in this macro widget action
     */
    return false;
}

/**
 * @return The tooltip
 */
QString
WuQMacroWidgetAction::getToolTip() const
{
    return m_toolTip;
}

/**
 * @return The name of the widget action
 */
QString
WuQMacroWidgetAction::getName() const
{
    return objectName();
}

/**
 * Set the data value but DOES NOT emit valueChanged() signal
 *
 * @param value
 *     New data value for widget action
 */
void
WuQMacroWidgetAction::setDataValue(const QVariant& value)
{
    for (auto w : m_widgets) {
        setWidgetValue(w,
                       value);
    }
}

/**
 * Set the value and will emit the valueChanged signal
 *
 * @param value
 *     New value.
 */
void
WuQMacroWidgetAction::setValuePrivate(const QVariant& value)
{
    for (auto w : m_widgets) {
        setWidgetValue(w,
                       value);
    }
    
    emit setModelValue(value);
}

/**
 * Set a widget's value
 *
 * @param widget
 *     The widget
 * @param value
 *     Value for the widget
 */
void
WuQMacroWidgetAction::setWidgetValue(QWidget* widget,
                                     const QVariant& value)
{
    /*
     * Some widgets, such as spin box, will emit a signal
     * when its value is changed so block its signals
     */
    QSignalBlocker sb(widget);
    
    switch (m_widgetType) {
        case WidgetType::CHECK_BOX_BOOLEAN:
        {
            QCheckBox* cb = qobject_cast<QCheckBox*>(widget);
            CaretAssert(cb);
            cb->setChecked(value.toBool());
        }
            break;
        case WidgetType::COMBO_BOX_STRING_LIST:
        {
            QComboBox* cb  = qobject_cast<QComboBox*>(widget);
            CaretAssert(cb);
            cb->setCurrentText(value.toString());
        }
            break;
        case WidgetType::LINE_EDIT_STRING:
        {
            QLineEdit* le = qobject_cast<QLineEdit*>(widget);
            CaretAssert(le);
            le->setText(value.toString());
        }
            break;
        case WidgetType::SPIN_BOX_FLOAT:
        {
            QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(widget);
            CaretAssert(sb);
            sb->setValue(value.toFloat());
        }
            break;
        case WidgetType::SPIN_BOX_INTEGER:
        {
            QSpinBox* sb = qobject_cast<QSpinBox*>(widget);
            CaretAssert(sb);
            sb->setValue(value.toInt());
        }
            break;
    }
}

/**
 * May be called when a dialog is created or updated
 * to update the model value in the given widget
 *
 * @param widget
 *     The widget
 * @return
 *     True if widget was updated with model value
 */
bool
WuQMacroWidgetAction::updateWidgetWithModelValue(QWidget* widget)
{
    CaretAssert(widget);
    if (m_widgets.find(widget) != m_widgets.end()) {
        QVariant value;
        emit getModelValue(value);
        
        if (value.isNull()) {
            const QString msg("Value was not updated by model, need to connect getModelValue() signal");
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
        }
        else {
            setWidgetValue(widget,
                           value);
            return true;
        }
    }
    
    return false;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
WuQMacroWidgetAction::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

