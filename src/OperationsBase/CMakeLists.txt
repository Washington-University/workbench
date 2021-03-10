#
# Name of project
#
PROJECT (OperationsBase)

#
# Add QT for includes
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    #include_directories(${Qt5Network_INCLUDE_DIRS})
endif()

#
# Create the helper library
#
ADD_LIBRARY(OperationsBase
AbstractOperation.h
CaretCommandGlobalOptions.h
OperationParameters.h
OperationParametersEnum.h

AbstractOperation.cxx
CaretCommandGlobalOptions.cxx
OperationParameters.cxx
OperationParametersEnum.cxx
)

TARGET_LINK_LIBRARIES(OperationsBase ${CARET_QT5_LINK})

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
