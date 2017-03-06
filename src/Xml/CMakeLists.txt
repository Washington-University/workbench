#
# Name of Project
#
PROJECT (Xml)

#
# QT include files
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    SET(QT_USE_QTXML TRUE)
    SET(QT_DONT_USE_QTGUI TRUE)
    SET(QT_USE_QTNETWORK TRUE)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

#
# Create a Library
#
ADD_LIBRARY(Xml
XmlAttributes.h
XmlException.h
XmlSaxParser.h
XmlSaxParserException.h
XmlSaxParserHandlerInterface.h
XmlSaxParserWithQt.h
XmlUnexpectedElementSaxParser.h
XmlUtilities.h
XmlWriter.h

XmlAttributes.cxx
XmlException.cxx
XmlSaxParser.cxx
XmlSaxParserException.cxx
XmlSaxParserWithQt.cxx
XmlUnexpectedElementSaxParser.cxx
XmlUtilities.cxx
XmlWriter.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

TARGET_LINK_LIBRARIES(Xml ${CARET_QT5_LINK})

