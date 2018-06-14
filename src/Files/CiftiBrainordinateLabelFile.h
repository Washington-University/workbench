#ifndef __CIFTI_BRAINORDINATE_LABEL_FILE_H__
#define __CIFTI_BRAINORDINATE_LABEL_FILE_H__

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


#include "CiftiMappableDataFile.h"
#include "VoxelIJK.h"

namespace caret {

    class CiftiBrainordinateLabelFile :
    public CiftiMappableDataFile {
        
    public:
        CiftiBrainordinateLabelFile();
        
        virtual ~CiftiBrainordinateLabelFile();
        
        void getNodeIndicesWithLabelKey(const StructureEnum::Enum structure,
                                        const int32_t surfaceNumberOfNodes,
                                        const int32_t mapIndex,
                                        const int32_t labelKey,
                                        std::vector<int32_t>& nodeIndicesOut) const;
        
        void getVoxelIndicesWithLabelKey(const int32_t mapIndex,
                                         const int32_t labelKey,
                                         std::vector<VoxelIJK>& voxelIndicesOut) const;
        
        void getVoxelCoordinatesWithLabelKey(const int32_t mapIndex,
                                         const int32_t labelKey,
                                         std::vector<float>& voxelXyzOut) const;

    protected:
        CiftiBrainordinateLabelFile(const DataFileTypeEnum::Enum dataFileType);
        
    private:
        CiftiBrainordinateLabelFile(const CiftiBrainordinateLabelFile&);

        CiftiBrainordinateLabelFile& operator=(const CiftiBrainordinateLabelFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_BRAINORDINATE_LABEL_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_BRAINORDINATE_LABEL_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_BRAINORDINATE_LABEL_FILE_H__
