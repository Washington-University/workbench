#ifndef __CIFTI_LABEL_FILE__H_
#define __CIFTI_LABEL_FILE__H_

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


#include "CaretMappableDataFile.h"
#include "EventListenerInterface.h"

namespace caret {

    class CiftiFile;
    class CiftiInterface;
    class DescriptiveStatistics;
    class FastStatistics;
    class Histogram;
    class Palette;
    class VolumeFile;
    
    class CiftiLabelFile : public CaretMappableDataFile, public EventListenerInterface {
        
    public:
        CiftiLabelFile();
        
        virtual ~CiftiLabelFile();
        
        void receiveEvent(Event* event);
        
        virtual bool isEmpty() const;
        
        virtual void clear();
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps()const ;
        
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
        
        bool getSurfaceNodeColoring(const StructureEnum::Enum structure,
                                    const int32_t mapIndex,
                                    const Palette* palette,
                                    float* nodeRGBA,
                                    const int32_t numberOfNodes);
        
        bool getVolumeVoxelValue(const int32_t mapIndex,
                                 const float xyz[3],
                                 int64_t ijkOut[3],
                                 float &valueOut) const;
        
        bool getSurfaceNodeValue(const StructureEnum::Enum structure,
                                 const int32_t mapIndex,
                                 const int32_t nodeIndex,
                                 const int32_t numberOfNodes,
                                 float& valueOut) const;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex,
                                             const PaletteFile* paletteFile);
    private:
        CiftiLabelFile(const CiftiLabelFile&);

        CiftiLabelFile& operator=(const CiftiLabelFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        /**
         * Data for each map.
         */
        class MapData : public CaretObject {
        public:
            MapData(const int32_t numberOfElements);
            
            ~MapData();
            
            VolumeFile* getVolumeFile();
            
            void createVolumeFile(CiftiInterface* ciftiInterface,
                                  const int32_t numberOfDataElements);
            
            void initializeMetaData(const AString& fileName,
                                   const int32_t numberOfDataElements);
            
            DescriptiveStatistics* m_descriptiveStatistics;
            
            FastStatistics* m_fastStatistics;
            
            Histogram* m_histogram;
            
            //PaletteColorMapping* m_paletteColorMapping;
            
            mutable GiftiMetaData* m_metadata;
            
            float* m_data;
            
            float* m_dataRGBA;
            
            float m_isColoringValid;
            
        private:
            VolumeFile* m_volumeFile;
            
        };
        
        void clearPrivate();
        
        CiftiFile* m_ciftiDiskFile;
        
        CiftiInterface* m_ciftiInterface;
        
        GiftiMetaData* m_metadata;
        
        int32_t m_numberOfDataElements;

        bool m_isSurfaceMappable;
        
        bool m_isVolumeMappable;
        
        AString m_uniqueID;
        
        bool m_isColoringValid;
        
        std::vector<MapData*> m_mapData;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_SCALAR_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_SCALAR_FILE_DECLARE__

} // namespace

#endif  //__CIFTI_LABEL_FILE__H_
