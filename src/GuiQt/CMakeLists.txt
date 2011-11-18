
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
# Also LIST each of these headers in the 
# source files section so that they show
# up in development tools such as XCode.
#
SET(MOC_INPUT_HEADER_FILES
BrainBrowserWindow.h
BrainBrowserWindowToolBar.h
BrainBrowserWindowToolBox.h
BrainOpenGLWidget.h
ConnectivityLoaderControl.h
GuiManager.h
HyperLinkTextBrowser.h
ImageCaptureDialog.h
ManageLoadedFilesDialog.h
OverlaySelectionControl.h
OverlaySelectionControlLayer.h
PaletteColorMappingEditorDialog.h
PreferencesDialog.h
SpecFileDialog.h
StructureSurfaceSelectionControl.h
TimeSeriesManager.h
WuQDialog.h
WuQDialogModal.h
WuQDialogNonModal.h
WuQFileDialog.h
WuQMessageBox.h
WuQSpinBoxGroup.h
WuQWidgetObjectGroup.h
)

#
# Header files
# 
# If the header file is qt 'moc' file
# also list it in the section above
# named MOC_INPUT_HEADER_FILES
#
SET(SOURCE_FILES
BrainBrowserWindow.h
BrainBrowserWindowToolBar.h
BrainBrowserWindowToolBox.h
BrainOpenGLWidget.h
ConnectivityLoaderControl.h
EventBrowserWindowContentGet.h
EventBrowserWindowNew.h
EventGraphicsUpdateAllWindows.h
EventGraphicsUpdateOneWindow.h
EventPaletteColorMappingEditor.h
EventUserInterfaceUpdate.h
GuiManager.h
HyperLinkTextBrowser.h
ImageCaptureDialog.h
ManageLoadedFilesDialog.h
MouseEvent.h
MouseEventTypeEnum.h
OverlaySelectionControl.h
OverlaySelectionControlLayer.h
PaletteColorMappingEditorDialog.h
PreferencesDialog.h
SpecFileDialog.h
StructureSurfaceSelectionControl.h
TimeSeriesManager.h
UserInputModeView.h
UserInputReceiverInterface.h
ViewModeEnum.h
WuQDialog.h
WuQDialogModal.h
WuQDialogNonModal.h
WuQFileDialog.h
WuQFileDialogIcons.h
WuQMessageBox.h
WuQSpinBoxGroup.h
WuQWidgetObjectGroup.h
WuQtUtilities.h


BrainBrowserWindow.cxx
BrainBrowserWindowToolBar.cxx
BrainBrowserWindowToolBox.cxx
BrainOpenGLWidget.cxx
ConnectivityLoaderControl.cxx
EventBrowserWindowContentGet.cxx
EventBrowserWindowNew.cxx
EventGraphicsUpdateAllWindows.cxx
EventGraphicsUpdateOneWindow.cxx
EventPaletteColorMappingEditor.cxx
EventUserInterfaceUpdate.cxx
GuiManager.cxx
HyperLinkTextBrowser.cxx
ImageCaptureDialog.cxx
ManageLoadedFilesDialog.cxx
MouseEvent.cxx
MouseEventTypeEnum.cxx
OverlaySelectionControl.cxx
OverlaySelectionControlLayer.cxx
PaletteColorMappingEditorDialog.cxx
PreferencesDialog.cxx
SpecFileDialog.cxx
StructureSurfaceSelectionControl.cxx
TimeSeriesManager.cxx
UserInputModeView.cxx
ViewModeEnum.cxx
WuQDialog.cxx
WuQDialogModal.cxx
WuQDialogNonModal.cxx
WuQFileDialog.cxx
WuQMessageBox.cxx
WuQSpinBoxGroup.cxx
WuQWidgetObjectGroup.cxx
WuQtUtilities.cxx
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
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
