#
# Name of project
#
PROJECT (Commands)

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
ADD_LIBRARY(Commands
CommandClassAddMember.h
CommandClassCreate.h
CommandClassCreateBase.h
CommandClassCreateEnum.h
CommandException.h
CommandGiftiConvert.h
CommandNiftiConvert.h
CommandNiftiInformation.h
CommandOperation.h
CommandOperationManager.h
CommandParser.h
CommandSurfaceInformation.h
CommandSurfaceMatch.h
CommandUnitTest.h

CommandClassAddMember.cxx
CommandClassCreate.cxx
CommandClassCreateBase.cxx
CommandClassCreateEnum.cxx
CommandException.cxx
CommandGiftiConvert.cxx
CommandNiftiConvert.cxx
CommandNiftiInformation.cxx
CommandOperation.cxx
CommandOperationManager.cxx
CommandParser.cxx
CommandSurfaceInformation.cxx
CommandSurfaceMatch.cxx
CommandUnitTest.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Commands
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
