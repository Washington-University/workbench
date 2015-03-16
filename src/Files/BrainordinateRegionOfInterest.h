#ifndef __BRAINORDINATE_REGION_OF_INTEREST_H__
#define __BRAINORDINATE_REGION_OF_INTEREST_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {
    class CaretMappableDataFile;
    class CiftiMappableDataFile;
    class CiftiParcelsMap;
    class SceneClassAssistant;

    class BrainordinateRegionOfInterest : public CaretObject, public SceneableInterface {
        
    public:
        BrainordinateRegionOfInterest();
        
        virtual ~BrainordinateRegionOfInterest();
        
        BrainordinateRegionOfInterest(const BrainordinateRegionOfInterest& obj);

        BrainordinateRegionOfInterest& operator=(const BrainordinateRegionOfInterest& obj);

        void clear();
        
        bool hasSurfaceNodes() const;
        
        bool hasNodesForSurfaceStructure(const StructureEnum::Enum structure,
                                         const int64_t surfaceNumberOfNodes) const;
        
        const std::vector<int64_t>& getNodesForSurfaceStructure(const StructureEnum::Enum structure,
                                                                const int64_t surfaceNumberOfNodes) const;
        
        bool hasVolumeVoxels() const;
        
        void  getVolumeVoxelSize(float voxelSizeOut[3]) const;
        
        const std::vector<float>& getVolumeVoxelsXYZ() const;
        
        void setSurfaceNodes(const StructureEnum::Enum structure,
                             const int64_t surfaceNumberOfNodes,
                             const std::vector<int64_t>& surfaceNodeIndices);
        
        void setVolumeVoxels(const float voxelSize[3],
                             const std::vector<float>& voxelsXYZ);
        
        bool isBrainordinateHighlightingEnabled() const;
        
        void setBrainordinateHighlightingEnabled(const bool highlighting);

        bool setWithLabelFileLabel(const CaretMappableDataFile* caretMappableDataFile,
                                   const int32_t mapIndex,
                                   const AString& labelName,
                                   AString& errorMessageOut);
        
        bool setWithCiftiParcelLoadingBrainordinates(const CiftiMappableDataFile* ciftiMappableDataFile,
                                                     const int32_t mapIndex,
                                                     const AString& parcelName,
                                                     AString& errorMessageOut);
        
        bool setWithCiftiParcelMappingBrainordinates(const CiftiMappableDataFile* ciftiMappableDataFile,
                                                     const int32_t mapIndex,
                                                     const AString& parcelName,
                                                     AString& errorMessageOut);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperBrainordinateRegionOfInterest(const BrainordinateRegionOfInterest& obj);

        bool setWithCiftiParcelBrainordinates(const CiftiMappableDataFile* ciftiMappableDataFile,
                                              const CiftiParcelsMap* ciftiParcelsMap,
                                              const AString& parcelName,
                                              AString& errorMessageOut);
        
        SceneClassAssistant* m_sceneAssistant;

        class SurfaceNodesInfo {
        public:
            SurfaceNodesInfo(const StructureEnum::Enum structure,
                             const int64_t surfaceNumberOfNodes,
                             const std::vector<int64_t>& surfaceNodeIndices)
            : m_structure(structure),
            m_surfaceNumberOfNodes(surfaceNumberOfNodes),
            m_surfaceNodeIndices(surfaceNodeIndices)
            {
                /* nothing */
            }
            
            ~SurfaceNodesInfo() { }
            
            const StructureEnum::Enum m_structure;
            
            const int64_t m_surfaceNumberOfNodes;
            
            const std::vector<int64_t> m_surfaceNodeIndices;
        };
        
        std::vector<SurfaceNodesInfo*> m_surfaceNodesInfo;
        
        float m_voxelSize[3];
        
        std::vector<float> m_voxelXYZ;
        
        static std::vector<int64_t> s_emptySurfaceNodes;
        
        bool m_highlighingEnabled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAINORDINATE_REGION_OF_INTEREST_DECLARE__
    std::vector<int64_t> BrainordinateRegionOfInterest::s_emptySurfaceNodes;
#endif // __BRAINORDINATE_REGION_OF_INTEREST_DECLARE__

} // namespace
#endif  //__BRAINORDINATE_REGION_OF_INTEREST_H__
