#ifndef __WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_H__
#define __WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include <stdint.h>

#include <vector>
#include <QWidget>

class QAbstractButton;
class QButtonGroup;
class QFrame;
class QGridLayout;
class QRadioButton;

namespace caret {

    class WuQGroupBoxExclusiveWidget : public QWidget {
        
        Q_OBJECT

    public:
        WuQGroupBoxExclusiveWidget(QWidget* parent = 0);
        
        virtual ~WuQGroupBoxExclusiveWidget();
        
        int32_t addWidget(QWidget* widget,
                          const QString& textLabel);
        
        int32_t count() const;
        
        int32_t currentIndex() const;
        
        QWidget* currentWidget() const;

        int32_t indexOf(QWidget* widget) const;
        
        QWidget* widget(int32_t index) const;
        
        void setWidgetEnabled(int32_t index,
                              const bool enabled);
        
        bool isWidgetEnabled(const int32_t index) const;
        
        // ADD_NEW_METHODS_HERE

    signals:
        /**
         * Emitted when the user changes the selected widget.
         *
         * @param 
         *     Index of the selected widget.
         */
        void currentChanged(int32_t index);
        
    public slots:
        void setCurrentIndex(int32_t index);
        
        void setCurrentWidget(QWidget* widget);
        
    private slots:
        void radioButtonClicked(QAbstractButton* button);
        
    private:
        WuQGroupBoxExclusiveWidget(const WuQGroupBoxExclusiveWidget&);

        WuQGroupBoxExclusiveWidget& operator=(const WuQGroupBoxExclusiveWidget&);

        void updateSelectionWithValidWidget();
        
        // ADD_NEW_MEMBERS_HERE

        struct WidgetData {
            QWidget* m_widget;
            
            QFrame* m_frameBox;
            
            QRadioButton* m_radioButton;
        };
        
        
        std::vector<WidgetData> m_widgetDatas;
        
        QButtonGroup* m_radioButtonGroup;
        
        QGridLayout* m_widgetsGridLayout;
        
        mutable int32_t m_currentWidgetIndex;
    };
    
#ifdef __WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_GROUP_BOX_EXCLUSIVE_WIDGET_H__
