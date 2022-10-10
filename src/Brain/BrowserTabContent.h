#ifndef __BROWSER_TAB_CONTENT__H_
#define __BROWSER_TAB_CONTENT__H_

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

#include <memory>
#include <set>

#include <QQuaternion>

#include "BoundingBox.h"
#include "CaretObject.h"
#include "ChartTwoAxisOrientationTypeEnum.h"
#include "ChartTwoAxisScaleRangeModeEnum.h"
#include "ClippingPlanePanningModeEnum.h"
#include "EventListenerInterface.h"
#include "HistologyCoordinate.h"
#include "Matrix4x4.h"
#include "MediaDisplayCoordinateModeEnum.h"
#include "Model.h"
#include "ModelTypeEnum.h"
#include "Plane.h"
#include "ProjectionViewTypeEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "TabContentBase.h"
#include "VolumeMprOrientationModeEnum.h"
#include "VolumeMprViewModeEnum.h"
#include "VolumeSliceDrawingTypeEnum.h"
#include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewAllPlanesLayoutEnum.h"
#include "YokingGroupEnum.h"

namespace caret {

    class AnnotationBrowserTab;
    class AnnotationColorBar;
    class AnnotationScaleBar;
    class BrainOpenGLViewportContent;
    class CaretDataFile;
    class CaretMappableDataFile;
    class ChartTwoCartesianOrientedAxes;
    class ChartTwoMatrixDisplayProperties;
    class ChartTwoOverlaySet;
    class ClippingPlaneGroup;
    class EventCaretMappableDataFilesAndMapsInDisplayedOverlays;
    class GraphicsRegionSelectionBox;
    class HistologyOverlaySet;
    class HistologySliceSettings;
    class HistologySlicesFile;
    class Matrix4x4;
    class MediaOverlaySet;
    class ModelChart;
    class ModelChartTwo;
    class ModelHistology;
    class ModelMedia;
    class ModelSurface;
    class ModelSurfaceMontage;
    class ModelSurfaceSelector;
    class ModelTransform;
    class ModelVolume;
    class ModelWholeBrain;
    class OverlaySet;
    class Palette;
    class PlainTextStringBuilder;
    class SceneClassAssistant;
    class Surface;
    class ViewingTransformations;
    class ViewingTransformationsCerebellum;
    class ViewingTransformationsMedia;
    class ViewingTransformationsVolume;
    class VolumeMappableInterface;
    class VolumeSliceSettings;
    class VolumeSurfaceOutlineSetModel;
    class WholeBrainSurfaceSettings;
    
    class BrowserTabContent : public TabContentBase, public EventListenerInterface, public SceneableInterface {
        
    public:
        BrowserTabContent(const int32_t tabNumber);
        
        virtual ~BrowserTabContent();
        
        void cloneBrowserTabContent(BrowserTabContent* tabToClone);
        
        virtual void receiveEvent(Event* event);
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        virtual AString toString() const;
        
        AString getTabName() const override;
        
        AString getUserTabName() const;
        
        void setUserTabName(const AString& userName);
        
        OverlaySet* getOverlaySet();
        
        const OverlaySet* getOverlaySet() const;
        
        ChartTwoOverlaySet* getChartTwoOverlaySet();
        
        const ChartTwoOverlaySet* getChartTwoOverlaySet() const;
        
        std::vector<ChartTwoCartesianOrientedAxes*> getYokedAxes(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                                 const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode) const;
        
        HistologyOverlaySet* getHistologyOverlaySet();
        
        const HistologyOverlaySet* getHistologyOverlaySet() const;
        
        MediaOverlaySet* getMediaOverlaySet();
        
        const MediaOverlaySet* getMediaOverlaySet() const;
        
        std::set<AString> getDisplayedMediaFileNames() const;
        
        MediaDisplayCoordinateModeEnum::Enum getMediaDisplayCoordinateMode() const;
        
        void setMediaDisplayCoordinateMode(const MediaDisplayCoordinateModeEnum::Enum mediaDisplayCoordinateMode);
        
        MediaDisplayCoordinateModeEnum::Enum getHistologyDisplayCoordinateMode() const;
        
        void setHistologyDisplayCoordinateMode(const MediaDisplayCoordinateModeEnum::Enum histologyDisplayCoordinateMode);
        
        int32_t getTabNumber() const;
        
        ModelTypeEnum::Enum getSelectedModelType() const;
        
        void setSelectedModelType(ModelTypeEnum::Enum selectedModelType);
        
        const Model* getModelForDisplay() const;
        
        Model* getModelForDisplay();
        
        ModelChart* getDisplayedChartOneModel();
        
        const ModelChart* getDisplayedChartOneModel() const;
        
        ModelChartTwo* getDisplayedChartTwoModel();
        
        const ModelChartTwo* getDisplayedChartTwoModel() const;
        
        ModelMedia* getDisplayedMediaModel();
        
        const ModelMedia* getDisplayedMediaModel() const;
        
        ModelHistology* getDisplayedHistologyModel();
        
        const ModelHistology* getDisplayedHistologyModel() const;
        
        ModelSurface* getDisplayedSurfaceModel();
        
        const ModelSurface* getDisplayedSurfaceModel() const;
        
        ModelVolume* getDisplayedVolumeModel();
        
        const ModelVolume* getDisplayedVolumeModel() const;
        
        ModelWholeBrain* getDisplayedWholeBrainModel();
        
        ModelSurfaceMontage* getDisplayedSurfaceMontageModel();
        
        const ModelSurfaceMontage* getDisplayedSurfaceMontageModel() const;
        
        const std::vector<ModelSurface*> getAllSurfaceModels() const;
        
        ModelSurfaceSelector* getSurfaceModelSelector();
        
        std::vector<StructureEnum::Enum> getSurfaceStructuresDisplayed();
        
        bool isCerebellumDisplayed() const;
        
        bool isChartOneDisplayed() const;
        
        bool isChartTwoDisplayed() const;
        
        bool isFlatSurfaceDisplayed() const;
        
        bool isVolumeSlicesDisplayed() const;
        
        bool isVolumeMprDisplayed() const;
        
        bool isHistologyDisplayed() const;

        bool isMediaDisplayed() const;
        
        bool isWholeBrainDisplayed() const;
        
        void getFilesDisplayedInTab(std::vector<CaretDataFile*>& displayedDataFilesOut);
        
        void getFilesAndMapIndicesInOverlays(EventCaretMappableDataFilesAndMapsInDisplayedOverlays* fileAndMapsEvent);

        void update(const std::vector<Model*> models);
        
        bool isChartOneModelValid() const;
        
        bool isChartTwoModelValid() const;
        
        bool isSurfaceModelValid() const;
        
        bool isVolumeSliceModelValid() const;
        
        bool isWholeBrainModelValid() const;

        bool isSurfaceMontageModelValid() const;
        
        bool isHistologyModelValid() const;
        
        bool isMediaModelValid() const;
        
        void getAnnotationColorBars(std::vector<AnnotationColorBar*>& colorBarsOut);
        
        void getDisplayedPaletteMapFiles(std::vector<CaretMappableDataFile*>& mapFiles,
                                         std::vector<int32_t>& mapIndices);
        
        VolumeSurfaceOutlineSetModel* getVolumeSurfaceOutlineSet();
        
        const VolumeSurfaceOutlineSetModel* getVolumeSurfaceOutlineSet() const;
        
        bool isAspectRatioLocked() const;
        
        void setAspectRatioLocked(const bool locked);
        
        float getAspectRatio() const;
        
        void setAspectRatio(const float aspectRatio);
        

        void getClippingPlaneEnabled(bool& xEnabled,
                                     bool& yEnabled,
                                     bool& zEnabled,
                                     bool& surfaceEnabled,
                                     bool& volumeEnabled,
                                     bool& featuresEnabled) const;
        
        void setClippingPlaneEnabled(const bool xEnabled,
                                     const bool yEnabled,
                                     const bool zEnabled,
                                     const bool surfaceEnabled,
                                     const bool volumeEnabled,
                                     const bool featuresEnabled);
        
        void getClippingPlaneTransformation(float panning[3],
                                            float rotation[3],
                                            float thickness[3],
                                            bool& displayClippingBox) const;
        
        void setClippingPlaneTransformation(const float panning[3],
                                            const float rotation[3],
                                            const float thickness[3],
                                            const bool displayClippingBox);

        const ClippingPlaneGroup* getClippingPlaneGroup() const;

        void resetClippingPlaneTransformation();
        
        bool isClippingPlanesEnabled();
        
        void setClippingPlanesEnabled(const bool status);
        
        void setClippingPlanesEnabledAndEnablePlanes(const bool status);
        
        void setClippingPanningMode(const ClippingPlanePanningModeEnum::Enum panningMode);
        
        ClippingPlanePanningModeEnum::Enum getClippingPanningMode() const;
        

        const float* getTranslation() const;
        
        void getTranslation(float translationOut[3]) const;
        
        void setTranslation( const float translation[3]);
        
        void setTranslation(const float translationX,
                                    const float translationY,
                                    const float translationZ);

        float getScaling() const;
        
        void setScaling(const float scaling);
        
        Matrix4x4 getRotationMatrix() const;
        
        void setRotationMatrix(const Matrix4x4& rotationMatrix);
        
        Matrix4x4 getObliqueVolumeRotationMatrix() const;
        
        void setObliqueVolumeRotationMatrix(const Matrix4x4& obliqueRotationMatrix);

        Matrix4x4 getFlatRotationMatrix() const;
        
        void setFlatRotationMatrix(const Matrix4x4& flatRotationMatrix);
        
        void resetMprRotations();

        float getMprRotationX() const;
        
        float getMprRotationY() const;
        
        float getMprRotationZ() const;
        
        Matrix4x4 getMprRotationMatrix4x4ForSlicePlane(const ModelTypeEnum::Enum modelType,
                                                       const VolumeSliceViewPlaneEnum::Enum slicePlane) const;
        
        void getRightCortexFlatMapOffset(float& offsetX,
                                         float& offsetY) const;
        
        void setRightCortexFlatMapOffset(const float offsetX,
                                         const float offsetY);
        
        float getRightCortexFlatMapZoomFactor() const;
        
        void setRightCortexFlatMapZoomFactor(const float zoomFactor);
        
        ProjectionViewTypeEnum::Enum getProjectionViewType() const;
        
        void resetView();
        
        void rightView();
        
        void leftView();
        
        void anteriorView();
        
        void posteriorView();
        
        void dorsalView();
        
        void ventralView();
        
        void applyMouseVolumeSliceIncrement(BrainOpenGLViewportContent* viewportContent,
                                            const int32_t mousePressX,
                                            const int32_t mousePressY,
                                            const int32_t mouseDY);
        
        void applyMouseRotation(BrainOpenGLViewportContent* viewportContent,
                                const int32_t mousePressX,
                                const int32_t mousePressY,
                                const int32_t mouseX,
                                const int32_t mouseY,
                                const int32_t mouseDeltaX,
                                const int32_t mouseDeltaY);
        
        void applyHistologyMouseScaling(BrainOpenGLViewportContent* viewportContent,
                                        const int32_t mousePressX,
                                        const int32_t mousePressY,
                                        const int32_t mouseDY,
                                        const float dataX,
                                        const float dataY,
                                        const bool dataXYValidFlag);

        void applyMediaMouseScaling(BrainOpenGLViewportContent* viewportContent,
                                    const int32_t mousePressX,
                                    const int32_t mousePressY,
                                    const int32_t mouseDY,
                                    const float dataX,
                                    const float dataY,
                                    const bool dataXYValidFlag);
        
        void setHistologyScalingFromGui(BrainOpenGLViewportContent* viewportContent,
                                        const float scaling);
        
        void setMediaScalingFromGui(BrainOpenGLViewportContent* viewportContent,
                                    const float scaling);
        
        void setHistologyViewToBounds(const BrainOpenGLViewportContent* viewportContent,
                                      const BoundingBox* windowBounds,
                                      const GraphicsRegionSelectionBox* selectionBounds);

        void setMediaViewToBounds(const BrainOpenGLViewportContent* viewportContent,
                                  const BoundingBox* windowBounds,
                                  const GraphicsRegionSelectionBox* selectionBounds);
        
        void applyMouseScaling(BrainOpenGLViewportContent* viewportContent,
                               const int32_t mousePressX,
                               const int32_t mousePressY,
                               const int32_t mouseDX,
                               const int32_t mouseDY);
        
        void applyMouseTranslation(BrainOpenGLViewportContent* viewportContent,
                                   const int32_t mousePressX,
                                   const int32_t mousePressY,
                                   const int32_t mouseX,
                                   const int32_t mouseY,
                                   const int32_t mouseDX,
                                   const int32_t mouseDY);
        
        void getTransformationsForOpenGLDrawing(const ProjectionViewTypeEnum::Enum projectionViewType,
                                                float translationOut[3],
                                                double rotationMatrixOut[16],
                                                float& scalingOut) const;
        
        void applyChartTwoAxesBoundSelection(const int32_t viewport[4],
                                             const int32_t x1,
                                             const int32_t y1,
                                             const int32_t x2,
                                             const int32_t y2);
        
        void finalizeChartTwoAxesBoundSelection(const int32_t viewport[4],
                                                const int32_t x1,
                                                const int32_t y1,
                                                const int32_t x2,
                                                const int32_t y2);

        void getTransformationsInModelTransform(ModelTransform& modelTransform) const;
        
        void setTransformationsFromModelTransform(const ModelTransform& modelTransform);
        
        ChartTwoMatrixDisplayProperties* getChartTwoMatrixDisplayProperties();

        const ChartTwoMatrixDisplayProperties* getChartTwoMatrixDisplayProperties() const;
        
        VolumeMprOrientationModeEnum::Enum getVolumeMprOrientationMode() const;
        
        void setVolumeMprOrientationMode(const VolumeMprOrientationModeEnum::Enum orientationMode);
        
        VolumeMprViewModeEnum::Enum getVolumeMprViewMode() const;
        
        void setVolumeMprViewMode(const VolumeMprViewModeEnum::Enum viewType);
        
        float getVolumeMprSliceThickness() const;
        
        void setVolumeMprSliceThickness(const float sliceThickness);
        
        bool isVolumeMprAllViewThicknessEnabled() const;
        
        void setVolumeMprAllViewThicknessEnabled(const bool enabled);
        
        bool isVolumeMprAxialSliceThicknessEnabled() const;
        
        void setVolumeMprAxialSliceThicknessEnabled(const bool enabled);
        
        bool isVolumeMprCoronalSliceThicknessEnabled() const;
        
        void setVolumeMprCoronalSliceThicknessEnabled(const bool enabled);
        
        bool isVolumeMprParasagittalSliceThicknessEnabled() const;
        
        void setVolumeMprParasagittalSliceThicknessEnabled(const bool enabled);
        
        VolumeSliceViewPlaneEnum::Enum getVolumeSliceViewPlane() const;
        
        void setVolumeSliceViewPlane(VolumeSliceViewPlaneEnum::Enum sliceAxisMode);
        
        VolumeSliceViewAllPlanesLayoutEnum::Enum getVolumeSlicePlanesAllViewLayout() const;
        
        void setVolumeSlicePlanesAllViewLayout(const VolumeSliceViewAllPlanesLayoutEnum::Enum slicePlanesAllViewLayout);
        
        VolumeSliceDrawingTypeEnum::Enum getVolumeSliceDrawingType() const;
        
        void setVolumeSliceDrawingType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType);
        
        void getValidVolumeSliceProjectionTypes(std::vector<VolumeSliceProjectionTypeEnum::Enum>& sliceProjectionTypesOut) const;
        
        VolumeSliceProjectionTypeEnum::Enum getVolumeSliceProjectionType() const;
        
        void setVolumeSliceProjectionType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType);
        
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum getVolumeSliceInterpolationEdgeEffectsMaskingType() const;
        
        void setVolumeSliceInterpolationEdgeEffectsMaskingType(const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType);
        
        int32_t getVolumeMontageNumberOfColumns() const;
        
        void setVolumeMontageNumberOfColumns(const int32_t montageNumberOfColumns);
        
        int32_t getVolumeMontageNumberOfRows() const;
        
        void setVolumeMontageNumberOfRows(const int32_t montageNumberOfRows);
        
        int32_t getVolumeMontageSliceSpacing() const;
        
        void setVolumeMontageSliceSpacing(const int32_t montageSliceSpacing);
        
        void setVolumeSlicesToOrigin();
        
        float getVolumeSliceCoordinateAxial() const;
        
        void setVolumeSliceCoordinateAxial(const float x);
        
        float getVolumeSliceCoordinateCoronal() const;
        
        void setVolumeSliceCoordinateCoronal(const float y);
        
        float getVolumeSliceCoordinateParasagittal() const;
        
        void setVolumeSliceCoordinateParasagittal(const float z);
        
        int64_t getVolumeSliceIndexAxial(const VolumeMappableInterface* volumeFile) const;
        
        void setVolumeSliceIndexAxial(const VolumeMappableInterface* volumeFile,
                                const int64_t sliceIndexAxial);
        
        int64_t getVolumeSliceIndexCoronal(const VolumeMappableInterface* volumeFile) const;
        
        void setVolumeSliceIndexCoronal(const VolumeMappableInterface* volumeFile,
                                  const int64_t sliceIndexCoronal);
        
        int64_t getVolumeSliceIndexParasagittal(const VolumeMappableInterface* volumeFile) const;
        
        void setVolumeSliceIndexParasagittal(const VolumeMappableInterface* volumeFile,
                                       const int64_t sliceIndexParasagittal);
        
        bool isVolumeSliceParasagittalEnabled() const;
        
        void setVolumeSliceParasagittalEnabled(const bool sliceEnabledParasagittal);
        
        bool isVolumeSliceCoronalEnabled() const;
        
        void setVolumeSliceCoronalEnabled(const bool sliceEnabledCoronal);
        
        bool isVolumeSliceAxialEnabled() const;
        
        void setVolumeSliceAxialEnabled(const bool sliceEnabledAxial);
        
        void selectVolumeSlicesAtOrigin();
        
        void selectVolumeSlicesAtCoordinate(const float xyz[3]);
        
        bool isIdentificationUpdateHistologySlices() const;

        void setIdentificationUpdatesHistologySlices(const bool status);
        
        bool isIdentificationUpdatesVolumeSlices() const;
        
        void setIdentificationUpdatesVolumeSlices(const bool status);

        bool isVolumeAxesCrosshairsDisplayed() const;
        
        void setVolumeAxesCrosshairsDisplayed(const bool displayed);
        
        bool isVolumeAxesCrosshairLabelsDisplayed() const;
        
        void setVolumeAxesCrosshairLabelsDisplayed(const bool displayed);
        
        bool isVolumeMontageAxesCoordinatesDisplayed() const;
        
        void setVolumeMontageAxesCoordinatesDisplayed(const bool displayed);

        int32_t getVolumeMontageCoordinatePrecision() const;
        
        void setVolumeMontageCoordinatePrecision(const int32_t volumeMontageCoordinatePrecision);
        
        HistologyCoordinate getHistologySelectedCoordinate(const HistologySlicesFile* histologySlicesFile) const;
        
        void setHistologySelectedCoordinate(const HistologyCoordinate& histologyCoordinate);
                
        void selectHistologySlicesAtOrigin(const HistologySlicesFile* histologySlicesFile);
        
        bool isLightingEnabled() const;
        
        void setLightingEnabled(const bool lightingEnabled);
        
        void reset();

        void updateChartModelYokedBrowserTabs();
        
        void updateHistologyModelYokedBrowserTabs();
        
        void updateMediaModelYokedBrowserTabs();
        
        bool isBrainModelYoked() const;
        
        bool isChartModelYoked() const;
        
        bool isMediaModelYoked() const;
        
        YokingGroupEnum::Enum getBrainModelYokingGroup() const;
        
        void setBrainModelYokingGroup(const YokingGroupEnum::Enum brainModelYokingType);
        
        YokingGroupEnum::Enum getChartModelYokingGroup() const;
        
        void setChartModelYokingGroup(const YokingGroupEnum::Enum chartModelYokingType);
        
        YokingGroupEnum::Enum getHistologyModelYokingGroup() const;
        
        void setHistologyModelYokingGroup(const YokingGroupEnum::Enum mediaModelYokingType);
        
        YokingGroupEnum::Enum getMediaModelYokingGroup() const;
        
        void setMediaModelYokingGroup(const YokingGroupEnum::Enum mediaModelYokingType);
        
        bool isWholeBrainLeftEnabled() const;
        
        void setWholeBrainLeftEnabled(const bool enabled);
        
        bool isWholeBrainRightEnabled() const;
        
        void setWholeBrainRightEnabled(const bool enabled);
        
        bool isWholeBrainCerebellumEnabled() const;
        
        void setWholeBrainCerebellumEnabled(const bool enabled);
        
        float getWholeBrainLeftRightSeparation() const;
        
        void setWholeBrainLeftRightSeparation(const float separation);
        
        float getWholeBrainCerebellumSeparation() const;
        
        void setWholeBrainCerebellumSeparation(const float separation);
        
        ViewingTransformations* getViewingTransformation();
        
        const ViewingTransformations* getViewingTransformation() const;
        
        AnnotationBrowserTab* getManualLayoutBrowserTabAnnotation();
        
        const AnnotationBrowserTab* getManualLayoutBrowserTabAnnotation() const;
        
        bool isDefaultManualTabGeometryBounds() const;
        
        AnnotationScaleBar* getScaleBar();
        
        const AnnotationScaleBar* getScaleBar() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        void setClosedStatusFromSessionManager(const bool closedStatus);
        
        void setClosedTabWindowTabBarPositionIndex(const int32_t tabBarPosition);
        
        int32_t getClosedTabWindowTabBarPositionIndex() const;
        
        void setClosedTabWindowIndex(const int32_t windowIndex);
        
        int32_t getClosedTabWindowIndex() const;
        
        GraphicsRegionSelectionBox* getMediaRegionSelectionBox();

        const GraphicsRegionSelectionBox* getMediaRegionSelectionBox() const;
        
    private:
        class ColorBarFileMap {
        public:
            ColorBarFileMap(AnnotationColorBar* colorBar,
                            CaretMappableDataFile* mapFile,
                            const int32_t mapIndex)
            : m_colorBar(colorBar),
            m_mapFile(mapFile),
            m_mapIndex(mapIndex)
            {
            }
            
            AnnotationColorBar* m_colorBar;
            CaretMappableDataFile* m_mapFile;
            int32_t m_mapIndex;
        };
        
        BrowserTabContent(const BrowserTabContent&);
        
        BrowserTabContent& operator=(const BrowserTabContent&);
        
        void updateBrainModelYokedBrowserTabs();
        
        void updateYokedModelBrowserTabs();
        
        AString getDefaultName() const;
        
        AString getTabNamePrefix() const override;
        
        void initializeScaleBar();
        
        void testForRestoreSceneWarnings(const SceneAttributes* sceneAttributes,
                                         const int32_t sceneVersion);
        
        void setMediaScaling(const float newScaleValue);
        
        static std::vector<BrowserTabContent*> getOpenBrowserTabs();
        
        void applyMouseTranslationVolumeMPR(BrainOpenGLViewportContent* viewportContent,
                                            const int32_t mousePressX,
                                            const int32_t mousePressY,
                                            const int32_t mouseX,
                                            const int32_t mouseY,
                                            const int32_t mouseDX,
                                            const int32_t mouseDY);
        
        /** Number of this tab */
        int32_t m_tabNumber;
        
        /** Selected surface model */
        ModelSurfaceSelector* m_surfaceModelSelector;
        
        /** Selected model type */
        ModelTypeEnum::Enum m_selectedModelType;
        
        /** All surface models */
        std::vector<ModelSurface*> m_allSurfaceModels;
        
        /** The volume model */
        ModelVolume* m_volumeModel;
        
        /** The whole brain model */
        ModelWholeBrain* m_wholeBrainModel;
        
        /** The surface montage model */
        ModelSurfaceMontage* m_surfaceMontageModel;
        
        /** The chart model */
        ModelChart* m_chartModel;
        
        /** The chart two model */
        ModelChartTwo* m_chartTwoModel;
        
        /** The histology model */
        ModelHistology* m_histologyModel;
        
        /** The multi-media model */
        ModelMedia* m_mediaModel;

        MediaDisplayCoordinateModeEnum::Enum m_mediaDisplayCoordinateMode = MediaDisplayCoordinateModeEnum::PIXEL;
        
        MediaDisplayCoordinateModeEnum::Enum m_histologyDisplayCoordinateMode = MediaDisplayCoordinateModeEnum::PLANE;
        
        /**
         * Name requested by user interface - reflects contents 
         * such as Surface, Volume Slices, etc
         */
        AString m_guiName;
        
        /**
         * User can set the name of the tab.
         */
        AString m_userName;
        
        /**
         * Clipping planes
         */
        ClippingPlaneGroup* m_clippingPlaneGroup;
        
        /**
         * Rotation matrix for oblique volume viewing
         */
        Matrix4x4* m_obliqueVolumeRotationMatrix;
        
        /** Brain Model Yoking group */
        YokingGroupEnum::Enum m_brainModelYokingGroup;
        
        /** Chart Model Yoking group */
        YokingGroupEnum::Enum m_chartModelYokingGroup;
        
        YokingGroupEnum::Enum m_histologyModelYokingGroup;
        
        /** Media Model Yoking Group */
        YokingGroupEnum::Enum m_mediaModelYokingGroup;
        
        /** Volume Surface Outlines */
        VolumeSurfaceOutlineSetModel* m_volumeSurfaceOutlineSetModel;
        
        /** Assists with creating/restoring scenes */
        SceneClassAssistant* m_sceneClassAssistant;
        
        /** Transformation for cerebellum viewing */
        ViewingTransformationsCerebellum* m_cerebellumViewingTransformation;
        
        /** Transformation for surface/all viewing */
        ViewingTransformations* m_viewingTransformation;
        
        /** Transformation for surface/all viewing */
        ViewingTransformations* m_flatSurfaceViewingTransformation;
        
        /** Transformation for media */
        ViewingTransformationsMedia* m_histologyViewingTransformation;
        
        /** Transformation for media viewing */
        ViewingTransformationsMedia* m_mediaViewingTransformation;
        
        /** Transformation for volume slices viewing */
        ViewingTransformationsVolume* m_volumeSliceViewingTransformation;

        /** Transformation for matrix chart two viewing */
        ViewingTransformations* m_chartTwoMatrixViewingTranformation;
        
        /** Display properties for chart two matrix */
        ChartTwoMatrixDisplayProperties* m_chartTwoMatrixDisplayProperties;
        
        /** Histology settings for histology slices */
        HistologySliceSettings* m_histologySliceSettings;
        
        /** Volume slice settings for volume slices */
        VolumeSliceSettings* m_volumeSliceSettings;
        
        /** Whole brain surface settings. */
        WholeBrainSurfaceSettings* m_wholeBrainSurfaceSettings;
        
        std::unique_ptr<AnnotationScaleBar> m_scaleBar;
        
        float m_mprRotationX = 0.0;
        
        float m_mprRotationY = 0.0;
        
        float m_mprRotationZ = 0.0;
        
        /** aspect ratio */
        float m_aspectRatio;
        
        /** aspect ratio locked */
        bool m_aspectRatioLocked;
        
        /** 
         * If true, selected volume slices in tab move to location
         *  of the identification operation.
         */
        bool m_identificationUpdatesVolumeSlices;
        
        /**
         * If true, selected histology slices in tab move to location
         *  of the identification operation.
         */
        bool m_identificationUpdatesHistologySlices;
        
        /** display crosshairs on volume slices */
        bool m_displayVolumeAxesCrosshairs;
        
        /** display crosshair labels on volume slices */
        bool m_displayVolumeAxesCrosshairLabels;
        
        /** display coordinates on montage */
        bool m_displayVolumeMontageAxesCoordinates;
        
        /** precision for coordinate on montage */
        int32_t m_volumeMontageCoordinatePrecision;
        
        /** enable lighting (shading) added 29 March 2018 */
        bool m_lightingEnabled;
        
        /*
         * True if constructing an instance
         */
        bool isExecutingConstructor;
        
        /** Manual layout brower tab annotation */
        std::unique_ptr<AnnotationBrowserTab> m_manualLayoutBrowserTabAnnotation;
        
        /** Default bounds of manual tab geometry */
        float m_defaultManualTabGeometryBounds[4];
        
        /** True if browser tab content has been closed but is available for reopening */
        bool m_closedFlag = false;
        
        /** Position in the tab bar befpre  a tab that was closed */
        int32_t m_closedTabBarPosition = -1;
        
        /** Index of window before tab was closed */
        int32_t m_closedWindowIndex = -1;
        
        /** Selection box for meda NOT copied when tab cloned*/
        std::unique_ptr<GraphicsRegionSelectionBox> m_mediaRegionSelectionBox;
        
        /**
         * NEVER access this directly as it may contain tabs that are closed but available for reopening.
         * Instead, call getOpenBrowserTabs().
         * Contains all active browser tab content instances
         */
        static std::set<BrowserTabContent*> s_allBrowserTabContent;
        
    };
    
#ifdef __BROWSER_TAB_CONTENT_DECLARE__
    std::set<BrowserTabContent*> BrowserTabContent::s_allBrowserTabContent;
#endif // __BROWSER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_CONTENT__H_
