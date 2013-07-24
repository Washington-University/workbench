#ifndef __CIFTI_FIBER_TRAJECTORY_FILE__H_
#define __CIFTI_FIBER_TRAJECTORY_FILE__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#include "BrainConstants.h"
#include "CaretMappableDataFile.h"
#include "CaretSparseFile.h"
#include "DisplayGroupEnum.h"

namespace caret {

    class CiftiFiberOrientationFile;
    class FiberOrientationTrajectory;
    class FiberTrajectoryMapProperties;
    class GiftiMetaData;
    
    class CiftiFiberTrajectoryFile : public CaretMappableDataFile {
        
    public:
        CiftiFiberTrajectoryFile();
        
        virtual ~CiftiFiberTrajectoryFile();
        
        virtual void clear();
        
        bool isEmpty() const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps() const;
        
        virtual bool hasMapAttributes() const;
        
        virtual AString getMapName(const int32_t mapIndex) const;
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual bool isMappedWithPalette() const;
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex);
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues);
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex,
                                                const PaletteFile* paletteFile);
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        void loadDataForSurfaceNode(CiftiFiberOrientationFile* fiberOrientFile,
                                    const StructureEnum::Enum structure,
                                    const int32_t surfaceNumberOfNodes,
                                    const int32_t nodeIndex) throw (DataFileException);
        
        void loadDataAverageForSurfaceNodes(CiftiFiberOrientationFile* fiberOrientFile,
                                            const StructureEnum::Enum structure,
                                            const int32_t surfaceNumberOfNodes,
                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        const std::vector<FiberOrientationTrajectory*>& getLoadedFiberOrientationTrajectories() const;
        
        void clearLoadedFiberOrientations();
        
        FiberTrajectoryMapProperties* getFiberTrajectoryMapProperties();
        
        const FiberTrajectoryMapProperties* getFiberTrajectoryMapProperties() const;
        
        bool isDataLoadingEnabled() const;
        
        void setDataLoadingEnabled(const bool loadingEnabled);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        CiftiFiberTrajectoryFile(const CiftiFiberTrajectoryFile&);

        CiftiFiberTrajectoryFile& operator=(const CiftiFiberTrajectoryFile&);
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        

    private:

        void clearPrivate();
        
        CaretSparseFile* m_sparseFile;
        
        GiftiMetaData* m_metadata;
        
        std::vector<FiberOrientationTrajectory*> m_fiberOrientationTrajectories;

        FiberTrajectoryMapProperties* m_fiberTrajectoryMapProperties;
        
        bool m_dataLoadingEnabled;
        
        AString m_loadedDataDescriptionForMapName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_FIBER_TRAJECTORY_FILE__H_
