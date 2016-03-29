#ifndef __VOLUME_FILE_EDITOR_DELEGATE_H__
#define __VOLUME_FILE_EDITOR_DELEGATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include "CaretObject.h"
#include "Matrix4x4.h"
#include "VolumeEditingModeEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"


namespace caret {

    class CaretUndoStack;
    class VolumeFile;
    class VolumeMapUndoCommand;
    
    class VolumeFileEditorDelegate : public CaretObject {
        
    public:
        VolumeFileEditorDelegate(VolumeFile* volumeFile);
        
        virtual ~VolumeFileEditorDelegate();
        
        void clear();
        
        void updateIfVolumeFileChangedNumberOfMaps();
        
        bool performEditingOperation(const int64_t mapIndex,
                                     const VolumeEditingModeEnum::Enum mode,
                                     const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                     const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                     const Matrix4x4& obliqueRotationMatrix,
                                     const float voxelDiffXYZ[3],
                                     const int64_t voxelIJK[3],
                                     const int64_t brushSize[3],
                                     const float voxelValueOn,
                                     const float voxelValueOff,
                                     AString& errorMessageOut);
        
        bool undo(const int64_t mapIndex,
                  AString& errorMessageOut);
        
        bool reset(const int64_t mapIndex,
                   AString& errorMessageOut);
        
        bool redo(const int64_t mapIndex,
                  AString& errorMessageOut);
        
        bool isLocked(const int64_t mapIndex) const;
        
        void setLocked(const int64_t mapIndex,
                       const bool locked);
        
        // ADD_NEW_METHODS_HERE

    private:
        class EditInfo {
        public:
            EditInfo(const int32_t mapIndex,
                     const VolumeEditingModeEnum::Enum mode,
                     const VolumeSliceViewPlaneEnum::Enum slicePlane,
                     const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                     const Matrix4x4& obliqueRotationMatrix,
                     const float voxelDiffXYZ[3],
                     const int64_t voxelIJK[3],
                     const int64_t ijkMin[3],
                     const int64_t ijkMax[3],
                     const int64_t brushSize[3],
                     const float voxelValueOn,
                     const float voxelValueOff)
            : m_mapIndex(mapIndex),
            m_mode(mode),
            m_slicePlane(slicePlane),
            m_sliceProjectionType(sliceProjectionType),
            m_obliqueRotationMatrix(obliqueRotationMatrix),
            m_voxelValueOn(voxelValueOn),
            m_voxelValueOff(voxelValueOff) {
                m_voxelDiffXYZ[0] = voxelDiffXYZ[0];
                m_voxelDiffXYZ[1] = voxelDiffXYZ[1];
                m_voxelDiffXYZ[2] = voxelDiffXYZ[2];
                
                m_voxelIJK[0] = voxelIJK[0];
                m_voxelIJK[1] = voxelIJK[1];
                m_voxelIJK[2] = voxelIJK[2];
                
                m_brushSize[0] = brushSize[0];
                m_brushSize[1] = brushSize[1];
                m_brushSize[2] = brushSize[2];
                
                m_ijkMin[0] = ijkMin[0];
                m_ijkMin[1] = ijkMin[1];
                m_ijkMin[2] = ijkMin[2];
                
                m_ijkMax[0] = ijkMax[0];
                m_ijkMax[1] = ijkMax[1];
                m_ijkMax[2] = ijkMax[2];
            }
            
            const int32_t m_mapIndex;
            const VolumeEditingModeEnum::Enum m_mode;
            const VolumeSliceViewPlaneEnum::Enum m_slicePlane;
            const VolumeSliceProjectionTypeEnum::Enum m_sliceProjectionType;
            const Matrix4x4 m_obliqueRotationMatrix;
            float m_voxelDiffXYZ[3];
            int64_t m_voxelIJK[3];
            int64_t m_ijkMin[3];
            int64_t m_ijkMax[3];
            int64_t m_brushSize[3];
            const float m_voxelValueOn;
            const float m_voxelValueOff;
        };
        
        VolumeFileEditorDelegate(const VolumeFileEditorDelegate&);

        VolumeFileEditorDelegate& operator=(const VolumeFileEditorDelegate&);
        
        bool performTurnOnOrOffOblique(const EditInfo& editInfo,
                                          AString& errorMessageOut);
        
        bool performTurnOnOrOffOrthogonal(const EditInfo& editInfo,
                                          AString& errorMessageOut);
        
        bool performDilateOrErode(const EditInfo& editInfo,
                           AString& errorMessageOut);
        
        bool performFloodFill2D(const EditInfo& editInfo,
                                AString& errorMessageOut);
        
        bool performFloodFill3D(const EditInfo& editInfo,
                                AString& errorMessageOut);
        
        bool performRemoveConnected2D(const EditInfo& editInfo,
                                      AString& errorMessageOut);
        
        bool performRemoveConnected3D(const EditInfo& editInfo,
                                      AString& errorMessageOut);
        
        bool performRetainConnected3D(const EditInfo& editInfo,
                                      AString& errorMessageOut);
        
        bool performFloodFillAndRemoveConnected(const EditInfo& editInfo,
                                                AString& errorMessageOut);
        
        int64_t clampDimensionIndex(const int64_t maxDim,
                               int64_t dimIndex) const;
        
        void clampVoxelIndices(int64_t ijk[3]) const;
        
        void clampVoxelIndices(int64_t& i, int64_t& j, int64_t& k) const;
        
        void addToMapUndoStacks(const int32_t mapIndex,
                                VolumeMapUndoCommand* modifiedVoxels);
        
        VolumeFile* m_volumeFile;

        /** 
         * Holds modifications for undo/redo operations.
         * Index into vector is the map index.
         */
        std::vector<CaretUndoStack*> m_volumeMapUndoStacks;
        
        /**
         * IJK dimensions of the volume.
         */
        int64_t m_volumeDimensions[3];
        
        /**
         * A "lock" to prevent editing of a volume's map.
         */
        std::vector<bool> m_volumeMapEditingLocked;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
    
#ifdef __VOLUME_FILE_EDITOR_DELEGATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_FILE_EDITOR_DELEGATE_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_EDITOR_DELEGATE_H__
