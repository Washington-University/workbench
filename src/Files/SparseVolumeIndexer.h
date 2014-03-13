#ifndef __SPARSE_VOLUME_INDEXER_H__
#define __SPARSE_VOLUME_INDEXER_H__

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

#include "CaretCompact3DLookup.h"
#include "CaretObject.h"
#include "CiftiXML.h"

namespace caret {
    class CiftiInterface;
    class VolumeSpace;
    
    class SparseVolumeIndexer : public CaretObject {
        
    public:
        /**
         * Location of voxel data.
         */
        enum ColumnOrRow {
            /** Use voxel data for columns */
            COLUMN,
            /** Use voxel data for rows */
            ROW
        };
        
        SparseVolumeIndexer(const CiftiInterface* ciftiInterface,
                            const std::vector<CiftiBrainModelsMap::VolumeMap>& ciftiVoxelMapping);
        
        virtual ~SparseVolumeIndexer();
        
        bool isValid() const;
        
        bool coordinateToIndices(const float x,
                                 const float y,
                                 const float z,
                                 int64_t& iOut,
                                 int64_t& jOut,
                                 int64_t& kOut) const;
        
        bool indicesToCoordinate(const int64_t i,
                                 const int64_t j,
                                 const int64_t k,
                                 float& xOut,
                                 float& yOut,
                                 float& zOut) const;
        
        int64_t getOffsetForIndices(const int64_t i,
                                    const int64_t j,
                                    const int64_t k) const;
        
        int64_t getOffsetForCoordinate(const float x,
                                       const float y,
                                       const float z) const;
        
    private:
        SparseVolumeIndexer(const SparseVolumeIndexer&);

        SparseVolumeIndexer& operator=(const SparseVolumeIndexer&);
        
    public:

        // ADD_NEW_METHODS_HERE


    private:
        // ADD_NEW_MEMBERS_HERE

        bool m_dataValid;
        
        int64_t m_dimI;
        
        int64_t m_dimJ;
        
        int64_t m_dimK;
        
        CaretCompact3DLookup<int64_t> m_voxelIndexLookup;
        
        const CiftiInterface* m_ciftiInterface;
        
        VolumeSpace* m_volumeSpace;
        
//        std::vector<int32_t> m_voxelOffsets;
        
    };
    
#ifdef __SPARSE_VOLUME_INDEXER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPARSE_VOLUME_INDEXER_DECLARE__

} // namespace
#endif  //__SPARSE_VOLUME_INDEXER_H__
