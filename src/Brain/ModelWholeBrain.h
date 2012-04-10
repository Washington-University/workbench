#ifndef __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__
#define __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include "BrainConstants.h"
#include "EventListenerInterface.h"
#include "Model.h"
#include "SurfaceTypeEnum.h"
#include "VolumeSliceCoordinateSelection.h"


namespace caret {

    class Brain;
    class VolumeFile;
    
    /// Controls the display of a whole brain.
    class ModelWholeBrain : public Model, public EventListenerInterface {
        
    public:
        ModelWholeBrain(Brain* brain);
        
        virtual ~ModelWholeBrain();
        
        VolumeFile* getUnderlayVolumeFile(const int32_t windowTabNumber) const;
        
        void getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum>& surfaceTypesOut);
        
        SurfaceTypeEnum::Enum getSelectedSurfaceType(const int32_t windowTabNumber);
        
        void setSelectedSurfaceType(const int32_t windowTabNumber,
                                    const SurfaceTypeEnum::Enum surfaceType);
        
        bool isLeftEnabled(const int32_t windowTabNumber) const;
        
        void setLeftEnabled(const int32_t windowTabNumber,
                            const bool enabled);
        
        bool isRightEnabled(const int32_t windowTabNumber) const;
        
        void setRightEnabled(const int32_t windowTabNumber,
                             const bool enabled);
        
        bool isCerebellumEnabled(const int32_t windowTabNumber) const;
        
        void setCerebellumEnabled(const int32_t windowTabNumber,
                                  const bool enabled);
        
        float getLeftRightSeparation(const int32_t windowTabNumber) const;
        
        void setLeftRightSeparation(const int32_t windowTabNumber,
                                    const float separation);
        
        float getCerebellumSeparation(const int32_t windowTabNumber) const;
        
        void setCerebellumSeparation(const int32_t windowTabNumber,
                                    const float separation);
        
        VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber);
        
        const VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber) const;
        
        virtual void setSlicesToOrigin(const int32_t windowTabNumber);
        
        Surface* getSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber);
        
        void setSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber,
                                    Surface* surface);
        
        void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
                                            
    private:
        ModelWholeBrain(const ModelWholeBrain&);
        
        ModelWholeBrain& operator=(const ModelWholeBrain&);
        
    private:
        void initializeMembersModelWholeBrain();
        
        void updateController();
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        /** Type of surface for display */
        mutable SurfaceTypeEnum::Enum selectedSurfaceType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Selected surface for structure/surface-type */
        std::map<std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum>, Surface*> selectedSurface[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Available surface types */
        std::vector<SurfaceTypeEnum::Enum> availableSurfaceTypes;
        
        bool leftEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool rightEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool cerebellumEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float leftRightSeparation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float cerebellumSeparation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        mutable VolumeSliceCoordinateSelection volumeSlicesSelected[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        VolumeFile* lastVolumeFile;

        /** Overlays sets for this model and for each tab */
        OverlaySet* overlaySet[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__
