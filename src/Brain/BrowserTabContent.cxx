
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <cmath>
#include <set>

#define __BROWSER_TAB_CONTENT_DECLARE__
#include "BrowserTabContent.h"
#undef __BROWSER_TAB_CONTENT_DECLARE__

#include <QMatrix4x4>
#include "AnnotationBrowserTab.h"
#include "AnnotationColorBar.h"
#include "AnnotationCoordinate.h"
#include "AnnotationScaleBar.h"
#include "BorderFile.h"
#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWindowContent.h"
#include "BrainStructure.h"
#include "BrainOpenGLVolumeMprTwoDrawing.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "ChartData.h"
#include "ChartMatrixDisplayProperties.h"
#include "CaretPreferences.h"
#include "ChartableMatrixInterface.h"
#include "ChartModelDataSeries.h"
#include "ChartTwoCartesianOrientedAxes.h"
#include "ChartTwoMatrixDisplayProperties.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "CiftiConnectivityMatrixParcelDynamicFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryMapFile.h"
#include "CiftiParcelSeriesFile.h"
#include "ClippingPlaneGroup.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyModel.h"
#include "GroupAndNameHierarchyName.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFoci.h"
#include "EventAnnotationBarsGet.h"
#include "EventCaretMappableDataFilesAndMapsInDisplayedOverlays.h"
#include "EventCaretMappableDataFileMapsViewedInOverlays.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "EventResetView.h"
#include "FociFile.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "HistologyOverlay.h"
#include "HistologyOverlaySet.h"
#include "HistologySlicesFile.h"
#include "HistologySliceSettings.h"
#include "IdentificationManager.h"
#include "ImageFile.h"
#include "LabelFile.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "MediaFile.h"
#include "MediaOverlay.h"
#include "MediaOverlaySet.h"
#include "MetricDynamicConnectivityFile.h"
#include "ModelChart.h"
#include "ModelChartTwo.h"
#include "ModelHistology.h"
#include "ModelMedia.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelSurfaceSelector.h"
#include "ModelTransform.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "MouseEvent.h"
#include "VolumeMprVirtualSliceView.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "PaletteColorMapping.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "ScenePrimitive.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceMontageConfigurationCerebellar.h"
#include "SurfaceMontageConfigurationCerebral.h"
#include "SurfaceMontageConfigurationFlatMaps.h"
#include "SurfaceMontageConfigurationHippocampus.h"
#include "SurfaceMontageConfigurationHippocampusFlatMaps.h"
#include "SurfaceSelectionModel.h"
#include "StructureEnum.h"
#include "TileTabsBrowserTabGeometry.h"
#include "TileTabsBrowserTabGeometrySceneHelper.h"
#include "VolumeFile.h"
#include "ViewingTransformations.h"
#include "ViewingTransformationsCerebellum.h"
#include "ViewingTransformationsMedia.h"
#include "ViewingTransformationsVolume.h"
#include "VolumeDynamicConnectivityFile.h"
#include "VolumeMprSettings.h"
#include "VolumeMprViewModeEnum.h"
#include "VolumeSliceSettings.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"
#include "WholeBrainSurfaceSettings.h"

using namespace caret;

/**
 * Constructor.
 * @param tabNumber
 *    Number for this tab.
 */
BrowserTabContent::BrowserTabContent(const int32_t tabNumber)
: TabContentBase()
{
    isExecutingConstructor = true;
    
    s_allBrowserTabContent.insert(this);
    
    const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    
    m_tabNumber = tabNumber;
    m_surfaceModelSelector = new ModelSurfaceSelector();
    m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
    m_volumeModel = NULL;
    m_wholeBrainModel = NULL;
    m_surfaceMontageModel = NULL;
    m_chartModel = NULL;
    m_chartTwoModel = NULL;
    m_histologyModel = NULL;
    m_mediaModel = NULL;
    m_guiName = "";
    m_userName = "";
    m_volumeSurfaceOutlineSetModel = new VolumeSurfaceOutlineSetModel();
    m_brainModelYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    m_chartModelYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    m_mediaModelYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    m_mediaDisplayCoordinateMode = MediaDisplayCoordinateModeEnum::PIXEL;
    m_identificationUpdatesVolumeSlices = prefs->isVolumeIdentificationDefaultedOn();
    m_identificationUpdatesHistologySlices = prefs->isHistologyIdentificationDefaultedOn();
    m_displayHistologyAxesCrosshairs = true;
    m_histologyFlipXAxisFlag = false;
    
    m_displayVolumeAxesCrosshairs = prefs->isVolumeAxesCrosshairsDisplayed();
    m_displayVolumeAxesCrosshairLabels = prefs->isVolumeAxesLabelsDisplayed();
    m_displayVolumeMontageAxesCoordinates = prefs->isVolumeMontageAxesCoordinatesDisplayed();
    m_volumeMontageCoordinateDisplayType = prefs->getVolumeMontageCoordinatesDislayType();
    m_volumeMontageCoordinatePrecision = prefs->getVolumeMontageCoordinatePrecision();
    m_volumeMontageCoordinateFontHeight = 10.0;
    m_volumeMontageCoordinateTextAlignment = VolumeMontageCoordinateTextAlignmentEnum::RIGHT;

    m_scaleBar.reset(new AnnotationScaleBar(AnnotationAttributesDefaultTypeEnum::NORMAL));
    initializeScaleBar();
    CaretAssert(m_scaleBar.get());
    
    m_lightingEnabled = true;
    
    m_aspectRatio = 1.0;
    m_aspectRatioLocked = false;
    
    m_cerebellumViewingTransformation  = new ViewingTransformationsCerebellum();
    m_flatSurfaceViewingTransformation = new ViewingTransformations();
    m_viewingTransformation            = new ViewingTransformations();
    m_histologyViewingTransformation   = new ViewingTransformationsMedia();
    m_mediaViewingTransformation       = new ViewingTransformationsMedia();
    m_volumeSliceViewingTransformation = new ViewingTransformationsVolume();
    m_chartTwoMatrixViewingTranformation  = new ViewingTransformations();
    m_chartTwoMatrixDisplayProperties = new ChartTwoMatrixDisplayProperties();
    
    m_wholeBrainSurfaceSettings        = new WholeBrainSurfaceSettings();
    
    m_obliqueVolumeRotationMatrix = new Matrix4x4();
    
    leftView();

    resetMprRotations();
    
    m_histologySliceSettings = new HistologySliceSettings();
    m_histologyOrientationAppliedToYokingFlag = false;
    
    m_volumeSliceSettings = new VolumeSliceSettings();
    
    m_clippingPlaneGroup = new ClippingPlaneGroup(m_tabNumber);
    
    m_manualLayoutBrowserTabAnnotation.reset(new AnnotationBrowserTab(AnnotationAttributesDefaultTypeEnum::NORMAL));
    m_manualLayoutBrowserTabAnnotation->setBrowserTabContent(this,
                                                             m_tabNumber);
    
    m_mouseLeftDragMode = MouseLeftDragModeEnum::INVALID;
    
    m_sceneClassAssistant = new SceneClassAssistant();
    m_sceneClassAssistant->add("m_tabNumber", 
                               &m_tabNumber);
    m_sceneClassAssistant->add("m_userName",
                               &m_userName);
    m_sceneClassAssistant->add<ModelTypeEnum, ModelTypeEnum::Enum>("m_selectedModelType",
                                                                   &m_selectedModelType);
    m_sceneClassAssistant->add("m_surfaceModelSelector",
                               "ModelSurfaceSelector",
                               m_surfaceModelSelector);
    
    m_sceneClassAssistant->add("m_cerebellumViewingTransformation",
                               "ViewingTransformations",
                               m_cerebellumViewingTransformation);
    
    m_sceneClassAssistant->add("m_flatSurfaceViewingTransformation",
                               "ViewingTransformations",
                               m_flatSurfaceViewingTransformation);
    m_sceneClassAssistant->add("m_viewingTransformation",
                               "ViewingTransformations",
                               m_viewingTransformation);
    
    m_sceneClassAssistant->add("m_mediaViewingTransformation",
                               "ViewingTransformationsMedia",
                               m_mediaViewingTransformation);
    
    m_sceneClassAssistant->add("m_histologyViewingTransformation",
                               "ViewingTransformationsMedia",
                               m_histologyViewingTransformation);
    
    m_sceneClassAssistant->add("m_volumeSliceViewingTransformation",
                               "ViewingTransformations",
                               m_volumeSliceViewingTransformation);
    
    m_sceneClassAssistant->add("m_chartTwoMatrixViewingTranformation",
                               "ViewingTransformations",
                               m_chartTwoMatrixViewingTranformation);
    
    m_sceneClassAssistant->add("m_chartTwoMatrixDisplayProperties",
                               "ChartTwoMatrixDisplayProperties",
                               m_chartTwoMatrixDisplayProperties);
    
    m_sceneClassAssistant->add("m_histologySliceSettings",
                               "HistologySliceSettings",
                               m_histologySliceSettings);
    
    m_sceneClassAssistant->add("m_histologyOrientationAppliedToYokingFlag",
                               &m_histologyOrientationAppliedToYokingFlag);
    
    m_sceneClassAssistant->add("m_volumeSliceSettings",
                               "VolumeSliceSettings",
                               m_volumeSliceSettings);

    m_sceneClassAssistant->add("m_wholeBrainSurfaceSettings",
                               "WholeBrainSurfaceSettings",
                               m_wholeBrainSurfaceSettings);

    m_sceneClassAssistant->add("m_identificationUpdatesVolumeSlices",
                               &m_identificationUpdatesVolumeSlices);
    m_sceneClassAssistant->add("m_identificationUpdatesHistologySlices",
                               &m_identificationUpdatesHistologySlices);
    m_sceneClassAssistant->add("m_displayHistologyAxesCrosshairs",
                               &m_displayHistologyAxesCrosshairs);
    m_sceneClassAssistant->add("m_histologyFlipXAxisFlag",
                               &m_histologyFlipXAxisFlag);
    
    m_sceneClassAssistant->add("m_displayVolumeAxesCrosshairs",
                               &m_displayVolumeAxesCrosshairs);
    m_sceneClassAssistant->add("m_displayVolumeAxesCrosshairLabels",
                               &m_displayVolumeAxesCrosshairLabels);
    m_sceneClassAssistant->add("m_displayVolumeMontageAxesCoordinates",
                               &m_displayVolumeMontageAxesCoordinates);
    m_sceneClassAssistant->add<VolumeMontageCoordinateDisplayTypeEnum, VolumeMontageCoordinateDisplayTypeEnum::Enum>("m_volumeMontageCoordinateDisplayType",
                                                                                                                     &m_volumeMontageCoordinateDisplayType);
    m_sceneClassAssistant->add("m_volumeMontageCoordinatePrecision",
                               &m_volumeMontageCoordinatePrecision);
    m_sceneClassAssistant->add("m_volumeMontageCoordinateFontHeight",
                               &m_volumeMontageCoordinateFontHeight);
    m_sceneClassAssistant->add<VolumeMontageCoordinateTextAlignmentEnum, VolumeMontageCoordinateTextAlignmentEnum::Enum>("m_volumeMontageCoordinateTextAlignment",
                                                                                                                         &m_volumeMontageCoordinateTextAlignment);

    m_sceneClassAssistant->add("m_lightingEnabled",
                               &m_lightingEnabled);
    m_sceneClassAssistant->add("m_aspectRatio",
                               &m_aspectRatio);
    m_sceneClassAssistant->add("m_aspectRatioLocked",
                               &m_aspectRatioLocked);
    
    m_sceneClassAssistant->add<YokingGroupEnum, YokingGroupEnum::Enum>("m_brainModelYokingGroup",
                                                                       &m_brainModelYokingGroup);
    m_sceneClassAssistant->add<YokingGroupEnum, YokingGroupEnum::Enum>("m_chartModelYokingGroup",
                                                                       &m_chartModelYokingGroup);
    m_sceneClassAssistant->add<YokingGroupEnum, YokingGroupEnum::Enum>("m_mediaModelYokingGroup",
                                                                       &m_mediaModelYokingGroup);

    m_sceneClassAssistant->add<MediaDisplayCoordinateModeEnum, MediaDisplayCoordinateModeEnum::Enum>("m_mediaDisplayCoordinateMode",
                                                                                                     &m_mediaDisplayCoordinateMode);

    m_sceneClassAssistant->add("m_scaleBar",
                               "AnnotationScaleBar",
                               m_scaleBar.get());

    m_sceneClassAssistant->add("m_mprRotationX",
                               &m_mprRotationX);
    m_sceneClassAssistant->add("m_mprRotationY",
                               &m_mprRotationY);
    m_sceneClassAssistant->add("m_mprRotationZ",
                               &m_mprRotationZ);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_ANNOTATION_BARS_GET);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS);
    
    isExecutingConstructor = false;
    
    /*
     * Media selection box is NOT saved to scenes NOR copie
     */
    m_regionSelectionBox.reset(new GraphicsRegionSelectionBox());
    
    /*
     * Need to be done from here
     */
    if (prefs->isYokingDefaultedOn()) {
        setBrainModelYokingGroup(YokingGroupEnum::YOKING_GROUP_A);
        setChartModelYokingGroup(YokingGroupEnum::YOKING_GROUP_OFF);
    }

    /*
     * Initialize the manual layout's default positions
     * These values are used when a Manual Layout is selected to determine if
     * the manual layout contains the defaults to that the bounds can be
     * changed the first time manual layout is selected.
     */
    AnnotationBrowserTab* annotationBrowserTab = getManualLayoutBrowserTabAnnotation();
    CaretAssert(annotationBrowserTab);
    
    TileTabsBrowserTabGeometry tabGeom(m_tabNumber);
    m_defaultManualTabGeometryBounds[0] = tabGeom.getMinX();
    m_defaultManualTabGeometryBounds[1] = tabGeom.getMaxX();
    m_defaultManualTabGeometryBounds[2] = tabGeom.getMinY();
    m_defaultManualTabGeometryBounds[3] = tabGeom.getMaxY();

    annotationBrowserTab->setBounds2D(m_defaultManualTabGeometryBounds[0],
                                      m_defaultManualTabGeometryBounds[1],
                                      m_defaultManualTabGeometryBounds[2],
                                      m_defaultManualTabGeometryBounds[3]);
    const float annWidth = (m_defaultManualTabGeometryBounds[1] - m_defaultManualTabGeometryBounds[0]);
    const float annHeight = (m_defaultManualTabGeometryBounds[3] - m_defaultManualTabGeometryBounds[2]);
    if (annWidth > 0.0) {
        m_aspectRatio = annHeight / annWidth;
    }
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    EventManager::get()->removeAllEventsFromListener(this);
 
    s_allBrowserTabContent.erase(this);
    
    delete m_clippingPlaneGroup;
    delete m_flatSurfaceViewingTransformation;
    delete m_cerebellumViewingTransformation;
    delete m_viewingTransformation;
    delete m_histologyViewingTransformation;
    delete m_mediaViewingTransformation;
    delete m_volumeSliceViewingTransformation;
    delete m_chartTwoMatrixViewingTranformation;
    delete m_chartTwoMatrixDisplayProperties;
    delete m_obliqueVolumeRotationMatrix;
    
    delete m_surfaceModelSelector;
    m_surfaceModelSelector = NULL;
    
    delete m_volumeSurfaceOutlineSetModel;
    m_volumeSurfaceOutlineSetModel = NULL;
    
    delete m_histologySliceSettings;
    delete m_volumeSliceSettings;
    
    delete m_wholeBrainSurfaceSettings;
    
    delete m_sceneClassAssistant;
    m_sceneClassAssistant = NULL;
}

/**
 * Clone the contents of the given browser tab.
 * @param tabToClone
 *    Tab whose contents is cloned.
 */
void
BrowserTabContent::cloneBrowserTabContent(BrowserTabContent* tabToClone)
{
    CaretAssert(tabToClone);
    m_surfaceModelSelector->setSelectedStructure(tabToClone->m_surfaceModelSelector->getSelectedStructure());
    m_surfaceModelSelector->setSelectedSurfaceModel(tabToClone->m_surfaceModelSelector->getSelectedSurfaceModel());

    m_selectedModelType = tabToClone->m_selectedModelType;

    /*
     *
     */
    EventModelGetAll allModelsEvent;
    EventManager::get()->sendEvent(allModelsEvent.getPointer());
    std::vector<Model*> allModels = allModelsEvent.getModels();
    for (std::vector<Model*>::iterator modelIter = allModels.begin();
         modelIter != allModels.end();
         modelIter++) {
        Model* model = *modelIter;
        model->copyTabContent(tabToClone->m_tabNumber,
                              m_tabNumber);
    }
    
    *m_clippingPlaneGroup = *tabToClone->m_clippingPlaneGroup;
    
    m_brainModelYokingGroup = tabToClone->m_brainModelYokingGroup;
    m_chartModelYokingGroup = tabToClone->m_chartModelYokingGroup;
    m_mediaModelYokingGroup = tabToClone->m_mediaModelYokingGroup;
    m_aspectRatio = tabToClone->m_aspectRatio;
    m_aspectRatioLocked = tabToClone->m_aspectRatioLocked;
    
    *m_cerebellumViewingTransformation = *tabToClone->m_cerebellumViewingTransformation;
    *m_flatSurfaceViewingTransformation = *tabToClone->m_flatSurfaceViewingTransformation;
    *m_viewingTransformation = *tabToClone->m_viewingTransformation;
    *m_histologyViewingTransformation = *tabToClone->m_histologyViewingTransformation;
    *m_mediaViewingTransformation = *tabToClone->m_mediaViewingTransformation;
    *m_volumeSliceViewingTransformation = *tabToClone->m_volumeSliceViewingTransformation;
    *m_chartTwoMatrixViewingTranformation = *tabToClone->m_chartTwoMatrixViewingTranformation;
    *m_chartTwoMatrixDisplayProperties = *tabToClone->m_chartTwoMatrixDisplayProperties;
    *m_histologySliceSettings = *tabToClone->m_histologySliceSettings;
    *m_volumeSliceSettings = *tabToClone->m_volumeSliceSettings;
    *m_wholeBrainSurfaceSettings = *tabToClone->m_wholeBrainSurfaceSettings;
    
    *m_obliqueVolumeRotationMatrix = *tabToClone->m_obliqueVolumeRotationMatrix;
    
    m_identificationUpdatesVolumeSlices = tabToClone->m_identificationUpdatesVolumeSlices;
    m_identificationUpdatesHistologySlices = tabToClone->m_identificationUpdatesHistologySlices;
    m_displayHistologyAxesCrosshairs = tabToClone->m_displayHistologyAxesCrosshairs;
    m_histologyFlipXAxisFlag = tabToClone->m_histologyFlipXAxisFlag;
    
    m_displayVolumeAxesCrosshairs = tabToClone->m_displayVolumeAxesCrosshairs;
    m_displayVolumeAxesCrosshairLabels = tabToClone->m_displayVolumeAxesCrosshairLabels;
    m_displayVolumeMontageAxesCoordinates = tabToClone->m_displayVolumeMontageAxesCoordinates;
    m_volumeMontageCoordinatePrecision = tabToClone->m_volumeMontageCoordinatePrecision;
    m_volumeMontageCoordinateFontHeight = tabToClone->m_volumeMontageCoordinateFontHeight;
    m_volumeMontageCoordinateTextAlignment = tabToClone->m_volumeMontageCoordinateTextAlignment;
    
    m_lightingEnabled = tabToClone->m_lightingEnabled;

    m_mediaDisplayCoordinateMode = tabToClone->m_mediaDisplayCoordinateMode;
    
    m_mprRotationX = tabToClone->m_mprRotationX;
    m_mprRotationY = tabToClone->m_mprRotationY;
    m_mprRotationZ = tabToClone->m_mprRotationZ;

    m_mprThreeRotationSeparateQuaternion = tabToClone->m_mprThreeRotationSeparateQuaternion;
    m_mprThreeAxialSeparateRotationQuaternion = tabToClone->m_mprThreeAxialSeparateRotationQuaternion;
    m_mprThreeCoronalSeparateRotationQuaternion = tabToClone->m_mprThreeCoronalSeparateRotationQuaternion;
    m_mprThreeParasagittalSeparateRotationQuaternion = tabToClone->m_mprThreeParasagittalSeparateRotationQuaternion;

    m_mprThreeAxialInverseRotationQuaternion = tabToClone->m_mprThreeAxialInverseRotationQuaternion;
    m_mprThreeCoronalInverseRotationQuaternion = tabToClone->m_mprThreeCoronalInverseRotationQuaternion;
    m_mprThreeParasagittalInverseRotationQuaternion = tabToClone->m_mprThreeParasagittalInverseRotationQuaternion;
    
    Model* model = getModelForDisplay();
    
    /* not cloned m_histologyOrientationAppliedToYokingFlag = tabToClone->m_histologyOrientationAppliedToYokingFlag; */
    
    if (model != NULL) {
        Brain* brain = model->getBrain();
        
        brain->copyDisplayPropertiesToTab(tabToClone->getTabNumber(),
                                          getTabNumber());
        
        brain->copyFilePropertiesToTab(tabToClone->getTabNumber(),
                                       getTabNumber());        
    }
    
    m_volumeSurfaceOutlineSetModel->copyVolumeSurfaceOutlineSetModel(tabToClone->getVolumeSurfaceOutlineSet());
    
    /*
     * For manual layout, make tab same size but put in bottom left corner
     */
    const AnnotationBrowserTab* cloneAnnotationBrowerTab = tabToClone->getManualLayoutBrowserTabAnnotation();
    CaretAssert(cloneAnnotationBrowerTab);
    const float minXY(10.0f);
    const float maxWidthHeight(100.0f - (minXY * 2));
    float tabWidth  = MathFunctions::limitRange(cloneAnnotationBrowerTab->getWidth(),  minXY, maxWidthHeight);
    float tabHeight = MathFunctions::limitRange(cloneAnnotationBrowerTab->getHeight(), minXY, maxWidthHeight);
    
    AnnotationBrowserTab* annotationBrowserTab = getManualLayoutBrowserTabAnnotation();
    CaretAssert(annotationBrowserTab);
    annotationBrowserTab->setBounds2D(minXY,
                                      minXY + tabWidth,
                                      minXY,
                                      minXY + tabHeight);
    
    setMouseLeftDragMode(MouseLeftDragModeEnum::INVALID);
}

/**
 * @return Default name for this tab.
 */
AString
BrowserTabContent::getDefaultName() const
{
    AString s = getTabNamePrefix();
    
    if ( ! m_closedFlag) {
        const Model* displayedModel =
        getModelForDisplay();
        if (displayedModel != NULL) {
            const AString name = displayedModel->getNameForBrowserTab();
            s += name;
        }
    }
    
    return s;
}

/**
 * @return Prefix for the tab name that consists of the 
 * tab number inside parenthesis.
 */
AString
BrowserTabContent::getTabNamePrefix() const
{
    const AString namePrefix = ("(" + AString::number(m_tabNumber + 1) + ") ");
    return namePrefix;
}


/**
 * Get the name of this browser tab.
 *
 * @return  Name of this tab.
 */
AString 
BrowserTabContent::getTabName() const
{
    if (m_userName.isEmpty()) {
        return getDefaultName();
    }

    const AString nameOut = (getTabNamePrefix()
                             + m_userName);
    return nameOut;
}

/**
 * Set the user name of this tab.  The user name
 * overrides the default naming.
 *
 * @param userName
 *    User name for tab.
 */
void 
BrowserTabContent::setUserTabName(const AString& userName)
{
    m_userName = userName.trimmed();
}

/**
 * @return The user name.
 */
AString 
BrowserTabContent::getUserTabName() const
{
    return m_userName;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrowserTabContent::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the window's content.
 *
 * @param descriptionOut
 *    Description of the window's content.
 */
void
BrowserTabContent::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    const int32_t tabIndex = getTabNumber();
    
    descriptionOut.addLine("Browser Tab "
                   + AString::number(tabIndex + 1)
                   + ": ");
    
    descriptionOut.pushIndentation();
    
    const Model* model = getModelForDisplay();
    
    if (model != NULL) {
        bool chartOneFlag = false;
        bool chartTwoFlag = false;
        bool histologyFlag = false;
        bool mediaFlag    = false;
        bool surfaceFlag  = false;
        bool surfaceMontageFlag = false;
        bool wholeBrainFlag = false;
        bool volumeFlag     = false;
        switch (model->getModelType()) {
            case ModelTypeEnum::MODEL_TYPE_CHART:
                chartOneFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                chartTwoFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_INVALID:
                break;
            case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                histologyFlag = true;
                break;
            case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                mediaFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE:
                surfaceFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                surfaceMontageFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                volumeFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                wholeBrainFlag = true;
                break;
        }
        
        if (chartOneFlag) {
            model->getDescriptionOfContent(tabIndex,
                                           descriptionOut);
        }
        else if (volumeFlag) {
            descriptionOut.addLine("Volume Slice View");
        }
        else if (wholeBrainFlag) {
            descriptionOut.addLine("All View");
            descriptionOut.pushIndentation();
            
            if (isWholeBrainCerebellumEnabled()) {
                const Surface* cerebellumSurface = m_wholeBrainModel->getSelectedSurface(StructureEnum::CEREBELLUM,
                                                                                         tabIndex);
                if (cerebellumSurface != NULL) {
                    cerebellumSurface->getDescriptionOfContent(descriptionOut);
                }
            }
            if (isWholeBrainLeftEnabled()) {
                const Surface* leftSurface = m_wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_LEFT,
                                                                                   tabIndex);
                if (leftSurface != NULL) {
                    leftSurface->getDescriptionOfContent(descriptionOut);
                }
            }
            if (isWholeBrainRightEnabled()) {
                const Surface* rightSurface = m_wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                                                                    tabIndex);
                if (rightSurface != NULL) {
                    rightSurface->getDescriptionOfContent(descriptionOut);
                }
            }
            
            descriptionOut.popIndentation();
        }
        else if (surfaceFlag) {
            model->getDescriptionOfContent(tabIndex,
                                           descriptionOut);
        }
        else if (surfaceMontageFlag) {
            model->getDescriptionOfContent(tabIndex,
                                           descriptionOut);
        }
        else if (mediaFlag) {
            model->getDescriptionOfContent(tabIndex,
                                           descriptionOut);
        }
        else if (histologyFlag) {
            model->getDescriptionOfContent(tabIndex,
                                           descriptionOut);
        }
        
        if (wholeBrainFlag
            || volumeFlag) {
            descriptionOut.pushIndentation();
            m_volumeSliceSettings->getDescriptionOfContent(model->getModelType(),
                                                           descriptionOut);
            descriptionOut.popIndentation();
        }
        
        if (chartOneFlag) {
            /* nothing */
        }
        else if (chartTwoFlag) {
            getChartTwoOverlaySet()->getDescriptionOfContent(descriptionOut);
        }
        else if (mediaFlag) {
            getMediaOverlaySet()->getDescriptionOfContent(descriptionOut);
        }
        else if (histologyFlag) {
            getHistologyOverlaySet()->getDescriptionOfContent(descriptionOut);
        }
        else {
            getOverlaySet()->getDescriptionOfContent(descriptionOut);
        }
    }
    
    descriptionOut.popIndentation();
}


/**
 * Get the selected model type.
 * 
 * @return The selected model type.
 */   
ModelTypeEnum::Enum 
BrowserTabContent::getSelectedModelType() const
{
    return m_selectedModelType;
}

/**
 * Set the selected model type.
 *
 * @param selectedModelType
 *    New selected model type. 
 */   
void 
BrowserTabContent::setSelectedModelType(ModelTypeEnum::Enum selectedModelType)
{
    m_selectedModelType = selectedModelType;
    setMouseLeftDragMode(MouseLeftDragModeEnum::INVALID);
}

/**
 * Get the model for DISPLAY.
 * 
 * @return  Pointer to displayed model or NULL
 *          if none are available.
 */   
Model* 
BrowserTabContent::getModelForDisplay()
{
    Model* mdc = NULL;
    
    switch (m_selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            mdc = m_histologyModel;
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            mdc = m_mediaModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            if (m_surfaceModelSelector != NULL) {
                mdc = m_surfaceModelSelector->getSelectedSurfaceModel();
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = m_surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = m_volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = m_wholeBrainModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            mdc = m_chartModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            mdc = m_chartTwoModel;
            break;
    }
    
    return mdc;
}

/**
 * Get the model model for DISPLAY.
 * 
 * @return  Pointer to displayed model or NULL
 *          if none are available.
 */   
const Model* 
BrowserTabContent::getModelForDisplay() const
{
    Model* mdc = NULL;
    
    switch (m_selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            mdc = m_histologyModel;
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            mdc = m_mediaModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = m_surfaceModelSelector->getSelectedSurfaceModel();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = m_surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = m_volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = m_wholeBrainModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            mdc = m_chartModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            mdc = m_chartTwoModel;
            break;
    }
    
    return mdc;
}

/**
 * Get the displayed chart model.
 *
 * @return  Pointer to displayed chart model or
 *          NULL if the displayed model is NOT a
 *          chart.
 */
ModelChart*
BrowserTabContent::getDisplayedChartOneModel()
{
    ModelChart* mc = dynamic_cast<ModelChart*>(getModelForDisplay());
    return mc;
}

/**
 * Get the displayed chart model.
 *
 * @return  Pointer to displayed chart model or
 *          NULL if the displayed model is NOT a
 *          chart.
 */
const ModelChart*
BrowserTabContent::getDisplayedChartOneModel() const
{
    const ModelChart* mc = dynamic_cast<const ModelChart*>(getModelForDisplay());
    return mc;
}

/**
 * Get the displayed chart model two.
 *
 * @return  Pointer to displayed chart model or
 *          NULL if the displayed model is NOT a
 *          chart.
 */
ModelChartTwo*
BrowserTabContent::getDisplayedChartTwoModel()
{
    ModelChartTwo* mc = dynamic_cast<ModelChartTwo*>(getModelForDisplay());
    return mc;
}

/**
 * Get the displayed chart model two.
 *
 * @return  Pointer to displayed chart model or
 *          NULL if the displayed model is NOT a
 *          chart.
 */
const ModelChartTwo*
BrowserTabContent::getDisplayedChartTwoModel() const
{
    const ModelChartTwo* mc = dynamic_cast<const ModelChartTwo*>(getModelForDisplay());
    return mc;
}

/**
 * Get the displayed histology model
 *
 * @return  Pointer to displayed histology model or
 *          NULL if the displayed model is NOT a
 *          histology.
 */
ModelHistology*
BrowserTabContent::getDisplayedHistologyModel()
{
    ModelHistology* mh(NULL);
    Model* model(getModelForDisplay());
    if (model != NULL) {
        mh = dynamic_cast<ModelHistology*>(getModelForDisplay());
    }
    return mh;
}

/**
 * Get the displayed histology model
 *
 * @return  Pointer to displayed histology model or
 *          NULL if the displayed model is NOT a
 *          histology.
 */
const ModelHistology*
BrowserTabContent::getDisplayedHistologyModel() const
{
    const ModelHistology* mh = dynamic_cast<const ModelHistology*>(getModelForDisplay());
    return mh;
}

/**
 * Get the displayed media model
 *
 * @return  Pointer to displayed media model or
 *          NULL if the displayed model is NOT a
 *          media.
 */
ModelMedia*
BrowserTabContent::getDisplayedMediaModel()
{
    ModelMedia* mm(NULL);
    Model* model(getModelForDisplay());
    if (model != NULL) {
        mm = dynamic_cast<ModelMedia*>(getModelForDisplay());
    }
    return mm;
}

/**
 * Get the displayed media model
 *
 * @return  Pointer to displayed media model or
 *          NULL if the displayed model is NOT a
 *          media.
 */
const ModelMedia*
BrowserTabContent::getDisplayedMediaModel() const
{
    const ModelMedia* mm = dynamic_cast<const ModelMedia*>(getModelForDisplay());
    return mm;
}


/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
ModelSurface* 
BrowserTabContent::getDisplayedSurfaceModel()
{
    ModelSurface* mdcs =
        dynamic_cast<ModelSurface*>(getModelForDisplay());
    return mdcs;
}

/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
const ModelSurface* 
BrowserTabContent::getDisplayedSurfaceModel() const
{
    const ModelSurface* mdcs =
    dynamic_cast<const ModelSurface*>(getModelForDisplay());
    return mdcs;
}

/**
 * Get the displayed volume model.
 * 
 * @return  Pointer to displayed volume model or 
 *          NULL if the displayed model is NOT a 
 *          volume.
 */   
ModelVolume* 
BrowserTabContent::getDisplayedVolumeModel()
{
    ModelVolume* mdcv =
        dynamic_cast<ModelVolume*>(getModelForDisplay());
    return mdcv;
}

/**
 * Get the displayed volume model.
 *
 * @return  Pointer to displayed volume model or
 *          NULL if the displayed model is NOT a
 *          volume.
 */
const ModelVolume*
BrowserTabContent::getDisplayedVolumeModel() const
{
    const ModelVolume* mdcv = dynamic_cast<const ModelVolume*>(getModelForDisplay());
    return mdcv;
}

/**
 * @return True if the displayed model is a cerebellum surface.
 */
bool
BrowserTabContent::isCerebellumDisplayed() const
{
    const ModelSurface* surfaceModel = getDisplayedSurfaceModel();
    if (surfaceModel != NULL) {
        if (surfaceModel->getSurface()->getStructure() == StructureEnum::CEREBELLUM) {
            return true;
        }
    }
    
    const ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
    if (montageModel != NULL) {
        if (montageModel->getSelectedConfigurationType(getTabNumber()) == SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return True if the displayed model is a flat surface.
 */
bool
BrowserTabContent::isFlatSurfaceDisplayed() const
{
    const ModelSurface* surfaceModel = getDisplayedSurfaceModel();
    if (surfaceModel != NULL) {
        if (surfaceModel->getSurface()->getSurfaceType() == SurfaceTypeEnum::FLAT) {
            return true;
        }
    }
    
    const ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
    if (montageModel != NULL) {
        switch (montageModel->getSelectedConfigurationType(getTabNumber())) {
            case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
                break;
            case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
                break;
            case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_CONFIGURATION:
                break;
            case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
                return true;
                break;
            case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION:
                return true;
                break;
        }
    }
    
    return false;
}

/**
 * @return True if the displayed model is a chart one
 */
bool
BrowserTabContent::isChartOneDisplayed() const
{
    const ModelChart* chartModel = getDisplayedChartOneModel();
    if (chartModel != NULL) {
        return true;
    }
    
    return false;
}

/**
 * @return True if the displayed model is a chart two
 */
bool
BrowserTabContent::isChartTwoDisplayed() const
{
    const ModelChartTwo* chartModel = getDisplayedChartTwoModel();
    if (chartModel != NULL) {
        return true;
    }
    
    return false;
}

/**
 * @return Is the displayed model a histology model?
 */
bool
BrowserTabContent::isHistologyDisplayed() const
{
    const ModelHistology* mh = dynamic_cast<const ModelHistology*>(getModelForDisplay());
    
    const bool histologyFlag = (mh != NULL);
    return histologyFlag;
}

/**
 * @return Is the displayed model a media model?
 */
bool
BrowserTabContent::isMediaDisplayed() const
{
    const ModelMedia* mm = dynamic_cast<const ModelMedia*>(getModelForDisplay());
    
    const bool mediaFlag = (mm != NULL);
    return mediaFlag;
}

/**
 * @return Is the displayed model a volume slice model?
 */
bool
BrowserTabContent::isVolumeSlicesDisplayed() const
{
    const ModelVolume* mdcv = dynamic_cast<const ModelVolume*>(getModelForDisplay());
    
    const bool volumeFlag = (mdcv != NULL);
    return volumeFlag;
}

/**
 * @return Function result indicating if a volume slices montage is
 * displayed and if not what is displayed.
 */
FunctionResult
BrowserTabContent::isVolumeSlicesMontageDisplayed() const
{
    AString msg;
    if (isVolumeSlicesDisplayed()) {
        switch (m_volumeSliceSettings->getSliceDrawingType()) {
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                break;
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                msg = ("Volume is displayed but montage view is not enabled in the selected tab: "
                       + getTabName());
                break;
        }
    }
    else {
        msg = ("Volume montage is not displayed in the selected tab: "
               + getTabName());
    }
    return FunctionResult(msg,
                          msg.isEmpty());
}

/**
 * @return  Is a MPR volume viewer displayed (OLD version)
 */
bool
BrowserTabContent::isVolumeMprOldDisplayed() const
{
    if (isVolumeSlicesDisplayed()) {
        switch (getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                return true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                break;
        }
    }
    
    return false;
}

/**
 * @return  Is a MPR volume viewer displayed (version 3)
 */
bool
BrowserTabContent::isVolumeMprThreeDisplayed() const
{
    if (isVolumeSlicesDisplayed()) {
        switch (getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                return true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                break;
        }
    }
    
    return false;
}

/**
 * @return Is the displayed model the whole brain model (ALL)?
 */
bool
BrowserTabContent::isWholeBrainDisplayed() const
{
    const ModelWholeBrain* mwb = dynamic_cast<const ModelWholeBrain*>(getModelForDisplay());
    const bool wholeBrainFlag = (mwb != NULL);
    return wholeBrainFlag;
}

/**
 * Get the displayed whole brain model.
 * 
 * @return  Pointer to displayed whole brain model or 
 *          NULL if the displayed model is NOT a 
 *          whole brain.
 */   
ModelWholeBrain* 
BrowserTabContent::getDisplayedWholeBrainModel()
{
    ModelWholeBrain* mdcwb =
        dynamic_cast<ModelWholeBrain*>(getModelForDisplay());
    return mdcwb;

}

/**
 * @return Pointer to displayed surface montage model
 * or NULL if the displayed model is not a surface
 * montage model.
 */
ModelSurfaceMontage* 
BrowserTabContent::getDisplayedSurfaceMontageModel()
{
    ModelSurfaceMontage* mdcsm =
    dynamic_cast<ModelSurfaceMontage*>(getModelForDisplay());
    return mdcsm;
}

/**
 * @return Pointer to displayed surface montage model
 * or NULL if the displayed model is not a surface
 * montage model.
 */
const ModelSurfaceMontage*
BrowserTabContent::getDisplayedSurfaceMontageModel() const
{
    const ModelSurfaceMontage* mdcsm =
    dynamic_cast<const ModelSurfaceMontage*>(getModelForDisplay());
    return mdcsm;
}

/**
 * Get all of the available surface models.
 * 
 * @return Vector containing all surface models.
 */   
const std::vector<ModelSurface*> 
BrowserTabContent::getAllSurfaceModels() const
{
    return m_allSurfaceModels;
}

/**
 * @return The surface model selector used to 
 * select a surface and structure.
 */
ModelSurfaceSelector* 
BrowserTabContent::getSurfaceModelSelector()
{
    return m_surfaceModelSelector;
}

/**
 * Get the overlay assignments for this tab.
 * 
 * @return  Overlay assignments for this tab or NULL if no valid model.
 */
OverlaySet* 
BrowserTabContent::getOverlaySet()
{
    Model* model = getModelForDisplay();
    if (model != NULL) {
       return model->getOverlaySet(m_tabNumber);
    }
    return NULL;
}

/**
 * Get the overlay assignments for this tab.
 *
 * @return  Overlay assignments for this tab or NULL if no valid model.
 */
const OverlaySet*
BrowserTabContent::getOverlaySet() const
{
    const Model* model = getModelForDisplay();
    if (model != NULL) {
        return model->getOverlaySet(m_tabNumber);
    }
    return NULL;
}

/**
 * @return Chart overlay set for this tab.
 */
ChartTwoOverlaySet*
BrowserTabContent::getChartTwoOverlaySet()
{
    if (m_chartTwoModel == NULL) {
        return NULL;
    }
    
    CaretAssert(m_chartTwoModel);
    return m_chartTwoModel->getChartTwoOverlaySet(m_tabNumber);
}

/**
 * @return Chart overlay set for this tab.
 */
const ChartTwoOverlaySet*
BrowserTabContent::getChartTwoOverlaySet() const
{
    if (m_chartTwoModel == NULL) {
        return NULL;
    }
    CaretAssert(m_chartTwoModel);
    return m_chartTwoModel->getChartTwoOverlaySet(m_tabNumber);
}

/**
 * @return Histology overlay set for this tab.
 */
HistologyOverlaySet*
BrowserTabContent::getHistologyOverlaySet()
{
    if (m_histologyModel == NULL) {
        return NULL;
    }
    
    CaretAssert(m_histologyModel);
    return m_histologyModel->getHistologyOverlaySet(m_tabNumber);
}

/**
 * @return Histology overlay set for this tab.
 */
const HistologyOverlaySet*
BrowserTabContent::getHistologyOverlaySet() const
{
    if (m_histologyModel == NULL) {
        return NULL;
    }
    CaretAssert(m_histologyModel);
    return m_histologyModel->getHistologyOverlaySet(m_tabNumber);
}

/**
 * @return Media overlay set for this tab.
 */
MediaOverlaySet*
BrowserTabContent::getMediaOverlaySet()
{
    if (m_mediaModel == NULL) {
        return NULL;
    }
    
    CaretAssert(m_mediaModel);
    return m_mediaModel->getMediaOverlaySet(m_tabNumber);
}

/**
 * @return Media overlay set for this tab.
 */
const MediaOverlaySet*
BrowserTabContent::getMediaOverlaySet() const
{
    if (m_mediaModel == NULL) {
        return NULL;
    }
    CaretAssert(m_mediaModel);
    return m_mediaModel->getMediaOverlaySet(m_tabNumber);
}

/**
 * @return Names of media files displayed in this overlay
 */
std::set<AString>
BrowserTabContent::getDisplayedMediaFileNames() const
{
    std::set<AString> names;
    
    if (isMediaDisplayed()) {
        const MediaOverlaySet* overlaySet = getMediaOverlaySet();
        if (overlaySet != NULL) {
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t i = 0; i < numOverlays; i++) {
                MediaOverlay* overlay = const_cast<MediaOverlay*>(overlaySet->getOverlay(i));
                CaretAssert(overlay);
                if (overlay->isEnabled()) {
                    const MediaOverlay::SelectionData selectionData(overlay->getSelectionData());
                    if (selectionData.m_selectedMediaFile != NULL) {
                        names.insert(selectionData.m_selectedMediaFile->getFileName());
                    }
                }
            }
        }

    }
    
    return names;
}

/**
 * @return Media display coordinate mode
 */
MediaDisplayCoordinateModeEnum::Enum
BrowserTabContent::getMediaDisplayCoordinateMode() const
{
    return m_mediaDisplayCoordinateMode;
}

/**
 * Set the media display coordinate mode
 * @param mediaDisplayCoordinateMode
 *    New media display coordinate mode
 */
void
BrowserTabContent::setMediaDisplayCoordinateMode(const MediaDisplayCoordinateModeEnum::Enum mediaDisplayCoordinateMode)
{
    if (mediaDisplayCoordinateMode != m_mediaDisplayCoordinateMode) {
        /*
         * Reset the view.  The pixel/plane coordinates may be substantially
         * different and if this is not done, the images may not be seen
         * until the Reset button is clicked.
         */
        resetView();
    }
    m_mediaDisplayCoordinateMode = mediaDisplayCoordinateMode;
    updateYokedModelBrowserTabs();
}

/**
 * Get all axes for chart two models (histogram, lines matrix) that are yoked with the given axes and range mode
 * @param axisOrientation
 *    The axes orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @return Vector containing all chart axes for the given orientation yoked to the given yoking range mode
 */
std::vector<ChartTwoCartesianOrientedAxes*>
BrowserTabContent::getYokedAxes(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode) const
{
    std::vector<ChartTwoCartesianOrientedAxes*> axesOut;
    
    if (m_chartTwoModel != NULL) {
        std::vector<ChartTwoOverlaySet*> overlaySets(m_chartTwoModel->getAllChartTwoOverlaySets(m_tabNumber));
        for (auto os : overlaySets) {
            switch (axisOrientation) {
                case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                    if (os->getHorizontalAxes()->getScaleRangeMode() == yokingRangeMode) {
                        axesOut.push_back(os->getHorizontalAxes());
                    }
                    break;
                case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                    if (os->getVerticalAxes()->getScaleRangeMode() == yokingRangeMode) {
                        axesOut.push_back(os->getVerticalAxes());
                    }
                    break;
            }
        }
    }

    return axesOut;
}


/**
 * @return Chart two matrix display properties.
 */
ChartTwoMatrixDisplayProperties*
BrowserTabContent::getChartTwoMatrixDisplayProperties()
{
    return m_chartTwoMatrixDisplayProperties;
}

/**
 * @return Chart two matrix display properties (const method)
 */
const ChartTwoMatrixDisplayProperties*
BrowserTabContent::getChartTwoMatrixDisplayProperties() const
{
    return m_chartTwoMatrixDisplayProperties;
}

/**
 * Get the tab number for this content.
 * 
 * @return  Tab number.
 */
int32_t 
BrowserTabContent::getTabNumber() const
{
    return m_tabNumber;
}

/**
 * Update the selected models.
 */
void 
BrowserTabContent::update(const std::vector<Model*> models)
{
    m_surfaceModelSelector->updateSelector(models);
    
    const int32_t numModels = static_cast<int32_t>(models.size());
    
    ModelVolume* previousVolumeModel = m_volumeModel;
    
    m_allSurfaceModels.clear();
    m_surfaceModelSelector->getSelectableSurfaceModels(m_allSurfaceModels);
    m_volumeModel = NULL;
    m_wholeBrainModel = NULL;
    m_surfaceMontageModel = NULL;
    m_chartModel = NULL;
    m_chartTwoModel = NULL;
    m_histologyModel = NULL;
    m_mediaModel = NULL;
    
    for (int i = 0; i < numModels; i++) {
        Model* mdc = models[i];
        
        ModelSurface* mdcs = dynamic_cast<ModelSurface*>(mdc);
        ModelVolume* mdcv = dynamic_cast<ModelVolume*>(mdc);
        ModelWholeBrain* mdcwb = dynamic_cast<ModelWholeBrain*>(mdc);
        ModelSurfaceMontage* mdcsm = dynamic_cast<ModelSurfaceMontage*>(mdc);
        ModelChart* mdch = dynamic_cast<ModelChart*>(mdc);
        ModelChartTwo* mdchTwo = dynamic_cast<ModelChartTwo*>(mdc);
        ModelHistology* mh = dynamic_cast<ModelHistology*>(mdc);
        ModelMedia* mdmm = dynamic_cast<ModelMedia*>(mdc);
        
        if (mdcs != NULL) {
            /* nothing to do since the surface model selector handles surfaces */
        }
        else if (mdcv != NULL) {
            CaretAssertMessage((m_volumeModel == NULL), "There is more than one volume model.");
            m_volumeModel = mdcv;
        }
        else if (mdcwb != NULL) {
            CaretAssertMessage((m_wholeBrainModel == NULL), "There is more than one whole brain model.");
            m_wholeBrainModel = mdcwb;
        }
        else if (mdcsm != NULL) {
            CaretAssertMessage((m_surfaceMontageModel == NULL), "There is more than one surface montage model.");
            m_surfaceMontageModel = mdcsm;
        }
        else if (mdch != NULL) {
            CaretAssertMessage((m_chartModel == NULL), "There is more than one chart model.");
            m_chartModel = mdch;
        }
        else if (mdchTwo != NULL) {
            CaretAssertMessage((m_chartTwoModel == NULL), "There is more than one chart two model.");
            m_chartTwoModel = mdchTwo;
        }
        else if (mh != NULL) {
            m_histologyModel = mh;
        }
        else if (mdmm != NULL) {
            m_mediaModel = mdmm;
        }
        else {
            CaretAssertMessage(0, (AString("Unknown type of brain model: ") + mdc->getNameForGUI(true)));
        }
    }
    
    switch (m_selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            if (m_histologyModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            if (m_mediaModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            if (m_surfaceModelSelector->getSelectedSurfaceModel() == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            if (m_surfaceMontageModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            if (m_volumeModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            if (m_wholeBrainModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            if (m_chartModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            if (m_chartTwoModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
    }
    
    if (m_selectedModelType == ModelTypeEnum::MODEL_TYPE_INVALID) {
        if (m_surfaceMontageModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE;
        }
        else if (m_volumeModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES;
        }
        else if (m_wholeBrainModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
        }
        else if (m_chartTwoModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_CHART_TWO;
        }
        else if (m_surfaceModelSelector->getSelectedSurfaceModel() != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE;
        }
        else if (m_chartModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_CHART;
        }
        else if (m_histologyModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_HISTOLOGY;
        }
        else if (m_mediaModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA;
        }
        
        setMouseLeftDragMode(MouseLeftDragModeEnum::INVALID);
    }
    
    if (m_volumeModel != NULL) {
        if (m_volumeModel != previousVolumeModel) {
        
            VolumeMappableInterface* underlayVolume = m_volumeModel->getOverlaySet(m_tabNumber)->getUnderlayVolume();
            if (underlayVolume != NULL) {
                /*
                 * Set montage slice spacing based upon slices
                 * in the Z-axis.
                 */
                std::vector<int64_t> dimensions;
                underlayVolume->getDimensions(dimensions);
                
                if (dimensions.size() >= 3) {
                    const int32_t dimZ = dimensions[2];
                    if (dimZ > 0) {
                        const int32_t maxZ = static_cast<int32_t>(dimZ * 0.90);
                        const int32_t minZ = static_cast<int32_t>(dimZ * 0.10);
                        const int32_t sliceRange = (maxZ - minZ);
                        int32_t sliceSpacing = 1;
                        if (sliceRange > 0) {
                            const int32_t numSlicesViewed = (m_volumeSliceSettings->getMontageNumberOfRows()
                                                             * m_volumeSliceSettings->getMontageNumberOfColumns());
                            sliceSpacing = (sliceRange / numSlicesViewed);
                        }
                        m_volumeSliceSettings->setMontageSliceSpacing(sliceSpacing);
                    }
                }
            }
        }
    }
    
    /*
     * Fixes selected slices for volume that do not
     * contain voxel at coordinate (0, 0, 0)
     */
    if (previousVolumeModel == NULL) {
        selectVolumeSlicesAtOriginPrivate();
    }
}

/**
 * Is the chart one model selection valid?
 *
 * @return bool indicating validity.
 */
bool
BrowserTabContent::isChartOneModelValid() const
{
    bool valid = (m_chartModel != NULL);
    return valid;
}

/**
 * Is the chart two model selection valid?
 *
 * @return bool indicating validity.
 */
bool
BrowserTabContent::isChartTwoModelValid() const
{
    bool valid = (m_chartTwoModel != NULL);
    return valid;
}

/**
 * Is the surface model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceModelValid() const
{
    bool valid = (m_allSurfaceModels.empty() == false);
    return valid;
}

/**
 * Is the histology model selection valid?
 * @return True if valid, else false.
 */
bool
BrowserTabContent::isHistologyModelValid() const
{
    const bool valid(m_histologyModel != NULL);
    return valid;
}

/**
 * Is the multi-medial model selection valid?
 * @return True if valid, else false.
 */
bool
BrowserTabContent::isMediaModelValid() const
{
    bool valid(m_mediaModel != NULL);
    return valid;
}

/**
 * Is the volume model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isVolumeSliceModelValid() const
{
    bool valid = (m_volumeModel != NULL);
    return valid;
}

/**
 * Is the whole brain model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isWholeBrainModelValid() const
{
    bool valid = (m_wholeBrainModel != NULL);
    return valid;
}

/**
 * Is the surface montage model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceMontageModelValid() const
{
    bool valid = (m_surfaceMontageModel != NULL);
    return valid;
}

/**
 * @return Is the aspect ratio locked?
 */
bool
BrowserTabContent::isAspectRatioLocked() const
{
    return m_aspectRatioLocked;
}

/**
 * Set the aspect ratio locked status.
 * 
 * @param locked
 *     New status.
 */
void
BrowserTabContent::setAspectRatioLocked(const bool locked)
{
    m_aspectRatioLocked = locked;
}

/**
 * @return The aspect ratio.
 */
float
BrowserTabContent::getAspectRatio() const
{
    return m_aspectRatio;
}

/**
 * Set the aspect ratio.
 *
 * @param aspectRatio
 *     New value for aspect ratio.
 */
void
BrowserTabContent::setAspectRatio(const float aspectRatio)
{
    m_aspectRatio = aspectRatio;
}


/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
BrowserTabContent::receiveEvent(Event* event)
{
    /*
     * Ignore events while closed but available for reopening
     */
    if (m_closedFlag) {
        return;
    }
    
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_BARS_GET) {
        EventAnnotationBarsGet* barsEvent = dynamic_cast<EventAnnotationBarsGet*>(event);
        CaretAssert(barsEvent);
        
        if (barsEvent->isGetAnnotationColorBarsForTabIndex(m_tabNumber)) {
            std::vector<AnnotationColorBar*> colorBars;
            getAnnotationColorBars(colorBars);

            barsEvent->addAnnotationColorBars(colorBars);
            
            if (m_scaleBar->isDisplayed()) {
                /*
                 * Scale bar is derived from color bar
                 */
                barsEvent->addAnnotationScaleBar(m_scaleBar.get());
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idLocationEvent =
        dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idLocationEvent);

        Model* model = getModelForDisplay();
        if (model == NULL) {
            return;
        }

        if (idLocationEvent->isTabSelected(getTabNumber())) {
            if (isIdentificationUpdateHistologySlices()) {
                const Vector3D xyz(idLocationEvent->getStereotaxicXYZ());
                HistologyOverlaySet* histologyOverlaySet(getHistologyOverlaySet());
                if (histologyOverlaySet != NULL) {
                    HistologyOverlay* histologyUnderlay(histologyOverlaySet->getUnderlay());
                    if (histologyUnderlay != NULL) {
                        HistologySlicesFile* histologySlicesFile(histologyUnderlay->getSelectionData().m_selectedFile);
                        if (histologySlicesFile != NULL ) {
                            HistologyCoordinate hc(HistologyCoordinate::newInstanceStereotaxicXYZ(histologySlicesFile,
                                                                                                  xyz));
                            if (hc.isValid()) {
                                setHistologySelectedCoordinate(histologySlicesFile,
                                                               hc,
                                                               MoveYokedVolumeSlices::MOVE_NO);
                            }
                        }
                    }
                }
            }
            
            if (isIdentificationUpdatesVolumeSlices()) {
                /*
                 * Use the identified XYZ.  Note that using voxel center
                 * may cause MPR slices to jump to a different slice (5/29/2024)
                 */
                Vector3D volumeSliceXYZ(idLocationEvent->getStereotaxicXYZ());
                
                /*
                 * If montage viewing, do not alter the slice
                 * coordinate in the axis being viewed
                 */
                if (getDisplayedVolumeModel() != NULL) {
                    bool keepSliceCoordinateForSelectedAxis = false;
                    switch (m_volumeSliceSettings->getSliceDrawingType()) {
                        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                            keepSliceCoordinateForSelectedAxis = true;
                            break;
                        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                            break;
                    }
                    
                    if (keepSliceCoordinateForSelectedAxis) {
                        switch (getVolumeSliceViewPlane()) {
                            case VolumeSliceViewPlaneEnum::ALL:
                                volumeSliceXYZ[0] = getVolumeSliceCoordinateParasagittal();
                                volumeSliceXYZ[1] = getVolumeSliceCoordinateCoronal();
                                volumeSliceXYZ[2] = getVolumeSliceCoordinateAxial();
                                break;
                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                volumeSliceXYZ[0] = getVolumeSliceCoordinateParasagittal();
                                break;
                            case VolumeSliceViewPlaneEnum::CORONAL:
                                volumeSliceXYZ[1] = getVolumeSliceCoordinateCoronal();
                                break;
                            case VolumeSliceViewPlaneEnum::AXIAL:
                                volumeSliceXYZ[2] = getVolumeSliceCoordinateAxial();
                                break;
                        }
                    }
                }

                selectVolumeSlicesAtCoordinate(volumeSliceXYZ);
            }
        }
        
        idLocationEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS) {
        EventCaretMappableDataFileMapsViewedInOverlays* mapOverlayEvent  =
        dynamic_cast<EventCaretMappableDataFileMapsViewedInOverlays*>(event);
        CaretAssert(mapOverlayEvent);
        
        OverlaySet* overlaySet = getOverlaySet();
        const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
        for (int32_t i = (numOverlays - 1); i >= 0; i--) {
            Overlay* overlay = overlaySet->getOverlay(i);
            if (overlay->isEnabled()) {
                CaretMappableDataFile* mapFile;
                int32_t mapFileIndex;
                overlay->getSelectionData(mapFile, mapFileIndex);
                if (mapFile != NULL) {
                    if (mapFile == mapOverlayEvent->getCaretMappableDataFile()) {
                        if ((mapFileIndex >= 0)
                            && (mapFileIndex < mapFile->getNumberOfMaps())) {
                            mapOverlayEvent->addMapIndex(mapFileIndex);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Get annotation color bars that should be drawn for this tab.
 *
 * @param colorBarsOut
 *     Colorbars that should be drawn.
 */
void
BrowserTabContent::getAnnotationColorBars(std::vector<AnnotationColorBar*>& colorBarsOut)
{
    colorBarsOut.clear();
    
    if (getModelForDisplay() == NULL) {
        return;
    }
    
    bool useOverlayFlag = false;
    bool useChartOneFlag  = false;
    bool useChartTwoFlag  = false;
    switch (getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            useChartOneFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            useChartTwoFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            useOverlayFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            useOverlayFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            useOverlayFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            useOverlayFlag = true;
            break;
    }
    
    
    std::vector<ColorBarFileMap> colorBarMapFileInfo;
    
    if (useOverlayFlag) {
        OverlaySet* overlaySet = getOverlaySet();
        const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
        for (int32_t i = (numOverlays - 1); i >= 0; i--) {
            Overlay* overlay = overlaySet->getOverlay(i);
            if (overlay->isEnabled()) {
                AnnotationColorBar* colorBar = overlay->getColorBar();
                CaretMappableDataFile* mapFile;
                int32_t mapIndex;
                overlay->getSelectionData(mapFile, mapIndex);
                
                colorBarMapFileInfo.push_back(ColorBarFileMap(colorBar,
                                                              mapFile,
                                                              mapIndex));
            }
        }
    }
    
    if (useChartOneFlag) {
        ModelChart* modelChart = getDisplayedChartOneModel();
        if (modelChart != NULL) {
            CaretDataFileSelectionModel* fileModel = NULL;
            
            switch (modelChart->getSelectedChartOneDataType(m_tabNumber)) {
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                    if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER) {
                        fileModel = modelChart->getChartableMatrixParcelFileSelectionModel(m_tabNumber);
                    }
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                    if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES) {
                        fileModel = modelChart->getChartableMatrixSeriesFileSelectionModel(m_tabNumber);
                    }
            }
            
            if (fileModel != NULL) {
                CaretDataFile* caretFile = fileModel->getSelectedFile();
                if (caretFile != NULL) {
                    CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(caretFile);
                    if (mapFile != NULL) {
                        ChartableMatrixInterface* matrixFile = dynamic_cast<ChartableMatrixInterface*>(mapFile);
                        if (matrixFile != NULL) {
                            ChartMatrixDisplayProperties* props = matrixFile->getChartMatrixDisplayProperties(m_tabNumber);
                            AnnotationColorBar* colorBar = props->getColorBar();
                            
                            const int32_t mapIndex = 0;
                            colorBarMapFileInfo.push_back(ColorBarFileMap(colorBar,
                                                                          mapFile,
                                                                          mapIndex));
                        }
                    }
                }
            }
        }
    }
    
    if (useChartTwoFlag) {
        ModelChartTwo* modelChartTwo = getDisplayedChartTwoModel();
        if (modelChartTwo != NULL) {
            ChartTwoOverlaySet* overlaySet = m_chartTwoModel->getChartTwoOverlaySet(m_tabNumber);
            if (overlaySet != NULL) {
                const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
                for (int32_t i = 0; i < numOverlays; i++) {
                    ChartTwoOverlay* chartOverlay = overlaySet->getOverlay(i);
                    if (chartOverlay->isEnabled()) {
                        switch (chartOverlay->getChartTwoDataType()) {
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                            {
                                CaretMappableDataFile* mapFile = NULL;
                                ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                                int32_t selectedIndex = -1;
                                chartOverlay->getSelectionData(mapFile,
                                                               selectedIndexType,
                                                               selectedIndex);
                                
                                
                                if (mapFile != NULL) {
                                    if (mapFile->isMappedWithPalette()) {
                                        AnnotationColorBar* colorBar = chartOverlay->getColorBar();
                                        colorBarMapFileInfo.push_back(ColorBarFileMap(colorBar,
                                                                                      mapFile,
                                                                                      selectedIndex));
                                    }
                                }
                            }
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                            {
                                CaretMappableDataFile* mapFile = NULL;
                                ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                                int32_t selectedIndex = -1;
                                chartOverlay->getSelectionData(mapFile,
                                                               selectedIndexType,
                                                               selectedIndex);
                                
                                
                                if (mapFile != NULL) {
                                    if (mapFile->isMappedWithPalette()) {
                                        AnnotationColorBar* colorBar = chartOverlay->getColorBar();
                                        /*
                                         * Matrix is all maps and uses map index 0
                                         */
                                        selectedIndex = 0;

                                        colorBarMapFileInfo.push_back(ColorBarFileMap(colorBar,
                                                                                      mapFile,
                                                                                      selectedIndex));
                                    }
                                }
                            }
                                break;
                        }
                    }
                }
            }
        }
    }
    
    const int32_t numColorBarMapInfo = static_cast<int32_t>(colorBarMapFileInfo.size());
    for (int32_t i = 0; i < numColorBarMapInfo; i++) {
        CaretAssertVectorIndex(colorBarMapFileInfo, i);
        const ColorBarFileMap& info = colorBarMapFileInfo[i];
        
        if (info.m_colorBar->isDisplayed()) {
            if (info.m_mapFile != NULL) {
                if (info.m_mapFile->isMappedWithPalette()) {
                    if ((info.m_mapIndex >= 0)
                        && (info.m_mapIndex < info.m_mapFile->getNumberOfMaps())) {
                        PaletteColorMapping* paletteColorMapping = info.m_mapFile->getMapPaletteColorMapping(info.m_mapIndex);
                        if (paletteColorMapping != NULL) {
                            FastStatistics* statistics = NULL;
                            switch (info.m_mapFile->getPaletteNormalizationMode()) {
                                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                                    statistics = const_cast<FastStatistics*>(info.m_mapFile->getFileFastStatistics());
                                    break;
                                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                                    statistics = const_cast<FastStatistics*>(info.m_mapFile->getMapFastStatistics(info.m_mapIndex));
                                    break;
                            }
                            
                            /*
                             * Statistics may be NULL for some instances of histograms
                             * from dynamically loaded files (dynconn)
                             */
                            if (statistics != NULL) {
                                paletteColorMapping->setupAnnotationColorBar(statistics,
                                                                             info.m_colorBar);
                                
                                info.m_colorBar->setTabIndex(m_tabNumber);
                                colorBarsOut.push_back(info.m_colorBar);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Get the map files for which a palette should be displayed in the
 * graphcis window.  Note that the order of map files and indices starts
 * with the bottom most layer and ends with the top most overlay.
 *
 * @param mapFiles
 *    Outut Map files that should have a palette displayed in the graphics window.
 * @param mapIndices
 *    Output Indices of the maps in the mapFiles.
 */
void 
BrowserTabContent::getDisplayedPaletteMapFiles(std::vector<CaretMappableDataFile*>& mapFiles,
                                               std::vector<int32_t>& mapIndices)
{
    mapFiles.clear();
    mapIndices.clear();
    
    if (getModelForDisplay() == NULL) {
        return;
    }
    
    bool useOverlayFlag = false;
    bool useChartOneFlag  = false;
    bool useChartTwoFlag  = false;
    switch (getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            useChartOneFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            useChartTwoFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            useOverlayFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            useOverlayFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            useOverlayFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            useOverlayFlag = true;
            break;
    }
    
    if (useOverlayFlag) {
        OverlaySet* overlaySet = getOverlaySet();
        const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
        for (int32_t i = (numOverlays - 1); i >= 0; i--) {
            Overlay* overlay = overlaySet->getOverlay(i);
            if (overlay->isEnabled()) {
                AnnotationColorBar* colorBar = overlay->getColorBar();
                if (colorBar->isDisplayed()) {
                    CaretMappableDataFile* mapFile;
                    int32_t mapFileIndex;
                    overlay->getSelectionData(mapFile, mapFileIndex);
                    if (mapFile != NULL) {
                        if (mapFile->isMappedWithPalette()) {
                            if ((mapFileIndex >= 0)
                                && (mapFileIndex < mapFile->getNumberOfMaps())) {
                                mapFiles.push_back(mapFile);
                                mapIndices.push_back(mapFileIndex);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (useChartOneFlag) {
        ModelChart* modelChart = getDisplayedChartOneModel();
        if (modelChart != NULL) {
            CaretDataFileSelectionModel* fileModel = NULL;
            
            switch (modelChart->getSelectedChartOneDataType(m_tabNumber)) {
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                    if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER) {
                        fileModel = modelChart->getChartableMatrixParcelFileSelectionModel(m_tabNumber);
                    }
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                    if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES) {
                        fileModel = modelChart->getChartableMatrixSeriesFileSelectionModel(m_tabNumber);
                    }
            }
            
            if (fileModel != NULL) {
                CaretDataFile* caretFile = fileModel->getSelectedFile();
                if (caretFile != NULL) {
                    CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(caretFile);
                    if (mapFile != NULL) {
                        ChartableMatrixInterface* matrixFile = dynamic_cast<ChartableMatrixInterface*>(mapFile);
                        if (matrixFile != NULL) {
                            ChartMatrixDisplayProperties* props = matrixFile->getChartMatrixDisplayProperties(m_tabNumber);
                            if (props->getColorBar()->isDisplayed()) {
                                /*
                                 * Matrix contains all file data and always
                                 * uses a map index of zero.
                                 */
                                mapFiles.push_back(mapFile);
                                mapIndices.push_back(0);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (useChartTwoFlag) {
        ModelChartTwo* modelChartTwo = getDisplayedChartTwoModel();
        if (modelChartTwo != NULL) {
            ChartTwoOverlaySet* overlaySet = m_chartTwoModel->getChartTwoOverlaySet(m_tabNumber);
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t i = (numOverlays - 1); i >= 0; i--) {
                ChartTwoOverlay* chartOverlay = overlaySet->getOverlay(i);
                if (chartOverlay->isEnabled()) {
                    switch (chartOverlay->getChartTwoDataType()) {
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                        {
                            CaretMappableDataFile* mapFile = NULL;
                            ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                            int32_t selectedIndex = -1;
                            chartOverlay->getSelectionData(mapFile,
                                                           selectedIndexType,
                                                           selectedIndex);
                            
                            if (mapFile != NULL) {
                                if (mapFile->isMappedWithPalette()) {
                                    AnnotationColorBar* colorBar = chartOverlay->getColorBar();
                                    if (colorBar->isDisplayed()) {
                                        mapFiles.push_back(mapFile);
                                        mapIndices.push_back(selectedIndex);
                                    }
                                }
                            }
                        }
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                        {
                            CaretMappableDataFile* mapFile = NULL;
                            ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                            int32_t selectedIndex = -1;
                            chartOverlay->getSelectionData(mapFile,
                                                           selectedIndexType,
                                                           selectedIndex);
                            
                            if (mapFile != NULL) {
                                if (mapFile->isMappedWithPalette()) {
                                    AnnotationColorBar* colorBar = chartOverlay->getColorBar();
                                    if (colorBar->isDisplayed()) {
                                        mapFiles.push_back(mapFile);
                                        mapIndices.push_back(0);
                                    }
                                }
                            }
                        }
                            break;
                    }
                }
            }
        }
    }
    
    CaretAssert(mapFiles.size() == mapIndices.size());
}

/**
 * @return The volume surface outline model for this tab.
 */
VolumeSurfaceOutlineSetModel*
BrowserTabContent::getVolumeSurfaceOutlineSet()
{
    return m_volumeSurfaceOutlineSetModel;
}

/**
 * @return The volume surface outline model for this tab.
 */
const VolumeSurfaceOutlineSetModel* 
BrowserTabContent::getVolumeSurfaceOutlineSet() const
{
    return m_volumeSurfaceOutlineSetModel;
}

/**
 * @return Structures from surface(s) displayed in this tab.
 */
std::vector<StructureEnum::Enum>
BrowserTabContent::getSurfaceStructuresDisplayed()
{
    std::vector<CaretDataFile*> allDisplayedFiles;
    getFilesDisplayedInTab(allDisplayedFiles);
    
    std::set<StructureEnum::Enum> structures;
    for (std::vector<CaretDataFile*>::iterator fileIter = allDisplayedFiles.begin();
         fileIter != allDisplayedFiles.end();
         fileIter++) {
        CaretDataFile* file = *fileIter;
        CaretAssert(file);
        if (file->getDataFileType() == DataFileTypeEnum::SURFACE) {
            SurfaceFile* surfaceFile = dynamic_cast<SurfaceFile*>(file);
            CaretAssert(surfaceFile);
            structures.insert(surfaceFile->getStructure());
        }
    }
    
    std::vector<StructureEnum::Enum> structuresOut(structures.begin(),
                                                   structures.end());
    return structuresOut;
}

/**
 * Get data files and their map indices in all displayed overlays.
 *
 * @param fileAndMapsEvent
 *     File and maps event to which files and maps are added.
 */
void
BrowserTabContent::getFilesAndMapIndicesInOverlays(EventCaretMappableDataFilesAndMapsInDisplayedOverlays* fileAndMapsEvent)
{
    Model* model = getModelForDisplay();
    if (model == NULL) {
        return;
    }

    bool addBrainordinateOverlaysFlag(false);
    const int32_t tabIndex = getTabNumber();
    switch (model->getModelType()) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
        {
            ModelChartTwo* chartTwoModel = getDisplayedChartTwoModel();
            CaretAssert(chartTwoModel);
            ChartTwoOverlaySet* chartOverlaySet = chartTwoModel->getChartTwoOverlaySet(tabIndex);
            const int32_t numOverlays = chartOverlaySet->getNumberOfDisplayedOverlays();
            for (int32_t i = 0; i < numOverlays; i++) {
                ChartTwoOverlay* overlay = chartOverlaySet->getOverlay(i);
                if (overlay->isEnabled()) {
                    CaretMappableDataFile* overlayDataFile = NULL;
                    ChartTwoOverlay::SelectedIndexType indexType;
                    int32_t mapIndex(0);
                    overlay->getSelectionData(overlayDataFile,
                                              indexType,
                                              mapIndex);
                    
                    if (overlayDataFile != NULL) {
                        switch (indexType) {
                            case ChartTwoOverlay::SelectedIndexType::INVALID:
                                if (overlay->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES) {
                                    mapIndex = 0;
                                }
                                break;
                            case ChartTwoOverlay::SelectedIndexType::COLUMN:
                                break;
                            case ChartTwoOverlay::SelectedIndexType::MAP:
                                break;
                            case ChartTwoOverlay::SelectedIndexType::ROW:
                                break;
                        }
                        if (mapIndex >= 0) {
                            fileAndMapsEvent->addChartTwoFileAndMap(overlayDataFile,
                                                                    mapIndex,
                                                                    m_tabNumber);
                        }
                    }
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
         {
             HistologyOverlaySet* overlaySet = model->getHistologyOverlaySet(tabIndex);
             const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
             for (int32_t i = 0; i < numOverlays; i++) {
                 HistologyOverlay* overlay = overlaySet->getOverlay(i);
                 if (overlay->isEnabled()) {
                     const HistologyOverlay::SelectionData selectionData(overlay->getSelectionData());
                     if (selectionData.m_selectedFile != NULL) {
                         std::vector<int32_t> frameIndices;
                         if (selectionData.m_selectedSliceIndex >= 0) {
                             fileAndMapsEvent->addHistologyFileAndSliceIndex(selectionData.m_selectedFile,
                                                                             selectionData.m_selectedSliceIndex,
                                                                             m_tabNumber);
                         }
                     }
                 }
             }
             
             addBrainordinateOverlaysFlag = true;
         }
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
        {
            MediaOverlaySet* overlaySet = model->getMediaOverlaySet(tabIndex);
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t i = 0; i < numOverlays; i++) {
                MediaOverlay* overlay = overlaySet->getOverlay(i);
                if (overlay->isEnabled()) {
                    const MediaOverlay::SelectionData selectionData(overlay->getSelectionData());
                    if (selectionData.m_selectedMediaFile != NULL) {
                        std::vector<int32_t> frameIndices;
                        if (selectionData.m_allFramesSelectedFlag) {
                            for (int32_t iFrame = 0; iFrame < selectionData.m_selectedMediaFile->getNumberOfFrames(); iFrame++) {
                                fileAndMapsEvent->addMediaFileAndFrame(selectionData.m_selectedMediaFile,
                                                                       iFrame,
                                                                       m_tabNumber);
                            }
                        }
                        else {
                            if (selectionData.m_selectedFrameIndex >= 0) {
                                fileAndMapsEvent->addMediaFileAndFrame(selectionData.m_selectedMediaFile,
                                                                       selectionData.m_selectedFrameIndex,
                                                                       m_tabNumber);
                            }
                        }
                    }
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            addBrainordinateOverlaysFlag = true;
            break;
    }
    
    if (addBrainordinateOverlaysFlag) {
        OverlaySet* overlaySet = model->getOverlaySet(tabIndex);
        const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
        for (int32_t i = 0; i < numOverlays; i++) {
            Overlay* overlay = overlaySet->getOverlay(i);
            if (overlay->isEnabled()) {
                CaretMappableDataFile* overlayDataFile = NULL;
                int32_t mapIndex;
                overlay->getSelectionData(overlayDataFile,
                                          mapIndex);
                
                if (overlayDataFile != NULL) {
                    if (mapIndex >= 0) {
                        fileAndMapsEvent->addBrainordinateFileAndMap(overlayDataFile,
                                                                     mapIndex,
                                                                     m_tabNumber);
                    }
                }
            }
        }
    }
}

/**
 * Get the data files displayed in this tab.
 * @param displayedDataFilesOut
 *    Displayed data file are loaded into this parameter.
 */
void 
BrowserTabContent::getFilesDisplayedInTab(std::vector<CaretDataFile*>& displayedDataFilesOut)
{
    displayedDataFilesOut.clear();
 
    Model* model = getModelForDisplay();
    if (model == NULL) {
        return;
    }
    
    std::set<CaretDataFile*> displayedDataFiles;
    
    const int32_t tabIndex = getTabNumber();
    switch (getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
        {
            HistologyOverlaySet* overlaySet = m_histologyModel->getHistologyOverlaySet(tabIndex);
            if (overlaySet != NULL) {
                const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
                for (int32_t i = 0; i < numOverlays; i++) {
                    HistologyOverlay* histologyOverlay = overlaySet->getOverlay(i);
                    if (histologyOverlay->isEnabled()) {
                        const HistologyOverlay::SelectionData selectionData(histologyOverlay->getSelectionData());
                        if (selectionData.m_selectedFile != NULL) {
                            displayedDataFiles.insert(selectionData.m_selectedFile);
                        }
                    }
                }
            }
        }
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
        {
            MediaOverlaySet* overlaySet = m_mediaModel->getMediaOverlaySet(tabIndex);
            if (overlaySet != NULL) {
                const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
                for (int32_t i = 0; i < numOverlays; i++) {
                    MediaOverlay* mediaOverlay = overlaySet->getOverlay(i);
                    if (mediaOverlay->isEnabled()) {
                        const MediaOverlay::SelectionData selectionData(mediaOverlay->getSelectionData());
                        if (selectionData.m_selectedMediaFile != NULL) {
                            displayedDataFiles.insert(selectionData.m_selectedMediaFile);
                        }
                    }
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        {
            ModelSurface* ms = getDisplayedSurfaceModel();
            displayedDataFiles.insert(ms->getSurface());
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
        {
            ModelSurfaceMontage* msm = getDisplayedSurfaceMontageModel();
            switch (msm->getSelectedConfigurationType(tabIndex)) {
                case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
                {
                    SurfaceMontageConfigurationCerebellar* smcc = msm->getCerebellarConfiguration(tabIndex);
                    if (smcc->isFirstSurfaceEnabled()) {
                        displayedDataFiles.insert(smcc->getFirstSurfaceSelectionModel()->getSurface());
                    }
                    if (smcc->isSecondSurfaceEnabled()) {
                        displayedDataFiles.insert(smcc->getSecondSurfaceSelectionModel()->getSurface());
                    }
                }
                    break;
                case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
                {
                    SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
                    if (smcc->isFirstSurfaceEnabled()) {
                        if (smcc->isLeftEnabled()) {
                            displayedDataFiles.insert(smcc->getLeftFirstSurfaceSelectionModel()->getSurface());
                        }
                        if (smcc->isRightEnabled()) {
                            displayedDataFiles.insert(smcc->getRightFirstSurfaceSelectionModel()->getSurface());
                        }
                    }
                    if (smcc->isSecondSurfaceEnabled()) {
                        if (smcc->isLeftEnabled()) {
                            displayedDataFiles.insert(smcc->getLeftSecondSurfaceSelectionModel()->getSurface());
                        }
                        if (smcc->isRightEnabled()) {
                            displayedDataFiles.insert(smcc->getRightSecondSurfaceSelectionModel()->getSurface());
                        }
                    }
                }
                    break;
                case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
                {
                    SurfaceMontageConfigurationFlatMaps* smcfm = msm->getFlatMapsConfiguration(tabIndex);
                    if (smcfm->isLeftEnabled()) {
                        displayedDataFiles.insert(smcfm->getLeftSurfaceSelectionModel()->getSurface());
                    }
                    if (smcfm->isRightEnabled()) {
                        displayedDataFiles.insert(smcfm->getRightSurfaceSelectionModel()->getSurface());
                    }
                    if (smcfm->isCerebellumEnabled()) {
                        displayedDataFiles.insert(smcfm->getCerebellumSurfaceSelectionModel()->getSurface());
                    }
                }
                    break;
                case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_CONFIGURATION:
                {
                    SurfaceMontageConfigurationHippocampus* smhc = msm->getHippocampusConfiguration(tabIndex);
                    if (smhc->isFirstSurfaceEnabled()) {
                        if (smhc->isLeftEnabled()) {
                            displayedDataFiles.insert(smhc->getLeftFirstSurfaceSelectionModel()->getSurface());
                        }
                        if (smhc->isRightEnabled()) {
                            displayedDataFiles.insert(smhc->getRightFirstSurfaceSelectionModel()->getSurface());
                        }
                    }
                    if (smhc->isSecondSurfaceEnabled()) {
                        if (smhc->isLeftEnabled()) {
                            displayedDataFiles.insert(smhc->getLeftSecondSurfaceSelectionModel()->getSurface());
                        }
                        if (smhc->isRightEnabled()) {
                            displayedDataFiles.insert(smhc->getRightSecondSurfaceSelectionModel()->getSurface());
                        }
                    }
                }
                    break;
                case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION:
                {
                    SurfaceMontageConfigurationHippocampusFlatMaps* smhfc = msm->getHippocampusFlatMapsConfiguration(tabIndex);
                    if (smhfc->isFirstSurfaceEnabled()) {
                        if (smhfc->isLeftEnabled()) {
                            displayedDataFiles.insert(smhfc->getLeftFirstSurfaceSelectionModel()->getSurface());
                        }
                        if (smhfc->isRightEnabled()) {
                            displayedDataFiles.insert(smhfc->getRightFirstSurfaceSelectionModel()->getSurface());
                        }
                    }
                    if (smhfc->isSecondSurfaceEnabled()) {
                        if (smhfc->isLeftEnabled()) {
                            displayedDataFiles.insert(smhfc->getLeftSecondSurfaceSelectionModel()->getSurface());
                        }
                        if (smhfc->isRightEnabled()) {
                            displayedDataFiles.insert(smhfc->getRightSecondSurfaceSelectionModel()->getSurface());
                        }
                    }
                }
                    break;
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
        {
            const int32_t numOutlines = m_volumeSurfaceOutlineSetModel->getNumberOfDislayedVolumeSurfaceOutlines();
            for (int32_t i = 0; i < numOutlines; i++) {
                VolumeSurfaceOutlineModel* model = m_volumeSurfaceOutlineSetModel->getVolumeSurfaceOutlineModel(i);
                if (model->isDisplayed()) {
                    displayedDataFiles.insert(model->getSurface());
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
        {
            ModelWholeBrain* wbm = getDisplayedWholeBrainModel();
            if (isWholeBrainLeftEnabled()) {
                displayedDataFiles.insert(wbm->getSelectedSurface(StructureEnum::CORTEX_LEFT, tabIndex));
            }
            if (isWholeBrainRightEnabled()) {
                displayedDataFiles.insert(wbm->getSelectedSurface(StructureEnum::CORTEX_RIGHT, tabIndex));
            }
            if (isWholeBrainCerebellumEnabled()) {
                displayedDataFiles.insert(wbm->getSelectedSurface(StructureEnum::CEREBELLUM, tabIndex));
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
        {
            const ChartTwoOverlaySet* overlaySet = m_chartTwoModel->getChartTwoOverlaySet(tabIndex);
            if (overlaySet != NULL) {
                const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
                for (int32_t i = 0; i < numOverlays; i++) {
                    const ChartTwoOverlay* chartOverlay = overlaySet->getOverlay(i);
                    if (chartOverlay->isEnabled()) {
                        CaretMappableDataFile* mapFile = NULL;
                        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                        int32_t selectedIndex = -1;
                        chartOverlay->getSelectionData(mapFile,
                                                       selectedIndexType,
                                                       selectedIndex);
                        
                        if (mapFile != NULL) {
                            displayedDataFiles.insert(mapFile);
                        }
                    }
                }
            }
        }
            break;
    }

    /*
     * Check overlay files
     */
    OverlaySet* overlaySet = model->getOverlaySet(tabIndex);
    const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        Overlay* overlay = overlaySet->getOverlay(i);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* overlayDataFile = NULL;
            int32_t mapIndex;
            overlay->getSelectionData(overlayDataFile,
                                      mapIndex);
            
            if (overlayDataFile != NULL) {
                switch (overlayDataFile->getDataFileType()) {
                    case DataFileTypeEnum::ANNOTATION:
                        break;
                    case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                        break;
                    case DataFileTypeEnum::BORDER:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_DENSE:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                    {
                        CiftiConnectivityMatrixDenseDynamicFile* dynFile = dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(overlayDataFile);
                        CaretAssert(dynFile);
                        displayedDataFiles.insert(dynFile->getParentBrainordinateDataSeriesFile());
                    }
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                    {
                        CiftiConnectivityMatrixParcelDynamicFile* dynFile = dynamic_cast<CiftiConnectivityMatrixParcelDynamicFile*>(overlayDataFile);
                        CaretAssert(dynFile);
                        displayedDataFiles.insert(dynFile->getParentParcelSeriesFile());
                    }
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
                    {
                        CiftiFiberTrajectoryMapFile* trajMapFile = dynamic_cast<CiftiFiberTrajectoryMapFile*>(overlayDataFile);
                        CaretAssert(trajMapFile);
                        displayedDataFiles.insert(trajMapFile->getMatchingFiberOrientationFile());
                    }
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                        break;
                    case DataFileTypeEnum::CZI_IMAGE_FILE:
                        break;
                    case DataFileTypeEnum::FOCI:
                        break;
                    case DataFileTypeEnum::IMAGE:
                        break;
                    case DataFileTypeEnum::HISTOLOGY_SLICES:
                        break;
                    case DataFileTypeEnum::LABEL:
                        break;
                    case DataFileTypeEnum::METRIC:
                        break;
                    case DataFileTypeEnum::METRIC_DYNAMIC:
                    {
                        MetricDynamicConnectivityFile* metricDynFile = dynamic_cast<MetricDynamicConnectivityFile*>(overlayDataFile);
                        CaretAssert(metricDynFile);
                        displayedDataFiles.insert(metricDynFile);
                    }
                        break;
                    case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
                        break;
                    case DataFileTypeEnum::PALETTE:
                        break;
                    case DataFileTypeEnum::RGBA:
                        break;
                    case DataFileTypeEnum::SAMPLES:
                        break;
                    case DataFileTypeEnum::SCENE:
                        break;
                    case DataFileTypeEnum::SPECIFICATION:
                        break;
                    case DataFileTypeEnum::SURFACE:
                        break;
                    case DataFileTypeEnum::UNKNOWN:
                        break;
                    case DataFileTypeEnum::VOLUME:
                        break;
                    case DataFileTypeEnum::VOLUME_DYNAMIC:
                        {
                            VolumeDynamicConnectivityFile* volDynFile = dynamic_cast<VolumeDynamicConnectivityFile*>(overlayDataFile);
                            CaretAssert(volDynFile);
                            displayedDataFiles.insert(volDynFile->getParentVolumeFile());
                        }
                        break;
                }
                
                displayedDataFiles.insert(overlayDataFile);
                
                if (overlayDataFile->isMappedWithPalette()) {
                    /*
                     * If mapped with palette, there may be thresholding with another file
                     * so need to include that file
                     */
                    if (overlayDataFile->getMapPaletteColorMapping(mapIndex)->getThresholdType() == PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE) {
                        CaretMappableDataFileAndMapSelectionModel* mapFileSelector = overlayDataFile->getMapThresholdFileSelectionModel(mapIndex);
                        CaretAssert(mapFileSelector);
                        CaretMappableDataFile* thresholdFile = mapFileSelector->getSelectedFile();
                        if (thresholdFile != NULL) {
                            displayedDataFiles.insert(thresholdFile);
                        }
                    }
                }
            }
        }
    }
    
    
    /*
     * Check border files
     */
    Brain* brain = model->getBrain();
    const DisplayPropertiesBorders* borderDisplayProperties = brain->getDisplayPropertiesBorders();
    const DisplayGroupEnum::Enum borderDisplayGroup = borderDisplayProperties->getDisplayGroupForTab(tabIndex);
    if (borderDisplayProperties->isDisplayed(borderDisplayGroup,
                                             tabIndex)) {
        const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
        for (int32_t i = 0; i < numBorderFiles; i++) {
            BorderFile* borderFile = brain->getBorderFile(i);
            const GroupAndNameHierarchyModel* classAndNameSelection = borderFile->getGroupAndNameHierarchyModel();
            if (classAndNameSelection->isSelected(borderDisplayGroup,
                                                  tabIndex)) {
                displayedDataFilesOut.push_back(borderFile);
            }
        }
    }

    /*
     * Check foci files
     */
    const DisplayPropertiesFoci* fociDisplayProperties = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum fociDisplayGroup = fociDisplayProperties->getDisplayGroupForTab(tabIndex);
    
    if (fociDisplayProperties->isDisplayed(fociDisplayGroup,
                                           tabIndex)) {
        const int32_t numFociFiles = brain->getNumberOfFociFiles();
        for (int32_t i = 0; i < numFociFiles; i++) {
            FociFile* fociFile = brain->getFociFile(i);
            
            const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
            if (classAndNameSelection->isSelected(fociDisplayGroup,
                                                  tabIndex)) {
                displayedDataFilesOut.push_back(fociFile);
            }
        }
    }
    
    /*
     * Might be NULLs so filter them out and return the results
     */
    for (std::set<CaretDataFile*>::iterator iter = displayedDataFiles.begin();
         iter != displayedDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        if (cdf != NULL) {
            displayedDataFilesOut.push_back(cdf);
        }
    }
}

ViewingTransformations*
BrowserTabContent::getViewingTransformation()
{
    if (isVolumeSlicesDisplayed()) {
        return m_volumeSliceViewingTransformation;
    }
    else if (isFlatSurfaceDisplayed()) {
        return m_flatSurfaceViewingTransformation;
    }
    else if (isCerebellumDisplayed()) {
        return m_cerebellumViewingTransformation;
    }
    else if (isChartTwoDisplayed()) {
        return m_chartTwoMatrixViewingTranformation;
    }
    else if (isHistologyDisplayed()) {
        return m_histologyViewingTransformation;
    }
    else if (isMediaDisplayed()) {
        return m_mediaViewingTransformation;
    }
    return m_viewingTransformation;
}

const ViewingTransformations*
BrowserTabContent::getViewingTransformation() const
{
    if (isVolumeSlicesDisplayed()) {
        return m_volumeSliceViewingTransformation;
    }
    else if (isFlatSurfaceDisplayed()) {
        return m_flatSurfaceViewingTransformation;
    }
    else if (isCerebellumDisplayed()) {
        return m_cerebellumViewingTransformation;
    }
    else if (isChartTwoDisplayed()) {
        return m_chartTwoMatrixViewingTranformation;
    }
    else if (isHistologyDisplayed()) {
        return m_histologyViewingTransformation;
    }
    else if (isMediaDisplayed()) {
        return m_mediaViewingTransformation;
    }
    return m_viewingTransformation;
}


/**
 * @return The viewing translation.
 */
const float*
BrowserTabContent::getTranslation() const
{
    return getViewingTransformation()->getTranslation();
}

/**
 * Get the viewing translation.
 *
 * @param translationOut
 *    Translation values output.
 */
void
BrowserTabContent::getTranslation(float translationOut[3]) const
{
    return getViewingTransformation()->getTranslation(translationOut);
}

/**
 * Set the viewing translation.
 *
 * @param translation
 *    New translation values.
 */
void
BrowserTabContent::setTranslation( const float translation[3])
{
    getViewingTransformation()->setTranslation(translation);
    updateYokedModelBrowserTabs();
}

/**
 * Set the viewing translation.
 *
 * @param translationX
 *    New translation X-value.
 * @param translationY
 *    New translation Y-value.
 * @param translationZ
 *    New translation Z-value.
 */
void
BrowserTabContent::setTranslation(const float translationX,
                                  const float translationY,
                                  const float translationZ)
{
    getViewingTransformation()->setTranslation(translationX,
                                               translationY,
                                               translationZ);
    updateYokedModelBrowserTabs();
}

/**
 * @return The viewing scaling.
 */
float
BrowserTabContent::getScaling() const
{
    return getViewingTransformation()->getScaling();
}

/**
 * Set the viewing scaling.
 * @param scaling
 *    New value for scaling.
 */
void
BrowserTabContent::setScaling(const float scaling)
{
    return getViewingTransformation()->setScaling(scaling);
    updateYokedModelBrowserTabs();
}

/**
 * @return The rotation matrix.
 */
Matrix4x4
BrowserTabContent::getRotationMatrix() const
{
    return getViewingTransformation()->getRotationMatrix();
}

/**
 * Set the rotation matrix.
 *
 * @param rotationMatrix
 *    The new rotation matrix.
 */
void
BrowserTabContent::setRotationMatrix(const Matrix4x4& rotationMatrix)
{
    getViewingTransformation()->setRotationMatrix(rotationMatrix);
    updateYokedModelBrowserTabs();
}

/**
 * @return The oblique volume rotation matrix.
 */
Matrix4x4
BrowserTabContent::getObliqueVolumeRotationMatrix() const
{
    return *m_obliqueVolumeRotationMatrix;
}

/**
 * Set the oblique rotation matrix.
 *
 * @param obliqueRotationMatrix
 *    The new oblique rotation matrix.
 */
void
BrowserTabContent::setObliqueVolumeRotationMatrix(const Matrix4x4& obliqueRotationMatrix)
{
    *m_obliqueVolumeRotationMatrix = obliqueRotationMatrix;
    
    updateBrainModelYokedBrowserTabs();
}

Matrix4x4
BrowserTabContent::getFlatRotationMatrix() const
{
    return getViewingTransformation()->getFlatRotationMatrix();
}

void
BrowserTabContent::setFlatRotationMatrix(const Matrix4x4& flatRotationMatrix)
{
    getViewingTransformation()->setFlatRotationMatrix(flatRotationMatrix);
    updateYokedModelBrowserTabs();
}

/**
 * @return MPR rotation X-angle
 */
float
BrowserTabContent::getMprRotationX() const
{
    CaretAssert(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR);
    return m_mprRotationX;
}

/**
 * @return MPR rotation Y-angle
 */
float
BrowserTabContent::getMprRotationY() const
{
    CaretAssert(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR);
    return m_mprRotationY;
}

/**
 * @return MPR rotation Z-angle
 */
float
BrowserTabContent::getMprRotationZ() const
{
    CaretAssert(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR);
    return m_mprRotationZ;
}

/**
 * Reset  MPR rotations to zero.
 */
void
BrowserTabContent::resetMprRotations()
{
    m_mprRotationX = 0.0;
    m_mprRotationY = 0.0;
    m_mprRotationZ = 0.0;

    m_mprThreeRotationSeparateQuaternion = QQuaternion();
    m_mprThreeAxialSeparateRotationQuaternion = QQuaternion();
    m_mprThreeCoronalSeparateRotationQuaternion = QQuaternion();
    m_mprThreeParasagittalSeparateRotationQuaternion = QQuaternion();

    m_mprThreeAxialInverseRotationQuaternion = QQuaternion();
    m_mprThreeCoronalInverseRotationQuaternion = QQuaternion();
    m_mprThreeParasagittalInverseRotationQuaternion = QQuaternion();
}

/**
 * @return The MPR rotation quaternion in a matrix
 */
Matrix4x4
BrowserTabContent::getMprThreeRotationMatrix() const
{
    CaretAssert(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE);
    QMatrix3x3 mat33(m_mprThreeRotationSeparateQuaternion.toRotationMatrix());
    Matrix4x4 matrix;
    for (int32_t i = 0; i < 3; i++) {
        for (int32_t j = 0; j < 3; j++) {
            matrix.setMatrixElement(i, j, mat33(i, j));
        }
    }
    return matrix;
}

/**
 * @return Quaterion converted from the given matrix
 * @return matrix
 *    The matrix
 */
QQuaternion
BrowserTabContent::matrixToQuaternion(const Matrix4x4& matrix)
{
    QMatrix3x3 qm;
    for (int32_t i = 0; i < 3; i++) {
        for (int32_t j = 0; j < 3; j++) {
            qm(i, j) = matrix.getMatrixElement(i, j);
        }
    }
    QQuaternion q(QQuaternion::fromRotationMatrix(qm));
    return q;
}

/**
 * @return Matrix converted from the given quaternion
 * @param quaternion
 */
Matrix4x4
BrowserTabContent::quaternionToMatrix(const QQuaternion& quaternion)
{
    QMatrix3x3 qm(quaternion.toRotationMatrix());
    
    Matrix4x4 m;
    for (int32_t i = 0; i < 3; i++) {
        for (int32_t j = 0; j < 3; j++) {
            m.setMatrixElement(i, j, qm(i, j));
        }
    }
    return m;
}

/**
 * @return a 4-element array containing the quaternion's vector and scalar (x, y, z, scalar)
 * @param quaternion
 *    the quaternion
 */
std::array<float, 4>
BrowserTabContent::quaternionToArrayXYZS(const QQuaternion& quaternion)
{
    const std::array<float, 4> a {
        quaternion.x(),
        quaternion.y(),
        quaternion.z(),
        quaternion.scalar()
    };
    
    return a;
}

/**
 * @return a 4-element array containing the quaternion's scalar vector and (scalar, x, y, z)
 * @param quaternion
 *    the quaternion
 */
void
BrowserTabContent::quaternionToArraySXYZ(const QQuaternion& quaternion,
                                         float arraySXYZ[4])

{
    arraySXYZ[0] = quaternion.scalar();
    arraySXYZ[1] = quaternion.x();
    arraySXYZ[2] = quaternion.y();
    arraySXYZ[3] = quaternion.z();
}

/**
 * @return a 4-element array containing the quaternion's scalar and vector (scalar, x, y, z)
 * @param quaternion
 *    the quaternion
 */
std::array<float, 4>
BrowserTabContent::quaternionToArraySXYZ(const QQuaternion& quaternion)
{
    const std::array<float, 4> a {
        quaternion.scalar(),
        quaternion.x(),
        quaternion.y(),
        quaternion.z()
    };
    
    return a;
}

/**
 * A quaternion converted from an array containing the vector and scalar.
 * @param arrayXYZS
 *   The 4-element array with x, y, z, scalar
 */
QQuaternion
BrowserTabContent::arrayXYZSToQuaternion(const std::array<float, 4>& arrayXYZS)
{
    QQuaternion q;
    q.setVector(arrayXYZS[0], arrayXYZS[1], arrayXYZS[2]);
    q.setScalar(arrayXYZS[3]);
    return q;
}

/**
 * A quaternion converted from an array containing the scalar and vector.
 * @param arraySXYZ
 *   The 4-element array with scalar, x, y, z
 */
QQuaternion
BrowserTabContent::arraySXYZToQuaternion(const std::array<float, 4>& arraySXYZ)
{
    QQuaternion q;
    q.setScalar(arraySXYZ[0]);
    q.setVector(arraySXYZ[1], arraySXYZ[2], arraySXYZ[3]);
    return q;
}

QQuaternion
BrowserTabContent::arraySXYZToQuaternion(const float arraySXYZ[4])
{
    QQuaternion q;
    q.setScalar(arraySXYZ[0]);
    q.setVector(arraySXYZ[1], arraySXYZ[2], arraySXYZ[3]);
    return q;
}

/*
 * Convert a quaternion to an array
 * @param quaternion
 *   The quaternion
 * @param arraySxyzOut
 *   Output array with x, y, z, scalar
 */
void
BrowserTabContent::quaternionToArrayXYZS(const QQuaternion& quaternion,
                                         float arrayXYZS[4])
{
    arrayXYZS[0] = quaternion.x();
    arrayXYZS[1] = quaternion.y();
    arrayXYZS[2] = quaternion.z();
    arrayXYZS[3] = quaternion.scalar();
}

/**
 * Convert an array to a quaternion
 * @param array
 */
QQuaternion
BrowserTabContent::arrayXYZSToQuaternion(const float arrayXYZS[4])
{
    QQuaternion q;
    q.setVector(arrayXYZS[0], arrayXYZS[1], arrayXYZS[2]);
    q.setScalar(arrayXYZS[3]);
    return q;
}

/**
 * @return The MPR rotation quaternion in a matrix for the given slice plane
 * @param slicePlane
 *    The slice plane
 */
Matrix4x4
BrowserTabContent::getMprThreeRotationMatrixForSlicePlane(const VolumeSliceViewPlaneEnum::Enum slicePlane)
{
    CaretAssert(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE);
    QQuaternion q;
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            q = m_mprThreeAxialSeparateRotationQuaternion;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            q = m_mprThreeCoronalSeparateRotationQuaternion;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            q = m_mprThreeParasagittalSeparateRotationQuaternion;
            break;
    }
    return quaternionToMatrix(q);
}

/**
 * @return Matrix for the given slice plane for MPR rotation
 * @param slicePlane
 *    The slice plane
 */
Matrix4x4
BrowserTabContent::getMprRotationMatrix4x4ForSlicePlane(const ModelTypeEnum::Enum modelType,
                                                        const VolumeSliceViewPlaneEnum::Enum slicePlane) const
{
    CaretAssert(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR);
    
    /*
     * Correct rotation of volume so that it matches crosshairs
     * Crosshairs and volume were rotating opposite direction
     * in volume slice mode.
     */
    static bool fixFlipVolumeSliceRotationFlag(true);
    
    Matrix4x4 matrixOut;

    bool wholeBrainFlag(false);
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
        case ModelTypeEnum::MODEL_TYPE_CHART:
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
        case ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            CaretAssert(0);
            return matrixOut;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            if (fixFlipVolumeSliceRotationFlag) {
                wholeBrainFlag = true;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            wholeBrainFlag = true;
            break;
    }
    
    const float noRotationAngle(0.0);
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            matrixOut.setRotation(-m_mprRotationX, -m_mprRotationY, m_mprRotationZ);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            if (wholeBrainFlag) {
                matrixOut.setRotation(-m_mprRotationX, -m_mprRotationY, noRotationAngle);
            }
            else {
                matrixOut.setRotation(m_mprRotationX, m_mprRotationY, noRotationAngle);
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            if (wholeBrainFlag) {
                matrixOut.setRotation(-m_mprRotationX, noRotationAngle, m_mprRotationZ);
            }
            else {
                matrixOut.setRotation(m_mprRotationX, noRotationAngle, -m_mprRotationZ);
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            if (wholeBrainFlag) {
                matrixOut.setRotation(noRotationAngle, -m_mprRotationY, m_mprRotationZ);
            }
            else {
                matrixOut.setRotation(noRotationAngle, m_mprRotationY, -m_mprRotationZ);
            }
            break;
    }
    return matrixOut;
}

/**
 * @return Vector for rotating in the given slice plane
 * @param slicePlane
 *    The slice plane
 */
Vector3D
BrowserTabContent::getMprThreeRotationVectorForSlicePlane(const VolumeSliceViewPlaneEnum::Enum slicePlane) const
{
    Vector3D vectorOut;
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            vectorOut = m_mprThreeAxialRotationVector;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            vectorOut = m_mprThreeCoronalRotationVector;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            vectorOut = m_mprThreeParasagittalRotationVector;
            break;
    }
    return vectorOut;
}

/**
 * Set the vector for rotating in the given slice plane
 * @param slicePlane
 *    The slice plane
 * @param rotationVectoir
 *    Vector for rotation
 */
void
BrowserTabContent::setMprThreeRotationVectorForSlicePlane(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                          const Vector3D& rotationVector)
{
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            m_mprThreeAxialRotationVector = rotationVector;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            m_mprThreeCoronalRotationVector = rotationVector;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            m_mprThreeParasagittalRotationVector = rotationVector;
            break;
    }
}

/**
 * Get the offset for the right flat map offset.
 *
 * @param offsetX
 *    Output with X-offset.
 * @param offsetY
 *    Output with Y-offset.
 */
void
BrowserTabContent::getRightCortexFlatMapOffset(float& offsetX,
                                               float& offsetY) const
{
    getViewingTransformation()->getRightCortexFlatMapOffset(offsetX, offsetY);
}

/**
 * Set the offset for the right flat map offset.
 *
 * @param offsetX
 *    X-offset.
 * @param offsetY
 *    Y-offset.
 */
void
BrowserTabContent::setRightCortexFlatMapOffset(const float offsetX,
                                               const float offsetY)
{
    getViewingTransformation()->setRightCortexFlatMapOffset(offsetX, offsetY);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return The right cortex flat map zoom factor.
 */
float
BrowserTabContent::getRightCortexFlatMapZoomFactor() const
{
    return getViewingTransformation()->getRightCortexFlatMapZoomFactor();
}

/**
 * Set the right cortex flat map zoom factor.
 *
 * @param zoomFactor
 *     The zoom factor.
 */
void
BrowserTabContent::setRightCortexFlatMapZoomFactor(const float zoomFactor)
{
    getViewingTransformation()->setRightCortexFlatMapZoomFactor(zoomFactor);
}

/**
 * Reset the view to the default view.
 */
void
BrowserTabContent::resetView()
{
    getViewingTransformation()->resetView();
    if (isVolumeSlicesDisplayed()
        || isWholeBrainDisplayed()) {
        m_obliqueVolumeRotationMatrix->identity();
        resetMprRotations();
    }
    updateYokedModelBrowserTabs();
    
    EventResetView resetViewEvent(getTabNumber());
    EventManager::get()->sendEvent(resetViewEvent.getPointer());
}

/**
 * Set to a right side view.
 */
void
BrowserTabContent::rightView()
{
    if (isVolumeSlicesDisplayed()) {
        return;
    }
    getViewingTransformation()->rightView();
    updateYokedModelBrowserTabs();
}

/**
 * set to a left side view.
 */
void
BrowserTabContent::leftView()
{
    if (isVolumeSlicesDisplayed()) {
        return;
    }
    getViewingTransformation()->leftView();
    updateYokedModelBrowserTabs();
}

/**
 * set to a anterior view.
 */
void
BrowserTabContent::anteriorView()
{
    if (isVolumeSlicesDisplayed()) {
        return;
    }
    getViewingTransformation()->anteriorView();
    updateYokedModelBrowserTabs();
}

/**
 * set to a posterior view.
 */
void
BrowserTabContent::posteriorView()
{
    if (isVolumeSlicesDisplayed()) {
        return;
    }
    getViewingTransformation()->posteriorView();
    updateYokedModelBrowserTabs();
}

/**
 * set to a dorsal view.
 */
void
BrowserTabContent::dorsalView()
{
    if (isVolumeSlicesDisplayed()) {
        return;
    }
    getViewingTransformation()->dorsalView();
    updateYokedModelBrowserTabs();
}

/**
 * set to a ventral view.
 */
void
BrowserTabContent::ventralView()
{
    if (isVolumeSlicesDisplayed()) {
        return;
    }
    getViewingTransformation()->ventralView();
    updateYokedModelBrowserTabs();
}

/**
 * Apply volume slice increment while dragging mouse
 *
 * @param viewportContent
 *    Content of viewport
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    Y coordinate of where mouse was pressed.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseVolumeSliceIncrement(BrainOpenGLViewportContent* viewportContent,
                                                  const int32_t mousePressX,
                                                  const int32_t mousePressY,
                                                  const int32_t mouseDY)
{
    VolumeSliceViewPlaneEnum::Enum sliceViewPlane = getVolumeSliceViewPlane();
    bool incrementFlag(false);
    if (isVolumeSlicesDisplayed()) {
        switch (getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                incrementFlag = true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                incrementFlag = true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                incrementFlag = true;
                break;
        }
    }
    
    if ( ! incrementFlag) {
        return;
    }
    if (mouseDY == 0) {
        return;
    }
    
    const int32_t sliceDelta(mouseDY);
    const int32_t tabIndex = viewportContent->getTabIndex();
    VolumeMappableInterface* underlayVolume(NULL);
    ModelVolume* volumeModel = getDisplayedVolumeModel();
    if (volumeModel != NULL) {
        underlayVolume = volumeModel->getUnderlayVolumeFile(tabIndex);
    }
    
    if (sliceViewPlane == VolumeSliceViewPlaneEnum::ALL) {
        int viewport[4];
        viewportContent->getModelViewport(viewport);
        int sliceViewport[4] = {
            viewport[0],
            viewport[1],
            viewport[2],
            viewport[3]
        };
        sliceViewPlane = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                        getVolumeSlicePlanesAllViewLayout(),
                                                                                        mousePressX,
                                                                                        mousePressY,
                                                                                        sliceViewport);
    }
    
    if (isVolumeMprOldDisplayed()) {
        if (sliceViewPlane != VolumeSliceViewPlaneEnum::ALL) {
            Vector3D sliceVector(0.0, 0.0, 0.0);
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    sliceVector[2] = 1.0;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    sliceVector[1] = 1.0;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    sliceVector[0] = 1.0;
                    break;
            }
            
            const Matrix4x4 rotMatrix(getMprRotationMatrix4x4ForSlicePlane(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                                                                           sliceViewPlane));
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    rotMatrix.multiplyPoint3(sliceVector);
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    rotMatrix.multiplyPoint3(sliceVector);
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    rotMatrix.multiplyPoint3(sliceVector);
                    break;
            }
            
            const float axialDelta(std::round(sliceVector[2] * sliceDelta));
            const float coronalDelta(std::round(sliceVector[1] * sliceDelta));
            const float paraDelta(std::round(sliceVector[0] * sliceDelta));
            
            setVolumeSliceCoordinateAxial(getVolumeSliceCoordinateAxial() + axialDelta);
            setVolumeSliceCoordinateCoronal(getVolumeSliceCoordinateCoronal() + coronalDelta);
            setVolumeSliceCoordinateParasagittal(getVolumeSliceCoordinateParasagittal() + paraDelta);
        }
    }
    else if (isVolumeMprThreeDisplayed()) {
        if (sliceViewPlane != VolumeSliceViewPlaneEnum::ALL) {
            const Vector3D sliceVector(getMprThreeRotationVectorForSlicePlane(sliceViewPlane));
                
            const float axialDelta(std::round(sliceVector[2] * sliceDelta));
            const float coronalDelta(std::round(sliceVector[1] * sliceDelta));
            const float paraDelta(std::round(sliceVector[0] * sliceDelta));
            
            setVolumeSliceCoordinateAxial(getVolumeSliceCoordinateAxial() + axialDelta);
            setVolumeSliceCoordinateCoronal(getVolumeSliceCoordinateCoronal() + coronalDelta);
            setVolumeSliceCoordinateParasagittal(getVolumeSliceCoordinateParasagittal() + paraDelta);
        }
    }

    else {
        /*
         * Note: Functions that set slice indices will prevent
         * invalid slice indices
         */
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                setVolumeSliceIndexAxial(underlayVolume,
                                   (getVolumeSliceIndexAxial(underlayVolume) + sliceDelta));
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                setVolumeSliceIndexCoronal(underlayVolume,
                                     (getVolumeSliceIndexCoronal(underlayVolume) + sliceDelta));
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                setVolumeSliceIndexParasagittal(underlayVolume,
                                          (getVolumeSliceIndexParasagittal(underlayVolume) + sliceDelta));
                break;
        }
    }

    updateYokedModelBrowserTabs();
}


/**
 * Apply mouse rotation to the displayed model.
 *
 * @param viewportContent
 *    Content of viewport
 * @param mprCrosshairAxis
 *    The MPR crosshair that indicates the type of rotation
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    Y coordinate of where mouse was pressed.
 * @param mouseX
 *    X coordinate of mouse.
 * @param mouseY
 *    Y coordinate of mouse.
 * @param mouseDeltaX
 *    Change in mouse X coordinate.
 * @param mouseDeltaY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseRotation(BrainOpenGLViewportContent* viewportContent,
                                      const SelectionItemVolumeMprCrosshair::Axis mprCrosshairAxis,
                                      const int32_t mousePressX,
                                      const int32_t mousePressY,
                                      const int32_t mouseX,
                                      const int32_t mouseY,
                                      const int32_t mouseDeltaX,
                                      const int32_t mouseDeltaY)
{
    if (isVolumeSlicesDisplayed()) {
        switch (getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                if (viewportContent == NULL) {
                    /*
                     * When no viewport content is available, apply 'ALL' rotation
                     */
                    Matrix4x4 rotationMatrix = getObliqueVolumeRotationMatrix();
                    rotationMatrix.rotateX(-mouseDeltaY);
                    rotationMatrix.rotateY(mouseDeltaX);
                    setObliqueVolumeRotationMatrix(rotationMatrix);
                }
                else {
                    int viewport[4];
                    viewportContent->getModelViewport(viewport);
                    VolumeSliceViewPlaneEnum::Enum slicePlane = this->getVolumeSliceViewPlane();
                    int sliceViewport[4] = {
                        viewport[0],
                        viewport[1],
                        viewport[2],
                        viewport[3]
                    };
                    if (slicePlane == VolumeSliceViewPlaneEnum::ALL) {
                        slicePlane = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                                        getVolumeSlicePlanesAllViewLayout(),
                                                                                                        mousePressX,
                                                                                                        mousePressY,
                                                                                                        sliceViewport);
                    }
                    
                    Matrix4x4 rotationMatrix = getObliqueVolumeRotationMatrix();
                    
                    if (slicePlane == VolumeSliceViewPlaneEnum::ALL) {
                        rotationMatrix.rotateX(-mouseDeltaY);
                        rotationMatrix.rotateY(mouseDeltaX);
                    }
                    else {
                        if ((mouseDeltaX != 0)
                            || (mouseDeltaY != 0)) {
                            
                            const int previousMouseX = mouseX - mouseDeltaX;
                            const int previousMouseY = mouseY - mouseDeltaY;
                            
                            /*
                             * Need to account for the quadrants!!!!
                             */
                            const float viewportCenter[3] = {
                                (float)(sliceViewport[0] + sliceViewport[2] / 2),
                                ((float)sliceViewport[1] + sliceViewport[3] / 2),
                                0.0
                            };
                            
                            const float oldPos[3] = {
                                (float)previousMouseX,
                                (float)previousMouseY,
                                0.0
                            };
                            
                            const float newPos[3] = {
                                (float)mouseX,
                                (float)mouseY,
                                0.0
                            };
                            
                            /*
                             * Compute normal vector from viewport center to
                             * old mouse position to new mouse position.
                             * If normal-Z is positive, mouse has been moved
                             * in a counter clockwise motion relative to center.
                             * If normal-Z is negative, mouse has moved clockwise.
                             */
                            float normalDirection[3];
                            MathFunctions::normalVectorDirection(viewportCenter,
                                                                 oldPos,
                                                                 newPos,
                                                                 normalDirection);
                            bool isClockwise = false;
                            bool isCounterClockwise = false;
                            if (normalDirection[2] > 0.0) {
                                isCounterClockwise = true;
                            }
                            else if (normalDirection[2] < 0.0) {
                                isClockwise = true;
                            }
                            
                            if (isClockwise
                                || isCounterClockwise) {
                                float mouseDelta = std::sqrt(static_cast<float>((mouseDeltaX * mouseDeltaX)
                                                                                + (mouseDeltaY * mouseDeltaY)));
                                
                                switch (slicePlane) {
                                    case VolumeSliceViewPlaneEnum::ALL:
                                    {
                                        CaretAssert(0);
                                    }
                                        break;
                                    case VolumeSliceViewPlaneEnum::AXIAL:
                                    {
                                        Matrix4x4 rotation;
                                        if (isClockwise) {
                                            rotation.rotateZ(mouseDelta);
                                        }
                                        else if (isCounterClockwise) {
                                            rotation.rotateZ(-mouseDelta);
                                        }
                                        rotationMatrix.premultiply(rotation);
                                    }
                                        break;
                                    case VolumeSliceViewPlaneEnum::CORONAL:
                                    {
                                        Matrix4x4 rotation;
                                        if (isClockwise) {
                                            rotation.rotateY(-mouseDelta);
                                        }
                                        else if (isCounterClockwise) {
                                            rotation.rotateY(mouseDelta);
                                        }
                                        rotationMatrix.premultiply(rotation);
                                    }
                                        break;
                                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                    {
                                        Matrix4x4 rotation;
                                        if (isClockwise) {
                                            rotation.rotateX(-mouseDelta);
                                        }
                                        else if (isCounterClockwise) {
                                            rotation.rotateX(mouseDelta);
                                        }
                                        rotationMatrix.premultiply(rotation);
                                    }
                                        break;
                                }
                            }
                        }
                    }
                    
                    setObliqueVolumeRotationMatrix(rotationMatrix);
                }
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                if (viewportContent != NULL) {
                    bool radiologicalFlag(false);
                    switch (getVolumeMprOrientationMode()) {
                        case VolumeMprOrientationModeEnum::RADIOLOGICAL:
                            radiologicalFlag = true;
                            break;
                        case VolumeMprOrientationModeEnum::NEUROLOGICAL:
                            radiologicalFlag = false;
                            break;
                    }

                    int viewport[4];
                    viewportContent->getModelViewport(viewport);
                    VolumeSliceViewPlaneEnum::Enum slicePlane = this->getVolumeSliceViewPlane();
                    int sliceViewport[4] = {
                        viewport[0],
                        viewport[1],
                        viewport[2],
                        viewport[3]
                    };
                    
                    /*
                     * Only allow rotation if in an ALL view.
                     * Do not rotate if single plane view.
                     */
                    bool allowRotationFlag(false);
                    if (slicePlane == VolumeSliceViewPlaneEnum::ALL) {
                        slicePlane = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                                        getVolumeSlicePlanesAllViewLayout(),
                                                                                                        mousePressX,
                                                                                                        mousePressY,
                                                                                                        sliceViewport);
                        allowRotationFlag = true;
                    }
                    else {
                        /*
                         * If true, rotation is allowed when viewing a single axis
                         * slice view (just one of axial, coronal, parasagittal).
                         * But user only sees crosshair
                         */
                        allowRotationFlag = true;
                    }
                    
                    if (allowRotationFlag
                        && (slicePlane != VolumeSliceViewPlaneEnum::ALL)) {
                        if ((mouseDeltaX != 0)
                            || (mouseDeltaY != 0)) {
                            
                            const int previousMouseX = mouseX - mouseDeltaX;
                            const int previousMouseY = mouseY - mouseDeltaY;
                            
                            /*
                             * Need to account for the quadrants!!!!
                             */
                            const float viewportCenter[3] = {
                                (float)(sliceViewport[0] + sliceViewport[2] / 2),
                                ((float)sliceViewport[1] + sliceViewport[3] / 2),
                                0.0
                            };
                            
                            const float oldPos[3] = {
                                (float)previousMouseX,
                                (float)previousMouseY,
                                0.0
                            };
                            
                            const float newPos[3] = {
                                (float)mouseX,
                                (float)mouseY,
                                0.0
                            };
                            
                            /*
                             * Compute normal vector from viewport center to
                             * old mouse position to new mouse position.
                             * If normal-Z is positive, mouse has been moved
                             * in a counter clockwise motion relative to center.
                             * If normal-Z is negative, mouse has moved clockwise.
                             */
                            float normalDirection[3];
                            MathFunctions::normalVectorDirection(viewportCenter,
                                                                 oldPos,
                                                                 newPos,
                                                                 normalDirection);
                            bool isClockwise = false;
                            bool isCounterClockwise = false;
                            if (normalDirection[2] > 0.0) {
                                isCounterClockwise = true;
                            }
                            else if (normalDirection[2] < 0.0) {
                                isClockwise = true;
                            }
                            
                            if (isClockwise
                                || isCounterClockwise) {
                                float mouseDelta = std::sqrt(static_cast<float>((mouseDeltaX * mouseDeltaX)
                                                                                + (mouseDeltaY * mouseDeltaY)));
                                
                                if (isVolumeMprThreeDisplayed()) {
                                    applyMouseRotationMprThree(viewportContent,
                                                               mprCrosshairAxis,
                                                               viewport,
                                                               Vector3D(mousePressX, mousePressY, 0.0),
                                                               Vector3D(mouseX, mouseY, 0.0),
                                                               Vector3D(previousMouseX, previousMouseY, 0.0));
                                }
                                else {
                                    switch (slicePlane) {
                                        case VolumeSliceViewPlaneEnum::ALL:
                                        {
                                            CaretAssert(0);
                                        }
                                            break;
                                        case VolumeSliceViewPlaneEnum::AXIAL:
                                        {
                                            const float mprRotZ(isClockwise
                                                                ? -mouseDelta
                                                                : mouseDelta);
                                            if (radiologicalFlag) {
                                                m_mprRotationZ -= mprRotZ;
                                            }
                                            else {
                                                m_mprRotationZ += mprRotZ;
                                            }
                                        }
                                            break;
                                        case VolumeSliceViewPlaneEnum::CORONAL:
                                        {
                                            const float mprRotY(isClockwise
                                                                ? - mouseDelta
                                                                : mouseDelta);
                                            if (radiologicalFlag) {
                                                m_mprRotationY -= mprRotY;
                                            }
                                            else {
                                                m_mprRotationY += mprRotY;
                                            }
                                        }
                                            break;
                                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                        {
                                            const float mprRotX(isClockwise
                                                                ? -mouseDelta
                                                                : mouseDelta);
                                            m_mprRotationX += mprRotX;
                                        }
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
        }
    }
    else if (isChartOneDisplayed()
             || isChartTwoDisplayed()) {
        /* no rotation for chart */
    }
    else if (isMediaDisplayed()) {
        /* no rotation for media */
    }
    else if (isCerebellumDisplayed()) {
        const float screenDX = mouseDeltaX;
        const float screenDY = mouseDeltaY;
        
        float rotateDX = 0.0;
        float rotateDY = 0.0;
        float rotateDZ = 0.0;
        float rotateFlat = 0.0;
        bool flatFlag(false);
        
        ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
        if (montageModel != NULL) {
            std::vector<const SurfaceMontageViewport*> montageViewports;
            montageModel->getSurfaceMontageViewportsForTransformation(getTabNumber(),
                                                                      montageViewports);
            
            bool foundMontageViewportFlag = false;
            
            const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
            for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                const SurfaceMontageViewport* smv = montageViewports[ivp];
                
                if (smv->isInside(mousePressX,
                                  mousePressY)) {
                    switch (smv->getProjectionViewType()) {
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
                            rotateDX =  screenDY;
                            rotateDZ =  screenDX;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
                            rotateDX = -screenDY;
                            rotateDY =  screenDX;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
                            rotateDX =  -screenDY;
                            rotateDZ =   screenDX;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
                            rotateDX = -screenDY;
                            rotateDY = -screenDX;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_DENTATE_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_DENTATE_SURFACE:
                            break;
                    }
                }
                if (foundMontageViewportFlag) {
                    break;
                }
            }
        }
        else {
            ModelSurface* modelSurface = getDisplayedSurfaceModel();
            if (modelSurface != NULL) {
                if (isFlatSurfaceDisplayed()) {
                    flatFlag   = true;
                    rotateFlat = -screenDY;
                }
                else {
                    rotateDX = -screenDY;
                    rotateDY =  screenDX;
                }
            }
        }
        
        if (flatFlag) {
            if (rotateFlat != 0.0) {
                int viewport[4];
                viewportContent->getModelViewport(viewport);
                if ((viewport[2] > 0)
                    && (viewport[3] > 0)) {
                    if ((mouseDeltaX != 0)
                        || (mouseDeltaY != 0)) {
                        
                        const int previousMouseX = mouseX - mouseDeltaX;
                        const int previousMouseY = mouseY - mouseDeltaY;
                        
                        /*
                         * Need to account for the quadrants!!!!
                         */
                        const float viewportCenter[3] = {
                            (float)(viewport[0] + viewport[2] / 2),
                            ((float)viewport[1] + viewport[3] / 2),
                            0.0
                        };
                        
                        const float oldPos[3] = {
                            (float)previousMouseX,
                            (float)previousMouseY,
                            0.0
                        };
                        
                        const float newPos[3] = {
                            (float)mouseX,
                            (float)mouseY,
                            0.0
                        };
                        
                        /*
                         * Compute normal vector from viewport center to
                         * old mouse position to new mouse position.
                         * If normal-Z is positive, mouse has been moved
                         * in a counter clockwise motion relative to center.
                         * If normal-Z is negative, mouse has moved clockwise.
                         */
                        float normalDirection[3];
                        MathFunctions::normalVectorDirection(viewportCenter,
                                                             oldPos,
                                                             newPos,
                                                             normalDirection);
                        rotateFlat = std::fabs(rotateFlat);
                        if (normalDirection[2] > 0.0) {
                            /* mouse movied counter-clockwise */
                        }
                        else if (normalDirection[2] < 0.0) {
                            /* mouse movied clockwise */
                            rotateFlat = -rotateFlat;
                        }
                    }
                }
                ViewingTransformations* viewingTransform = getViewingTransformation();
                Matrix4x4 flatRotationMatrix = viewingTransform->getFlatRotationMatrix();
                flatRotationMatrix.rotateZ(rotateFlat);
                viewingTransform->setFlatRotationMatrix(flatRotationMatrix);
            }
        }
        else {
            Matrix4x4 rotationMatrix = m_cerebellumViewingTransformation->getRotationMatrix();
            rotationMatrix.rotateX(rotateDX);
            rotationMatrix.rotateY(rotateDY);
            rotationMatrix.rotateZ(rotateDZ);
            m_cerebellumViewingTransformation->setRotationMatrix(rotationMatrix);
        }
    }
    else {
        ViewingTransformations* viewingTransform = getViewingTransformation();
        if (getProjectionViewType() == ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL) {
            Matrix4x4 rotationMatrix = viewingTransform->getRotationMatrix();
            rotationMatrix.rotateX(-mouseDeltaY);
            rotationMatrix.rotateY(-mouseDeltaX);
            viewingTransform->setRotationMatrix(rotationMatrix);
        }
        else {
            float dx = mouseDeltaX;
            float dy = mouseDeltaY;
            
            StructureEnum::Enum flatStructure(StructureEnum::INVALID);
            float flatRotate(0.0);
            int32_t flatViewport[4] { -1, -1, -1, -1 };
            
            ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
            if (montageModel != NULL) {
                std::vector<const SurfaceMontageViewport*> montageViewports;
                montageModel->getSurfaceMontageViewportsForTransformation(getTabNumber(),
                                                                          montageViewports);
                
                bool isValid = false;
                bool isFlat  = false;
                bool isLeft = false;
                bool isLateral = true;
                const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
                for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                    const SurfaceMontageViewport* smv = montageViewports[ivp];
                    if (smv->isInside(mousePressX,
                                     mousePressY)) {
                        switch (smv->getProjectionViewType()) {
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
                                isFlat = true;
                                smv->getViewport(flatViewport);
                                flatStructure = StructureEnum::CEREBELLUM;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                                isLeft = true;
                                isLateral = true;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                                isLeft = true;
                                isLateral = false;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_DENTATE_SURFACE:
                                isLeft = true;
                                isFlat = true;
                                smv->getViewport(flatViewport);
                                flatStructure = StructureEnum::CORTEX_LEFT;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                                isLeft = false;
                                isLateral = true;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                                isLeft = false;
                                isLateral = false;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_DENTATE_SURFACE:
                                isLeft = false;
                                isFlat = true;
                                smv->getViewport(flatViewport);
                                flatStructure = StructureEnum::CORTEX_RIGHT;
                                break;
                        }
                        isValid = true;
                    }
                    
                    if (isValid) {
                        break;
                    }
                }
                
                if (isFlat) {
                    /*
                     * Special flat map rotation
                     */
                    flatRotate = dy;
                    
                    /*
                     * No 3D rotation.
                     */
                    dx = 0.0;
                    dy = 0.0;
                }
                else if (isValid) {
                    if (isLeft == false) {
                        dx = -dx;
                    }
                    if (isLateral == false) {
                        dy = -dy;
                    }
                }
            }
            
            ModelSurface* modelSurface = getDisplayedSurfaceModel();
            if (modelSurface != NULL) {
                const Surface* surface = modelSurface->getSurface();
                if (surface != NULL) {
                    switch (surface->getSurfaceType()) {
                        case SurfaceTypeEnum::ANATOMICAL:
                        case SurfaceTypeEnum::ELLIPSOID:
                        case SurfaceTypeEnum::HULL:
                        case SurfaceTypeEnum::INFLATED:
                        case SurfaceTypeEnum::RECONSTRUCTION:
                        case SurfaceTypeEnum::SEMI_SPHERICAL:
                        case SurfaceTypeEnum::SPHERICAL:
                        case SurfaceTypeEnum::UNKNOWN:
                        case SurfaceTypeEnum::VERY_INFLATED:
                            break;
                        case SurfaceTypeEnum::FLAT:
                            viewportContent->getModelViewport(flatViewport);
                            flatRotate = dy;
                            flatStructure = surface->getStructure();
                            break;
                    }
                }
            }
            
            Matrix4x4 rotationMatrix = viewingTransform->getRotationMatrix();
            rotationMatrix.rotateX(-dy);
            rotationMatrix.rotateY(dx);
            viewingTransform->setRotationMatrix(rotationMatrix);
            
            if (flatRotate != 0.0) {
                if ((flatViewport[2] > 0)
                    && (flatViewport[3] > 0)) {
                    if ((mouseDeltaX != 0)
                        || (mouseDeltaY != 0)) {
                        
                        const int previousMouseX = mouseX - mouseDeltaX;
                        const int previousMouseY = mouseY - mouseDeltaY;
                        
                        /*
                         * Need to account for the quadrants!!!!
                         */
                        const float viewportCenter[3] = {
                            (float)(flatViewport[0] + flatViewport[2] / 2),
                            ((float)flatViewport[1] + flatViewport[3] / 2),
                            0.0
                        };
                        
                        const float oldPos[3] = {
                            (float)previousMouseX,
                            (float)previousMouseY,
                            0.0
                        };
                        
                        const float newPos[3] = {
                            (float)mouseX,
                            (float)mouseY,
                            0.0
                        };
                        
                        /*
                         * Compute normal vector from viewport center to
                         * old mouse position to new mouse position.
                         * If normal-Z is positive, mouse has been moved
                         * in a counter clockwise motion relative to center.
                         * If normal-Z is negative, mouse has moved clockwise.
                         */
                        float normalDirection[3];
                        MathFunctions::normalVectorDirection(viewportCenter,
                                                             oldPos,
                                                             newPos,
                                                             normalDirection);
                        flatRotate = std::fabs(flatRotate);
                        if (normalDirection[2] > 0.0) {
                            /* mouse movied counter-clockwise */
                        }
                        else if (normalDirection[2] < 0.0) {
                            /* mouse movied clockwise */
                            flatRotate = -flatRotate;
                        }
                        
                        if (StructureEnum::isRight(flatStructure)) {
                            flatRotate = -flatRotate;
                        }
                    }
                }
                Matrix4x4 flatRotationMatrix = viewingTransform->getFlatRotationMatrix();
                flatRotationMatrix.rotateZ(flatRotate);
                viewingTransform->setFlatRotationMatrix(flatRotationMatrix);
            }
        }
    }
    updateYokedModelBrowserTabs();
}

/**
 * @return the angle of the mouse movement from a triangle formed by
 * rotationCenterXY, previousMouseXY, and mouseXY.
 * Angle is in degrees with POSITIVE COUNTER-CLOCKWISE
 *
 * @param rotationCenterXY
 *    Point rotating around (coordinate of the selected slices)
 * @param mouseXY
 *    Current XY of mouse
 * @param previousMouseXY
 *    Previous XY of mouse
 */

float
BrowserTabContent::getMouseMovementAngleCCW(const Vector3D& rotationCenterXY,
                                            const Vector3D& mouseXY,
                                            const Vector3D& previousMouseXY) const
{
    /*
     * Compute normal vector from viewport center to
     * old mouse position to new mouse position.
     * If normal-Z is positive, mouse has been moved
     * in a counter clockwise motion relative to center.
     * If normal-Z is negative, mouse has moved clockwise.
     */
    Vector3D normalDirection;
    MathFunctions::normalVectorDirection(rotationCenterXY,
                                         previousMouseXY,
                                         mouseXY,
                                         normalDirection);
    bool isClockwise = false;
    if (normalDirection[2] < 0.0) {
        isClockwise = true;
    }
    
    /*
     * Heron (Hero) of Alexandria Angles
     * https://www.mathsisfun.com/geometry/herons-formula.html
     */
    const float a((rotationCenterXY - mouseXY).length());
    const float b((rotationCenterXY - previousMouseXY).length());
    const float c((mouseXY - previousMouseXY).length());
    const float denominator(2 * a * b);
    float angleOut(0.0);
    if (denominator != 0.0) {
        const float numerator(a*a + b*b - c*c);
        float value(numerator / denominator);
        
        /*
         * Prevent NaN for when value is slightly out of range (ie 1.0000001).
         */
        if (value > 1.0) {
            value = 1.0;
        }
        if (value < -1.0) {
            value = -1.0;
        }
        const float angle(std::acos(value));
        angleOut = MathFunctions::toDegrees(angle);
    }

    /*
     * angle from std::acos() is always positive
     * but clockwise needs to be negative
     */
    if (isClockwise) {
        angleOut = -angleOut;
    }
    
    if (MathFunctions::isNaN(angleOut)) {
        CaretAssertMessage(0, "Mouse rotation angle is NaN in getMouseMovementAngleCCW()");
        angleOut = 0.0;
    }
    
    return angleOut;
}

/**
 * Process MPR volume slice rotation
 * @param viewportContent
 *    Content of the viewport
 * @param mprCrosshairAxis
 *    The MPR crosshair that indicates the type of rotation
 * @param viewport
 *    The viewport
 * @param mousePressWindowXY
 *    XY where mouse was pressed when first dragging in window coordinates
 * @param mouseWindowXY
 *    Current XY of mouse in window coordinate
 * @param previousMouseXY
 *    Previous XY of mouse in window coordinates
 */
void
BrowserTabContent::applyMouseRotationMprThree(BrainOpenGLViewportContent* viewportContent,
                                              const SelectionItemVolumeMprCrosshair::Axis mprCrosshairAxis,
                                              const GraphicsViewport& viewportIn,
                                              const Vector3D& mousePressWindowXY,
                                              const Vector3D& mouseWindowXY,
                                              const Vector3D& previousMouseWindowXY)
{
    GraphicsViewport viewport(viewportIn);
    
    VolumeSliceViewPlaneEnum::Enum sliceViewPlane = this->getVolumeSliceViewPlane();
    GraphicsViewport sliceViewport;
    if (sliceViewPlane == VolumeSliceViewPlaneEnum::ALL) {
        sliceViewPlane = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                            getVolumeSlicePlanesAllViewLayout(),
                                                                                            mousePressWindowXY,
                                                                                            sliceViewport);
    }

    bool montageFlag(false);
    switch (getVolumeSliceDrawingType()) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
        {
            GraphicsViewport mvp(getMprThreeMontageViewportContainingMouse(mousePressWindowXY));
            if (mvp.isValid()) {
                viewport = mvp;
                montageFlag = true;
            }
        }
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            break;
    }
    
    /*
     * Transform selected slice coordinates to a window coordinate
     */
    const GraphicsObjectToWindowTransform* transform(viewportContent->getVolumeGraphicsObjectToWindowTransform(sliceViewPlane));
    if (transform == NULL) {
        return;
    }
    else if ( ! transform->isValid()) {
        CaretLogWarning("GraphicsObjectToWindowTransform invalid");
        return;
    }
    
    const Vector3D sliceXYZ(getVolumeSliceCoordinates());
    Vector3D windowXYZ;
    transform->transformPoint(sliceXYZ, windowXYZ);
    
    /*
     * We want to rotate around the intersection of the selected slices
     * in the viewport containing the mouse.
     */
    Vector3D rotationCenterXYZ(windowXYZ - Vector3D(viewport.getXF(),
                                                          viewport.getYF(),
                                                          0.0));
    if (montageFlag) {
        /*
         * For montage slice, use center of the slice as rotation center
         */
        rotationCenterXYZ = viewport.getCenter() - viewport.getBottomLeft();
    }
    
    const Vector3D rotationVector(getMprThreeRotationVectorForSlicePlane(sliceViewPlane));
    
    const Vector3D mouseViewportXY(mouseWindowXY - viewport.getBottomLeft());
    const Vector3D previousMouseViewportXY(previousMouseWindowXY - viewport.getBottomLeft());
    const float rotationAngleCCW(getMouseMovementAngleCCW(rotationCenterXYZ,
                                                          mouseViewportXY,
                                                          previousMouseViewportXY));
    if (MathFunctions::isNaN(rotationAngleCCW)) {
        CaretAssertMessage(0, "Mouse rotation angle is NaN");
        return;
    }
    
    bool rotateSliceFlag(false);
    bool rotateTransformFlag(false);
    switch (mprCrosshairAxis) {
        case SelectionItemVolumeMprCrosshair::Axis::INVALID:
            CaretAssert(0);
            return;
            break;
        case SelectionItemVolumeMprCrosshair::Axis::ROTATE_SLICE:
            rotateSliceFlag = true;
            break;
        case SelectionItemVolumeMprCrosshair::Axis::ROTATE_TRANSFORM:
            rotateTransformFlag = true;
            break;
        case SelectionItemVolumeMprCrosshair::Axis::SELECT_SLICE:
            CaretAssert(0);
            break;
    }

    /*
     * Separate rotation matrices
     */
    {
        if (rotateTransformFlag) {
            const QQuaternion rotationQuaternion(QQuaternion::fromAxisAndAngle(rotationVector[0], rotationVector[1], rotationVector[2],
                                                                               -rotationAngleCCW));

            m_mprThreeRotationSeparateQuaternion *= rotationQuaternion;
            m_mprThreeAxialSeparateRotationQuaternion *= rotationQuaternion;
            m_mprThreeCoronalSeparateRotationQuaternion *= rotationQuaternion;
            m_mprThreeParasagittalSeparateRotationQuaternion *= rotationQuaternion;
            
            const QQuaternion oppositeRotationQuaternion(QQuaternion::fromAxisAndAngle(rotationVector[0], rotationVector[1], rotationVector[2],
                                                                                       rotationAngleCCW));
            
            /*
             * Pre-multiplying seems to result in slice remaining
             * static (not rotating)
             */
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    m_mprThreeAxialSeparateRotationQuaternion *= oppositeRotationQuaternion;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    m_mprThreeCoronalSeparateRotationQuaternion *= oppositeRotationQuaternion;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    m_mprThreeParasagittalSeparateRotationQuaternion *= oppositeRotationQuaternion;
                    break;
            }
        }
        else if (rotateSliceFlag) {
            const QQuaternion rotationQuaternion(QQuaternion::fromAxisAndAngle(rotationVector[0], rotationVector[1], rotationVector[2],
                                                                               rotationAngleCCW));

            m_mprThreeRotationSeparateQuaternion *= rotationQuaternion;
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    m_mprThreeAxialSeparateRotationQuaternion *= rotationQuaternion;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    m_mprThreeCoronalSeparateRotationQuaternion *= rotationQuaternion;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    m_mprThreeParasagittalSeparateRotationQuaternion *= rotationQuaternion;
                    break;
            }
        }
    }
    
    /*
     * Inverse rotations
     */
    {
        /*
         * From https://www.mathworks.com/help/fusion/ug/rotations-orientation-and-quaternions.html
         * "The quaternion class, and this example, use the "right-hand
         * rule" convention to define rotations. That is, positive
         * rotations are clockwise around the axis of rotation when
         * viewed from the origin (looking from negative axis to
         * positive axis).
         *
         * Neurological convention views the brain from the back of
         * the head (left side of the brain is on the left side of the
         * screen).
         *
         * Radiological convention views the brain from the front of
         * the head (right side of the brain is on the left side of
         * the screen).
         *
         *       Neurological  Screen Screen        Looking    Positive
         * Axis  Radiological  Left   Right Up Down From       Rotation
         *  P       N/A         +Y     -Y   +Z  -Z  Left         CW
         *  C        N          -X     +X   +Z  -Z  Posterior    CW
         *  C        R          +X     -Y   +Z  -Z  Anterior     CCW
         *  A        N          -X     +X   +Z  -Z  Superior     CCW
         *  A        R          +X     -X   +Z  -Z  Inferior     CW
         *
         * Other references:
         * - https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
         * - https://danceswithcode.net/engineeringnotes/rotations_in_3d/rotations_in_3d_part1.html
         */
        
        const QQuaternion rotationQuaternion(QQuaternion::fromAxisAndAngle(rotationVector[0], rotationVector[1], rotationVector[2],
                                                                           rotationAngleCCW));
        
        if (rotateTransformFlag) {
            const QQuaternion oppositeRotationQuaternion(QQuaternion::fromAxisAndAngle(rotationVector[0], rotationVector[1], rotationVector[2],
                                                                                       -rotationAngleCCW));
            
            /*
             * Pre-multiplying seems to result in slice remaining
             * static (not rotating)
             */
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    m_mprThreeAxialInverseRotationQuaternion = (oppositeRotationQuaternion
                                                                * m_mprThreeAxialInverseRotationQuaternion);
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    m_mprThreeCoronalInverseRotationQuaternion = (oppositeRotationQuaternion
                                                                  * m_mprThreeCoronalInverseRotationQuaternion);
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    m_mprThreeParasagittalInverseRotationQuaternion = (oppositeRotationQuaternion
                                                                       * m_mprThreeParasagittalInverseRotationQuaternion);
                    break;
            }
        }
        else if (rotateSliceFlag) {
            /*
             * Pre-multiplying seems to result in slice remaining
             * static (not rotating)
             */
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    m_mprThreeAxialInverseRotationQuaternion = (m_mprThreeAxialInverseRotationQuaternion
                                                                * rotationQuaternion);
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    m_mprThreeCoronalInverseRotationQuaternion = (m_mprThreeCoronalInverseRotationQuaternion
                                                                  * rotationQuaternion);
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    m_mprThreeParasagittalInverseRotationQuaternion = (m_mprThreeParasagittalInverseRotationQuaternion
                                                                       * rotationQuaternion);
                    break;
            }
        }
    }
}

/**
 * Apply mouse scaling to the displayed model.
 *
 * @param viewportContent
 *    Content of the viewport
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 * @param dataX
 *    X-coordinate of data at mouse press
 * @param dataY
 *    Y-coordinate of data at mouse press
 * @param dataXYValidFlag
 *    True if mouseDX
 */
void
BrowserTabContent::applyHistologyMouseScaling(BrainOpenGLViewportContent* viewportContent,
                                              const int32_t mousePressX,
                                              const int32_t mousePressY,
                                              const int32_t mouseDY,
                                              const float dataX,
                                              const float dataY,
                                              const bool dataXYValidFlag)
{
    if (isHistologyDisplayed()) {
        const GraphicsObjectToWindowTransform* xform = viewportContent->getHistologyGraphicsObjectToWindowTransform();
        getViewingTransformation()->scaleAboutMouse(xform,
                                                    mousePressX,
                                                    mousePressY,
                                                    mouseDY,
                                                    dataX,
                                                    dataY,
                                                    dataXYValidFlag);
    }
    else {
        CaretAssertMessage(0, "HISTOLOGY ONLY");
    }
    updateYokedModelBrowserTabs();
}


/**
 * Apply mouse scaling to the displayed model.
 *
 * @param viewportContent
 *    Content of the viewport
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 * @param dataX
 *    X-coordinate of data at mouse press
 * @param dataY
 *    Y-coordinate of data at mouse press
 * @param dataXYValidFlag
 *    True if mouseDX
 */
void
BrowserTabContent::applyMediaMouseScaling(BrainOpenGLViewportContent* viewportContent,
                                          const int32_t mousePressX,
                                          const int32_t mousePressY,
                                          const int32_t mouseDY,
                                          const float dataX,
                                          const float dataY,
                                          const bool dataXYValidFlag)
{
    if (isMediaDisplayed()) {
        const GraphicsObjectToWindowTransform* xform = viewportContent->getMediaGraphicsObjectToWindowTransform();
        getViewingTransformation()->scaleAboutMouse(xform,
                                                    mousePressX,
                                                    mousePressY,
                                                    mouseDY,
                                                    dataX,
                                                    dataY,
                                                    dataXYValidFlag);
    }
    else {
        CaretAssertMessage(0, "MEDIA ONLY");
    }
    updateYokedModelBrowserTabs();
}

/**
 * Set the scaling for histology from GUI
 * @param viewportContent
 *    Content of the viewport
 * @param scaling
 *    New scaling (zoom) value
 */
void
BrowserTabContent::setHistologyScalingFromGui(BrainOpenGLViewportContent* viewportContent,
                                              const float scaling)
{
    if (isHistologyDisplayed()) {
        const GraphicsObjectToWindowTransform* xform = viewportContent->getHistologyGraphicsObjectToWindowTransform();
        getViewingTransformation()->setHistologyScaling(xform,
                                                        scaling);
    }
    else {
        CaretAssertMessage(0, "MEDIA ONLY");
    }
    updateYokedModelBrowserTabs();
}


/**
 * Set the scaling for media from GUI
 * @param viewportContent
 *    Content of the viewport
 * @param scaling
 *    New scaling (zoom) value
 */
void
BrowserTabContent::setMediaScalingFromGui(BrainOpenGLViewportContent* viewportContent,
                                          const float scaling)
{
    if (isMediaDisplayed()) {
        const GraphicsObjectToWindowTransform* xform = viewportContent->getMediaGraphicsObjectToWindowTransform();
        getViewingTransformation()->setMediaScaling(xform,
                                                    scaling);
    }
    else {
        CaretAssertMessage(0, "MEDIA ONLY");
    }
    updateYokedModelBrowserTabs();
}

/**
 * Set the media scaling to the given value
 */
void
BrowserTabContent::setMediaScaling(const float newScaleValue)
{
    ModelMedia* mediaModel = getDisplayedMediaModel();
    if (mediaModel !=NULL) {
        MediaOverlaySet* overlaySet = getMediaOverlaySet();
        MediaOverlay* underlay = overlaySet->getBottomMostEnabledOverlay();
        if (underlay != NULL) {
            const MediaOverlay::SelectionData selectionData(underlay->getSelectionData());
            if (selectionData.m_selectedMediaFile != NULL) {
                const float oldScaleValue = m_mediaViewingTransformation->getScaling();
                if ((newScaleValue > 0.0)
                    && (oldScaleValue > 0.0)) {
                    const float imageWidth(selectionData.m_selectedMediaFile->getWidth());
                    const float imageHeight(selectionData.m_selectedMediaFile->getHeight());
                    
                    /*
                     * Width/height of image with previous scaling
                     */
                    const float oldImageWidth(imageWidth * oldScaleValue);
                    const float oldImageHeight(imageHeight * oldScaleValue);
                    
                    /*
                     * Width/height of image with new scaling
                     */
                    const float newImageWidth(imageWidth * newScaleValue);
                    const float newImageHeight(imageHeight * newScaleValue);
                    
                    /*
                     * Need to keep image center in same location.
                     * Origin is at the bottom left corner.
                     * So, need to translate using the change in image width/height
                     * and also take into account the default scaling (scales image
                     * to fit height of window in default view).
                     */
                    float tx(((oldImageWidth - newImageWidth) / 2.0));
                    float ty(((oldImageHeight - newImageHeight) / 2.0));
                    
                    /*
                     * Update translation
                     */
                    float translation[3];
                    m_mediaViewingTransformation->getTranslation(translation);
                    translation[0] += tx;
                    translation[1] += ty;
                    m_mediaViewingTransformation->setTranslation(translation);
                    
                    /*
                     * Set new scaling.
                     */
                    m_mediaViewingTransformation->setScaling(newScaleValue);
                }
            }
        }
    }
}

/**
 * Set the bounds of the view to the given selection bounds.
 * @param allViewportContent
 *    Content of all viewports in all windows
 * @param mouseEvent
 *    The mouse event
 * @param selectionBounds
 *    Box containing bounds of selection
 */
void
BrowserTabContent::setViewToBounds(const std::vector<const BrainOpenGLViewportContent*>& allViewportContent,
                                   const MouseEvent* mouseEvent,
                                   const GraphicsRegionSelectionBox* selectionBounds)
{
    CaretAssert(mouseEvent);
    const BrainOpenGLViewportContent* viewportContent(mouseEvent->getViewportContent());
    CaretAssert(viewportContent);
    CaretAssert(selectionBounds);
    
    if (getDisplayedHistologyModel() != NULL) {
        HistologySlice* histologySlice(NULL);
        ModelHistology* histologyModel(getDisplayedHistologyModel());
        if (histologyModel != NULL) {
            HistologyOverlaySet* histologyOverlaySet(getHistologyOverlaySet());
            if (histologyOverlaySet != NULL) {
                HistologyOverlay* histologyOverlay(histologyOverlaySet->getUnderlay());
                if (histologyOverlay != NULL) {
                    HistologyOverlay::SelectionData selectionData(histologyOverlay->getSelectionData());
                    if (selectionData.m_selectedFile != NULL) {
                        histologySlice = selectionData.m_selectedFile->getHistologySliceByIndex(selectionData.m_selectedSliceIndex);
                    }
                }
            }
        }
        
        updateBrainModelYokedBrowserTabs();

        const GraphicsObjectToWindowTransform* xform = viewportContent->getHistologyGraphicsObjectToWindowTransform();
        if (m_histologyViewingTransformation->setMediaViewToBounds(mouseEvent,
                                                                   selectionBounds,
                                                                   xform,
                                                                   histologySlice)) {
            if (getBrainModelYokingGroup() != YokingGroupEnum::YOKING_GROUP_OFF) {
                setVolumeSliceViewsToHistologyRegion(getBrainModelYokingGroup(),
                                                     histologySlice,
                                                     allViewportContent,
                                                     selectionBounds);
            }
        }
    }
    else if (getDisplayedMediaModel() != NULL) {
        const GraphicsObjectToWindowTransform* xform = viewportContent->getMediaGraphicsObjectToWindowTransform();
        HistologySlice* histologySlice(NULL);
        m_mediaViewingTransformation->setMediaViewToBounds(mouseEvent,
                                                           selectionBounds,
                                                           xform,
                                                           histologySlice);
        updateMediaModelYokedBrowserTabs();
    }
    else if (getDisplayedVolumeModel() != NULL) {
        
        VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion(VolumeSliceViewPlaneEnum::ALL);
        const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab(getVolumeSliceViewPlane());
        switch (sliceViewPlaneSelectedInTab) {
            case VolumeSliceViewPlaneEnum::ALL:
                    {
                        /*
                         * Find out which slice plane contains mouse
                         */
                        int viewport[4];
                        viewportContent->getModelViewport(viewport);
                        int sliceViewport[4] = {
                            viewport[0],
                            viewport[1],
                            viewport[2],
                            viewport[3]
                        };
                        sliceViewPlaneForFitToRegion = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                                            getVolumeSlicePlanesAllViewLayout(),
                                                                                                            mouseEvent->getPressedX(),
                                                                                                            mouseEvent->getPressedY(),
                                                                                                            sliceViewport);
                        if (sliceViewPlaneForFitToRegion == VolumeSliceViewPlaneEnum::ALL) {
                            /* Not in slice plane*/
                            return;
                        }
                    }
                        break;
            case VolumeSliceViewPlaneEnum::CORONAL:
            case VolumeSliceViewPlaneEnum::AXIAL:
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                sliceViewPlaneForFitToRegion = sliceViewPlaneSelectedInTab;
                break;
        }
        CaretAssert(sliceViewPlaneForFitToRegion != VolumeSliceViewPlaneEnum::ALL);

        switch (getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                m_volumeSliceViewingTransformation->setViewToBounds(mouseEvent->getViewportContent(),
                                                                    sliceViewPlaneSelectedInTab,
                                                                    sliceViewPlaneForFitToRegion,
                                                                    selectionBounds,
                                                                    this);
                break;
        }
        updateBrainModelYokedBrowserTabs();
    }
}

/**
 * Apply mouse scaling to the displayed model.
 *
 * @param viewportContent
 *    Content of the viewport
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseDX
 *    Change in mouse X coordinate.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseScaling(BrainOpenGLViewportContent* viewportContent,
                                     const int32_t mousePressX,
                                     const int32_t mousePressY,
                                     const int32_t /*mouseDX*/,
                                     const int32_t mouseDY)
{
    if (isChartOneDisplayed()) {
        ModelChart* modelChart = getDisplayedChartOneModel();
        CaretAssert(modelChart);
        
        CaretDataFileSelectionModel* matrixSelectionModel = NULL;
        if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER) {
            matrixSelectionModel = modelChart->getChartableMatrixParcelFileSelectionModel(m_tabNumber);
        }
        
        if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES) {
            matrixSelectionModel = modelChart->getChartableMatrixSeriesFileSelectionModel(m_tabNumber);
        }
        if (matrixSelectionModel != NULL) {
            ChartableMatrixInterface* chartableInterface = matrixSelectionModel->getSelectedFileOfType<ChartableMatrixInterface>();
            if (chartableInterface != NULL) {
                ChartMatrixDisplayProperties* matrixProperties = chartableInterface->getChartMatrixDisplayProperties(m_tabNumber);
                matrixProperties->setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL);
                float scaling = matrixProperties->getViewZooming();
                if (mouseDY != 0.0) {
                    scaling *= (1.0f + (mouseDY * 0.01));
                }
                if (scaling < 0.01) {
                    scaling = 0.01;
                }
                matrixProperties->setViewZooming(scaling);
            }
        }
    }
    else if (isChartTwoDisplayed()) {
        ChartTwoOverlaySet* overlaySet = getChartTwoOverlaySet();
        if (overlaySet != NULL) {
            int32_t viewport[4];
            Matrix4x4 m1, m2;
            if (viewportContent->getChartDataMatricesAndViewport(m1, m2, viewport)) {
                overlaySet->applyMouseScaling(viewport,
                                              mousePressX,
                                              mousePressY,
                                              mouseDY);
            }
        }
    }
    else if (isHistologyDisplayed()) {
        CaretAssertMessage(0, "Use applyHistologyMouseScaling() when scaling media data");
    }
    else if (isMediaDisplayed()) {
        CaretAssertMessage(0, "Use applyMediaMouseScaling() when scaling media data");
    }
    else {
        float scaling = getViewingTransformation()->getScaling();
        if (mouseDY != 0.0) {
            scaling *= (1.0f + (mouseDY * 0.01));
        }
        if (scaling < 0.01) {
            scaling = 0.01;
        }
        getViewingTransformation()->setScaling(scaling);
    }
    updateYokedModelBrowserTabs();
}

/**
 * Apply mouse translation to the displayed model.
 *
 * @param viewportContent
 *    Content of the viewport.
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseX
 *    Mouse X coordinate.
 * @param mouseY
 *    Mouse Y coordinate.
 * @param mouseDX
 *    Change in mouse X coordinate.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseTranslation(BrainOpenGLViewportContent* viewportContent,
                                         const int32_t mousePressX,
                                         const int32_t mousePressY,
                                         const int32_t mouseX,
                                         const int32_t mouseY,
                                         const int32_t mouseDX,
                                         const int32_t mouseDY)
{
    const int tabIndex = getTabNumber();
    
    if (isVolumeSlicesDisplayed()) {
        bool mprFlag(false);
        switch (getVolumeSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                mprFlag = true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                mprFlag = true;
                break;
        }
        if (mprFlag) {
            applyMouseTranslationVolumeMPR(viewportContent,
                                           mousePressX,
                                           mousePressY,
                                           mouseX,
                                           mouseY,
                                           mouseDX,
                                           mouseDY);
        }
        else {
            
            const float volumeSliceScaling = m_volumeSliceViewingTransformation->getScaling();
            ModelVolume* modelVolume = getDisplayedVolumeModel();
            VolumeMappableInterface* vf = modelVolume->getUnderlayVolumeFile(tabIndex);
            BoundingBox mybox;
            vf->getVoxelSpaceBoundingBox(mybox);
            float cubesize = std::max(std::max(mybox.getDifferenceX(), mybox.getDifferenceY()), mybox.getDifferenceZ());//factor volume bounding box into slowdown for zoomed in
            float slowdown = 0.005f * cubesize / volumeSliceScaling;//when zoomed in, make the movements slower to match - still changes based on viewport currently
            slowdown = 1.0;
            
            float dx = 0.0;
            float dy = 0.0;
            float dz = 0.0;
            
            int viewport[4];
            viewportContent->getModelViewport(viewport);
            int sliceViewport[4];
            viewportContent->getModelViewport(sliceViewport);
            
            VolumeSliceViewPlaneEnum::Enum slicePlane = getVolumeSliceViewPlane();
            if (slicePlane == VolumeSliceViewPlaneEnum::ALL) {
                slicePlane = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                                getVolumeSlicePlanesAllViewLayout(),
                                                                                                mousePressX,
                                                                                                mousePressY,
                                                                                                sliceViewport);
            }
            
            switch (slicePlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    dx = mouseDX * slowdown;
                    dy = mouseDY * slowdown;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    dx = mouseDX * slowdown;
                    dz = mouseDY * slowdown;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    dy = -mouseDX * slowdown;
                    dz = mouseDY * slowdown;
                    break;
            }
            
            float translation[3];
            m_volumeSliceViewingTransformation->getTranslation(translation);
            translation[0] += dx;
            translation[1] += dy;
            translation[2] += dz;
            m_volumeSliceViewingTransformation->setTranslation(translation);
        }
    }
    else if (isChartOneDisplayed()) {
        ModelChart* modelChart = getDisplayedChartOneModel();
        CaretAssert(modelChart);
        
        CaretDataFileSelectionModel* matrixSelectionModel = NULL;
        if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER) {
            matrixSelectionModel = modelChart->getChartableMatrixParcelFileSelectionModel(m_tabNumber);
        }
        
        if (modelChart->getSelectedChartOneDataType(m_tabNumber) == ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES) {
            matrixSelectionModel = modelChart->getChartableMatrixSeriesFileSelectionModel(m_tabNumber);
        }
        if (matrixSelectionModel != NULL) {
            ChartableMatrixInterface* chartableInterface = matrixSelectionModel->getSelectedFileOfType<ChartableMatrixInterface>();
            if (chartableInterface != NULL) {
                ChartMatrixDisplayProperties* matrixProperties = chartableInterface->getChartMatrixDisplayProperties(m_tabNumber);
                matrixProperties->setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL);
                float translation[2];
                matrixProperties->getViewPanning(translation);
                translation[0] += mouseDX;
                translation[1] += mouseDY;
                matrixProperties->setViewPanning(translation);
            }
        }
    }
    else if (isChartTwoDisplayed()) {
        ChartTwoOverlaySet* overlaySet = getChartTwoOverlaySet();
        if (overlaySet != NULL) {
            int32_t viewport[4];
            Matrix4x4 m1, m2;
            if (viewportContent->getChartDataMatricesAndViewport(m1, m2, viewport)) {
                overlaySet->applyMouseTranslation(viewport,
                                                  mouseDX,
                                                  mouseDY);
            }
        }
    }
    else if (isHistologyDisplayed()) {
        float txyz[3];
        m_histologyViewingTransformation->getTranslation(txyz);
        /*
         * Mouse movement is in viewport (window) coordinates.
         * Need to convert to plane coordinates.
         */
        const GraphicsObjectToWindowTransform* transform(viewportContent->getHistologyGraphicsObjectToWindowTransform());
        CaretAssert(transform);
        int32_t modelViewport[4];
        viewportContent->getModelViewport(modelViewport);
        const Vector3D topLeftViewport(modelViewport[0], modelViewport[1] + modelViewport[3], 0.0);
        const Vector3D bottomRightViewport(modelViewport[0] + modelViewport[2], modelViewport[1], 0.0);
        Vector3D topLeftCoord;
        Vector3D bottomRightCoord;
        transform->inverseTransformPoint(topLeftViewport, topLeftCoord);
        transform->inverseTransformPoint(bottomRightViewport, bottomRightCoord);
        const float coordWidth(std::fabs(topLeftCoord[0] - bottomRightCoord[0]));
        const float coordHeight(std::fabs(topLeftCoord[1] - bottomRightCoord[1]));
        const float zoom(getScaling());
        const float scaleX((coordWidth / modelViewport[2]) * zoom);
        const float scaleY((coordHeight / modelViewport[3]) * zoom);
        
        const float accelerate(1.0);
        if (isHistologyFlipXEnabled()) {
            txyz[0] -= ((mouseDX * scaleX) * accelerate);
        }
        else {
            txyz[0] += ((mouseDX * scaleX) * accelerate);
        }
        txyz[1] += ((mouseDY * scaleY) * accelerate);
        m_histologyViewingTransformation->setTranslation(txyz);
    }
    else if (isMediaDisplayed()) {
        float txyz[3];
        m_mediaViewingTransformation->getTranslation(txyz);
        switch (getMediaDisplayCoordinateMode()) {
            case MediaDisplayCoordinateModeEnum::PIXEL:
            {
                const float accelerate(3.0);
                txyz[0] += (mouseDX * accelerate);
                txyz[1] += (mouseDY * accelerate);
            }
                break;
            case MediaDisplayCoordinateModeEnum::PLANE:
            {
                /*
                 * Mouse movement is in viewport (window) coordinates.
                 * Need to convert to plane coordinates.
                 */
                const GraphicsObjectToWindowTransform* transform(viewportContent->getMediaGraphicsObjectToWindowTransform());
                CaretAssert(transform);
                int32_t modelViewport[4];
                viewportContent->getModelViewport(modelViewport);
                const Vector3D topLeftViewport(modelViewport[0], modelViewport[1] + modelViewport[3], 0.0);
                const Vector3D bottomRightViewport(modelViewport[0] + modelViewport[2], modelViewport[1], 0.0);
                Vector3D topLeftCoord;
                Vector3D bottomRightCoord;
                transform->inverseTransformPoint(topLeftViewport, topLeftCoord);
                transform->inverseTransformPoint(bottomRightViewport, bottomRightCoord);
                const float coordWidth(std::fabs(topLeftCoord[0] - bottomRightCoord[0]));
                const float coordHeight(std::fabs(topLeftCoord[1] - bottomRightCoord[1]));
                const float zoom(getScaling());
                const float scaleX((coordWidth / modelViewport[2]) * zoom);
                const float scaleY((coordHeight / modelViewport[3]) * zoom);

                const float accelerate(1.0);
                txyz[0] += ((mouseDX * scaleX) * accelerate);
                txyz[1] += ((mouseDY * scaleY) * accelerate);
            }
                break;
        }
        m_mediaViewingTransformation->setTranslation(txyz);
    }
    else if (isCerebellumDisplayed()) {
        const float screenDX = mouseDX;
        const float screenDY = mouseDY;
        
        float translateDX = 0.0;
        float translateDY = 0.0;
        float translateDZ = 0.0;
        
        ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
        if (montageModel != NULL) {
            std::vector<const SurfaceMontageViewport*> montageViewports;
            montageModel->getSurfaceMontageViewportsForTransformation(getTabNumber(),
                                                                      montageViewports);
            
            bool foundMontageViewportFlag = false;
            
            const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
            for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                const SurfaceMontageViewport* smv = montageViewports[ivp];
                
                if (smv->isInside(mousePressX,
                                  mousePressY)) {
                    switch (smv->getProjectionViewType()) {
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
                            translateDX =  screenDX;
                            translateDY =  screenDY;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
                            translateDX = screenDX;
                            translateDY = screenDY;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
                            translateDX = screenDX;
                            translateDY = screenDY;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
                            translateDX =  screenDX;
                            translateDY =  screenDY;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
                            translateDX = screenDX;
                            translateDY = screenDY;
                            foundMontageViewportFlag = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_DENTATE_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_DENTATE_SURFACE:
                            break;
                    }
                }
                if (foundMontageViewportFlag) {
                    break;
                }
            }
        }
        else {
            translateDX = screenDX;
            translateDY = screenDY;
        }

        float translation[3];
        m_cerebellumViewingTransformation->getTranslation(translation);
        translation[0] += translateDX;
        translation[1] += translateDY;
        translation[2] += translateDZ;
        m_cerebellumViewingTransformation->setTranslation(translation);
    }
    else {
        float dx = mouseDX;
        float dy = mouseDY;
        
        ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
        
        
        if (montageModel != NULL) {
            std::vector<const SurfaceMontageViewport*> montageViewports;
            montageModel->getSurfaceMontageViewportsForTransformation(getTabNumber(),
                                                                      montageViewports);
            
            bool isValid = false;
            bool isLeft = true;
            bool isLateral = false;
            const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
            for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                const SurfaceMontageViewport* smv = montageViewports[ivp];
                if (smv->isInside(mousePressX,
                                 mousePressY)) {
                    switch (smv->getProjectionViewType()) {
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                            isLeft = true;
                            isLateral = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                            isLeft = true;
                            isLateral = false;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_DENTATE_SURFACE:
                            isLeft = true;
                            isLateral = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                            isLeft = false;
                            isLateral = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                            isLeft = false;
                            isLateral = false;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_DENTATE_SURFACE:
                            isLeft = false;
                            isLateral = true;
                            break;
                    }
                    isValid = true;
                }
                
                if (isValid) {
                    break;
                }
            }
            
            if (isValid) {
                if (isLeft == false) {
                    dx = -dx;
                }
                if (isLateral == false) {
                    dx = -dx;
                }
                
                float translation[3];
                getViewingTransformation()->getTranslation(translation);
                translation[0] += dx;
                translation[1] += dy;
                getViewingTransformation()->setTranslation(translation);
            }
        }
        else {
            if (getProjectionViewType() == ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL) {
                dx = -dx;
            }
            else if (getProjectionViewType() == ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE) {
                dx = -dx;
            }
            float translation[3];
            getViewingTransformation()->getTranslation(translation);
            translation[0] += dx;
            translation[1] += dy;
            getViewingTransformation()->setTranslation(translation);
        }
    }
    updateYokedModelBrowserTabs();
}



/**
 * Apply mouse translation to the displayed MPR volume model
 *
 * @param viewportContent
 *    Content of the viewport.
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseX
 *    Mouse X coordinate.
 * @param mouseY
 *    Mouse Y coordinate.
 * @param mouseDX
 *    Change in mouse X coordinate.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseTranslationVolumeMPR(BrainOpenGLViewportContent* viewportContent,
                                                  const int32_t mousePressX,
                                                  const int32_t mousePressY,
                                                  const int32_t mouseX,
                                                  const int32_t mouseY,
                                                  const int32_t mouseDX,
                                                  const int32_t mouseDY)
{
    int viewport[4];
    viewportContent->getModelViewport(viewport);
    int sliceViewport[4];
    viewportContent->getModelViewport(sliceViewport);
    
    VolumeSliceViewPlaneEnum::Enum slicePlane = getVolumeSliceViewPlane();
    if (slicePlane == VolumeSliceViewPlaneEnum::ALL) {
        slicePlane = BrainOpenGLViewportContent::getSliceViewPlaneForVolumeAllSliceView(viewport,
                                                                                        getVolumeSlicePlanesAllViewLayout(),
                                                                                        mousePressX,
                                                                                        mousePressY,
                                                                                        sliceViewport);
    }
    
    const GraphicsObjectToWindowTransform* objectToWindowXform = viewportContent->getVolumeGraphicsObjectToWindowTransform(slicePlane);
    if (objectToWindowXform != NULL) {
        /*
         * Previous position of mouse
         */
        const float oldMouseX(mouseX - mouseDX);
        const float oldMouseY(mouseY - mouseDY);
        
        /*
         * Convert location of mouse from window to model coordinates
         */
        Vector3D windowOneXYZ(oldMouseX, oldMouseY, 0.0);
        Vector3D windowTwoXYZ((float)mouseX, (float)mouseY, 0.0);
        Vector3D objectOneXYZ;
        Vector3D objectTwoXYZ;
        objectToWindowXform->inverseTransformPoint(windowOneXYZ, objectOneXYZ);
        objectToWindowXform->inverseTransformPoint(windowTwoXYZ, objectTwoXYZ);
        
        /*
         * Convert mouse movement from window to model coordinates
         */
        Vector3D objDXYZ((objectTwoXYZ[0] - objectOneXYZ[0]),
                         (objectTwoXYZ[1] - objectOneXYZ[1]),
                         (objectTwoXYZ[2] - objectOneXYZ[2]));
        const float objLength(objDXYZ.length());
        Vector3D mouseNormalDXY(Vector3D(mouseDX, mouseDY, 0.0).normal());
        
        
        const float horizontalMovement(mouseNormalDXY[0] * objLength);
        const float verticalMovement(mouseNormalDXY[1] * objLength);
        
        float dx = 0.0;
        float dy = 0.0;
        float dz = 0.0;
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                dx = horizontalMovement;
                dy = verticalMovement;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                dx = horizontalMovement;
                dz = verticalMovement;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                /*
                 * Need to invert 'dy' since parasagittal
                 * has postitive on the left and negative
                 * on the right
                 */
                dy = -horizontalMovement;
                dz =  verticalMovement;
                break;
        }
        
        float translation[3];
        m_volumeSliceViewingTransformation->getTranslation(translation);
        translation[0] += dx;
        translation[1] += dy;
        translation[2] += dz;
        m_volumeSliceViewingTransformation->setTranslation(translation);
    }
    else {
        CaretLogSevere("PROGRAM ERROR: GraphicsObjectToWindowTransform failed, unable to pan view");
    }
}

/**
 * Apply chart two bounds selection as user drags the mouse
 * @param viewport
 * Chart viewport
 * @param x1
 * X from first pair of coordinates
 * @param y1
 * Y from first pair of coordinates
 * @param x2
 * X from second pair of coordinates
 * @param y2
 * Y from second pair of coordinates
 */
void
BrowserTabContent::applyChartTwoAxesBoundSelection(const int32_t viewport[4],
                                                   const int32_t x1,
                                                   const int32_t y1,
                                                   const int32_t x2,
                                                   const int32_t y2)
{
    ChartTwoOverlaySet* ctos = getChartTwoOverlaySet();
    if (ctos != NULL) {
        ctos->applyChartTwoAxesBoundSelection(viewport,
                                              x1, y1, x2, y2);
    }
}

/**
 * Finalize chart two bounds selection to set the bounds of the chart
 * @param viewport
 * Chart viewport
 * @param x1
 * X from first pair of coordinates
 * @param y1
 * Y from first pair of coordinates
 * @param x2
 * X from second pair of coordinates
 * @param y2
 * Y from second pair of coordinates
 */
void
BrowserTabContent::finalizeChartTwoAxesBoundSelection(const int32_t viewport[4],
                                                      const int32_t x1,
                                                      const int32_t y1,
                                                      const int32_t x2,
                                                      const int32_t y2)
{
    ChartTwoOverlaySet* ctos = getChartTwoOverlaySet();
    if (ctos != NULL) {
        ctos->finalizeChartTwoAxesBoundSelection(viewport,
                                                 x1, y1, x2, y2);
    }
}

/**
 * Get the transformations for drawing a model.
 *
 * @param projectionViewType
 *    Type of projection view
 * @param translationOut
 *    Translation
 * @param rotationMatrixOut
 *    OpenGL rotation matrix.
 * @param scalingOut
 *    Scaling.
 */
void
BrowserTabContent::getTransformationsForOpenGLDrawing(const ProjectionViewTypeEnum::Enum projectionViewType,
                                                      float translationOut[3],
                                                      double rotationMatrixOut[16],
                                                      float& scalingOut) const
{
    /*
     * Check for volume slice viewing
     */
    if (isVolumeSlicesDisplayed()) {
        m_volumeSliceViewingTransformation->getTranslation(translationOut);
        
        Matrix4x4 rotationMatrix = m_volumeSliceViewingTransformation->getRotationMatrix();
        rotationMatrix.getMatrixForOpenGL(rotationMatrixOut);
        
        scalingOut = m_volumeSliceViewingTransformation->getScaling();
        
        return;
    }
    
    if (isChartTwoDisplayed()) {
        m_chartTwoMatrixViewingTranformation->getTranslation(translationOut);
        Matrix4x4 matrix;
        matrix.identity();
        matrix.getMatrixForOpenGL(rotationMatrixOut);
        scalingOut = m_chartTwoMatrixViewingTranformation->getScaling();
        return;
    }
    
    /*
     * Surfaces may need a modification to the rotation matrix
     * dependent upon the projection view type.
     */
    
    Matrix4x4 rotationMatrix = getViewingTransformation()->getRotationMatrix();
    getViewingTransformation()->getTranslation(translationOut);
    scalingOut = getViewingTransformation()->getScaling();
    
    double rotationX, rotationY, rotationZ;
    rotationMatrix.getRotation(rotationX,
                               rotationY,
                               rotationZ);
    const double rotationFlippedX = -rotationX;
    const double rotationFlippedY = 180.0 - rotationY;
    
    switch (projectionViewType) {
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_ANTERIOR:
        {
            Matrix4x4 matrixOut = rotationMatrix;
            
            Matrix4x4 anteriorMatrix;
            anteriorMatrix.setRotation(90.0, 0.0, -180.0);
            
            matrixOut.postmultiply(anteriorMatrix);
            matrixOut.getMatrixForOpenGL(rotationMatrixOut);
            return;
        }
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_DORSAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_POSTERIOR:
        {
            Matrix4x4 matrixOut = rotationMatrix;
            
            Matrix4x4 posteriorMatrix;
            posteriorMatrix.setRotation(-90.0, 0.0, 0.0);
            
            matrixOut.postmultiply(posteriorMatrix);
            matrixOut.getMatrixForOpenGL(rotationMatrixOut);
            return;
        }
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_VENTRAL:
        {
            Matrix4x4 matrixOut = rotationMatrix;
            
            Matrix4x4 ventralMatrix;
            ventralMatrix.setRotation(0.0, 180.0, 180.0);
            
            matrixOut.postmultiply(ventralMatrix);
            matrixOut.getMatrixForOpenGL(rotationMatrixOut);
            return;
        }
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE:
            getFlatRotationMatrix().getRotation(rotationX,
                                                rotationY,
                                                rotationZ);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_DENTATE_SURFACE:
            getFlatRotationMatrix().getRotation(rotationX,
                                                rotationY,
                                                rotationZ);
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
            rotationX = rotationFlippedX;
            rotationY = rotationFlippedY;
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
            rotationX = rotationFlippedX;
            rotationY = rotationFlippedY;
            break;
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE:
        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_DENTATE_SURFACE:
            translationOut[0] = -translationOut[0];
            getFlatRotationMatrix().getRotation(rotationX,
                                                rotationY,
                                                rotationZ);
            rotationZ = -rotationZ;
            break;
    }
    
    Matrix4x4 matrix;
    matrix.setRotation(rotationX,
                       rotationY,
                       rotationZ);
    matrix.getMatrixForOpenGL(rotationMatrixOut);
}

/**
 * Place the transformations for the given window tab into
 * the model transform.
 * @param windowTabNumber
 *    Tab number for transformations.
 * @param modelTransform
 *    Model transform into which transformations are loaded.
 */
void
BrowserTabContent::getTransformationsInModelTransform(ModelTransform& modelTransform) const
{
    modelTransform.setTranslation(getTranslation());
    
    const Matrix4x4 rotMatrix = getRotationMatrix();
    float m[4][4];
    rotMatrix.getMatrix(m);
    modelTransform.setRotation(m);
    
    const Matrix4x4 obliqueRotationMatrix = getObliqueVolumeRotationMatrix();
    float mob[4][4];
    obliqueRotationMatrix.getMatrix(mob);
    modelTransform.setObliqueRotation(mob);

    const float mprTwoRotationAngles[3] {
        m_mprRotationX,
        m_mprRotationY,
        m_mprRotationZ
    };
    modelTransform.setMprTwoRotationAngles(mprTwoRotationAngles);

    const QVector3D mprThreeQuaternionAngles(m_mprThreeRotationSeparateQuaternion.toEulerAngles());
    const float mprThreeRotationAngles[3] {
        mprThreeQuaternionAngles[0],
        mprThreeQuaternionAngles[1],
        mprThreeQuaternionAngles[2]
    };
    modelTransform.setMprThreeRotationAngles(mprThreeRotationAngles);
    
    const Matrix4x4 flatRotationMatrix = getFlatRotationMatrix();
    float fm[4][4];
    flatRotationMatrix.getMatrix(fm);
    modelTransform.setFlatRotation(fm);
    
    float rightFlatX, rightFlatY;
    getRightCortexFlatMapOffset(rightFlatX, rightFlatY);
    modelTransform.setRightCortexFlatMapOffset(rightFlatX, rightFlatY);
    
    modelTransform.setRightCortexFlatMapZoomFactor(getRightCortexFlatMapZoomFactor());

    modelTransform.setScaling(getScaling());
}

/**
 * Apply the transformations to the browser tab.
 * @param modelTransform
 *    Model transform into which transformations are retrieved.
 * @param mprThreeRotationUpdateType
 *    Type of update made to MPR Three rotations
 */
void
BrowserTabContent::setTransformationsFromModelTransform(const ModelTransform& modelTransform,
                                                        const MprThreeRotationUpdateType mprThreeRotationUpdateType)
{
    float translation[3];
    modelTransform.getTranslation(translation);
    const float tx = translation[0];
    const float ty = translation[1];
    const float tz = translation[2];
    
    setTranslation(tx, ty, tz);

    float m[4][4];
    modelTransform.getRotation(m);
    
    Matrix4x4 rotationMatrix;
    rotationMatrix.setMatrix(m);
    setRotationMatrix(rotationMatrix);

    float mob[4][4];
    modelTransform.getObliqueRotation(mob);
    Matrix4x4 obliqueRotationMatrix;
    obliqueRotationMatrix.setMatrix(mob);
    setObliqueVolumeRotationMatrix(obliqueRotationMatrix);

    float mprTwoRotationAngles[3];
    modelTransform.getMprTwoRotationAngles(mprTwoRotationAngles);
    m_mprRotationX = mprTwoRotationAngles[0];
    m_mprRotationY = mprTwoRotationAngles[1];
    m_mprRotationZ = mprTwoRotationAngles[2];

    switch (mprThreeRotationUpdateType) {
        case MprThreeRotationUpdateType::DELTA:
        {
            float angles[3];
            modelTransform.getMprThreeRotationAngles(angles);
            QVector3D oldAngles(m_mprThreeRotationSeparateQuaternion.toEulerAngles());
            const QVector3D newAngles(angles[0] - oldAngles[0],
                                      angles[1] - oldAngles[1],
                                      angles[2] - oldAngles[2]);
            QQuaternion rotationQuaternion(QQuaternion::fromEulerAngles(newAngles));
            m_mprThreeRotationSeparateQuaternion             *= rotationQuaternion;
            m_mprThreeAxialSeparateRotationQuaternion        *= rotationQuaternion;
            m_mprThreeCoronalSeparateRotationQuaternion      *= rotationQuaternion;
            m_mprThreeParasagittalSeparateRotationQuaternion *= rotationQuaternion;
        }
            break;
        case MprThreeRotationUpdateType::REPLACE:
        {
            float mprThreeRotationAngles[3];
            modelTransform.getMprThreeRotationAngles(mprThreeRotationAngles);
            m_mprThreeRotationSeparateQuaternion = QQuaternion::fromEulerAngles(mprThreeRotationAngles[0],
                                                                                mprThreeRotationAngles[1],
                                                                                mprThreeRotationAngles[2]);
            m_mprThreeAxialSeparateRotationQuaternion        = m_mprThreeRotationSeparateQuaternion;
            m_mprThreeCoronalSeparateRotationQuaternion      = m_mprThreeRotationSeparateQuaternion;
            m_mprThreeParasagittalSeparateRotationQuaternion = m_mprThreeRotationSeparateQuaternion;
            
            m_mprThreeAxialInverseRotationQuaternion        = QQuaternion();
            m_mprThreeCoronalInverseRotationQuaternion      = QQuaternion();
            m_mprThreeParasagittalInverseRotationQuaternion = QQuaternion();
        }
            break;
        case MprThreeRotationUpdateType::UNCHANGED:
            /*
             * No change to MPR Three rotations
             */
            break;
    }

    float fm[4][4];
    modelTransform.getFlatRotation(fm);
    Matrix4x4 flatRotationMatrix;
    flatRotationMatrix.setMatrix(fm);
    setFlatRotationMatrix(flatRotationMatrix);
    
    const float scale = modelTransform.getScaling();
    setScaling(scale);
    
    float rightFlatX, rightFlatY;
    modelTransform.getRightCortexFlatMapOffset(rightFlatX, rightFlatY);
    setRightCortexFlatMapOffset(rightFlatX, rightFlatY);
    
    const float rightFlatZoom = modelTransform.getRightCortexFlatMapZoomFactor();
    setRightCortexFlatMapZoomFactor(rightFlatZoom);
    
    updateYokedModelBrowserTabs();
}

/**
 * Set the MPR rotation angles if this tab is yoked to the given yoking group (and not OFF)
 * @param yokingGroup
 *    The yoking group
 * @param mprRotationAngles
 *    The MPR rotation angles
 */
void
BrowserTabContent::setMprThreeRotationAnglesForYokingGroup(const YokingGroupEnum::Enum yokingGroup,
                                                           const Vector3D& mprRotationAngles)
{
    if (m_brainModelYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
        if (m_brainModelYokingGroup == yokingGroup) {
            setMprThreeRotationAngles(mprRotationAngles);
            updateBrainModelYokedBrowserTabs();
        }
    }
}

/**
 * Set the MPR rotation angles
 * @param mprRotationAngles
 *    The MPR rotation angles
 */
void
BrowserTabContent::setMprThreeRotationAngles(const Vector3D& mprRotationAngles)
{
    m_mprThreeRotationSeparateQuaternion = QQuaternion::fromEulerAngles(mprRotationAngles[0],
                                                                        mprRotationAngles[1],
                                                                        mprRotationAngles[2]);
    m_mprThreeAxialSeparateRotationQuaternion        = m_mprThreeRotationSeparateQuaternion;
    m_mprThreeCoronalSeparateRotationQuaternion      = m_mprThreeRotationSeparateQuaternion;
    m_mprThreeParasagittalSeparateRotationQuaternion = m_mprThreeRotationSeparateQuaternion;
    
    m_mprThreeAxialInverseRotationQuaternion        = QQuaternion();
    m_mprThreeCoronalInverseRotationQuaternion      = QQuaternion();
    m_mprThreeParasagittalInverseRotationQuaternion = QQuaternion();
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
BrowserTabContent::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrowserTabContent",
                                            9);   // Media image origin moved to top
                                            //8);   // Default image scaling and CZI added
                                            //7); // matrices no longer support translation/zooming
                                            //6); // WB-491 Flat Fixes
                                            //5); // WB-576
                                            //4);  // WB-491, 1/28/2015
                                            //3); // version 3 as of 4/22/2014

    float obliqueMatrix[16];
    m_obliqueVolumeRotationMatrix->getMatrixForOpenGL(obliqueMatrix);
    sceneClass->addFloatArray("m_obliqueVolumeRotationMatrix", obliqueMatrix, 16);
    
    TileTabsBrowserTabGeometry manualLayoutTabGeometry(m_tabNumber);
    m_manualLayoutBrowserTabAnnotation->getTileTabsGeometry(&manualLayoutTabGeometry);
    TileTabsBrowserTabGeometrySceneHelper geometryHelper(&manualLayoutTabGeometry);
    sceneClass->addClass(geometryHelper.saveToScene(sceneAttributes,
                                                    "m_manualLayoutTabGeometry"));

    sceneClass->addChild(m_clippingPlaneGroup->saveToScene(sceneAttributes,
                                                           "m_clippingPlaneGroup"));
    m_sceneClassAssistant->saveMembers(sceneAttributes,
                                       sceneClass);
    
    /*
     * volume surface outline is restored in part 2 of scene restoration
     */
    sceneClass->addChild(m_volumeSurfaceOutlineSetModel->saveToScene(sceneAttributes,
                                                                     "m_volumeSurfaceOutlineSetModel"));

    saveQuaternionToScene(sceneClass, "m_mprThreeRotationSeparateQuaternion", m_mprThreeRotationSeparateQuaternion);
    saveQuaternionToScene(sceneClass, "m_mprThreeAxialSeparateRotationQuaternion", m_mprThreeAxialSeparateRotationQuaternion);
    saveQuaternionToScene(sceneClass, "m_mprThreeCoronalSeparateRotationQuaternion", m_mprThreeCoronalSeparateRotationQuaternion);
    saveQuaternionToScene(sceneClass, "m_mprThreeParasagittalSeparateRotationQuaternion", m_mprThreeParasagittalSeparateRotationQuaternion);

    QQuaternion m_mprThreeRotationQuaternion; /* Only used to identify newer scenes */
    saveQuaternionToScene(sceneClass, "m_mprThreeRotationQuaternion", m_mprThreeRotationQuaternion);
    saveQuaternionToScene(sceneClass, "m_mprThreeAxialInverseRotationQuaternion", m_mprThreeAxialInverseRotationQuaternion);
    saveQuaternionToScene(sceneClass, "m_mprThreeCoronalInverseRotationQuaternion", m_mprThreeCoronalInverseRotationQuaternion);
    saveQuaternionToScene(sceneClass, "m_mprThreeParasagittalInverseRotationQuaternion", m_mprThreeParasagittalInverseRotationQuaternion);

    return sceneClass;
}

/**
 * Save a quaternion to the scene
 * @param sceneClass
 *   The scene class
 * @param memberName
 *   Name of the member
 * @param quaternion
 *   Quaternion that is saved
 */
void
BrowserTabContent::saveQuaternionToScene(SceneClass* sceneClass,
                                         const AString& memberName,
                                         const QQuaternion& quaternion)
{
    /* Scenes use XYZS for quaternion order */
    std::array<float,4> rotQuat(quaternionToArrayXYZS(quaternion));
    sceneClass->addFloatArray(memberName, rotQuat.data(), rotQuat.size());
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void 
BrowserTabContent::restoreFromScene(const SceneAttributes* sceneAttributes,
                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_brainModelYokingGroup = YokingGroupEnum::YOKING_GROUP_A;
    m_chartModelYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    m_mediaModelYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    
    initializeScaleBar();

    m_sceneClassAssistant->restoreMembers(sceneAttributes,
                                          sceneClass);
    
    /*
     * Tab number may change when 'm_sceneClassAssistant->restoreMembers' restores the tab number
     * so need to update the tab number in the scroll bar
     */
    m_scaleBar->setTabIndex(m_tabNumber);
    
    /*
     * Need to recreate clipping plane group since tab is passed
     * to constructor and is constant in the instance
     */
    if (m_clippingPlaneGroup != NULL) {
        delete m_clippingPlaneGroup;
    }
    m_clippingPlaneGroup = new ClippingPlaneGroup(m_tabNumber);
    m_clippingPlaneGroup->restoreFromScene(sceneAttributes,
                                           sceneClass->getClass("m_clippingPlaneGroup"));
    
    TileTabsBrowserTabGeometry manualLayoutTabGeometry(m_tabNumber);
    TileTabsBrowserTabGeometrySceneHelper geometryHelper(&manualLayoutTabGeometry);
    geometryHelper.restoreFromScene(sceneAttributes,
                                    sceneClass->getClass("m_manualLayoutTabGeometry"));
    m_manualLayoutBrowserTabAnnotation->setFromTileTabsGeometry(&manualLayoutTabGeometry);
    m_manualLayoutBrowserTabAnnotation->setBrowserTabContent(this,
                                                             m_tabNumber);
    
    /*
     * With charting version two, yoking was split into chart and non-chart yoking
     * If old yoking group is found, apply it to the brain model yoking group
     */
    const AString oldYokingGroupName = sceneClass->getEnumeratedTypeValueAsString("m_yokingGroup",
                                                                                  "XXXXXX");
    if ( ! oldYokingGroupName.isEmpty()) {
        bool validFlag = false;
        const YokingGroupEnum::Enum oldYokeGroup = YokingGroupEnum::fromName(oldYokingGroupName,
                                                                             &validFlag);
        if (validFlag) {
            m_brainModelYokingGroup = oldYokeGroup;
            m_chartModelYokingGroup = oldYokeGroup;
        }
    }
    
    m_obliqueVolumeRotationMatrix->identity();
    float obliqueMatrix[16];
    const int32_t numInObliqueArray = sceneClass->getFloatArrayValue("m_obliqueVolumeRotationMatrix", obliqueMatrix, 16);
    if (numInObliqueArray == 16) {
        m_obliqueVolumeRotationMatrix->setMatrixFromOpenGL(obliqueMatrix);
    }
    
    /*
     * In older version of workbench, transformation were stored in the
     * model for each tab, so try to restore them.
     */
    if (sceneClass->getVersionNumber() < 2) {
        float translation[3];
        float scaling;
        float rotationMatrix[4][4];
        
        const Model* model = getModelForDisplay();
        if (model != NULL) {
            const bool valid = model->getOldSceneTransformation(m_tabNumber,
                                                                translation,
                                                                scaling,
                                                                rotationMatrix);
            if (valid) {
                setTranslation(translation);
                setScaling(scaling);
                Matrix4x4 m;
                m.setMatrix(rotationMatrix);
                
                ModelSurface* ms = getDisplayedSurfaceModel();
                if (ms != NULL) {
                    /*
                     * Right hemispheres need rotations changed for
                     * proper viewing.
                     */
                    const StructureEnum::Enum structure = ms->getSurface()->getStructure();
                    if (StructureEnum::isRight(structure)) {
                        double rotationX, rotationY, rotationZ;
                        m.getRotation(rotationX,
                                      rotationY,
                                      rotationZ);
                        rotationY = 90 + rotationY;
                        rotationZ = -rotationZ;
                        m.identity();
                        m.setRotation(rotationX,
                                           rotationY,
                                           rotationZ);
                    }
                }
                setRotationMatrix(m);
            }
        }
    }
    
    if (getDisplayedWholeBrainModel() != NULL) {
        /*
         * As of 19sep2013 whole brain and volume slice settings were merged
         * (whole brain slice settings removed).  For compatibility, if a
         * whole brain model is being viewed and whole brain slice settings
         * are found, allow them to override volume slice settings.
         */
        const SceneClass* wholeBrainVolumeSettings = sceneClass->getClass("m_wholeBrainSliceSettings");
        if (wholeBrainVolumeSettings != NULL) {
            VolumeSliceSettings settings;
            settings.restoreFromScene(sceneAttributes, wholeBrainVolumeSettings);
            *m_volumeSliceSettings = settings;
        }
    }
    
    /**
     * Check for now obsolete clipping coordinate array.  If found it is an
     * old scene so update the clipping planes.
     */
    const SceneClassArray* oldClippingCoordinateClassArray = sceneClass->getClassArray("m_clippingCoordinate");
    if (oldClippingCoordinateClassArray != NULL) {
        float clipCoords[3];
        if (sceneClass->getFloatArrayValue("m_clippingCoordinate",
                                           clipCoords,
                                           3) != 3) {
            clipCoords[0] = 0.0;
            clipCoords[1] = 0.0;
            clipCoords[2] = 0.0;
        }
        
        float clipThick[3];
        if (sceneClass->getFloatArrayValue("m_clippingThickness",
                                           clipThick,
                                           3) != 3) {
            clipThick[0] = 0.0;
            clipThick[1] = 0.0;
            clipThick[2] = 0.0;
        }
        
        bool clipEnabled[3];
        if (sceneClass->getBooleanArrayValue("m_clippingEnabled",
                                             clipEnabled,
                                             3) != 3) {
            clipEnabled[0] = false;
            clipEnabled[1] = false;
            clipEnabled[2] = false;
        }
        
        m_clippingPlaneGroup->resetToDefaultValues();
        
        m_clippingPlaneGroup->setXAxisSelected(clipEnabled[0]);
        m_clippingPlaneGroup->setYAxisSelected(clipEnabled[1]);
        m_clippingPlaneGroup->setZAxisSelected(clipEnabled[2]);
        
        m_clippingPlaneGroup->setTranslation(clipCoords);
        m_clippingPlaneGroup->setThickness(clipThick);
    }

    /*
     * In older version of workbench, there was no flat surface
     * viewing transformation as it used the same transformations
     * as other surfaces.
     */
    if (sceneClass->getVersionNumber() < 3) {
        *m_flatSurfaceViewingTransformation = *m_viewingTransformation;
    }
    
    /*
     * Prior to WB-491 surface drawing used the maximum dimension for 
     * scaling to fit the window height and this was almost
     * always the Y-axis.  This worked well when the default view was
     * a dorsal view with the anterior pole was at the top of the display
     * and the posterior pole at the bottom of the display.  However
     * the default view was changed to be a lateral view so the surface
     * did not scale to fit the window and there were problems with the
     * surfaces scaling improperly when the overlay toolbox was changed
     * in height.
     *
     * This code adjusts the surface scaling for older scenes so that 
     * older scenes are restored properly with the newer default scaling
     * for a lateral view.
     *
     * See also: BrainOpenGLFixedPipeline::setOrthographicProjectionForWithBoundingBox()
     */
    if (sceneClass->getVersionNumber() < 4) {
        Surface* surface = NULL;
        switch (getSelectedModelType()) {
            case ModelTypeEnum::MODEL_TYPE_CHART:
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                break;
            case ModelTypeEnum::MODEL_TYPE_INVALID:
                break;
            case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                break;
            case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE:
            {
                ModelSurface* modelSurface = getDisplayedSurfaceModel();
                if (modelSurface != NULL) {
                    surface = modelSurface->getSurface();
                }
            }
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            {
                ModelSurfaceMontage* modelMontage = getDisplayedSurfaceMontageModel();
                if (modelMontage != NULL) {
                    std::vector<SurfaceMontageViewport*> surfaceMontageViewports;
                    modelMontage->getSurfaceMontageViewportsForDrawing(getTabNumber(),
                                                                       surfaceMontageViewports);
                    
                    for (std::vector<SurfaceMontageViewport*>::iterator iter = surfaceMontageViewports.begin();
                         iter != surfaceMontageViewports.end();
                         iter++) {
                        SurfaceMontageViewport* smv = *iter;
                        if (smv->getSurface() != NULL) {
                            surface = smv->getSurface();
                            break;
                        }
                    }
                }
            }
                break;
            case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                break;
            case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            {
                ModelWholeBrain* modelWholeBrain = getDisplayedWholeBrainModel();
                if (modelWholeBrain != NULL) {
                    std::vector<Surface*> allSurfaces = modelWholeBrain->getSelectedSurfaces(getTabNumber());
                    if ( ! allSurfaces.empty()) {
                        surface = allSurfaces[0];
                    }
                }
            }
                break;
        }
        
        if (surface != NULL) {
            if (surface->getSurfaceType() != SurfaceTypeEnum::FLAT) {
                const BoundingBox* boundingBox = surface->getBoundingBox();
                const float zDiff = boundingBox->getDifferenceZ();
                const float maxDim =  std::max(std::max(boundingBox->getDifferenceX(),
                                                        boundingBox->getDifferenceY()),
                                               zDiff);
                if (zDiff > 0.0) {
                    const float scaleAdjustment = zDiff / maxDim;
                    float scaling = getScaling();
                    scaling *= scaleAdjustment;
                    setScaling(scaling);
                }
            }
        }
    }
    
    if (sceneClass->getVersionNumber() < 5) {
        if ( ! m_userName.isEmpty()) {
            /*
             * Prior to version 5 and WB-576,
             * the tab number was ALWAYS displayed.
             */
            m_userName.insert(0,
                              "(" + AString::number(m_tabNumber + 1) + ") ");

        }
    }

    if (sceneClass->getVersionNumber() < 6) {
        /*
         * WB-491 sets the viewport for flat surfaces to fit the 
         * flat surface when viewing a flat montage
         */
        const ModelSurfaceMontage* msm = getDisplayedSurfaceMontageModel();
        if (msm != NULL) {
            const SurfaceMontageConfigurationAbstract* smc = msm->getSelectedConfiguration(m_tabNumber);
            if (smc != NULL) {
                if (smc->getConfigurationType() == SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION) {
                    setScaling(1.0);
                }
            }
        }
    }
    
    if (sceneClass->getVersionNumber() < 8) {
        if (getDisplayedMediaModel() != NULL) {
            MediaOverlay* underlay(getMediaOverlaySet()->getBottomMostEnabledOverlay());
            if (underlay != NULL) {
                const MediaOverlay::SelectionData selectionData(underlay->getSelectionData());
                if (selectionData.m_selectedMediaFile != NULL) {
                    const float imageHeight(selectionData.m_selectedMediaFile->getHeight());
                    if (imageHeight > 0.0) {
                        /*
                         * Old orthographic projection was +/- 500 vertically
                         * and there was no default view/transform.  So
                         * try to make old scenes load correctly.
                         */
                        const float oldViewportHeight(500.0);
                        const float halfImageHeight(imageHeight / 2.0);
                        const float scaleToFitWindow(halfImageHeight / oldViewportHeight);
                        const float sceneScale = m_mediaViewingTransformation->getScaling();

                        setMediaScaling(scaleToFitWindow * sceneScale);
                    }
                }
            }
        }
    }

    if (sceneClass->getVersionNumber() < 9) {
        /*
         * Versions 8 and earlier have origin at bottom.
         * Version 9 moves origin to top with positive-Y moving down.
         */
        if (getDisplayedMediaModel() != NULL) {
            MediaOverlay* underlay(getMediaOverlaySet()->getBottomMostEnabledOverlay());
            if (underlay != NULL) {
                const MediaOverlay::SelectionData selectionData(underlay->getSelectionData());
                if (selectionData.m_selectedMediaFile != NULL) {
                    const float imageWidth(selectionData.m_selectedMediaFile->getWidth());
                    const float imageHeight(selectionData.m_selectedMediaFile->getHeight());
                    
                    /*
                     * Older scenes were drawn in pixels with origin at bottom left.
                     * We don't know the size of the viewport, so we cannot process the
                     * translation correctly.  So, we ignore the translation.  But we
                     * do use the scaling from the scene and try to set the translation
                     * so that the image is centered.
                     *
                     * Code below is from ViewingTransformationsMedia::setMediaScaling().
                     */
                    const float scale(m_mediaViewingTransformation->getScaling());
                    const float halfWidth(-imageWidth / 2.0);
                    const float halfHeight(imageHeight / 2.0);
                    float tx = -((halfWidth * scale) - halfWidth);
                    float ty = -((halfHeight * scale) - halfHeight);
                    m_mediaViewingTransformation->resetView();
                    m_mediaViewingTransformation->setScaling(scale);
                    m_mediaViewingTransformation->setTranslation(tx, -ty, 0.0);
                }
            }
        }
    }

    /*
     * Coordinate display type on volume montages slices
     * from bool to enum on 25 jan 2023.  If not found,
     * default it to 'offset'.
     */
    const SceneObject* volumeMontageCoordinateDisplayTypeObject(sceneClass->getObjectWithName("m_volumeMontageCoordinateDisplayType"));
    if ( ! volumeMontageCoordinateDisplayTypeObject) {
        m_volumeMontageCoordinateDisplayType = VolumeMontageCoordinateDisplayTypeEnum::OFFSET;
    }

    bool tryToRestoreOldMprRotationsFlag(false);
    bool tryToRestoreOldMprRotationsToSeparateFlag(false);

    if ( ! restoreQuaternionFromScene(sceneClass,
                                      "m_mprThreeRotationSeparateQuaternion",
                                      m_mprThreeRotationSeparateQuaternion)) {
        tryToRestoreOldMprRotationsToSeparateFlag = true;
    }
    restoreQuaternionFromScene(sceneClass,
                               "m_mprThreeAxialSeparateRotationQuaternion",
                               m_mprThreeAxialSeparateRotationQuaternion);
    restoreQuaternionFromScene(sceneClass,
                               "m_mprThreeCoronalSeparateRotationQuaternion",
                               m_mprThreeCoronalSeparateRotationQuaternion);
    restoreQuaternionFromScene(sceneClass,
                               "m_mprThreeParasagittalSeparateRotationQuaternion",
                               m_mprThreeParasagittalSeparateRotationQuaternion);
    
    QQuaternion m_mprThreeRotationQuaternion; /* If NOT present, it is older scene */
    if ( ! restoreQuaternionFromScene(sceneClass,
                                      "m_mprThreeRotationQuaternion",
                                      m_mprThreeRotationQuaternion)) {
        tryToRestoreOldMprRotationsFlag = true;
    }
    restoreQuaternionFromScene(sceneClass,
                               "m_mprThreeAxialInverseRotationQuaternion",
                               m_mprThreeAxialInverseRotationQuaternion);
    restoreQuaternionFromScene(sceneClass,
                               "m_mprThreeCoronalInverseRotationQuaternion",
                               m_mprThreeCoronalInverseRotationQuaternion);
    restoreQuaternionFromScene(sceneClass,
                               "m_mprThreeParasagittalInverseRotationQuaternion",
                               m_mprThreeParasagittalInverseRotationQuaternion);
    
    if (tryToRestoreOldMprRotationsFlag) {
        Matrix4x4 rotationMatrix;
        Matrix4x4 axialInverseRotationMatrix;
        Matrix4x4 coronalInverseRotationMatrix;
        Matrix4x4 parasagittalInverseRotationMatrix;
        
        if (m_mprRotationX != 0.0) {
            if (m_mprRotationY != 0.0) {
                if (m_mprRotationZ != 0.0) {
                    /* All three rotations are set */
                    rotationMatrix.setRotation(-m_mprRotationX,
                                               -m_mprRotationY,
                                               m_mprRotationZ);
                }
                else {
                    /* X and Y rotations are set, no Z rotation*/
                    rotationMatrix.setRotation(-m_mprRotationX,
                                               -m_mprRotationY,
                                               0.0);
                    parasagittalInverseRotationMatrix.setRotation(m_mprRotationX,
                                                                  0.0,
                                                                  0.0);
                    coronalInverseRotationMatrix.setRotation(0.0,
                                                             m_mprRotationY,
                                                             0.0);
                }
            }
            else {
                /* X rotation only */
                rotationMatrix.setRotation(-m_mprRotationX, 0.0, 0.0);
                parasagittalInverseRotationMatrix.setRotation(m_mprRotationX, 0.0, 0.0);
            }
        }
        else if (m_mprRotationY != 0.0) {
            if (m_mprRotationZ != 0.0) {
                /* Y and Z rotation set*/
                rotationMatrix.setRotation(0.0,
                                           -m_mprRotationY,
                                           m_mprRotationZ);
                coronalInverseRotationMatrix.setRotation(0.0,
                                                         m_mprRotationY,
                                                         0.0);
                axialInverseRotationMatrix.setRotation(0.0,
                                                       0.0,
                                                       -m_mprRotationZ);
            }
            else {
                /* Y  rotation only */
                rotationMatrix.setRotation(0.0,
                                           -m_mprRotationY,
                                           0.0);
                coronalInverseRotationMatrix.setRotation(0.0,
                                                         m_mprRotationY,
                                                         0.0);
            }
        }
        else if (m_mprRotationZ != 0.0) {
            /* Z rotation only */
            rotationMatrix.setRotation(0.0,
                                       0.0,
                                       m_mprRotationZ);
            axialInverseRotationMatrix.setRotation(0.0,
                                                   0.0,
                                                   -m_mprRotationZ);
        }
        m_mprThreeAxialInverseRotationQuaternion = matrixToQuaternion(axialInverseRotationMatrix);
        m_mprThreeCoronalInverseRotationQuaternion = matrixToQuaternion(coronalInverseRotationMatrix);
        m_mprThreeParasagittalInverseRotationQuaternion = matrixToQuaternion(parasagittalInverseRotationMatrix);
    }
    
    if (tryToRestoreOldMprRotationsToSeparateFlag) {
        Matrix4x4 rotationMatrix;
        Matrix4x4 axialRotationMatrix;
        Matrix4x4 coronalRotationMatrix;
        Matrix4x4 parasagittalRotationMatrix;
        
        rotationMatrix.setRotation(m_mprRotationX, m_mprRotationY, -m_mprRotationZ);

        if (m_mprRotationX != 0.0) {
            if (m_mprRotationY != 0.0) {
                if (m_mprRotationZ != 0.0) {
                    /* All three rotations are set */
                    rotationMatrix.setRotation(m_mprRotationX, m_mprRotationY, -m_mprRotationZ);
                    axialRotationMatrix        = rotationMatrix;
                    coronalRotationMatrix      = rotationMatrix;
                    parasagittalRotationMatrix = rotationMatrix;
                }
                else {
                    /* X and Y rotations are set, no Z rotation*/
                    rotationMatrix.setRotation(m_mprRotationX, m_mprRotationY, 0.0);
                    axialRotationMatrix        = rotationMatrix;
                    coronalRotationMatrix      = rotationMatrix;
                    parasagittalRotationMatrix = rotationMatrix;
                }
            }
            else {
                /* X rotation only */
                rotationMatrix.setRotation(m_mprRotationX, 0.0, 0.0);
                axialRotationMatrix.setRotation(m_mprRotationX, 0.0, 0.0);
                coronalRotationMatrix.setRotation(m_mprRotationX, 0.0, 0.0);
                parasagittalRotationMatrix.identity();
            }
        }
        else if (m_mprRotationY != 0.0) {
            if (m_mprRotationZ != 0.0) {
                /* Y and Z rotation set*/
                rotationMatrix.setRotation(0.0, m_mprRotationY, -m_mprRotationZ);
                axialRotationMatrix        = rotationMatrix;
                coronalRotationMatrix      = rotationMatrix;
                parasagittalRotationMatrix = rotationMatrix;
            }
            else {
                /* Y  rotation only */
                rotationMatrix.setRotation(0.0, m_mprRotationY, 0.0);
                axialRotationMatrix.setRotation(0.0, m_mprRotationY, 0.0);
                coronalRotationMatrix.identity();
                parasagittalRotationMatrix.setRotation(0.0, m_mprRotationY, 0.0);
            }
        }
        else if (m_mprRotationZ != 0.0) {
            /* Z rotation only */
            rotationMatrix.setRotation(0.0, 0.0, -m_mprRotationZ);
            axialRotationMatrix.identity();
            coronalRotationMatrix.setRotation(0.0, 0.0, -m_mprRotationZ);
            parasagittalRotationMatrix.setRotation(0.0, 0.0, -m_mprRotationZ);
        }
        
        m_mprThreeRotationSeparateQuaternion = matrixToQuaternion(rotationMatrix);
        m_mprThreeAxialSeparateRotationQuaternion = matrixToQuaternion(axialRotationMatrix);
        m_mprThreeCoronalSeparateRotationQuaternion = matrixToQuaternion(coronalRotationMatrix);
        m_mprThreeParasagittalSeparateRotationQuaternion = matrixToQuaternion(parasagittalRotationMatrix);
    }


    testForRestoreSceneWarnings(sceneAttributes,
                                sceneClass->getVersionNumber());
}

/**
 * Restore a quaternion
 * @param sceneClass
 *   The scene class
 * @param memberName
 *   Name of the quaternion class member
 * @param quaternion
 *   Quaternion that is restored
 * @return
 *   True if the quaternion was restored else false and quaternion is set to the default value
 */
bool
BrowserTabContent::restoreQuaternionFromScene(const SceneClass* sceneClass,
                                              const AString& memberName,
                                              QQuaternion& quaternion) const
{
    if (sceneClass->getObjectWithName(memberName)) {
        std::array<float,4> rotValues;
        sceneClass->getFloatArrayValue(memberName,
                                       rotValues.data(),
                                       rotValues.size());
        /* Scene uses quaterion in order XYZS */
        quaternion = arrayXYZSToQuaternion(rotValues);
        return true;
    }

    quaternion = QQuaternion();
    return false;
}

/**
 * Part TWO Restore the state of an instance of a class.
 * Some members may refer to other tabs so these members must be
 * restored AFTER ALL OTHER TABS have been restored.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
BrowserTabContent::restoreFromScenePartTwo(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    m_volumeSurfaceOutlineSetModel->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_volumeSurfaceOutlineSetModel"));
}

/**
 * Test for scene warnings
 */
void
BrowserTabContent::testForRestoreSceneWarnings(const SceneAttributes* sceneAttributes,
                                               const int32_t sceneVersion)
{
    if (sceneVersion <= 6) {
        ModelChartTwo* chartModel = getDisplayedChartTwoModel();
        if (chartModel != NULL) {
            if (chartModel->getSelectedChartTwoDataType(m_tabNumber) == ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX) {
                float translation[3];
                getTranslation(translation);
                const float zoom = getScaling();
                if (MathFunctions::compareValuesEqual(translation, 2, 0.0, 0.001)
                    && MathFunctions::compareValuesEqual(&zoom, 1, 1.0, 0.001)) {
                    /* OK */
                }
                else {
                    sceneAttributes->setSceneRestoreWarningCode(SceneRestoreWarningCodesEnum::CHART_TWO_MATRIX_TRANSFORM);
                }
            }
        }
    }
}
 
/**
 * Get the clipping planes enabled attributes
 *
 * @param xEnabled
 *    X clipping plane enabled.
 * @param yEnabled
 *    Y clipping plane enabled.
 * @param zEnabled
 *    Z clipping plane enabled.
 * @param surfaceEnabled
 *    Surface clipping enabled.
 * @param volumeEnabled
 *    Volume clipping enabled.
 * @param featuresEnabled
 *    Features enabled.
 * @param displayClippingBox
 *    Display the clipping box.
 */
void
BrowserTabContent::getClippingPlaneEnabled(bool& xEnabled,
                                           bool& yEnabled,
                                           bool& zEnabled,
                                           bool& surfaceEnabled,
                                           bool& volumeEnabled,
                                           bool& featuresEnabled) const
{
    xEnabled = m_clippingPlaneGroup->isXAxisSelected();
    yEnabled = m_clippingPlaneGroup->isYAxisSelected();
    zEnabled = m_clippingPlaneGroup->isZAxisSelected();
    
    surfaceEnabled = m_clippingPlaneGroup->isSurfaceSelected();
    volumeEnabled  = m_clippingPlaneGroup->isVolumeSelected();
    featuresEnabled = m_clippingPlaneGroup->isFeaturesSelected();
}

/**
 * Set the clipping planes enabled attributes
 *
 * @param xEnabled
 *    X clipping plane enabled.
 * @param yEnabled
 *    Y clipping plane enabled.
 * @param zEnabled
 *    Z clipping plane enabled.
 * @param surfaceEnabled
 *    Surface clipping enabled.
 * @param volumeEnabled
 *    Volume clipping enabled.
 * @param featuresEnabled
 *    Features enabled.
 * @param displayClippingBox
 *    Display the clipping box.
 */
void
BrowserTabContent::setClippingPlaneEnabled(const bool xEnabled,
                                           const bool yEnabled,
                                           const bool zEnabled,
                                           const bool surfaceEnabled,
                                           const bool volumeEnabled,
                                           const bool featuresEnabled)
{
    m_clippingPlaneGroup->setXAxisSelected(xEnabled);
    m_clippingPlaneGroup->setYAxisSelected(yEnabled);
    m_clippingPlaneGroup->setZAxisSelected(zEnabled);
    
    m_clippingPlaneGroup->setSurfaceSelected(surfaceEnabled);
    m_clippingPlaneGroup->setVolumeSelected(volumeEnabled);
    m_clippingPlaneGroup->setFeaturesSelected(featuresEnabled);

    updateYokedModelBrowserTabs();
}

/**
 * Get the clipping planes transformations.
 *
 * @param panning
 *    Panning (translation) of the clipping planes.
 * @param rotation
 *    Rotation of clipping planes.
 * @param thickness
 *    Thickness of the clipping planes.
 */
void
BrowserTabContent::getClippingPlaneTransformation(float panning[3],
                                                  float rotation[3],
                                                  float thickness[3],
                                                  bool& displayClippingBox) const
{
    m_clippingPlaneGroup->getTranslation(panning);
    m_clippingPlaneGroup->getRotationAngles(rotation);
    m_clippingPlaneGroup->getThickness(thickness);
    
    displayClippingBox = m_clippingPlaneGroup->isDisplayClippingBoxSelected();
}

/**
 * Set the clipping planes transformations.
 *
 * @param panning
 *    Panning (translation) of the clipping planes.
 * @param rotation
 *    Rotation of clipping planes.
 * @param thickness
 *    Thickness of the clipping planes.
 */
void
BrowserTabContent::setClippingPlaneTransformation(const float panning[3],
                                                  const float rotation[3],
                                                  const float thickness[3],
                                                  const bool displayClippingBox)
{
    m_clippingPlaneGroup->setTranslation(panning);
    m_clippingPlaneGroup->setRotationAngles(rotation);
    m_clippingPlaneGroup->setThickness(thickness);
    
    m_clippingPlaneGroup->setDisplayClippingBoxSelected(displayClippingBox);

    updateYokedModelBrowserTabs();
}


/**
 * Get the clipping plane group (const method).
 *
 * NOTE: Because of yoking, only a const instance of the clipping plane
 * group is available.  To adjust the clipping planes use the methods
 * in this class so that yoking is properly updated.
 */
const ClippingPlaneGroup*
BrowserTabContent::getClippingPlaneGroup() const
{
    return m_clippingPlaneGroup;
}

/**
 * Reset the clipping plane transformations.
 */
void
BrowserTabContent::resetClippingPlaneTransformation()
{
    m_clippingPlaneGroup->resetTransformation();
    updateYokedModelBrowserTabs();
}

/**
 * @return Clipping planes enabled
 */
bool BrowserTabContent::isClippingPlanesEnabled()
{
    return m_clippingPlaneGroup->isEnabled();
}

/**
 * Set clipping planes enabled
 * @param status
 *  New enabled status
 */
void
BrowserTabContent::setClippingPlanesEnabled(const bool status)
{
    m_clippingPlaneGroup->setEnabled(status);
}

/**
 * Set clipping planes enabled
 * If toggled on and no planes are enabled, enable all planes
 * @param status
 *   New enabled status
 */
void
BrowserTabContent::setClippingPlanesEnabledAndEnablePlanes(const bool status)
{
    m_clippingPlaneGroup->setEnabledAndEnablePlanes(status);
}

/**
 * Set the clipping panning mode
 * @param panningMode
 *    New panning mode
 */
void
BrowserTabContent::setClippingPanningMode(const ClippingPlanePanningModeEnum::Enum panningMode)
{
    m_clippingPlaneGroup->setPanningMode(panningMode);
    updateYokedModelBrowserTabs();
}

/**
 * @return The clipping panning mode
 */
ClippingPlanePanningModeEnum::Enum
BrowserTabContent::getClippingPanningMode() const
{
    return m_clippingPlaneGroup->getPanningMode();
}

/**
 * @return the projection view type (view from left/right)
 */
ProjectionViewTypeEnum::Enum
BrowserTabContent::getProjectionViewType() const
{
    ProjectionViewTypeEnum::Enum projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL;
    
    const ModelSurface* modelSurface = getDisplayedSurfaceModel();
    if (modelSurface != NULL) {
        const SurfaceFile* surfaceFile = modelSurface->getSurface();
        if (surfaceFile != NULL) {
            const StructureEnum::Enum structure = surfaceFile->getStructure();
            const SurfaceTypeEnum::Enum surfaceType = surfaceFile->getSurfaceType();
            if (StructureEnum::isRight(structure)) {
                projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL;
                if (surfaceType == SurfaceTypeEnum::FLAT) {
                    projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE;
                }
            }
            else {
                projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL;
                if (surfaceType == SurfaceTypeEnum::FLAT) {
                    projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE;
                }
            }
        }
    }
    
    return projectionViewType;
}

/**
 * @return MPR Orientation mode
 */
VolumeMprOrientationModeEnum::Enum
BrowserTabContent::getVolumeMprOrientationMode() const
{
    return m_volumeSliceSettings->getMprSettings()->getOrientationMode();
}

/**
 * Set the MPR Orientation mode
 * @param orientationMode
 *   New orientation mode
 */
void
BrowserTabContent::setVolumeMprOrientationMode(const VolumeMprOrientationModeEnum::Enum orientationMode)
{
    m_volumeSliceSettings->getMprSettings()->setOrientationMode(orientationMode);
    updateBrainModelYokedBrowserTabs();
}

VolumeMontageSliceOrderModeEnum::Enum
BrowserTabContent::getVolumeMontageSliceOrderMode() const
{
    return m_volumeSliceSettings->getMontageSliceOrderMode();
}

void
BrowserTabContent::setVolumeMontageSliceOrderMode(const VolumeMontageSliceOrderModeEnum::Enum sliceDirectionMode)
{
    m_volumeSliceSettings->setMontageSliceOrderMode(sliceDirectionMode);
    updateBrainModelYokedBrowserTabs();
}

/**
 * Is the order of the slices in the volume monage flipped (inverted)
 * @param sliceViewPlane
 *    The slice view plane
 * @return
 *    True if order should be flipped (first slice drawn at bottom right), else false (first slice drawn at top left)
 */
bool
BrowserTabContent::isVolumeMontageSliceOrderFlippedForSliceViewPlane(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const
{
    bool flipFlag(false);
    
    const bool mprFlag(getVolumeSliceProjectionType() == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE);
    
    const VolumeMontageSliceOrderModeEnum::Enum direction(getVolumeMontageSliceOrderMode());
    switch (direction) {
        case VolumeMontageSliceOrderModeEnum::NEGATIVE:
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    flipFlag = false;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    flipFlag = false;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    if (mprFlag) {
                        flipFlag = false;
                    }
                    else {
                        flipFlag = true;
                    }
                    break;
            }
            break;
        case VolumeMontageSliceOrderModeEnum::POSITIVE:
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    flipFlag = true;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    flipFlag = true;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    if (mprFlag) {
                        flipFlag = true;
                    }
                    else {
                        flipFlag = false;
                    }
                    break;
            }
            break;
        case VolumeMontageSliceOrderModeEnum::WORKBENCH:
            flipFlag = false;
            break;
    }

    return flipFlag;
}

/**
 * @return MPR volume view mode
 */
VolumeMprViewModeEnum::Enum
BrowserTabContent::getVolumeMprViewMode() const
{
    return m_volumeSliceSettings->getMprSettings()->getViewMode();
}

/**
 * Set MPR volume view mode
 * @param viewMode
 *    New mode
 */
void
BrowserTabContent::setVolumeMprViewMode(const VolumeMprViewModeEnum::Enum viewMode)
{
    m_volumeSliceSettings->getMprSettings()->setViewMode(viewMode);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return MPR volume slice thickness
 */
float
BrowserTabContent::getVolumeMprSliceThickness() const
{
    return m_volumeSliceSettings->getMprSettings()->getSliceThickness();
}

/**
 * Set MPR volume slice thickness
 * @param sliceThickness
 *    New slice thickness
 */
void
BrowserTabContent::setVolumeMprSliceThickness(const float sliceThickness)
{
    m_volumeSliceSettings->getMprSettings()->setSliceThickness(sliceThickness);
    updateYokedModelBrowserTabs();
}

/**
 * @return Is ALL view thickness enabled
 */
bool
BrowserTabContent::isVolumeMprAllViewThicknessEnabled() const
{
    return m_volumeSliceSettings->getMprSettings()->isAllViewThicknessEnabled();
}

/**
 * Set the ALL view thickness enabled
 * @param enabled
 *    New status
 */
void
BrowserTabContent::setVolumeMprAllViewThicknessEnabled(const bool enabled)
{
    m_volumeSliceSettings->getMprSettings()->setAllViewThicknessEnabled(enabled);
}

/**
 * @return Is Volume MPR Axial slice thickness enabled
 */
bool
BrowserTabContent::isVolumeMprAxialSliceThicknessEnabled() const
{
    return m_volumeSliceSettings->getMprSettings()->isAxialSliceThicknessEnabled();
}

/**
 * Set Volume MPR axial slice thickness enabled
 */
void
BrowserTabContent::setVolumeMprAxialSliceThicknessEnabled(const bool enabled)
{
    m_volumeSliceSettings->getMprSettings()->setAxialSliceThicknessEnabled(enabled);
    updateYokedModelBrowserTabs();
}

/**
 * @return Is Volume MPR Coronal slice thickness enabled
 */
bool
BrowserTabContent::isVolumeMprCoronalSliceThicknessEnabled() const
{
    return m_volumeSliceSettings->getMprSettings()->isCoronalSliceThicknessEnabled();
}

/**
 * Set Volume MPR coronal slice thickness enabled
 */
void
BrowserTabContent::setVolumeMprCoronalSliceThicknessEnabled(const bool enabled)
{
    m_volumeSliceSettings->getMprSettings()->setCoronalSliceThicknessEnabled(enabled);
    updateYokedModelBrowserTabs();
}

/**
 * @return Is Volume MPR Parasagittal slice thickness enabled
 */
bool
BrowserTabContent::isVolumeMprParasagittalSliceThicknessEnabled() const
{
    return m_volumeSliceSettings->getMprSettings()->isParasagittalSliceThicknessEnabled();
}

/**
 * Set Volume MPR slice parasagital thickness enabled
 * @param enabled
 *    New status
 */
void
BrowserTabContent::setVolumeMprParasagittalSliceThicknessEnabled(const bool enabled)
{
    m_volumeSliceSettings->getMprSettings()->setParasagittalSliceThicknessEnabled(enabled);
    updateYokedModelBrowserTabs();
}

/**
 * @return The slice view plane.
 *
 */
VolumeSliceViewPlaneEnum::Enum
BrowserTabContent::getVolumeSliceViewPlane() const
{
    return m_volumeSliceSettings->getSliceViewPlane();
}

/**
 * Set the slice view plane.
 * @param windowTabNumber
 *    New value for slice plane.
 */
void
BrowserTabContent::setVolumeSliceViewPlane(const VolumeSliceViewPlaneEnum::Enum slicePlane)
{
    m_volumeSliceSettings->setSliceViewPlane(slicePlane);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return The layout for all slices view (grid, row, column)
 */
VolumeSliceViewAllPlanesLayoutEnum::Enum
BrowserTabContent::getVolumeSlicePlanesAllViewLayout() const
{
    return m_volumeSliceSettings->getSlicePlanesAllViewLayout();
}

/**
 * Set the layout for all slices view (grid, row, column)
 *
 * @param slicePlanesAllViewLayout
 *     New value for layout.
 */
void
BrowserTabContent::setVolumeSlicePlanesAllViewLayout(const VolumeSliceViewAllPlanesLayoutEnum::Enum slicePlanesAllViewLayout)
{
    m_volumeSliceSettings->setSlicePlanesAllViewLayout(slicePlanesAllViewLayout);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Type of slice drawing (single/montage)
 */
VolumeSliceDrawingTypeEnum::Enum
BrowserTabContent::getVolumeSliceDrawingType() const
{
    return m_volumeSliceSettings->getSliceDrawingType();
}

/**
 * Set type of slice drawing (single/montage)
 *
 * @param sliceDrawingType
 *    New value for slice drawing type.
 */
void
BrowserTabContent::setVolumeSliceDrawingType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType)
{
    m_volumeSliceSettings->setSliceDrawingType(sliceDrawingType);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Selected type of slice projection (oblique/orthogonal)
 */
VolumeSliceProjectionTypeEnum::Enum
BrowserTabContent::getVolumeSliceProjectionType() const
{
    std::vector<VolumeSliceProjectionTypeEnum::Enum> sliceProjectionTypes;
    getValidVolumeSliceProjectionTypes(sliceProjectionTypes);
    
    /*
     * Selected projection type may not be valid and needs to be set to a valid projection type
     */
    VolumeSliceProjectionTypeEnum::Enum projType = m_volumeSliceSettings->getSliceProjectionType();
    if (std::find(sliceProjectionTypes.begin(),
                  sliceProjectionTypes.end(),
                  projType) == sliceProjectionTypes.end()) {
        if ( ! sliceProjectionTypes.empty()) {
            CaretAssertVectorIndex(sliceProjectionTypes, 0);
            projType = sliceProjectionTypes[0];
            const_cast<VolumeSliceSettings*>(m_volumeSliceSettings)->setSliceProjectionType(projType);
        }
    }
    
    return projType;
}

/**
 * Get valid slice projection types (ortho is NOT valid if any volume is oblique only)
 * @param sliceProjectionTypesOut
 *    Output containing valid slice projection types based upon selected files in overlays
 */
void
BrowserTabContent::getValidVolumeSliceProjectionTypes(std::vector<VolumeSliceProjectionTypeEnum::Enum>& sliceProjectionTypesOut) const
{
    sliceProjectionTypesOut.clear();

    std::vector<VolumeSliceProjectionTypeEnum::Enum> allSliceProjectionTypes;
    VolumeSliceProjectionTypeEnum::getAllEnums(allSliceProjectionTypes);
    
    bool orthoValidFlag(false);
    for (auto spt : allSliceProjectionTypes) {
        switch (spt) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                switch (getSelectedModelType()) {
                    case ModelTypeEnum::MODEL_TYPE_CHART:
                    case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                    case ModelTypeEnum::MODEL_TYPE_INVALID:
                    case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                    case ModelTypeEnum::MODEL_TYPE_SURFACE:
                    case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                        orthoValidFlag = true;
                        break;
                    case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                    case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                    {
                        /*
                         * Note: OverlaySet will be NULL if loading a scene
                         * and the volume file(s) in the scene are missing (do not exist)
                         */
                        const OverlaySet* overlaySet = getOverlaySet();
                        if (overlaySet != NULL) {
                            if ( ! overlaySet->hasObliqueOnlyVolumeSelected()) {
                                orthoValidFlag = true;
                            }
                        }
                    }
                        break;
                }
                break;
        }
    }
    
    if (orthoValidFlag) {
        sliceProjectionTypesOut.push_back(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL);
    }
    sliceProjectionTypesOut.push_back(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE);
    sliceProjectionTypesOut.push_back(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE);
    if (SessionManager::get()->hasSceneWithMprOld()) {
        sliceProjectionTypesOut.push_back(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR);
    }
}

/**
 * Set type of slice projection (oblique/orthogonal)
 *
 * @param sliceProjectionType
 *    New value for slice projection type.
 */
void
BrowserTabContent::setVolumeSliceProjectionType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType)
{
    m_volumeSliceSettings->setSliceProjectionType(sliceProjectionType);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return The masking used when drawing an oblique volume slice
 */
VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum
BrowserTabContent::getVolumeSliceInterpolationEdgeEffectsMaskingType() const
{
    return m_volumeSliceSettings->getVolumeSliceInterpolationEdgeEffectsMaskingType();
}

/**
 * Set the masking used when drawing an oblique volume slice.
 *
 * @param maskingType
 *     Type of masking.
 */
void
BrowserTabContent::setVolumeSliceInterpolationEdgeEffectsMaskingType(const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType)
{
    m_volumeSliceSettings->setVolumeSliceInterpolationEdgeEffectsMaskingType(maskingType);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return the montage number of columns for the given window tab.
 */
int32_t
BrowserTabContent::getVolumeMontageNumberOfColumns() const
{
    return m_volumeSliceSettings->getMontageNumberOfColumns();
}

/**
 * Set the montage number of columns in the given window tab.
 * @param montageNumberOfColumns
 *    New value for montage number of columns
 */
void
BrowserTabContent::setVolumeMontageNumberOfColumns(const int32_t montageNumberOfColumns)
{
    m_volumeSliceSettings->setMontageNumberOfColumns(montageNumberOfColumns);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return the montage number of rows for the given window tab.
 */
int32_t
BrowserTabContent::getVolumeMontageNumberOfRows() const
{
    return m_volumeSliceSettings->getMontageNumberOfRows();
}

/**
 * Set the montage number of rows.
 * @param montageNumberOfRows
 *    New value for montage number of rows
 */
void
BrowserTabContent::setVolumeMontageNumberOfRows(const int32_t montageNumberOfRows)
{
    m_volumeSliceSettings->setMontageNumberOfRows(montageNumberOfRows);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return the montage slice spacing.
 */
int32_t
BrowserTabContent::getVolumeMontageSliceSpacing() const
{
    return m_volumeSliceSettings->getMontageSliceSpacing();
}

/**
 * Set the montage slice spacing.
 * @param montageSliceSpacing
 *    New value for montage slice spacing
 */
void
BrowserTabContent::setVolumeMontageSliceSpacing(const int32_t montageSliceSpacing)
{
    m_volumeSliceSettings->setMontageSliceSpacing(montageSliceSpacing);
    updateBrainModelYokedBrowserTabs();
}

/**
 * Set the slice indices so that they are at the origin.  However, if the coordinate (0, 0, 0) is outside
 * of the volume, select coordinate at the middle of the volume.
 */
void
BrowserTabContent::selectVolumeSlicesAtOrigin()
{
    selectVolumeSlicesAtOriginPrivate();
    updateBrainModelYokedBrowserTabs();
}

/**
 * Set the slice indices so that they are at the origin.  However, if the coordinate (0, 0, 0) is outside
 * of the volume, select coordinate at the middle of the volume.
 */
void
BrowserTabContent::selectVolumeSlicesAtOriginPrivate()
{
    VolumeMappableInterface* volumeInterface(NULL);
    if (m_volumeModel != NULL) {
        volumeInterface = m_volumeModel->getOverlaySet(m_tabNumber)->getUnderlayVolume();
    }
    
    m_volumeSliceSettings->selectSlicesAtOrigin(volumeInterface);
}

/**
 * Set the selected slices to the given coordinate.
 * @param xyz
 *    Coordinate for selected slices.
 */
void
BrowserTabContent::selectVolumeSlicesAtCoordinate(const float xyz[3])
{
    m_volumeSliceSettings->selectSlicesAtCoordinate(xyz);
    updateBrainModelYokedBrowserTabs();
}

/**
 * If true, selected volume slices in tab move to location
 * of the identification operation.
 */
bool
BrowserTabContent::isIdentificationUpdatesVolumeSlices() const
{
    return m_identificationUpdatesVolumeSlices;
}

/**
 * Update selected volume slices in tab move to location
 * of the identification operation.
 *
 * @param status
 *    New status.
 */
void
BrowserTabContent::setIdentificationUpdatesVolumeSlices(const bool status)
{
    m_identificationUpdatesVolumeSlices = status;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Is volume axis crosshairs displayed
 */
bool
BrowserTabContent::isVolumeAxesCrosshairsDisplayed() const
{
    return m_displayVolumeAxesCrosshairs;
}

/**
 * Set volume axis crosshairs displayed
 *
 * @param displayed
 *     New status
 */
void
BrowserTabContent::setVolumeAxesCrosshairsDisplayed(const bool displayed)
{
    m_displayVolumeAxesCrosshairs = displayed;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Is volume axis crosshairs labels displayed
 */
bool
BrowserTabContent::isVolumeAxesCrosshairLabelsDisplayed() const
{
    return m_displayVolumeAxesCrosshairLabels;
}

/**
 * Set volume axis crosshairs labels displayed
 *
 * @param displayed
 *     New status
 */
void
BrowserTabContent::setVolumeAxesCrosshairLabelsDisplayed(const bool displayed)
{
    m_displayVolumeAxesCrosshairLabels = displayed;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Is volume montage axes coordinates displayed
 */
bool
BrowserTabContent::isVolumeMontageAxesCoordinatesDisplayed() const
{
    return m_displayVolumeMontageAxesCoordinates;
}

/**
 * Set volume montage axes coordinates displayed
 *
 * @param displayed
 *     New status
 */
void
BrowserTabContent::setVolumeMontageAxesCoordinatesDisplayed(const bool displayed)
{
    m_displayVolumeMontageAxesCoordinates = displayed;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Type of coordinate displayed on volume montage slices
 */
VolumeMontageCoordinateDisplayTypeEnum::Enum
BrowserTabContent::getVolumeMontageCoordinatesDislayType() const
{
    return m_volumeMontageCoordinateDisplayType;
}

/**
 * Set the type of coordinate displayed on volume montage slices
 * @param displayType
 *    Type to display
 */
void
BrowserTabContent::setVolumeMontageCoordinateDisplayType(const VolumeMontageCoordinateDisplayTypeEnum::Enum displayType)
{
    m_volumeMontageCoordinateDisplayType = displayType;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Digits right of decimal for montage coordinates
 */
int32_t
BrowserTabContent::getVolumeMontageCoordinatePrecision() const
{
    return m_volumeMontageCoordinatePrecision;
}

/**
 * Set digits right of decimal for montage coordinates
 *
 * @param volumeMontageCoordinatePrecision
 *     New precision
 */
void
BrowserTabContent::setVolumeMontageCoordinatePrecision(const int32_t volumeMontageCoordinatePrecision)
{
    m_volumeMontageCoordinatePrecision = volumeMontageCoordinatePrecision;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Font height for montage coordinates
 */
float
BrowserTabContent::getVolumeMontageCoordinateFontHeight() const
{
    return m_volumeMontageCoordinateFontHeight;
}

/**
 * Set font height for montage coordinates
 *
 * @param volumeMontageCoordinateFontHeight
 *     New font height
 */
void
BrowserTabContent::setVolumeMontageCoordinateFontHeight(const float volumeMontageCoordinateFontHeight)
{
    m_volumeMontageCoordinateFontHeight = volumeMontageCoordinateFontHeight;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Alignment for volume montage coordinate text
 */
VolumeMontageCoordinateTextAlignmentEnum::Enum
BrowserTabContent::getVolumeMontageCoordinateTextAlignment() const
{
    return m_volumeMontageCoordinateTextAlignment;
}

/**
 * Set alignment for volume montage coordinate text
 * @param alignmnent
 *    New text alignment
 */
void
BrowserTabContent::setVolumeMontageCoordinateTextAlignment(const VolumeMontageCoordinateTextAlignmentEnum::Enum alignment)
{
    m_volumeMontageCoordinateTextAlignment = alignment;
    updateBrainModelYokedBrowserTabs();
}

/**
 * If true, selected histology slices in tab move to location
 * of the identification operation.
 */
bool
BrowserTabContent::isIdentificationUpdateHistologySlices() const
{
    return m_identificationUpdatesHistologySlices;
}

/**
 * Update selected histology slices in tab move to location
 * of the identification operation.
 *
 * @param status
 *    New status.
 */
void
BrowserTabContent::setIdentificationUpdatesHistologySlices(const bool status)
{
    m_identificationUpdatesHistologySlices = status;
    updateBrainModelYokedBrowserTabs();
}
/**
 * Set the histology slice indices so that they are at the origin.
 */
void
BrowserTabContent::selectHistologySlicesAtOrigin(const HistologySlicesFile* histologySlicesFile)
{
    m_histologySliceSettings->selectSlicesAtCenter(histologySlicesFile);
    updateHistologyModelYokedBrowserTabs();
}

/**
 * @return The selected histology coordinate
 */
HistologyCoordinate
BrowserTabContent::getHistologySelectedCoordinate(const HistologySlicesFile* histologySlicesFile) const
{
    return m_histologySliceSettings->getHistologyCoordinate(histologySlicesFile);
}

/**
 * She selected histology coordinate
 * @param histologySlicesFile
 *    The histology slices file
 * @param histologyCoordinate
 *    New value
 * @param moveYokedVolumeSlices
 *    Indicates if yoked volume slices should be moved to same coordinate as histology slices
 */
void
BrowserTabContent::setHistologySelectedCoordinate(const HistologySlicesFile* histologySlicesFile,
                                                  const HistologyCoordinate& histologyCoordinate,
                                                  const MoveYokedVolumeSlices moveYokedVolumeSlices)
{
    m_histologySliceSettings->setHistologyCoordinate(histologyCoordinate);
    updateHistologyModelYokedBrowserTabs();
    
    switch (moveYokedVolumeSlices) {
        case MoveYokedVolumeSlices::MOVE_NO:
            break;
        case MoveYokedVolumeSlices::MOVE_YES:
            moveYokedVolumeSlicesToHistologyCoordinate(getHistologySelectedCoordinate(histologySlicesFile));
            break;
    }
}

/**
 * @return Are the histology orientation angles applied to yoking?
 */
bool
BrowserTabContent::isHistologyOrientationAppliedToYoking() const
{
    return m_histologyOrientationAppliedToYokingFlag;
}

/**
 * Set the histology orientation angles applied to yoking
 * @param status
 *    New status
 */
void
BrowserTabContent::setHistologyOrientationAppliedToYoking(const bool status)
{
    m_histologyOrientationAppliedToYokingFlag = status;
}

/**
 * Apply histology yoking of orientation to MPR rotation angles
 * @return
 *    Yoking group that was updated by this tab or YokingGroupEnum::YOKING_GROUP_OFF
 *    if no action was taken.
 */
YokingGroupEnum::Enum
BrowserTabContent::applyHistologyOrientationYoking()
{
    if (isHistologyDisplayed()) {
        if (isHistologyOrientationAppliedToYoking()) {
            if (getBrainModelYokingGroup() != YokingGroupEnum::YOKING_GROUP_OFF) {
                HistologyOverlaySet* overlaySet(getHistologyOverlaySet());
                const int32_t numberOfOverlays(overlaySet->getNumberOfDisplayedOverlays());
                for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
                    HistologyOverlay* overlay = overlaySet->getOverlay(iOverlay);
                    CaretAssert(overlay);
                    
                    if (overlay->isEnabled()) {
                        const HistologyOverlay::SelectionData selectionData(overlay->getSelectionData());
                        const HistologySlicesFile* selectedFile(selectionData.m_selectedFile);
                        if (selectedFile != NULL) {
                            const HistologyCoordinate histologyCoordinate(getHistologySelectedCoordinate(selectionData.m_selectedFile));
                            if (histologyCoordinate.isValid()) {
                                int32_t selectedSliceIndex(histologyCoordinate.getSliceIndex());
                                const HistologySlice* histologySlice(selectedFile->getHistologySliceByIndex(selectedSliceIndex));
                                if (histologySlice != NULL) {
                                    Vector3D rotationAngles;
                                    if (histologySlice->getSliceRotationAngles(rotationAngles)) {
                                        /*
                                         * Need to invert rotation angles (may have to do with quaternions)
                                         */
                                        rotationAngles = -rotationAngles;
                                        
                                        setMprThreeRotationAnglesForYokingGroup(getBrainModelYokingGroup(),
                                                                                rotationAngles);
                                        return getBrainModelYokingGroup();
                                    }
                                }
                            }
                        }
                    }
                }
    
            }
        }
    }
    
    return YokingGroupEnum::YOKING_GROUP_OFF;
}

/**
 * @return Is histology axis crosshairs displayed
 */
bool
BrowserTabContent::isHistologyAxesCrosshairsDisplayed() const
{
    return m_displayHistologyAxesCrosshairs;
}

/**
 * Set histology axis crosshairs displayed
 *
 * @param displayed
 *     New status
 */
void
BrowserTabContent::setHistologyAxesCrosshairsDisplayed(const bool displayed)
{
    m_displayHistologyAxesCrosshairs = displayed;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Is flip about X axis enabled for histology
 */
bool
BrowserTabContent::isHistologyFlipXEnabled() const
{
    return m_histologyFlipXAxisFlag;
}

/**
 * Set  flip about X axis enabled for histology
 *
 * @param status
 *     New status
 */
void
BrowserTabContent::setHistologyFlipXEnabled(const bool status)
{
    m_histologyFlipXAxisFlag = status;
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Is lighting enabled ?
 */
bool
BrowserTabContent::isLightingEnabled() const
{
    return m_lightingEnabled;
}

/**
 * Set lighting enabled.
 *
 * @param lightingEnabled
 *     New status for lighting.
 */
void
BrowserTabContent::setLightingEnabled(const bool lightingEnabled)
{
    m_lightingEnabled = lightingEnabled;
}


/**
 * Return the axial slice index.
 * @return
 *   Axial slice index or negative if invalid
 */
int64_t
BrowserTabContent::getVolumeSliceIndexAxial(const VolumeMappableInterface* volumeFile) const
{
    return m_volumeSliceSettings->getSliceIndexAxial(volumeFile);
}

/**
 * Set the axial slice index.
 * @param
 *    New value for axial slice index.
 */
void
BrowserTabContent::setVolumeSliceIndexAxial(const VolumeMappableInterface* volumeFile,
                                        const int64_t sliceIndexAxial)
{
    m_volumeSliceSettings->setSliceIndexAxial(volumeFile, sliceIndexAxial);
    updateBrainModelYokedBrowserTabs();
}

/**
 * Return the coronal slice index.
 * @return
 *   Coronal slice index.
 */
int64_t
BrowserTabContent::getVolumeSliceIndexCoronal(const VolumeMappableInterface* volumeFile) const
{
    return m_volumeSliceSettings->getSliceIndexCoronal(volumeFile);
}


/**
 * Set the coronal slice index.
 * @param sliceIndexCoronal
 *    New value for coronal slice index.
 */
void
BrowserTabContent::setVolumeSliceIndexCoronal(const VolumeMappableInterface* volumeFile,
                                          const int64_t sliceIndexCoronal)
{
    m_volumeSliceSettings->setSliceIndexCoronal(volumeFile, sliceIndexCoronal);
    updateBrainModelYokedBrowserTabs();
}

/**
 * Return the parasagittal slice index.
 * @return
 *   Parasagittal slice index.
 */
int64_t
BrowserTabContent::getVolumeSliceIndexParasagittal(const VolumeMappableInterface* volumeFile) const
{
    return m_volumeSliceSettings->getSliceIndexParasagittal(volumeFile);
}

/**
 * Set the parasagittal slice index.
 * @param sliceIndexParasagittal
 *    New value for parasagittal slice index.
 */
void
BrowserTabContent::setVolumeSliceIndexParasagittal(const VolumeMappableInterface* volumeFile,
                                               const int64_t sliceIndexParasagittal)
{
    m_volumeSliceSettings->setSliceIndexParasagittal(volumeFile,
                                                         sliceIndexParasagittal);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Coordinate of axial slice.
 */
float
BrowserTabContent::getVolumeSliceCoordinateAxial() const
{
    return m_volumeSliceSettings->getSliceCoordinateAxial();
}

/**
 * Set the coordinate for the axial slice.
 * @param z
 *    Z-coordinate for axial slice.
 */
void
BrowserTabContent::setVolumeSliceCoordinateAxial(const float z)
{
    m_volumeSliceSettings->setSliceCoordinateAxial(z);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Coordinate of coronal slice.
 */
float
BrowserTabContent::getVolumeSliceCoordinateCoronal() const
{
    return m_volumeSliceSettings->getSliceCoordinateCoronal();
}

/**
 * Set the coordinate for the coronal slice.
 * @param y
 *    Y-coordinate for coronal slice.
 */
void
BrowserTabContent::setVolumeSliceCoordinateCoronal(const float y)
{
    m_volumeSliceSettings->setSliceCoordinateCoronal(y);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Coordinate of parasagittal slice.
 */
float
BrowserTabContent::getVolumeSliceCoordinateParasagittal() const
{
    return m_volumeSliceSettings->getSliceCoordinateParasagittal();
}

/**
 * Set the coordinate for the parasagittal slice.
 * @param x
 *    X-coordinate for parasagittal slice.
 */
void
BrowserTabContent::setVolumeSliceCoordinateParasagittal(const float x)
{
    m_volumeSliceSettings->setSliceCoordinateParasagittal(x);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return The slice coordinates
 */
Vector3D
BrowserTabContent::getVolumeSliceCoordinates() const
{
    return Vector3D(getVolumeSliceCoordinateParasagittal(),
                    getVolumeSliceCoordinateCoronal(),
                    getVolumeSliceCoordinateAxial());
}

/**
 * Is the parasagittal slice enabled?
 * @return
 *    Enabled status of parasagittal slice.
 */
bool
BrowserTabContent::isVolumeSliceParasagittalEnabled() const
{
    return m_volumeSliceSettings->isSliceParasagittalEnabled();
}

/**
 * Set the enabled status of the parasagittal slice.
 * @param sliceEnabledParasagittal
 *    New enabled status.
 */
void
BrowserTabContent::setVolumeSliceParasagittalEnabled(const bool sliceEnabledParasagittal)
{
    m_volumeSliceSettings->setSliceParasagittalEnabled(sliceEnabledParasagittal);
    updateBrainModelYokedBrowserTabs();
}

/**
 * Is the coronal slice enabled?
 * @return
 *    Enabled status of coronal slice.
 */
bool
BrowserTabContent::isVolumeSliceCoronalEnabled() const
{
    return m_volumeSliceSettings->isSliceCoronalEnabled();
}

/**
 * Set the enabled status of the coronal slice.
 * @param sliceEnabledCoronal
 *    New enabled status.
 */
void
BrowserTabContent::setVolumeSliceCoronalEnabled(const bool sliceEnabledCoronal)
{
    m_volumeSliceSettings->setSliceCoronalEnabled(sliceEnabledCoronal);
    updateBrainModelYokedBrowserTabs();
}

/**
 * Is the axial slice enabled?
 * @return
 *    Enabled status of axial slice.
 */
bool
BrowserTabContent::isVolumeSliceAxialEnabled() const
{
    return m_volumeSliceSettings->isSliceAxialEnabled();
}

/**
 * Set the enabled status of the axial slice.
 * @param sliceEnabledAxial
 *    New enabled status.
 */
void
BrowserTabContent::setVolumeSliceAxialEnabled(const bool sliceEnabledAxial)
{
    m_volumeSliceSettings->setSliceAxialEnabled(sliceEnabledAxial);
    updateBrainModelYokedBrowserTabs();
}



/**
 * @return Enabled status for left cerebral cortex.
 */
bool
BrowserTabContent::isWholeBrainLeftEnabled() const
{
    return m_wholeBrainSurfaceSettings->isLeftEnabled();
}

/**
 * Set the enabled status for the left hemisphere.
 * @param windowTabNumber
 *    Index of window tab.
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainLeftEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setLeftEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for right cerebral cortex.
 */
bool
BrowserTabContent::isWholeBrainRightEnabled() const
{
    return m_wholeBrainSurfaceSettings->isRightEnabled();
}

/**
 * Set the enabled status for the right hemisphere.
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainRightEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setRightEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for cerebellum.
 */
bool
BrowserTabContent::isWholeBrainCerebellumEnabled() const
{
    return m_wholeBrainSurfaceSettings->isCerebellumEnabled();
}

/**
 * Set the enabled status for the cerebellum.
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainCerebellumEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setCerebellumEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for hippocampus.
 */
bool
BrowserTabContent::isWholeBrainHippocampusEnabled() const
{
    return m_wholeBrainSurfaceSettings->isHippocampusEnabled();
}

/**
 * Set the enabled status for the hippocampus.
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainHippocampusEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setHippocampusEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for hippocampus left
 */
bool
BrowserTabContent::isWholeBrainHippocampusLeftEnabled() const
{
    return m_wholeBrainSurfaceSettings->isHippocampusLeftEnabled();
}

/**
 * Set the enabled status for the hippocampus left
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainHippocampusLeftEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setHippocampusLeftEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for hippocampus right
 */
bool
BrowserTabContent::isWholeBrainHippocampusRightEnabled() const
{
    return m_wholeBrainSurfaceSettings->isHippocampusRightEnabled();
}

/**
 * Set the enabled status for the hippocampus right
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainHippocampusRightEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setHippocampusRightEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for dentate hippocampus left
 */
bool
BrowserTabContent::isWholeBrainDentateHippocampusLeftEnabled() const
{
    return m_wholeBrainSurfaceSettings->isDentateHippocampusLeftEnabled();
}

/**
 * Set the enabled status for the dentate hippocampus left
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainDentateHippocampusLeftEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setDentateHippocampusLeftEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Enabled status for dentate hippocampus right
 */
bool
BrowserTabContent::isWholeBrainDentateHippocampusRightEnabled() const
{
    return m_wholeBrainSurfaceSettings->isDentateHippocampusRightEnabled();
}

/**
 * Set the enabled status for the dentate hippocampus right
 * @param enabled
 *    New enabled status.
 */
void
BrowserTabContent::setWholeBrainDentateHippocampusRightEnabled(const bool enabled)
{
    m_wholeBrainSurfaceSettings->setDentateHippocampusRightEnabled(enabled);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return The separation between the left and right surfaces.
 */
float
BrowserTabContent::getWholeBrainLeftRightSeparation() const
{
    return m_wholeBrainSurfaceSettings->getLeftRightSeparation();
}

/**
 * Set the separation between the cerebellum and the left/right surfaces.
 * @param separation
 *     New value for separation.
 */
void
BrowserTabContent::setWholeBrainLeftRightSeparation(const float separation)
{
    m_wholeBrainSurfaceSettings->setLeftRightSeparation(separation);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return The separation between the left/right surfaces.
 */
float
BrowserTabContent::getWholeBrainCerebellumSeparation() const
{
    return m_wholeBrainSurfaceSettings->getCerebellumSeparation();
}

/**
 * Set the separation between the cerebellum and the left and right surfaces.
 * @param separation
 *     New value for separation.
 */
void
BrowserTabContent::setWholeBrainCerebellumSeparation(const float separation)
{
    m_wholeBrainSurfaceSettings->setCerebellumSeparation(separation);
    updateBrainModelYokedBrowserTabs();
}

/**
 * @return Selected yoking group for charts
 */
YokingGroupEnum::Enum
BrowserTabContent::getChartModelYokingGroup() const
{
    return m_chartModelYokingGroup;
}

/**
 * Set the selected yoking group for charts.
 *
 * @param chartModelYokingType
 *    New value for yoking group.
 */
void
BrowserTabContent::setChartModelYokingGroup(const YokingGroupEnum::Enum chartModelYokingType)
{
    m_chartModelYokingGroup = chartModelYokingType;
    
    if (m_chartModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    int32_t copyFromTabIndex = -1;
    
    /*
     * Find another browser tab using the same yoking as 'me' and copy
     * yoked data from the other browser tab.
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            if (btc->getChartModelYokingGroup() == m_chartModelYokingGroup) {
                copyFromTabIndex = btc->getTabNumber();
                /*
                 * If anything is added, also need to update updateYokedBrowserTabs()
                 */
                *m_chartTwoMatrixViewingTranformation = *btc->m_chartTwoMatrixViewingTranformation;
                *m_chartTwoMatrixDisplayProperties = *btc->m_chartTwoMatrixDisplayProperties;
                break;
            }
        }
    }
    
    if (copyFromTabIndex >= 0) {
        /*
         * Maybe NULL when restoring scenes
         */
        if (m_chartTwoModel != NULL) {
            m_chartTwoModel->copyChartTwoCartesianAxes(copyFromTabIndex,
                                                       m_tabNumber);
        }
    }
}

/**
 * @return Selected yoking group for media
 */
YokingGroupEnum::Enum
BrowserTabContent::getMediaModelYokingGroup() const
{
    return m_mediaModelYokingGroup;
}

/**
 * Set the selected yoking group for media.
 *
 * @param mediaModelYokingType
 *    New value for yoking group.
 */
void
BrowserTabContent::setMediaModelYokingGroup(const YokingGroupEnum::Enum mediaModelYokingType)
{
    m_mediaModelYokingGroup = mediaModelYokingType;
    
    if (m_mediaModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    /*
     * Find another browser tab using the same yoking as 'me' and copy
     * yoked data from the other browser tab.
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            if (btc->getMediaModelYokingGroup() == m_mediaModelYokingGroup) {
                *m_mediaViewingTransformation = *btc->m_mediaViewingTransformation;
                break;
            }
        }
    }
}

/**
 * @return Selected yoking group for brain models (surface or volumes)
 */
YokingGroupEnum::Enum
BrowserTabContent::getBrainModelYokingGroup() const
{
    return m_brainModelYokingGroup;
}

/**
 * Set the selected yoking group for brain models (surface or volumes)
 *
 * @param brainModelYokingType
 *    New value for yoking group.
 */
void
BrowserTabContent::setBrainModelYokingGroup(const YokingGroupEnum::Enum brainModelYokingType)
{
    m_brainModelYokingGroup = brainModelYokingType;
    
    if (m_brainModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    /*
     * Find another browser tab using the same yoking as 'me' and copy
     * yoked data from the other browser tab.
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            if (btc->getBrainModelYokingGroup() == m_brainModelYokingGroup) {
                /*
                 * If anything is added, also need to update updateYokedBrowserTabs()
                 */
                *m_viewingTransformation = *btc->m_viewingTransformation;
                *m_flatSurfaceViewingTransformation = *btc->m_flatSurfaceViewingTransformation;
                *m_cerebellumViewingTransformation = *btc->m_cerebellumViewingTransformation;
                *m_volumeSliceViewingTransformation = *btc->m_volumeSliceViewingTransformation;
                *m_histologyViewingTransformation = *btc->m_histologyViewingTransformation;
                *m_mediaViewingTransformation = *btc->m_mediaViewingTransformation;
                m_volumeSliceSettings->copyToMeForYoking(*btc->m_volumeSliceSettings);
                *m_wholeBrainSurfaceSettings = *btc->m_wholeBrainSurfaceSettings;

                *m_obliqueVolumeRotationMatrix = *btc->m_obliqueVolumeRotationMatrix;
                *m_clippingPlaneGroup = *btc->m_clippingPlaneGroup;
                m_identificationUpdatesVolumeSlices = btc->m_identificationUpdatesVolumeSlices;
                m_identificationUpdatesHistologySlices = btc->m_identificationUpdatesHistologySlices;
                m_displayHistologyAxesCrosshairs = btc->m_displayHistologyAxesCrosshairs;
                m_histologyFlipXAxisFlag = btc->m_histologyFlipXAxisFlag;
                
                m_displayVolumeAxesCrosshairs = btc->m_displayVolumeAxesCrosshairs;
                m_displayVolumeAxesCrosshairLabels = btc->m_displayVolumeAxesCrosshairLabels;
                m_displayVolumeMontageAxesCoordinates = btc->m_displayVolumeMontageAxesCoordinates;
                m_volumeMontageCoordinateDisplayType = btc->m_volumeMontageCoordinateDisplayType;
                m_volumeMontageCoordinatePrecision = btc->m_volumeMontageCoordinatePrecision;
                m_volumeMontageCoordinateFontHeight = btc->m_volumeMontageCoordinateFontHeight;
                m_volumeMontageCoordinateTextAlignment = btc->m_volumeMontageCoordinateTextAlignment;
                m_mprRotationX = btc->m_mprRotationX;
                m_mprRotationY = btc->m_mprRotationY;
                m_mprRotationZ = btc->m_mprRotationZ;

                m_mprThreeRotationSeparateQuaternion     = btc->m_mprThreeRotationSeparateQuaternion;
                m_mprThreeAxialSeparateRotationQuaternion        = btc->m_mprThreeAxialSeparateRotationQuaternion;
                m_mprThreeCoronalSeparateRotationQuaternion      = btc->m_mprThreeCoronalSeparateRotationQuaternion;
                m_mprThreeParasagittalSeparateRotationQuaternion = btc->m_mprThreeParasagittalSeparateRotationQuaternion;

                m_mprThreeAxialInverseRotationQuaternion = btc->m_mprThreeAxialInverseRotationQuaternion;
                m_mprThreeCoronalInverseRotationQuaternion = btc->m_mprThreeCoronalInverseRotationQuaternion;
                m_mprThreeParasagittalInverseRotationQuaternion = btc->m_mprThreeParasagittalInverseRotationQuaternion;

                /**
                 * lighting enabled NOT yoked 
                 * m_lightingEnabled = btc->m_lightingEnabled;
                 */

                
                if (m_histologyModel != NULL) {
                    HistologySlicesFile* histologySlicesFile(NULL);
                    HistologyOverlaySet* overlaySet(btc->getHistologyOverlaySet());
                    if (overlaySet != NULL) {
                        const HistologyOverlay* underlay(overlaySet->getUnderlay());
                        if (underlay != NULL) {
                            histologySlicesFile = underlay->getSelectionData().m_selectedFile;
                        }
                    }
                    m_histologySliceSettings->copyYokedSettings(histologySlicesFile,
                                                                *btc->m_histologySliceSettings);
                    m_histologyViewingTransformation->copyFromOther(*btc->m_histologyViewingTransformation);
                }
                
                break;
            }
        }
    }
}

/**
 * @return Is this browser tab brain model yoked?
 */
bool
BrowserTabContent::isBrainModelYoked() const
{
    const bool yoked = (m_brainModelYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF);
    return yoked;
}

/**
 * @return Is this browser tab chart model yoked?
 */
bool
BrowserTabContent::isChartModelYoked() const
{
    const bool yoked = (m_chartModelYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF);
    return yoked;
}

/**
 * @return Is this browser tab media model yoked?
 */
bool
BrowserTabContent::isMediaModelYoked() const
{
    const bool yoked = (m_mediaModelYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF);
    return yoked;
}


/**
 * Update other browser tabs with brain or chart yoked data dependent upon active model
 */
void
BrowserTabContent::updateYokedModelBrowserTabs()
{
    bool chartFlag = false;
    bool mediaFlag = false;
    
    switch (getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            mediaFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            break;
    }
 
    if (chartFlag) {
        updateChartModelYokedBrowserTabs();
    }
    else if (mediaFlag) {
        updateMediaModelYokedBrowserTabs();
    }
    else {
        updateBrainModelYokedBrowserTabs();
    }
}

/**
 * Update other browser tabs with brain model yoked data.
 */
void
BrowserTabContent::updateBrainModelYokedBrowserTabs()
{
    if (isExecutingConstructor) {
        return;
    }
    
    if (m_brainModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    /*
     * Copy yoked data from 'me' to all other yoked browser tabs
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            /*
             * If anything is added, also need to update setYokingGroup()
             */
            if (btc->getBrainModelYokingGroup() == m_brainModelYokingGroup) {
                *btc->m_viewingTransformation = *m_viewingTransformation;
                *btc->m_flatSurfaceViewingTransformation = *m_flatSurfaceViewingTransformation;
                *btc->m_cerebellumViewingTransformation = *m_cerebellumViewingTransformation;
                *btc->m_volumeSliceViewingTransformation = *m_volumeSliceViewingTransformation;
                btc->m_volumeSliceSettings->copyToMeForYoking(*m_volumeSliceSettings);
                *btc->m_wholeBrainSurfaceSettings = *m_wholeBrainSurfaceSettings;
                *btc->m_obliqueVolumeRotationMatrix = *m_obliqueVolumeRotationMatrix;
                *btc->m_clippingPlaneGroup = *m_clippingPlaneGroup;
                btc->m_identificationUpdatesVolumeSlices = m_identificationUpdatesVolumeSlices;
                btc->m_identificationUpdatesHistologySlices = m_identificationUpdatesHistologySlices;
                btc->m_displayHistologyAxesCrosshairs = m_displayHistologyAxesCrosshairs;
                btc->m_histologyFlipXAxisFlag = m_histologyFlipXAxisFlag;
                btc->m_displayVolumeAxesCrosshairs = m_displayVolumeAxesCrosshairs;
                btc->m_displayVolumeAxesCrosshairLabels = m_displayVolumeAxesCrosshairLabels;
                btc->m_displayVolumeMontageAxesCoordinates = m_displayVolumeMontageAxesCoordinates;
                btc->m_volumeMontageCoordinateDisplayType = m_volumeMontageCoordinateDisplayType;
                btc->m_volumeMontageCoordinatePrecision = m_volumeMontageCoordinatePrecision;
                btc->m_volumeMontageCoordinateFontHeight = m_volumeMontageCoordinateFontHeight;
                btc->m_volumeMontageCoordinateTextAlignment = m_volumeMontageCoordinateTextAlignment;
                btc->m_mprRotationX = m_mprRotationX;
                btc->m_mprRotationY = m_mprRotationY;
                btc->m_mprRotationZ = m_mprRotationZ;

                btc->m_mprThreeRotationSeparateQuaternion     = m_mprThreeRotationSeparateQuaternion;
                btc->m_mprThreeAxialSeparateRotationQuaternion        = m_mprThreeAxialSeparateRotationQuaternion;
                btc->m_mprThreeCoronalSeparateRotationQuaternion      = m_mprThreeCoronalSeparateRotationQuaternion;
                btc->m_mprThreeParasagittalSeparateRotationQuaternion = m_mprThreeParasagittalSeparateRotationQuaternion;

                btc->m_mprThreeAxialInverseRotationQuaternion = m_mprThreeAxialInverseRotationQuaternion;
                btc->m_mprThreeCoronalInverseRotationQuaternion = m_mprThreeCoronalInverseRotationQuaternion;
                btc->m_mprThreeParasagittalInverseRotationQuaternion = m_mprThreeParasagittalInverseRotationQuaternion;

                /*
                 * DO NOT YOKE MEDIA TRANSFORMATION (but might have its own yoking in the future 
                 * *btc->m_mediaViewingTransformation = *m_mediaViewingTransformation;
                 */

                /**
                 * lighting enabled NOT yoked
                 * btc->m_lightingEnabled = m_lightingEnabled;
                 */
            }
        }
    }
    
    updateHistologyModelYokedBrowserTabs();
}

/**
 * Move yoked volume slices to the histology coordinate's stereotaxic position
 * @param histologyCoordinate
 *    The histology coordinate
 */
void
BrowserTabContent::moveYokedVolumeSlicesToHistologyCoordinate(const HistologyCoordinate& histologyCoordinate)
{
    if (isExecutingConstructor) {
        return;
    }
    
    if (m_brainModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    if (m_histologyModel == NULL) {
        return;
    }
    
    if (histologyCoordinate.isStereotaxicXYZValid()) {
        const Vector3D xyz(histologyCoordinate.getStereotaxicXYZ());
        setVolumeSliceCoordinateParasagittal(xyz[0]);
        setVolumeSliceCoordinateCoronal(xyz[1]);
        setVolumeSliceCoordinateAxial(xyz[2]);

        /*
         * Copy yoked data from 'me' to all other yoked browser tabs
         */
        std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
        for (auto btc : activeTabs) {
            if (btc != this) {
                if (btc->getBrainModelYokingGroup() == m_brainModelYokingGroup) {
                    btc->m_volumeSliceSettings->copyToMeForYoking(*m_volumeSliceSettings);
                }
            }
        }
    }
}

/**
 * Set volume slices in yoked tabs to same view as histology slice view
 * @param yokingGroup
 *    The yoking group
 * @param histologySlice
 *    The histology slice
 * @param allViewportContent
 *    Content of all viewports in all windows
 * @param histologySelectionBounds
 *    The selection bounds in the histology slice
 */
void
BrowserTabContent::setVolumeSliceViewsToHistologyRegion(const YokingGroupEnum::Enum yokingGroup,
                                                        const HistologySlice* histologySlice,
                                                        const std::vector<const BrainOpenGLViewportContent*>& allViewportContent,
                                                        const GraphicsRegionSelectionBox* histologySelectionBounds)
{
    CaretAssert(histologySlice);
    CaretAssert(histologySelectionBounds);
    if (yokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    const std::unique_ptr<GraphicsRegionSelectionBox> stereotaxicBounds(histologySlice->planeRegionToStereotaxicRegion(histologySelectionBounds));
    if ( ! stereotaxicBounds) {
        return;
    }

    const Plane histologyPlane(histologySlice->getStereotaxicPlane());
    if ( ! histologyPlane.isValidPlane()) {
        return;
    }
    
    const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion(VolumeSliceViewPlaneEnum::fromPlane(histologyPlane));
    if (sliceViewPlaneForFitToRegion == VolumeSliceViewPlaneEnum::ALL) {
        return;
    }
    
    
    for (const BrainOpenGLViewportContent* vpContent : allViewportContent) {
        CaretAssert(vpContent);
        const BrowserTabContent* btc(vpContent->getBrowserTabContent());
        CaretAssert(btc);
        if (btc->m_closedFlag) {
            continue;
        }
        if (yokingGroup != btc->getBrainModelYokingGroup()) {
            continue;
        }
        if (btc->getSelectedModelType() == ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES) {
            const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab(btc->getVolumeSliceViewPlane());
            switch (sliceViewPlaneSelectedInTab) {
                case VolumeSliceViewPlaneEnum::ALL:
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                case VolumeSliceViewPlaneEnum::CORONAL:
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    if (sliceViewPlaneForFitToRegion != sliceViewPlaneSelectedInTab) {
                        continue;
                    }
                    break;
            }
                     
            /*
             * Need to uses steretaxic bounds and convert to viewport coordinates
             */
            const GraphicsObjectToWindowTransform* xform(vpContent->getVolumeGraphicsObjectToWindowTransform(sliceViewPlaneForFitToRegion));
            CaretAssert(xform);
            if (xform == NULL) {
                continue;
            }
            
            /*
             * Get viewport coordinates in volume slices view
             */
            const BoundingBox bb(stereotaxicBounds->getBounds());
            Vector3D bottomLeftXYZ, topRightXYZ;
            switch (sliceViewPlaneForFitToRegion) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                {
                    const float sliceZ(bb.getCenterZ());
                    bottomLeftXYZ.set(bb.getMinX(), bb.getMinY(), sliceZ);
                    topRightXYZ.set(bb.getMaxX(), bb.getMaxY(), sliceZ);
                }
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                {
                    const float sliceY(bb.getCenterY());
                    bottomLeftXYZ.set(bb.getMinX(), sliceY, bb.getMinZ());
                    topRightXYZ.set(bb.getMaxX(), sliceY, bb.getMaxZ());
                }
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                {
                    /* Parasagittal has positive Y on left (left view) */
                    const float sliceX(bb.getCenterX());
                    bottomLeftXYZ.set(sliceX, bb.getMaxY(), bb.getMinZ());
                    topRightXYZ.set(sliceX, bb.getMinY(), bb.getMaxZ());
                }
            }
            
            /*
             * Compute region's viewport width/height
             */
            Vector3D windowBottomLeftXYZ, windowTopRightXYZ;
            xform->transformPoint(bottomLeftXYZ, windowBottomLeftXYZ);
            xform->transformPoint(topRightXYZ, windowTopRightXYZ);

            /*
             * Box with stereotaxic bounds and viewport coordinates
             * in volume's viewport
             */
            GraphicsRegionSelectionBox sliceBox;
            sliceBox.initialize(bottomLeftXYZ[0], bottomLeftXYZ[1], bottomLeftXYZ[2],
                                windowBottomLeftXYZ[0], windowBottomLeftXYZ[1]);
            sliceBox.update(topRightXYZ[0], topRightXYZ[1], topRightXYZ[2],
                            windowTopRightXYZ[0], windowTopRightXYZ[1]);
            
            switch (btc->getVolumeSliceProjectionType()) {
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                    btc->m_volumeSliceViewingTransformation->setViewToBounds(vpContent,
                                                                             sliceViewPlaneSelectedInTab,
                                                                             sliceViewPlaneForFitToRegion,
                                                                             &sliceBox,
                                                                             btc);
                    break;
            }
        }
    }
}

/**
 * Update other browser tabs with brain model yoked data.
 */
void
BrowserTabContent::updateChartModelYokedBrowserTabs()
{
    if (isExecutingConstructor) {
        return;
    }
    
    if (m_chartModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    /*
     * Copy yoked data from 'me' to all other yoked browser tabs
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            /*
             * If anything is added, also need to update setYokingGroup()
             */
            if (btc->getChartModelYokingGroup() == m_chartModelYokingGroup) {
                *btc->m_chartTwoMatrixViewingTranformation = *m_chartTwoMatrixViewingTranformation;
                *btc->m_chartTwoMatrixDisplayProperties = *m_chartTwoMatrixDisplayProperties;
            }
        }
    }
}

/**
 * Update other browser tabs with histology model yoked data.
 */
void
BrowserTabContent::updateHistologyModelYokedBrowserTabs()
{
    if (isExecutingConstructor) {
        return;
    }
    
    if (m_brainModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }

    if (m_histologyModel == NULL) {
        return;
    }
    
    /*
     * Copy yoked data from 'me' to all other yoked browser tabs
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            /*
             * If anything is added, also need to update setYokingGroup()
             */
            if (btc->getBrainModelYokingGroup() == m_brainModelYokingGroup) {
                HistologySlicesFile* histologySlicesFile(NULL);
                HistologyOverlaySet* overlaySet(btc->getHistologyOverlaySet());
                if (overlaySet != NULL) {
                    const HistologyOverlay* underlay(overlaySet->getUnderlay());
                    if (underlay != NULL) {
                        histologySlicesFile = underlay->getSelectionData().m_selectedFile;
                    }
                }
                btc->m_histologySliceSettings->copyYokedSettings(histologySlicesFile,
                                                                 *m_histologySliceSettings);
                btc->m_histologyViewingTransformation->copyFromOther(*m_histologyViewingTransformation);
            }
        }
    }
}

/**
 * Update other browser tabs with media model yoked data.
 */
void
BrowserTabContent::updateMediaModelYokedBrowserTabs()
{
    if (isExecutingConstructor) {
        return;
    }
    
    if (m_mediaModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    float myImageWidthHeight[2] { 0.0f, 0.0f };
    MediaOverlaySet* myOverlaySet(getMediaOverlaySet());
    if (myOverlaySet != NULL) {
        const MediaFile* myMediaFile(myOverlaySet->getBottomMostMediaFile());
        myImageWidthHeight[0] = myMediaFile->getWidth();
        myImageWidthHeight[1] = myMediaFile->getHeight();
    }
    
    /*
     * Copy yoked data from 'me' to all other yoked browser tabs
     */
    std::vector<BrowserTabContent*> activeTabs = BrowserTabContent::getOpenBrowserTabs();
    for (auto btc : activeTabs) {
        if (btc != this) {
            /*
             * If anything is added, also need to update setYokingGroup()
             */
            if (btc->getMediaModelYokingGroup() == m_mediaModelYokingGroup) {
                float btcImageWidthHeight[2] { 0.0f, 0.0f };
                MediaOverlaySet* btcOverlaySet(btc->getMediaOverlaySet());
                if (btcOverlaySet != NULL) {
                    const MediaFile* btcMediaFile(btcOverlaySet->getBottomMostMediaFile());
                    btcImageWidthHeight[0] = btcMediaFile->getWidth();
                    btcImageWidthHeight[1] = btcMediaFile->getHeight();
                }

                btc->m_mediaViewingTransformation->copyTransformsForYoking(*m_mediaViewingTransformation,
                                                                           myImageWidthHeight,
                                                                           btcImageWidthHeight);
            }
        }
    }
}

/**
 * @return Pointer to the manual layout browser tab annotation
 */
AnnotationBrowserTab*
BrowserTabContent::getManualLayoutBrowserTabAnnotation()
{
    return m_manualLayoutBrowserTabAnnotation.get();
}

/**
 * @return Pointer to the manual layout browser tab annotation (const method)
 */
const
AnnotationBrowserTab*
BrowserTabContent::getManualLayoutBrowserTabAnnotation() const
{
    return m_manualLayoutBrowserTabAnnotation.get();
}

/**
 * @return True if the manual tab geometry bounds are still
 * set to the default bounds.
 */
bool
BrowserTabContent::isDefaultManualTabGeometryBounds() const
{
    float xMin(0.0), xMax(0.0), yMin(0.0), yMax(0.0);
    m_manualLayoutBrowserTabAnnotation->getBounds2D(xMin, xMax, yMin, yMax);

    
    if (xMin != m_defaultManualTabGeometryBounds[0]) {
        return false;
    }
    if (xMax != m_defaultManualTabGeometryBounds[1]) {
        return false;
    }
    if (yMin != m_defaultManualTabGeometryBounds[2]) {
        return false;
    }
    if (yMax != m_defaultManualTabGeometryBounds[3]) {
        return false;
    }
    
    return true;
}

/**
 * @return Pointer to the scale bar
 */
AnnotationScaleBar*
BrowserTabContent::getScaleBar()
{
    return m_scaleBar.get();
}

/**
 * @return Pointer to the scale bar
 */
const AnnotationScaleBar*
BrowserTabContent::getScaleBar() const
{
    return m_scaleBar.get();
}

/**
 * Initialize a scale bar
 */
void
BrowserTabContent::initializeScaleBar()
{
    m_scaleBar->setTabIndex(m_tabNumber);
    m_scaleBar->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
    m_scaleBar->getCoordinate()->setXYZ(10.0, 10.0, 0.0);
}

/**
 * This method should be called by SessionManager and NOTHING ELSE.
 * Set the closed status (tab has been closed by user but is available for reopening.
 * @param closedStatus
 * New closed status
 */
void
BrowserTabContent::setClosedStatusFromSessionManager(const bool closedStatus)
{
    m_closedFlag = closedStatus;
    
    /*
     * If reopening, may need to update from yoking
     */
    if ( ! m_closedFlag) {
        /*
         * May need to update yoking
         */
        setChartModelYokingGroup(getChartModelYokingGroup());
        setBrainModelYokingGroup(getBrainModelYokingGroup());
    }
}

/**
 * Set the position in the toolbar of a tab from when the tab was closed
 * @param tabBarPosition
 *     Position (index) of the tab in the tab bar
 */
void
BrowserTabContent::setClosedTabWindowTabBarPositionIndex(const int32_t tabBarPosition)
{
    m_closedTabBarPosition = tabBarPosition;
}

/**
 * @return Position in the toolbar of a tab from when the tab was closed
 */
int32_t
BrowserTabContent::getClosedTabWindowTabBarPositionIndex() const
{
    return m_closedTabBarPosition;
}

/**
 * Set the position in the toolbar of a tab from when the tab was closed
 * @param tabBarPosition
 *     Position (index) of the tab in the tab bar
 */
void
BrowserTabContent::setClosedTabWindowIndex(const int32_t windowIndex)
{
    m_closedWindowIndex = windowIndex;
}

/**
 * @return Position in the toolbar of a tab from when the tab was closed
 */
int32_t
BrowserTabContent::getClosedTabWindowIndex() const
{
    return m_closedWindowIndex;
}


/**
 *@return Browser tabs the are open and in use by the user.
 * Any tabs that are closed but available for reopening are excluded
 */
std::vector<BrowserTabContent*>
BrowserTabContent::getOpenBrowserTabs()
{
    std::vector<BrowserTabContent*> openTabs;
    
    for (auto tab : s_allBrowserTabContent) {
        if ( ! tab->m_closedFlag) {
            openTabs.push_back(tab);
        }
    }
    
    return openTabs;
}

/**
 * @return Media selection box
 */
GraphicsRegionSelectionBox*
BrowserTabContent::getRegionSelectionBox()
{
    return m_regionSelectionBox.get();
}

/**
 * @return Media selection box (const method)
 */
const GraphicsRegionSelectionBox*
BrowserTabContent::getRegionSelectionBox() const
{
    return m_regionSelectionBox.get();
}

/**
 * @return Modes supported for left drag with mouse for current model
 */
std::vector<MouseLeftDragModeEnum::Enum>
BrowserTabContent::getSupportedMouseLeftDragModes() const
{
    std::vector<MouseLeftDragModeEnum::Enum> leftDragModes;
    
    bool allowsRegionSelectionFlag(false);
    switch (getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            allowsRegionSelectionFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            allowsRegionSelectionFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            switch (getVolumeSliceProjectionType()) {
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                    allowsRegionSelectionFlag = true;
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                    allowsRegionSelectionFlag = true;
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                    /* No region selection for Oblique slice viewing */
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                    allowsRegionSelectionFlag = true;
                    break;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            break;
    }
    
    leftDragModes.push_back(MouseLeftDragModeEnum::DEFAULT);
    if (allowsRegionSelectionFlag) {
        leftDragModes.push_back(MouseLeftDragModeEnum::REGION_SELECTION);
    }
    
    return leftDragModes;
}

/**
 * @return The mode for a mouse left drag
 */
MouseLeftDragModeEnum::Enum
BrowserTabContent::getMouseLeftDragMode() const
{
    /*
     * Verify that left drag mode type is still valid.  Not all
     * models support all drag modes.
     */
    std::vector<MouseLeftDragModeEnum::Enum> allModes(getSupportedMouseLeftDragModes());
    if (std::find(allModes.begin(),
                  allModes.end(),
                  m_mouseLeftDragMode) == allModes.end()) {
        /*
         * Current mode is invalid (model may have changed)
         * Use first mode, typically the default mode.
         * Some models may prefer a different mode.
         */
        if ( ! allModes.empty()) {
            m_mouseLeftDragMode = allModes.front();
        }
        switch (getSelectedModelType()) {
            case ModelTypeEnum::MODEL_TYPE_CHART:
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                break;
            case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                m_mouseLeftDragMode = MouseLeftDragModeEnum::REGION_SELECTION;
                break;
            case ModelTypeEnum::MODEL_TYPE_INVALID:
                break;
            case ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                m_mouseLeftDragMode = MouseLeftDragModeEnum::REGION_SELECTION;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE:
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                break;
            case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                 break;
            case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                break;
        }
        
        CaretAssert(std::find(allModes.begin(),
                              allModes.end(),
                              m_mouseLeftDragMode) != allModes.end());
    }
    return m_mouseLeftDragMode;
}

/**
 * Set the mode for a left mouse drag
 * @param mouseLeftDragMode
 *    New mode
 */
void
BrowserTabContent::setMouseLeftDragMode(const MouseLeftDragModeEnum::Enum mouseLeftDragMode)
{
    m_mouseLeftDragMode = mouseLeftDragMode;
}

/**
 * Add an MPR three montage viewport
 * @param viewport
 *    Montage viewport from drawing a slice
 */
void
BrowserTabContent::addMprThreeMontageViewport(const GraphicsViewport& viewport)
{
    m_mprThreeMontageViewports.push_back(viewport);
}

/**
 * @return The viewport containing the mouse from MPR drawing
 * @param mouseXY
 *    XY of the mouse.
 */
GraphicsViewport
BrowserTabContent::getMprThreeMontageViewportContainingMouse(const Vector3D mouseXY) const
{
    for (auto& gvp : m_mprThreeMontageViewports) {
        if (gvp.containsWindowXY(mouseXY[0], mouseXY[1])) {
            return gvp;
        }
    }
    return GraphicsViewport();
}
