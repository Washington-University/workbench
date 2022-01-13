
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

#include <cmath>

#define __CZI_UTILITIES_DECLARE__
#include "CziUtilities.h"
#undef __CZI_UTILITIES_DECLARE__

#include "AString.h"
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
 * @return-
 *    A Qt QRectt
 */
QRectF
CziUtilities::intRectToQRect(const libCZI::IntRect& intRect)
{
    QRectF r;
    
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
CziUtilities::qRectToIntRect(const QRectF& qRect)
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

/**
 * @return A string representing the given CZI IntRect instance
 * @param intRect
 *    The CZI IntRect
 */
QString
CziUtilities::intRectToString(const libCZI::IntRect& intRect)
{
    QString s("Invalid");
    if (intRect.IsValid()) {
        s = QString("x=%1, y=%2, width=%3, height=%4").arg(intRect.x).arg(intRect.y).arg(intRect.w).arg(intRect.h);
    }
    return s;
}

/**
 * @return A string representing the given QRect instance
 * @param qRect
 *    The QRect
 */
QString
CziUtilities::qRectToString(const QRectF& qRect)
{
    QString s("Invalid");
    if (qRect.isValid()) {
        s = QString("x=%1, y=%2, width=%3, height=%4").arg(qRect.x()).arg(qRect.y()).arg(qRect.width()).arg(qRect.height());
    }
    return s;
}

/**
 * @return A string representing the given QRect instance as Left, Right, Bottom, Top
 * @param qRect
 *    The QRect
 */
QString
CziUtilities::qRectToLrbtString(const QRectF& qRect)
{
    QString s("Invalid");
    if (qRect.isValid()) {
        s = QString("left=%1, right=%2, bottom=%3, top=%4").arg(qRect.left()).arg(qRect.right()).arg(qRect.bottom()).arg(qRect.top());
    }
    return s;
}

/**
 * @return True if the rectangles are equal using the given tolerance
 * @param rectOne
 *    FIrst rectangle
 * @param rectTwo
 *    Second rectangle
 * @param tolerance
 *    Distance between corresponding edges must be within this value
 */
bool
CziUtilities::equalWithTolerance(const QRectF& rectOne,
                                 const QRectF& rectTwo,
                                 const float tolerance)
{
    const float tol(5.0);
    if (   (std::fabs(rectOne.left()   - rectTwo.left())   < tolerance)
        && (std::fabs(rectOne.right()  - rectTwo.right())  < tolerance)
        && (std::fabs(rectOne.top()    - rectTwo.top())    < tolerance)
        && (std::fabs(rectOne.bottom() - rectTwo.bottom()) < tolerance)) {
        return true;
    }
    return false;
}

/**
 * @return Area of the intersection of the two given rectangles
 * @param rectOne
 *    FIrst rectangle
 * @param rectTwo
 *    Second rectangle
 */
float
CziUtilities::intersectionArea(const QRectF& rectOne,
                               const QRectF& rectTwo)
{
    float area(0.0);
    
    if (rectOne.intersects(rectTwo)) {
        const QRectF intersectionRect(rectOne.intersected(rectTwo));
        area = (intersectionRect.width()
                * intersectionRect.height());
    }
    
    return area;
}

/**
 * @return The given rectangle expanded by the given percentage
 * @param rect
 *    The rectangle
 * @param expandPercentage
 *    Percentage 1.0 = one percent; 100.0 = one-hundred percent
 *    Use negative value to contract
 */
QRectF
CziUtilities::expandByPercentage(const QRectF rect,
                                 const float expandPercentage)
{
    const float halfPct(expandPercentage / 200.0);
    const float widthToAdd(rect.width() * halfPct);
    const float heightToAdd(rect.height() * halfPct);
    
    const QMargins marginsToAdd(widthToAdd,   /* top */
                                heightToAdd,  /* left */
                                widthToAdd,   /* right */
                                heightToAdd); /* bottom */
    const QRectF newRect(rect.marginsAdded(marginsToAdd));
    
    return newRect;
}


