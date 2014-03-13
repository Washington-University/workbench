
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

#define __CHART_POINT_DECLARE__
#include "ChartPoint.h"
#undef __CHART_POINT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ChartPoint 
 * \brief Point displayed in a chart.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param x
 *    X coordinate of point.
 * @param y
 *    Y coordinate of point.
 */
ChartPoint::ChartPoint(const float x,
                       const float y)
: CaretObject()
{
    m_xyz[0] = x;
    m_xyz[1] = y;
    m_xyz[2] = 0.0;
}

/**
 * Destructor.
 */
ChartPoint::~ChartPoint()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartPoint::ChartPoint(const ChartPoint& obj)
: CaretObject(obj)
{
    this->copyHelperChartPoint(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartPoint&
ChartPoint::operator=(const ChartPoint& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartPoint(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartPoint::copyHelperChartPoint(const ChartPoint& obj)
{
    m_xyz[0] = obj.m_xyz[0];
    m_xyz[1] = obj.m_xyz[1];
    m_xyz[2] = obj.m_xyz[2];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartPoint::toString() const
{
    return "ChartPoint";
}

/**
 * @return Pointer to X & Y coordinates.
 */
const float*
ChartPoint::getXY() const
{
    return &m_xyz[0];
}

/**
 * @return X coordinate.
 */
float
ChartPoint::getX() const
{
    return m_xyz[0];
}

/**
 * @return Y coordinate.
 */
float
ChartPoint::getY() const
{
    return m_xyz[1];
}


