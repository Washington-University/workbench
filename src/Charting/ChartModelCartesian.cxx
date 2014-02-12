
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

#include <cmath>

#define __CHART_MODEL_CARTESIAN_DECLARE__
#include "ChartModelCartesian.h"
#undef __CHART_MODEL_CARTESIAN_DECLARE__

#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartDataCartesian.h"
#include "ChartPoint.h"
#include "ChartScaleAutoRanging.h"

using namespace caret;


    
/**
 * \class caret::ChartModelCartesian 
 * \brief Chart Model for cartesian data.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param chartDataDataType
 *    The chart model data type.
 * @param dataAxisUnitsX
 *    Units for the X-axis.
 * @param dataAxisUnitsY
 *    Units for the Y-axis.
 */
ChartModelCartesian::ChartModelCartesian(const ChartDataTypeEnum::Enum chartDataType,
                                                                               const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                                                               const ChartAxisUnitsEnum::Enum dataAxisUnitsY)
: ChartModel(chartDataType,
             ChartModel::SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO)
{
    m_averageChartData = NULL;
    
    getLeftAxis()->setAxisUnits(dataAxisUnitsY);
    getLeftAxis()->setVisible(true);
    getBottomAxis()->setAxisUnits(dataAxisUnitsX);
    getBottomAxis()->setVisible(true);
}

/**
 * Destructor.
 */
ChartModelCartesian::~ChartModelCartesian()
{
    if (m_averageChartData != NULL) {
        delete m_averageChartData;
    }
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartModelCartesian::ChartModelCartesian(const ChartModelCartesian& obj)
: ChartModel(obj)
{
    this->copyHelperChartModelCartesian(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartModelCartesian&
ChartModelCartesian::operator=(const ChartModelCartesian& obj)
{
    if (this != &obj) {
        ChartModel::operator=(obj);
        this->copyHelperChartModelCartesian(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartModelCartesian::copyHelperChartModelCartesian(const ChartModelCartesian& /*obj*/)
{
    if (m_averageChartData != NULL) {
        delete m_averageChartData;
        m_averageChartData = NULL;
    }
}

/**
 * @return Is an average of data supported?
 */
bool
ChartModelCartesian::isAverageChartDisplaySupported() const
{
    return true;
}

/**
 * @return The average chart data.  Will return NULL if either
 * no data to average or model does not support an average.
 * Includes only those chart data that are displayed.
 */
const ChartData*
ChartModelCartesian::getAverageChartDataForDisplay() const
{
    if (m_averageChartData != NULL) {
        delete m_averageChartData;
        m_averageChartData = NULL;
    }
    
    
    /*
     * Data may be from multiple files so compute an average of those
     * that match the first (newest) file.
     */
    const std::vector<const ChartData*>  allData = getAllSelectedChartDatas();
    if ( ! allData.empty()) {
        std::vector<float> xValue;
        std::vector<double> ySum;
        int64_t averageCounter = 0;
        ChartDataTypeEnum::Enum firstChartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
        
        bool firstFlag = true;
        for (std::vector<const ChartData*>::const_iterator iter = allData.begin();
             iter != allData.end();
             iter++) {
            const ChartData* chartData = *iter;
            const ChartDataCartesian* cartesianData = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(cartesianData);
            
            const int64_t numPoints = cartesianData->getNumberOfPoints();
            if (firstFlag) {
                if (numPoints > 0) {
                    firstFlag = false;
                    
                    xValue.resize(numPoints);
                    ySum.resize(numPoints);
                    for (int64_t i = 0; i < numPoints; i++) {
                        const ChartPoint* point = cartesianData->getPointAtIndex(i);
                        xValue[i] = point->getX();
                        ySum[i]   = point->getY();
                    }
                    
                    firstChartDataType = cartesianData->getChartDataType();
                    averageCounter = 1;
                }
            }
            else {
                if (numPoints == static_cast<int64_t>(ySum.size())) {
                    for (int64_t i = 0; i < numPoints; i++) {
                        const ChartPoint* point = cartesianData->getPointAtIndex(i);
                        ySum[i] += point->getY();
                    }
                    averageCounter++;
                }
            }
        }
        
        if (averageCounter > 0) {
            const int64_t numPoints = static_cast<int64_t>(ySum.size());
            for (int64_t i = 0; i < numPoints; i++) {
                ySum[i] /= averageCounter;
            }
            
            m_averageChartData = dynamic_cast<ChartDataCartesian*>(ChartData::newChartDataForChartDataType(firstChartDataType));
            for (int32_t i = 0; i < numPoints; i++) {
                m_averageChartData->addPoint(xValue[i],
                                             ySum[i]);
            }
        }
    }
    
    return m_averageChartData;
}



/**
 * Reset the axes ranges to the default range for the current data.
 */
void
ChartModelCartesian::resetAxesToDefaultRange()
{
    float boundsMinX = 0.0;
    float boundsMaxX = 0.0;
    float boundsMinY = 0.0;
    float boundsMaxY = 0.0;

    const std::vector<ChartData*>  allData = getAllChartDatas();
    if ( ! allData.empty()) {
        boundsMinX =  std::numeric_limits<float>::max();
        boundsMaxX = -std::numeric_limits<float>::max();
        boundsMinY =  std::numeric_limits<float>::max();
        boundsMaxY = -std::numeric_limits<float>::max();
        
        for (std::vector<ChartData*>::const_iterator iter = allData.begin();
             iter != allData.end();
             iter++) {
            const ChartData* chartData = *iter;
            const ChartDataCartesian* cartesianData = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(cartesianData);
            
            float xMin, xMax, yMin, yMax;
            cartesianData->getBounds(xMin, xMax, yMin, yMax);
            
            if (xMin < boundsMinX) boundsMinX = xMin;
            if (xMax > boundsMaxX) boundsMaxX = xMax;
            if (yMin < boundsMinY) boundsMinY = yMin;
            if (yMax > boundsMaxY) boundsMaxY = yMax;
        }
    }
    
    ChartScaleAutoRanging::adjustAxisDefaultRange(boundsMinX, boundsMaxX);
    ChartScaleAutoRanging::adjustAxisDefaultRange(boundsMinY, boundsMaxY);

    ChartAxis* ba = getBottomAxis();
    if (ba->isAutoRangeScale()) {
        ba->setMinimumValue(boundsMinX);
        ba->setMaximumValue(boundsMaxX);
    }
    ChartAxis* la = getLeftAxis();
    if (la->isAutoRangeScale()) {
        la->setMinimumValue(boundsMinY);
        la->setMaximumValue(boundsMaxY);
    }
    
}
