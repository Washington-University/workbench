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
AnnotationAlignmentEnum.h
AnnotationTextAlignHorizontalEnum.h
AnnotationTextAlignVerticalEnum.h
AnnotationCoordinateSpaceEnum.h
AnnotationFontNameEnum.h
AnnotationFontSizeEnum.h
AnnotationShapeTypeEnum.h
AnnotationText.h
AnnotationTextOrientationEnum.h
AnnotationTypeEnum.h

Annotation.cxx
AnnotationAlignmentEnum.cxx
AnnotationTextAlignHorizontalEnum.cxx
AnnotationTextAlignVerticalEnum.cxx
AnnotationCoordinateSpaceEnum.cxx
AnnotationFontNameEnum.cxx
AnnotationFontSizeEnum.cxx
AnnotationShapeTypeEnum.cxx
AnnotationText.cxx
AnnotationTextOrientationEnum.cxx
AnnotationTypeEnum.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
)

