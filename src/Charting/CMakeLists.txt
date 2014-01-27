
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
ChartModel.h
ChartModelCartesian.h
ChartModelLineSeries.h
ChartData.h
ChartDataCartesian.h
ChartDataTypeEnum.h
ChartPoint.h

ChartAxis.cxx
ChartAxisUnitsEnum.cxx
ChartModel.cxx
ChartModelCartesian.cxx
ChartModelLineSeries.cxx
ChartData.cxx
ChartDataCartesian.cxx
ChartDataTypeEnum.cxx
ChartPoint.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Common
)

