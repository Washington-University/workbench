
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

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "ChartPoint.h"
#include "GraphicsPrimitiveV3f.h"
#include "MapFileDataSelector.h"
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
    m_color             = CaretColorEnum::RED;
    m_lineWidth         = getDefaultLineWidth();
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
    
    m_sceneAssistant->add("m_selectionStatus",
                          &m_selectionStatus);
    m_sceneAssistant->add<CaretUnitsTypeEnum, CaretUnitsTypeEnum::Enum>("m_dataAxisUnitsX",
                                                                &m_dataAxisUnitsX);
    m_sceneAssistant->add<CaretUnitsTypeEnum, CaretUnitsTypeEnum::Enum>("m_dataAxisUnitsY",
                                                                &m_dataAxisUnitsY);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_color",
                                                                &m_color);
    m_sceneAssistant->add("m_lineWidth",
                          &m_lineWidth);
    m_sceneAssistant->add("m_timeStartInSecondsAxisX",
                          &m_timeStartInSecondsAxisX);
    m_sceneAssistant->add("m_timeStepInSecondsAxisX",
                          &m_timeStepInSecondsAxisX);
    m_sceneAssistant->add("m_mapFileDataSelector",
                          "MapFileDataSelector",
                          m_mapFileDataSelector.get());
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
    
    m_color             = obj.m_color;
    m_lineWidth         = obj.m_lineWidth;
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
    
    /*
     * When selection status is true,
     * notify parent.
     */
    //    if (m_selectionStatus[tabIndex]) {
    //        if (m_parentChartModel != NULL) {
    //            m_parentChartModel->childChartTwoDataSelectionChanged(this);
    //        }
    //    }
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
        m_graphicsPrimitive->replaceAllVertexSolidFloatRGBA(rgba);
    }
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
    
//    chartDataCartesian->addClass(m_mapFileDataSelector->saveToScene(sceneAttributes,
//                                                            "m_mapFileDataSelector"));
    
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
    
//    m_mapFileDataSelector->restoreFromScene(sceneAttributes, chartDataCartesian->getClass("m_mapFileDataSelector"));
    
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

