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
#include "VolumeEditingModeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"


namespace caret {

    class VolumeFile;
    class VolumeFileMemento;
    
    class VolumeFileEditorDelegate : public CaretObject {
        
    public:
        VolumeFileEditorDelegate(VolumeFile* volumeFile);
        
        virtual ~VolumeFileEditorDelegate();
        
        bool performEditingOperation(const int64_t mapIndex,
                                     const VolumeEditingModeEnum::Enum mode,
                                     const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                     const int64_t voxelIJK[3],
                                     const int64_t brushSize[3],
                                     const float voxelValue,
                                     AString& errorMessageOut);
        
        void undo(const int64_t mapIndex);
        
        void redo(const int64_t mapIndex);
        
        // ADD_NEW_METHODS_HERE

    private:
        VolumeFileEditorDelegate(const VolumeFileEditorDelegate&);

        VolumeFileEditorDelegate& operator=(const VolumeFileEditorDelegate&);
        
        bool performTurnOnOrOff(const VolumeEditingModeEnum::Enum mode,
                                const int64_t mapIndex,
                           const VolumeSliceViewPlaneEnum::Enum slicePlane,
                           const int64_t minIJK[3],
                           const int64_t maxIJK[3],
                           const float voxelValue,
                           AString& errorMessageOut);
        
        bool performDilateOrErode(const VolumeEditingModeEnum::Enum mode,
                           const int64_t mapIndex,
                           const VolumeSliceViewPlaneEnum::Enum slicePlane,
                           const int64_t voxelIJK[3],
                           const int64_t brushSize[3],
                           const float voxelValue,
                           AString& errorMessageOut);
        
        bool performFloodFill2D(const int64_t mapIndex,
                                const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                const int64_t voxelIJK[3],
                                const int64_t brushSize[3],
                                const float voxelValue,
                                AString& errorMessageOut);
        
        bool performFloodFill3D(const int64_t mapIndex,
                                const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                const int64_t voxelIJK[3],
                                const int64_t brushSize[3],
                                const float voxelValue,
                                AString& errorMessageOut);
        
        bool performRemoveConnected2D(const int64_t mapIndex,
                                      const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                      const int64_t voxelIJK[3],
                                      const int64_t brushSize[3],
                                      const float voxelValue,
                                      AString& errorMessageOut);
        
        bool performRemoveConnected3D(const int64_t mapIndex,
                                      const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                      const int64_t voxelIJK[3],
                                      const int64_t brushSize[3],
                                      const float voxelValue,
                                      AString& errorMessageOut);
        
        bool performRetainConnected3D(const int64_t mapIndex,
                                      const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                      const int64_t voxelIJK[3],
                                      const int64_t brushSize[3],
                                      const float voxelValue,
                                      AString& errorMessageOut);
        
        bool performFloodFillAndRemoveConnected(const VolumeEditingModeEnum::Enum mode,
                                                const int64_t mapIndex,
                                                const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                const int64_t voxelIJK[3],
                                                const int64_t brushSize[3],
                                                const float voxelValue,
                                                AString& errorMessageOut);
        
        int64_t clampDimensionIndex(const int64_t maxDim,
                               int64_t dimIndex) const;
        
        void clampVoxelIndices(int64_t ijk[3]) const;
        
        void clampVoxelIndices(int64_t& i, int64_t& j, int64_t& k) const;
        
        void addToMapUndoStacks(const int32_t mapIndex,
                                VolumeFileMemento* modifiedVoxels);
        
        VolumeFile* m_volumeFile;
        
        class UndoStackForMap {
        public:
            UndoStackForMap();
            
            ~UndoStackForMap();
            
            void addModifiedVoxels(VolumeFileMemento* modifiedVoxels);

            bool hasUndo() const;
            
            bool hasRedo() const;
            
            std::vector<VolumeFileMemento*> m_undoStack;
            
            int32_t m_undoStackIndex;
        };
        
        /** 
         * Holds modifications for undo/redo operations.
         * Index into vector is the map index.
         */
        std::vector<UndoStackForMap*> m_volumeMapUndoStacks;
        
        /**
         * IJK dimensions of the volume.
         */
        int64_t m_volumeDimensions[3];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
    
#ifdef __VOLUME_FILE_EDITOR_DELEGATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_FILE_EDITOR_DELEGATE_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_EDITOR_DELEGATE_H__
