
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

#define __WU_Q_WIDGET_DECLARE__
#include "WuQWidget.h"
#undef __WU_Q_WIDGET_DECLARE__

#include <QWidget>

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::WuQWidget 
 * \brief Class for extending Qt GUI Widgets through encapsulation
 *
 * It is often desirable to extend a Qt GUI widget but one does
 * not want to directly subclass a QWidget.  For example, a 
 * QComboBox is a useful control for selection of an enumerated
 * type.  It is desirable to allow the user of the class to only
 * access the combo box by setting and getting the enumerated type.
 * However, if one directly subclasses QComboBox, it allows access
 * to many methods including those that get and set via an index
 * or name which could allow the insertion of invalid values.  So,
 * by encapsualting, one can provide accessor methods using the
 * enumerated types.  Protected inheritance is not a viable solution
 * because it prevents the connection of signals and slots.
 *
 * This class is not derived from QWidget since that would require.
 * the additional of a layout to hold the actual widget.  Instead,
 * this class is derived from QObject so that there are no 'widget'
 * methods available to the user and so that the signal and slot
 * mechanism is available.  Subclasses can define, signals and slots
 * that are appropriate, such as those that use an enumerated type
 * as a parameter.
 *
 * The parent of derived classes must be passed to the contructor.
 * The parent is typically some deriviative of QWidget such as 
 * QDialog.  By using a parent, Qt will destroy an instance of 
 * this class when the parent is destroyed.
 *
 * An instance of this class is never added to a layout.  Instead,
 * deriving classes implement the getWidget() method to provide
 * the enapsulated widget for insertion into a layout.
 *
 * Since the encapsulated QWidget is added to a layout, never
 * delete the encapsulated widget since it will have a Qt parent
 * which will destroy it.
 */


/**
 * Constructor.
 */
WuQWidget::WuQWidget(QObject* parent)
: QObject(parent)
{
    CaretAssert(parent);
}

/**
 * Destructor.
 */
WuQWidget::~WuQWidget()
{
}

/**
 * Sets both the minimum and maximum width of the widget to w without changing the heights.
 *
 * @param width
 *     Width value.
 */
void
WuQWidget::setFixedWidth(int width)
{
    QWidget* widget = getWidget();
    CaretAssert(widget);
    widget->setFixedWidth(width);
}


/**
 * Set the tooltip for this spin box.
 *
 * @param tooltip
 *     Text for the tooltip.
 */
void
WuQWidget::setToolTip(const QString& tooltip)
{
    QWidget* widget = getWidget();
    CaretAssert(widget);
    widget->setToolTip(tooltip);
}



