
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

#define __LINEAR_EQUATION_TRANSFORM_DECLARE__
#include "LinearEquationTransform.h"
#undef __LINEAR_EQUATION_TRANSFORM_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::LinearEquationTransform 
 * \brief Creates transform using a linear equation (y = mx + b)
 * \ingroup Common
 */

/**
 * Constructor creates a linear transform from the given points (y = mx + b).
 * @param x1
 *    First X-value
 * @param x2
 *    Second X-value
 * @param y1
 *    First Y-value
 * @param y2
 *    Second Y-value
 * @param x0
 *    An X value corresponding to y0
 * @param y0
 *    A Y value corresponding to x0
 */
LinearEquationTransform::LinearEquationTransform(const float x1,
                                                 const float x2,
                                                 const float y1,
                                                 const float y2,
                                                 const float x0,
                                                 const float y0)
: CaretObject()
{
    const float dy(y2 - y1);
    const float dx(x2 - x1);
    if ((dx == 0.0)
        && (dy == 0.0)) {
        /*
         * Not a rectangle but is a point
         */
        m_errorMessage = "Not a rectangle, both X and Y-points are coincident";
        return;
    }
    else if (dy == 0.0) {
        /*
         * Horizontal line
         */
        m_errorMessage = "Not a rectangle but is horizontal line";
        return;
    }
    else if (dx == 0.0) {
        /*
         * Vertical line
         */
        m_errorMessage = "Not a rectangle but is vertical line";
        return;
    }
    
    CaretAssert(dy != 0.0);
    CaretAssert(dx != 0.0);
    
    m_slope     = (dy / dx);
    m_intercept = y0 - (m_slope * x0);

    m_valid = true;
}

/**
 * Destructor.
 */
LinearEquationTransform::~LinearEquationTransform()
{
}

/**
 * Create a linear transform from the given points (y = mx + b).
 * @param x1
 *    First X-value
 * @param x2
 *    Second X-value
 * @param y1
 *    First Y-value
 * @param y2
 *    Second Y-value
 * @param x0
 *    An X value corresponding to y0
 * @param y0
 *    A Y value corresponding to x0
 * @param errorMessageOut
 *    Contains description of any error
 * @return
 *    Pointer to the transform or NULL if there is an error.
 */
std::unique_ptr<LinearEquationTransform>
LinearEquationTransform::newInstance(const float x1,
                                     const float x2,
                                     const float y1,
                                     const float y2,
                                     const float x0,
                                     const float y0,
                                     AString& errorMessageOut)
{
    std::unique_ptr<LinearEquationTransform> ptr;
    ptr.reset(new LinearEquationTransform(x1,
                                          x2,
                                          y1,
                                          y2,
                                          x0,
                                          y0));
    if ( ! ptr->m_valid) {
        CaretAssert( ! ptr->m_errorMessage.isEmpty());
        errorMessageOut = ptr->m_errorMessage;
        ptr.reset();
    }
    
    return ptr;
}

/**
 * Transform the given X-value to a Y-value
 */
float
LinearEquationTransform::transform(const float x) const
{
    float yOut = m_intercept + (m_slope * x);
    return yOut;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
LinearEquationTransform::toString() const
{
    return ("LinearEquationTransform m="
            + AString::number(m_slope)
            + ", b="
            + AString::number(m_intercept));
}

