#ifndef __BROWSER_TAB_CONTENT__H_
#define __BROWSER_TAB_CONTENT__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "ModelTypeEnum.h"
#include "SceneableInterface.h"

namespace caret {

    class CaretDataFile;
    class CaretMappableDataFile;
    class Model;
    class ModelSurface;
    class ModelSurfaceMontage;
    class ModelSurfaceSelector;
    class ModelVolume;
    class ModelWholeBrain;
    class ModelYokingGroup;
    class OverlaySet;
    class Palette;
    class SceneClassAssistant;
    class Surface;
    class VolumeSurfaceOutlineSetModel;
    
    /// Maintains content in a brower's tab
    class BrowserTabContent : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        BrowserTabContent(const int32_t tabNumber);
        
        virtual ~BrowserTabContent();
        
        void cloneBrowserTabContent(BrowserTabContent* tabToClone);
        
        virtual void receiveEvent(Event* event);
        
        virtual AString toString() const;
        
        AString getName() const;
        
        //void setGuiName(const AString& name);
        
        AString getUserName() const;
        
        void setUserName(const AString& userName);
        
        OverlaySet* getOverlaySet();
        
        int32_t getTabNumber() const;
        
        ModelTypeEnum::Enum getSelectedModelType() const;
        
        void setSelectedModelType(ModelTypeEnum::Enum selectedModelType);
        
        const Model* getModelControllerForDisplay() const;
        
        Model* getModelControllerForDisplay();
        
        Model* getModelControllerForTransformation();
        
        ModelSurface* getDisplayedSurfaceModel();
        
        const ModelSurface* getDisplayedSurfaceModel() const;
        
        ModelVolume* getDisplayedVolumeModel();
        
        ModelWholeBrain* getDisplayedWholeBrainModel();
        
        ModelSurfaceMontage* getDisplayedSurfaceMontageModel();
        
        const std::vector<ModelSurface*> getAllSurfaceModels() const;
        
        ModelSurfaceSelector* getSurfaceModelSelector();
        
        void getFilesDisplayedInTab(std::vector<CaretDataFile*>& displayedDataFilesOut);
        
        void update(const std::vector<Model*> modelDisplayControllers);
        
        bool isSurfaceModelValid() const;
        
        bool isVolumeSliceModelValid() const;
        
        bool isWholeBrainModelValid() const;

        bool isSurfaceMontageModelValid() const;
        
        void updateTransformationsForYoking();
        
        bool isDisplayedModelSurfaceRightLateralMedialYoked() const;
        
        void getDisplayedPaletteMapFiles(std::vector<CaretMappableDataFile*>& mapFiles,
                                         std::vector<int32_t>& mapIndices);
        
        VolumeSurfaceOutlineSetModel* getVolumeSurfaceOutlineSet();
        
        const VolumeSurfaceOutlineSetModel* getVolumeSurfaceOutlineSet() const;
        
        const ModelYokingGroup* getSelectedYokingGroup() const;
        
        ModelYokingGroup* getSelectedYokingGroup();
        
        const ModelYokingGroup* getSelectedYokingGroupForModel(const Model* model) const;
        
        ModelYokingGroup* getSelectedYokingGroupForModel(const Model* model);
        
        void setSelectedYokingGroup(ModelYokingGroup* selectedYokingGroup);
        
        bool isClippingPlaneEnabled(const int32_t indx) const;
        
        void setClippingPlaneEnabled(const int32_t indx,
                                     const bool status);
        
        float getClippingPlaneThickness(const int32_t indx) const;
        
        void setClippingPlaneThickness(const int32_t indx,
                                       const float value);
        
        float getClippingPlaneCoordinate(const int32_t indx) const;
        
        void setClippingPlaneCoordinate(const int32_t indx,
                                        const float value);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        BrowserTabContent(const BrowserTabContent&);
        
        BrowserTabContent& operator=(const BrowserTabContent&);
        
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
         * Clipping thickness along axes.
         */
        float m_clippingThickness[3];
        
        /**
         * Clipping coordinate along axes.
         */
        float m_clippingCoordinate[3];
        
        /**
         * Clipping enabled.
         */
        bool m_clippingEnabled[3];
        
        /** Controls yoking */
        ModelYokingGroup* m_selectedYokingGroup;
        
        /** Volume Surface Outlines */
        VolumeSurfaceOutlineSetModel* m_volumeSurfaceOutlineSetModel;
        
        /** Assists with creating/restoring scenes */
        SceneClassAssistant* m_sceneClassAssistant;
        
    };
    
#ifdef __BROWSER_TAB_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BROWSER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_CONTENT__H_
