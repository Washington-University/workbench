
#
# Name of Project
#
PROJECT(Charting)

#
# Qt include files
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()

#
# Create a library
#
ADD_LIBRARY(Charting
ChartAxis.h
ChartAxisCartesian.h
ChartAxisLocationEnum.h
ChartAxisTypeEnum.h
ChartAxisUnitsEnum.h
ChartData.h
ChartDataCartesian.h
ChartDataSource.h
ChartDataSourceModeEnum.h
ChartMatrixDisplayProperties.h
ChartMatrixLoadingDimensionEnum.h
ChartMatrixScaleModeEnum.h
ChartModel.h
ChartModelCartesian.h
ChartModelDataSeries.h
ChartModelFrequencySeries.h
ChartModelTimeSeries.h
ChartPoint.h
ChartScaleAutoRanging.h
ChartSelectionModeEnum.h
ChartOneDataTypeEnum.h
ChartingVersionEnum.h

ChartTwoAxisScaleRangeModeEnum.h
ChartTwoCartesianAxis.h
ChartTwoCompoundDataType.h
ChartTwoData.h
ChartTwoDataCartesian.h
ChartTwoDataTypeEnum.h
ChartTwoHistogramContentTypeEnum.h
ChartTwoLineSeriesContentTypeEnum.h
ChartTwoMatrixContentTypeEnum.h
ChartTwoMatrixDisplayProperties.h
ChartTwoMatrixLoadingDimensionEnum.h
ChartTwoMatrixTriangularViewingModeEnum.h
ChartTwoNumericSubdivisionsModeEnum.h
ChartTwoLineSeriesHistory.h
EventChartTwoLoadLineSeriesData.h
MapFileDataSelector.h

ChartTwoAxisScaleRangeModeEnum.cxx
ChartAxis.cxx
ChartAxisCartesian.cxx
ChartAxisLocationEnum.cxx
ChartAxisTypeEnum.cxx
ChartAxisUnitsEnum.cxx
ChartData.cxx
ChartDataCartesian.cxx
ChartDataSource.cxx
ChartDataSourceModeEnum.cxx
ChartMatrixDisplayProperties.cxx
ChartMatrixLoadingDimensionEnum.cxx
ChartMatrixScaleModeEnum.cxx
ChartModel.cxx
ChartModelCartesian.cxx
ChartModelDataSeries.cxx
ChartModelFrequencySeries.cxx
ChartModelTimeSeries.cxx
ChartPoint.cxx
ChartScaleAutoRanging.cxx
ChartSelectionModeEnum.cxx
ChartOneDataTypeEnum.cxx
ChartingVersionEnum.cxx

ChartTwoCartesianAxis.cxx
ChartTwoCompoundDataType.cxx
ChartTwoData.cxx
ChartTwoDataCartesian.cxx
ChartTwoDataTypeEnum.cxx
ChartTwoHistogramContentTypeEnum.cxx
ChartTwoLineSeriesContentTypeEnum.cxx
ChartTwoMatrixContentTypeEnum.cxx
ChartTwoMatrixDisplayProperties.cxx
ChartTwoMatrixLoadingDimensionEnum.cxx
ChartTwoMatrixTriangularViewingModeEnum.cxx
ChartTwoNumericSubdivisionsModeEnum.cxx
ChartTwoLineSeriesHistory.cxx
EventChartTwoLoadLineSeriesData.cxx
MapFileDataSelector.cxx
)

TARGET_LINK_LIBRARIES(Charting ${CARET_QT5_LINK})

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Graphics
${CMAKE_SOURCE_DIR}/Scenes
)

