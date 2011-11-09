#ifndef __CONNECTIVITY_LOADER_FILE_H__
#define __CONNECTIVITY_LOADER_FILE_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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


#include <AString.h>

#include "CaretMappableDataFile.h"
#include "StructureEnum.h"

namespace caret {

    class CiftiFile;
    class CiftiXnat;
    class CiftiInterface;
    class SurfaceFile;
    class VolumeFile;
    
    /// Loads rows/columns on demand from a CIFTI file
    class ConnectivityLoaderFile : public CaretMappableDataFile {
        
    public:
        
        ConnectivityLoaderFile();
        
        virtual ~ConnectivityLoaderFile();
        
    public:
        virtual void clear();
        
        virtual bool isEmpty() const;

        void setupLocalFile(const AString& filename,
                            const DataFileTypeEnum::Enum connectivityFileType) throw (DataFileException);
        
        void setupNetworkFile(const AString& url,
                              const DataFileTypeEnum::Enum connectivityFileType,
                              const AString& username,
                              const AString& password) throw (DataFileException);
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual AString toString() const;
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
                
        PaletteColorMapping* getPaletteColorMapping(const int32_t columnIndex);
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps() const;
        
        virtual AString getMapName(const int32_t mapIndex) const;
        
        virtual int32_t getMapIndexFromName(const AString& mapName);
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        virtual bool isMappedWithPalette() const;
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;        
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        bool isDense() const;
        
        bool isDenseTimeSeries() const;
        
        AString getCiftiTypeName() const;
        
        void loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                    const int32_t nodeIndex) throw (DataFileException);
        
        void loadDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);
        
        void loadTimePointAtTime(const float seconds) throw (DataFileException);
        
        int32_t getNumberOfDataElements() const;
        
        float* getData();
        
        float* getDataRGBA();
        
        bool getVolumeVoxelValue(const float xyz[3],
                                 int64_t ijkOut[3],
                                 float &valueOut) const;
        
        bool getSurfaceNodeValue(const StructureEnum::Enum structure,
                                 const int nodeIndex,
                                 const int32_t numberOfNodes,
                                 float& valueOut) const;
        
        bool getSurfaceNodeColoring(const StructureEnum::Enum structure,
                                    float* nodeRGBA,
                                    const int32_t numberOfNodes);
        

        VolumeFile* getConnectivityVolumeFile();
        int64_t getNumberOfTimePoints();

        float getTimeStep() const;
        
        bool isTimeSeriesGraphEnabled() const;
        
        void setTimeSeriesGraphEnabled(const bool showGraph);
        
    private:
        enum LoaderType {
            LOADER_TYPE_INVALID,
            LOADER_TYPE_DENSE,
            LOADER_TYPE_DENSE_TIME_SERIES
        };
        
        enum MapToType {
            MAP_TO_TYPE_INVALID,
            MAP_TO_TYPE_BRAINORDINATES,
            MAP_TO_TYPE_TIMEPOINTS
        };
        
        void setup(const AString& url,
                   const DataFileTypeEnum::Enum connectivityFileType,
                   const AString& username,
                   const AString& password) throw (DataFileException);
        
        void clearData();
        
        void reset();
        
        void allocateData(const int32_t numberOfDataElements);
        
        void zeroizeData();
        
        LoaderType loaderType;
        
        MapToType mapToType;
        
        CiftiXnat* ciftiXnatFile;
        
        CiftiFile* ciftiDiskFile;
        
        CiftiInterface* ciftiInterface;
        
        DescriptiveStatistics* descriptiveStatistics;
        
        PaletteColorMapping* paletteColorMapping;
        
        GiftiLabelTable* labelTable;
        
        GiftiMetaData* metadata;
        
        float* data;
        float* dataRGBA;
        int32_t numberOfDataElements;
        
        VolumeFile* connectivityVolumeFile;
        
        bool timeSeriesGraphEnabled;
    };
    
} // namespace

#endif // __CONNECTIVITY_LOADER_FILE_H__

