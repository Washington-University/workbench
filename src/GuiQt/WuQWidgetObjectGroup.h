
#ifndef __QT_WIDGET_OBJECT_GROUP_H__
#define __QT_WIDGET_OBJECT_GROUP_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <QObject>
#include <QVector>

class QObject;

namespace caret {
    /**
     * Groups QWidget and/or QObjects for applying operations to
     * all such as blocking signals and setting visibility.
     */
    class WuQWidgetObjectGroup : public QObject {
        Q_OBJECT
        
    public:
        //WuQWidgetObjectGroup(QWidget* parent);
        
        WuQWidgetObjectGroup(QObject* parent);
        
        ~WuQWidgetObjectGroup();
        
        void add(QObject* w);
        
        void clear();
        
        QObject* getObject() { return dynamic_cast<QObject*>(this); }
        
    public slots:
        void blockAllSignals(bool blockTheSignals);
        
        void setEnabled(bool enable);
        
        void setDisabled(bool disable);
        
        void setVisible(bool makeVisible);
        
        void setHidden(bool hidden);
        
        void resizeAllToLargestSizeHint();
        
        void setAllCheckBoxesChecked(const bool b);
        
    protected:
        QVector<QObject*> objects;
        
    private:
        // prevent access to QObject's blockSignals() method
        bool blockSignals(bool);
    };
} // namespace

#endif // __QT_WIDGET_OBJECT_GROUP_H__
