
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
#include "ChartTwoDataCartesianHistory.h"
#include "CiftiMappableDataFile.h"
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

    m_lineChartHistory = std::unique_ptr<ChartTwoDataCartesianHistory>(new ChartTwoDataCartesianHistory());
    m_sceneAssistant->add("m_lineChartHistory",
                           "ChartTwoDataCartesianHistory",
                            m_lineChartHistory.get());

    /*
     * Must have two or more elements
     */
    if (xAxisNumberOfElements <= 1) {
        m_lineSeriesContentType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED;
    }
    
    updateChartCompoundDataTypeAfterFileChanges(ChartTwoCompoundDataType::newInstanceForLineSeries(xAxisUnits,
                                                                                                   xAxisNumberOfElements));
}

/**
 * Destructor.
 */
ChartableTwoFileLineSeriesChart::~ChartableTwoFileLineSeriesChart()
{
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
ChartTwoDataCartesianHistory*
ChartableTwoFileLineSeriesChart::getHistory()
{
    return m_lineChartHistory.get();
}

/**
 * @return History of line charts (const method)
 */
const ChartTwoDataCartesianHistory*
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

