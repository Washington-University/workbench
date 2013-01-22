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


#ifdef CARET_OS_WINDOWS
#include <io.h>
//#define SSIZE_MAX 32767
#define SSIZE_MAX 2147483647
#else //not CARET_OS_WINDOWS
#include <unistd.h>
#endif //ifdef CARET_OS_WINDOWS

/*
 * Mac does not seem to have off64_t
 * Apparently, off_t is 64-bit on mac: http://sourceforge.net/mailarchive/forum.php?set=custom&viewmonth=&viewday=&forum_name=stlport-devel&style=nested&max_rows=75&submit=Change+View
 */
#ifdef CARET_OS_MACOSX
#define off64_t off_t
#endif // CARET_OS_MACOSX

#ifndef _LARGEFILE64_SOURCE 
#define _LARGEFILE64_SOURCE
#define _LFS64_LARGEFILE 1
#endif

#include "NiftiMatrix.h"
#include "QFile"
#include <limits>
using namespace std;

using namespace caret;

//Matrix Function definitions

NiftiMatrix::NiftiMatrix()
{
    init();
}

void NiftiMatrix::init()
{
    needsSwapping = false;
    matrixStartOffset = 0;
    matrix = NULL;
    matrixLength = 0;
    layoutSet = false;    
    currentTime = 0;
    file = NULL;
    zFile = NULL;
    timeLength = 0;
    m_usingVolume = false;
}

bool NiftiMatrix::isCompressed()
{
    if(file) return false;
    else if(zFile) return true;
    else return false;
}

void NiftiMatrix::getMatrixLayoutOnDisk(LayoutType &layout)
{

    layout = (*this);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize());
    timeLength = dimensions.size()>3?dimensions[3]:1;
    layoutSet = true;
    reAllocateMatrixIfNeeded();
}

void NiftiMatrix::setMatrixLayoutOnDisk(LayoutType &layout)
{
    *(static_cast <LayoutType *> (this)) = layout;
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize());
    timeLength = dimensions.size()>3?dimensions[3]:1;
    layoutSet = true;
    reAllocateMatrixIfNeeded();
}

void NiftiMatrix::getMatrixLayoutOnDisk(std::vector<int64_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut, int64_t &frameLengthOut, int64_t &frameSizeOut )
{
    dimensionsOut = dimensions;
    componentDimensionsOut = componentDimensions;
    valueByteSizeOut = valueByteSize();
    needsSwappingOut = needsSwapping;
    frameLengthOut = calculateFrameLength(dimensions);
    frameSizeOut = calculateFrameSizeInBytes(frameLength, valueByteSize());
    timeLength = dimensions.size()>3?dimensions[3]:1;
    reAllocateMatrixIfNeeded();
}

void NiftiMatrix::setMatrixLayoutOnDisk(const std::vector <int64_t> &dimensionsIn, const int &componentDimensionsIn, const bool &needsSwappingIn )
{
    dimensions = dimensionsIn;
    componentDimensions = componentDimensionsIn;
    needsSwapping = needsSwappingIn;
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize());
    timeLength = dimensions.size()>3?dimensions[3]:1;
    layoutSet = true;
    reAllocateMatrixIfNeeded();
}

void NiftiMatrix::setMatrixLayoutOnDisk(const Nifti1Header &headerIn)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    headerIn.getNiftiDataTypeEnum(this->niftiDataType);
    headerIn.getDimensions(this->dimensions);
    headerIn.getComponentDimensions(this->componentDimensions);
    headerIn.getNeedsSwapping(this->needsSwapping);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize());
    headerIn.getScaling(sclSlope, sclIntercept);
    timeLength = dimensions.size()>3?dimensions[3]:1;
    layoutSet = true;
    reAllocateMatrixIfNeeded();
}

void NiftiMatrix::setMatrixLayoutOnDisk(const Nifti2Header &headerIn)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    headerIn.getNiftiDataTypeEnum(this->niftiDataType);
    headerIn.getDimensions(this->dimensions);
    headerIn.getComponentDimensions(this->componentDimensions);
    headerIn.getNeedsSwapping(this->needsSwapping);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize());
    headerIn.getScaling(sclSlope, sclIntercept);
    timeLength = dimensions.size()>3?dimensions[3]:1;
    layoutSet = true;
    reAllocateMatrixIfNeeded();
}

void NiftiMatrix::clearMatrix()
{
    if(matrix) delete []matrix;
    matrixLength = 0;
}

void NiftiMatrix::reAllocateMatrixIfNeeded()
{
    if(this->m_usingVolume) return;
    
    if(matrixLength != frameLength*componentDimensions*timeLength) {
        if(matrix) delete []matrix;
        matrixLength = frameLength*componentDimensions*timeLength;
        matrix = new float[matrixLength];
    }    
}

void NiftiMatrix::readFile(QFile &fileIn) throw (NiftiException)
{
    file = &fileIn;
    zFile = NULL;
    readFile();
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::readFile(gzFile fileIn) throw (NiftiException)
{
    file = NULL;
    zFile = fileIn;
    readFile();
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::writeFile(QFile &fileOut) throw (NiftiException)
{
    file = &fileOut;
    zFile = NULL;
    writeFile();
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::writeFile(gzFile fileOut) throw (NiftiException)
{
    file = NULL;
    zFile = fileOut;
    writeFile();
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::readFile() throw (NiftiException)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    uint64_t matrixSize = frameSize*timeLength*componentDimensions;
    int8_t *bytes = NULL;
    bytes = new int8_t[matrixSize];
    if(!bytes) {
        throw NiftiException("There was an error allocating memory for reading.");
        return;
    }
    try {
        readMatrixBytes((char *)bytes,matrixSize);
    }
    catch (NiftiException& e)
    {
        throw e;
    }
    catch (...) {
        throw NiftiException("Exception reading matrix from file");
        //std::cout << file.FileError << std::endl;
    }
    //convert to floats
    //for the special case of RGB, we convert each byte in an RGB array to floats so that we can use the same float matrix for all other functions

    //convert matrices to floats
    reAllocateMatrixIfNeeded();
    switch ((NiftiDataTypeEnum::toIntegerCode(niftiDataType))) {
    case NIFTI_TYPE_FLOAT32:
        if(needsSwapping) ByteSwapping::swapBytes((float *)bytes,matrixLength);
        memcpy((void *)matrix,(void *)bytes, matrixSize);
        break;
    case NIFTI_TYPE_FLOAT64:
        if(needsSwapping) ByteSwapping::swapBytes((double *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((double *)bytes)[i];
        break;
    case NIFTI_TYPE_RGB24:
    {
        if(matrixLength%3) throw NiftiException("Reading RGB type but frame bytes aren't divisible by 3.");
        int64_t GFrameOffset = frameLength;
        int64_t BFrameOffset = frameLength*2;
        for(int t = 0;t<dimensions[3];t++)
        {        
            for(int i=frameLength*t;i<frameLength*(t+1);i+=3)
            {
                matrix[i] = bytes[i];
                matrix[GFrameOffset+i]=bytes[i+1];
                matrix[BFrameOffset+i]=bytes[i+2];
            }
        }
    }
        break;
    case NIFTI_TYPE_INT8:
        for(int i=0;i<matrixLength;i++) matrix[i] = ((int8_t *)bytes)[i];
        break;
    case NIFTI_TYPE_INT16:
        if(needsSwapping) ByteSwapping::swapBytes((int16_t *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((int16_t *)bytes)[i];
        break;
    case NIFTI_TYPE_INT32:
        if(needsSwapping) ByteSwapping::swapBytes((int32_t *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((int32_t *)bytes)[i];
        break;
    case NIFTI_TYPE_INT64:
        if(needsSwapping) ByteSwapping::swapBytes((int64_t *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((int64_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT8:
        for(int i=0;i<matrixLength;i++) matrix[i] = ((uint8_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT16:
        if(needsSwapping) ByteSwapping::swapBytes((uint16_t *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((uint16_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT32:
        if(needsSwapping) ByteSwapping::swapBytes((uint32_t *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((uint32_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT64:
        if(needsSwapping) ByteSwapping::swapBytes((uint64_t *)bytes, matrixLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((uint64_t *)bytes)[i];
        break;
    default:
        throw NiftiException("Unrecognized Nifti Data Type found when reading frame.");
        break;
    }
    //apply scaling
    if(sclSlope != 0.0)
    {
        for(int64_t i =0;i<matrixLength;i++)
        {
            matrix[i] = sclSlope*matrix[i]+sclIntercept;
        }
    }

    //cleanup
    matrixLoaded = true;
    
    delete [] bytes;
}

/* WARNING!!!
   The function below currently canot seek when reading gz files, one must start at the beginning and
   read a frame at a time.  This will hopefully be fixed by updating QT on windows.
*/
void NiftiMatrix::readMatrixBytes(char *bytes, int64_t size, int64_t frameOffset)
{
    if(isCompressed())
    {
        /*if((frameOffset+matrixStartOffset)!= gztell64(zFile))
        {
            int64_t bytes_seeked = gzseek64(zFile,matrixStartOffset+frameOffset, 0);
            if ( bytes_seeked != (matrixStartOffset+frameOffset))
            {
                throw NiftiException("failed to seek in file");
            }
        }*/
        if(frameOffset==0)
        {
#ifdef CARET_OS_MACOSX
            int64_t bytes_seeked = gzseek(zFile,matrixStartOffset+frameOffset, 0);
#elif ZLIB_VERNUM > 0x1232
        int64_t bytes_seeked = gzseek64(zFile,matrixStartOffset+frameOffset, 0);
#else // ZLIB_VERNUM > 0x1232
        int64_t bytes_seeked = gzseek(zFile,matrixStartOffset+frameOffset, 0);
#endif // ZLIB_VERNUM > 0x1232
            if ( bytes_seeked != (matrixStartOffset+frameOffset))
            {
                throw NiftiException("failed to seek in file");
            }
        }
        int64_t chunk_size = size;
        if (chunk_size > numeric_limits<int>::max())
        {
            chunk_size = numeric_limits<int>::max();
        }
        int64_t total = 0;
        while (total < size)
        {
            int64_t request = min(chunk_size, size - total);
            int64_t ret = gzread(zFile,bytes + total,request);
            if (ret < 1)
            {
                throw NiftiException("failed to read bytes");
            }
            total += ret;
        }
        
    }
    else
    {       
        //file->seek(matrixStartOffset);
        //file->read(bytes,size);
        //QT can't read files over a certain size
        int fh = file->handle();
        if(frameOffset == 0)
        {

#ifdef CARET_OS_WINDOWS
            if (_lseeki64(fh,matrixStartOffset+frameOffset,0) != (matrixStartOffset+frameOffset))
#else
#ifdef CARET_OS_LINUX
            if (lseek64(fh,matrixStartOffset+frameOffset,0) != (matrixStartOffset+frameOffset))
#else //Fallback to lseek for MAC which doesn't have lseek64, and others
            if (lseek(fh,matrixStartOffset+frameOffset,0) != (matrixStartOffset+frameOffset))
#endif //CARET_OS_LINUX
#endif //CARET_OS_WINDOWS


            {
                throw NiftiException("failed to seek in file");
            }
        }
        int64_t chunk_size = size;
        if (chunk_size > SSIZE_MAX)
        {
            chunk_size = SSIZE_MAX;
        }
        int64_t total = 0;
        while (total < size)
        {
            int64_t request = min(chunk_size, size - total);
            int64_t ret = read(fh,bytes + total,request);
            if (ret < 1)
            {
                throw NiftiException("failed to read bytes");
            }
            total += ret;
        }
    }
}

/* WARNING!!!
   The function below currently canot seek when writing gz files, one must start at the beginning and
   read a frame at a time.  This will hopefully be fixed by updating QT on windows.
*/
void NiftiMatrix::writeMatrixBytes(char *bytes, int64_t size,int64_t frameOffset)
{
    if(isCompressed())
    {
        /*if((frameOffset+matrixStartOffset)!= gztell64(zFile))
        {
            int64_t bytes_seeked = gzseek64(zFile,matrixStartOffset+frameOffset, 0);
            if ( bytes_seeked != (matrixStartOffset+frameOffset))
            {
                throw NiftiException("failed to seek in file");
            }
        }*/
        if(frameOffset==0)
        {
#ifdef CARET_OS_MACOSX
            int64_t bytes_seeked = gzseek(zFile,matrixStartOffset+frameOffset, 0);
#elif ZLIB_VERNUM > 0x1232
            int64_t bytes_seeked = gzseek64(zFile,matrixStartOffset+frameOffset, 0);
#else // ZLIB_VERNUM > 0x1232
            int64_t bytes_seeked = gzseek(zFile,matrixStartOffset+frameOffset, 0);
#endif // ZLIB_VERNUM > 0x1232
            if ( bytes_seeked != (matrixStartOffset+frameOffset))
            {
                throw NiftiException("failed to seek in file");
            }
        }        
        int64_t chunk_size = size;
        if (chunk_size > numeric_limits<int>::max())
        {
            chunk_size = numeric_limits<int>::max();
        }
        int64_t total = 0;
        while (total < size)
        {
            int64_t request = min(chunk_size, size - total);
            int64_t ret = gzwrite(zFile,bytes + total,request);
            if (ret < 1)
            {
                throw NiftiException("failed to write bytes");
            }
            total += ret;
        }
    }
    else
    {
        //file->seek(matrixStartOffset);
        //file->write(bytes,size);
        file->flush();//need to make QFile write its buffer to the handle before we use the handle, or else its buffered data can end up AFTER the matrix data, for unexpected results
        int fh = file->handle();
        if(frameOffset == 0)
        {
#ifdef CARET_OS_WINDOWS
            if (_lseeki64(fh,matrixStartOffset+frameOffset,0) != (matrixStartOffset+frameOffset))
#else
#ifdef CARET_OS_LINUX
            if (lseek64(fh,matrixStartOffset+frameOffset,0) != (matrixStartOffset+frameOffset))
#else //Fallback to lseek for MAC which doesn't have lseek64, and others
            if (lseek(fh,matrixStartOffset+frameOffset,0) != (matrixStartOffset+frameOffset))
#endif //CARET_OS_LINUX
#endif //CARET_OS_WINDOWS
        {
            throw NiftiException("failed to seek in file");
        }
        }
        int64_t chunk_size = size;
        if (chunk_size > SSIZE_MAX)
        {
            chunk_size = SSIZE_MAX;
        }

        int64_t total = 0;
        while (total < size)
        {
            int64_t request = min(chunk_size, size - total);
            int64_t ret = write(fh,bytes + total,request);
            if (ret < 1)
            {
                throw NiftiException("failed to write bytes");
            }
            total += ret;
        }
    }
}

//for in place editing of files, we need to respect the original layout
void NiftiMatrix::writeFile() throw (NiftiException)
{
    uint64_t matrixSize = frameSize*componentDimensions*timeLength;
    //remove scaling, TODO, make a copy of frame to avoid round off errors when we
    //reapply scaling at the end of this function
    if(sclSlope != 0.0)
    {
        for(int64_t i =0;i<matrixLength;i++)
        {
            matrix[i] = (matrix[i]-sclIntercept)/sclSlope;
        }
    }

    switch(NiftiDataTypeEnum::toIntegerCode(this->niftiDataType))
    {
    case NIFTI_TYPE_FLOAT32:
        if(needsSwapping) ByteSwapping::swapBytes(matrix,matrixLength);
        writeMatrixBytes((char *)matrix,matrixSize);
        if(needsSwapping) ByteSwapping::swapBytes(matrix,matrixLength);
        break;
    case NIFTI_TYPE_FLOAT64:
    {
        double *outMatrix = new double[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(double));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_RGB24:
    {
        if(matrixLength%3) throw NiftiException("Reading RGB type but frame bytes aren't divisible by 3.");
        int64_t GFrameOffset = frameLength;
        int64_t BFrameOffset = frameLength*2;
        uint8_t *bytes = NULL;
        int64_t RGBOutFrameLength = frameLength*3;
        bytes = new uint8_t [RGBOutFrameLength];
        for(int t=0;t<dimensions[3];t++)
        {
            for(int i=t*RGBOutFrameLength;i<(t+1)*RGBOutFrameLength;i+=3)
            {
                bytes[i]=matrix[i];
                bytes[i+1]=matrix[GFrameOffset+i];
                bytes[i+2]=matrix[BFrameOffset+i];
            }
        }
        writeMatrixBytes((char *)bytes,matrixSize);
        delete [] bytes;
    }
        break;
    case NIFTI_TYPE_INT8:
        writeMatrixBytes((char *)matrix, matrixLength);
        break;
    case NIFTI_TYPE_INT16:
    {
        int16_t *outMatrix = new int16_t[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(int16_t));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_INT32:
    {
        int32_t *outMatrix = new int32_t[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(int32_t));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_INT64:
    {
        int64_t *outMatrix = new int64_t[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(int64_t));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_UINT8:
        writeMatrixBytes((char *)matrix, matrixLength);
        break;
    case NIFTI_TYPE_UINT16:
    {
        uint16_t *outMatrix = new uint16_t[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(int16_t));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_UINT32:
    {
        uint32_t *outMatrix = new uint32_t[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(uint32_t));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_UINT64:
    {
        uint64_t *outMatrix = new uint64_t[matrixLength];
        for(int i = 0;i<matrixLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, matrixLength);

        writeMatrixBytes((char *)outMatrix, matrixLength*sizeof(uint64_t));
        delete [] outMatrix;
    }
        break;
    default:
        throw NiftiException("Unsupported Data type for writes.");
        break;
    }    
    //reapply scaling
    if(sclSlope != 0.0)
    {
        for(int64_t i =0;i<matrixLength;i++)
        {
            matrix[i] = sclSlope*matrix[i]+sclIntercept;
        }
    }   
}

void NiftiMatrix::setFrame(const float *matrixIn, const int64_t &frameLengthIn, const int64_t &timeSlice, const int64_t &componentIndex) throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    if(componentIndex>(componentDimensions-1)) throw NiftiException("Component index exceeds the size of component dimensions.");
    
    if(this->frameLength != frameLengthIn) throw NiftiException("frame size does not match expected frame size!");
    
    char * tempFrame = (char *)&matrix[componentIndex*frameLength+frameLength*timeSlice*componentDimensions];//for RGB format
    memcpy(tempFrame,matrixIn,frameLength*sizeof(float));
}

void NiftiMatrix::getFrame(float *frameOut, const int64_t &timeSlice, const int64_t &componentIndex) throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    if(!matrixLoaded) throw NiftiException("Please load frame before getting component.");
    if(componentIndex>(componentDimensions-1)) throw NiftiException("Component index exceeds the size of component dimensions.");
    //copy data to output frame    
    char * tempFrame = (char *)&matrix[componentIndex*frameLength+frameLength*timeSlice*componentDimensions];//for RGB format
    memcpy((char *)frameOut,(char *)tempFrame,this->frameLength*sizeof(float));
}

int64_t NiftiMatrix::calculateFrameLength(const std::vector<int64_t> &dimensionsIn) const
{
    int64_t frameLength = 1;
    for(int i=0;i<MATRIX_FRAME_DIMENSIONS;i++)
        frameLength*=dimensionsIn[i];
    return frameLength;
}

int64_t NiftiMatrix::calculateFrameSizeInBytes(const int64_t &frameLengthIn, const int32_t &valueByteSizeIn) const
{
    return frameLengthIn*valueByteSizeIn;
}


//
// The functions below are specifically for reading/writing using the workbench volumefile class.
// 

void NiftiMatrix::getVolume(VolumeBase &vol)
{
    float *frame= new float [this->frameLength];
    for(int t=0;t<timeLength;t++)
    {
        for(int i=0;i<componentDimensions;i++)
        {
            this->getFrame(frame,t,i);
            vol.setFrame(frame,t,i);
        }
    }	
    delete [] frame;
}

void NiftiMatrix::setVolume(VolumeBase &vol)
{
    for(int t=0;t<timeLength;t++)
    {
        for(int i=0;i<componentDimensions;i++)
        {
            this->setFrame(vol.getFrame(t,i),frameLength,t,i);
        }
    }
}

void NiftiMatrix::convertBytes(char *&bytes, float *&frameOut, int64_t &size) throw (NiftiException)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    //convert to floats
    //for the special case of RGB, we convert each byte in an RGB array to floats so that we can use the same float matrix for all other functions

    //convert matrices to floats
    
    switch ((NiftiDataTypeEnum::toIntegerCode(niftiDataType))) {
    case NIFTI_TYPE_FLOAT32:
        if(needsSwapping) ByteSwapping::swapBytes((float *)bytes,frameLength);
        memcpy((void *)frameOut,(void *)bytes, size);
        break;
    case NIFTI_TYPE_FLOAT64:
        if(needsSwapping) ByteSwapping::swapBytes((double *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((double *)bytes)[i];
        break;
    case NIFTI_TYPE_RGB24:
    {
        if(frameLength%3) throw NiftiException("Reading RGB type but frame bytes aren't divisible by 3.");
        int64_t GFrameOffset = frameLength;
        int64_t BFrameOffset = frameLength*2;
                
        for(int i=0;i<frameLength*3;i+=3)
        {
            frameOut[i] = bytes[i];
            frameOut[GFrameOffset+i]=bytes[i+1];
            frameOut[BFrameOffset+i]=bytes[i+2];
        }
        
    }
        break;
    case NIFTI_TYPE_INT8:
        for(int i=0;i<frameLength;i++) frameOut[i] = ((int8_t *)bytes)[i];
        break;
    case NIFTI_TYPE_INT16:
        if(needsSwapping) ByteSwapping::swapBytes((int16_t *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((int16_t *)bytes)[i];
        break;
    case NIFTI_TYPE_INT32:
        if(needsSwapping) ByteSwapping::swapBytes((int32_t *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((int32_t *)bytes)[i];
        break;
    case NIFTI_TYPE_INT64:
        if(needsSwapping) ByteSwapping::swapBytes((int64_t *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((int64_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT8:
        for(int i=0;i<frameLength;i++) frameOut[i] = ((uint8_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT16:
        if(needsSwapping) ByteSwapping::swapBytes((uint16_t *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((uint16_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT32:
        if(needsSwapping) ByteSwapping::swapBytes((uint32_t *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((uint32_t *)bytes)[i];
        break;
    case NIFTI_TYPE_UINT64:
        if(needsSwapping) ByteSwapping::swapBytes((uint64_t *)bytes, frameLength);
        for(int i=0;i<frameLength;i++) frameOut[i] = ((uint64_t *)bytes)[i];
        break;
    default:
        throw NiftiException("Unrecognized Nifti Data Type found when reading frame.");
        break;
    }
    //apply scaling
    if(sclSlope != 0.0)
    {
        for(int64_t i =0;i<frameLength;i++)
        {
            frameOut[i] = sclSlope*frameOut[i]+sclIntercept;
        }
    }     
}

void NiftiMatrix::readVolume(QFile &fileIn, VolumeBase &vol) throw (NiftiException)
{
    file = &fileIn;
    zFile = NULL;
    readVolume(vol);
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::readVolume(gzFile fileIn, VolumeBase &vol) throw (NiftiException)
{
    file = NULL;
    zFile = fileIn;
    readVolume(vol);
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::readVolume(VolumeBase &vol) throw (NiftiException)
{
    m_usingVolume = true;
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    int64_t size = frameSize*componentDimensions;
    char *bytes = NULL;
    bytes = new char[size];
    float *frame = NULL;
    frame = new float[frameLength*componentDimensions];
    if(!bytes) {
        throw NiftiException("There was an error allocating memory for reading.");
        return;
    }

    for(int t = 0;t<this->timeLength;t++)
    {
        try {
            readMatrixBytes(bytes,size,t*size);
        } catch (NiftiException& e) {
            throw e;
        } catch (...) {
            throw NiftiException("Exception reading matrix from file");
        }
        convertBytes(bytes, frame, size);
        
        for(int i=0;i<componentDimensions;i++)
        {
            //vol.setFrame(frame,
            vol.setFrame(&frame[frameLength*i],t,i);
            //this->setFrame(vol.getFrame(t,i),frameLength,t,i);
        }
    }

    delete [] bytes;
    delete [] frame;
}

/*void NiftiMatrix::writeMatrixBytes(char *bytes, int64_t size,int64_t frameOffset = 0)
{
    if(isCompressed())
    {
        gzseek(zFile,matrixStartOffset+frameOffset, 0);
        gzwrite(zFile,bytes,size);
    }
    else
    {
        file->seek(matrixStartOffset+frameOffset);
        file->write(bytes,size);
    }
}*/

void NiftiMatrix::convertFrame(float *&frameIn, char *&bytesOut, int64_t &size) throw (NiftiException)
{
    if(sclSlope != 0.0)
    {
        for(int64_t i = 0;i<frameLength;i++)
        {
            frameIn[i] = (frameIn[i]-sclIntercept)/sclSlope;
        }
    }
    
    switch(NiftiDataTypeEnum::toIntegerCode(this->niftiDataType))
    {
    case NIFTI_TYPE_FLOAT32:
    {
        memcpy(bytesOut,(char *)frameIn,size);
        float *frameOut = (float *)bytesOut;        
        if(needsSwapping) ByteSwapping::swapBytes(frameOut,frameLength);
    }
        break;
    case NIFTI_TYPE_FLOAT64:
    {
        double *frameOut = (double *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);    
    }
        break;
    case NIFTI_TYPE_RGB24:
    {
        if(size%3) throw NiftiException("Reading RGB type but frame bytes aren't divisible by 3.");
        int64_t GFrameOffset = frameLength;
        int64_t BFrameOffset = frameLength*2;        
        int64_t RGBOutFrameLength = frameLength*3;        
        for(int i=0;i<RGBOutFrameLength;i+=3)
        {
            bytesOut[i]=frameIn[i];
            bytesOut[i+1]=frameIn[GFrameOffset+i];
            bytesOut[i+2]=frameIn[BFrameOffset+i];
        }
    }
        break;
    case NIFTI_TYPE_INT8:
    {
        int8_t *frameOut = (int8_t *)bytesOut;
        for(int i=0;i<frameLength;i++) frameOut[i]=frameIn[i];
    }
        break;
    case NIFTI_TYPE_INT16:
    {
        int16_t *frameOut = (int16_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);
    }
        break;
    case NIFTI_TYPE_INT32:
    {
        int32_t *frameOut = (int32_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);
    }
        break;
    case NIFTI_TYPE_INT64:
    {
        int64_t *frameOut = (int64_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);
    }
        break;
    case NIFTI_TYPE_UINT8:   
    {
        uint8_t *frameOut = (uint8_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
    }
        break;
    case NIFTI_TYPE_UINT16:
    {
        uint16_t *frameOut = (uint16_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);
    }
        break;
    case NIFTI_TYPE_UINT32:
    {
        uint32_t *frameOut = (uint32_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);
    }
        break;
    case NIFTI_TYPE_UINT64:
    {
        uint64_t *frameOut = (uint64_t *)bytesOut;
        for(int i = 0;i<frameLength;i++) frameOut[i]=frameIn[i];
        if(needsSwapping) ByteSwapping::swapBytes(frameOut, frameLength);
    }
        break;
    default:
        throw NiftiException("Unsupported Data type for writes.");
        break;
    }

    //reapply scaling
    if(sclSlope != 0.0)
    {
        for(int64_t i =0;i<frameLength;i++)
        {
            frameIn[i] = sclSlope*frameIn[i]+sclIntercept;
        }
    }
}

void NiftiMatrix::writeVolume(QFile &fileOut, VolumeBase &vol) throw (NiftiException)
{
    file = &fileOut;
    zFile = NULL;
    writeVolume(vol);
    file = NULL;
    zFile = NULL;
}

void NiftiMatrix::writeVolume(gzFile fileOut, VolumeBase &vol) throw (NiftiException)
{
    file = NULL;
    zFile = fileOut;
    writeVolume(vol);
    file = NULL;
    zFile = NULL;
}

int8_t * NiftiMatrix::allocateFrame()
{
    int8_t *frame = NULL;
    int64_t size = frameLength*componentDimensions;
    switch(NiftiDataTypeEnum::toIntegerCode(this->niftiDataType))
    {
    case NIFTI_TYPE_INT8:
    case NIFTI_TYPE_UINT8:
    case NIFTI_TYPE_RGB24:
        frame = new int8_t[size];
        break;
    case NIFTI_TYPE_INT16:    
    case NIFTI_TYPE_UINT16:
        frame = new int8_t[size*2];
        break;
    case NIFTI_TYPE_FLOAT32:
    case NIFTI_TYPE_INT32:
    case NIFTI_TYPE_UINT32:
        frame = new int8_t[size*4];
        break;
    case NIFTI_TYPE_FLOAT64:
    case NIFTI_TYPE_INT64:    
    case NIFTI_TYPE_UINT64:
        frame = new int8_t[size*8];
        break;
    default:
        throw NiftiException("Unsupported Data type for writes.");
        break;
    }
    return frame;
}
//for in place editing of files, we need to respect the original layout
void NiftiMatrix::writeVolume(VolumeBase &vol) throw (NiftiException)
{
    m_usingVolume = true;
    int64_t size = frameSize*componentDimensions;
    //remove scaling, TODO, make a copy of frame to avoid round off errors when we
    //reapply scaling at the end of this function
    char *frameOut = (char *)this->allocateFrame();
    float * frameIn = new float[frameLength*componentDimensions];
    for(int t = 0;t<this->timeLength;t++)
    {
        
        for(int i = 0;i<componentDimensions;i++)
        {
            const float * frame = vol.getFrame(t,i);
            memcpy(&frameIn[i*frameLength],frame,frameSize);
        }
        convertFrame(frameIn,frameOut,size);
        writeMatrixBytes((char *)frameOut,size, t*size);
    }
    delete [] frameOut;
    delete [] frameIn;
   
}

