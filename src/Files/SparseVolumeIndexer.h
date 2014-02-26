#ifndef __SPARSE_VOLUME_INDEXER_H__
#define __SPARSE_VOLUME_INDEXER_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
