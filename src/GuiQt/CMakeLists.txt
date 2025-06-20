
#
# Name of project
#
PROJECT (GuiQt)

#
# QT include files
#
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Core5Compat_INCLUDE_DIRS})
    include_directories(${Qt6OpenGL_INCLUDE_DIRS})
    include_directories(${Qt6Network_INCLUDE_DIRS})
    include_directories(${Qt6OpenGLWidgets_INCLUDE_DIRS})
    include_directories(${Qt6Widgets_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
#    include_directories(${Qt5Concurrent_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    if(HAVE_QT_WEBKIT)
        include_directories(${Qt5WebEngine_INCLUDE_DIRS})
        include_directories(${Qt5WebEngineCore_INCLUDE_DIRS})
        include_directories(${Qt5WebEngineWidgets_INCLUDE_DIRS})
        include_directories(${Qt5WebView_INCLUDE_DIRS})
    endif(HAVE_QT_WEBKIT)
endif()

SET (MESA_OR_OPENGL_LIBRARIES "")
IF (OSMESA_FOUND)
    SET(MESA_OR_OPENGL_LIBRARIES
        ${OSMESA_OFFSCREEN_LIBRARY}
        ${OSMESA_GL_LIBRARY}
        ${OSMESA_GLU_LIBRARY})
ELSE()
    IF (OPENGL_FOUND)
        SET(MESA_OR_OPENGL_LIBRARIES ${OPENGL_LIBRARIES})
    ELSE()
        MESSAGE(SEND_ERROR "Must have either OpenGL or Mesa3D for command line")
    ENDIF()
ENDIF()

#
# Let CMake find files for moc and uic.
#

#
# With AUTOMOC, do not need to specify files
# that contain Q_OBJECT macro for Qt to process with 'moc'
# (meta-object compiler).
#
IF(WORKBENCH_USE_CMAKE_AUTOMOC)
    SET(CMAKE_AUTOMOC ON)
    SET(CMAKE_AUTOUIC ON)
ELSE()
    SET(MOC_INPUT_HEADER_FILES
        AboutWorkbenchDialog.h
	AnnotationBackgroundTypeWidget.h
	AnnotationBoundsWidget.h
        AnnotationColorWidget.h
	AnnotationCoordinateCenterXYWidget.h
        AnnotationCoordinateSpaceWidget.h
        AnnotationCoordinatesWidget.h
        AnnotationCreateDialog.h
        AnnotationDeleteWidget.h
        AnnotationFontWidget.h
        AnnotationFormatWidget.h
        AnnotationInsertNewWidget.h
        AnnotationLineArrowTipsWidget.h
        AnnotationMenuArrange.h
        AnnotationMenuFileSelection.h
	AnnotationNameWidget.h
        AnnotationPasteDialog.h
	AnnotationPolyTypeDrawEditWidget.h
        AnnotationRedoUndoWidget.h
        AnnotationRotationWidget.h
	AnnotationSamplesInsertNewWidget.h
	AnnotationSamplesMetaDataDialog.h
	AnnotationSamplesModifyWidget.h
        AnnotationSelectionViewController.h
        AnnotationTextAlignmentWidget.h
        AnnotationTextEditorDialog.h
        AnnotationTextEditorWidget.h
        AnnotationTextOrientationWidget.h
	AnnotationTextSubstitutionLayerViewController.h
        AnnotationTextSubstitutionLayerSetViewController.h
        AnnotationWidthHeightWidget.h
        BalsaDatabaseUploadSceneFileDialog.h
        BalsaStudySelectionDialog.h
        BorderEditingSelectionDialog.h
        BorderFileSplitDialog.h
        BorderOptimizeDialog.h
        BorderPropertiesEditorDialog.h
        BorderSelectionViewController.h
        BrainBrowserWindow.h
        BrainBrowserWindowComboBox.h
        BrainBrowserWindowOrientedToolBox.h
        BrainBrowserWindowToolBar.h
        BrainBrowserWindowToolBarAllSurface.h
        BrainBrowserWindowToolBarChartAttributes.h
        BrainBrowserWindowToolBarChartAxes.h
        BrainBrowserWindowToolBarChartTwoAttributes.h
        BrainBrowserWindowToolBarChartTwoOrientation.h
        BrainBrowserWindowToolBarChartTwoOrientedAxes.h
        BrainBrowserWindowToolBarChartTwoType.h
        BrainBrowserWindowToolBarChartType.h
        BrainBrowserWindowToolBarComponent.h
	BrainBrowserWindowToolBarHistology.h
	BrainBrowserWindowToolBarImage.h
	BrainBrowserWindowToolBarOrientation.h
        BrainBrowserWindowToolBarSlicePlane.h
        BrainBrowserWindowToolBarSliceSelection.h
	BrainBrowserWindowToolBarSurface.h
        BrainBrowserWindowToolBarSurfaceMontage.h
        BrainBrowserWindowToolBarTab.h
        BrainBrowserWindowToolBarTabPopUpMenu.h
	BrainBrowserWindowToolBarView.h
        BrainBrowserWindowToolBarVolumeMPR.h
        BrainBrowserWindowToolBarVolumeMontage.h
        BrainOpenGLWidget.h
        BugReportDialog.h
	CardinalDirectionEnumMenu.h
        CaretColorEnumComboBox.h
        CaretColorEnumMenu.h
	CaretColorToolButton.h
        CaretDataFileSelectionComboBox.h
        CaretFileDialog.h
        CaretFileDialogExtendable.h
        CaretFileRemoteDialog.h
	CaretFiveFileImportDialog.h
        CaretMappableDataFileAndMapSelector.h
        CaretMappableDataFileAndMapSelectorObject.h
        ChartHistoryViewController.h
        ChartLinesSelectionViewController.h
        ChartMatrixParcelSelectionViewController.h
        ChartMatrixSeriesSelectionViewController.h
        ChartTwoAxisPropertiesEditorWidget.h
	ChartTwoCartesianCustomSubdivisionsEditorWidget.h
	ChartTwoLineLayerNormalizationWidget.h
        ChartTwoOverlaySetViewController.h
        ChartTwoOverlayViewController.h
	ChartTwoTitleEditorWidget.h
        ChartSelectionViewController.h
        ChartToolBoxViewController.h
	ChooseBorderFocusFromFileDialog.h
        CiftiConnectivityMatrixViewController.h
        CiftiParcelSelectionComboBox.h
	ClippingPlanesWidget.h
        ColorEditorWidget.h
	ConnectivityCorrelationSettingsMenu.h
        CopyPaletteColorMappingToFilesDialog.h
        CustomViewDialog.h
	CziImageExportDialog.h
        DataFileContentCopyMoveDialog.h
	DataFileEditorDialog.h
	DingOntologyTermsDialog.h
        DisplayGroupAndTabItemViewController.h
        DisplayGroupEnumComboBox.h
	DynConnViewController.h
        EnumComboBoxTemplate.h
	ExitProgramModifiedFilesDialog.h
        FiberOrientationSelectionViewController.h
        FiberSamplesOpenGLWidget.h
        FociProjectionDialog.h
        FociPropertiesEditorDialog.h
        FociSelectionViewController.h
        GapsAndMarginsDialog.h
        GiftiLabelTableEditor.h
        GiftiLabelTableSelectionComboBox.h
        GroupAndNameHierarchyViewController.h
        GuiManager.h
        HelpViewerDialog.h
	HistologyOverlaySetViewController.h
	HistologyOverlaySettingsMenu.h
	HistologyOverlayViewController.h
        HyperLinkTextBrowser.h
	IdentificationDisplayDialog.h
	IdentificationDisplayWidget.h
	IdentificationFileFilteringTableWidget.h
        IdentifyBrainordinateDialog.h
        ImageCaptureDialog.h
        ImageFileConvertToVolumeFileDialog.h
        ImageSelectionViewController.h
        InformationDisplayDialog.h
        InformationDisplayPropertiesDialog.h
	LabelSelectionDialog.h
	LabelSelectionWidget.h
        LabelSelectionViewController.h
	LabelSelectionViewHierarchyController.h
	LabelSelectionViewWidget.h
    	LockAspectWarningDialog.h
        MacApplication.h
        MacDockMenu.h
        MapSettingsChartTwoLineHistoryWidget.h
        MapSettingsColorBarPaletteOptionsWidget.h
        MapSettingsColorBarWidget.h
        MapSettingsFiberTrajectoryWidget.h
        MapSettingsLabelsWidget.h
        MapSettingsLayerWidget.h
	MapSettingsModulateWidget.h
        MapSettingsPaletteColorMappingWidget.h
        MapSettingsParcelsWidget.h
        MapYokingGroupComboBox.h
	MediaOverlaySettingsMenu.h
	MediaOverlayViewController.h
	MediaOverlaySetViewController.h
        MetaDataEditorDialog.h
        MetaDataEditorWidget.h
	MovieRecordingDialog.h
        OffScreenOpenGLRenderer.h
	OpenFileQuicklyDialog.h
        OverlaySetViewController.h
        OverlaySettingsEditorDialog.h
        OverlayViewController.h
        PaletteColorMappingEditorDialog.h
	PaletteCreateNewDialog.h
	PaletteEditorDialog.h
	PaletteEditorRangeRow.h
	PaletteEditorRangeWidget.h
	PaletteSelectionWidget.h
        PlotMagnifier.h
        PlotPanner.h
	PreferencesDevelopOptionsWidget.h
        PreferencesDialog.h
	PreferencesDisplayWidget.h
        PreferencesImageWidget.h
	PreferencesRecentFilesWidget.h
        ProgressReportingDialog.h
        ProgressReportingFromEvent.h
        ProgressReportingWithSlots.h
	RecentFilesDialog.h
	RecentFilesTableWidget.h
	RecentSceneMenu.h
        RegionOfInterestCreateFromBorderDialog.h
	SamplesSelectionViewController.h
	ScaleBarWidget.h
        SceneBasePathWidget.h
	SceneClassInfoWidget.h
        SceneCreateReplaceDialog.h
        SceneDialog.h
        SceneFileInformationDialog.h
        SceneDataFileTreeItemModel.h
        ScenePreviewDialog.h
        SceneReplaceAllDialog.h
	SceneSelectionWidget.h
        SceneShowOptionsDialog.h
        SpecFileManagementDialog.h
        StructureEnumComboBox.h
        StructureSurfaceSelectionControl.h
        SurfacePropertiesEditorDialog.h
        SurfaceSelectionViewController.h
        ThresholdingSetMapsDialog.h
	TileTabGridRowColumnWidgets.h
        TileTabsConfigurationDialog.h
	TileTabsLayoutConfigurationTypeWidget.h
	TileTabsManualTabGeometryWidget.h
	UnicodeCharacterEntryDialog.h
        UserInputModeAnnotationsContextMenu.h
        UserInputModeAnnotationsWidget.h
        UserInputModeBordersWidget.h
        UserInputModeFociWidget.h
        UserInputModeImageWidget.h
	UserInputModeTileTabsLayoutContextMenu.h
        UserInputModeViewContextMenu.h
	UserInputModeViewContextTileTabsSubMenu.h
        UserInputModeVolumeEditWidget.h
        UsernamePasswordWidget.h
        VolumeFileCreateDialog.h
	VolumeFileResampleDialog.h
	VolumeMprSettingsWidget.h
	VolumeObliqueOptionsWidget.h
	VolumePropertiesEditorDialog.h
        VolumeSurfaceOutlineColorOrTabViewController.h
        VolumeSurfaceOutlineSetViewController.h
        VolumeSurfaceOutlineViewController.h
	WbMacroHelper.h
	WbMacroWidgetActionsManager.h
	WorkbenchInstallationAssistantDialog.h
        WuQCollapsibleWidget.h
	WuQColorEditorWidget.h
        WuQDataEntryDialog.h
        WuQDialog.h
        WuQDialogModal.h
        WuQDialogNonModal.h
        WuQDoubleSlider.h
        WuQDoubleSpinBox.h
        WuQEventBlockingFilter.h
        WuQGridLayoutGroup.h
        WuQGroupBoxExclusiveWidget.h
	WuQHyperlinkToolTip.h
        WuQImageLabel.h
        WuQListWidget.h
	WuQMacroCommandParameterWidget.h
	WuQMacroCopyDialog.h
        WuQMacroCreateDialog.h
        WuQMacroDialog.h
        WuQMacroExecutor.h
        WuQMacroExecutorMonitor.h
        WuQMacroHelperInterface.h
        WuQMacroManager.h
        WuQMacroMenu.h
	WuQMacroNewCommandSelectionDialog.h
        WuQMacroShortCutKeyComboBox.h
        WuQMacroSignalEmitter.h
        WuQMacroSignalWatcher.h
        WuQMacroWidgetAction.h
        WuQMessageBox.h
	WuQMessageBoxTwo.h
        WuQwtPlot.h
	WuQScrollArea.h
        WuQSpecialIncrementDoubleSpinBox.h
	WuQSpinBox.h
        WuQSpinBoxGroup.h
        WuQSpinBoxOddValue.h
        WuQTabBar.h
        WuQTabWidget.h
        WuQTabWidgetWithSizeHint.h
	WuQTextEditWithToolBarWidget.h
        WuQTextEditorDialog.h
        WuQTimedMessageDisplay.h
	WuQToolTipHelper.h
        WuQTreeWidget.h
        WuQTrueFalseComboBox.h
	WuQValueChangedSignalWatcher.h
        #WuQWebView.h
        WuQWidget.h
        WuQWidgetObjectGroup.h
        ZipSceneFileDialog.h
    )

    IF(Qt6_FOUND)
        QT6_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
    IF(Qt5_FOUND)
        QT5_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
ENDIF()

#
# Header files
#
SET(SOURCE_FILES
AboutWorkbenchDialog.h
AnnotationBackgroundTypeWidget.h
AnnotationBoundsWidget.h
AnnotationColorWidget.h
AnnotationCoordinateCenterXYWidget.h
AnnotationCoordinateInformation.h
AnnotationCoordinateSpaceWidget.h
AnnotationCoordinatesWidget.h
AnnotationCreateDialog.h
AnnotationDeleteWidget.h
AnnotationFontWidget.h
AnnotationFormatWidget.h
AnnotationInsertNewWidget.h
AnnotationLineArrowTipsWidget.h
AnnotationMenuArrange.h
AnnotationMenuFileSelection.h
AnnotationNameWidget.h
AnnotationPasteDialog.h
AnnotationPastingInformation.h
AnnotationPolyTypeDrawEditWidget.h
AnnotationRedoUndoWidget.h
AnnotationRotationWidget.h
AnnotationSamplesInsertNewWidget.h
AnnotationSamplesMetaDataDialog.h
AnnotationSamplesModifyWidget.h
AnnotationSelectionViewController.h
AnnotationTextAlignmentWidget.h
AnnotationTextEditorDialog.h
AnnotationTextEditorWidget.h
AnnotationTextOrientationWidget.h
AnnotationTextSubstitutionLayerViewController.h
AnnotationTextSubstitutionLayerSetViewController.h
AnnotationWidgetParentEnum.h
AnnotationWidthHeightWidget.h
BalsaDatabaseManager.h
BalsaDatabaseUploadSceneFileDialog.h
BalsaStudySelectionDialog.h
BalsaStudyInformation.h
BalsaUserRoles.h
BorderEditingSelectionDialog.h
BorderFileSplitDialog.h
BorderOptimizeDialog.h
BorderOptimizeExecutor.h
BorderPropertiesEditorDialog.h
BorderSelectionViewController.h
BrainBrowserWindow.h
BrainBrowserWindowComboBox.h
BrainBrowserWindowEditMenuItemEnum.h
BrainBrowserWindowOrientedToolBox.h
BrainBrowserWindowToolBar.h
BrainBrowserWindowToolBarAllSurface.h
BrainBrowserWindowToolBarChartAttributes.h
BrainBrowserWindowToolBarChartAxes.h
BrainBrowserWindowToolBarChartTwoAttributes.h
BrainBrowserWindowToolBarChartTwoOrientation.h
BrainBrowserWindowToolBarChartTwoOrientedAxes.h
BrainBrowserWindowToolBarChartTwoType.h
BrainBrowserWindowToolBarChartType.h
BrainBrowserWindowToolBarComponent.h
BrainBrowserWindowToolBarHistology.h
BrainBrowserWindowToolBarImage.h
BrainBrowserWindowToolBarOrientation.h
BrainBrowserWindowToolBarSlicePlane.h
BrainBrowserWindowToolBarSliceSelection.h
BrainBrowserWindowToolBarSurface.h
BrainBrowserWindowToolBarSurfaceMontage.h
BrainBrowserWindowToolBarTab.h
BrainBrowserWindowToolBarTabPopUpMenu.h
BrainBrowserWindowToolBarView.h
BrainBrowserWindowToolBarVolumeMPR.h
BrainBrowserWindowToolBarVolumeMontage.h
BrainOpenGLWidget.h
BugReportDialog.h
CardinalDirectionEnumMenu.h
CaretColorEnumComboBox.h
CaretColorEnumMenu.h
CaretColorToolButton.h
CaretDataFileSelectionComboBox.h
CaretFileDialog.h
CaretFileDialogExtendable.h
CaretFileRemoteDialog.h
CaretFiveFileImportDialog.h
CaretMappableDataFileAndMapSelector.h
CaretMappableDataFileAndMapSelectorObject.h
CaretResultDialog.h
ChartHistoryViewController.h
ChartLinesSelectionViewController.h
ChartMatrixParcelSelectionViewController.h
ChartMatrixSeriesSelectionViewController.h
ChartTwoAxisPropertiesEditorDialog.h
ChartTwoAxisPropertiesEditorWidget.h
ChartTwoCartesianCustomSubdivisionsEditorWidget.h
ChartTwoLineLayerNormalizationWidget.h
ChartTwoOverlaySetViewController.h
ChartTwoOverlayViewController.h
ChartTwoTitleEditorWidget.h
ChartSelectionViewController.h
ChartToolBoxViewController.h
ChooseBorderFocusFromFileDialog.h
CiftiConnectivityMatrixViewController.h
CiftiParcelSelectionComboBox.h
ClippingPlanesWidget.h
ColorEditorWidget.h
ConnectivityCorrelationSettingsMenu.h
ContextSensitiveMenuItemsEnum.h
CopyPaletteColorMappingToFilesDialog.h
CursorDisplayScoped.h
CursorEnum.h
CursorManager.h
CustomViewDialog.h
CziImageExportDialog.h
DataFileContentCopyMoveDialog.h
DataFileEditorDialog.h
DingOntologyTermsDialog.h
DisplayGroupAndTabItemTreeWidgetItem.h
DisplayGroupAndTabItemViewController.h
DisplayGroupEnumComboBox.h
DynConnViewController.h
EnumComboBoxTemplate.h
EventAnnotationCreateNewType.h
EventAnnotationDrawingFinishCancel.h
EventAnnotationGetDrawnInWindow.h
EventAnnotationGetSelectedInsertNewFile.h
EventBrowserTabCloseInToolBar.h
EventBrowserTabDeleteInToolBar.h
EventBrowserTabNewInGUI.h
EventBrowserWindowDrawingContent.h
EventBrowserWindowCreateTabs.h
EventBrowserWindowGraphicsRedrawn.h
EventBrowserWindowNew.h
EventBrowserWindowTileTabOperation.h
EventGetOrSetUserInputModeProcessor.h
EventGraphicsPaintNowAllWindows.h
EventGraphicsPaintNowOneWindow.h
EventGraphicsTimingOneWindow.h
EventGraphicsPaintSoonAllWindows.h
EventGraphicsPaintSoonOneWindow.h
EventGraphicsWindowShowToolTip.h
EventHelpViewerDisplay.h
EventIdentificationRequest.h
EventMovieManualModeRecording.h
EventOperatingSystemRequestOpenDataFile.h
EventOverlaySettingsEditorDialogRequest.h
EventPaletteColorMappingEditorDialogRequest.h
EventShowDataFileReadWarningsDialog.h
EventUpdateInformationWindows.h
EventUpdateVolumeEditingToolBar.h
EventUpdateYokedWindows.h
EventUserInterfaceUpdate.h
ExitProgramModifiedFilesDialog.h
FiberOrientationSelectionViewController.h
FiberSamplesOpenGLWidget.h
FociProjectionDialog.h
FociPropertiesEditorDialog.h
FociSelectionViewController.h
GapsAndMarginsDialog.h
GestureEvent.h
GiftiLabelTableEditor.h
GiftiLabelTableSelectionComboBox.h
GroupAndNameHierarchyTreeWidgetItem.h
GroupAndNameHierarchyViewController.h
GuiManager.h
HelpViewerDialog.h
HistologyOverlaySetViewController.h
HistologyOverlaySettingsMenu.h
HistologyOverlayViewController.h
HyperLinkTextBrowser.h
IdentificationDisplayDialog.h
IdentificationDisplayWidget.h
IdentificationFileFilteringTableWidget.h
IdentifyBrainordinateDialog.h
ImageCaptureDialog.h
ImageFileConvertToVolumeFileDialog.h
ImageSelectionViewController.h
InformationDisplayDialog.h
InformationDisplayPropertiesDialog.h
KeyEvent.h
LabelSelectionDialog.h
LabelSelectionWidget.h
LabelSelectionViewController.h
LabelSelectionViewHierarchyController.h
LabelSelectionViewWidget.h
LockAspectWarningDialog.h
MacApplication.h
MacDockMenu.h
MapSettingsChartTwoLineHistoryWidget.h
MapSettingsColorBarPaletteOptionsWidget.h
MapSettingsColorBarWidget.h
MapSettingsFiberTrajectoryWidget.h
MapSettingsLabelsWidget.h
MapSettingsLayerWidget.h
MapSettingsModulateWidget.h
MapSettingsPaletteColorMappingWidget.h
MapSettingsParcelsWidget.h
MapYokingGroupComboBox.h
MediaOverlaySettingsMenu.h
MediaOverlayViewController.h
MediaOverlaySetViewController.h
MetaDataEditorDialog.h
MetaDataEditorWidget.h
MovieRecordingDialog.h
OffScreenOpenGLRenderer.h
OpenFileQuicklyDialog.h
OverlaySetViewController.h
OverlaySettingsEditorDialog.h
OverlayViewController.h
PaletteColorMappingEditorDialog.h
PaletteCreateNewDialog.h
PaletteEditorDialog.h
PaletteEditorRangeRow.h
PaletteEditorRangeWidget.h
PalettePixmapPainter.h
PaletteSelectionWidget.h
PlotMagnifier.h
PlotPanner.h
PreferencesDevelopOptionsWidget.h
PreferencesDialog.h
PreferencesDisplayWidget.h
PreferencesImageWidget.h
PreferencesRecentFilesWidget.h
ProgressReportingDialog.h
ProgressReportingFromEvent.h
ProgressReportingWithSlots.h
QGLWidgetTextRenderer.h
RecentFilesDialog.h
RecentFilesTableWidget.h
RecentSceneMenu.h
RegionOfInterestCreateFromBorderDialog.h
SamplesSelectionViewController.h
ScaleBarWidget.h
SceneBasePathWidget.h
SceneClassInfoWidget.h
SceneCreateReplaceDialog.h
SceneDialog.h
SceneFileInformationDialog.h
SceneDataFileTreeItem.h
SceneDataFileTreeItemModel.h
ScenePreviewDialog.h
SceneReplaceAllDialog.h
SceneSelectionWidget.h
SceneShowOptionsDialog.h
SceneWindowGeometry.h
SpecFileManagementDialog.h
StructureEnumComboBox.h
StructureSurfaceSelectionControl.h
SurfacePropertiesEditorDialog.h
SurfaceSelectionViewController.h
ThresholdingSetMapsDialog.h
TileTabGridRowColumnWidgets.h
TileTabsConfigurationDialog.h
TileTabsGridConfigurationModifier.h
TileTabsLayoutConfigurationTypeWidget.h
TileTabsManualTabGeometryWidget.h
UnicodeCharacterEntryDialog.h
UserInputModeAbstract.h
UserInputModeAnnotations.h
UserInputModeAnnotationsContextMenu.h
UserInputModeAnnotationsWidget.h
UserInputModeBorders.h
UserInputModeBordersWidget.h
UserInputModeFociWidget.h
UserInputModeFoci.h
UserInputModeImageWidget.h
UserInputModeImage.h
UserInputModeSamplesEdit.h
UserInputModeTileTabsLayout.h
UserInputModeTileTabsLayoutContextMenu.h
UserInputModeView.h
UserInputModeViewContextMenu.h
UserInputModeViewContextTileTabsSubMenu.h
UserInputModeVolumeEdit.h
UserInputModeVolumeEditWidget.h
UsernamePasswordWidget.h
ViewModeEnum.h
VolumeFileCreateDialog.h
VolumeFileResampleDialog.h
VolumeMprSettingsWidget.h
VolumeObliqueOptionsWidget.h
VolumePropertiesEditorDialog.h
VolumeSurfaceOutlineColorOrTabViewController.h
VolumeSurfaceOutlineSetViewController.h
VolumeSurfaceOutlineViewController.h
WbMacroCustomDataInfo.h
WbMacroCustomDataTypeEnum.h
WbMacroCustomOperationAnimateOverlayCrossFade.h
WbMacroCustomOperationAnimateRotation.h
WbMacroCustomOperationAnimateSurfaceInterpolation.h
WbMacroCustomOperationAnimateVolumeSliceSequence.h
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade.h
WbMacroCustomOperationBase.h
WbMacroCustomOperationDelay.h
WbMacroCustomOperationIncrementRotation.h
WbMacroCustomOperationIncrementVolumeSlice.h
WbMacroCustomOperationManager.h
WbMacroCustomOperationSurfaceDefaultColor.h
WbMacroCustomOperationTypeEnum.h
WbMacroHelper.h
WbMacroWidgetActionNames.h
WbMacroWidgetActionsManager.h
WorkbenchInstallationAssistantDialog.h
WuQCollapsibleWidget.h
WuQColorEditorWidget.h
WuQDataEntryDialog.h
WuQDialog.h
WuQDialogModal.h
WuQDialogNonModal.h
WuQDoubleSlider.h
WuQDoubleSpinBox.h
WuQEventBlockingFilter.h
WuQFactory.h
WuQGridLayoutGroup.h
WuQGroupBoxExclusiveWidget.h
WuQHyperlinkToolTip.h
WuQImageLabel.h
WuQListWidget.h
WuQMacroCommandParameterWidget.h
WuQMacroCopyDialog.h
WuQMacroCreateDialog.h
WuQMacroCustomOperationManagerInterface.h
WuQMacroDialog.h
WuQMacroExecutor.h
WuQMacroExecutorMonitor.h
WuQMacroExecutorOptions.h
WuQMacroHelperInterface.h
WuQMacroManager.h
WuQMacroMenu.h
WuQMacroMouseEventWidgetInterface.h
WuQMacroNewCommandSelectionDialog.h
WuQMacroShortCutKeyComboBox.h
WuQMacroSignalEmitter.h
WuQMacroSignalWatcher.h
WuQMacroWidgetAction.h
WuQMessageBox.h
WuQMessageBoxTwo.h
WuQwtPlot.h
WuQScrollArea.h
WuQSpecialIncrementDoubleSpinBox.h
WuQSpinBox.h
WuQSpinBoxGroup.h
WuQSpinBoxOddValue.h
WuQTabBar.h
WuQTabWidget.h
WuQTabWidgetWithSizeHint.h
WuQTextEditWithToolBarWidget.h
WuQTextEditorDialog.h
WuQTimedMessageDisplay.h
WuQToolTipHelper.h
WuQTreeWidget.h
WuQTrueFalseComboBox.h
WuQValueChangedSignalWatcher.h
#WuQWebView.h
WuQWidget.h
WuQWidgetDisabler.h
WuQWidgetObjectGroup.h
WuQtUtilities.h
ZipSceneFileDialog.h

AboutWorkbenchDialog.cxx
AnnotationBackgroundTypeWidget.cxx
AnnotationBoundsWidget.cxx
AnnotationColorWidget.cxx
AnnotationCoordinateCenterXYWidget.cxx
AnnotationCoordinateInformation.cxx
AnnotationCoordinateSpaceWidget.cxx
AnnotationCoordinatesWidget.cxx
AnnotationCreateDialog.cxx
AnnotationDeleteWidget.cxx
AnnotationFontWidget.cxx
AnnotationFormatWidget.cxx
AnnotationInsertNewWidget.cxx
AnnotationLineArrowTipsWidget.cxx
AnnotationMenuArrange.cxx
AnnotationMenuFileSelection.cxx
AnnotationNameWidget.cxx
AnnotationPasteDialog.cxx
AnnotationPastingInformation.cxx
AnnotationPolyTypeDrawEditWidget.cxx
AnnotationRedoUndoWidget.cxx
AnnotationRotationWidget.cxx
AnnotationSamplesInsertNewWidget.cxx
AnnotationSamplesMetaDataDialog.cxx
AnnotationSamplesModifyWidget.cxx
AnnotationSelectionViewController.cxx
AnnotationTextAlignmentWidget.cxx
AnnotationTextEditorDialog.cxx
AnnotationTextEditorWidget.cxx
AnnotationTextOrientationWidget.cxx
AnnotationTextSubstitutionLayerSetViewController.cxx
AnnotationTextSubstitutionLayerViewController.cxx
AnnotationWidgetParentEnum.cxx
AnnotationWidthHeightWidget.cxx
BalsaDatabaseManager.cxx
BalsaDatabaseUploadSceneFileDialog.cxx
BalsaStudyInformation.cxx
BalsaUserRoles.cxx
BalsaStudySelectionDialog.cxx
BorderEditingSelectionDialog.cxx
BorderFileSplitDialog.cxx
BorderOptimizeDialog.cxx
BorderOptimizeExecutor.cxx
BorderPropertiesEditorDialog.cxx
BorderSelectionViewController.cxx
BrainBrowserWindow.cxx
BrainBrowserWindowComboBox.cxx
BrainBrowserWindowEditMenuItemEnum.cxx
BrainBrowserWindowOrientedToolBox.cxx
BrainBrowserWindowToolBar.cxx
BrainBrowserWindowToolBarAllSurface.cxx
BrainBrowserWindowToolBarChartAttributes.cxx
BrainBrowserWindowToolBarChartAxes.cxx
BrainBrowserWindowToolBarChartTwoAttributes.cxx
BrainBrowserWindowToolBarChartTwoOrientation.cxx
BrainBrowserWindowToolBarChartTwoOrientedAxes.cxx
BrainBrowserWindowToolBarChartTwoType.cxx
BrainBrowserWindowToolBarChartType.cxx
BrainBrowserWindowToolBarComponent.cxx
BrainBrowserWindowToolBarHistology.cxx
BrainBrowserWindowToolBarImage.cxx
BrainBrowserWindowToolBarOrientation.cxx
BrainBrowserWindowToolBarSlicePlane.cxx
BrainBrowserWindowToolBarSliceSelection.cxx
BrainBrowserWindowToolBarSurface.cxx
BrainBrowserWindowToolBarSurfaceMontage.cxx
BrainBrowserWindowToolBarTab.cxx
BrainBrowserWindowToolBarTabPopUpMenu.cxx
BrainBrowserWindowToolBarView.cxx
BrainBrowserWindowToolBarVolumeMPR.cxx
BrainBrowserWindowToolBarVolumeMontage.cxx
BrainOpenGLWidget.cxx
BugReportDialog.cxx
CardinalDirectionEnumMenu.cxx
CaretColorEnumComboBox.cxx
CaretColorEnumMenu.cxx
CaretColorToolButton.cxx
CaretDataFileSelectionComboBox.cxx
CaretFileDialog.cxx
CaretFileDialogExtendable.cxx
CaretFileRemoteDialog.cxx
CaretFiveFileImportDialog.cxx
CaretMappableDataFileAndMapSelector.cxx
CaretMappableDataFileAndMapSelectorObject.cxx
CaretResultDialog.cxx
ChartHistoryViewController.cxx
ChartLinesSelectionViewController.cxx
ChartMatrixParcelSelectionViewController.cxx
ChartMatrixSeriesSelectionViewController.cxx
ChartTwoAxisPropertiesEditorDialog.cxx
ChartTwoAxisPropertiesEditorWidget.cxx
ChartTwoCartesianCustomSubdivisionsEditorWidget.cxx
ChartTwoLineLayerNormalizationWidget.cxx
ChartTwoOverlaySetViewController.cxx
ChartTwoOverlayViewController.cxx
ChartTwoTitleEditorWidget.cxx
ChartSelectionViewController.cxx
ChartToolBoxViewController.cxx
ChooseBorderFocusFromFileDialog.cxx
CiftiConnectivityMatrixViewController.cxx
CiftiParcelSelectionComboBox.cxx
ClippingPlanesWidget.cxx
ColorEditorWidget.cxx
ConnectivityCorrelationSettingsMenu.cxx
ContextSensitiveMenuItemsEnum.cxx
CopyPaletteColorMappingToFilesDialog.cxx
CursorDisplayScoped.cxx
CursorEnum.cxx
CursorManager.cxx
CustomViewDialog.cxx
CziImageExportDialog.cxx
DataFileContentCopyMoveDialog.cxx
DataFileEditorDialog.cxx
DingOntologyTermsDialog.cxx
DisplayGroupAndTabItemTreeWidgetItem.cxx
DisplayGroupAndTabItemViewController.cxx
DisplayGroupEnumComboBox.cxx
DynConnViewController.cxx
EventBrowserTabCloseInToolBar.cxx
EventBrowserTabDeleteInToolBar.cxx
EventBrowserTabNewInGUI.cxx
EventAnnotationCreateNewType.cxx
EventAnnotationDrawingFinishCancel.cxx
EventAnnotationGetDrawnInWindow.cxx
EventAnnotationGetSelectedInsertNewFile.cxx
EventBrowserWindowDrawingContent.cxx
EventBrowserWindowCreateTabs.cxx
EventBrowserWindowGraphicsRedrawn.cxx
EventBrowserWindowNew.cxx
EventBrowserWindowTileTabOperation.cxx
EventGetOrSetUserInputModeProcessor.cxx
EventGraphicsPaintNowAllWindows.cxx
EventGraphicsPaintNowOneWindow.cxx
EventGraphicsTimingOneWindow.cxx
EventGraphicsPaintSoonAllWindows.cxx
EventGraphicsPaintSoonOneWindow.cxx
EventGraphicsWindowShowToolTip.cxx
EventHelpViewerDisplay.cxx
EventIdentificationRequest.cxx
EventMovieManualModeRecording.cxx
EventOperatingSystemRequestOpenDataFile.cxx
EventOverlaySettingsEditorDialogRequest.cxx
EventPaletteColorMappingEditorDialogRequest.cxx
EventShowDataFileReadWarningsDialog.cxx
EventUpdateInformationWindows.cxx
EventUpdateVolumeEditingToolBar.cxx
EventUpdateYokedWindows.cxx
EventUserInterfaceUpdate.cxx
ExitProgramModifiedFilesDialog.cxx
FiberOrientationSelectionViewController.cxx
FiberSamplesOpenGLWidget.cxx
FociProjectionDialog.cxx
FociPropertiesEditorDialog.cxx
FociSelectionViewController.cxx
GapsAndMarginsDialog.cxx
GestureEvent.cxx
GiftiLabelTableEditor.cxx
GiftiLabelTableSelectionComboBox.cxx
GroupAndNameHierarchyTreeWidgetItem.cxx
GroupAndNameHierarchyViewController.cxx
GuiManager.cxx
HelpViewerDialog.cxx
HistologyOverlaySetViewController.cxx
HistologyOverlaySettingsMenu.cxx
HistologyOverlayViewController.cxx
HyperLinkTextBrowser.cxx
IdentificationDisplayDialog.cxx
IdentificationDisplayWidget.cxx
IdentificationFileFilteringTableWidget.cxx
IdentifyBrainordinateDialog.cxx
ImageCaptureDialog.cxx
ImageFileConvertToVolumeFileDialog.cxx
ImageSelectionViewController.cxx
InformationDisplayDialog.cxx
InformationDisplayPropertiesDialog.cxx
KeyEvent.cxx
LabelSelectionDialog.cxx
LabelSelectionWidget.cxx
LabelSelectionViewController.cxx
LabelSelectionViewHierarchyController.cxx
LabelSelectionViewWidget.cxx
LockAspectWarningDialog.cxx
MacApplication.cxx
MacDockMenu.cxx
MapSettingsChartTwoLineHistoryWidget.cxx
MapSettingsColorBarPaletteOptionsWidget.cxx
MapSettingsColorBarWidget.cxx
MapSettingsFiberTrajectoryWidget.cxx
MapSettingsLabelsWidget.cxx
MapSettingsLayerWidget.cxx
MapSettingsModulateWidget.cxx
MapSettingsPaletteColorMappingWidget.cxx
MapSettingsParcelsWidget.cxx
MapYokingGroupComboBox.cxx
MediaOverlaySettingsMenu.cxx
MediaOverlayViewController.cxx
MediaOverlaySetViewController.cxx
MetaDataEditorDialog.cxx
MetaDataEditorWidget.cxx
MovieRecordingDialog.cxx
OffScreenOpenGLRenderer.cxx
OpenFileQuicklyDialog.cxx
OverlaySetViewController.cxx
OverlayViewController.cxx
OverlaySettingsEditorDialog.cxx
PaletteColorMappingEditorDialog.cxx
PaletteCreateNewDialog.cxx
PaletteEditorDialog.cxx
PaletteEditorRangeRow.cxx
PaletteEditorRangeWidget.cxx
PalettePixmapPainter.cxx
PaletteSelectionWidget.cxx
PlotMagnifier.cxx
PlotPanner.cxx
PreferencesDevelopOptionsWidget.cxx
PreferencesDialog.cxx
PreferencesDisplayWidget.cxx
PreferencesImageWidget.cxx
PreferencesRecentFilesWidget.cxx
ProgressReportingDialog.cxx
ProgressReportingFromEvent.cxx
ProgressReportingWithSlots.cxx
QGLWidgetTextRenderer.cxx
RecentFilesDialog.cxx
RecentFilesTableWidget.cxx
RecentSceneMenu.cxx
RegionOfInterestCreateFromBorderDialog.cxx
SamplesSelectionViewController.cxx
ScaleBarWidget.cxx
SceneBasePathWidget.cxx
SceneClassInfoWidget.cxx
SceneCreateReplaceDialog.cxx
SceneDialog.cxx
SceneFileInformationDialog.cxx
SceneDataFileTreeItem.cxx
SceneDataFileTreeItemModel.cxx
ScenePreviewDialog.cxx
SceneReplaceAllDialog.cxx
SceneSelectionWidget.cxx
SceneShowOptionsDialog.cxx
SceneWindowGeometry.cxx
SpecFileManagementDialog.cxx
StructureEnumComboBox.cxx
StructureSurfaceSelectionControl.cxx
SurfacePropertiesEditorDialog.cxx
SurfaceSelectionViewController.cxx
ThresholdingSetMapsDialog.cxx
TileTabGridRowColumnWidgets.cxx
TileTabsConfigurationDialog.cxx
TileTabsGridConfigurationModifier.cxx
TileTabsLayoutConfigurationTypeWidget.cxx
TileTabsManualTabGeometryWidget.cxx
UnicodeCharacterEntryDialog.cxx
UserInputModeAbstract.cxx
UserInputModeAnnotations.cxx
UserInputModeAnnotationsContextMenu.cxx
UserInputModeAnnotationsWidget.cxx
UserInputModeBorders.cxx
UserInputModeBordersWidget.cxx
UserInputModeFoci.cxx
UserInputModeFociWidget.cxx
UserInputModeImage.cxx
UserInputModeImageWidget.cxx
UserInputModeSamplesEdit.cxx
UserInputModeTileTabsLayout.cxx
UserInputModeTileTabsLayoutContextMenu.cxx
UserInputModeView.cxx
UserInputModeViewContextMenu.cxx
UserInputModeViewContextTileTabsSubMenu.cxx
UserInputModeVolumeEdit.cxx
UserInputModeVolumeEditWidget.cxx
UsernamePasswordWidget.cxx
ViewModeEnum.cxx
VolumeFileCreateDialog.cxx
VolumeFileResampleDialog.cxx
VolumeMprSettingsWidget.cxx
VolumeObliqueOptionsWidget.cxx
VolumePropertiesEditorDialog.cxx
VolumeSurfaceOutlineColorOrTabViewController.cxx
VolumeSurfaceOutlineSetViewController.cxx
VolumeSurfaceOutlineViewController.cxx
WbMacroCustomDataInfo.cxx
WbMacroCustomDataTypeEnum.cxx
WbMacroCustomOperationAnimateOverlayCrossFade.cxx
WbMacroCustomOperationAnimateRotation.cxx
WbMacroCustomOperationAnimateSurfaceInterpolation.cxx
WbMacroCustomOperationAnimateVolumeSliceSequence.cxx
WbMacroCustomOperationAnimateVolumeToSurfaceCrossFade.cxx
WbMacroCustomOperationBase.cxx
WbMacroCustomOperationDelay.cxx
WbMacroCustomOperationIncrementRotation.cxx
WbMacroCustomOperationIncrementVolumeSlice.cxx
WbMacroCustomOperationManager.cxx
WbMacroCustomOperationSurfaceDefaultColor.cxx
WbMacroCustomOperationTypeEnum.cxx
WbMacroHelper.cxx
WbMacroWidgetActionsManager.cxx
WorkbenchInstallationAssistantDialog.cxx
WuQCollapsibleWidget.cxx
WuQColorEditorWidget.cxx
WuQDataEntryDialog.cxx
WuQDialog.cxx
WuQDialogModal.cxx
WuQDialogNonModal.cxx
WuQDoubleSlider.cxx
WuQDoubleSpinBox.cxx
WuQEventBlockingFilter.cxx
WuQFactory.cxx
WuQGridLayoutGroup.cxx
WuQGroupBoxExclusiveWidget.cxx
WuQHyperlinkToolTip.cxx
WuQImageLabel.cxx
WuQListWidget.cxx
WuQMacroCommandParameterWidget.cxx
WuQMacroCopyDialog.cxx
WuQMacroCreateDialog.cxx
WuQMacroDialog.cxx
WuQMacroExecutor.cxx
WuQMacroExecutorMonitor.cxx
WuQMacroExecutorOptions.cxx
WuQMacroManager.cxx
WuQMacroMenu.cxx
WuQMacroNewCommandSelectionDialog.cxx
WuQMacroShortCutKeyComboBox.cxx
WuQMacroSignalEmitter.cxx
WuQMacroSignalWatcher.cxx
WuQMacroWidgetAction.cxx
WuQMessageBox.cxx
WuQMessageBoxTwo.cxx
WuQwtPlot.cxx
WuQScrollArea.cxx
WuQSpecialIncrementDoubleSpinBox.cxx
WuQSpinBox.cxx
WuQSpinBoxGroup.cxx
WuQSpinBoxOddValue.cxx
WuQTabBar.cxx
WuQTabWidget.cxx
WuQTabWidgetWithSizeHint.cxx
WuQTextEditWithToolBarWidget.cxx
WuQTextEditorDialog.cxx
WuQTimedMessageDisplay.cxx
WuQToolTipHelper.cxx
WuQTreeWidget.cxx
WuQTrueFalseComboBox.cxx
WuQValueChangedSignalWatcher.cxx
#WuQWebView.cxx
WuQWidget.cxx
WuQWidgetDisabler.cxx
WuQWidgetObjectGroup.cxx
WuQtUtilities.cxx
ZipSceneFileDialog.cxx
)

#
# For macos, removes items that apple automatically
# adds to the view menu 
#
IF (APPLE)
   LIST(APPEND SOURCE_FILES
        macos.h
        macos.mm)
ENDIF()

#
# Process the header files with moc producing moc_*.cpp files
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/GuiQt)

#
# Create the GUI library
#
ADD_LIBRARY(GuiQt
    ${MOC_SOURCE_FILES}
    ${SOURCE_FILES}
)

TARGET_LINK_LIBRARIES(
    GuiQt
    ${CARET_QT5_LINK}
    ${MESA_OR_OPENGL_LIBRARIES}
)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/GuiQt
${Qwt_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Commands
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/CZIlib/CZI
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Graphics
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/OSMesaDummy
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/OmeZarr
${CMAKE_SOURCE_DIR}/Common
)
