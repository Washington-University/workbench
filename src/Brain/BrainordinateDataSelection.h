#ifndef __BRAINORDINATE_DATA_SELECTION_H__
#define __BRAINORDINATE_DATA_SELECTION_H__

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


#include "CaretObject.h"
#include "SceneableInterface.h"
#include "VoxelIJK.h"

namespace caret {

    class SurfaceFile;
    
    class BrainordinateDataSelection
    : public CaretObject,
    public SceneableInterface {
    
    public:
        BrainordinateDataSelection();
        
        virtual ~BrainordinateDataSelection();
        
    private:
        BrainordinateDataSelection(const BrainordinateDataSelection&);

        BrainordinateDataSelection& operator=(const BrainordinateDataSelection&);
        
    public:
        enum Mode {
            MODE_NONE,
            MODE_SURFACE_AVERAGE,
            MODE_SURFACE_NODE,
            MODE_VOXEL_AVERAGE,
            MODE_VOXEL_XYZ
        };
        
        void reset();
        
        Mode getMode() const;
        
        AString getSurfaceFileName() const;
        
        std::vector<int32_t> getSurfaceNodeIndices() const;
        
        void getVoxelXYZ(float xyzOut[3]) const;
        
        void setSurfaceLoading(const SurfaceFile* surfaceFile,
                               const int32_t nodeInde);
        
        void setSurfaceAverageLoading(const SurfaceFile* surfaceFile,
                                      const std::vector<int32_t>& nodeIndices);
        
        void setVolumeLoading(const float xyz[3]);
        
        void setVolumeAverageLoading(const int64_t volumeDimensionIJK[3],
                                     const std::vector<VoxelIJK>& voxelIndices);
        
        void getVolumeAverageVoxelIndices(int64_t volumeDimensionIJK[3],
                                          std::vector<VoxelIJK>& voxelIndices) const;
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName);
        
    private:
        // ADD_NEW_MEMBERS_HERE

        Mode m_mode;
        
        AString m_surfaceFileName;
        
        std::vector<int32_t> m_surfaceFileNodeIndices;
        
        float m_voxelXYZ[3];
        
        int32_t m_volumeAverageDimensionsIJK[3];
        
        std::vector<VoxelIJK> m_volumeAverageVoxelIndices;
    };
    
#ifdef __BRAINORDINATE_DATA_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAINORDINATE_DATA_SELECTION_DECLARE__

} // namespace
#endif  //__BRAINORDINATE_DATA_SELECTION_H__
