
#ifndef __VOLUME_BASE_H__
#define __VOLUME_BASE_H__

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

#include "stdint.h"
#include <vector>
#include "CaretAssert.h"
#include "NiftiEnums.h"
#include "StructureEnum.h"
#include "CaretPointer.h"

namespace caret {

    class DescriptiveStatistics;
    class GiftiLabelTable;
    class GiftiMetaData;
    class PaletteColorMapping;
    
    struct AbstractVolumeExtension
    {
        enum ExtensionType
        {
            NIFTI1,
            NIFTI2
        };
        ExtensionType type;
        CaretArray<char> m_bytes;
        virtual ExtensionType getType() = 0;
    };
    
    struct AbstractHeader
    {
        enum HeaderType
        {
            NIFTI1,
            NIFTI2
        };
        HeaderType type;
        virtual HeaderType getType() = 0;
    };
    
    class VolumeBase 
    {
    protected:
        std::vector<std::vector<float> > m_indexToSpace;
        std::vector<std::vector<float> > m_spaceToIndex;//not valid yet, need MathUtilities
        float* m_data;
        int64_t m_dataSize;
        int64_t m_dimensions[5];//store internally as 4d+component
        std::vector<int64_t> m_origDims;//but keep track of the original dimensions
        float*** m_indexRef;//some magic to avoid multiply during getVoxel/setVoxel
        int64_t* m_jMult;//some magic for fast getIndex/getValue/setValue
        int64_t* m_kMult;
        int64_t* m_bMult;
        int64_t* m_cMult;
        
        void freeMemory();
        void setupIndexing();//sets up the magic

        void createAttributes();
        void freeAttributes();
        GiftiMetaData* m_metadata; // file's metadata
        GiftiLabelTable* m_labelTable;
        PaletteColorMapping* m_paletteColorMapping;
        
        class BrickAttributes {
        public:
            BrickAttributes();
            
            ~BrickAttributes();
            
            GiftiMetaData* m_metadata;
            DescriptiveStatistics* m_statistics;
        };
        
        NiftiIntentEnum::Enum m_niftiIntent;
        std::vector<BrickAttributes*> m_brickAttributes;
        
        bool m_ModifiedFlag;
        
    public:
        enum OrientTypes
        {
            LEFT_TO_RIGHT,
            RIGHT_TO_LEFT,
            POSTERIOR_TO_ANTERIOR,
            ANTERIOR_TO_POSTERIOR,
            INFERIOR_TO_SUPERIOR,
            SUPERIOR_TO_INFERIOR
        };
        
        VolumeBase();
        VolumeBase(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1);
        //convenience method for unsigned
        VolumeBase(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1);
        virtual ~VolumeBase();

        ///there isn't much VolumeFile can do to restrict access to extensions, so just have them public
        std::vector<CaretPointer<AbstractVolumeExtension> > m_extensions;
        
        ///ditto for header, but make it self-deleting
        CaretPointer<AbstractHeader> m_header;
        
        ///recreates the volume file storage with new size and spacing
        void reinitialize(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1);
        void reinitialize(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1);
        
        ///get the spacing info
        inline const std::vector<std::vector<float> >& getVolumeSpace() const {
            return m_indexToSpace;
        }

        ///get the originally specified dimensions vector
        inline const std::vector<int64_t>& getOriginalDimensions() const {
            return m_origDims;
        }
        
        ///returns true if volume space is not skew, and each axis and index is separate
        bool isPlumb() const;

        ///returns orientation, spacing, and center (spacing/center can be negative, spacing/center is LPI rearranged to ijk (first dimension uses first element), will assert false if isOblique is true)
        void getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* centerOut) const;

        //not to worry, simple passthrough convenience functions like these get partially optimized to the main one by even -O1, and completely optimized together by -O2 or -O3

        ///returns coordinate triplet of an index triplet
        void indexToSpace(const int64_t* indexIn, float* coordOut) const;
        ///returns coordinate triplet of three indexes
        //void indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float* coordOut) const;
        ///returns three coordinates of an index triplet
        void indexToSpace(const int64_t* indexIn, float& coordOut1, float& coordOut2, float& coordOut3) const;
        ///returns three coordinates of three indexes
        //void indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3) const;

        ///returns coordinate triplet of a floating point index triplet
        void indexToSpace(const float* indexIn, float* coordOut) const;
        ///returns coordinate triplet of three floating point indexes
        void indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float* coordOut) const;
        ///returns three coordinates of a floating point index triplet
        void indexToSpace(const float* indexIn, float& coordOut1, float& coordOut2, float& coordOut3) const;
        ///returns three coordinates of three floating point indexes
        void indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3) const;

        ///returns floating point index triplet of a given coordinate triplet
        void spaceToIndex(const float* coordIn, float* indexOut) const;
        ///returns floating point index triplet of three given coordinates
        void spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float* indexOut) const;
        ///returns three floating point indexes of a given coordinate triplet
        void spaceToIndex(const float* coordIn, float& indexOut1, float& indexOut2, float& indexOut3) const;
        ///returns three floating point indexes of three given coordinates
        void spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3) const;

        ///returns integer index triplet of voxel whose center is closest to the coordinate triplet
        void closestVoxel(const float* coordIn, int64_t* indexOut) const;
        ///returns integer index triplet of voxel whose center is closest to the three coordinates
        void closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t* indexOut) const;
        ///returns integer indexes of voxel whose center is closest to the coordinate triplet
        void closestVoxel(const float* coordIn, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const;
        ///returns integer indexes of voxel whose center is closest to the three coordinates
        void closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const;

        ///get a value at an index triplet and optionally timepoint
        inline float getValue(const int64_t* indexIn, const int64_t brickIndex = 0, const int64_t component = 0) const
        {
            return getValue(indexIn[0], indexIn[1], indexIn[2], brickIndex, component);
        }
        
        ///get a value at three indexes and optionally timepoint
        inline float getValue(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0) const
        {
            CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, brickIndex, component));//assert so release version isn't slowed by checking
            if (m_indexRef != NULL)
            {
                return m_indexRef[component][brickIndex][indexIn1 + m_jMult[indexIn2] + m_kMult[indexIn3]];
            } else {
                return m_data[getIndex(indexIn1, indexIn2, indexIn3, brickIndex, component)];
            }
        }

        ///get a frame (const)
        const float* getFrame(const int64_t brickIndex = 0, const int64_t component = 0) const;
        
        ///set a value at an index triplet and optionally timepoint
        inline void setValue(const float& valueIn, const int64_t* indexIn, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            setValue(valueIn, indexIn[0], indexIn[1], indexIn[2], brickIndex, component);
        }
        
        ///set a value at an index triplet and optionally timepoint
        inline void setValue(const float& valueIn, const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, brickIndex, component));//assert so release version isn't slowed by checking
            if (m_indexRef != NULL)
            {
                m_indexRef[component][brickIndex][indexIn1 + m_jMult[indexIn2] + m_kMult[indexIn3]] = valueIn;
            } else {
                m_data[getIndex(indexIn1, indexIn2, indexIn3, brickIndex, component)] = valueIn;
            }
            setModified();
        }
        
        /// set every voxel to the given value
        void setValueAllVoxels(const float value);
        
        ///set a frame
        void setFrame(const float* frameIn, const int64_t brickIndex = 0, const int64_t component = 0);

        ///gets dimensions as a vector of 5 integers, 3 spatial, time, components
        void getDimensions(std::vector<int64_t>& dimOut) const;
        ///gets dimensions
        void getDimensions(int64_t& dimOut1, int64_t& dimOut2, int64_t& dimOut3, int64_t& dimTimeOut, int64_t& numComponents) const;

        ///gets index into data array for three indexes plus time index
        inline int64_t getIndex(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0) const
        {
            CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, brickIndex, component));
            if (m_indexRef != NULL)
            {
                //TRICK: use pointer math and the indexing arrays to get the index if it is set up
                return (m_indexRef[component][brickIndex] + indexIn1 + m_jMult[indexIn2] + m_kMult[indexIn3]) - m_data;
            } else {//otherwise, calculate via precalculated multiply arrays
                return indexIn1 + m_jMult[indexIn2] + m_kMult[indexIn3] + m_bMult[brickIndex] + m_cMult[component];
            }
        }
        
        inline int64_t getIndex(const int64_t* indexIn, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            return getIndex(indexIn[0], indexIn[1], indexIn[2], brickIndex, component);
        }

        inline bool indexValid(const int64_t* indexIn, const int64_t brickIndex = 0, const int64_t component = 0) const
        {
            return indexValid(indexIn[0], indexIn[1], indexIn[2], brickIndex, component);
        }

        ///checks if an index is within array dimensions
        inline bool indexValid(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0) const
        {//inlined so that getValue and setValue can get optimized out entirely
            if (indexIn1 < 0 || indexIn1 >= m_dimensions[0]) return false;
            if (indexIn2 < 0 || indexIn2 >= m_dimensions[1]) return false;
            if (indexIn3 < 0 || indexIn3 >= m_dimensions[2]) return false;
            if (brickIndex < 0 || brickIndex >= m_dimensions[3]) return false;
            if (component < 0 || component >= m_dimensions[4]) return false;
            return true;
        }

        void setModified() { m_ModifiedFlag = true; }
        void clearModified() { m_ModifiedFlag = false;}
        bool isModified() { return m_ModifiedFlag; }
        
        bool isEmpty() const;
        
        
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
        

        
        bool isSurfaceMappable() const;
        
        bool isVolumeMappable() const;
        
        int32_t getNumberOfMaps() const;
        
        AString getMapName(const int32_t mapIndex) const;
        
        int32_t getMapIndexFromName(const AString& mapName);
        
        void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        bool isMappedWithPalette() const;
        
        PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        bool isMappedWithLabelTable() const;
        
        GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;

        AString getMapUniqueID(const int32_t mapIndex) const;
        
        int32_t getMapIndexFromUniqueID(const AString& uniqueID) const;
        
    };

}

#endif //__VOLUME_BASE_H__
