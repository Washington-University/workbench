
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

#define __CHART_TWO_DATA_CARTESIAN_DECLARE__
#include "ChartTwoDataCartesian.h"
#undef __CHART_TWO_DATA_CARTESIAN_DECLARE__

#include <limits>

#include <QTextStream>

#include "CaretAssert.h"
#include "ChartPoint.h"
#include "GraphicsPrimitiveV3f.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoDataCartesian 
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
 * @param graphicsPrimitiveType
 *   Primitive type for graphics primitive drawing.
 */
ChartTwoDataCartesian::ChartTwoDataCartesian(const ChartTwoDataTypeEnum::Enum chartDataType,
                                             const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                             const ChartAxisUnitsEnum::Enum dataAxisUnitsY,
                                             const GraphicsPrimitive::PrimitiveType graphicsPrimitiveType)
: ChartTwoData(chartDataType),
m_dataAxisUnitsX(dataAxisUnitsX),
m_dataAxisUnitsY(dataAxisUnitsY),
m_graphicsPrimitiveType(graphicsPrimitiveType)
{
    initializeMembersChartTwoDataCartesian();
    
    switch (getChartTwoDataType()) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            CaretAssert(0);
            break;
    }
}

/**
 * Destructor.
 */
ChartTwoDataCartesian::~ChartTwoDataCartesian()
{
    removeAllPoints();
    
    delete m_sceneAssistant;
}

/**
 * Initialize members of a new instance.
 */
void
ChartTwoDataCartesian::initializeMembersChartTwoDataCartesian()
{
    m_boundsValid       = false;
    m_color             = CaretColorEnum::RED;
    m_timeStartInSecondsAxisX = 0.0;
    m_timeStepInSecondsAxisX  = 1.0;
    
    m_graphicsPrimitive = createGraphicsPrimitive();
    
    std::vector<CaretColorEnum::Enum> colorEnums;
    CaretColorEnum::getColorEnums(colorEnums);
    const int32_t numCaretColors = static_cast<int32_t>(colorEnums.size());
    
    bool colorFound = false;
    while ( ! colorFound) {
        ChartTwoDataCartesian::caretColorIndex++;
        if (ChartTwoDataCartesian::caretColorIndex >= numCaretColors) {
            ChartTwoDataCartesian::caretColorIndex = 0;
        }
        
        if (colorEnums[ChartTwoDataCartesian::caretColorIndex] == CaretColorEnum::BLACK) {
            /* do not use black */
        }
        else if (colorEnums[ChartTwoDataCartesian::caretColorIndex] == CaretColorEnum::WHITE) {
            /* do not use white */
        }
        else {
            m_color = colorEnums[ChartTwoDataCartesian::caretColorIndex];
            colorFound = true;
        }
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<ChartAxisUnitsEnum, ChartAxisUnitsEnum::Enum>("m_dataAxisUnitsX",
                                                                &m_dataAxisUnitsX);
    m_sceneAssistant->add<ChartAxisUnitsEnum, ChartAxisUnitsEnum::Enum>("m_dataAxisUnitsY",
                                                                &m_dataAxisUnitsY);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_color",
                                                                &m_color);
    m_sceneAssistant->add("m_timeStartInSecondsAxisX",
                          &m_timeStartInSecondsAxisX);
    m_sceneAssistant->add("m_timeStepInSecondsAxisX",
                          &m_timeStepInSecondsAxisX);
}

/**
 * @return A new instance of the graphics primitive.
 */
std::unique_ptr<GraphicsPrimitiveV3f>
ChartTwoDataCartesian::createGraphicsPrimitive()
{
    float rgba[4];
    CaretColorEnum::toRGBAFloat(m_color, rgba);
    
    /*
     * Note: LINES (line segments) are used so that individual segments can be identified.
     * Cannot use line strip since it is identified as a single item.
     */
    return std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(m_graphicsPrimitiveType,
                                                                                    rgba));
}
/**
 * Remove all points in the model.
 */
void
ChartTwoDataCartesian::removeAllPoints()
{
    m_graphicsPrimitive = createGraphicsPrimitive();

    for (std::vector<ChartPoint*>::const_iterator iter = m_points.begin();
         iter != m_points.end();
         iter++) {
        delete *iter;
    }
    m_points.clear();
    
    m_boundsValid = false;
}

/**
 * At times a copy of chart data will be needed BUT it must be
 * the proper subclass so copy constructor and assignment operator
 * will no function when this abstract, base class is used.  Each
 * subclass will override this method so that the returned class
 * is of the proper type.
 *
 * @return Copy of this instance that is the actual subclass.
 */
ChartTwoData*
ChartTwoDataCartesian::clone() const
{
    ChartTwoDataCartesian* cloneCopy = new ChartTwoDataCartesian(*this);
    return cloneCopy;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoDataCartesian::ChartTwoDataCartesian(const ChartTwoDataCartesian& obj)
: ChartTwoData(obj),
m_dataAxisUnitsX(obj.m_dataAxisUnitsX),
m_dataAxisUnitsY(obj.m_dataAxisUnitsY),
m_graphicsPrimitiveType(obj.m_graphicsPrimitiveType)
{
    initializeMembersChartTwoDataCartesian();
    this->copyHelperChartTwoDataCartesian(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoDataCartesian&
ChartTwoDataCartesian::operator=(const ChartTwoDataCartesian& obj)
{
    if (this != &obj) {
        ChartTwoData::operator=(obj);
        this->copyHelperChartTwoDataCartesian(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoDataCartesian::copyHelperChartTwoDataCartesian(const ChartTwoDataCartesian& obj)
{
    CaretAssert(0);
    m_dataAxisUnitsX = obj.m_dataAxisUnitsX;
    m_dataAxisUnitsY = obj.m_dataAxisUnitsY;
    
    removeAllPoints();

    for (std::vector<ChartPoint*>::const_iterator iter = obj.m_points.begin();
         iter != obj.m_points.end();
         iter++) {
        const ChartPoint* cp = *iter;
        m_points.push_back(new ChartPoint(*cp));
    }

    
    m_boundsValid       = false;
    m_color             = obj.m_color;
    m_timeStartInSecondsAxisX = obj.m_timeStartInSecondsAxisX;
    m_timeStepInSecondsAxisX  = obj.m_timeStepInSecondsAxisX;
}

/**
 * @return Graphics primitive for drawing cartesian data.
 */
GraphicsPrimitiveV3f*
ChartTwoDataCartesian::getGraphicsPrimitive() const
{
    return m_graphicsPrimitive.get();
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
ChartTwoDataCartesian::addPoint(const float x,
                                  const float y)
{
    m_graphicsPrimitive->addVertex(x, y);
    m_points.push_back(new ChartPoint(x, y));
    m_boundsValid = false;
}

/**
 * @return Number of points.
 */
int32_t
ChartTwoDataCartesian::getNumberOfPoints() const
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
ChartTwoDataCartesian::getPointAtIndex(const int32_t pointIndex) const
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
ChartTwoDataCartesian::getBounds(float& xMinimumOut,
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
ChartTwoDataCartesian::getTimeStartInSecondsAxisX() const
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
ChartTwoDataCartesian::setTimeStartInSecondsAxisX(const float timeStartInSecondsAxisX)
{
    m_timeStartInSecondsAxisX = timeStartInSecondsAxisX;
}

/**
 * @return The time step in seconds for the X-Axis (Valid when
 * the X-axis is time)
 */
float
ChartTwoDataCartesian::getTimeStepInSecondsAxisX() const
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
ChartTwoDataCartesian::setTimeStepInSecondsAxisX(const float timeStepInSecondsAxisX)
{
    m_timeStepInSecondsAxisX = timeStepInSecondsAxisX;
}

/**
 * @return Data units for X axis
 */
ChartAxisUnitsEnum::Enum
ChartTwoDataCartesian::getDataAxisUnitsX()
{
    return m_dataAxisUnitsX;
}

/**
 * @return Data units for Y axis
 */
ChartAxisUnitsEnum::Enum
ChartTwoDataCartesian::getDataAxisUnitsY()
{
    return m_dataAxisUnitsY;
}

/**
 * @return Color for chart
 */
CaretColorEnum::Enum
ChartTwoDataCartesian::getColor() const
{
    return m_color;
}

/**
 * Set the color for the chart.
 *
 * @param color
 *    New color for chart.
 */
void
ChartTwoDataCartesian::setColor(const CaretColorEnum::Enum color)
{
    m_color = color;
    
    if (m_graphicsPrimitive != NULL) {
        float rgba[4];
        CaretColorEnum::toRGBAFloat(m_color, rgba);
        m_graphicsPrimitive->replaceColoring(rgba);
    }
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
ChartTwoDataCartesian::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                     SceneClass* sceneClass)
{
    SceneClass* chartDataCartesian = new SceneClass("chartDataCartesian",
                                               "ChartTwoDataCartesian",
                                               1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  chartDataCartesian);
    
    const int32_t numPoints2D = getNumberOfPoints();
    if (numPoints2D > 0) {
        chartDataCartesian->addInteger("numberOfPoints2D",
                                       numPoints2D);
        
        AString pointString;
        pointString.reserve(numPoints2D * 2 * 10);
        QTextStream textStream(&pointString,
                               QIODevice::WriteOnly);
        
        for (int32_t i = 0; i < numPoints2D; i++) {
            const float* xy = m_points[i]->getXY();
            textStream << xy[0] << " " << xy[1] << " ";
        }
        
        chartDataCartesian->addString("points2D",
                                      pointString);
    }
    
    sceneClass->addClass(chartDataCartesian);
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
ChartTwoDataCartesian::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass)
{
    removeAllPoints();
    
    const SceneClass* chartDataCartesian = sceneClass->getClass("chartDataCartesian");
    if (chartDataCartesian == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, chartDataCartesian);
    
    const int32_t numPoints2D = chartDataCartesian->getIntegerValue("numberOfPoints2D",
                                                                  -1);
    
    if (numPoints2D > 0) {
        AString pointString = chartDataCartesian->getStringValue("points2D",
                                                                       "");
        if ( ! pointString.isEmpty()) {
            float x, y;
            QTextStream textStream(&pointString,
                                   QIODevice::ReadOnly);
            for (int32_t i = 0; i < numPoints2D; i++) {
                if (textStream.atEnd()) {
                    sceneAttributes->addToErrorMessage("Tried to read "
                                                       + AString::number(numPoints2D)
                                                       + " but only got "
                                                       + AString::number(i));
                    break;
                }
                
                textStream >> x;
                textStream >> y;
                m_points.push_back(new ChartPoint(x, y));
            }
        }
    }
}

