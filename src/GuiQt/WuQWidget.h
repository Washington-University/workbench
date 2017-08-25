#ifndef __WU_Q_WIDGET__H_
#define __WU_Q_WIDGET__H_

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


#include <QObject>

class QWidget;

namespace caret {

    class WuQWidget : public QObject {
        
        Q_OBJECT

    public:
        WuQWidget(QObject* parent);
        
        virtual ~WuQWidget();
        
        virtual QWidget* getWidget() = 0;
        
        void setToolTip(const QString& tooltip);
        
        void setFixedWidth(int w);

    private:
        WuQWidget(const WuQWidget&);

        WuQWidget& operator=(const WuQWidget&);
        
    public:
    private:
    };
    
#ifdef __WU_Q_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_WIDGET__H_
