#ifndef __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
#define __MODEL_DISPLAY_CONTROLLER_VOLUME_H__

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


#include "ModelDisplayController.h"

#include "VolumeSliceIndicesSelection.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewModeEnum.h"

namespace caret {

    class Brain;
    class VolumeFile;
    
    /// Controls the display of a volumes.
    class ModelDisplayControllerVolume : public ModelDisplayController {
        
    public:        
        ModelDisplayControllerVolume(Brain* brain);
        
        virtual ~ModelDisplayControllerVolume();
        
        Brain* getBrain();
        
        VolumeFile* getUnderlayVolumeFile(const int32_t windowTabNumber);
        
        VolumeSliceViewPlaneEnum::Enum getSliceViewPlane(const int32_t windowTabNumber) const;
        
        void setSliceViewPlane(const int32_t windowTabNumber,
                              VolumeSliceViewPlaneEnum::Enum sliceAxisMode);
        
        VolumeSliceViewModeEnum::Enum getSliceViewMode(const int32_t windowTabNumber) const;
        
        void setSliceViewMode(const int32_t windowTabNumber,
                              VolumeSliceViewModeEnum::Enum sliceViewMode);
                
        int32_t getMontageNumberOfColumns(const int32_t windowTabNumber) const;
        
        void setMontageNumberOfColumns(const int32_t windowTabNumber,
                                    const int32_t montageNumberOfColumns);
        
        int32_t getMontageNumberOfRows(const int32_t windowTabNumber) const;
        
        void setMontageNumberOfRows(const int32_t windowTabNumber,
                                    const int32_t montageNumberOfRows);
        
        int32_t getMontageSliceSpacing(const int32_t windowTabNumber) const;
        
        void setMontageSliceSpacing(const int32_t windowTabNumber,
                                    const int32_t montageSliceSpacing);
        
        void updateController(const int32_t windowTabNumber);
        
        VolumeSliceIndicesSelection* getSelectedVolumeSlices(const int32_t windowTabNumber);
        
        const VolumeSliceIndicesSelection* getSelectedVolumeSlices(const int32_t windowTabNumber) const;
        
        virtual void setSlicesToOrigin(const int32_t windowTabNumber);
        
    private:
        ModelDisplayControllerVolume(const ModelDisplayControllerVolume&);
        
        ModelDisplayControllerVolume& operator=(const ModelDisplayControllerVolume&);
        
    private:
        void initializeMembersModelDisplayControllerVolume();
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        /** Brain which contains the volumes */
        Brain* brain;
        
        /** Axis of slice being viewed */
        VolumeSliceViewPlaneEnum::Enum sliceViewPlane[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Type of slice viewing */
        VolumeSliceViewModeEnum::Enum sliceViewMode[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Number of montage rows */
        int32_t montageNumberOfRows[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Number of montage columns */
        int32_t montageNumberOfColumns[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Montage slice spacing */
        int32_t montageSliceSpacing[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        VolumeSliceIndicesSelection volumeSlicesSelected[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        VolumeFile* lastVolumeFile;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
