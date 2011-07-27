
#
# Name of project
#
PROJECT (GuiQt)

#
# Use OpenGL from QT
#
SET(QT_USE_QTOPENGL TRUE)

#
# QT include files
#
INCLUDE(${QT_USE_FILE})

#
# Headers that must be processed with QT's "moc"
# Any class that derives from a QT class and contains
# the "Q_OBJECT" macro must be listed here.
#
SET(MOC_INPUT_HEADER_FILES
Actions.h
BrainOpenGLWidget.h
WindowMain.h
)

#
# Source files 
#
SET(SOURCE_FILES
GuiGlobals.h

${MOC_INPUT_HEADER_FILES}
Actions.cxx
BrainOpenGLWidget.cxx
GuiGlobals.cxx
WindowMain.cxx
)

#SET(FORMS
#    mainwindow.ui)

#
# Process the header files with moc producing moc_*.cpp files
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
#QT4_WRAP_UI(FORMS ${FORMS})
QT4_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})

#
# Create the GUI library
#
ADD_LIBRARY(GuiQt
${SOURCE_FILES}
${MOC_SOURCE_FILES}
${FORMS}
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/GuiQt
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
