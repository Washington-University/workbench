
#ifndef __QT_WIDGET_OBJECT_GROUP_H__
#define __QT_WIDGET_OBJECT_GROUP_H__

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

#include <QObject>
#include <QVector>

class QLayout;
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
        
        void addLayout(QLayout* layout);
        
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
