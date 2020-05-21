
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

#define __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_DECLARE__
#include "ChartableTwoFileLineLayerChart.h"
#undef __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_DECLARE__

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartTwoDataCartesian.h"
#include "ChartTwoLineSeriesHistory.h"
#include "CiftiMappableDataFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "EventChartTwoLoadLineSeriesData.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"

using namespace caret;
    
/**
 * \class caret::ChartableTwoFileLineLayerChart
 * \brief Implementation of base chart delegate for line layer charts.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param lineLayerContentType
 *     Content type of the line series data.
 * @param parentCaretMappableDataFile
 *     Parent caret mappable data file that this delegate supports.
 */
ChartableTwoFileLineLayerChart::ChartableTwoFileLineLayerChart(const ChartTwoLineLayerContentTypeEnum::Enum lineLayerContentType,
                                                                                 CaretMappableDataFile* parentCaretMappableDataFile)
: ChartableTwoFileBaseChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER,
                                    parentCaretMappableDataFile),
m_lineLayerContentType(lineLayerContentType)
{
    CaretUnitsTypeEnum::Enum xAxisUnits = CaretUnitsTypeEnum::NONE;
    int32_t xAxisNumberOfElements = 0;
    
    m_defaultColor = ChartableTwoFileLineLayerChart::generateDefaultColor();
    validateDefaultColor();
    
    m_defaultLineWidth = ChartTwoDataCartesian::getDefaultLineWidth();

    CaretMappableDataFile* cmdf = getCaretMappableDataFile();
    CaretAssert(cmdf);
    
    const bool brainordinateDataSupportedFlag(false);
    switch (lineLayerContentType) {
        case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED:
            break;
        case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA:
            if (brainordinateDataSupportedFlag) {
                if (cmdf->getNumberOfMaps() > 1) {
                    const NiftiTimeUnitsEnum::Enum mapUnits = cmdf->getMapIntervalUnits();
                    xAxisUnits = CaretUnitsTypeEnum::NONE;
                    switch (mapUnits) {
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                            xAxisUnits = CaretUnitsTypeEnum::HERTZ;
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                            xAxisUnits = CaretUnitsTypeEnum::SECONDS;
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                            xAxisUnits = CaretUnitsTypeEnum::PARTS_PER_MILLION;
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                            xAxisUnits = CaretUnitsTypeEnum::SECONDS;
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                            xAxisUnits = CaretUnitsTypeEnum::SECONDS;
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                            break;
                    }
                    xAxisNumberOfElements = cmdf->getNumberOfMaps();
                }
            }
            break;
        case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_ROW_SCALAR_DATA:
        {
            const CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
            CaretAssert(ciftiMapFile);
            std::vector<int64_t> dims;
            ciftiMapFile->getMapDimensions(dims);
            CaretAssertVectorIndex(dims, 1);
            const int32_t numCols = dims[0];
            const int32_t numRows = dims[1];
            
            if ((numRows > 0)
                && (numCols > 1)) {
                const NiftiTimeUnitsEnum::Enum mapUnits = ciftiMapFile->getMapIntervalUnits();
                xAxisUnits = CaretUnitsTypeEnum::NONE;
                switch (mapUnits) {
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                        xAxisUnits = CaretUnitsTypeEnum::HERTZ;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                        xAxisUnits = CaretUnitsTypeEnum::SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                        xAxisUnits = CaretUnitsTypeEnum::PARTS_PER_MILLION;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                        xAxisUnits = CaretUnitsTypeEnum::SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                        xAxisUnits = CaretUnitsTypeEnum::SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                        break;
                }
                xAxisNumberOfElements = numCols;
            }
        }
            break;
    }

    /*
     * Must have two or more elements
     */
    if (xAxisNumberOfElements <= 1) {
        m_lineLayerContentType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED;
    }
    
    updateChartTwoCompoundDataTypeAfterFileChanges(ChartTwoCompoundDataType::newInstanceForLineLayer(xAxisUnits,
                                                                                                   xAxisNumberOfElements));
    CaretMappableDataFile* mapFile = getCaretMappableDataFile();
    CaretAssert(mapFile);
    m_mapLineCharts.resize(mapFile->getNumberOfMaps());
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_defaultColor",
                                                                &m_defaultColor);
    m_sceneAssistant->add("m_defaultLineWidth",
                          &m_defaultLineWidth);

}

/**
 * Destructor.
 */
ChartableTwoFileLineLayerChart::~ChartableTwoFileLineLayerChart()
{
    /*
     * If events added, remove only events add by this instance
     * since parent also receives events
     */
}

/**
 * Clear the chart lines
 */
void
ChartableTwoFileLineLayerChart::clearChartLines()
{
    for (auto& ptr : m_mapLineCharts) {
        ptr.reset();
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartableTwoFileLineLayerChart::receiveEvent(Event* event)
{
    ChartableTwoFileBaseChart::receiveEvent(event);
}

/**
 * Get a bounding box for map data displayed within this overlay.
 * Bounds are provided for histogram and line-series charts only.
 * @param mapIndex
 *     Index of map
 * @param boundingBox
 *     Upon exit contains bounds for data within this overlay
 * @return
 *     True if the bounds are valid, else false.
 */bool
ChartableTwoFileLineLayerChart::getBounds(const int32_t mapIndex,
                                          BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.resetForUpdate();
    ChartableTwoFileLineLayerChart* nonConst = const_cast<ChartableTwoFileLineLayerChart*>(this);
    CaretAssert(nonConst);
    const ChartTwoDataCartesian* cd = nonConst->getLineChartForMap(mapIndex);
    CaretAssert(cd);
    cd->getBounds(boundingBoxOut);
    return true;
}


/**
 * Get the chart lines for a map
 * @param mapIndex
 * Index of the map
 * @retrurn Line chart for map or NULL if not available.
 */
ChartTwoDataCartesian*
ChartableTwoFileLineLayerChart::getLineChartForMap(const int32_t mapIndex)
{
    ChartTwoDataCartesian* chartDataOut(NULL);
    
    CaretMappableDataFile* mapFile = getCaretMappableDataFile();
    CaretAssert(mapFile);
    const AString mapFileName(mapFile->getFileName());
    
    if ((mapIndex >= 0)
        && (mapIndex < mapFile->getNumberOfMaps())) {
        CaretAssertVectorIndex(m_mapLineCharts, mapIndex);
        if ( ! m_mapLineCharts[mapIndex]) {
            MapFileDataSelector mapFileSelector;
            bool loadDataFlag(true);
            switch (getLineLayerContentType()) {
                case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED:
                    break;
                case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA:
                    break;
                case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_ROW_SCALAR_DATA:
                    mapFileSelector.setRowIndex(mapFile,
                                                mapFileName,
                                                mapIndex);
                    loadDataFlag = true;
                    break;
            }
            
            if (loadDataFlag) {
                ChartTwoDataCartesian* cd = loadChartForMapFileSelector(mapFileSelector);
                if (cd != NULL) {
                    CaretAssertVectorIndex(m_mapLineCharts, mapIndex);
                    m_mapLineCharts[mapIndex].reset(cd);
                }
            }
        }
        
        CaretAssertVectorIndex(m_mapLineCharts, mapIndex);
        chartDataOut = m_mapLineCharts[mapIndex].get();
    }
    
    return chartDataOut;
}

/**
 * Get the chart lines for a map file selector
 * @param mapFileDataSelector
 * Map file selector
 * @retrurn Line chart for map or NULL if not available.
 */
ChartTwoDataCartesian*
ChartableTwoFileLineLayerChart::loadChartForMapFileSelector(const MapFileDataSelector& mapFileDataSelector)
{
    ChartTwoDataCartesian* chartDataOut(NULL);
    
    int32_t scalarRowIndex = -1;
    bool loadDataFlag = false;
    switch (m_lineLayerContentType) {
        case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED:
            return NULL;
            break;
        case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA:
            switch (mapFileDataSelector.getDataSelectionType()) {
                case MapFileDataSelector::DataSelectionType::INVALID:
                    break;
                case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
                    break;
                case MapFileDataSelector::DataSelectionType::ROW_DATA:
                    break;
                case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
                    loadDataFlag = true;
                    break;
                case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
                    loadDataFlag = true;
                    break;
                case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
                    loadDataFlag = true;
                    break;
            }
            break;
        case ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_ROW_SCALAR_DATA:
            switch (mapFileDataSelector.getDataSelectionType()) {
                case MapFileDataSelector::DataSelectionType::INVALID:
                    break;
                case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
                {
                    CaretMappableDataFile* mapFile = NULL;
                    AString mapFileName;
                    int32_t columnIndex = -1;
                    mapFileDataSelector.getColumnIndex(mapFile, mapFileName, columnIndex);
                    if (mapFile == getCaretMappableDataFile()) {
                        loadDataFlag = true;
                    }
                }
                    break;
                case MapFileDataSelector::DataSelectionType::ROW_DATA:
                {
                    CaretMappableDataFile* mapFile = NULL;
                    AString mapFileName;
                    int32_t rowIndex = -1;
                    mapFileDataSelector.getRowIndex(mapFile, mapFileName, rowIndex);
                    if (mapFile == getCaretMappableDataFile()) {
                        loadDataFlag = true;
                    }
                    scalarRowIndex = rowIndex;
                }
                    break;
                case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
                    break;
                case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
                    break;
                case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
                    break;
            }
            break;
    }
    
    if (loadDataFlag) {
        std::vector<float> data;
        getCaretMappableDataFile()->getDataForSelector(mapFileDataSelector,
                                                       data);
        if ( ! data.empty()) {
            CaretAssert(getChartTwoCompoundDataType().getLineChartNumberOfElementsAxisX() == static_cast<int32_t>(data.size()));
            chartDataOut = createChartData();
            chartDataOut->setMapFileDataSelector(mapFileDataSelector);
            
            float x = 0.0f;
            float xStep = 0.0f;
            getCaretMappableDataFile()->getMapIntervalStartAndStep(x, xStep);
            
            const int32_t numData = static_cast<int32_t>(data.size());
            for (int32_t i = 0; i < numData; i++) {
                CaretAssertVectorIndex(data, i);
                chartDataOut->addPoint(x, data[i]);
                x += xStep;
            }
        }
    }
    
    return chartDataOut;
}

ChartTwoDataCartesian*
ChartableTwoFileLineLayerChart::createChartData() const
{
    const CaretUnitsTypeEnum::Enum xUnits = getChartTwoCompoundDataType().getLineChartUnitsAxisX();
    return new ChartTwoDataCartesian(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES,
                                     xUnits,
                                     CaretUnitsTypeEnum::NONE,
                                     GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN);
}

/**
 * @return Content type of the line series data.
 */
ChartTwoLineLayerContentTypeEnum::Enum
ChartableTwoFileLineLayerChart::getLineLayerContentType() const
{
    return m_lineLayerContentType;
}

/**
 * @return Is this charting valid ?
 */
bool
ChartableTwoFileLineLayerChart::isValid() const
{
    return (m_lineLayerContentType != ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED);
}

/**
 * @retrurn Is this charting empty (no data at this time)
 */
bool
ChartableTwoFileLineLayerChart::isEmpty() const
{
    if ( ! isValid()) {
        return true;
    }

    return getCaretMappableDataFile()->isEmpty();
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
ChartableTwoFileLineLayerChart::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    SceneObjectMapIntegerKey* objMap(NULL);
    
    const int32_t numItems = static_cast<int32_t>(m_mapLineCharts.size());
    for (int32_t i = 0; i < numItems; i++) {
        ChartTwoDataCartesian* cd = getLineChartForMap(i);
        if (cd != NULL) {
            if (objMap == NULL) {
                objMap = new SceneObjectMapIntegerKey("m_mapLineCharts",
                                                      SceneObjectDataTypeEnum::SCENE_CLASS);
            }
            const QString name("m_mapLineCharts["
                               + AString::number(i)
                               + "]");
            CaretAssert(objMap);
            objMap->addClass(i,
                             cd->saveToScene(sceneAttributes,
                                             name));
        }
    }
    
    if (objMap != NULL) {
        sceneClass->addChild(objMap);
    }
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
ChartableTwoFileLineLayerChart::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    clearChartLines();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const SceneObjectMapIntegerKey* objMap = sceneClass->getMapIntegerKey("m_mapLineCharts");
    if (objMap != NULL) {
        if (objMap->getDataType() == SceneObjectDataTypeEnum::SCENE_CLASS) {
            const std::vector<int32_t> indexKeys = objMap->getKeys();
            for (const auto key : indexKeys) {
                const SceneObject* so = objMap->getObject(key);
                if (so != NULL) {
                    const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
                    if (sc != NULL) {
                        ChartTwoDataCartesian* cartData = createChartData();
                        CaretAssert(cartData);
                        cartData->restoreFromScene(sceneAttributes,
                                                   sc);
                        m_mapLineCharts[key].reset(cartData);
                    }
                }
            }
        }
    }
}

/**
 * Generate the default color.
 */
CaretColorEnum::Enum
ChartableTwoFileLineLayerChart::generateDefaultColor()
{
    /*
     * No black or white since they are used for backgrounds
     */
    std::vector<CaretColorEnum::Enum> colors;
    CaretColorEnum::getColorEnumsNoBlackOrWhite(colors);
    CaretAssert( ! colors.empty());
    CaretColorEnum::Enum color = colors[0];
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    CaretAssert(numColors > 0);
    if (s_defaultColorIndexGenerator < 0) {
        s_defaultColorIndexGenerator = 0;
    }
    else if (s_defaultColorIndexGenerator >= numColors) {
        s_defaultColorIndexGenerator = 0;
    }
    
    CaretAssertVectorIndex(colors, s_defaultColorIndexGenerator);
    color = colors[s_defaultColorIndexGenerator];
    
    /* move to next color */
    ++s_defaultColorIndexGenerator;
    
    return color;
}

/**
 * Validate the default color.
 */
void
ChartableTwoFileLineLayerChart::validateDefaultColor()
{
    std::vector<CaretColorEnum::Enum> allEnums;
    CaretColorEnum::getColorAndOptionalEnums(allEnums, (CaretColorEnum::ColorOptions::OPTION_INCLUDE_CUSTOM_COLOR
                                                        | CaretColorEnum::CaretColorEnum::OPTION_INCLUDE_NONE_COLOR));
    if (std::find(allEnums.begin(),
                  allEnums.end(),
                  m_defaultColor) == allEnums.end()) {
        const AString msg("Default color enum is invalid.  Integer value: " + AString::number((int)m_defaultColor));
        CaretLogSevere(msg);
        m_defaultColor = CaretColorEnum::RED;
    }
    
    if (m_defaultColor == CaretColorEnum::CUSTOM) {
        const AString msg("Default color CUSTOM is not allowed");
        CaretLogSevere(msg);
        m_defaultColor = CaretColorEnum::RED;
    }
    else if (m_defaultColor == CaretColorEnum::NONE) {
        const AString msg("Default color NONE is not allowed");
        CaretLogSevere(msg);
        m_defaultColor = CaretColorEnum::RED;
    }
}

/**
 * @return The default color.
 */
CaretColorEnum::Enum
ChartableTwoFileLineLayerChart::getDefaultColor() const
{
    return m_defaultColor;
}

/**
 * Set the default color.
 *
 * @param defaultColor New value for default color.
 */
void
ChartableTwoFileLineLayerChart::setDefaultColor(const CaretColorEnum::Enum defaultColor)
{
    if (defaultColor != m_defaultColor) {
        m_defaultColor = defaultColor;
        validateDefaultColor();
        setModified();
    }
}

/**
 * @return Default width of lines
 */
float
ChartableTwoFileLineLayerChart::getDefaultLineWidth() const
{
    return m_defaultLineWidth;
}

/**
 * Set Default width of lines
 *
 * @param defaultLineWidth
 *    New value for Default width of lines
 */
void
ChartableTwoFileLineLayerChart::setDefaultLineWidth(const float defaultLineWidth)
{
    if (defaultLineWidth != m_defaultLineWidth) {
        m_defaultLineWidth = defaultLineWidth;
        setModified();
    }
}
