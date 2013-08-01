#ifndef __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_H__
#define __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_H__

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


#include "CiftiMappableDataFile.h"
#include "VoxelIJK.h"

namespace caret {

    class ConnectivityDataLoaded;
    class SceneClassAssistant;
    
    class CiftiMappableConnectivityMatrixDataFile : public CiftiMappableDataFile {
        
    protected:
        CiftiMappableConnectivityMatrixDataFile(const DataFileTypeEnum::Enum dataFileType,
                                                const FileReading fileReading,
                                                const IndicesMapToDataType rowIndexType,
                                                const IndicesMapToDataType columnIndexType,
                                                const DataAccess brainordinateMappedDataAccess,
                                                const DataAccess seriesDataAccess);
        
    public:
        virtual ~CiftiMappableConnectivityMatrixDataFile();
        
        bool isMapDataLoadingEnabled(const int32_t mapIndex) const;
        
        void setMapDataLoadingEnabled(const int32_t mapIndex,
                                      const bool enabled);
        
        virtual int64_t loadMapDataForSurfaceNode(const int32_t mapIndex,
                                                  const int32_t surfaceNumberOfNodes,
                                                  const StructureEnum::Enum structure,
                                                  const int32_t nodeIndex) throw (DataFileException);
        
        virtual void loadMapAverageDataForSurfaceNodes(const int32_t mapIndex,
                                                       const int32_t surfaceNumberOfNodes,
                                                       const StructureEnum::Enum structure,
                                                       const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        virtual int64_t loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                        const float xyz[3]) throw (DataFileException);

        virtual bool loadMapAverageDataForVoxelIndices(const int32_t mapIndex,
                                                       const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices) throw (DataFileException);

        void loadDataForRowIndex(const int64_t rowIndex) throw (DataFileException);
                
//        bool getStructureAndNodeIndexFromRowIndex(const int64_t rowIndex,
//                                                  StructureEnum::Enum& structureOut,
//                                                  int64_t& nodeIndexOut) const;
//
//        bool getVoxelIndexAndCoordinateFromRowIndex(const int64_t rowIndex,
//                                                    int64_t ijkOut[3],
//                                                    float xyzOut[3]) const;
        
        virtual void clear();
        
        virtual bool isEmpty() const;

		virtual AString getMapName(const int32_t mapIndex) const;

        AString getRowName(const int32_t rowIndex) const;
        
        AString getColumnName(const int32_t rowIndex) const;

        AString getRowLoadedText() const;

		int64_t getRowLoadedIndex() const;

        virtual void getMapData(const int32_t mapIndex, std::vector<float>& dataOut) const;

        bool loadMapData(const int32_t rowIndex) throw (DataFileException);
        
    private:
        CiftiMappableConnectivityMatrixDataFile(const CiftiMappableConnectivityMatrixDataFile&);

        CiftiMappableConnectivityMatrixDataFile& operator=(const CiftiMappableConnectivityMatrixDataFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    private:
        void clearPrivate();
        
        void setLoadedRowDataToAllZeros();
        
        int64_t getRowIndexForNodeWhenLoading(const StructureEnum::Enum structure,
                                              const int64_t surfaceNumberOfNodes,
                                              const int64_t nodeIndex);
        
        int64_t getRowIndexForVoxelAtCoordinateWhenLoading(const float xyz[3]);
        
        int64_t getRowIndexForVoxelIndexWhenLoading(const int64_t ijk[3]);
        
        
        // ADD_NEW_MEMBERS_HERE
        
        SceneClassAssistant* m_sceneAssistant;
        
        bool m_dataLoadingEnabled;
        
        std::vector<float> m_loadedRowData;
        
        AString m_rowLoadedTextForMapName;

        AString m_rowLoadedText;
        
        ConnectivityDataLoaded* m_connectivityDataLoaded;
        
        friend class CiftiBrainordinateScalarFile;

    };
    
#ifdef __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_H__
