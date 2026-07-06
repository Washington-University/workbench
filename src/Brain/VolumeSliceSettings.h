#ifndef __VOLUME_SLICE_SETTINGS_H__
#define __VOLUME_SLICE_SETTINGS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <memory>

#include "CaretObject.h"
#include "ModelTypeEnum.h"
#include "SceneableInterface.h"
#include "VolumeMontageSliceOrderModeEnum.h"
#include "VolumeSliceDrawingTypeEnum.h"
#include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewAllPlanesLayoutEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class PlainTextStringBuilder;
    class SceneClassAssistant;
    class VolumeMappableInterface;
    class VolumeMprSettings;
    
    class VolumeSliceSettings : public CaretObject, public SceneableInterface {
        
    public:
        VolumeSliceSettings();
        
        virtual ~VolumeSliceSettings();
        
        VolumeSliceSettings(const VolumeSliceSettings& obj);

        VolumeSliceSettings& operator=(const VolumeSliceSettings& obj);
        
        void copyToMeForYoking(const VolumeSliceSettings& volumeSliceSettings);
        
        bool isShowVolumeViewAxialSlice() const;
        
        void setShowVolumeViewAxialSlice(const bool status);
        
        bool isShowVolumeViewCoronalSlice() const;
        
        void setShowVolumeViewCoronalSlice(const bool status);
        
        bool isShowVolumeViewParasagittalSlice() const;
        
        void setShowVolumeViewParasagittalSlice(const bool status);
        
        bool isShowVolumeViewRotationAxis() const;
        
        void setShowVolumeViewRotationAxis(const bool status);
        
        VolumeSliceViewAllPlanesLayoutEnum::Enum getSlicePlanesAllViewLayout() const;
        
        void setSlicePlanesAllViewLayout(const VolumeSliceViewAllPlanesLayoutEnum::Enum slicePlanesAllViewLayout);
        
        VolumeSliceDrawingTypeEnum::Enum getSliceDrawingType() const;

        void setSliceDrawingType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType);
        
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum getVolumeSliceInterpolationEdgeEffectsMaskingType() const;
        
        void setVolumeSliceInterpolationEdgeEffectsMaskingType(const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType);
        
        VolumeSliceProjectionTypeEnum::Enum getSliceProjectionType() const;
        
        void setSliceProjectionType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType);
        
        int32_t getMontageNumberOfColumns() const;
        
        void setMontageNumberOfColumns(const int32_t montageNumberOfColumns);
        
        int32_t getMontageNumberOfRows() const;
        
        void setMontageNumberOfRows(const int32_t montageNumberOfRows);
        
        float getMontageSliceSpacing() const;
        
        void setMontageSliceSpacing(const float montageSliceSpacing);
        
        float getMontageSliceSpacingStepValue() const;
        
        int32_t getMontageSliceSpacingDecimals() const;
        
        VolumeMontageSliceOrderModeEnum::Enum getMontageSliceOrderMode() const;
        
        void setMontageSliceOrderMode(const VolumeMontageSliceOrderModeEnum::Enum sliceOrderMode);
        
        void getSlicesParasagittalCoronalAxial(const VolumeMappableInterface* volumeInterface,
                                               int64_t& parasagittalIndexOut,
                                               int64_t& coronalIndexOut,
                                               int64_t& axialIndexOut) const;
        
        void setSlicesParasagittalCoronalAxial(const VolumeMappableInterface* volumeInterface,
                                               const int64_t newParasagittalIndex,
                                               const int64_t newCoronalIndex,
                                               const int64_t newAxialIndex);
        
        float getSliceCoordinateAxial() const;
        
        void setSliceCoordinateAxial(const float x);
        
        float getSliceCoordinateCoronal() const;
        
        void setSliceCoordinateCoronal(const float y);
        
        float getSliceCoordinateParasagittal() const;
        
        void setSliceCoordinateParasagittal(const float z);
        
        int64_t getSliceIndexAxial(const VolumeMappableInterface* volumeFile) const;
        
        void setSliceIndexAxial(const VolumeMappableInterface* volumeFile,
                                const int64_t sliceIndexAxial);
        
        int64_t getSliceIndexCoronal(const VolumeMappableInterface* volumeFile) const;
        
        void setSliceIndexCoronal(const VolumeMappableInterface* volumeFile,
                                  const int64_t sliceIndexCoronal);
        
        int64_t getSliceIndexParasagittal(const VolumeMappableInterface* volumeFile) const;
        
        void setSliceIndexParasagittal(const VolumeMappableInterface* volumeFile,
                                       const int64_t sliceIndexParasagittal);
        
        bool isWholeBrainSliceParasagittalEnabled() const;
        
        void setWholeBrainSliceParasagittalEnabled(const bool sliceEnabledParasagittal);
        
        bool isWholeBrainSliceCoronalEnabled() const;
        
        void setWholeBrainSliceCoronalEnabled(const bool sliceEnabledCoronal);
        
        bool isWholeBrainSliceAxialEnabled() const;
        
        void setWholeBrainSliceAxialEnabled(const bool sliceEnabledAxial);
        
        void selectSlicesAtOrigin(const VolumeMappableInterface* volumeInterface);
        
        void selectSlicesAtCoordinate(const float xyz[3]);
        
        VolumeMprSettings* getMprSettings();
        
        const VolumeMprSettings* getMprSettings() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(const ModelTypeEnum::Enum modelType,
                                             PlainTextStringBuilder& descriptionOut) const;
        
    private:
        void copyHelperVolumeSliceSettings(const VolumeSliceSettings& obj);

        void reset();
        
        // ADD_NEW_MEMBERS_HERE

        /* Show Axial Slice in Volume View */
        bool m_showVolumeViewAxialSliceFlag = true;
        
        /* Show Coronal Slice in Volume View */
        bool m_showVolumeViewCoronalSliceFlag = true;
        
        /* Show Parasagittal Slice in Volume View  */
        bool m_showVolumeViewParasagittalSliceFlag = true;
        
        /* Show Rotation Axis in Volume View */
        bool m_showVolumeViewRotationAxisFlag = true;
        
        /** Axis of slice being viewed No longer used but it is in older scenes and used during scene restoration */
        VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane = VolumeSliceViewPlaneEnum::ALL;

        /** Layout of slice in all slices view */
        VolumeSliceViewAllPlanesLayoutEnum::Enum m_slicePlanesAllViewLayout;
        
        /** Type of slice drawing (single/montage) */
        VolumeSliceDrawingTypeEnum::Enum m_sliceDrawingType;
        
        /** Type of masking for oblique slice drawing */
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum m_volumeSliceInterpolationEdgeEffectsMaskingType;
        
        /** Type of slice projection (oblique/orthogonal) */
        VolumeSliceProjectionTypeEnum::Enum m_sliceProjectionType;
        
        /** Number of montage rows */
        int32_t m_montageNumberOfRows;
        
        /** Number of montage columns */
        int32_t m_montageNumberOfColumns;
        
        /** Montage slice spacing */
        mutable float m_montageSliceSpacing;
        
        VolumeMontageSliceOrderModeEnum::Enum m_montageSliceOrderMode = VolumeMontageSliceOrderModeEnum::WORKBENCH;

        mutable float m_sliceCoordinateParasagittal;
        
        mutable float m_sliceCoordinateCoronal;
        
        mutable float m_sliceCoordinateAxial;

        bool m_sliceEnabledParasagittal;
        
        bool m_sliceEnabledCoronal;
        
        bool m_sliceEnabledAxial;
        
        bool m_initializedFlag;
        
        std::unique_ptr<VolumeMprSettings> m_mprSettings;

        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __VOLUME_SLICE_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SLICE_SETTINGS_DECLARE__

} // namespace
#endif  //__VOLUME_SLICE_SETTINGS_H__
