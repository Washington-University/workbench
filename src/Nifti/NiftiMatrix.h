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
#include "Nifti1Header.h"
#include "Nifti2Header.h"
#include "NiftiEnums.h"
#include "NiftiException.h"
#include "QFile"
#include "AString.h"
#include "VolumeBase.h"
#include "stdint.h"
#include "zlib.h"
#include "QMap"
#include "Layout.h"

namespace caret {

class NiftiMatrix : public LayoutType //so we don't have to qualify layouts
{
public:
    NiftiMatrix();    
    ~NiftiMatrix() { clearMatrix();}

    // Below are low level functions for operating on generic matrix files, currently used by Nifti

    // SECTION 1: matrix reader set up
    /*use functions below to setup layout first before reading a frame, so that the matrix reader "knows" how interpret/load/format the data into the internal matrix
      it's flexible, but the recommended usage is:
      1. setMatrixOffset(if non-zero)
      2. setDataType (the data type as it is stored on disk)
      3. setMatrixLayoutOnDisk();*/

    /// sets the offset of the start of the matrix in the file, in case we are reading from a file that has a header before the matrix.
    void setMatrixOffset(const int64_t &offsetin) throw (NiftiException) { this->matrixStartOffset = offsetin; }

    // the following two functions are nifti specific
    /// sets the matrix layout by deriving information from a Nifti1Header
    void setMatrixLayoutOnDisk(const Nifti1Header &headerIn);
    /// sets the matrix layout by deriving information from a Nifti2Header
    void setMatrixLayoutOnDisk(const Nifti2Header &headerIn);

    /// Gets the matrix's layout on disk, since this matrix class only operates on one file at a time, it affects both reads and writes
    void getMatrixLayoutOnDisk(LayoutType &layout);
    /// set the matrix layout on disk by handing it a predefined layout struct
    void setMatrixLayoutOnDisk(LayoutType &layout);
    /// get the layout On Disk
    void getMatrixLayoutOnDisk(std::vector<int64_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut,int64_t &frameLengthOut, int64_t &frameSizeOut );
    /// set the layout on Disk by handing it required params
    void setMatrixLayoutOnDisk(const std::vector<int64_t> &dimensionsIn, const int &componentDimensionsIn,const bool &needsSwappingIn );

    //after setting matrix layout, a frame may be read.

    void readFile(QFile &fileIn) throw (NiftiException);
    void readFile(gzFile fileIn) throw (NiftiException);
    void writeFile(QFile &fileOut) throw (NiftiException);
    void writeFile(gzFile fileOut) throw (NiftiException);

    //readMatrix(
    //readFrame(
    //writeMatrix(
    //writeFrame(
    
    //template <class retType> retType * convertFrame(int8_t *&bytes, int64_t &size) throw (NiftiException);
    void readVolume(QFile &fileIn, VolumeBase &vol) throw (NiftiException);
    void readVolume(gzFile fileIn, VolumeBase &vol) throw (NiftiException);
    void readVolume(VolumeBase &vol) throw (NiftiException);
    void convertBytes(char *&bytes, float *&byteOut, int64_t &size) throw (NiftiException);

    int8_t *allocateFrame();
    void writeVolume(QFile &fileOut, VolumeBase &vol) throw (NiftiException);
    void writeVolume(gzFile fileOut, VolumeBase &vol) throw (NiftiException);
    void writeVolume(VolumeBase &vol) throw (NiftiException);
    void convertFrame(float *&frameIn, char *&bytesOut, int64_t &size) throw (NiftiException);

    // For reading the entire Matrix
    /// sets the entire volume file Matrix, be sure to set the matrix layout before calling this function
    void setMatrix(float *matrixIn, const int64_t &matrixLengthIn) throw (NiftiException);
    /// gets the entire volume file Matrix
    void getMatrix(float *matrixOut) throw (NiftiException);

    // For reading a frame at a time from previously loaded matrix
    /// Sets the current frame for writing, doesn't load any data from disk, can hand in a frame pointer for speed, writes out previous frame to disk if needed
    void setFrame(const float *matrixIn, const int64_t &frameLengthIn, const int64_t &timeSlice = 0L, const int64_t &componentIndex=0L)  throw(NiftiException);
    /// Sets the current frame (for writing), doesn't load any data from disk, writes out previous frame to disk if needed
    void getFrame(float *frame, const int64_t &timeSlice = 0L,const int64_t &componentIndex=0L) throw (NiftiException);
    
    /// Volume read/write Functions
    /// get Volume
    void getVolume(VolumeBase &vol);
    /// Set VolumeFrame
    void setVolume(VolumeBase &vol);
    
    /// Gets the Size in bytes of the current frame
    int64_t getFrameSize() { return frameSize;}
    /// Gets the number of individual elements (array size) of the current frame
    int64_t getFrameLength() { return frameLength; }
    /// returns the number of frames
    int64_t getFrameCount() { return timeLength;/*time is the 4th dimension or number of frames*/ }

    /// Gets the Size in bytes of the entire volume matrix
    int64_t getMatrixSize() { return matrixLength*(this->valueByteSize());}
    /// Gets the number of individual elements (array size) of the entire volume matrix
    int64_t getMatrixLength() { return matrixLength; }

    void setUsingVolume(bool usingVolume) { m_usingVolume = usingVolume; }

   



private:
    /// Reads frame from disk into memory, flushes previous frame to disk if changes were made.
    void readFile()  throw (NiftiException);//for loading a frame at a time
    void readMatrixBytes(char *bytes, int64_t size, int64_t frameOffset = 0);
    /// Writes the current frame to disk.
    void writeFile() throw (NiftiException);
    void writeMatrixBytes(char *bytes, int64_t size, int64_t frameOffset = 0);


    bool isCompressed();
    void reAllocateMatrixIfNeeded();
    void clearMatrix();
    void init();
    
    //frames represent brain volumes on disk, the matrix is the internal storage for the matrix after it has been loaded from the file.
    int64_t calculateFrameLength(const std::vector<int64_t> &dimensionsIn) const;
    int64_t calculateFrameSizeInBytes(const int64_t &frameLengthIn, const int32_t &valueByteSizeIn) const;    
    
    QFile *file;
    gzFile zFile;
    int64_t matrixStartOffset;

    //layout
    int64_t frameLength;
    int64_t frameSize;    

    // matrix data
    int64_t currentTime;
    float *matrix;
    int64_t matrixLength;//length of array
    bool matrixLoaded;

	int64_t timeLength;
    bool m_usingVolume;
};

}
#endif // NIFTIMATRIX_H

