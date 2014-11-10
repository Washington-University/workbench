#ifndef __SELECTION_ITEM_VOXEL_EDITING_H__
#define __SELECTION_ITEM_VOXEL_EDITING_H__

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


#include "SelectionItemVoxel.h"



namespace caret {

    class VolumeFile;
    
    class SelectionItemVoxelEditing : public SelectionItemVoxel {
        
    public:
        SelectionItemVoxelEditing();
        
        virtual ~SelectionItemVoxelEditing();
        
        const VolumeFile* getVolumeFileForEditing() const;
        
        VolumeFile* getVolumeFileForEditing();
        
        void setVolumeFileForEditing(VolumeFile* volumeFile);
        
        void setVoxelDiffXYZ(const float voxelDiffXYZ[3]);
        
        void getVoxelDiffXYZ(float voxelDiffXYZ[3]) const;
        
        virtual void reset();
        
        // ADD_NEW_METHODS_HERE

    private:
        SelectionItemVoxelEditing(const SelectionItemVoxelEditing&);

        SelectionItemVoxelEditing& operator=(const SelectionItemVoxelEditing&);
        
        VolumeFile* m_volumeFileForEditing;
        
        float m_voxelDiffXYZ[3];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_VOXEL_EDITING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_VOXEL_EDITING_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_VOXEL_EDITING_H__
