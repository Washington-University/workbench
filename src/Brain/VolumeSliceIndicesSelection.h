#ifndef __VOLUME_SLICE_INDICES_SELECTION__H_
#define __VOLUME_SLICE_INDICES_SELECTION__H_

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

namespace caret {

    class VolumeFile;
    
    class VolumeSliceIndicesSelection : public CaretObject {
        
    public:
        VolumeSliceIndicesSelection();
        
        virtual ~VolumeSliceIndicesSelection();
        
        int64_t getSliceIndexAxial() const;
        
        void setSliceIndexAxial(const int64_t sliceIndexAxial);
        
        int64_t getSliceIndexCoronal() const;
        
        void setSliceIndexCoronal(const int64_t sliceIndexCoronal);
        
        int64_t getSliceIndexParasagittal() const;
        
        void setSliceIndexParasagittal(const int64_t sliceIndexParasagittal);
        
        bool isSliceParasagittalEnabled() const;
        
        void setSliceParasagittalEnabled(const bool sliceEnabledParasagittal);
        
        bool isSliceCoronalEnabled() const;
        
        void setSliceCoronalEnabled(const bool sliceEnabledCoronal);
        
        bool isSliceAxialEnabled() const;
        
        void setSliceAxialEnabled(const bool sliceEnabledAxial);
        
        void updateForVolumeFile(/*const*/ VolumeFile* volumeFile);
        
        void selectSlicesAtOrigin(/*const*/ VolumeFile* volumeFile);
        
        void reset();
        
    private:
        VolumeSliceIndicesSelection(const VolumeSliceIndicesSelection&);

        VolumeSliceIndicesSelection& operator=(const VolumeSliceIndicesSelection&);
        
    public:
        virtual AString toString() const;
        
    private:
        int64_t sliceIndexParasagittal;
        
        int64_t sliceIndexCoronal;
        
        int64_t sliceIndexAxial;
        
        bool sliceEnabledParasagittal;
        
        bool sliceEnabledCoronal;
        
        bool sliceEnabledAxial;
        
        bool initializedFlag;
    };
    
#ifdef __VOLUME_SLICE_INDICES_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SLICE_INDICES_SELECTION_DECLARE__

} // namespace
#endif  //__VOLUME_SLICE_INDICES_SELECTION__H_
