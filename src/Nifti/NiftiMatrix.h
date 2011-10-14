/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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

#ifndef NIFTIMATRIX_H
#define NIFTIMATRIX_H
#include "NiftiEnums.h"
#include "NiftiException.h"
#include "QFile"
#include "AString.h"
#include "VolumeFile.h"
namespace caret {

struct  LayoutType {
bool layoutSet;
bool needsSwapping;
uint32_t componentDimensions;
std::vector <int> dimensions;
NiftiDataTypeEnum::Enum niftiDataType;//we borrow this enum from nifti, but could be used in generic matrix implementation
uint32_t valueByteSize;//redundant, and derivable from Nifti
};



class NiftiMatrix : public LayoutType //so we don't have to qualify layouts
{
public:
    NiftiMatrix();
    //NiftiMatrix(const AString &filename) throw (NiftiException);
    //NiftiMatrix(const AString &filename, int64_t &offsetin) throw (NiftiException);
    NiftiMatrix(const QFile &filein);
    NiftiMatrix(const QFile &filein, const int64_t &offsetin);
    ~NiftiMatrix() { clearMatrix();};

    void init();

    void getVolumeFrame(VolumeFile &volume, int64_t &timeslice) throw (NiftiException);
    void setVolumeFrame(VolumeFile &volume, int64_t &timeslice) throw (NiftiException);

    // Below are low level functions for operating on generic matrix files, currently used by Nifti

    // !!! NIFTI SPECIFIC nifti specific will, move to Nifti file !!!
    void setNiftiHeader(Nifti1Header &headerin);
    void setNiftiHeader(Nifti2Header &headerin);

    void getLayoutFromNiftiHeader(const Nifti1Header &headerIn);
    void getLayoutFromNiftiHeader(const Nifti2Header &headerIn);
    // !!! END NIFTI SPECIFIC !!!

    // !!!SECTION 1: matrix reader set up!!!
    /*use functions below to setup layout first before reading a frame, so that the matrix reader "knows" how interpret/load/format the data into the internal matrix
      it's flexible, but the recommended usage is:
      1. setMatrixOffset(if non-zero)
      2. setDataType (the data type as it is stored on disk)
      3. setMatrixLayoutOnDisk();*/

    void setMatrixOffset(const int64_t &offsetin) throw (NiftiException);

    //these two are a bit redundant, since one can get this info from set/getMatrixLayoutOnDisk
    void setneedsSwapping(bool &needsSwappingIn) { needsSwapping = needsSwappingIn; }
    void getSwapNeeed(bool &needsSwappingOut) const { needsSwappingOut = needsSwapping; }

    void setDataType(const NiftiDataTypeEnum::Enum &typein);
    void getDataType(NiftiDataTypeEnum::Enum &typeout) const;
    void getMatrixLayoutOnDisk(LayoutType &layout);
    void setMatrixLayoutOnDisk(LayoutType &layout);
    void getMatrixLayoutOnDisk(std::vector<int32_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut,uint64_t &frameLengthOut, uint64_t &frameSizeOut ) const;
    void setMatrixLayoutOnDisk(const std::vector<int32_t> &dimensionsIn, const int &componentDimensionsIn,const  int &valueByteSizeIn, const bool &needsSwappingIn );

    // !!!SECTION 2: frame reading set up, call AFTER using set up functions above!!!
    //after setting matrix layout, a frame may be read.
    void readFrame(int64_t timeSlice=0L)  throw (NiftiException);//for loading a frame at a time
    void setFrame(float *matrixIn, int64_t &matrixLengthIn, int64_t timeSlice = 0L)  throw(NiftiException);
    void writeFrame(int64_t &timeSlice) throw(NiftiException);
    // TODO: another option is loading the entire nifti matrix, then readFrame simply copies the current adddress of the timeslice offset,not implemented yet
    //void readMatrix() {}//for loading the entire matrix, not implemented
    //void setMatrix...
    //void writeMatrix...

    // !!!SECTION 3: after frame has been loaded, use functions below to manipulate frame data
    //once a frame has been loaded, use the functions below to manipulate it
    void translateVoxel(const uint64_t &i, const uint64_t &j, const uint64_t &k, uint64_t &index) const;
    float getComponent(const uint64_t &index, const uint32_t &componentIndex) const throw (NiftiException);
    void setComponent(const uint64_t &index, const uint32_t &componentIndex, const float &value) throw (NiftiException);

    // !!!SECTION 4: convenient getter/setters
    uint64_t getFrameSize() { return frameSize;}
    uint64_t getFrameLength() { return frameLength; }
    private:
    void reAllocateMatrixIfNeeded();
    void clearMatrix();
    //frames represent brain volumes on disk, the matrix is the internal storage for the matrix after it has been loaded from the file.
    uint64_t calculateFrameLength(const std::vector<int> &dimensionsIn) const;
    uint64_t calculateFrameSizeInBytes(const uint64_t &frameLengthIn, const uint32_t &valueByteSizeIn, const uint32_t &componentDimensionsIn) const;
    uint64_t calculateMatrixLength(const uint64_t &frameLengthIn, const uint64_t &componentDimensionsIn) const;
    uint64_t calculateMatrixSizeInBytes(const uint64_t &frameSizeIn, const uint64_t &componentDimensionsIn) const;

    QFile file;
    int64_t matrixStartOffset;

    //nifti specific
    Nifti1Header n1Header;
    Nifti2Header n2Header;
    int niftiVersion;

    //non-nifti specific
    //layout

    uint64_t frameLength;
    uint64_t frameSize;
    bool frameLoaded;

    // matrix data
    int64_t currentTime;
    float_t *matrix;
    int64_t matrixLength;//length of array
    bool matrixLoaded;
};

}
#endif // NIFTIMATRIX_H

