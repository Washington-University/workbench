
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __CZI_UTILITIES_DECLARE__
#include "CziUtilities.h"
#undef __CZI_UTILITIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::CziUtilities 
 * \brief Utilities for working with CZI files
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziUtilities::CziUtilities()
{
    
}

/**
 * Destructor.
 */
CziUtilities::~CziUtilities()
{
}

/**
 * Convert a libCZI IntRect to a Qt QRect
 * @param intRect
 *    The libCZI IntRect instance
 * @return
 *    A Qt QRectt
 */
QRect
CziUtilities::intRectToQRect(const libCZI::IntRect& intRect)
{
    QRect r;
    
    if (intRect.IsValid()) {
        r.setRect(intRect.x, intRect.y, intRect.w, intRect.h);
    }
    return r;
}

/**
 * Convert a Qt QRect to a libCZI IntRect
 * @param qRect
 *    The Qt QRect instance
 * @return
 *    A libCZI IntRect
 */
libCZI::IntRect
CziUtilities::qRectToIntRect(const QRect& qRect)
{
    libCZI::IntRect r;
    r.Invalidate();
    
    if (qRect.isValid()) {
        r.x = qRect.x();
        r.y = qRect.y();
        r.w = qRect.width();
        r.h = qRect.height();
    }
    
    return r;
}
