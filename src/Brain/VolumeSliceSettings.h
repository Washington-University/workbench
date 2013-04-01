#ifndef __VOLUME_SLICE_SETTINGS_H__
#define __VOLUME_SLICE_SETTINGS_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretObject.h"
#include "SceneableInterface.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSliceViewModeEnum.h"

namespace caret {

    class SceneClassAssistant;
    class VolumeFile;
    class VolumeSliceCoordinateSelection;
    
    class VolumeSliceSettings : public CaretObject, public SceneableInterface {
        
    public:
        VolumeSliceSettings();
        
        virtual ~VolumeSliceSettings();
        
        VolumeSliceSettings(const VolumeSliceSettings& obj);

        VolumeSliceSettings& operator=(const VolumeSliceSettings& obj);
        
        VolumeSliceViewPlaneEnum::Enum getSliceViewPlane() const;
        
        void setSliceViewPlane(VolumeSliceViewPlaneEnum::Enum sliceAxisMode);
        
        VolumeSliceViewModeEnum::Enum getSliceViewMode() const;
        
        void setSliceViewMode(VolumeSliceViewModeEnum::Enum sliceViewMode);
        
        int32_t getMontageNumberOfColumns() const;
        
        void setMontageNumberOfColumns(const int32_t montageNumberOfColumns);
        
        int32_t getMontageNumberOfRows() const;
        
        void setMontageNumberOfRows(const int32_t montageNumberOfRows);
        
        int32_t getMontageSliceSpacing() const;
        
        void setMontageSliceSpacing(const int32_t montageSliceSpacing);
        
        VolumeSliceCoordinateSelection* getSelectedVolumeSlices(VolumeFile* underlayVolumeFile);
        
        const VolumeSliceCoordinateSelection* getSelectedVolumeSlices(VolumeFile* underlayVolumeFile) const;
        
        void setSlicesToOrigin();

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
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperVolumeSliceSettings(const VolumeSliceSettings& obj);

        // ADD_NEW_MEMBERS_HERE

        /** Axis of slice being viewed */
        VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;
        
        /** Type of slice viewing */
        VolumeSliceViewModeEnum::Enum m_sliceViewMode;
        
        /** Number of montage rows */
        int32_t m_montageNumberOfRows;
        
        /** Number of montage columns */
        int32_t m_montageNumberOfColumns;
        
        /** Montage slice spacing */
        int32_t m_montageSliceSpacing;
        
        mutable float m_sliceCoordinateParasagittal;
        
        mutable float m_sliceCoordinateCoronal;
        
        mutable float m_sliceCoordinateAxial;
        
        bool m_sliceEnabledParasagittal;
        
        bool m_sliceEnabledCoronal;
        
        bool m_sliceEnabledAxial;
        
        bool m_initializedFlag;
        
        //VolumeFile* m_lastVolumeFile;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __VOLUME_SLICE_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SLICE_SETTINGS_DECLARE__

} // namespace
#endif  //__VOLUME_SLICE_SETTINGS_H__
