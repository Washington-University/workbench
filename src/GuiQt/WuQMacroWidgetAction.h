#ifndef __WU_Q_MACRO_WIDGET_ACTION_H__
#define __WU_Q_MACRO_WIDGET_ACTION_H__

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

#include <memory>
#include <set>

#include <QObject>

#include "EventListenerInterface.h"
#include "WuQMacroDataValueTypeEnum.h"

namespace caret {

//    class ModelPropertySetGet {
//    public:
//        virtual QVariant getValue() const = 0;
//        virtual void setValue(const QVariant& value) = 0;
//    };
    
    class WuQMacroWidgetAction : public QObject, public EventListenerInterface {
        
        Q_OBJECT

    public:
        /**
         * Type of widget
         */
        enum class WidgetType {
            /** CheckBox for boolean value */
            CHECK_BOX_BOOLEAN,
            /** Combo Box for string list selection */
            COMBO_BOX_STRING_LIST,
            /** Line edit for text */
            LINE_EDIT_STRING,
            /** Spin box for float value */
            SPIN_BOX_FLOAT,
            /** Spin box for integer value */
            SPIN_BOX_INTEGER
        };
        
        WuQMacroWidgetAction(const WidgetType widgetType,
                             const QString& objectName,
                             const QString& objectToolTip,
                             QObject* parent);
        
        static void initialize(std::map<QString, WidgetType>& namesAndTypes);
        
        static void destroy();
        
        virtual ~WuQMacroWidgetAction();
        
        WuQMacroWidgetAction(const WuQMacroWidgetAction&) = delete;

        WuQMacroWidgetAction& operator=(const WuQMacroWidgetAction&) = delete;
        
        QWidget* requestWidget(QWidget* parent);
        
        bool releaseWidget(QWidget* widget);
        
        WidgetType getWidgetType() const;

        QString getName() const;
        
        QString getToolTip() const;
        
        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

        void setDataValue(const QVariant& value);
        
        void updateWidgetWithModelValue(QWidget* widget);
        
    signals:
        /**
         * Emitted when the value changes.  Used by macro signal watcher.
         *
         * @param value
         *     The new value
         */
        void valueChanged(const QVariant& value);
        
        /**
         * Connect this signal to get the model's data value
         *
         * @param valueOut
         *     Reference for signal receiver to set with model's data value
         */
        void getModelValue(QVariant& valueOut);
        
        /**
         * Connect this signal to set the model's data value
         *
         * @param value
         *     New value for model
         */
        void setModelValue(const QVariant& value);
        
    private slots:
        void setValuePrivate(const QVariant& value);
        
    private:
        void setWidgetValue(QWidget* widget,
                            const QVariant& value);
        
        const WidgetType m_widgetType;
        
        const QString m_toolTip;
        
        std::set<QWidget*> m_widgets;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_WIDGET_ACTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_WIDGET_ACTION_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_WIDGET_ACTION_H__
