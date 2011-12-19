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

#include "NiftiMatrix.h"
#include "QFile"

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
    catch (...) {
        std::cout << "Exception reading matrix from file" << std::endl;
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

void NiftiMatrix::readMatrixBytes(char *bytes, int64_t size)
{
    if(isCompressed())
    {
        gzseek(zFile,matrixStartOffset, 0);
        gzread(zFile,bytes,size);
    }
    else
    {       
        file->seek(matrixStartOffset);
        file->read(bytes,size);
    }
}

void NiftiMatrix::writeMatrixBytes(char *bytes, int64_t size)
{
    if(isCompressed())
    {
        gzseek(zFile,matrixStartOffset, 0);
        gzwrite(zFile,bytes,size);
    }
    else
    {
        file->seek(matrixStartOffset);
        file->write(bytes,size);
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