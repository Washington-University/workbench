#ifndef __MODEL_VOLUME_INTERFACE_H__
#define __MODEL_VOLUME_INTERFACE_H__

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

#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewModeEnum.h"

namespace caret {

    class VolumeSliceCoordinateSelection;
    
    /// Interface for models that contain volume slicees    
    class ModelVolumeInterface {
        
    protected:        
        ModelVolumeInterface() { }
        
    public:        
        virtual ~ModelVolumeInterface() { }
        
        virtual VolumeSliceViewPlaneEnum::Enum getSliceViewPlane(const int32_t windowTabNumber) const = 0;
        
        virtual void setSliceViewPlane(const int32_t windowTabNumber,
                              VolumeSliceViewPlaneEnum::Enum sliceAxisMode) = 0;
        
        virtual VolumeSliceViewModeEnum::Enum getSliceViewMode(const int32_t windowTabNumber) const = 0;
        
        virtual void setSliceViewMode(const int32_t windowTabNumber,
                              VolumeSliceViewModeEnum::Enum sliceViewMode) = 0;
                
        virtual int32_t getMontageNumberOfColumns(const int32_t windowTabNumber) const = 0;
        
        virtual void setMontageNumberOfColumns(const int32_t windowTabNumber,
                                    const int32_t montageNumberOfColumns) = 0;
        
        virtual int32_t getMontageNumberOfRows(const int32_t windowTabNumber) const = 0;
        
        virtual void setMontageNumberOfRows(const int32_t windowTabNumber,
                                    const int32_t montageNumberOfRows) = 0;
        
        virtual int32_t getMontageSliceSpacing(const int32_t windowTabNumber) const = 0;
        
        virtual void setMontageSliceSpacing(const int32_t windowTabNumber,
                                    const int32_t montageSliceSpacing) = 0;
        
        virtual VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber) = 0;
        
        virtual const VolumeSliceCoordinateSelection* getSelectedVolumeSlices(const int32_t windowTabNumber) const = 0;
        
        virtual void setSlicesToOrigin(const int32_t windowTabNumber) = 0;
        
    private:
        ModelVolumeInterface(const ModelVolumeInterface&);
        
        ModelVolumeInterface& operator=(const ModelVolumeInterface&);
        
    };

} // namespace

#endif // __MODEL_VOLUME_INTERFACE_H__
