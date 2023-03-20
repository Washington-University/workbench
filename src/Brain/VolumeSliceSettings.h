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
#include "VolumeMprOrientationModeEnum.h"
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
        
        VolumeSliceViewPlaneEnum::Enum getSliceViewPlane() const;
        
        void setSliceViewPlane(VolumeSliceViewPlaneEnum::Enum sliceAxisMode);
        
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
        
        int32_t getMontageSliceSpacing() const;
        
        void setMontageSliceSpacing(const int32_t montageSliceSpacing);
        
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
        
        bool isSliceParasagittalEnabled() const;
        
        void setSliceParasagittalEnabled(const bool sliceEnabledParasagittal);
        
        bool isSliceCoronalEnabled() const;
        
        void setSliceCoronalEnabled(const bool sliceEnabledCoronal);
        
        bool isSliceAxialEnabled() const;
        
        void setSliceAxialEnabled(const bool sliceEnabledAxial);
        
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

        /** Axis of slice being viewed */
        VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;

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
        int32_t m_montageSliceSpacing;
        
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
