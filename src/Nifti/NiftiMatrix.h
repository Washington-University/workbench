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
#include "stdint.h"
namespace caret {

struct  LayoutType {
    bool layoutSet;
    bool needsSwapping;
    int32_t componentDimensions;
    std::vector <int64_t> dimensions;
    NiftiDataTypeEnum::Enum niftiDataType;//we borrow this enum from nifti, but could be used in generic matrix implementation
    int32_t valueByteSize;//redundant, and derivable from Nifti
};



class NiftiMatrix : public LayoutType //so we don't have to qualify layouts
{
public:
    NiftiMatrix();
    //NiftiMatrix(const AString &filename) throw (NiftiException);
    //NiftiMatrix(const AString &filename, int64_t &offsetin) throw (NiftiException);
    NiftiMatrix(const QFile &filein);
    NiftiMatrix(const QFile &filein, const int64_t &offsetin);
    ~NiftiMatrix() { flushCurrentFrame(); clearMatrix();}

    void flushCurrentFrame();
    void resetMatrix();
    void setMatrixFile(const QFile &filein);
    void init();

    // Below are low level functions for operating on generic matrix files, currently used by Nifti

    // !!!SECTION 1: matrix reader set up!!!
    /*use functions below to setup layout first before reading a frame, so that the matrix reader "knows" how interpret/load/format the data into the internal matrix
      it's flexible, but the recommended usage is:
      1. setMatrixOffset(if non-zero)
      2. setDataType (the data type as it is stored on disk)
      3. setMatrixLayoutOnDisk();*/

    void setMatrixOffset(const int64_t &offsetin) throw (NiftiException) { this->matrixStartOffset = offsetin; }

    // the following two functions are nifti specific
    void setMatrixLayoutOnDisk(const Nifti1Header &headerIn);
    void setMatrixLayoutOnDisk(const Nifti2Header &headerIn);

    void getMatrixLayoutOnDisk(LayoutType &layout);
    void setMatrixLayoutOnDisk(LayoutType &layout);
    void getMatrixLayoutOnDisk(std::vector<int64_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut,int64_t &frameLengthOut, int64_t &frameSizeOut ) const;
    void setMatrixLayoutOnDisk(const std::vector<int64_t> &dimensionsIn, const int &componentDimensionsIn,const  int &valueByteSizeIn, const bool &needsSwappingIn );

    // !!!SECTION 2: frame reading set up, call AFTER using set up functions above!!!
    //after setting matrix layout, a frame may be read.

    // use read frame where data preservation is important and you care about what is in that frame.
    // use set frame to write to frame that hasn't previously been written, or to completely over-write a frame if
    // you don't care about the data it contains.
    /// Reads frame from disk into memory, flushes previous frame to disk if changes were made.
    void readFrame(int64_t timeSlice=0L)  throw (NiftiException);//for loading a frame at a time

    /// Sets the current frame for writing, doesn't load any data from disk, can hand in a frame pointer for speed, writes out previous frame to disk if needed
    void setFrame(float *matrixIn, const int64_t &matrixLengthIn, const int64_t &timeSlice = 0L)  throw(NiftiException);
    /// Sets the current frame (for writing), doesn't load any data from disk, writes out previous frame to disk if needed
    void setFrame(const int64_t &timeSlice=0L) throw(NiftiException);
    /// Gets the entire loaded frame as floats, for easier manipulation
    void getFrame(float *frame) throw (NiftiException);
    /// Writes the current frame to disk.
    void writeFrame() throw(NiftiException);
    // TODO: another option is loading the entire nifti matrix, then readFrame simply copies the current adddress of the timeslice offset,not implemented yet
    //void readMatrix() {}//for loading the entire matrix, not implemented
    //void setMatrix...
    //void writeMatrix...

    // !!!SECTION 3: after frame has been loaded, use functions below to manipulate frame data
    //once a frame has been loaded, use the functions below to manipulate it
    void translateVoxel(const int64_t &i, const int64_t &j, const int64_t &k, int64_t &index) const;
    float getComponent(const int64_t &index, const int64_t &componentIndex) const throw (NiftiException);
    void setComponent(const int64_t &index, const int64_t &componentIndex, const float &value) throw (NiftiException);
    /// volume read/write Functions
    /// get VolumeFrame
    void getVolumeFrame(VolumeFile &frameOut, const int64_t timeSlice, const int64_t component=0);
    /// set VolumeFrame
    void setVolumeFrame(VolumeFile &frameIn, const int64_t & timeSlice, const int64_t component=0);
    /* void getVolumeFrame(VolumeFile &volume, int64_t &timeslice) throw (NiftiException);
    void setVolumeFrame(VolumeFile &volume, int64_t &timeslice) throw (NiftiException);
    */
    // !!!SECTION 4: convenient getter/setters
    int64_t getFrameSize() { return frameSize;}
    int64_t getFrameLength() { return frameLength; }
    private:
    void reAllocateMatrixIfNeeded();
    void clearMatrix();
    //frames represent brain volumes on disk, the matrix is the internal storage for the matrix after it has been loaded from the file.
    int64_t calculateFrameLength(const std::vector<int64_t> &dimensionsIn) const;
    int64_t calculateFrameSizeInBytes(const int64_t &frameLengthIn, const int32_t &valueByteSizeIn, const int64_t &componentDimensionsIn) const;
    int64_t calculateMatrixLength(const int64_t &frameLengthIn, const int64_t &componentDimensionsIn) const;
    int64_t calculateMatrixSizeInBytes(const int64_t &frameSizeIn, const int64_t &componentDimensionsIn) const;

    QFile file;
    int64_t matrixStartOffset;

    //layout

    int64_t frameLength;
    int64_t frameSize;
    bool frameLoaded;
    bool frameNeedsWriting;

    // matrix data
    int64_t currentTime;
    float *matrix;
    int64_t matrixLength;//length of array
    bool matrixLoaded;
};

}
#endif // NIFTIMATRIX_H

