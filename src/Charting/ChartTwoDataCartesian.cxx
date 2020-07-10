
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

#include <cmath>
#include <limits>

#include <QTextStream>

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "ChartPoint.h"
#include "GraphicsPrimitiveV3f.h"
#include "MapFileDataSelector.h"
#include "MathFunctions.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "ScenePrimitiveArray.h"

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
                                             const CaretUnitsTypeEnum::Enum dataAxisUnitsX,
                                             const CaretUnitsTypeEnum::Enum dataAxisUnitsY,
                                             const GraphicsPrimitive::PrimitiveType graphicsPrimitiveType)
: CaretObjectTracksModification(),
m_dataAxisUnitsX(dataAxisUnitsX),
m_dataAxisUnitsY(dataAxisUnitsY),
m_graphicsPrimitiveType(graphicsPrimitiveType)
{
    initializeMembersChartTwoDataCartesian();
    
    switch (chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
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
    delete m_sceneAssistant;
}

/**
 * Initialize members of a new instance.
 */
void
ChartTwoDataCartesian::initializeMembersChartTwoDataCartesian()
{
    m_mapFileDataSelector = std::unique_ptr<MapFileDataSelector>(new MapFileDataSelector());
    
    m_selectionStatus   = true;
    m_lineWidth         = getDefaultLineWidth();
    m_timeStartInSecondsAxisX = 0.0;
    m_timeStepInSecondsAxisX  = 1.0;
    
    m_graphicsPrimitive = createGraphicsPrimitive();
    
    std::vector<CaretColorEnum::Enum> colorEnums;
    CaretColorEnum::getColorEnumsNoBlackOrWhite(colorEnums);
    const int32_t numCaretColors = static_cast<int32_t>(colorEnums.size());
    
    CaretColorEnum::Enum defaultColor = CaretColorEnum::RED;
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
            defaultColor = colorEnums[ChartTwoDataCartesian::caretColorIndex];
            colorFound = true;
        }
    }
    setColorEnum(defaultColor);
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_selectionStatus",
                          &m_selectionStatus);
    m_sceneAssistant->add<CaretUnitsTypeEnum, CaretUnitsTypeEnum::Enum>("m_dataAxisUnitsX",
                                                                &m_dataAxisUnitsX);
    m_sceneAssistant->add<CaretUnitsTypeEnum, CaretUnitsTypeEnum::Enum>("m_dataAxisUnitsY",
                                                                &m_dataAxisUnitsY);
    m_sceneAssistant->add("m_lineWidth",
                          &m_lineWidth);
    m_sceneAssistant->add("m_timeStartInSecondsAxisX",
                          &m_timeStartInSecondsAxisX);
    m_sceneAssistant->add("m_timeStepInSecondsAxisX",
                          &m_timeStepInSecondsAxisX);
    m_sceneAssistant->add("m_mapFileDataSelector",
                          "MapFileDataSelector",
                          m_mapFileDataSelector.get());
    m_sceneAssistant->add("m_selectedPointIndex",
                          &m_selectedPointIndex);
    m_sceneAssistant->add("m_selectedPointDisplayed",
                          &m_selectedPointDisplayed);
}

/**
 * @return A new instance of the graphics primitive.
 */
std::unique_ptr<GraphicsPrimitiveV3f>
ChartTwoDataCartesian::createGraphicsPrimitive()
{
    std::array<uint8_t, 4> rgba = m_caretColor.getRGBA();
    
    /*
     * Note: LINES (line segments) are used so that individual segments can be identified.
     * Cannot use line strip since it is identified as a single item.
     */
    return std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(m_graphicsPrimitiveType,
                                                                                    rgba.data()));
}

/**
 * @return The map file data selector that will indicate the source of data for this data.
 */
const MapFileDataSelector*
ChartTwoDataCartesian::getMapFileDataSelector() const
{
    return m_mapFileDataSelector.get();
}

/**
 * @param
 */
void
ChartTwoDataCartesian::setMapFileDataSelector(const MapFileDataSelector& mapFileDataSelector)
{
    *m_mapFileDataSelector = mapFileDataSelector;
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
ChartTwoDataCartesian*
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
: CaretObjectTracksModification(obj),
SceneableInterface(obj),
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
        CaretObjectTracksModification::operator=(obj);
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
    m_selectionStatus = obj.m_selectionStatus;
    *m_mapFileDataSelector = *obj.m_mapFileDataSelector;
    m_dataAxisUnitsX = obj.m_dataAxisUnitsX;
    m_dataAxisUnitsY = obj.m_dataAxisUnitsY;

    m_graphicsPrimitive.reset(dynamic_cast<GraphicsPrimitiveV3f*>(obj.m_graphicsPrimitive->clone()));
    
    m_caretColor              = obj.m_caretColor;
    m_lineWidth               = obj.m_lineWidth;
    m_timeStartInSecondsAxisX = obj.m_timeStartInSecondsAxisX;
    m_timeStepInSecondsAxisX  = obj.m_timeStepInSecondsAxisX;
    
    m_selectedPointIndex      = obj.m_selectedPointIndex;
    m_selectedPointDisplayed  = obj.m_selectedPointDisplayed;
}

/**
 * @return Graphics primitive for drawing cartesian data.
 */
GraphicsPrimitiveV3f*
ChartTwoDataCartesian::getGraphicsPrimitive() const
{
    m_graphicsPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                      m_lineWidth);
    return m_graphicsPrimitive.get();
}

/**
 * @return The selection status
 */
bool
ChartTwoDataCartesian::isSelected() const
{
    return m_selectionStatus;
}

/**
 * Set the selection status.
 *
 * @param selectionStatus
 *    New selection status.
 */
void
ChartTwoDataCartesian::setSelected(const bool selectionStatus)
{
    m_selectionStatus = selectionStatus;
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
}

/**
 * Get the coordinates of the point at the given index
 * @param pointIndex
 *    Index of the point
 * @param xyzOut
 *    Output containing XYZ of point
 */
void
ChartTwoDataCartesian::getPointXYZ(const int32_t pointIndex,
                                   float xyzOut[3]) const
{
    if ((pointIndex >= 0)
        && (pointIndex < m_graphicsPrimitive->getNumberOfVertices())) {
        m_graphicsPrimitive->getVertexFloatXYZ(pointIndex,
                                               xyzOut);
    }
    else {
        xyzOut[0] = 0.0;
        xyzOut[1] = 0.0;
        xyzOut[2] = 0.0;
    }
}


/**
 * Get a bounds box for the cartesian data.
 *
 * @param boundingBoxOut
 *     Output bounding box.
 * @return
 *     True if bounding box is valid, else false.
 */
bool
ChartTwoDataCartesian::getBounds(BoundingBox& boundingBoxOut) const
{
    return m_graphicsPrimitive->getVertexBounds(boundingBoxOut);
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
CaretUnitsTypeEnum::Enum
ChartTwoDataCartesian::getDataAxisUnitsX()
{
    return m_dataAxisUnitsX;
}

/**
 * @return Data units for Y axis
 */
CaretUnitsTypeEnum::Enum
ChartTwoDataCartesian::getDataAxisUnitsY()
{
    return m_dataAxisUnitsY;
}

/**
 * @return Color enum for chart
 */
CaretColorEnum::Enum
ChartTwoDataCartesian::getColorEnum() const
{
    return m_caretColor.getCaretColorEnum();
}

/**
 * @return Color for chart
 */
CaretColor
ChartTwoDataCartesian::getColor() const
{
    return m_caretColor;
}

/**
 * Set the color for the chart.
 *
 * @param color
 *    New color for chart.
 */
void
ChartTwoDataCartesian::setColor(const CaretColor& color)
{
    m_caretColor = color;
    
    if (m_graphicsPrimitive != NULL) {
        m_graphicsPrimitive->replaceAllVertexSolidByteRGBA(m_caretColor.getRGBA().data());
    }
}

/**
 * Set the color for the chart with a color enum
 *
 * @param colorEnum
 *    New color for chart.
 */
void
ChartTwoDataCartesian::setColorEnum(const CaretColorEnum::Enum colorEnum)
{
    CaretColor cc;
    cc.setCaretColorEnum(colorEnum);
    setColor(cc);
}

/**
 * @return The line width.
 */
float
ChartTwoDataCartesian::getLineWidth() const
{
    return m_lineWidth;
}

/**
 * Set line width.
 *
 * @param lineWidth
 *     New value for line width.
 */
void
ChartTwoDataCartesian::setLineWidth(const float lineWidth)
{
    m_lineWidth = lineWidth;
    m_graphicsPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                      m_lineWidth);
}

/**
 * @return Index of line segment containing X.  Index is of point at start of line segment.
 * Negative if X is not within range of line.
 *
 * WARNING: Points MUST be ordered in ascending order by the X-coordinate.  If not,
 * behavior is undefined.
 *
 * @param x
 *    The X-coordinate
 */
int32_t
ChartTwoDataCartesian::getLineSegmentIndexContainingX(const float x) const
{
    int32_t segmentIndex(-1);
    
    const float numLineSegments = m_graphicsPrimitive->getNumberOfVertices() - 1;
    if (numLineSegments >= 1) {
        /*
         * Verify X is within range of the endpoints
         */
        float leftXYZ[3];
        m_graphicsPrimitive->getVertexFloatXYZ(0, leftXYZ);
        float rightXYZ[3];
        m_graphicsPrimitive->getVertexFloatXYZ(numLineSegments, rightXYZ);
        if ((x < leftXYZ[0])
            || (x > rightXYZ[0])) {
            return segmentIndex;
        }

        int32_t left(0);
        int32_t right(numLineSegments);
        while (left <= right) {
            const int32_t middle = left + (right - left) / 2;
            
            float p1[3], p2[3];
            m_graphicsPrimitive->getVertexFloatXYZ(middle, p1);
            m_graphicsPrimitive->getVertexFloatXYZ(middle + 1, p2);
            
            if ((x >= p1[0])
                && (x <= p2[0])) {
                segmentIndex = middle;
                break;
            }
            else {
                if (x > p2[0]) {
                    left = middle + 1;
                }
                else {
                    right = middle;
                }
            }
        }

#ifdef VERIFY_FLAG
        /*
         * This is a slow linear search, used only in debug
         * mode to verify binary search is correct
         */
        int32_t linearIndex(-1);
        for (int32_t i = 0; i < numLineSegments; i++) {
            float p1[3], p2[3];
            m_graphicsPrimitive->getVertexFloatXYZ(i, p1);
            m_graphicsPrimitive->getVertexFloatXYZ(i + 1, p2);
            if ((x >= p1[0])
                && (x <= p2[0])) {
                linearIndex = i;
            }
        }
        CaretAssert(segmentIndex == linearIndex);
#endif // VERIFY_FLAG
    }

    return segmentIndex;
}

/**
 * @return The vertical distance from the line to the given X, Y coordinates.  A negative
 * value is returned if the X-coordinate is not within the line's range of X-coordinates.
 * @param x
 *    The X-coordinate
 * @param y
 *    The Y-coordinate
 */
bool
ChartTwoDataCartesian::getVerticalDistanceToXY(const float x,
                                               const float y,
                                               float& distanceOut,
                                               int32_t& pointIndexOut,
                                               float chartXYZOut[3]) const
{
    distanceOut   = std::numeric_limits<float>::max();
    pointIndexOut = -1;
    const int32_t leftIndex = getLineSegmentIndexContainingX(x);
    
    if (leftIndex >= 0) {
        const int32_t rightIndex(leftIndex + 1);
        const float xyz[3] { x, y, 0.0f };

        float p1[3], p2[3];
        m_graphicsPrimitive->getVertexFloatXYZ(leftIndex, p1);
        m_graphicsPrimitive->getVertexFloatXYZ(rightIndex, p2);
        p1[2] = 0.0;
        p2[2] = 0.0;
        const float dist1(MathFunctions::distance3D(xyz, p1));
        const float dist2(MathFunctions::distance3D(xyz, p2));
        
        const float dx = p2[0] - p1[0];
        if (dx != 0.0) {
            /*
             * Vertical distance from point to the line segment
             */
            const float dy = p2[1] - p1[1];
            const float m  = dy / dx;
            const float b  = p1[1] - (m * p1[0]);
            const float yOnSegment  = m * x + b;
            distanceOut = std::fabs(yOnSegment - y);
            if (dist1 < dist2) {
                pointIndexOut = leftIndex;
            }
            else {
                pointIndexOut = rightIndex;
            }
            
            chartXYZOut[0] = x;
            chartXYZOut[1] = yOnSegment;
            chartXYZOut[2] = p1[2];
        }
        else {
            /*
             * Vertical line so use nearest point in line
             */
            if (dist1 < dist2) {
                pointIndexOut = leftIndex;
                distanceOut   = dist1;
                chartXYZOut[0] = p1[0];
                chartXYZOut[1] = p1[1];
                chartXYZOut[2] = p1[2];
            }
            else {
                pointIndexOut = rightIndex;
                distanceOut   = dist2;
                chartXYZOut[0] = p2[0];
                chartXYZOut[1] = p2[1];
                chartXYZOut[2] = p2[2];
            }
        }
    }
    
    return (pointIndexOut >= 0);
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartTwoDataCartesian::saveToScene(const SceneAttributes* sceneAttributes,
                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoDataCartesian",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);

    /*
     * Note: For line layer charts, color is from the chart overlay not this color
     */
    sceneClass->addString("m_caretColor",
                          m_caretColor.encodeInXML());
    
    const std::vector<float>& xyz = m_graphicsPrimitive->getFloatXYZ();
    const int32_t numXYZ = static_cast<int32_t>(xyz.size());
    if (numXYZ > 0) {
        sceneClass->addFloatArray("xyz",
                                  &xyz[0],
                                  numXYZ);
    }

    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartTwoDataCartesian::restoreFromScene(const SceneAttributes* sceneAttributes,
                               const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    m_graphicsPrimitive = createGraphicsPrimitive();
    
    m_sceneAssistant->restoreMembers(sceneAttributes, sceneClass);
    
    /*
     * m_caretColor (instance of CaretColor) replaced
     * m_color (instance CaretColorEnum)
     * Note: For line layer charts, color is from the chart overlay not this color
     */
    const QString caretColorText = sceneClass->getStringValue("m_caretColor", "");
    if ( ! caretColorText.isEmpty()) {
        AString errorMessage;
        if ( ! m_caretColor.decodeFromXML(caretColorText,
                                          errorMessage)) {
            sceneAttributes->addToErrorMessage(errorMessage);
        }
    }
    else {
        const CaretColorEnum::Enum color = sceneClass->getEnumeratedTypeValue<CaretColorEnum,CaretColorEnum::Enum>("m_color",
                                                                                                                   CaretColorEnum::BLUE);
        m_caretColor.setCaretColorEnum(color);
    }
    
    
    const ScenePrimitiveArray* xyzArray = sceneClass->getPrimitiveArray("xyz");
    if (xyzArray != NULL) {
        const int32_t numElements = xyzArray->getNumberOfArrayElements();
        if (numElements > 0) {
            std::vector<float> data(numElements);
            xyzArray->floatValues(&data[0],
                                  numElements,
                                  0.0f);
            const int32_t numXYZ = (numElements / 3);
            for (int32_t i = 0; i < numXYZ; i++) {
                const int32_t i3 = i * 3;
                CaretAssertVectorIndex(data, i3+2);
                addPoint(data[i3], data[i3+1]);
            }
        }
    }
    
    setColor(getColor());
}

