
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

#define __CHARTABLE_TWO_FILE_DELEGATE_DECLARE__
#include "ChartableTwoFileDelegate.h"
#undef __CHARTABLE_TWO_INTERFACE_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartableTwoFileHistogramChart.h"
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
 * @param type of matrix (if any) supported by implementing file.
 *
 * @param caretMappableDataFile
 *     Caret Mappable Data File this delegate charts.
 * @param histogramChart
 *     Histogram charting for Caret Mappable Data File (NULL if histogram charting not supported).
 * @param lineSeriesChart
 *     Line series charting for Caret Mappable Data File (NULL if line series charting not supported).
 * @param matrixChart
 *     Matrix charting for Caret Mappable Data File (NULL if matrix charting not supported).
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
    ChartTwoMatrixContentTypeEnum::Enum matrixType         = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED;
    
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
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA;
            matrixType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SCALARS;
            break;
        case DataFileTypeEnum::ANNOTATION:
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
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
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
            histogramType = ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA;
            lineSeriesType = ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA;
            break;
            
    }
    
    m_histogramCharting = std::unique_ptr<ChartableTwoFileHistogramChart>(new ChartableTwoFileHistogramChart(histogramType,
                                                                                           m_caretMappableDataFile));
    
    m_lineSeriesCharting = std::unique_ptr<ChartableTwoFileLineSeriesChart>(new ChartableTwoFileLineSeriesChart(lineSeriesType,
                                                                                              m_caretMappableDataFile));
    
    m_matrixCharting = std::unique_ptr<ChartableTwoFileMatrixChart>(new ChartableTwoFileMatrixChart(matrixType,
                                                                                  m_caretMappableDataFile));
}

/**
 * Clear the content.
 */
void
ChartableTwoFileDelegate::clear()
{
    m_histogramCharting = std::unique_ptr<ChartableTwoFileHistogramChart>(new ChartableTwoFileHistogramChart(ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED,
                                                                                                             m_caretMappableDataFile));
    
    m_lineSeriesCharting = std::unique_ptr<ChartableTwoFileLineSeriesChart>(new ChartableTwoFileLineSeriesChart(ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED,
                                                                                                                m_caretMappableDataFile));
    
    m_matrixCharting = std::unique_ptr<ChartableTwoFileMatrixChart>(new ChartableTwoFileMatrixChart(ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED,
                                                                                                    m_caretMappableDataFile));
}

/**
 * Clear the modification status of this instance.
 */
void
ChartableTwoFileDelegate::clearModified()
{
    m_histogramCharting->clearModified();
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

ChartableTwoFileHistogramChart* ChartableTwoFileDelegate::getHistogramCharting()
{
    return m_histogramCharting.get();
}

const ChartableTwoFileHistogramChart* ChartableTwoFileDelegate::getHistogramCharting() const
{
    return m_histogramCharting.get();
}

ChartableTwoFileLineSeriesChart* ChartableTwoFileDelegate::getLineSeriesCharting()
{
    return m_lineSeriesCharting.get();
}

const ChartableTwoFileLineSeriesChart* ChartableTwoFileDelegate::getLineSeriesCharting() const
{
    return m_lineSeriesCharting.get();
}

ChartableTwoFileMatrixChart* ChartableTwoFileDelegate::getMatrixCharting()
{
    return m_matrixCharting.get();
}

const ChartableTwoFileMatrixChart* ChartableTwoFileDelegate::getMatrixCharting() const
{
    return m_matrixCharting.get();
}

///**
// * @return The CiftiMappableDataFile that implements this interface.
// *         NULL is returned if not a CiftiMappableDataFile.
// */
//CiftiMappableDataFile*
//ChartableTwoFileDelegate::getAsCiftiMappableDataFile()
//{
//    return dynamic_cast<CiftiMappableDataFile*>(this);
//}
//
///**
// * @return The CiftiMappableDataFile that implements this interface.
// *         NULL is returned if not a CiftiMappableDataFile.
// */
//const CiftiMappableDataFile*
//ChartableTwoFileDelegate::getAsCiftiMappableDataFile() const
//{
//    return dynamic_cast<const CiftiMappableDataFile*>(this);
//}

/**
 * Does this file support any type of charting?
 */
bool
ChartableTwoFileDelegate::isChartingSupported() const
{
    std::vector<ChartTwoDataTypeEnum::Enum> chartDataTypes;
    getSupportedChartDataTypes(chartDataTypes);
    
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
ChartableTwoFileDelegate::isChartingSupportedForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const
{
    std::vector<ChartTwoDataTypeEnum::Enum> chartDataTypes;
    getSupportedChartDataTypes(chartDataTypes);
    
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
ChartableTwoFileDelegate::isChartingSupportedForChartCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const
{
    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
    getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
    
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
ChartableTwoFileDelegate::getSupportedChartDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    
    if (m_histogramCharting != NULL) {
        chartDataTypesOut.push_back(m_histogramCharting->getChartDataType());
    }
    if (m_lineSeriesCharting!= NULL) {
        chartDataTypesOut.push_back(m_lineSeriesCharting->getChartDataType());
    }
    if (m_matrixCharting != NULL) {
        chartDataTypesOut.push_back(m_matrixCharting->getChartDataType());
    }
    
    CaretAssertMessage((! chartDataTypesOut.empty()),
                       "Why is this delegate used if parent file does not support charts?");
}

/**
 * Get chart data types supported by this file.
 *
 * @param chartDataTypesOut
 *     Output containing all chart data types supported by this data file.
 */
void
ChartableTwoFileDelegate::getSupportedChartCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const
{
    chartCompoundDataTypesOut.clear();
    
    
    if (m_histogramCharting != NULL) {
        chartCompoundDataTypesOut.push_back(m_histogramCharting->getChartCompoundDataType());
    }
    if (m_lineSeriesCharting != NULL) {
        chartCompoundDataTypesOut.push_back(m_lineSeriesCharting->getChartCompoundDataType());
    }
    if (m_matrixCharting != NULL) {
        chartCompoundDataTypesOut.push_back(m_matrixCharting->getChartCompoundDataType());
    }
    
    CaretAssertMessage((! chartCompoundDataTypesOut.empty()),
                       "Why is this delegate used if parent file does not support charts?");
    
//    const CaretMappableDataFile* cmdf = getAsCaretMappableDataFile();
//    if (cmdf == NULL) {
//        return;
//    }
//    
//    switch (m_histogramContentType) {
//        case ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED:
//            break;
//        case ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA:
//            if (cmdf->getNumberOfMaps() > 0) {
//                chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForHistogram());
//            }
//            break;
//    }
//    
//    switch (m_lineSeriesContentType) {
//        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED:
//            break;
//        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA:
//            if (cmdf->getNumberOfMaps() > 1) {
//                const NiftiTimeUnitsEnum::Enum mapUnits = cmdf->getMapIntervalUnits();
//                ChartAxisUnitsEnum::Enum xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
//                switch (mapUnits) {
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
//                        CaretAssert(0);
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
//                        break;
//                }
//                const int32_t numMaps = cmdf->getNumberOfMaps();
//                chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForLineSeries(xAxisUnits,
//                                                                                                       numMaps));
//            }
//            break;
//        case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA:
//        {
//            const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(this);
//            CaretAssert(ciftiMapFile);
//            std::vector<int64_t> dims;
//            ciftiMapFile->getMapDimensions(dims);
//            CaretAssertVectorIndex(dims, 1);
//            const int32_t numCols = dims[0];
//            const int32_t numRows = dims[1];
//            
//            if ((numRows > 0)
//                && (numCols > 1)) {
//                const NiftiTimeUnitsEnum::Enum mapUnits = ciftiMapFile->getMapIntervalUnits();
//                ChartAxisUnitsEnum::Enum xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
//                switch (mapUnits) {
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_FREQUENCY_HERTZ;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
//                        CaretAssert(0);
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
//                        xAxisUnits = ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS;
//                        break;
//                    case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
//                        break;
//                }
//                chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForLineSeries(xAxisUnits,
//                                                                                                       numCols));
//            }
//        }            break;
//    }
//    
//    switch (m_matrixContentType) {
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED:
//            break;
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE:
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SCALARS:
//        {
//            const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(this);
//            CaretAssert(ciftiMapFile);
//            std::vector<int64_t> dims;
//            ciftiMapFile->getMapDimensions(dims);
//            CaretAssertVectorIndex(dims, 1);
//            const int32_t numCols = dims[0];
//            const int32_t numRows = dims[1];
//            
//            chartCompoundDataTypesOut.push_back(ChartTwoCompoundDataType::newInstanceForMatrix(numRows,
//                                                                                               numCols));
//        }            break;
//    }
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
ChartableTwoFileDelegate::getChartCompoundDataTypeForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
                                                                ChartTwoCompoundDataType& chartCompoundDataTypeOut) const
{
    std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
    getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
    for (auto& cdt : chartCompoundDataTypes) {
        if (cdt.getChartDataType() == chartDataType) {
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
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartableTwoFileDelegate",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_histogramCharting != NULL) {
        sceneClass->addClass(m_histogramCharting->saveToScene(sceneAttributes,
                                                              "m_histogramCharting"));
        sceneClass->addClass(m_lineSeriesCharting->saveToScene(sceneAttributes,
                                                              "m_lineSeriesCharting"));
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


///**
// * @return The content type for histograms.
// */
//ChartTwoHistogramContentTypeEnum::Enum ChartableTwoFileDelegate::getHistogramContentType() const
//{
//    return m_histogramContentType;
//}
//
///**
// * @return The content type for line series.
// */
//ChartTwoLineSeriesContentTypeEnum::Enum ChartableTwoFileDelegate::getLineSeriesContentType() const
//{
//    return m_lineSeriesContentType;
//}
//
///**
// * @return The content type for matrix.
// */
//ChartTwoMatrixContentTypeEnum::Enum ChartableTwoFileDelegate::getMatrixContentType() const
//{
//    return m_matrixContentType;
//}
//
//
///**
// * Get the matrix dimensions.
// *
// * @param numberOfRowsOut
// *    Number of rows in the matrix.
// * @param numberOfColumnsOut
// *    Number of rows in the matrix.
// */
//void
//ChartableTwoFileDelegate::getTwoMatrixDimensions(int32_t& numberOfRowsOut,
//                                                    int32_t& numberOfColumnsOut) const
//{
//    if (m_matrixContentType == ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED) {
//        numberOfRowsOut = 0;
//        numberOfColumnsOut = 0;
//        return;
//    }
//    
//    const CiftiMappableDataFile* ciftiMapFile = getAsCiftiMappableDataFile();
//    CaretAssert(ciftiMapFile);
//    CaretAssertToDoFatal();
////    ciftiMapFile->helpMapFileGetMatrixDimensions(numberOfRowsOut,
////                                                 numberOfColumnsOut);
//    //    std::vector<int64_t> dims;
//    //    ciftiMapFile->getMapDimensions(dims);
//    //    CaretAssertVectorIndex(dims, 1);
//    //    numberOfRowsOut    = dims[0];
//    //    numberOfColumnsOut = dims[1];
//}
//
///**
// * Get the matrix RGBA coloring for this matrix data creator.
// *
// * @param numberOfRowsOut
// *    Number of rows in the coloring matrix.
// * @param numberOfColumnsOut
// *    Number of rows in the coloring matrix.
// * @param rgbaOut
// *    RGBA coloring output with number of elements
// *    (numberOfRowsOut * numberOfColumnsOut * 4).
// * @return
// *    True if data output data is valid, else false.
// */
//bool
//ChartableTwoFileDelegate::getTwoMatrixDataRGBA(int32_t& numberOfRowsOut,
//                                                  int32_t& numberOfColumnsOut,
//                                                  std::vector<float>& rgbaOut) const
//{
//    const CaretMappableDataFile* mapFile = getAsCaretMappableDataFile();
//    CaretAssert(mapFile);
//    
//    bool useMapFileHelperFlag = false;
//    bool useMatrixFileHelperFlag = false;
//    
//    std::vector<int32_t> parcelReorderedRowIndices;
//    
//    switch (mapFile->getDataFileType()) {
//        case DataFileTypeEnum::CONNECTIVITY_DENSE:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
//        {
//            useMatrixFileHelperFlag    = true;
//            
//            const CiftiConnectivityMatrixParcelFile* parcelConnFile = dynamic_cast<const CiftiConnectivityMatrixParcelFile*>(mapFile);
//            CaretAssert(parcelConnFile);
//            if (parcelConnFile != NULL) {
//                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
//                int32_t parcelLabelFileMapIndex = -1;
//                bool reorderingEnabledFlag = false;
//                
//                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
//                parcelConnFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
//                                                                              parcelLabelReorderingFile,
//                                                                              parcelLabelFileMapIndex,
//                                                                              reorderingEnabledFlag);
//                
//                if (reorderingEnabledFlag) {
//                    const CiftiParcelReordering* parcelReordering = parcelConnFile->getParcelReordering(parcelLabelReorderingFile,
//                                                                                                        parcelLabelFileMapIndex);
//                    if (parcelReordering != NULL) {
//                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
//                    }
//                }
//            }
//        }
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
//        {
//            useMapFileHelperFlag = true;
//            
//            const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(mapFile);
//            CaretAssert(parcelLabelFile);
//            if (parcelLabelFile != NULL) {
//                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
//                int32_t parcelLabelFileMapIndex = -1;
//                bool reorderingEnabledFlag = false;
//                
//                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
//                parcelLabelFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
//                                                                               parcelLabelReorderingFile,
//                                                                               parcelLabelFileMapIndex,
//                                                                               reorderingEnabledFlag);
//                
//                if (reorderingEnabledFlag) {
//                    const CiftiParcelReordering* parcelReordering = parcelLabelFile->getParcelReordering(parcelLabelReorderingFile,
//                                                                                                         parcelLabelFileMapIndex);
//                    if (parcelReordering != NULL) {
//                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
//                    }
//                }
//            }
//        }
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
//        {
//            useMapFileHelperFlag = true;
//            
//            const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(mapFile);
//            CaretAssert(parcelScalarFile);
//            if (parcelScalarFile != NULL) {
//                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
//                int32_t parcelLabelFileMapIndex = -1;
//                bool reorderingEnabledFlag = false;
//                
//                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
//                parcelScalarFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
//                                                                                parcelLabelReorderingFile,
//                                                                                parcelLabelFileMapIndex,
//                                                                                reorderingEnabledFlag);
//                
//                if (reorderingEnabledFlag) {
//                    const CiftiParcelReordering* parcelReordering = parcelScalarFile->getParcelReordering(parcelLabelReorderingFile,
//                                                                                                          parcelLabelFileMapIndex);
//                    if (parcelReordering != NULL) {
//                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
//                    }
//                }
//            }
//        }
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
//            useMatrixFileHelperFlag = true;
//            break;
//        case DataFileTypeEnum::ANNOTATION:
//            break;
//        case DataFileTypeEnum::BORDER:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
//            break;
//        case DataFileTypeEnum::FOCI:
//            break;
//        case DataFileTypeEnum::IMAGE:
//            break;
//        case DataFileTypeEnum::LABEL:
//            break;
//        case DataFileTypeEnum::METRIC:
//            break;
//        case DataFileTypeEnum::PALETTE:
//            break;
//        case DataFileTypeEnum::RGBA:
//            break;
//        case DataFileTypeEnum::SCENE:
//            break;
//        case DataFileTypeEnum::SPECIFICATION:
//            break;
//        case DataFileTypeEnum::SURFACE:
//            break;
//        case DataFileTypeEnum::UNKNOWN:
//            break;
//        case DataFileTypeEnum::VOLUME:
//            break;
//    }
//    
//    if (( ! useMapFileHelperFlag)
//        && ( ! useMatrixFileHelperFlag)) {
//        CaretAssertMessage(0, "Trying to get matrix from a file that does not support matrix display");
//        return false;
//    }
//    
//    const CiftiMappableDataFile* ciftiMapFile = getAsCiftiMappableDataFile();
//    CaretAssert(ciftiMapFile);
//    
//    
//    bool validDataFlag = false;
//    if (useMapFileHelperFlag) {
//        CaretAssertToDoFatal();
////        validDataFlag = ciftiMapFile->helpMapFileLoadChartDataMatrixRGBA(numberOfRowsOut,
////                                                                         numberOfColumnsOut,
////                                                                         parcelReorderedRowIndices,
////                                                                         rgbaOut);
//    }
//    else if (useMatrixFileHelperFlag) {
//        CaretAssertToDoFatal();
////        validDataFlag = ciftiMapFile->helpMatrixFileLoadChartDataMatrixRGBA(numberOfRowsOut,
////                                                                            numberOfColumnsOut,
////                                                                            parcelReorderedRowIndices,
////                                                                            rgbaOut);
//    }
//    switch (m_matrixContentType) {
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED:
//            break;
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE:
//            break;
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SCALARS:
//            break;
//    }
//    
//    return validDataFlag;
//}

