
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

#define __CHARTABLE_TWO_INTERFACE_DECLARE__
#include "ChartableTwoInterface.h"
#undef __CHARTABLE_TWO_INTERFACE_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelScalarFile.h"
using namespace caret;


    
/**
 * \class caret::ChartableTwoInterface 
 * \brief Interface for files that are chartable.  Contains functionality but no data.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param type of matrix (if any) supported by implementing file.
 *
 * @param histogramContentType
 *     Type of histogram chart supported (or not supported) by implementing file.
 * @param lineSeriesContentType
 *     Type of line series chart supported (or not supported) by implementing file.
 * @param matrixContentType
 *     Type of matrix chart supported (or not supported) by implementing file.
 */
ChartableTwoInterface::ChartableTwoInterface()
{
    
}

/**
 * Destructor.
 */
ChartableTwoInterface::~ChartableTwoInterface()
{
}

///**
// * @return The CaretMappableDataFile that implements this interface.
// *         NULL is returned if not a CaretMappableDataFile.
// */
//CaretMappableDataFile*
//ChartableTwoInterface::getAsCaretMappableDataFile()
//{
//    return dynamic_cast<CaretMappableDataFile*>(this);
//}
//
///**
// * @return The CaretMappableDataFile that implements this interface.
// *         NULL is returned if not a CaretMappableDataFile.
// */
//const CaretMappableDataFile*
//ChartableTwoInterface::getAsCaretMappableDataFile() const
//{
//    return dynamic_cast<const CaretMappableDataFile*>(this);
//}
//
///**
// * @return The CiftiMappableDataFile that implements this interface.
// *         NULL is returned if not a CiftiMappableDataFile.
// */
//CiftiMappableDataFile*
//ChartableTwoInterface::getAsCiftiMappableDataFile()
//{
//    return dynamic_cast<CiftiMappableDataFile*>(this);
//}
//
///**
// * @return The CiftiMappableDataFile that implements this interface.
// *         NULL is returned if not a CiftiMappableDataFile.
// */
//const CiftiMappableDataFile*
//ChartableTwoInterface::getAsCiftiMappableDataFile() const
//{
//    return dynamic_cast<const CiftiMappableDataFile*>(this);
//}

/**
 * Does this file support any type of charting?
 */
bool
ChartableTwoInterface::isChartingSupported() const
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
ChartableTwoInterface::isChartingSupportedForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const
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
ChartableTwoInterface::isChartingSupportedForChartCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const
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
ChartableTwoInterface::getSupportedChartDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    
    if (getHistogramChartDelegate() != NULL) {
        chartDataTypesOut.push_back(getHistogramChartDelegate()->getChartDataType());
    }
    if (getLineSeriesChartDelegate() != NULL) {
        chartDataTypesOut.push_back(getLineSeriesChartDelegate()->getChartDataType());
    }
    if (getMatrixChartDelegate() != NULL) {
        chartDataTypesOut.push_back(getMatrixChartDelegate()->getChartDataType());
    }
    
    CaretAssertMessage((! chartDataTypesOut.empty()),
                       "Why is this interface implemented if implementing file does not support charts?");
}

/**
 * Get chart data types supported by this file.
 *
 * @param chartDataTypesOut
 *     Output containing all chart data types supported by this data file.
 */
void
ChartableTwoInterface::getSupportedChartCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const
{
    chartCompoundDataTypesOut.clear();
    
    
    if (getHistogramChartDelegate() != NULL) {
        chartCompoundDataTypesOut.push_back(getHistogramChartDelegate()->getChartCompoundDataType());
    }
    if (getLineSeriesChartDelegate() != NULL) {
        chartCompoundDataTypesOut.push_back(getLineSeriesChartDelegate()->getChartCompoundDataType());
    }
    if (getMatrixChartDelegate() != NULL) {
        chartCompoundDataTypesOut.push_back(getMatrixChartDelegate()->getChartCompoundDataType());
    }
    
    
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
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SERIES:
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
ChartableTwoInterface::getChartCompoundDataTypeForChartDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
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


///**
// * @return The content type for histograms.
// */
//ChartTwoHistogramContentTypeEnum::Enum ChartableTwoInterface::getHistogramContentType() const
//{
//    return m_histogramContentType;
//}
//
///**
// * @return The content type for line series.
// */
//ChartTwoLineSeriesContentTypeEnum::Enum ChartableTwoInterface::getLineSeriesContentType() const
//{
//    return m_lineSeriesContentType;
//}
//
///**
// * @return The content type for matrix.
// */
//ChartTwoMatrixContentTypeEnum::Enum ChartableTwoInterface::getMatrixContentType() const
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
//ChartableTwoInterface::getTwoMatrixDimensions(int32_t& numberOfRowsOut,
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
//ChartableTwoInterface::getTwoMatrixDataRGBA(int32_t& numberOfRowsOut,
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
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SERIES:
//            break;
//    }
//    
//    return validDataFlag;
//}

