#
# Name of Project
#
PROJECT (Xml)

#
# Use XML from Qt but not GUI.
#
SET(QT_USE_QTXML TRUE)
SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTNETWORK TRUE)

#
# QT include files
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

#
# Create a Library
#
ADD_LIBRARY(Xml
XmlWriter.h
XmlException.h
XmlAttributes.h
XmlSaxParser.h
XmlSaxParserException.h
XmlSaxParserHandlerInterface.h
XmlSaxParserWithQt.h
XmlUtilities.h

XmlWriter.cxx
XmlException.cxx
XmlAttributes.cxx
XmlSaxParser.cxx
XmlSaxParserException.cxx
XmlSaxParserWithQt.cxx
XmlUtilities.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

