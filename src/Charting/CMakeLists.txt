
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
ChartDataMatrix.h
ChartDataSource.h
ChartDataSourceModeEnum.h
ChartDataTypeEnum.h
ChartModel.h
ChartModelCartesian.h
ChartModelDataSeries.h
ChartModelMatrix.h
ChartPoint.h
ChartScaleAutoRanging.h

ChartAxis.cxx
ChartAxisCartesian.cxx
ChartAxisLocationEnum.cxx
ChartAxisTypeEnum.cxx
ChartAxisUnitsEnum.cxx
ChartData.cxx
ChartDataCartesian.cxx
ChartDataMatrix.cxx
ChartDataSource.cxx
ChartDataSourceModeEnum.cxx
ChartDataTypeEnum.cxx
ChartModel.cxx
ChartModelCartesian.cxx
ChartModelDataSeries.cxx
ChartModelMatrix.cxx
ChartPoint.cxx
ChartScaleAutoRanging.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
)

