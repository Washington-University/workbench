#
# Name of Project
#
PROJECT (Annotations)

#
# Use XML from Qt but not GUI.
#
SET(QT_DONT_USE_QTGUI TRUE)

#
# QT include files
#
###INCLUDE(${QT_USE_FILE})

#
# Create a Library
#
ADD_LIBRARY(Annotations
Annotation.h
AnnotationAlignHorizontalEnum.h
AnnotationAlignVerticalEnum.h
AnnotationCoordinateSpaceEnum.h
AnnotationDrawingTypeEnum.h
AnnotationText.h

Annotation.cxx
AnnotationAlignHorizontalEnum.cxx
AnnotationAlignVerticalEnum.cxx
AnnotationCoordinateSpaceEnum.cxx
AnnotationDrawingTypeEnum.cxx
AnnotationText.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
)

