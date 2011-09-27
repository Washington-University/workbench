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
#include "ModelDisplayControllerTypeEnum.h"
#include "SurfaceOverlaySet.h"
#include "YokingTypeEnum.h"

namespace caret {

    class ModelDisplayController;
    class ModelDisplayControllerSurface;
    class ModelDisplayControllerVolume;
    class ModelDisplayControllerWholeBrain;
    class Surface;
    class SurfaceNodeColoring;
    
    /// Maintains content in a brower's tab
    class BrowserTabContent : public CaretObject, public EventListenerInterface {
        
    public:
        BrowserTabContent(const int32_t tabNumber);
        
        virtual ~BrowserTabContent();
        
        virtual void receiveEvent(Event* event);
        
        virtual AString toString() const;
        
        AString getName() const;
        
        //void setGuiName(const AString& name);
        
        AString getUserName() const;
        
        void setUserName(const AString& userName);
        
        SurfaceOverlaySet* getSurfaceOverlaySet();
        
        int32_t getYokeToTabNumber() const;
        
        void setYokeToTabNumber(const int32_t yokeToTabNumber);
        
        YokingTypeEnum::Enum getYokingType() const;
        
        void setYokingType(const YokingTypeEnum::Enum yokingType);
        
        int32_t getTabNumber() const;
        
        ModelDisplayControllerTypeEnum::Enum getSelectedModelType() const;
        
        void setSelectedModelType(ModelDisplayControllerTypeEnum::Enum selectedModelType);
        
        const ModelDisplayController* getDisplayedModelController() const;
        
        ModelDisplayController* getDisplayedModelController();
        
        ModelDisplayControllerSurface* getDisplayedSurfaceModel();
        
        ModelDisplayControllerVolume* getSelectedVolumeModel();
        
        ModelDisplayControllerWholeBrain* getSelectedWholeBrainModel();
        
        const std::vector<ModelDisplayControllerSurface*> getAllSurfaceModels() const;
        
        ModelDisplayControllerSurface* getSelectedSurfaceModel();
        
        void setSelectedSurfaceModel(ModelDisplayControllerSurface* selectedSurfaceModel);
        
        void update(const std::vector<ModelDisplayController*> modelDisplayControllers);
        
        bool isSurfaceModelValid() const;
        
        bool isVolumeSliceModelValid() const;
        
        bool isWholeBrainModelValid() const;

        void invalidateSurfaceColoring();
        
        const float* getSurfaceColoring(const Surface* surface);
    private:
        BrowserTabContent(const BrowserTabContent&);
        
        BrowserTabContent& operator=(const BrowserTabContent&);
        
        /** Number of this tab */
        int32_t tabNumber;
        
        /** Selected surface model */
        ModelDisplayControllerSurface* selectedSurfaceModel;
        
        /** Selected model type */
        ModelDisplayControllerTypeEnum::Enum selectedModelType;
        
        /** All surface models */
        std::vector<ModelDisplayControllerSurface*> allSurfaceModels;
        
        /** The volume model */
        ModelDisplayControllerVolume* volumeModel;
        
        /** The whole brain model */
        ModelDisplayControllerWholeBrain* wholeBrainModel;
        
        /** 
         * Name requested by user interface - reflects contents 
         * such as Surface, Volume Slices, etc
         */
        AString guiName;
        
        /**
         * User can set the name of the tab.
         */
        AString userName;
        
        /** Tab number to which this controller is yoked */
        int32_t yokeToTabNumber;
        
        /** Type of yoking */
        YokingTypeEnum::Enum yokingType;
        
        /** The surface overlay assignments */
        SurfaceOverlaySet surfaceOverlayAssignment;
        
        /** Last cerebellum surface that was colored for this browser tab */
        Surface* surfaceCerebellumLastColored;
        
        /** Last cerebellum surface that was colored for this browser tab  */
        Surface* surfaceLeftLastColored;
        
        /** Last cerebellum surface that was colored for this browser tab  */
        Surface* surfaceRightLastColored;
        
        /** Node coloring for cerebellum surface in this browser tab */
        std::vector<float> surfaceCerebellumColoringRGBA;
        
        /** Node coloring for left surface in this browser tab */
        std::vector<float> surfaceLeftColoringRGBA;
        
        /** Node coloring for right surface in this browser tab */
        std::vector<float> surfaceRightColoringRGBA;
        
        /** Performs surface node coloring */
        SurfaceNodeColoring* surfaceColoring;
    };
    
#ifdef __BROWSER_TAB_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BROWSER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_CONTENT__H_
