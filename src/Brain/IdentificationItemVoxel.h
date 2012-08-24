#ifndef __IDENTIFICATION_ITEM_VOXEL__H_
#define __IDENTIFICATION_ITEM_VOXEL__H_

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

#include <vector>
#include "IdentificationItem.h"

namespace caret {

    class VolumeFile;
    
    class IdentificationItemVoxel : public IdentificationItem {
        
    public:
        IdentificationItemVoxel();
        
        virtual ~IdentificationItemVoxel();
        
        IdentificationItemVoxel(const IdentificationItemVoxel&);
        
        IdentificationItemVoxel& operator=(const IdentificationItemVoxel&);
        
        virtual bool isValid() const;
        
        const VolumeFile* getVolumeFile() const;
        
        void getVoxelIJK(int64_t voxelIJK[3]) const;
        
        void setVolumeFile(VolumeFile* volumeFile);
        
        void setVoxelIJK(const int64_t voxelIJK[3]);
        
        virtual void reset();
        
        virtual AString toString() const;

    private:
        void copyHelperIdentificationItemVoxel(const IdentificationItemVoxel& idItem);

        VolumeFile* m_volumeFile;
        
        int64_t m_voxelIJK[3];
    };
    
#ifdef __IDENTIFICATION_ITEM_VOXEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_ITEM_VOXEL_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_ITEM_VOXEL__H_
