
#
# Name of project
#
PROJECT (GuiQt)

#
# Use OpenGL from QT
#
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTWEBKIT TRUE)

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
CaretColorEnumSelectionControl.h
CaretFileDialog.h
ConnectivityLoaderControl.h
ConnectomeDBBrowserWindow.h
DisplayControlDialog.h
DisplayControlVolumeSurfaceOutlinePage.h
GuiManager.h
HyperLinkTextBrowser.h
ImageCaptureDialog.h
InformationDisplayWidget.h
ManageLoadedFilesDialog.h
MapScalarDataColorMappingEditorDialog.h
MultiPageDialog.h
MultiPageDialogPage.h
OverlaySelectionControl.h
OverlaySelectionControlLayer.h
PreferencesDialog.h
SpecFileDialog.h
StructureSelectionControl.h
StructureSurfaceSelectionControl.h
SurfaceSelectionControl.h
TimeSeriesManager.h
UserInputModeBordersWidget.h
WuQDataEntryDialog.h
WuQDialog.h
WuQDialogModal.h
WuQDialogNonModal.h
WuQDoubleSlider.h
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
BrainBrowserWindowScreenModeEnum.h
BrainBrowserWindowToolBar.h
BrainBrowserWindowToolBox.h
BrainOpenGLWidget.h
BrainOpenGLWidgetTextRenderer.h
CaretColorEnumSelectionControl.h
CaretFileDialog.h
ConnectivityLoaderControl.h
ConnectomeDBBrowserWindow.h
DisplayControlDialog.h
DisplayControlVolumeSurfaceOutlinePage.h
EventBrowserWindowContentGet.h
EventBrowserWindowNew.h
EventGetOrSetUserInputModeProcessor.h
EventGraphicsUpdateAllWindows.h
EventGraphicsUpdateOneWindow.h
EventMapScalarDataColorMappingEditor.h
EventUserInterfaceUpdate.h
GuiManager.h
HyperLinkTextBrowser.h
ImageCaptureDialog.h
InformationDisplayWidget.h
ManageLoadedFilesDialog.h
MapScalarDataColorMappingEditorDialog.h
MouseEvent.h
MouseEventTypeEnum.h
MultiPageDialog.h
MultiPageDialogPage.h
OverlaySelectionControl.h
OverlaySelectionControlLayer.h
PreferencesDialog.h
SpecFileDialog.h
StructureSelectionControl.h
StructureSurfaceSelectionControl.h
SurfaceSelectionControl.h
TimeSeriesManager.h
UserInputModeBorders.h
UserInputModeBordersWidget.h
UserInputModeView.h
UserInputReceiverInterface.h
ViewModeEnum.h
WuQDataEntryDialog.h
WuQDialog.h
WuQDialogModal.h
WuQDialogNonModal.h
WuQDoubleSlider.h
WuQFileDialog.h
WuQFileDialogIcons.h
WuQMessageBox.h
WuQSpinBoxGroup.h
WuQWidgetObjectGroup.h
WuQtUtilities.h


BrainBrowserWindow.cxx
BrainBrowserWindowScreenModeEnum.cxx
BrainBrowserWindowToolBar.cxx
BrainBrowserWindowToolBox.cxx
BrainOpenGLWidget.cxx
BrainOpenGLWidgetTextRenderer.cxx
CaretColorEnumSelectionControl.cxx
CaretFileDialog.cxx
ConnectivityLoaderControl.cxx
ConnectomeDBBrowserWindow.cxx
DisplayControlDialog.cxx
DisplayControlVolumeSurfaceOutlinePage.cxx
EventBrowserWindowContentGet.cxx
EventBrowserWindowNew.cxx
EventGetOrSetUserInputModeProcessor.cxx
EventGraphicsUpdateAllWindows.cxx
EventGraphicsUpdateOneWindow.cxx
EventMapScalarDataColorMappingEditor.cxx
EventUserInterfaceUpdate.cxx
GuiManager.cxx
HyperLinkTextBrowser.cxx
ImageCaptureDialog.cxx
InformationDisplayWidget.cxx
ManageLoadedFilesDialog.cxx
MapScalarDataColorMappingEditorDialog.cxx
MouseEvent.cxx
MouseEventTypeEnum.cxx
MultiPageDialog.cxx
MultiPageDialogPage.cxx
OverlaySelectionControl.cxx
OverlaySelectionControlLayer.cxx
PreferencesDialog.cxx
SpecFileDialog.cxx
StructureSelectionControl.cxx
StructureSurfaceSelectionControl.cxx
SurfaceSelectionControl.cxx
TimeSeriesManager.cxx
UserInputModeBorders.cxx
UserInputModeBordersWidget.cxx
UserInputModeView.cxx
ViewModeEnum.cxx
WuQDataEntryDialog.cxx
WuQDialog.cxx
WuQDialogModal.cxx
WuQDialogNonModal.cxx
WuQDoubleSlider.cxx
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
${CMAKE_SOURCE_DIR}/Qwt
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
