
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __HISTOGRAM_DRAWING_INFO_DECLARE__
#include "HistogramDrawingInfo.h"
#undef __HISTOGRAM_DRAWING_INFO_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::HistogramDrawingInfo 
 * \brief Contains data and metadata for drawing a histogram
 * \ingroup Common
 */

/**
 * Constructor creates invalid instance.
 */
HistogramDrawingInfo::HistogramDrawingInfo()
: CaretObject()
{
    reset();
}

/**
 * Destructor.
 */
HistogramDrawingInfo::~HistogramDrawingInfo()
{
}

/**
 * Initialize for the given number of data points.
 *
 * @param numberOfDataPoints
 *     Number of data points for the histogram.
 */
void
HistogramDrawingInfo::initialize(const int32_t numberOfDataPoints)
{
    CaretAssert(numberOfDataPoints > 0);
    m_dataX.resize(numberOfDataPoints, 0.0);
    m_dataY.resize(numberOfDataPoints, 0.0);
    m_dataRGBA.resize(numberOfDataPoints * 4, 0.0);
}

/**
 * Reset the histogram drawing information.
 */
void
HistogramDrawingInfo::reset()
{
    m_dataX.clear();
    m_dataY.clear();
    m_dataRGBA.clear();
    m_boundsValid = false;
    m_thresholdOneBoundsXYZ.clear();
    m_thresholdTwoBoundsXYZ.clear();
    m_thresholdOneBoundsValid = false;
    m_thresholdOneBoundsValid = false;
}


/**
 * @return Is the histogram drawing information valid?
 */
bool
HistogramDrawingInfo::isValid() const
{
    if ( ! m_dataX.empty()) {
        CaretAssert(m_dataX.size() == m_dataY.size());
        CaretAssert((m_dataX.size() * 4) == m_dataRGBA.size());
        if ((m_dataX.size() == m_dataY.size())
            && (m_dataX.size() * 4) == m_dataRGBA.size()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Get the bounds for the data.
 *
 * @param boundsOut
 *     Output with bounds (minX, maxX, minY, maxY)
 * @return
 *     True if bounds valid, else false.
 */
bool
HistogramDrawingInfo::getBounds(float boundsOut[4]) const
{
  
    if ( ! m_boundsValid) {
        if (isValid()) {
            m_bounds[0] = *std::min_element(m_dataX.begin(), m_dataX.end());
            m_bounds[1] = *std::max_element(m_dataX.begin(), m_dataX.end());
            m_bounds[2] = *std::min_element(m_dataY.begin(), m_dataY.end());
            m_bounds[3] = *std::max_element(m_dataY.begin(), m_dataY.end());
            m_boundsValid = true;
        }
    }
    
    boundsOut[0] = m_bounds[0];
    boundsOut[1] = m_bounds[1];
    boundsOut[2] = m_bounds[2];
    boundsOut[3] = m_bounds[3];
    
    return m_boundsValid;
}

///**
// * Get the bounds for the threshold one.
// *
// * @param boundsOut
// *     Output with bounds (minX, maxX, minY, maxY)
// * @return
// *     True if bounds valid, else false.
// */
//const 
//HistogramDrawingInfo::getThresholdOneBounds(float boundsOut[4]) const
//{
//    boundsOut[0] = m_thresholdOneBounds[0];
//    boundsOut[1] = m_thresholdOneBounds[1];
//    boundsOut[2] = m_thresholdOneBounds[2];
//    boundsOut[3] = m_thresholdOneBounds[3];
//    
//    return m_thresholdOneBoundsValid;
//}

///**
// * Get the bounds for threshold two.
// *
// * @param boundsOut
// *     Output with bounds (minX, maxX, minY, maxY)
// * @return
// *     True if bounds valid, else false.
// */
//bool
//HistogramDrawingInfo::getThresholdTwoBounds(float boundsOut[4]) const
//{
//    boundsOut[0] = m_thresholdTwoBounds[0];
//    boundsOut[1] = m_thresholdTwoBounds[1];
//    boundsOut[2] = m_thresholdTwoBounds[2];
//    boundsOut[3] = m_thresholdTwoBounds[3];
//    
//    return m_thresholdTwoBoundsValid;
//}

/**
 * @return Number of data points in the histogram.
 */
int32_t
HistogramDrawingInfo::getNumberOfDataPoints() const
{
    CaretAssert(m_dataX.size() == m_dataY.size());
    CaretAssert((m_dataX.size() * 4) == m_dataRGBA.size());
    
    return m_dataX.size();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistogramDrawingInfo::toString() const
{
    return "HistogramDrawingInfo";
}

