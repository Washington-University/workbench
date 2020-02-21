
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __WU_Q_SCROLL_AREA_DECLARE__
#include "WuQScrollArea.h"
#undef __WU_Q_SCROLL_AREA_DECLARE__

#include <iostream>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQScrollArea 
 * \brief Scroll area that allows a specific fixed width and/or height
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param fixedWidth
 *  If positive, scroll area will be this widgth
 * @param fixedHeight
 *  If positive, scroll area will be this height
 */
WuQScrollArea::WuQScrollArea(const int32_t fixedWidth,
                             const int32_t fixedHeight)
: QScrollArea(),
m_fixedWidth(fixedWidth),
m_fixedHeight(fixedHeight)
{
}

/**
 * Destructor.
 */
WuQScrollArea::~WuQScrollArea()
{
}

/**
 * @param fixedWidth
 *  If positive, scroll area will be this widgth
 * @param fixedHeight
 *  If positive, scroll area will be this height
 * @return New instance of scroll area
 */
WuQScrollArea*
WuQScrollArea::newInstance(const int32_t fixedWidth,
                           const int32_t fixedHeight)
{
    WuQScrollArea* sa = new WuQScrollArea(fixedWidth,
                                          fixedHeight);
    
    /*
     * These are virtual functions so cannot be called
     * from a constructor
     */
    QSizePolicy sp(sa->sizePolicy());
    if (fixedWidth > 0) {
        sp.setHorizontalPolicy(QSizePolicy::Fixed);
    }
    if (fixedHeight > 0) {
        sp.setVerticalPolicy(QSizePolicy::Fixed);
    }
    sa->setSizePolicy(sp);
    
    return sa;
}

QSize
WuQScrollArea::sizeHint() const
{
    QSize sz = QScrollArea::sizeHint();
    
    if (m_fixedWidth > 0) {
        sz.setWidth(m_fixedWidth);
    }
    if (m_fixedHeight > 0) {
        sz.setHeight(m_fixedHeight);
    }
    
    return sz;
}

