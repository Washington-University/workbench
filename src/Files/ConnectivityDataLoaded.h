#ifndef __CONNECTIVITY_DATA_LOADED_H__
#define __CONNECTIVITY_DATA_LOADED_H__

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
#include "StructureEnum.h"
#include "VoxelIJK.h"

namespace caret {

    class SceneClassAssistant;
    class SurfaceFile;
    
    class ConnectivityDataLoaded : public CaretObject, public SceneableInterface {
    
    public:
        ConnectivityDataLoaded();
        
        virtual ~ConnectivityDataLoaded();
        
    private:
        ConnectivityDataLoaded(const ConnectivityDataLoaded&);

        ConnectivityDataLoaded& operator=(const ConnectivityDataLoaded&);
        
    public:
        enum Mode {
            MODE_NONE,
            MODE_SURFACE_NODE,
            MODE_SURFACE_NODE_AVERAGE,
            MODE_VOXEL,
            MODE_VOXEL_AVERAGE
        };
        
        void reset();
        
        Mode getMode() const;
        
        void getSurfaceNodeLoading(StructureEnum::Enum& structure,
                                   int32_t& surfaceNumberOfNodes,
                                   int32_t& surfaceNodeIndex) const;
        
        void setSurfaceNodeLoading(const StructureEnum::Enum structure,
                                   const int32_t surfaceNumberOfNodes,
                                   const int32_t surfaceNodeIndex);
        
        void getSurfaceAverageNodeLoading(StructureEnum::Enum& structure,
                                   int32_t& surfaceNumberOfNode,
                                   std::vector<int32_t>& surfaceNodeIndices) const;
        
        void setSurfaceAverageNodeLoading(const StructureEnum::Enum structure,
                                          const int32_t surfaceNumberOfNodes,
                                          const std::vector<int32_t>& surfaceNodeIndices);
        
        void getVolumeVoxelLoading(VoxelIJK& voxelIndexIJK) const;
        
        void setVolumeVoxelLoading(const VoxelIJK& voxelIndexIJK);
        
        void getVolumeAverageVoxelLoading(std::vector<VoxelIJK>& voxelIndicesIJK) const;
        
        void setVolumeAverageVoxelLoading(const std::vector<VoxelIJK>& voxelIndicesIJK);
                
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName);
        
    private:
        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        Mode m_mode;
        
        StructureEnum::Enum m_surfaceStructure;
        
        int32_t m_surfaceNumberOfNodes;
        
        std::vector<int32_t> m_surfaceNodeIndices;
        
        std::vector<VoxelIJK> m_voxelIndices;
    };
    
#ifdef __CONNECTIVITY_DATA_LOADED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_DATA_LOADED_DECLARE__

} // namespace
#endif  // __CONNECTIVITY_DATA_LOADED_H__

