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


//// DO NOT USE!!!!!
//// THIS IS THE BEGINNING OF AN IMPLEMENTATION OF A GENERIC MATRIX READER/WRITER
//// IT'S NOT READY YET, SO DON'T BOTHER EVEN LOOKING, AS FUNCTION NAMES, HOW IT'S USED
//// CAN AND WILL CHANGE!!!!
#if 0
#ifndef MATRIX_H
#define MATRIX_H
#include "Layout.h"

namespace caret {

struct Matrix : public LayoutType {
public:
    Matrix();
    //Matrix(const AString &filename) throw (NiftiException);
    //Matrix(const AString &filename, int64_t &offsetin) throw (NiftiException);
    Matrix(const QFile &filein);
    Matrix(const QFile &filein, const int64_t &offsetin);
    ~Matrix() { flushCache(); clearMatrix();}

    /// Writes out any changes made to the current Frame to the disk, if it has been changed
    void flushCache();
    void setMatrixFile(const QFile &filein);
    bool isCompressed();

    // Below are low level functions for operating on generic matrix files, currently used by Nifti

    // !!!SECTION 1: matrix reader set up!!!
    /*use functions below to setup layout first before reading a frame, so that the matrix reader "knows" how interpret/load/format the data into the internal matrix
      it's flexible, but the recommended usage is:
      1. setMatrixOffset(if non-zero)
      2. setDataType (the data type as it is stored on disk)
      3. setMatrixLayoutOnDisk();*/

    /// sets the offset of the start of the matrix in the file, in case we are reading from a file that has a header before the matrix.
    void setMatrixOffset(const int64_t &offsetin) throw (NiftiException) { this->matrixStartOffset = offsetin; }

    /// Gets the matrix's layout on disk, since this matrix class only operates on one file at a time, it affects both reads and writes
    void getMatrixLayoutOnDisk(LayoutType &layout);
    /// set the matrix layout on disk by handing it a predefined layout struct
    void setMatrixLayoutOnDisk(LayoutType &layout);
    /// get the layout On Disk
    void getMatrixLayoutOnDisk(std::vector<int64_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut,int64_t &frameLengthOut, int64_t &frameSizeOut );
    /// set the layout on Disk by handing it required params
    void setMatrixLayoutOnDisk(const std::vector<int64_t> &dimensionsIn, const int &componentDimensionsIn,const bool &needsSwappingIn );

    /// Gets the matrix's layout, since this matrix class only operates on one file at a time, it affects both reads and writes
    void getMatrixLayout(LayoutType &layout);
    /// set the matrix layout on disk by handing it a predefined layout struct
    void setMatrixLayout(LayoutType &layout);
    /// get the layout
    void getMatrixLayout(std::vector<int64_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut,int64_t &frameLengthOut, int64_t &frameSizeOut );
    /// set the layout by handing it required params
    void setMatrixLayout(const std::vector<int64_t> &dimensionsIn, const int &componentDimensionsIn,const bool &needsSwappingIn );

    // !!!SECTION 2: frame reading set up, call AFTER using set up functions above!!!
    //after setting matrix layout, a frame may be read.

    // use read frame where data preservation is important and you care about what is in that frame.
    // use set frame to write to frame that hasn't previously been written, or to completely over-write a frame if
    // you don't care about the data it contains.
    /// Reads frame from disk into memory, flushes previous frame to disk if changes were made.
    void readChunk(int64_t index=0L)  throw (NiftiException);//for loading a frame at a time
    /// Sets the current frame for writing, doesn't load any data from disk, can hand in a frame pointer for speed, writes out previous frame to disk if needed
    void setChunk(float *matrixIn, const int64_t &matrixLengthIn, const int64_t &timeSlice = 0L, const int64_t &componentIndex=0L)  throw(NiftiException);
    /// Sets the current frame (for writing), doesn't load any data from disk, writes out previous frame to disk if needed
    void setChunk(const int64_t &timeSlice=0L) throw(NiftiException);
    /// Gets the entire loaded frame as floats, for easier manipulation
    void getChunk(float *frame, const int64_t &componentIndex=0L) throw (NiftiException);
    /// Writes the current frame to disk.
    void writeChunk() throw(NiftiException);
    // TODO: another option is loading the entire nifti matrix, then readFrame simply copies the current adddress of the timeslice offset,not implemented yet
    //void readMatrix() {}//for loading the entire matrix, not implemented
    //void setMatrix...
    //void writeMatrix...

    // !!!SECTION 3: after frame has been loaded, use functions below to manipulate frame data
    //once a frame has been loaded, use the functions below to manipulate it
    void translateCoordsToIndex(std::vector<int32_t> &coords, int64_t &index) const;

    // !!!SECTION 4: convenient getter/setters
    /// Gets the Size in bytes of the current frame
    int64_t getChunkSize() { return frameSize;}
    /// Gets the number of individual elements (array size) of the current frame
    int64_t getChunkLength() { return frameLength; }


protected:
    void reAllocateMatrixIfNeeded();
    void clearMatrix();
    void init();
    void resetMatrix();
    //frames represent brain volumes on disk, the matrix is the internal storage for the matrix after it has been loaded from the file.
    int64_t calculateFrameLength(const std::vector<int64_t> &dimensionsIn) const;
    int64_t calculateFrameSizeInBytes(const int64_t &frameLengthIn, const int32_t &valueByteSizeIn) const;
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

    //supported matrix storage types
    double * doubleMatrix;
    float * floatMatrix;
    int64_t * int64Matrix;
    int32_t * int32Matrix;
    int16_t * int16Matrix;
    int8_t * int8Matrix;
    uint64_t * uint64Matrix;
    uint32_t * uint32Matrix;
    uint16_t * uint16Matrix;
    uint8_t * uint8Matrix;
};

}

#endif //MATRIX_H
#endif //0
