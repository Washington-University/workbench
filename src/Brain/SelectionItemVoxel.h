#ifndef __SELECTION_ITEM_VOXEL__H_
#define __SELECTION_ITEM_VOXEL__H_

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

#include <vector>
#include "Plane.h"
#include "SelectionItem.h"
#include "Vector3D.h"

namespace caret {

    class VolumeMappableInterface;
    
    class SelectionItemVoxel : public SelectionItem {
        
    public:
        SelectionItemVoxel();
        
        virtual ~SelectionItemVoxel();
        
        SelectionItemVoxel(const SelectionItemVoxel&);
        
        SelectionItemVoxel& operator=(const SelectionItemVoxel&);
        
        virtual bool isValid() const;
        
        const VolumeMappableInterface* getVolumeFile() const;
        
        void getVoxelIJK(int64_t voxelIJK[3]) const;
        
        Vector3D getVoxelXYZ() const;
        
        Plane getPlane() const;
        
        void setVoxelIdentification(Brain* brain,
                                    VolumeMappableInterface* volumeFile,
                                    const int64_t voxelIJK[3],
                                    const Vector3D& voxelXYZ,
                                    const Plane& plane,
                                    const double screenDepth);
        
        virtual void reset();
        
        virtual AString toString() const;

    protected:
        SelectionItemVoxel(const SelectionItemDataTypeEnum::Enum itemDataType);
        
    private:
        void copyHelperSelectionItemVoxel(const SelectionItemVoxel& idItem);

        void resetPrivate();
        
        VolumeMappableInterface* m_volumeFile = NULL;
        
        int64_t m_voxelIJK[3];
        
        Vector3D m_voxelXYZ;
        
        Plane m_plane;
        
    };
    
#ifdef __SELECTION_ITEM_VOXEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_VOXEL_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_VOXEL__H_
