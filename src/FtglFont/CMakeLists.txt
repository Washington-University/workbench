#
# Name of project
#
PROJECT (Ftgl)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Use OpenGL from QT
#
#SET(QT_USE_QTOPENGL TRUE)

#
# Add QT for includes
#
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()


#
# Need OpenGL
#
FIND_PACKAGE(OpenGL REQUIRED)
IF (OPENGL_FOUND)
   #
   # Need help finding includes on Apple
   #
   IF (APPLE) 
      # When searching for the include directory, find the location
      # for the OpenGL framework rather than an individual header file.
      FIND_PATH(OPENGL_INCLUDE_DIR OpenGL.framework
        /System/Library/Frameworks
        /Library/Frameworks
        ~/Library/Frameworks
      )
   ENDIF (APPLE)
   #
   # OpenGL Include Directory
   #
   INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
ELSE (OPENGL_FOUND)
   MESSAGE(FATAL_ERROR "OpenGL Libraries were not found")
ENDIF (OPENGL_FOUND)

#
# Create the brain library
#
ADD_LIBRARY(FtglFont
FTCharToGlyphIndexMap.h
FTContour.h
FTFace.h
FTFont/FTBitmapFontImpl.h
FTFont/FTBufferFontImpl.h
FTFont/FTExtrudeFontImpl.h
FTFont/FTFontImpl.h
FTFont/FTOutlineFontImpl.h
FTFont/FTPixmapFontImpl.h
FTFont/FTPolygonFontImpl.h
FTFont/FTTextureFontImpl.h
FTGL/FTBBox.h
FTGL/FTBitmapGlyph.h
FTGL/FTBuffer.h
FTGL/FTBufferFont.h
FTGL/FTBufferGlyph.h
FTGL/FTExtrdGlyph.h
FTGL/FTFont.h
FTGL/ftgl.h
FTGL/FTGLBitmapFont.h
FTGL/FTGLExtrdFont.h
FTGL/FTGLOutlineFont.h
FTGL/FTGLPixmapFont.h
FTGL/FTGLPolygonFont.h
FTGL/FTGLTextureFont.h
FTGL/FTGLGlyph.h
FTGL/FTLayout.h
FTGL/FTOutlineGlyph.h
FTGL/FTPixmapGlyph.h
FTGL/FTPoint.h
FTGL/FTPolyGlyph.h
FTGL/FTSimpleLayout.h
FTGL/FTTextureGlyph.h
FTGlyph/FTBitmapGlyphImpl.h
FTGlyph/FTBufferGlyphImpl.h
FTGlyph/FTExtrudeGlyphImpl.h
FTGlyph/FTGlyphImpl.h
FTGlyph/FTOutlineGlyphImpl.h
FTGlyph/FTPixmapGlyphImpl.h
FTGlyph/FTPolygonGlyphImpl.h
FTGlyph/FTTextureGlyphImpl.h
FTGlyphContainer.h
FTInternals.h
FTLayout/FTLayoutImpl.h
FTLayout/FTSimpleLayoutImpl.h
FTLibrary.h
FTList.h
FTSize.h
FTUnicode.h
FTVector.h
FTVectoriser.h

FtglConfig.h

FTBuffer.cpp
FTCharmap.cpp
FTContour.cpp
FTFace.cpp
FTFont/FTBitmapFont.cpp
FTFont/FTBufferFont.cpp
FTFont/FTExtrudeFont.cpp
FTFont/FTFont.cpp
FTFont/FTFontGlue.cpp
FTFont/FTOutlineFont.cpp
FTFont/FTPixmapFont.cpp
FTFont/FTPolygonFont.cpp
FTFont/FTTextureFont.cpp
FTGlyph/FTBitmapGlyph.cpp
FTGlyph/FTBufferGlyph.cpp
FTGlyph/FTExtrudeGlyph.cpp
FTGlyph/FTGlyph.cpp
FTGlyph/FTGlyphGlue.cpp
FTGlyph/FTOutlineGlyph.cpp
FTGlyph/FTPixmapGlyph.cpp
FTGlyph/FTPolygonGlyph.cpp
FTGlyph/FTTextureGlyph.cpp
FTGlyphContainer.cpp
FTLayout/FTLayout.cpp
FTLayout/FTLayoutGlue.cpp
FTLayout/FTSimpleLayout.cpp
FTLibrary.cpp
FTPoint.cpp
FTSize.cpp
FTVectoriser.cpp
) 

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${FREETYPE_INCLUDE_DIR_ft2build}
${FREETYPE_INCLUDE_DIR_freetype2}
${CMAKE_SOURCE_DIR}/FtglFont
${CMAKE_SOURCE_DIR}/FtglFont/FTFont
${CMAKE_SOURCE_DIR}/FtglFont/FTGL
${CMAKE_SOURCE_DIR}/FtglFont/FTGlyph
)

SET(FTGL_LIBRARIES FtglFont PARENT_SCOPE)
SET(FTGL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/FTGL PARENT_SCOPE)
