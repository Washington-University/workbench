#ifndef __VOLUME_FILE_MEMENTO_H__
#define __VOLUME_FILE_MEMENTO_H__

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



namespace caret {

    class VolumeFile;
    
    class VolumeFileMemento : public CaretObject {

    public:
        
        class VoxelModification {
        public:
            VoxelModification(const int64_t ijk[3],
                              const float value);
            
            ~VoxelModification();
            
            int64_t m_ijk[3];
            float m_value;
        };
        
        VolumeFileMemento(VolumeFile* volumeFile,
                          const int32_t mapIndex);
        
        virtual ~VolumeFileMemento();
        
        void addVoxel(const int64_t ijk[3],
                      const float value);
        
        void undo();
        
        bool isEmpty() const;
        
        int64_t getNumberOfModifiedVoxels() const;
        
        const VoxelModification* getModifiedVoxel(const int64_t index) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        
        VolumeFileMemento(const VolumeFileMemento&);

        VolumeFileMemento& operator=(const VolumeFileMemento&);
        
        VolumeFile* m_volumeFile;
        
        const int32_t m_mapIndex;
        
        std::vector<VoxelModification*> m_voxelModifications;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_FILE_MEMENTO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_FILE_MEMENTO_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_MEMENTO_H__
