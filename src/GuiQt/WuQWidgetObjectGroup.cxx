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

#include <iostream>

#include <QCheckBox>
#include <QWidget>

#include "WuQWidgetObjectGroup.h"

using namespace caret;

/**
 * constructor.
 *
WuQWidgetObjectGroup::WuQWidgetObjectGroup(QWidget* parent)
   : QObject(parent)
{
}
*/

/**
 * constructor.
 */
WuQWidgetObjectGroup::WuQWidgetObjectGroup(QObject* parent)
: QObject(parent)
{
}

/**
 * destructor.
 */
WuQWidgetObjectGroup::~WuQWidgetObjectGroup()
{
    this->clear();
}

/**
 * Remove all objects from this group.  The objects are NOT deleted
 * since widgets are typically 'owned' by their parents.
 */
void 
WuQWidgetObjectGroup::clear()
{
    this->objects.clear();
}

/**
 * add a QObject (QWidget is descendent of QObject) to the group.
 */
void 
WuQWidgetObjectGroup::add(QObject* o)
{
   this->objects.push_back(o);
}

/**
 * enable the group's widgets.
 */
void 
WuQWidgetObjectGroup::setEnabled(bool enable)
{
   for (int i = 0; i < this->objects.size(); i++) {
       QWidget* widget = dynamic_cast<QWidget*>(this->objects[i]);
       if (widget != NULL) {
           widget->setEnabled(enable);
       }
   }
}

/**
 * disable the group's widgets.
 */
void 
WuQWidgetObjectGroup::setDisabled(bool disable)
{
   for (int i = 0; i < this->objects.size(); i++) {
       QWidget* widget = dynamic_cast<QWidget*>(this->objects[i]);
       if (widget != NULL) {
           widget->setDisabled(disable);
       }
   }
}

/**
 * @return true if any of the widgets are visible.
 */
bool
WuQWidgetObjectGroup::isVisible() const
{
    for (int i = 0; i < this->objects.size(); i++) {
        QWidget* widget = dynamic_cast<QWidget*>(this->objects[i]);
        if (widget != NULL) {
            if (widget->isVisible()) {
                return true;
            }
        }
    }
    return false;
}

/**
 * make the group's widgets visible.
 */
void 
WuQWidgetObjectGroup::setVisible(bool makeVisible)
{
   for (int i = 0; i < this->objects.size(); i++) {
       QWidget* widget = dynamic_cast<QWidget*>(this->objects[i]);
       if (widget != NULL) {
           widget->setVisible(makeVisible);
       }
   }
}

/**
 * make the group's widgets hidden.
 */
void 
WuQWidgetObjectGroup::setHidden(bool hidden)
{
   setVisible(! hidden);
}

/**
 * block signals.  
 */
void 
WuQWidgetObjectGroup::blockAllSignals(bool blockTheSignals)
{
   for (int i = 0; i < this->objects.size(); i++) {
      this->objects.at(i)->blockSignals(blockTheSignals);
   }
}
      
/**
 * set status of all checkboxes.
 */
void 
WuQWidgetObjectGroup::setAllCheckBoxesChecked(const bool b)
{
   for (int i = 0; i < this->objects.size(); i++) {
      QCheckBox* cb = dynamic_cast<QCheckBox*>(this->objects.at(i));
      if (cb != NULL) {
         cb->setChecked(b);
      }
   }
}
      
/** 
 * make all of the widgets in the group the same size as size hint
 * of largest widget.
 */
void 
WuQWidgetObjectGroup::resizeAllToLargestSizeHint()
{
    int largestWidth  = -1;
    int largestHeight = -1;
    
    for (int i = 0; i < this->objects.size(); i++) {
        QWidget* widget = dynamic_cast<QWidget*>(this->objects[i]);
        if (widget == NULL) {
            continue;
        }
        const QSize size = widget->sizeHint();
        if (size.width() > largestWidth) {
            largestWidth = size.width();
        }
        if (size.height() > largestHeight) {
            largestHeight = size.height();
        }
    }
    
    if ((largestWidth > 0) &&
        (largestHeight > 0)) {
        QSize newSize(largestWidth, largestHeight);
        
        for (int i = 0; i < this->objects.size(); i++) {
            QWidget* widget = dynamic_cast<QWidget*>(this->objects[i]);
            if (widget != NULL) {
                widget->setFixedSize(newSize);
            }
        }
    }
}


