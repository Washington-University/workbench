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


#include "EventListenerInterface.h"
#include "Model.h"
#include "ModelVolumeInterface.h"

#include "VolumeSliceCoordinateSelection.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewModeEnum.h"

namespace caret {

    class Brain;
    class VolumeFile;
    
    /// Controls the display of a volumes.
    class ModelVolume : public Model, public ModelVolumeInterface, public EventListenerInterface {
        
    public:        
        ModelVolume(Brain* brain);
        
        virtual ~ModelVolume();
        
        VolumeFile* getUnderlayVolumeFile(const int32_t windowTabNumber) const;
        
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
        
        VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber);
        
        const VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber) const;
        
        virtual void setSlicesToOrigin(const int32_t windowTabNumber);
        
        void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        virtual void copyTransformationsAndViews(const Model& controller,
                                                 const int32_t windowTabNumberSource,
                                                 const int32_t windowTabNumberTarget);
        
    private:
        ModelVolume(const ModelVolume&);
        
        ModelVolume& operator=(const ModelVolume&);
        
    private:
        void initializeMembersModelVolume();
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
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
        
        mutable VolumeSliceCoordinateSelection volumeSlicesSelected[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        VolumeFile* lastVolumeFile;

        /** Overlays sets for this model and for each tab */
        OverlaySet* overlaySet[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
