#ifndef __WU_Q_COLLAPSIBLE_WIDGET__H_
#define __WU_Q_COLLAPSIBLE_WIDGET__H_

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


#include <QVector>
#include <QWidget>

class QAction;
class QActionGroup;
class QVBoxLayout;

namespace caret {

    class WuQCollapsibleWidget : public QWidget {
        
        Q_OBJECT

    public:
        WuQCollapsibleWidget(QWidget* parent = 0);
        
        virtual ~WuQCollapsibleWidget();
        
        void addItem(QWidget* widget,
                     const QString& text);
        
        virtual QSize sizeHint() const;
        
    private slots:
        void showHideActionGroupTriggered(QAction*);
        
    private:
        WuQCollapsibleWidget(const WuQCollapsibleWidget&);

        WuQCollapsibleWidget& operator=(const WuQCollapsibleWidget&);
        
        QVBoxLayout* collapsibleLayout;
        
        QActionGroup* showHideActionGroup;
        
        QVector<QWidget*> widgets;
    };
    
#ifdef __WU_Q_COLLAPSIBLE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_COLLAPSIBLE_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_COLLAPSIBLE_WIDGET__H_
