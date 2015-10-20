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

#include <set>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "Model.h"
#include "ModelTypeEnum.h"
#include "Plane.h"
#include "ProjectionViewTypeEnum.h"
#include "SceneableInterface.h"
#include "VolumeSliceDrawingTypeEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "YokingGroupEnum.h"

namespace caret {

    class BrainOpenGLViewportContent;
    class CaretDataFile;
    class CaretMappableDataFile;
    class ClippingPlaneGroup;
    class Matrix4x4;
    class ModelChart;
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
    class ViewingTransformationsVolume;
    class VolumeMappableInterface;
    class VolumeSliceSettings;
    class VolumeSurfaceOutlineSetModel;
    class WholeBrainSurfaceSettings;
    
    /// Maintains content in a brower's tab
    class BrowserTabContent : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        BrowserTabContent(const int32_t tabNumber);
        
        virtual ~BrowserTabContent();
        
        void cloneBrowserTabContent(BrowserTabContent* tabToClone);
        
        virtual void receiveEvent(Event* event);
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        virtual AString toString() const;
        
        AString getName() const;
        
        //void setGuiName(const AString& name);
        
        AString getUserName() const;
        
        void setUserName(const AString& userName);
        
        OverlaySet* getOverlaySet();
        
        const OverlaySet* getOverlaySet() const;
        
        int32_t getTabNumber() const;
        
        ModelTypeEnum::Enum getSelectedModelType() const;
        
        void setSelectedModelType(ModelTypeEnum::Enum selectedModelType);
        
        const Model* getModelForDisplay() const;
        
        Model* getModelForDisplay();
        
        ModelChart* getDisplayedChartModel();
        
        const ModelChart* getDisplayedChartModel() const;
        
        ModelSurface* getDisplayedSurfaceModel();
        
        const ModelSurface* getDisplayedSurfaceModel() const;
        
        ModelVolume* getDisplayedVolumeModel();
        
        const ModelVolume* getDisplayedVolumeModel() const;
        
        ModelWholeBrain* getDisplayedWholeBrainModel();
        
        ModelSurfaceMontage* getDisplayedSurfaceMontageModel();
        
        const ModelSurfaceMontage* getDisplayedSurfaceMontageModel() const;
        
        const std::vector<ModelSurface*> getAllSurfaceModels() const;
        
        ModelSurfaceSelector* getSurfaceModelSelector();
        
        bool isCerebellumDisplayed() const;
        
        bool isChartDisplayed() const;
        
        bool isFlatSurfaceDisplayed() const;
        
        bool isVolumeSlicesDisplayed() const;
        
        bool isWholeBrainDisplayed() const;
        
        void getFilesDisplayedInTab(std::vector<CaretDataFile*>& displayedDataFilesOut);
        
        void update(const std::vector<Model*> models);
        
        bool isChartModelValid() const;
        
        bool isSurfaceModelValid() const;
        
        bool isVolumeSliceModelValid() const;
        
        bool isWholeBrainModelValid() const;

        bool isSurfaceMontageModelValid() const;
        
        void getDisplayedPaletteMapFiles(std::vector<CaretMappableDataFile*>& mapFiles,
                                         std::vector<int32_t>& mapIndices);
        
        VolumeSurfaceOutlineSetModel* getVolumeSurfaceOutlineSet();
        
        const VolumeSurfaceOutlineSetModel* getVolumeSurfaceOutlineSet() const;
        
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

        ProjectionViewTypeEnum::Enum getProjectionViewType() const;
        
        void resetView();
        
        void rightView();
        
        void leftView();
        
        void anteriorView();
        
        void posteriorView();
        
        void dorsalView();
        
        void ventralView();
        
        void applyMouseRotation(BrainOpenGLViewportContent* viewportContent,
                                const int32_t mousePressX,
                                const int32_t mousePressY,
                                const int32_t mouseX,
                                const int32_t mouseY,
                                const int32_t mouseDeltaX,
                                const int32_t mouseDeltaY);
        
        void applyMouseScaling(const int32_t mouseDX,
                               const int32_t mouseDY);
        
        void applyMouseTranslation(BrainOpenGLViewportContent* viewportContent,
                                   const int32_t mousePressX,
                                   const int32_t mousePressY,
                                   const int32_t mouseDX,
                                   const int32_t mouseDY);
        
        void getTransformationsForOpenGLDrawing(const ProjectionViewTypeEnum::Enum projectionViewType,
                                                float translationOut[3],
                                                double rotationMatrixOut[16],
                                                float& scalingOut) const;
        
        void getTransformationsInModelTransform(ModelTransform& modelTransform) const;
        
        void setTransformationsFromModelTransform(const ModelTransform& modelTransform);
        
        VolumeSliceViewPlaneEnum::Enum getSliceViewPlane() const;
        
        void setSliceViewPlane(VolumeSliceViewPlaneEnum::Enum sliceAxisMode);
        
        VolumeSliceDrawingTypeEnum::Enum getSliceDrawingType() const;
        
        void setSliceDrawingType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType);
        
        VolumeSliceProjectionTypeEnum::Enum getSliceProjectionType() const;
        
        void setSliceProjectionType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType);
        
        int32_t getMontageNumberOfColumns() const;
        
        void setMontageNumberOfColumns(const int32_t montageNumberOfColumns);
        
        int32_t getMontageNumberOfRows() const;
        
        void setMontageNumberOfRows(const int32_t montageNumberOfRows);
        
        int32_t getMontageSliceSpacing() const;
        
        void setMontageSliceSpacing(const int32_t montageSliceSpacing);
        
        void setSlicesToOrigin();
        
        float getSliceCoordinateAxial() const;
        
        void setSliceCoordinateAxial(const float x);
        
        float getSliceCoordinateCoronal() const;
        
        void setSliceCoordinateCoronal(const float y);
        
        float getSliceCoordinateParasagittal() const;
        
        void setSliceCoordinateParasagittal(const float z);
        
        int64_t getSliceIndexAxial(const VolumeMappableInterface* volumeFile) const;
        
        void setSliceIndexAxial(const VolumeMappableInterface* volumeFile,
                                const int64_t sliceIndexAxial);
        
        int64_t getSliceIndexCoronal(const VolumeMappableInterface* volumeFile) const;
        
        void setSliceIndexCoronal(const VolumeMappableInterface* volumeFile,
                                  const int64_t sliceIndexCoronal);
        
        int64_t getSliceIndexParasagittal(const VolumeMappableInterface* volumeFile) const;
        
        void setSliceIndexParasagittal(const VolumeMappableInterface* volumeFile,
                                       const int64_t sliceIndexParasagittal);
        
        bool isSliceParasagittalEnabled() const;
        
        void setSliceParasagittalEnabled(const bool sliceEnabledParasagittal);
        
        bool isSliceCoronalEnabled() const;
        
        void setSliceCoronalEnabled(const bool sliceEnabledCoronal);
        
        bool isSliceAxialEnabled() const;
        
        void setSliceAxialEnabled(const bool sliceEnabledAxial);
        
        void updateForVolumeFile(const VolumeMappableInterface* volumeFile);
        
        void selectSlicesAtOrigin();
        
        void selectSlicesAtCoordinate(const float xyz[3]);
        
        bool isIdentificationUpdatesVolumeSlices() const;
        
        void setIdentificationUpdatesVolumeSlices(const bool status);

        void reset();

        void updateYokedBrowserTabs();
        
        bool isYoked() const;
        
        YokingGroupEnum::Enum getYokingGroup() const;
        
        void setYokingGroup(const YokingGroupEnum::Enum yokingType);
        
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
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        BrowserTabContent(const BrowserTabContent&);
        
        BrowserTabContent& operator=(const BrowserTabContent&);
        
        VolumeSliceViewPlaneEnum::Enum getSliceViewPlaneForVolumeAllSliceView(const int viewport[4],
                                                                  const int32_t mousePressX,
                                                                  const int32_t mousePressY,
                                                                              int sliceViewportOut[4]) const;
        
        ViewingTransformations* getViewingTransformation();
        
        const ViewingTransformations* getViewingTransformation() const;
        
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
        
        /** Yoking group */
        YokingGroupEnum::Enum m_yokingGroup;
        
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
        
        /** Transformation for volume slices viewing */
        ViewingTransformationsVolume* m_volumeSliceViewingTransformation;

        /** Volume slice settings for volume slices */
        VolumeSliceSettings* m_volumeSliceSettings;
        
        /** Whole brain surface settings. */
        WholeBrainSurfaceSettings* m_wholeBrainSurfaceSettings;
        
        /** 
         * If true, selected volume slices in tab move to location
         *  of the identification operation.
         */
        bool m_identificationUpdatesVolumeSlices;
        
        /*
         * True if constructing an instance
         */
        bool isExecutingConstructor;
        
        /** Contains all active browser tab content instances */
        static std::set<BrowserTabContent*> s_allBrowserTabContent;
        
    };
    
#ifdef __BROWSER_TAB_CONTENT_DECLARE__
    std::set<BrowserTabContent*> BrowserTabContent::s_allBrowserTabContent;
#endif // __BROWSER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_CONTENT__H_
