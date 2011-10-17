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
#include "ModelDisplayController.h"
#include "SurfaceTypeEnum.h"


namespace caret {

    class Brain;
    class VolumeFile;
    
    /// Controls the display of a whole brain.
    class ModelDisplayControllerWholeBrain : public ModelDisplayController {
        
    public:
        ModelDisplayControllerWholeBrain(Brain* brain);
        
        virtual ~ModelDisplayControllerWholeBrain();
        
        Brain* getBrain();
        
        VolumeFile* getVolumeFile();
        
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
        
        int64_t getSliceIndexAxial(const int32_t windowTabNumber) const;
        
        void setSliceIndexAxial(const int32_t windowTabNumber,
                                const int64_t sliceIndexAxial);
        
        int64_t getSliceIndexCoronal(const int32_t windowTabNumber) const;
        
        void setSliceIndexCoronal(const int32_t windowTabNumber,
                                  const int64_t sliceIndexCoronal);
        
        int64_t getSliceIndexParagittal(const int32_t windowTabNumber) const;
        
        void setSliceIndexParasagittal(const int32_t windowTabNumber,
                                       const int64_t sliceIndexParasagittal);
        
        bool isSliceParasagittalEnabled(const int32_t windowTabNumber) const;
        
        void setSliceParasagittalEnabled(const int32_t windowTabNumber,
                                         const bool sliceEnabledParasagittal);
        
        bool isSliceCoronalEnabled(const int32_t windowTabNumber) const;
        
        void setSliceCoronalEnabled(const int32_t windowTabNumber,
                                         const bool sliceEnabledCoronal);
        
        bool isSliceAxialEnabled(const int32_t windowTabNumber) const;
        
        void setSliceAxialEnabled(const int32_t windowTabNumber,
                                         const bool sliceEnabledAxial);
    private:
        ModelDisplayControllerWholeBrain(const ModelDisplayControllerWholeBrain&);
        
        ModelDisplayControllerWholeBrain& operator=(const ModelDisplayControllerWholeBrain&);
        
    private:
        void initializeMembersModelDisplayControllerWholeBrain();
        
        void updateController();
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        /** Brain to which this controller belongs */
        Brain* brain;
        
        /** Type of surface for display */
        mutable SurfaceTypeEnum::Enum selectedSurfaceType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Available surface types */
        std::vector<SurfaceTypeEnum::Enum> availableSurfaceTypes;
        
        bool leftEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool rightEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool cerebellumEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float leftRightSeparation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float cerebellumSeparation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Parasagittal slice index */
        int64_t sliceIndexParasagittal[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Coronal slice index */
        int64_t sliceIndexCoronal[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Axial slice index */
        int64_t sliceIndexAxial[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool sliceEnabledParasagittal[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool sliceEnabledCoronal[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool sliceEnabledAxial[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__
