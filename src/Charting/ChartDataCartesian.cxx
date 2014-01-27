
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CHART_DATA_CARTESIAN_DECLARE__
#include "ChartDataCartesian.h"
#undef __CHART_DATA_CARTESIAN_DECLARE__

#include <limits>

#include "CaretAssert.h"
#include "ChartPoint.h"

using namespace caret;


    
/**
 * \class caret::ChartDataCartesian 
 * \brief Chart cartesian data.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *   Type of chart data model.
 * @param dataAxisUnitsX
 *   Data units for X-axis.
 * @param dataAxisUnitsY
 *   Data units for Y-axis.
 */
ChartDataCartesian::ChartDataCartesian(const ChartDataTypeEnum::Enum chartDataType,
                                                 const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                                 const ChartAxisUnitsEnum::Enum dataAxisUnitsY)
: ChartData(chartDataType),
m_dataAxisUnitsX(dataAxisUnitsX),
m_dataAxisUnitsY(dataAxisUnitsY)
{
    m_boundsValid       = false;
    m_color             = CaretColorEnum::BLACK;
    m_description       = "";
    m_encodedDataSource = "";
    m_timeStartInSecondsAxisX = 0.0;
    m_timeStepInSecondsAxisX  = 1.0;
}

/**
 * Destructor.
 */
ChartDataCartesian::~ChartDataCartesian()
{
    destroyAllPoints();
}

/**
 * Destroy (remove) all points in the model.
 */
void
ChartDataCartesian::destroyAllPoints()
{
    for (std::vector<ChartPoint*>::const_iterator iter = m_points.begin();
         iter != m_points.end();
         iter++) {
        delete *iter;
    }
    m_points.clear();
    
    m_boundsValid = false;
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartDataCartesian::ChartDataCartesian(const ChartDataCartesian& obj)
: ChartData(obj),
m_dataAxisUnitsX(obj.m_dataAxisUnitsX),
m_dataAxisUnitsY(obj.m_dataAxisUnitsY)
{
    this->copyHelperChartDataCartesian(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartDataCartesian&
ChartDataCartesian::operator=(const ChartDataCartesian& obj)
{
    if (this != &obj) {
        ChartData::operator=(obj);
        this->copyHelperChartDataCartesian(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartDataCartesian::copyHelperChartDataCartesian(const ChartDataCartesian& obj)
{
    destroyAllPoints();

    for (std::vector<ChartPoint*>::const_iterator iter = obj.m_points.begin();
         iter != obj.m_points.end();
         iter++) {
        const ChartPoint* cp = *iter;
        m_points.push_back(new ChartPoint(*cp));
    }

    m_boundsValid = false;
}

/**
 * Add a point.
 *
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 */
void
ChartDataCartesian::addPoint(const float x,
                                  const float y)
{
    m_points.push_back(new ChartPoint(x, y));
    m_boundsValid = false;
}

/**
 * @return Number of points.
 */
int32_t
ChartDataCartesian::getNumberOfPoints() const
{
    return m_points.size();
}

/**
 * Get the point at the given index.
 *
 * @param pointIndex
 *    Index of point.
 * @return
 *    Point at the given index.
 */
const ChartPoint*
ChartDataCartesian::getPointAtIndex(const int32_t pointIndex) const
{
    CaretAssertVectorIndex(m_points, pointIndex);
    return m_points[pointIndex];
}

/**
 * Get the bounds of all of the points.
 *
 * @param xMinimumOut
 *     Minimum X-coordinate of all points.
 * @param xMaximumOut
 *     Maximum X-coordinate of all points.
 * @param yMinimumOut
 *     Minimum Y-coordinate of all points.
 * @param yMaximumOut
 *     Maximum Y-coordinate of all points.
 */
void
ChartDataCartesian::getBounds(float& xMinimumOut,
                                   float& xMaximumOut,
                                   float& yMinimumOut,
                                   float& yMaximumOut) const
{
    if (! m_boundsValid) {
        float xMin = 0.0;
        float xMax = 0.0;
        float yMin = 0.0;
        float yMax = 0.0;
        float zMin = 0.0;
        float zMax = 0.0;
        const int32_t numPoints = getNumberOfPoints();
        if (numPoints > 0) {
            xMin = std::numeric_limits<float>::max();
            xMax = -std::numeric_limits<float>::max();
            yMin = std::numeric_limits<float>::max();
            yMax = -std::numeric_limits<float>::max();
            for (int32_t i = 0; i < numPoints; i++) {
                const float* xy = getPointAtIndex(i)->getXY();
                const float x = xy[0];
                const float y = xy[1];
                if (x < xMin) xMin = x;
                if (x > xMax) xMax = x;
                if (y < yMin) yMin = y;
                if (y > yMax) yMax = y;
            }
            
            m_boundsValid = true;
        }
        
        m_bounds[0] = xMin;
        m_bounds[1] = xMax;
        m_bounds[2] = yMin;
        m_bounds[3] = yMax;
        m_bounds[4] = zMin;
        m_bounds[5] = zMax;
    }
    
    xMinimumOut = m_bounds[0];
    xMaximumOut = m_bounds[1];
    yMinimumOut = m_bounds[2];
    yMaximumOut = m_bounds[3];
}

/**
 * @return The time start in seconds for the X-Axis (Valid when
 * the X-axis is time)
 */
float
ChartDataCartesian::getTimeStartInSecondsAxisX() const
{
    return m_timeStartInSecondsAxisX;
}

/**
 * Set the time start in seconds for the X-Axis (Valid when
 * the X-axis is time)
 *
 * @param timeStartInSecondsAxisX
 *     Time of first point in the X-axis.
 */
void
ChartDataCartesian::setTimeStartInSecondsAxisX(const float timeStartInSecondsAxisX)
{
    m_timeStartInSecondsAxisX = timeStartInSecondsAxisX;
}

/**
 * @return The time step in seconds for the X-Axis (Valid when
 * the X-axis is time)
 */
float
ChartDataCartesian::getTimeStepInSecondsAxisX() const
{
    return m_timeStepInSecondsAxisX;
}

/**
 * Set the time step in seconds for the X-Axis (Valid when
 * the X-axis is time)
 *
 * @param timeStepInSecondsAxisX
 *     Number of seconds between consecutive points in X-axis.
 */
void
ChartDataCartesian::setTimeStepInSecondsAxisX(const float timeStepInSecondsAxisX)
{
    m_timeStepInSecondsAxisX = timeStepInSecondsAxisX;
}

/**
 * @return Data units for X axis
 */
ChartAxisUnitsEnum::Enum
ChartDataCartesian::getDataAxisUnitsX()
{
    return m_dataAxisUnitsX;
}

/**
 * @return Data units for Y axis
 */
ChartAxisUnitsEnum::Enum
ChartDataCartesian::getDataAxisUnitsY()
{
    return m_dataAxisUnitsY;
}

/**
 * @return Color for chart
 */
CaretColorEnum::Enum
ChartDataCartesian::getColor()
{
    return m_color;
}

/**
 * @return Description of chart
 */
AString
ChartDataCartesian::getDescription() const
{
    return m_description;
}

/**
 * Set description of chart.
 *
 * @param description
 *    New value for chart description
 */
void
ChartDataCartesian::setDescription(const AString& description)
{
    m_description = description;
}

/**
 * @return Source of data encoded as a string.
 */
AString
ChartDataCartesian::getEncodedDataSource() const
{
    return m_encodedDataSource;
}

/**
 * Set source of data encoded as a string.
 * @param encodedDataSource
 *    New value for source of data encoded as a string.
 */
void
ChartDataCartesian::setEncodedDataSource(const AString& encodedDataSource)
{
    m_encodedDataSource = encodedDataSource;
}

