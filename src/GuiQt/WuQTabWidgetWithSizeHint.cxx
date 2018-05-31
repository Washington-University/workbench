
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_TAB_WIDGET_WITH_SIZE_HINT_DECLARE__
#include "WuQTabWidgetWithSizeHint.h"
#undef __WU_Q_TAB_WIDGET_WITH_SIZE_HINT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQTabWidgetWithSizeHint 
 * \brief A QTabWidget that allows user to set size hint width or height
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * 
 * @param parent
 *     Optional parent widget.
 */
WuQTabWidgetWithSizeHint::WuQTabWidgetWithSizeHint(QWidget* parent)
: QTabWidget(parent)
{
    
}

/**
 * Destructor.
 */
WuQTabWidgetWithSizeHint::~WuQTabWidgetWithSizeHint()
{
}

/**
 * Set the size hint width (non-positive uses default size hint for width)
 *
 * @param width
 *     Width for size hint.
 */
void
WuQTabWidgetWithSizeHint::setSizeHintWidth(const int width)
{
    m_sizeHintWidth = width;
}

/**
 * Set the size hint height (non-positive uses default size hint for height)
 *
 * @param height
 *     Height for size hint.
 */
void
WuQTabWidgetWithSizeHint::setSizeHintHeight(const int height)
{
    m_sizeHintHeight = height;
}

/**
 * @return Recommended size for this widget.
 */
QSize
WuQTabWidgetWithSizeHint::sizeHint() const
{
    QSize sh = QTabWidget::sizeHint();
    
    if (m_sizeHintWidth > 0) {
        sh.setWidth(m_sizeHintWidth);
    }
    if (m_sizeHintHeight > 0) {
        sh.setHeight(m_sizeHintHeight);
    }
    
    return sh;
}
