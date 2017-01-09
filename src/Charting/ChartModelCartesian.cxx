
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

#include <cmath>
#include <limits>

#define __CHART_MODEL_CARTESIAN_DECLARE__
#include "ChartModelCartesian.h"
#undef __CHART_MODEL_CARTESIAN_DECLARE__

#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartAxisCartesian.h"
#include "ChartDataCartesian.h"
#include "ChartPoint.h"
#include "ChartScaleAutoRanging.h"
#include "SceneClassAssistant.h"

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
ChartModelCartesian::ChartModelCartesian(const ChartOneDataTypeEnum::Enum chartDataType,
                                                                               const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                                                               const ChartAxisUnitsEnum::Enum dataAxisUnitsY)
: ChartModel(chartDataType,
             ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY)
{
    m_averageChartData = NULL;
    
    m_lineWidth = 1.0;
    
    setLeftAxis(ChartAxis::newChartAxisForTypeAndLocation(ChartAxisTypeEnum::CHART_AXIS_TYPE_CARTESIAN,
                                                          ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT));
    getLeftAxis()->setAxisUnits(dataAxisUnitsY);
    getLeftAxis()->setVisible(true);
    
    setBottomAxis(ChartAxis::newChartAxisForTypeAndLocation(ChartAxisTypeEnum::CHART_AXIS_TYPE_CARTESIAN,
                                                            ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM));
    getBottomAxis()->setAxisUnits(dataAxisUnitsX);
    getBottomAxis()->setVisible(true);
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_lineWidth", &m_lineWidth);
}

/**
 * Destructor.
 */
ChartModelCartesian::~ChartModelCartesian()
{
    if (m_averageChartData != NULL) {
        delete m_averageChartData;
    }
    
    delete m_sceneAssistant;
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
 * Get the average for charts in the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 *
 * @return 
 *     The average chart data.  Will return NULL if either
 * no data to average or model does not support an average.
 * Includes only those chart data that are displayed.
 */
const ChartData*
ChartModelCartesian::getAverageChartDataForDisplay(const int32_t tabIndex) const
{
    if (m_averageChartData != NULL) {
        delete m_averageChartData;
        m_averageChartData = NULL;
    }
    
    
    /*
     * Data may be from multiple files so compute an average of those
     * that match the first (newest) file.
     */
    const std::vector<const ChartData*>  allData = getAllSelectedChartDatas(tabIndex);
    if ( ! allData.empty()) {
        std::vector<float> xValue;
        std::vector<double> ySum;
        int64_t averageCounter = 0;
        ChartOneDataTypeEnum::Enum firstChartDataType = ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
        
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
 * Get the bounds of all of the data in themodel.
 *
 * @param boundsMinX
 *     Minimum X-coordinate of all points.
 * @param boundsMaxX
 *     Maximum X-coordinate of all points.
 * @param boundsMinY
 *     Minimum Y-coordinate of all points.
 * @param boundsMaxY
 *     Maximum Y-coordinate of all points.
 */
void
ChartModelCartesian::getBounds(float& boundsMinX,
                               float& boundsMaxX,
                               float& boundsMinY,
                               float& boundsMaxY) const
{
    boundsMinX = 0.0;
    boundsMaxX = 0.0;
    boundsMinY = 0.0;
    boundsMaxY = 0.0;
    
    const std::vector<const ChartData*> allData = getAllChartDatas();
    if ( ! allData.empty()) {
        boundsMinX =  std::numeric_limits<float>::max();
        boundsMaxX = -std::numeric_limits<float>::max();
        boundsMinY =  std::numeric_limits<float>::max();
        boundsMaxY = -std::numeric_limits<float>::max();
        
        for (std::vector<const ChartData*>::const_iterator iter = allData.begin();
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
}

/**
 * Get the line width for the chart.
 *
 * @return
 *    Line width for the chart.
 */
float
ChartModelCartesian::getLineWidth() const
{
    return m_lineWidth;
}

/**
 * Set the line width for the chart.
 *
 * param lineWidth
 *    Line width for chart.
 */
void
ChartModelCartesian::setLineWidth(const float lineWidth)
{
    m_lineWidth = lineWidth;
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
ChartModelCartesian::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ChartModelCartesian::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}



