
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

#include "stdint.h"
#include <vector>
#include "CaretAssert.h"
#include "DataFile.h"
#include "Nifti1Header.h"
#include "Nifti2Header.h"

namespace caret {

    class VolumeFile : public DataFile
    {
        enum StoredHeaderType
        {
            NIFTI_1,
            NIFTI_2,
            NONE
        };
        Nifti1Header m_N1Header;
        Nifti2Header m_N2Header;
        StoredHeaderType m_headerType;
        std::vector<std::vector<float> > m_indexToSpace;
        std::vector<std::vector<float> > m_spaceToIndex;//not valid yet, need MathUtilities
        float* m_data;
        int64_t m_dimensions[5];//don't support more than 4d volumes yet
        float***** m_indexRef;//some magic to avoid multiply during getVoxel/setVoxel
        std::vector<int64_t> m_jMult, m_kMult, m_bMult, m_cMult;//some magic for faster getIndex

        void freeMemory();
        void setupIndexing();//sets up the magic

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
        VolumeFile();
        VolumeFile(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1);
        ~VolumeFile();

        ///recreates the volume file storage with new size and spacing
        void reinitialize(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1);

        ///get the spacing info
        const std::vector<std::vector<float> >& getVolumeSpace() {
            return m_indexToSpace;
        }

        ///returns true if volume space is not skew, and each axis and index is separate
        bool isPlumb();

        ///returns orientation, spacing, and center (spacing/center can be negative, spacing/center is LPI rearranged to ijk (first dimension uses first element), will assert false if isOblique is true)
        void getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* centerOut);

        //not to worry, simple passthrough convenience functions like these get partially optimized to the main one by even -O1, and completely optimized together by -O2 or -O3

        ///returns coordinate triplet of an index triplet
        void indexToSpace(const int64_t* indexIn, float* coordOut);
        ///returns coordinate triplet of three indexes
        void indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float* coordOut);
        ///returns three coordinates of an index triplet
        void indexToSpace(const int64_t* indexIn, float& coordOut1, float& coordOut2, float& coordOut3);
        ///returns three coordinates of three indexes
        void indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3);

        ///returns coordinate triplet of a floating point index triplet
        void indexToSpace(const float* indexIn, float* coordOut);
        ///returns coordinate triplet of three floating point indexes
        void indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float* coordOut);
        ///returns three coordinates of a floating point index triplet
        void indexToSpace(const float* indexIn, float& coordOut1, float& coordOut2, float& coordOut3);
        ///returns three coordinates of three floating point indexes
        void indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3);

        ///returns floating point index triplet of a given coordinate triplet
        void spaceToIndex(const float* coordIn, float* indexOut);
        ///returns floating point index triplet of three given coordinates
        void spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float* indexOut);
        ///returns three floating point indexes of a given coordinate triplet
        void spaceToIndex(const float* coordIn, float& indexOut1, float& indexOut2, float& indexOut3);
        ///returns three floating point indexes of three given coordinates
        void spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3);

        ///returns integer index triplet of voxel whose center is closest to the coordinate triplet
        void closestVoxel(const float* coordIn, int64_t* indexOut);
        ///returns integer index triplet of voxel whose center is closest to the three coordinates
        void closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t* indexOut);
        ///returns integer indexes of voxel whose center is closest to the coordinate triplet
        void closestVoxel(const float* coordIn, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3);
        ///returns integer indexes of voxel whose center is closest to the three coordinates
        void closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3);

        ///get a value at an index triplet and optionally timepoint
        inline float getValue(const int64_t* indexIn, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            return getValue(indexIn[0], indexIn[1], indexIn[2], brickIndex, component);
        }
        ///get a value at three indexes and optionally timepoint
        inline float getValue(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, brickIndex, component));//assert so release version isn't slowed by checking
            return m_indexRef[component][brickIndex][indexIn3][indexIn2][indexIn1];
        }

        ///get a frame
        void getFrame(float* frameOut, const int64_t brickIndex = 0, const int64_t component = 0);
        
        ///set a value at an index triplet and optionally timepoint
        inline void setValue(const float& valueIn, const int64_t* indexIn, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            setValue(valueIn, indexIn[0], indexIn[1], indexIn[2], brickIndex, component);
        }
        ///set a value at an index triplet and optionally timepoint
        inline void setValue(const float& valueIn, const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0)
        {
            CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, brickIndex, component));//assert so release version isn't slowed by checking
            m_indexRef[component][brickIndex][indexIn3][indexIn2][indexIn1] = valueIn;
        }
        
        ///set a frame
        void setFrame(const float* frameIn, const int64_t brickIndex = 0, const int64_t component = 0);

        ///get the raw voxel data
        float* getVoxelDataRef() {
            return m_data;
        }

        ///gets dimensions as a vector of 5 integers, 3 spatial, time, components
        void getDimensions(std::vector<int64_t>& dimOut);
        ///gets dimensions
        void getDimensions(int64_t& dimOut1, int64_t& dimOut2, int64_t& dimOut3, int64_t& dimTimeOut, int64_t& numComponents);

        ///gets index into data array for three indexes plus time index
        int64_t getIndex(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0);

        ///checks if an index is within array dimensions
        inline bool indexValid(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex = 0, const int64_t component = 0)
        {//inlined so that getValue and setValue can get optimized out entirely
            if (indexIn1 < 0 || indexIn1 >= m_dimensions[0]) return false;
            if (indexIn2 < 0 || indexIn2 >= m_dimensions[1]) return false;
            if (indexIn3 < 0 || indexIn3 >= m_dimensions[2]) return false;
            if (brickIndex < 0 || brickIndex >= m_dimensions[3]) return false;
            if (component < 0 || component >= m_dimensions[4]) return false;
            return true;
        }

        virtual void readFile(const AString& filename) throw (DataFileException);

        virtual void writeFile(const AString& filename) throw (DataFileException);

        virtual bool isEmpty() const;

    };

}

#endif //__VOLUME_FILE_H__
