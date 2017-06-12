
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

#define __CHARTABLE_TWO_FILE_LINE_SERIES_CHART_DECLARE__
#include "ChartableTwoFileLineSeriesChart.h"
#undef __CHARTABLE_TWO_FILE_LINE_SERIES_CHART_DECLARE__

#include "CaretAssert.h"
#include "ChartTwoDataCartesian.h"
#include "ChartTwoLineSeriesHistory.h"
#include "CiftiMappableDataFile.h"
#include "EventChartTwoLoadLineSeriesData.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;
    
/**
 * \class caret::ChartableTwoFileLineSeriesChart 
 * \brief Implementation of base chart delegate for line series charts.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param lineSeriesContentType
 *     Content type of the line series data.
 * @param parentCaretMappableDataFile
 *     Parent caret mappable data file that this delegate supports.
 */
ChartableTwoFileLineSeriesChart::ChartableTwoFileLineSeriesChart(const ChartTwoLineSeriesContentTypeEnum::Enum lineSeriesContentType,
                                                                                 CaretMappableDataFile* parentCaretMappableDataFile)
: ChartableTwoFileBaseChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES,
                                    parentCaretMappableDataFile),
m_lineSeriesContentType(lineSeriesContentType)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    ChartAxisUnitsEnum::Enum xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
    int32_t xAxisNumberOfElements = 0;
    
    CaretMappableDataFile* cmdf = getCaretMappableDataFile();
    CaretAssert(cmdf);
    
    switch (lineSeriesContentType) {
        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED:
            break;
        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA:
            if (cmdf->getNumberOfMaps() > 1) {
                const NiftiTimeUnitsEnum::Enum mapUnits = cmdf->getMapIntervalUnits();
                xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
                switch (mapUnits) {
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                        CaretAssert(0);
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                        break;
                }
                xAxisNumberOfElements = cmdf->getNumberOfMaps();
            }
            break;
        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA:
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
                xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
                switch (mapUnits) {
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                        CaretAssert(0);
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
                        break;
                    case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                        break;
                }
                xAxisNumberOfElements = numCols;
            }
        }            break;
    }

    m_lineChartHistory = std::unique_ptr<ChartTwoLineSeriesHistory>(new ChartTwoLineSeriesHistory());
    m_sceneAssistant->add("m_lineChartHistory",
                           "ChartTwoLineSeriesHistory",
                            m_lineChartHistory.get());

    /*
     * Must have two or more elements
     */
    if (xAxisNumberOfElements <= 1) {
        m_lineSeriesContentType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED;
    }
    
    updateChartTwoCompoundDataTypeAfterFileChanges(ChartTwoCompoundDataType::newInstanceForLineSeries(xAxisUnits,
                                                                                                   xAxisNumberOfElements));
    
    if (m_lineSeriesContentType != ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED) {
        EventManager::get()->addEventListener(this,
                                              EventTypeEnum::EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA);
    }
}

/**
 * Destructor.
 */
ChartableTwoFileLineSeriesChart::~ChartableTwoFileLineSeriesChart()
{
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartableTwoFileLineSeriesChart::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_CHART_TWO_LOAD_LINE_SERIES_DATA) {
        const EventChartTwoLoadLineSeriesData* lineSeriesDataEvent = dynamic_cast<EventChartTwoLoadLineSeriesData*>(event);
        CaretAssert(lineSeriesDataEvent);
        
        if (m_lineSeriesContentType != ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED) {
            const std::vector<int32_t> tabIndicesForLoading = getTabIndicesForLoadingData(lineSeriesDataEvent->getValidTabIndices());
            if ( ! tabIndicesForLoading.empty()) {
                
                loadLineCharts(tabIndicesForLoading,
                               lineSeriesDataEvent);
            }
        }
        
        event->setEventProcessed();
    }
    else {
        ChartableTwoFileBaseChart::receiveEvent(event);
    }
}

/**
 * @param tabIndicesForLoading
 *     Tab indices for loading data.
 * @param lineSeriesDataEvent
 *     Event indicating data for loading.
 */
void
ChartableTwoFileLineSeriesChart::loadLineCharts(const std::vector<int32_t>& tabIndicesForLoading,
                                                const EventChartTwoLoadLineSeriesData* lineSeriesDataEvent)
{
    const MapFileDataSelector mapFileDataSelector = lineSeriesDataEvent->getMapFileDataSelector();
    
    bool loadDataFlag = false;
    switch (m_lineSeriesContentType) {
        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED:
            return;
            break;
        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA:
            switch (mapFileDataSelector.getDataSelectionType()) {
                case MapFileDataSelector::DataSelectionType::INVALID:
                    break;
                case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
                case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
                case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
                    loadDataFlag = true;
                    break;
            }
            break;
        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA:
            break;
    }
    
    std::cout << "Load data requested for file " << getCaretMappableDataFile()->getFileNameNoPath() << std::endl;
    
    if (loadDataFlag) {
        std::vector<float> data;
        getCaretMappableDataFile()->getDataForSelector(mapFileDataSelector,
                                                       data);
        if ( ! data.empty()) {
            std::cout << "Loaded data for file " << getCaretMappableDataFile()->getFileNameNoPath() << std::endl;
            
            const ChartAxisUnitsEnum::Enum xUnits = getChartTwoCompoundDataType().getLineChartUnitsAxisX();
            CaretAssert(getChartTwoCompoundDataType().getLineChartNumberOfElementsAxisX() == static_cast<int32_t>(data.size()));
            ChartTwoDataCartesian* cartesianData = new ChartTwoDataCartesian(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES,
                                                                             xUnits,
                                                                             ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                                                             GraphicsPrimitive::PrimitiveType::LINES);
            cartesianData->setMapFileDataSelector(mapFileDataSelector);
            
            float x = 0.0f;
            float xStep = 0.0f;
            getCaretMappableDataFile()->getMapIntervalStartAndStep(x, xStep);
            
            /*
             * Note: Start at index one since line segments are used
             */
            const int32_t numData = static_cast<int32_t>(data.size());
            for (int32_t i = 1; i < numData; i++) {
                CaretAssertVectorIndex(data, i - 1);
                cartesianData->addPoint(x, data[i - 1]);
                x += xStep;
                CaretAssertVectorIndex(data, i);
                cartesianData->addPoint(x, data[i]);
            }
//            for (float d : data) {
//                cartesianData->addPoint(x, d);
//                x += xStep;
//            }
            
            m_lineChartHistory->addHistoryItem(cartesianData);
        }
    }
}

/**
 * Find tab indices for which user has loading of data enabled in the given valid tab indices.
 *
 * @param validTabIndices
 *     All tabs that are valid.
 * @return 
 *     Tab indices for which data should be loaded.
 */
std::vector<int32_t>
ChartableTwoFileLineSeriesChart::getTabIndicesForLoadingData(const std::vector<int32_t>& validTabIndices) const
{
    std::vector<int32_t> tabIndicesOut;
    
    if ( ! validTabIndices.empty()) {
        if (m_lineChartHistory->isLoadingEnabled()) {
            tabIndicesOut = validTabIndices;
        }
    }
    
    return tabIndicesOut;
}


/**
 * @return Content type of the line series data.
 */
ChartTwoLineSeriesContentTypeEnum::Enum
ChartableTwoFileLineSeriesChart::getLineSeriesContentType() const
{
    return m_lineSeriesContentType;
}

/**
 * @return History of line charts.
 */
ChartTwoLineSeriesHistory*
ChartableTwoFileLineSeriesChart::getHistory()
{
    return m_lineChartHistory.get();
}

/**
 * @return History of line charts (const method)
 */
const ChartTwoLineSeriesHistory*
ChartableTwoFileLineSeriesChart::getHistory() const
{
    return m_lineChartHistory.get();
}


/**
 * @return Is this charting valid ?
 */
bool
ChartableTwoFileLineSeriesChart::isValid() const
{
    return (m_lineSeriesContentType != ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED);
}

/**
 * @retrurn Is this charting empty (no data at this time)
 */
bool
ChartableTwoFileLineSeriesChart::isEmpty() const
{
    CaretAssertToDoWarning();   // use history???
    return false;
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
ChartableTwoFileLineSeriesChart::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
ChartableTwoFileLineSeriesChart::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

