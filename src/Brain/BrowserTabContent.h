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

namespace caret {

    class BrowserTabYoking;
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
    class Surface;
    class VolumeSurfaceOutlineSetModel;
    
    /// Maintains content in a brower's tab
    class BrowserTabContent : public CaretObject, public EventListenerInterface {
        
    public:
        BrowserTabContent(const int32_t tabNumber,
                          ModelYokingGroup* defaultYokingGroup);
        
        virtual ~BrowserTabContent();
        
        virtual void receiveEvent(Event* event);
        
        virtual AString toString() const;
        
        AString getName() const;
        
        //void setGuiName(const AString& name);
        
        AString getUserName() const;
        
        void setUserName(const AString& userName);
        
        OverlaySet* getOverlaySet();
        
        BrowserTabYoking* getBrowserTabYoking();
        
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
        
        ModelVolume* getSelectedVolumeModel();
        
        ModelWholeBrain* getSelectedWholeBrainModel();
        
        ModelSurfaceMontage* getDisplayedSurfaceMontageModel();
        
        ModelSurfaceMontage* getSelectedSurfaceMontageModel();
        
        const std::vector<ModelSurface*> getAllSurfaceModels() const;
        
        ModelSurfaceSelector* getSurfaceModelSelector();
        
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
        
    private:
        BrowserTabContent(const BrowserTabContent&);
        
        BrowserTabContent& operator=(const BrowserTabContent&);
        
        /** Number of this tab */
        int32_t tabNumber;
        
        /** Selected surface model */
        ModelSurfaceSelector* surfaceModelSelector;
        
        /** Selected model type */
        ModelTypeEnum::Enum selectedModelType;
        
        /** All surface models */
        std::vector<ModelSurface*> allSurfaceModels;
        
        /** The volume model */
        ModelVolume* volumeModel;
        
        /** The whole brain model */
        ModelWholeBrain* wholeBrainModel;
        
        /** The surface montage model */
        ModelSurfaceMontage* surfaceMontageModel;
        
        /** 
         * Name requested by user interface - reflects contents 
         * such as Surface, Volume Slices, etc
         */
        AString guiName;
        
        /**
         * User can set the name of the tab.
         */
        AString userName;
        
        /** Controls yoking */
        BrowserTabYoking* browserTabYoking;       
        
        /** Volume Surface Outlines */
        VolumeSurfaceOutlineSetModel* volumeSurfaceOutlineSetModel;
    };
    
#ifdef __BROWSER_TAB_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BROWSER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_CONTENT__H_
