
#ifndef __VOLUME_FILE_H__
#define __VOLUME_FILE_H__

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

#include <map>
#include <memory>

#include "BrainConstants.h"
#include "VolumeBase.h"
#include "CaretMappableDataFile.h"
#include "CaretMutex.h"
#include "CaretVolumeExtension.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "GroupAndNameHierarchyUserInterface.h"
#include "StructureEnum.h"
#include "GiftiMetaData.h"
#include "BoundingBox.h"
#include "VolumeFileVoxelColorizer.h"
#include "VoxelIJK.h"
#include "VoxelInterpolationTypeEnum.h"

namespace caret {
    
    class GroupAndNameHierarchyModel;
    class VolumeDynamicConnectivityFile;
    class VolumeFileEditorDelegate;
    class VolumeFileVoxelColorizer;
    class VolumeGraphicsPrimitiveManager;
    class VolumeSpline;
    
    class VolumeFile : public VolumeBase, public CaretMappableDataFile, public ChartableLineSeriesBrainordinateInterface, public GroupAndNameHierarchyUserInterface
    {
        VolumeFile(const VolumeFile&);
        
        VolumeFile& operator=(const VolumeFile&);
        
        CaretVolumeExtension m_caretVolExt;
        
        void parseExtensions();//called after reading a file, in order to populate m_caretVolExt with best guesses
        
        void validateMembers();//called to ensure extension agrees with number of subvolumes
        
        void updateCaretExtension();//called before writing a file, erases all existing caret extensions from m_extensions, and rebuilds one from m_caretVolExt
        
        void checkStatisticsValid();
        
        struct BrickAttributes//for storing ONLY stuff that doesn't get saved to the caret extension
        {//TODO: prune this once statistics gets straightened out
            CaretPointer<FastStatistics> m_fastStatistics;
            CaretPointer<Histogram> m_histogram;
            int32_t m_histogramNumberOfBuckets = 100;
            CaretPointer<Histogram> m_histogramLimitedValues;
            int32_t m_histogramLimitedValuesNumberOfBuckets = 100;
            float m_histogramLimitedValuesMostPositiveValueInclusive;
            float m_histogramLimitedValuesLeastPositiveValueInclusive;
            float m_histogramLimitedValuesLeastNegativeValueInclusive;
            float m_histogramLimitedValuesMostNegativeValueInclusive;
            bool m_histogramLimitedValuesIncludeZeroValues;
        };
        
        mutable std::vector<BrickAttributes> m_brickAttributes;//because statistics and metadata construct lazily
        
        bool m_brickStatisticsValid;//so that setModified() doesn't do something slow
        
        /** Fast statistics used when statistics computed on all data in file */
        CaretPointer<FastStatistics> m_fileFastStatistics;
        
        /** Histogram used when statistics computed on all data in file */
        CaretPointer<Histogram> m_fileHistogram;
        
        int32_t m_fileHistogramNumberOfBuckets = 100;
        
        /** Histogram with limited values used when statistics computed on all data in file */
        CaretPointer<Histogram> m_fileHistorgramLimitedValues;
        
        int32_t m_fileHistogramLimitedValuesNumberOfBuckets;
        float m_fileHistogramLimitedValuesMostPositiveValueInclusive;
        float m_fileHistogramLimitedValuesLeastPositiveValueInclusive;
        float m_fileHistogramLimitedValuesLeastNegativeValueInclusive;
        float m_fileHistogramLimitedValuesMostNegativeValueInclusive;
        bool m_fileHistogramLimitedValuesIncludeZeroValues;
        
        /** Performs coloring of voxels.  Will be NULL if coloring is disabled. */
        CaretPointer<VolumeFileVoxelColorizer> m_voxelColorizer;
        
        std::unique_ptr<VolumeDynamicConnectivityFile> m_lazyInitializedDynamicConnectivityFile;
        
        std::unique_ptr<VolumeGraphicsPrimitiveManager> m_graphicsPrimitiveManager;
        
        /** True if the volume is a single slice, needed by interpolateValue() methods */
        bool m_singleSliceFlag;
        
        mutable CaretMutex m_splineMutex;
        
        mutable bool m_splinesValid;
        
        mutable std::vector<bool> m_frameSplineValid;
        
        mutable std::vector<VolumeSpline> m_frameSplines;
        
        bool m_chartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        mutable bool m_dataRangeValid;
        
        mutable float m_dataRangeMinimum;
        
        mutable float m_dataRangeMaximum;
        
        /** Have bounding box for each map */
        mutable std::vector<std::unique_ptr<BoundingBox>> m_nonZeroVoxelCoordinateBoundingBoxes;
        
        /** Holds class and name hierarchy used for display selection */
        mutable CaretPointer<GroupAndNameHierarchyModel> m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        mutable bool m_forceUpdateOfGroupAndNameHierarchy;
        
        CaretPointer<VolumeFileEditorDelegate> m_volumeFileEditorDelegate;
        
        static const AString s_paletteColorMappingNameInMetaData;

        int16_t m_writingDType;

        bool m_writingDoScale;

        double m_minScalingVal, m_maxScalingVal;
        
        mutable std::map<int32_t, std::unique_ptr<ClusterContainer>> m_mapLabelClusterContainers;
        
    protected:
        VolumeFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    public:
        
        enum InterpType
        {
            ENCLOSING_VOXEL,
            TRILINEAR,
            CUBIC
        };
        
        const static float INVALID_INTERP_VALUE;
        
        /** Enables coloring.  Coloring is almost always not needed for command line operations */
        static bool s_voxelColoringEnabled;
        
        static void setVoxelColoringEnabled(const bool enabled);
        
        VolumeFile();
        VolumeFile(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1,
                   SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY, const AbstractHeader* templateHeader = NULL);
        ~VolumeFile();
        
        virtual void clear();
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        ///recreates the volume file storage with new size and spacing
        void reinitialize(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1,
                          SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY, const AbstractHeader* templateHeader = NULL);
        
        ///convenient version for 3D or 4D from a VolumeSpace
        void reinitialize(const VolumeSpace& volSpaceIn, const int64_t numFrames = 1, const int64_t numComponents = 1,
                          SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY, const AbstractHeader* templateHeader = NULL);
        
        ///another convenience version for taking spatial dims, spacing, type and header from existing file
        void reinitialize(const VolumeFile* headerTemplate, const int64_t numFrames = 1, const int64_t numComponents = 1);
        
        void addSubvolumes(const int64_t& numToAdd);
        
        void setType(SubvolumeAttributes::VolumeType whatType);
        
        SubvolumeAttributes::VolumeType getType() const;
        
        void validateSpline(const int64_t brickIndex = 0, const int64_t component = 0) const;

        void freeSpline(const int64_t brickIndex = 0, const int64_t component = 0) const;

        float interpolateValue(const float* coordIn, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0, const float backgroundVal = INVALID_INTERP_VALUE) const;

        float interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0, const float backgroundVal = INVALID_INTERP_VALUE) const;

        float interpolateValue(const float* coordIn, const VoxelInterpolationTypeEnum::Enum interpType = VoxelInterpolationTypeEnum::TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0, const float backgroundVal = INVALID_INTERP_VALUE) const;
        
        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const VolumeFile* right) const;
        
        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const VolumeSpace& otherSpace) const;
        
        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const int64_t dims[3], const std::vector<std::vector<float> >& sform) const;
        
        virtual void readFile(const AString& filename);

        virtual void writeFile(const AString& filename);

        ///data type and scaling options
        void setWritingDataTypeNoScaling(const int16_t& type = NIFTI_TYPE_FLOAT32);
        void setWritingDataTypeAndScaling(const int16_t& type, const double& minval, const double& maxval);

        bool isEmpty() const { return VolumeBase::isEmpty(); }
        
        bool hasGoodSpatialInformation() const;
        
        virtual void setModified();
        
        virtual void clearModified();
        
        virtual bool isModifiedExcludingPaletteColorMapping() const;
        
        void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const override;
        
        static void dimensionOriginSpacingXyzToVoxelEdges(const Vector3D& dimensions,
                                                          const Vector3D& origin,
                                                          const Vector3D& spacing,
                                                          Vector3D& firstVoxelEdgeOut,
                                                          Vector3D& lastVoxelEdgeOut);
        
        virtual void getNonZeroVoxelCoordinateBoundingBox(const int32_t mapIndex,
                                                   BoundingBox& boundingBoxOut) const override;
        
        /**
         * @return The structure for this file.
         */
        StructureEnum::Enum getStructure() const;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        void setStructure(const StructureEnum::Enum structure);
        
        /**
         * @return Get access to the file's metadata.
         */
        GiftiMetaData* getFileMetaData();
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        const GiftiMetaData* getFileMetaData() const;
        
        bool isSurfaceMappable() const { return false; }
        
        bool isVolumeMappable() const { return true; }
        
        int32_t getNumberOfMaps() const { return getDimensionsPtr()[3]; }
        
        AString getMapName(const int32_t mapIndex) const;
        
        void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        const Histogram* getMapHistogram(const int32_t mapIndex);
        
        const Histogram* getMapHistogram(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues);
        
        
        virtual int64_t getDataSizeUncompressedInBytes() const;
        
        virtual const FastStatistics* getFileFastStatistics();
        
        virtual const Histogram* getFileHistogram();
        
        virtual const Histogram* getFileHistogram(const float mostPositiveValueInclusive,
                                                  const float leastPositiveValueInclusive,
                                                  const float leastNegativeValueInclusive,
                                                  const float mostNegativeValueInclusive,
                                                  const bool includeZeroValues);
        
        void getFileData(std::vector<float>& dataOut) const;
        
        bool isMappedWithPalette() const;
        
        virtual void getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const;
        
        PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        bool isMappedWithLabelTable() const;
        
        GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;

        virtual const ClusterContainer* getMapLabelTableClusters(const int32_t mapIndex) const override;

        void getVoxelIndicesWithLabelKey(const int32_t mapIndex,
                                         const int32_t labelKey,
                                         std::vector<VoxelIJK>& voxelIndicesOut) const;
        
        std::vector<int32_t> getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const;
        
        virtual bool isMappedWithRGBA() const;
        
        AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual CaretMappableDataFile* castToVolumeMappableDataFile() override;
        
        virtual const CaretMappableDataFile* castToVolumeMappableDataFile() const override;

        void updateScalarColoringForMap(const int32_t mapIndex) override;
        
        virtual int64_t getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                            const int64_t firstVoxelIJK[3],
                                            const int64_t rowStepIJK[3],
                                            const int64_t columnStepIJK[3],
                                            const int64_t numberOfRows,
                                            const int64_t numberOfColumns,
                                                    const TabDrawingInfo& tabDrawingInfo,
                                            uint8_t* rgbaOut) const override;
        
        virtual int64_t getVoxelColorsForSliceInMap(
                                         const int32_t mapIndex,
                                         const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                         const int64_t sliceIndex,
                                                    const TabDrawingInfo& tabDrawingInfo,
                                         uint8_t* rgbaOut) const override;

        virtual int64_t getVoxelColorsForSubSliceInMap(
                                                    const int32_t mapIndex,
                                                    const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                    const int64_t sliceIndex,
                                                    const int64_t firstCornerVoxelIndex[3],
                                                    const int64_t lastCornerVoxelIndex[3],
                                                    const int64_t voxelCountIJK[3],
                                                       const TabDrawingInfo& tabDrawingInfo,
                                                    uint8_t* rgbaOut) const override;
        
        virtual GraphicsPrimitiveV3fT3f* getVolumeDrawingTriangleStripPrimitive(const int32_t mapIndex,
                                                                                const TabDrawingInfo& tabDrawingInfo) const override;
        
        virtual GraphicsPrimitiveV3fT3f* getVolumeDrawingTriangleFanPrimitive(const int32_t mapIndex,
                                                                              const TabDrawingInfo& tabDrawingInfo) const override;
        
        virtual GraphicsPrimitiveV3fT3f* getVolumeDrawingTrianglesPrimitive(const int32_t mapIndex,
                                                                            const TabDrawingInfo& tabDrawingInfo) const override;

        virtual GraphicsPrimitive* getHistologyImageIntersectionPrimitive(const int32_t mapIndex,
                                                                          const TabDrawingInfo& tabDrawingInfo,
                                                                          const MediaFile* mediaFile,
                                                                          const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                          const float volumeSliceThickness,
                                                                          AString& errorMessageOut) const override;
        
        virtual std::vector<GraphicsPrimitive*> getHistologySliceIntersectionPrimitive(const int32_t mapIndex,
                                                                                       const TabDrawingInfo& tabDrawingInfo,
                                                                                       const HistologySlice* histologySlice,
                                                                                       const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                                       const float volumeSliceThickness,
                                                                                       AString& errorMessageOut) const override;

        void getVoxelValuesForSliceInMap(const int32_t mapIndex,
                                         const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                         const int64_t sliceIndex,
                                         float* sliceValues) const;
        
        virtual void getVoxelColorInMap(const int64_t i,
                                const int64_t j,
                                const int64_t k,
                                const int64_t mapIndex,
                                uint8_t rgbaOut[4]) const override;
        
        virtual void getVoxelColorInMap(const int64_t i,
                                const int64_t j,
                                const int64_t k,
                                const int64_t mapIndex,
                                        const TabDrawingInfo& tabDrawingInfo,
                                uint8_t rgbaOut[4]) const override;
        
        void clearVoxelColoringForMap(const int64_t mapIndex);
        
        virtual bool getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                             float& dataRangeMaximumOut) const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        const GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel() const;
        
        VolumeFileEditorDelegate* getVolumeFileEditorDelegate();
        
        virtual bool isLineSeriesChartingEnabled(const int32_t tabIndex) const;
        
        virtual void setLineSeriesChartingEnabled(const int32_t tabIndex,
                                        const bool enabled);
        
        virtual bool isLineSeriesChartingSupported() const;
        
        virtual ChartDataCartesian* loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                                const int32_t nodeIndex);
        
        virtual ChartDataCartesian* loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                                        const std::vector<int32_t>& nodeIndices);
        
        virtual ChartDataCartesian* loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3]);
        
        
        virtual void getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        virtual void getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                        std::vector<float>& dataOut) const override;
        
        virtual BrainordinateMappingMatch getBrainordinateMappingMatch(const CaretMappableDataFile* mapFile) const override;
       
        virtual bool getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                         const float xyz[3],
                                                         const AString& dataValueSeparator,
                                                         const int32_t digitsRightOfDecimal,
                                                         int64_t ijkOut[3],
                                                         AString& textOut) const;
        
        virtual NiftiTimeUnitsEnum::Enum getMapIntervalUnits() const override;
        
        virtual void getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                                float& mapIntervalStepValueOut) const override;
        
        VolumeDynamicConnectivityFile* getVolumeDynamicConnectivityFile();
        
        const VolumeDynamicConnectivityFile* getVolumeDynamicConnectivityFile() const;
        
        virtual bool isModifiedPaletteColorMapping() const override;
        
        virtual PaletteModifiedStatusEnum::Enum getPaletteColorMappingModifiedStatus() const override;
        
        virtual void groupAndNameHierarchyItemStatusChanged() override;

        void setValuesForVoxelEditing(const int32_t mapIndex,
                                      const std::vector<VoxelIJK>& voxelsIJK,
                                      const float value);
        
        void getNeigbors26(const VoxelIJK& voxelIJK,
                           const float* voxelValues,
                           const float minimumValue,
                           const float maximumValue,
                           std::vector<char>& voxelHasBeenSearchedFlags,
                           std::vector<VoxelIJK>& neighborIJKs) const;
        

    };

}

#endif //__VOLUME_FILE_H__
