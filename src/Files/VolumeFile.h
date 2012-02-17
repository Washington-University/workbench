
#ifndef __VOLUME_FILE_H__
#define __VOLUME_FILE_H__

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

#include "VolumeBase.h"
#include "CaretMappableDataFile.h"

namespace caret {

    class VolumeFile : public VolumeBase, public CaretMappableDataFile
    {
        
    public:
        
        enum InterpType
        {
            ENCLOSING_VOXEL,
            TRILINEAR
        };
        
        const static float INVALID_INTERP_VALUE;
        
        VolumeFile();
        VolumeFile(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1);
        //convenience method for unsigned
        VolumeFile(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1);
        ~VolumeFile();
        
        float interpolateValue(const float* coordIn, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0);

        float interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0);

        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const VolumeFile* right) const;
        
        void readFile(const AString& filename) throw (DataFileException);

        void writeFile(const AString& filename) throw (DataFileException);

        bool isEmpty() const { return VolumeBase::isEmpty(); }
        
        virtual void setModified() { VolumeBase::setModified(); }
        
        virtual void clearModified() { CaretMappableDataFile::clearModified(); VolumeBase::clearModified(); }
        
        virtual bool isModified() const { return (CaretMappableDataFile::isModified() || VolumeBase::isModified()); }
        
        /**
         * @return The structure for this file.
         */
        StructureEnum::Enum getStructure() const { return VolumeBase::getStructure(); }
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        void setStructure(const StructureEnum::Enum structure) { VolumeBase::setStructure(structure); }
        
        /**
         * @return Get access to the file's metadata.
         */
        GiftiMetaData* getFileMetaData() { return VolumeBase::getFileMetaData(); }
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        const GiftiMetaData* getFileMetaData() const { return VolumeBase::getFileMetaData(); }
        
        bool isSurfaceMappable() const { return VolumeBase::isSurfaceMappable(); }
        
        bool isVolumeMappable() const { return VolumeBase::isVolumeMappable(); }
        
        int32_t getNumberOfMaps() const { return VolumeBase::getNumberOfMaps(); }
        
        AString getMapName(const int32_t mapIndex) const { return VolumeBase::getMapName(mapIndex); }
        
        int32_t getMapIndexFromName(const AString& mapName) { return VolumeBase::getMapIndexFromName(mapName); }
        
        void setMapName(const int32_t mapIndex,
                                const AString& mapName) { VolumeBase::setMapName(mapIndex, mapName); }
        
        const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const { return VolumeBase::getMapMetaData(mapIndex); }
        
        GiftiMetaData* getMapMetaData(const int32_t mapIndex) { return VolumeBase::getMapMetaData(mapIndex); }
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex) { return VolumeBase::getMapStatistics(mapIndex); }
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                      const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues) {
            return VolumeBase::getMapStatistics(mapIndex,
                                                mostPositiveValueInclusive,
                                                leastPositiveValueInclusive,
                                                leastNegativeValueInclusive,
                                                mostNegativeValueInclusive,
                                                includeZeroValues);             
        }
        
        bool isMappedWithPalette() const { return VolumeBase::isMappedWithPalette(); }
        
        PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) { return VolumeBase::getMapPaletteColorMapping(mapIndex); }
        
        const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const { return VolumeBase::getMapPaletteColorMapping(mapIndex); }
        
        bool isMappedWithLabelTable() const { return VolumeBase::isMappedWithLabelTable(); }
        
        GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) { return VolumeBase::getMapLabelTable(mapIndex); }
        
        const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const { return VolumeBase::getMapLabelTable(mapIndex); }

        AString getMapUniqueID(const int32_t mapIndex) const { return VolumeBase::getMapUniqueID(mapIndex); }
        
        int32_t getMapIndexFromUniqueID(const AString& uniqueID) const { return VolumeBase::getMapIndexFromUniqueID(uniqueID); }
    };

}

#endif //__VOLUME_FILE_H__
