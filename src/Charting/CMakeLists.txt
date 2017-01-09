
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
ChartVersionOneDataTypeEnum.h
ChartingVersionEnum.h

ChartTwoCompoundDataType.h
ChartTwoData.h
ChartTwoDataCartesian.h
ChartTwoDataCartesianHistory.h
ChartTwoDataTypeEnum.h
ChartTwoMatrixViewingTypeEnum.h

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
ChartVersionOneDataTypeEnum.cxx
ChartingVersionEnum.cxx

ChartTwoCompoundDataType.cxx
ChartTwoData.cxx
ChartTwoDataCartesian.cxx
ChartTwoDataCartesianHistory.cxx
ChartTwoDataTypeEnum.cxx
ChartTwoMatrixViewingTypeEnum.cxx
)

TARGET_LINK_LIBRARIES(Charting ${CARET_QT5_LINK})

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
)

