
#
# Name of Project
#
PROJECT(Charting)

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
ChartDataTypeEnum.h
ChartMatrixDisplayProperties.h
ChartMatrixLoadingTypeEnum.h
ChartMatrixScaleModeEnum.h
ChartModel.h
ChartModelCartesian.h
ChartModelDataSeries.h
ChartModelTimeSeries.h
ChartPoint.h
ChartScaleAutoRanging.h
ChartSelectionModeEnum.h

ChartAxis.cxx
ChartAxisCartesian.cxx
ChartAxisLocationEnum.cxx
ChartAxisTypeEnum.cxx
ChartAxisUnitsEnum.cxx
ChartData.cxx
ChartDataCartesian.cxx
ChartDataSource.cxx
ChartDataSourceModeEnum.cxx
ChartDataTypeEnum.cxx
ChartMatrixDisplayProperties.cxx
ChartMatrixLoadingTypeEnum.cxx
ChartMatrixScaleModeEnum.cxx
ChartModel.cxx
ChartModelCartesian.cxx
ChartModelDataSeries.cxx
ChartModelTimeSeries.cxx
ChartPoint.cxx
ChartScaleAutoRanging.cxx
ChartSelectionModeEnum.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
)

