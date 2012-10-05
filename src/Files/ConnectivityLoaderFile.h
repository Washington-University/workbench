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
#include "TimeLine.h"
#include "CaretPointer.h"

namespace caret {

    class CiftiFile;
    class CiftiXnat;
    class CiftiInterface;
    class CiftiFiberOrientationAdapter;
    class CiftiScalarFile;
    class Palette;
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

        bool isDataLoadingEnabled() const;

        void setDataLoadingEnabled(const bool enabled);

        bool isYokeEnabled() const { return this->yokeEnabled; }

        void setYokeEnabled(const bool &enabled) { this->yokeEnabled = enabled; }
        
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
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual int32_t getMapIndexFromUniqueID(const AString& uniqueID) const;
        
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

        bool hasDataSeriesLabels();

        bool isAnimationEnabled() const;
        
        void setAnimationEnabled(const bool animationEnabled);
        
        AString getCiftiTypeName() const;
        
        int64_t loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                    const int32_t nodeIndex) throw (DataFileException);
        
        void loadAverageDataForSurfaceNodes(const StructureEnum::Enum structure,
                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException);

        void loadAverageTimeSeriesForSurfaceNodes(const StructureEnum::Enum structure,
                                    const std::vector<int32_t>& nodeIndices, const TimeLine &timeLine) throw (DataFileException);
        
        int64_t loadDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);
        
        void loadTimePointAtTime(const float seconds) throw (DataFileException);

        void loadFrame(const int frame) throw (DataFileException);
        
        int32_t getNumberOfDataElements() const;
        
        float* getData();
        
        float* getDataRGBA();
        
        void updateRGBAColoring(const Palette* palette,
                                const int32_t mapIndex);
        
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
        
        //float getSelectedTimePoint() const;

        int getSelectedFrame() const;

        int64_t loadTimeLineForSurfaceNode(const StructureEnum::Enum structure,
                          const int32_t nodeIndex, const TimeLine &timeLine) throw (DataFileException);

                         
        int64_t loadTimeLineForVoxelAtCoordinate(const float xyz[3],TimeLine &timeLine) throw (DataFileException);
        void getTimeLine(TimeLine &tl);
        
        int32_t getSurfaceNumberOfNodes(const StructureEnum::Enum structure) const;

        ///get the map name for an index along a column
        AString getMapNameForColumnIndex(const int& index) const;

        ///get the map name for an index along a row
        AString getMapNameForRowIndex(const int& index) const;
        
        CiftiFiberOrientationAdapter* getFiberOrientationAdapter();
        
    private:
        ConnectivityLoaderFile(const ConnectivityLoaderFile&);
        ConnectivityLoaderFile& operator=(const ConnectivityLoaderFile&);
        
        enum LoaderType {
            LOADER_TYPE_INVALID,
            LOADER_TYPE_DENSE,
            LOADER_TYPE_DENSE_LABELS,
            LOADER_TYPE_DENSE_SCALARS,
            LOADER_TYPE_DENSE_TIME_SERIES,
            LOADER_TYPE_FIBER_ORIENTATIONS
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
        
        void createFiberOrientationAdapter() throw (DataFileException);
        
        LoaderType loaderType;
        
        MapToType mapToType;
        
        CiftiXnat* ciftiXnatFile;
        
        CiftiFile* ciftiDiskFile;
        
        CiftiInterface* ciftiInterface;
        
        DescriptiveStatistics* descriptiveStatistics;
        
        CaretPointer<FastStatistics> m_fastStatistics;
        
        CaretPointer<Histogram> m_histogram;
        
        PaletteColorMapping* paletteColorMapping;
        
        GiftiLabelTable* labelTable;
        
        GiftiMetaData* metadata;
        
        float* data;
        float* dataRGBA;
        
        int32_t numberOfDataElements;
        
        VolumeFile* connectivityVolumeFile;
        
        bool timeSeriesGraphEnabled;
        
        bool dataLoadingEnabled;

        bool yokeEnabled;
        
        //float selectedTimePoint;

        int selectedFrame;

        TimeLine tl;

        bool animationEnabled;
                
        AString uniqueID; // DO NOT COPY
        
        CiftiFiberOrientationAdapter* m_fiberOrientationAdapter;
        
        friend class CiftiFiberOrientationAdapter;
        friend class CiftiFiberTrajectoryFile;
        friend class CiftiScalarFile;
    };
    
} // namespace

#endif // __CONNECTIVITY_LOADER_FILE_H__

