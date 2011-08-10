#
# Name of project
#
PROJECT (CommandOperations)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create the brain library
#
ADD_LIBRARY(CommandOperations
CommandOperation.h
CommandOperationManager.h
CommandException.h
CommandUnitTest.h

CommandOperation.cxx
CommandOperationManager.cxx
CommandException.cxx
CommandUnitTest.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/CommandOperations
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
