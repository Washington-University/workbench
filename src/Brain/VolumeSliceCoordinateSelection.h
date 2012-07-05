#ifndef __VOLUME_SLICE_COORDINATE_SELECTION__H_
#define __VOLUME_SLICE_COORDINATE_SELECTION__H_

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
#include "SceneClassAssistant.h"
#include "SceneableInterface.h"

namespace caret {

    class VolumeFile;
    
    class VolumeSliceCoordinateSelection : public CaretObject, public SceneableInterface {
        
    public:
        VolumeSliceCoordinateSelection();
        
        virtual ~VolumeSliceCoordinateSelection();
        
        float getSliceCoordinateAxial() const;
        
        void setSliceCoordinateAxial(const float x);
        
        float getSliceCoordinateCoronal() const;
        
        void setSliceCoordinateCoronal(const float y);
        
        float getSliceCoordinateParasagittal() const;
        
        void setSliceCoordinateParasagittal(const float z);
        
        int64_t getSliceIndexAxial(const VolumeFile* volumeFile) const;
        
        void setSliceIndexAxial(const VolumeFile* volumeFile,
                                const int64_t sliceIndexAxial);
        
        int64_t getSliceIndexCoronal(const VolumeFile* volumeFile) const;
        
        void setSliceIndexCoronal(const VolumeFile* volumeFile,
                                  const int64_t sliceIndexCoronal);
        
        int64_t getSliceIndexParasagittal(const VolumeFile* volumeFile) const;
        
        void setSliceIndexParasagittal(const VolumeFile* volumeFile,
                                       const int64_t sliceIndexParasagittal);
        
        bool isSliceParasagittalEnabled() const;
        
        void setSliceParasagittalEnabled(const bool sliceEnabledParasagittal);
        
        bool isSliceCoronalEnabled() const;
        
        void setSliceCoronalEnabled(const bool sliceEnabledCoronal);
        
        bool isSliceAxialEnabled() const;
        
        void setSliceAxialEnabled(const bool sliceEnabledAxial);
        
        void updateForVolumeFile(const VolumeFile* volumeFile);
        
        void selectSlicesAtOrigin();
        
        void selectSlicesAtCoordinate(const float xyz[3]);
        
        void reset();
        
        void copySelections(const VolumeSliceCoordinateSelection& vscs);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        VolumeSliceCoordinateSelection(const VolumeSliceCoordinateSelection&);

        VolumeSliceCoordinateSelection& operator=(const VolumeSliceCoordinateSelection&);
        
    public:
        virtual AString toString() const;
        
    private:
        mutable float m_sliceCoordinateParasagittal;
        
        mutable float m_sliceCoordinateCoronal;
        
        mutable float m_sliceCoordinateAxial;
        
        bool m_sliceEnabledParasagittal;
        
        bool m_sliceEnabledCoronal;
        
        bool m_sliceEnabledAxial;
        
        bool m_initializedFlag;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __VOLUME_SLICE_COORDINATE_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SLICE_COORDINATE_SELECTION_DECLARE__

} // namespace
#endif  //__VOLUME_SLICE_COORDINATE_SELECTION__H_
