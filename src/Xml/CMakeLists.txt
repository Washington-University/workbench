#
# Name of Project
#
PROJECT (Xml)

#
# QT include files
#
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Core5Compat_INCLUDE_DIRS})
    include_directories(${Qt6Network_INCLUDE_DIRS})
    include_directories(${Qt6Xml_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
endif()

#
# Create a Library
#
ADD_LIBRARY(Xml
GiftiException.h
GiftiMetaData.h
GiftiMetaDataElementDataTypeEnum.h
GiftiMetaDataValidatorInterface.h
GiftiMetaDataXmlElements.h
GiftiXmlElements.h

XmlAttributes.h
XmlException.h
XmlSaxParser.h
XmlSaxParserException.h
XmlSaxParserHandlerInterface.h
XmlSaxParserWithQt.h
XmlUnexpectedElementSaxParser.h
XmlUtilities.h
XmlWriter.h

GiftiException.cxx
GiftiMetaData.cxx
GiftiMetaDataElementDataTypeEnum.cxx
GiftiXmlElements.cxx

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

