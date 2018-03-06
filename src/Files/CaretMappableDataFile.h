#ifndef __CARET_MAPPABLE_DATA_FILE__H_
#define __CARET_MAPPABLE_DATA_FILE__H_

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

#include <memory>

#include "CaretDataFile.h"
#include "ChartOneDataTypeEnum.h"
#include "CaretPointer.h"
#include "CiftiXML.h"
#include "NiftiEnums.h"
#include "PaletteNormalizationModeEnum.h"

namespace caret {

    class ChartDataCartesian;
    class ChartableTwoFileDelegate;
    class FastStatistics;
    class GiftiMetaData;
    class GiftiLabelTable;
    class Histogram;
    class LabelDrawingProperties;
    class MapFileDataSelector;
    class PaletteColorMapping;
    class PaletteFile;
    
    /**
     * \class caret::CaretMappableDataFile 
     * \brief A Caret data file that is mappable to surfaces and/or volumes.
     *
     * This class is essentially an interface that defines methods for
     * files that are 'mappable', as an overlay, to surfaces and/or volumes.
     * Use of a common interface simplifies selection and application
     * of these data files.
     *
     * For a GIFTI File, the number of maps is the number of data arrays
     * in the GIFTI file.  For a volume, it may be the number of time points.
     * 
     * Note that Caret5 used the term 'column'.
     */
    
    class CaretMappableDataFile : public CaretDataFile {
        
    public:
        CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~CaretMappableDataFile();
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        /**
         * @return Is the data mappable to a surface?
         */
        virtual bool isSurfaceMappable() const = 0;
        
        /**
         * @return Is the data mappable to a volume?
         */
        virtual bool isVolumeMappable() const = 0;
        
        /**
         * @return The number of maps in the file.  
         * Note: Caret5 used the term 'columns'.
         */
        virtual int32_t getNumberOfMaps() const = 0;
        
        virtual bool isMappableToSurfaceStructure(const StructureEnum::Enum structure) const;
        
        /**
         * @return True if the file has map attributes (name and metadata).
         * For files that do not have map attributes, they should override
         * this method and return false.  If not overriden, this method
         * returns true.
         *
         * Some files (such as CIFTI Connectivity Matrix Files and CIFTI
         * Data-Series Files) do not have Map Attributes and thus there
         * is no map name nor map metadata and options to edit these 
         * attributes should not be presented to the user.
         * 
         * These CIFTI files do contain palette color mapping but it is 
         * associated with the file.  To simplify palette color mapping editing
         * these file will return the file's palette color mapping for any
         * calls to getMapPaletteColorMapping().
         */
        virtual bool hasMapAttributes() const;
        
        /**
         * Get the name of the map at the given index.
         * 
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Name of the map.
         */
        virtual AString getMapName(const int32_t mapIndex) const = 0;

        /**
         * Find the index of the map that uses the given name.
         * 
         * @param mapName
         *    Name of the desired map.
         * @return
         *    Index of the map using the given name.  If there is more
         *    than one map with the given name, this method is likely
         *    to return the index of the first map with the name.
         */
        virtual int32_t getMapIndexFromName(const AString& mapName) const;
        
        /**
         * Find the index of the map that uses the given name.
         * 
         * @param mapName
         *    Name of the desired map.
         * @return
         *    Index of the map using the given name.  If there is more
         *    than one map with the given name, this method is likely
         *    to return the index of the first map with the name.
         */
        virtual int32_t getMapIndexFromNameOrNumber(const AString& mapName) const;
        
        /**
         * Set the name of the map at the given index.
         *
         * If the file does not have map attributes (hasMapAttributes())
         * calling this method will have not change the file.
         *
         * @param mapIndex
         *    Index of the map.
         * @param mapName
         *    New name for the map.
         */
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName) = 0;
        
        /**
         * Get the metadata for the map at the given index
         *
         * If the file does not have map attributes (hasMapAttributes())
         * a valid metadata object will be returned but changing its
         * content will have no effect on the file.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Metadata for the map (const value).
         */         
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const = 0;
        
        /**
         * Get the metadata for the map at the given index
         *
         * If the file does not have map attributes (hasMapAttributes())
         * a valid metadata object will be returned but changing its
         * content will have no effect on the file.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Metadata for the map.
         */         
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex) = 0;
        
        /**
         * Get the unique ID (UUID) for the map at the given index.
         * 
         * @param mapIndex
         *    Index of the map.
         * @return
         *    String containing UUID for the map.
         */
        virtual AString getMapUniqueID(const int32_t mapIndex) const = 0;
        
        /**
         * Find the index of the map that uses the given unique ID (UUID).
         * 
         * @param uniqueID
         *    Unique ID (UUID) of the desired map.
         * @return
         *    Index of the map using the given UUID.
         */
        virtual int32_t getMapIndexFromUniqueID(const AString& uniqueID) const;
        
        /**
         * @return Is the data in the file mapped to colors using
         * a palette.
         */
        virtual bool isMappedWithPalette() const = 0;
        
        /**
         * @return The estimated size of data after it is uncompressed
         * and loaded into RAM.  A negative value indicates that the
         * file size cannot be computed.
         */
        virtual int64_t getDataSizeUncompressedInBytes() const = 0;
        
        /**
         * Get statistics describing the distribution of data
         * mapped with a color palette at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Fast statistics for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex) = 0;
        
        /**
         * Get a histogram for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Histogram for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const Histogram* getMapHistogram(const int32_t mapIndex) = 0;
        
        /**
         * Get a histogram for the map at the given index of data
         * mapped with a color palette at the given index for
         * data within the given ranges.
         *
         * @param mapIndex
         *    Index of the map.
         * @param mostPositiveValueInclusive
         *    Values more positive than this value are excluded.
         * @param leastPositiveValueInclusive
         *    Values less positive than this value are excluded.
         * @param leastNegativeValueInclusive
         *    Values less negative than this value are excluded.
         * @param mostNegativeValueInclusive
         *    Values more negative than this value are excluded.
         * @param includeZeroValues
         *    If true zero values (very near zero) are included.
         * @return
         *    Histogram for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues) = 0;
        
        /**
         * Get statistics describing the distribution of data
         * mapped with a color palette for all data within the file.
         *
         * @return
         *    Fast statistics for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const FastStatistics* getFileFastStatistics() = 0;
        
        /**
         * Get histogram describing the distribution of data
         * mapped with a color palette for all data within
         * the file.
         *
         * @return
         *    Histogram for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const Histogram* getFileHistogram() = 0;
        
        /**
         * Get histogram describing the distribution of data
         * mapped with a color palette for all data in the file
         * within the given range of values.
         *
         * @param mostPositiveValueInclusive
         *    Values more positive than this value are excluded.
         * @param leastPositiveValueInclusive
         *    Values less positive than this value are excluded.
         * @param leastNegativeValueInclusive
         *    Values less negative than this value are excluded.
         * @param mostNegativeValueInclusive
         *    Values more negative than this value are excluded.
         * @param includeZeroValues
         *    If true zero values (very near zero) are included.
         * @return
         *    Descriptive statistics for data (will be NULL for data
         *    not mapped using a palette).
         */
        virtual const Histogram* getFileHistogram(const float mostPositiveValueInclusive,
                                                   const float leastPositiveValueInclusive,
                                                   const float leastNegativeValueInclusive,
                                                   const float mostNegativeValueInclusive,
                                                  const bool includeZeroValues) = 0;

        int32_t getFileHistogramNumberOfBuckets() const;
        
        void setFileHistogramNumberOfBuckets(const int32_t numberOfBuckets);
        
        /**
         * Get the palette color mapping for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Palette color mapping for the map (will be NULL for data
         *    not mapped using a palette).
         */         
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) = 0;
        
        /**
         * Get the palette color mapping for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Palette color mapping for the map (constant) (will be NULL for data
         *    not mapped using a palette).
         */         
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const = 0;
        
        /**
         * @return Is the data in the file mapped to colors using
         * a label table.
         */
        virtual bool isMappedWithLabelTable() const = 0;
        
        /**
         * @return Is the data in the file mapped to colors using
         * Red, Green, Blue, Alpha values.
         */
        virtual bool isMappedWithRGBA() const;
        
        /**
         * Get the label table for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Label table for the map (will be NULL for data
         *    not mapped using a label table).
         */         
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) = 0;
        
        /**
         * Get the label table for the map at the given index.
         *
         * @param mapIndex
         *    Index of the map.
         * @return
         *    Label table for the map (constant) (will be NULL for data
         *    not mapped using a label table).
         */         
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const = 0;
        
        bool isMedialWallLabelInMapLabelTable(const int32_t mapIndex) const;
        
        /**
         * Get the palette normalization modes that are supported by the file.
         *
         * @param modesSupportedOut
         *     Palette normalization modes supported by a file.  Will be
         *     empty for files that are not mapped with a palette.  If there
         *     is more than one suppported mode, the first mode in the
         *     vector is assumed to be the default mode.
         */
        virtual void getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const = 0;
        
        /**
         * @return The palette normalization mode for the file.
         */
        virtual PaletteNormalizationModeEnum::Enum getPaletteNormalizationMode() const;

        /**
         * Set the palette normalization mode for the file.
         *
         * @param mode
         *     New value for palette normalization mode.
         */
        virtual void setPaletteNormalizationMode(const PaletteNormalizationModeEnum::Enum mode);
        
        /**
         * Update coloring for all maps.
         *
         * @param paletteFile
         *    Palette file containing palettes.
         */
        virtual void updateScalarColoringForAllMaps(const PaletteFile* paletteFile);
        
        /**
         * Update coloring for a map.
         *
         * @param mapIndex
         *    Index of map.
         * @param paletteFile
         *    Palette file containing palettes.
         */
        virtual void updateScalarColoringForMap(const int32_t mapIndex,
                                          const PaletteFile* paletteFile) = 0;
        
        void invalidateHistogramChartColoring();
        
        virtual bool isPaletteColorMappingEqualForAllMaps() const;
        
        /**
         * @return The units for the 'interval' between two consecutive maps.
         */
        virtual NiftiTimeUnitsEnum::Enum getMapIntervalUnits() const;
        
        /**
         * Get the units value for the first map and the 
         * quantity of units between consecutive maps.  If the 
         * units for the maps is unknown, value of one (1) are
         * returned for both output values.
         *
         * @param firstMapUnitsValueOut
         *     Output containing units value for first map.
         * @param mapIntervalStepValueOut
         *     Output containing number of units between consecutive maps.
         */
        virtual void getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                                float& mapIntervalStepValueOut) const;
        
        /* documented in cxx file */
        virtual bool getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                             float& dataRangeMaximumOut) const;

        /* documented in cxx file */
        virtual bool isModifiedExcludingPaletteColorMapping() const;
        
        /* documented in cxx file. */
        virtual bool isModifiedPaletteColorMapping() const;
        
        /**
         * Check whether the file contains Cifti XML (all cifti types and also wbsparse have it)
         */
        virtual bool hasCiftiXML() const;
        
        /**
         * Get the Cifti XML, if the file has it
         * This could be faster if it returned a reference, but then it would have to throw when there is no xml object
         */
        virtual const CiftiXML getCiftiXML() const;
        
        /* documented in cxx file. */
        virtual bool isModified() const override;
        
        virtual void clearModified() override;
        
        virtual void clear();
        
        void initializeCaretMappableDataFileInstance();
        
        LabelDrawingProperties* getLabelDrawingProperties();
        
        const LabelDrawingProperties* getLabelDrawingProperties() const;
        
        ChartableTwoFileDelegate* getChartingDelegate();
        
        const ChartableTwoFileDelegate* getChartingDelegate() const;
        
        virtual void getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                        std::vector<float>& dataOut) const = 0;
        
    protected:
        CaretMappableDataFile(const CaretMappableDataFile&);

        CaretMappableDataFile& operator=(const CaretMappableDataFile&);
        
        ChartDataCartesian* helpCreateCartesianChartData(const std::vector<float>& data);
        
        void helpGetSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        void updateChartingDelegateAfterFileDataChanges();
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    private:
        
        void copyCaretMappableDataFile(const CaretMappableDataFile&);
        
        std::unique_ptr<LabelDrawingProperties> m_labelDrawingProperties;

        mutable std::unique_ptr<ChartableTwoFileDelegate> m_chartingDelegate;
    };

#ifdef __CARET_MAPPABLE_DATA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_MAPPABLE_DATA_FILE_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE__H_
