
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

#include <QCheckBox>
#include <QWidget>

#include "WuQWidgetObjectGroup.h"

using namespace caret;

/**
 * constructor.
 */
WuQWidgetObjectGroup::WuQWidgetObjectGroup(QWidget* parent)
   : QObject(parent)
{
}

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
   //
   // Note: Do not "delete" the widgets !!!!
   //
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
WuQWidgetObjectGroup::blockSignals(bool blockTheSignals)
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


