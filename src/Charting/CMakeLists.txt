
#
# Name of Project
#
PROJECT(Charting)

#
# Create a library
#
ADD_LIBRARY(Charting
ChartAxis.h
ChartAxisUnitsEnum.h
ChartData.h
ChartDataCartesian.h
ChartDataSource.h
ChartDataSourceModeEnum.h
ChartDataTypeEnum.h
ChartModel.h
ChartModelCartesian.h
ChartModelDataSeries.h
ChartPoint.h

ChartAxis.cxx
ChartAxisUnitsEnum.cxx
ChartData.cxx
ChartDataCartesian.cxx
ChartDataSource.cxx
ChartDataSourceModeEnum.cxx
ChartDataTypeEnum.cxx
ChartModel.cxx
ChartModelCartesian.cxx
ChartModelDataSeries.cxx
ChartPoint.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
)

