#ifndef __WU_Q_GRID_LAYOUT_GROUP__H_
#define __WU_Q_GRID_LAYOUT_GROUP__H_

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

#include <QVector>

class QGridLayout;

namespace caret {

    class WuQGridLayoutGroup : public QObject {
        
        Q_OBJECT

    public:
        WuQGridLayoutGroup(QGridLayout* gridLayout,
                           QObject* parent = 0);
        
        virtual ~WuQGridLayoutGroup();
        
        void addWidget(QWidget* widget,
                       int row,
                       int column,
                       Qt::Alignment alignment = Qt::Alignment());
        
        void addWidget(QWidget* widget,
                       int fromRow,
                       int fromColumn,
                       int rowSpan,
                       int columnSpan,
                       Qt::Alignment alignment = Qt::Alignment());
        
        int rowCount() const;
        
        int columnCount() const;
        
        void setVisible(bool visible);
        
    private:
        WuQGridLayoutGroup(const WuQGridLayoutGroup&);

        WuQGridLayoutGroup& operator=(const WuQGridLayoutGroup&);
        
        class ItemRowCol {
        public:
            ItemRowCol(QWidget *widget,
                       int fromRow,
                       int fromColumn,
                       int rowSpan,
                       int columnSpan,
                       Qt::Alignment alignment);
            
            QWidget* widget;
            int fromRow;
            int fromColumn;
            int rowSpan;
            int columnSpan;
            Qt::Alignment alignment;
        };
        
        QGridLayout* gridLayout;
        
        bool areWidgetsInLayout;
      
        QVector<ItemRowCol*> layoutItems;
    };
    
#ifdef __WU_Q_GRID_LAYOUT_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_GRID_LAYOUT_GROUP_DECLARE__

} // namespace
#endif  //__WU_Q_GRID_LAYOUT_GROUP__H_
