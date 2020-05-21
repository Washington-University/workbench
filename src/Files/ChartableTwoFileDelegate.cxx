
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <algorithm>

#define __CHARTABLE_TWO_FILE_DELEGATE_DECLARE__
#include "ChartableTwoFileDelegate.h"
#undef __CHARTABLE_TWO_INTERFACE_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineLayerChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelScalarFile.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartableTwoFileDelegate 
 * \brief Interface for files that are chartable.  Contains functionality but no data.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param caretMappableDataFile
 *     Caret Mappable Data File this delegate charts.
 */
ChartableTwoFileDelegate::ChartableTwoFileDelegate(CaretMappableDataFile* caretMappableDataFile)
: CaretObjectTracksModification(),
SceneableInterface(),
m_caretMappableDataFile(caretMappableDataFile)
{
    CaretAssert(m_caretMappableDataFile);
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    clear();
}

/**
 * Destructor.
 */
ChartableTwoFileDelegate::~ChartableTwoFileDelegate()
{
}

/**
 * Update after the file has been read or content has changed.
 */
void
ChartableTwoFileDelegate::updateAfterFileChanged()
{
    CaretAssert(m_caretMappableDataFile);
    
    ChartTwoHistogramContentTypeEnum::Enum histogramType   = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED;
    ChartTwoLineSeriesContentTypeEnum::Enum lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED;
    ChartTwoLineLayerContentTypeEnum::Enum lineLayerType   = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED;
    ChartTwoMatrixContentTypeEnum::Enum matrixType         = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED;
    
    std::vector<ChartTwoMatrixLoadingDimensionEnum::Enum> validMatrixRowColumnSelectionDimensions;
    
    switch (m_caretMappableDataFile->getDataFileType()) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            validMatrixRowColumnSelectionDimensions.push_back(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW);
            validMatrixRowColumnSelectionDimensions.push_back(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN);
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            validMatrixRowColumnSelectionDimensions.push_back(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN);
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            validMatrixRowColumnSelectionDimensions.push_back(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN);
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            validMatrixRowColumnSelectionDimensions.push_back(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN);
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_ROW_SCALAR_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SCALARS;
            validMatrixRowColumnSelectionDimensions.push_back(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW);
            break;
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
        case DataFileTypeEnum::VOLUME:
            if ( ! m_caretMappableDataFile->isMappedWithLabelTable()) {
                histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
                lineLayerType = ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_BRAINORDINATE_DATA;
                lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            }
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
    }
    
    if (m_histogramCharting) {
        if (histogramType != m_histogramCharting->getHistogramContentType()) {
            m_histogramCharting.reset();
        }
    }
    if ( ! m_histogramCharting) {
        m_histogramCharting = std::unique_ptr<ChartableTwoFileHistogramChart>(new ChartableTwoFileHistogramChart(histogramType,
                                                                                                                 m_caretMappableDataFile));
    }
    
    if (m_lineLayerCharting) {
        if (lineLayerType != m_lineLayerCharting->getLineLayerContentType()) {
            m_lineLayerCharting.reset();
        }
    }
    if ( ! m_lineLayerCharting) {
        m_lineLayerCharting = std::unique_ptr<ChartableTwoFileLineLayerChart>(new ChartableTwoFileLineLayerChart(lineLayerType,
                                                                                                                    m_caretMappableDataFile));
    }
    
    if (m_lineSeriesCharting) {
        if (lineSeriesType != m_lineSeriesCharting->getLineSeriesContentType()) {
            m_lineSeriesCharting.reset();
        }
    }
    if ( ! m_lineSeriesCharting) {
        m_lineSeriesCharting = std::unique_ptr<ChartableTwoFileLineSeriesChart>(new ChartableTwoFileLineSeriesChart(lineSeriesType,
                                                                                                                    m_caretMappableDataFile));
    }
    
    if (m_matrixCharting) {
        if (matrixType != m_matrixCharting->getMatrixContentType()) {
            m_matrixCharting.reset();
        }
        else {
            /*
             * Test for change in valid dimensions but do not alter content of
             * "validMatrixRowColumnSelectionDimensions" since it order is important
             */
            std::vector<ChartTwoMatrixLoadingDimensionEnum::Enum> newDims(validMatrixRowColumnSelectionDimensions);
            std::sort(newDims.begin(),
                      newDims.end());
            std::vector<ChartTwoMatrixLoadingDimensionEnum::Enum> currentDims;
            m_matrixCharting->getValidRowColumnSelectionDimensions(currentDims);
            std::sort(currentDims.begin(),
                      currentDims.end());

            if ( ! std::equal(currentDims.begin(),
                              currentDims.end(),
                              newDims.begin())) {
                m_matrixCharting.reset();
            }
        }
    }
    
    if ( ! m_matrixCharting) {
        m_matrixCharting = std::unique_ptr<ChartableTwoFileMatrixChart>(new ChartableTwoFileMatrixChart(matrixType,
                                                                                                        m_caretMappableDataFile,
                                                                                                        validMatrixRowColumnSelectionDimensions));
    }
    
    clearModified();
}

/**
 * Clear the content.
 */
void
ChartableTwoFileDelegate::clear()
{
    m_histogramCharting = std::unique_ptr<ChartableTwoFileHistogramChart>(new ChartableTwoFileHistogramChart(ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED,
                                                                                                             m_caretMappableDataFile));
    
    m_lineLayerCharting = std::unique_ptr<ChartableTwoFileLineLayerChart>(new ChartableTwoFileLineLayerChart(ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED,
                                                                                                                m_caretMappableDataFile));
    
    m_lineSeriesCharting = std::unique_ptr<ChartableTwoFileLineSeriesChart>(new ChartableTwoFileLineSeriesChart(ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED,
                                                                                                                m_caretMappableDataFile));
    
    std::vector<ChartTwoMatrixLoadingDimensionEnum::Enum> validMatrixRowColumnSelectionDimensions;
    m_matrixCharting = std::unique_ptr<ChartableTwoFileMatrixChart>(new ChartableTwoFileMatrixChart(ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED,
                                                                                                    m_caretMappableDataFile,
                                                                                                    validMatrixRowColumnSelectionDimensions));
}

/**
 * Clear the modification status of this instance.
 */
void
ChartableTwoFileDelegate::clearModified()
{
    m_histogramCharting->clearModified();
    m_lineLayerCharting->clearModified();
    m_lineSeriesCharting->clearModified();
    m_matrixCharting->clearModified();
}

/**
 * @return True if this instance is modified, else false.
 */
bool
ChartableTwoFileDelegate::isModified() const
{
    if (m_histogramCharting->isModified()) {
        return true;
    }
    if (m_matrixCharting->isModified()) {
        return true;
    }
    if (m_lineLayerCharting->isModified()) {
        return true;
    }
    if (m_lineSeriesCharting->isModified()) {
        return true;
    }
    
    return false;
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 *         NULL is returned if not a CaretMappableDataFile.
 */
CaretMappableDataFile*
ChartableTwoFileDelegate::getCaretMappableDataFile()
{
    return m_caretMappableDataFile;
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 *         NULL is returned if not a CaretMappableDataFile.
 */
const CaretMappableDataFile*
ChartableTwoFileDelegate::getCaretMappableDataFile() const
{
    return m_caretMappableDataFile;
}

/**
 * @return Histogram charting.
 */
ChartableTwoFileHistogramChart*
ChartableTwoFileDelegate::getHistogramCharting()
{
    return m_histogramCharting.get();
}

/**
 * @return Histogram charting.
 */
const ChartableTwoFileHistogramChart*
ChartableTwoFileDelegate::getHistogramCharting() const
{
    return m_histogramCharting.get();
}

/**
 * @return Line layer charting.
 */
ChartableTwoFileLineLayerChart*
ChartableTwoFileDelegate::getLineLayerCharting()
{
    return m_lineLayerCharting.get();
}

/**
 * @return Line layer charting.
 */
const ChartableTwoFileLineLayerChart*
ChartableTwoFileDelegate::getLineLayerCharting() const
{
    return m_lineLayerCharting.get();
}

/**
 * @return Line series charting.
 */
ChartableTwoFileLineSeriesChart*
ChartableTwoFileDelegate::getLineSeriesCharting()
{
    return m_lineSeriesCharting.get();
}

/**
 * @return Line series charting.
 */
const ChartableTwoFileLineSeriesChart*
ChartableTwoFileDelegate::getLineSeriesCharting() const
{
    return m_lineSeriesCharting.get();
}

/**
 * @return Matrix charting.
 */
ChartableTwoFileMatrixChart*
ChartableTwoFileDelegate::getMatrixCharting()
{
    return m_matrixCharting.get();
}

/**
 * @return Matrix charting.
 */
const ChartableTwoFileMatrixChart*
ChartableTwoFileDelegate::getMatrixCharting() const
{
    return m_matrixCharting.get();
}

/**
 * Does this file support any type of charting?
 */
bool
ChartableTwoFileDelegate::isChartingTwoSupported() const
{
    std::vector<ChartTwoDataTypeEnum::Enum> chartDataTypes;
    getSupportedChartTwoDataTypes(chartDataTypes);
    
    return ( ! chartDataTypes.empty());
}

/**
 * Test for support of the given chart data type.
 *
 * @param chartDataType
 *     Type of chart data.
 * @return
 *     True if the chart data type is supported, else false.
 */
bool
ChartableTwoFileDelegate::isChartingSupportedForChartTwoDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const
{
    std::vector<ChartTwoDataTypeEnum::Enum> chartDataTypes;
    getSupportedChartTwoDataTypes(chartDataTypes);
    
    if (std::find(chartDataTypes.begin(),
                  chartDataTypes.end(),
                  chartDataType) != chartDataTypes.end()) {
        return true;
    }
    
    return false;
}

/**
 * Test for support of the given chart compound data type.
 *
 * @param chartCompoundDataType
 *     Type of chart compound data.
 * @return
 *     True if the chart compound data type is supported, else false.
 */
bool
ChartableTwoFileDelegate::isChartingSupportedForChartTwoCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const
{
    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
    getSupportedChartTwoCompoundDataTypes(chartCompoundDataTypes);
    
    for (auto& ccdt : chartCompoundDataTypes) {
        if (ccdt == chartCompoundDataType) {
            return true;
        }
    }
    
    return false;
}

/**
 * Get chart data types supported by this file.
 *
 * @param chartDataTypesOut
 *     Output containing all chart data types supported by this data file.
 */
void
ChartableTwoFileDelegate::getSupportedChartTwoDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    
    if (m_histogramCharting != NULL) {
        chartDataTypesOut.push_back(m_histogramCharting->getChartTwoDataType());
    }
    if (m_lineLayerCharting != NULL) {
        chartDataTypesOut.push_back(m_lineLayerCharting->getChartTwoDataType());
    }
    if (m_lineSeriesCharting!= NULL) {
        chartDataTypesOut.push_back(m_lineSeriesCharting->getChartTwoDataType());
    }
    if (m_matrixCharting != NULL) {
        chartDataTypesOut.push_back(m_matrixCharting->getChartTwoDataType());
    }
}

/**
 * Get chart data types supported by this file.
 *
 * @param chartDataTypesOut
 *     Output containing all chart data types supported by this data file.
 */
void
ChartableTwoFileDelegate::getSupportedChartTwoCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const
{
    chartCompoundDataTypesOut.clear();
    
    if (m_histogramCharting->getHistogramContentType() != ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED) {
        chartCompoundDataTypesOut.push_back(m_histogramCharting->getChartTwoCompoundDataType());
    }
    if (m_lineLayerCharting->getLineLayerContentType() != ChartTwoLineLayerContentTypeEnum::LINE_LAYER_CONTENT_UNSUPPORTED) {
        chartCompoundDataTypesOut.push_back(m_lineLayerCharting->getChartTwoCompoundDataType());
    }
    if (m_lineSeriesCharting->getLineSeriesContentType() != ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED) {
        chartCompoundDataTypesOut.push_back(m_lineSeriesCharting->getChartTwoCompoundDataType());
    }
    if (m_matrixCharting->getMatrixContentType() != ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED) {
        chartCompoundDataTypesOut.push_back(m_matrixCharting->getChartTwoCompoundDataType());
    }
}

/**
 * Get the chart compound data type supported by this file that uses the given
 * chart data type.
 *
 * @param chartDataType
 *     The chart data type.
 * @param chartCompoundDataTypeOut
 *     Output with the chart compound data type.
 * @return
 *     True if there is output chart compound data type is valid.
 *     False if output chart compound data type is invalid OR if chartDataType is invalid.
 */
bool
ChartableTwoFileDelegate::getChartTwoCompoundDataTypeForChartTwoDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
                                                                ChartTwoCompoundDataType& chartCompoundDataTypeOut) const
{
    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
    getSupportedChartTwoCompoundDataTypes(chartCompoundDataTypes);
    for (auto& cdt : chartCompoundDataTypes) {
        if (cdt.getChartTwoDataType() == chartDataType) {
            chartCompoundDataTypeOut = cdt;
            return true;
        }
    }
    
    /* default constructor is invalid data type */
    chartCompoundDataTypeOut = ChartTwoCompoundDataType();
    return false;
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
ChartableTwoFileDelegate::saveToScene(const SceneAttributes* sceneAttributes,
                                               const AString& instanceName)
{
    if ( ! isChartingTwoSupported()) {
        return NULL;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartableTwoFileDelegate",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_histogramCharting != NULL) {
        sceneClass->addClass(m_histogramCharting->saveToScene(sceneAttributes,
                                                              "m_histogramCharting"));
    }
    if (m_lineLayerCharting != NULL) {
        sceneClass->addClass(m_lineLayerCharting->saveToScene(sceneAttributes,
                                                              "m_lineLayerCharting"));
    }
    if (m_lineSeriesCharting != NULL) {
        sceneClass->addClass(m_lineSeriesCharting->saveToScene(sceneAttributes,
                                                               "m_lineSeriesCharting"));
    }
    if (m_matrixCharting != NULL) {
        sceneClass->addClass(m_matrixCharting->saveToScene(sceneAttributes,
                                                           "m_matrixCharting"));
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
ChartableTwoFileDelegate::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                    const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const SceneClass* histogramClass = sceneClass->getClass("m_histogramCharting");
    if (histogramClass != NULL) {
        CaretAssert(m_histogramCharting);
        m_histogramCharting->restoreFromScene(sceneAttributes,
                                              histogramClass);
    }
    
    const SceneClass* lineLayerClass = sceneClass->getClass("m_lineLayerCharting");
    if (lineLayerClass != NULL) {
        CaretAssert(m_lineLayerCharting);
        m_lineLayerCharting->restoreFromScene(sceneAttributes,
                                              lineLayerClass);
    }
    
    const SceneClass* lineSeriesClass = sceneClass->getClass("m_lineSeriesCharting");
    if (lineSeriesClass != NULL) {
        CaretAssert(m_lineSeriesCharting);
        m_lineSeriesCharting->restoreFromScene(sceneAttributes,
                                               lineSeriesClass);
    }
    
    const SceneClass* matrixClass = sceneClass->getClass("m_matrixCharting");
    if (matrixClass != NULL) {
        CaretAssert(m_matrixCharting);
        m_matrixCharting->restoreFromScene(sceneAttributes,
                                           matrixClass);
    }    
}

