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

NiftiMatrix::NiftiMatrix()
{
    init();
}

NiftiMatrix::NiftiMatrix(const QFile &filein)
{
    init();
    file.setFileName(filein.fileName());    
}

NiftiMatrix::NiftiMatrix(const QFile &filein, const int64_t &offsetin)
{
    init();
    file.setFileName(filein.fileName());
    matrixStartOffset = offsetin;
}

void NiftiMatrix::init()
{
    needsSwapping = false;
    matrixStartOffset = 0;
    matrix = NULL;
    matrixLength = 0;
    layoutSet = false;
    frameLoaded = false;
    frameNeedsWriting = false;
    currentTime = 0;
}

void NiftiMatrix::setMatrixFile(const QFile &filein){
    this->clearMatrix();
    init();
    file.setFileName(filein.fileName());
}

/*
void NiftiMatrix::getVolumeFrame(VolumeFile &volume, int64_t &timeSlice) throw (NiftiException)
{
    readFrame(timeSlice);
    int64_t index = 0;
    for(int32_t k=0;k<dimensions[3];k++)
        for(int32_t j=0;j<dimensions[2];j++)
            for(int32_t i=0;i<dimensions[1];i++)
            {
                translateVoxel(i,j,k,index);
                for(int c=0;c<componentDimensions;c++)
                    volume.setValue(matrix[index+c],i,j,k,timeSlice,c);

            }
}

void NiftiMatrix::setVolumeFrame(VolumeFile &volume, int64_t &timeSlice) throw (NiftiException)
{
    //flushCurrentFrame();
    int64_t index = 0;
    for(int32_t k=0;k<dimensions[3];k++)
        for(int32_t j=0;j<dimensions[2];j++)
            for(int32_t i=0;i<dimensions[1];i++)
            {
                translateVoxel(i,j,k,index);
                for(int c=0;c<componentDimensions;c++)
                    matrix[index+c]=volume.getValue(i,j,k,timeSlice,c);
            }
}*/

void NiftiMatrix::getMatrixLayoutOnDisk(LayoutType &layout)
{

    layout = (*this);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize(), componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::setMatrixLayoutOnDisk(LayoutType &layout)
{
    *(static_cast <LayoutType *> (this)) = layout;
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize(), componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::getMatrixLayoutOnDisk(std::vector<int64_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut, int64_t &frameLengthOut, int64_t &frameSizeOut )
{
    dimensionsOut = dimensions;
    componentDimensionsOut = componentDimensions;
    valueByteSizeOut = valueByteSize();
    needsSwappingOut = needsSwapping;
    frameLengthOut = calculateFrameLength(dimensions);
    frameSizeOut = calculateFrameSizeInBytes(frameLength, valueByteSize(), componentDimensions);
}

void NiftiMatrix::setMatrixLayoutOnDisk(const std::vector <int64_t> &dimensionsIn, const int &componentDimensionsIn, const int &valueByteSizeIn, const bool &needsSwappingIn )
{
    dimensions = dimensionsIn;
    componentDimensions = componentDimensionsIn;    
    needsSwapping = needsSwappingIn;
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize(), componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::setMatrixLayoutOnDisk(const Nifti1Header &headerIn)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    headerIn.getNiftiDataTypeEnum(this->niftiDataType);
    headerIn.getDimensions(this->dimensions);
    headerIn.getComponentDimensions(this->componentDimensions);
    headerIn.getNeedsSwapping(this->needsSwapping);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize(), componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::setMatrixLayoutOnDisk(const Nifti2Header &headerIn)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    headerIn.getNiftiDataTypeEnum(this->niftiDataType);
    headerIn.getDimensions(this->dimensions);
    headerIn.getComponentDimensions(this->componentDimensions);
    headerIn.getNeedsSwapping(this->needsSwapping);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize(), componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::clearMatrix()
{
    if(matrix) delete []matrix;
    matrixLength = 0;
}

void NiftiMatrix::resetMatrix()
{
    reAllocateMatrixIfNeeded();
    memset(matrix,matrixLength*sizeof(float),0x00);
}

void NiftiMatrix::reAllocateMatrixIfNeeded()
{
    if(matrixLength != calculateMatrixLength(frameLength,componentDimensions)) {
        if(matrix) delete []matrix;
        matrixLength = calculateMatrixLength(frameLength,componentDimensions);
        matrix = new float[matrixLength];
    }
    frameLoaded = false;
    frameNeedsWriting = false;
}

void NiftiMatrix::readFrame(int64_t timeSlice) throw (NiftiException)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    if(frameLoaded && timeSlice==currentTime) return;
    flushCurrentFrame();
    int64_t frameOffset = matrixStartOffset+frameSize*timeSlice;
    int8_t *bytes = NULL;
    bytes = new int8_t[frameSize];
    if(!bytes) {
        throw NiftiException("There was an error allocating memory for reading.");
        return;
    }
    try {
    file.open(QIODevice::ReadOnly);
    file.seek(frameOffset);
    file.read((char *)bytes,frameSize);
    file.close();
    }
    catch (...) {
        std::cout << "Exception reading from:" << file.fileName() << std::endl;
        //std::cout << file.FileError << std::endl;
    }
    //convert to floats
    //for the special case of RGB, we convert each byte in an RGB array to floats so that we can use the same float matrix for all other functions

    currentTime = timeSlice;
    //convert matrices to floats
    reAllocateMatrixIfNeeded();
    switch ((NiftiDataTypeEnum::toIntegerCode(niftiDataType))) {
    case NIFTI_TYPE_FLOAT32:
        if(needsSwapping) ByteSwapping::swapBytes((float *)bytes,frameLength);
        memcpy((void *)matrix,(void *)bytes, frameSize);
        break;
    case NIFTI_TYPE_FLOAT64:
        if(needsSwapping) ByteSwapping::swapBytes((double *)bytes, frameLength);
        for(int i=0;i<matrixLength;i++) matrix[i] = ((double *)bytes)[i];
        break;
    case NIFTI_TYPE_RGB24:
        for(int i=0;i<matrixLength;i++) matrix[i] = bytes[i];
        break;
    default:
        throw NiftiException("Unrecognized Nifti Data found when reading frame.");
        break;
    }
    //cleanup
    frameLoaded = true;
    delete [] bytes;
}

void NiftiMatrix::flushCurrentFrame()
{
    if(!frameLoaded) return;
    if(!frameNeedsWriting) return;
    this->writeFrame();
}

//for in place editing of files, we need to respect the original layout
void NiftiMatrix::writeFrame() throw (NiftiException)
{
    if(!frameLoaded) throw NiftiException("Writeframe is called but frame isn't loaded.");
    if(!frameNeedsWriting) return;
    int64_t frameOffset = matrixStartOffset+frameSize*this->currentTime;
    file.open(QIODevice::ReadWrite);
    file.seek(frameOffset);

    switch(NiftiDataTypeEnum::toIntegerCode(this->niftiDataType))
    {
    case NIFTI_TYPE_FLOAT32:
        if(needsSwapping) ByteSwapping::swapBytes(matrix,frameLength);

        file.write((char *)matrix,frameSize);
        if(needsSwapping) ByteSwapping::swapBytes(matrix,frameLength);
        break;
    case NIFTI_TYPE_FLOAT64:
    {
        double *outMatrix = new double[frameLength];
        for(int i = 0;i<frameLength;i++) outMatrix[i]=matrix[i];
        if(needsSwapping) ByteSwapping::swapBytes(outMatrix, frameLength);

        file.write((char *)outMatrix, frameLength*sizeof(double));
        delete [] outMatrix;
    }
        break;
    case NIFTI_TYPE_RGB24:
    {
        int8_t *bytes = NULL;
        bytes = new int8_t [frameSize];
        for(int i=0;i<frameLength;i++)
        {
            bytes[i]=matrix[i];
        }
        file.write((char *)bytes,frameLength);
        delete [] bytes;
    }
        break;
    default:
        throw NiftiException("Unsupported Data type for writes.");
        break;
    }
    file.close();

}

void NiftiMatrix::setFrame(float *matrixIn, const int64_t &matrixLengthIn, const int64_t &timeSlice) throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    flushCurrentFrame();
    this->resetMatrix();
    if(this->matrixLength != matrixLengthIn) throw NiftiException("frame size does not match expected frame size!");
    memcpy(matrix,matrixIn,matrixLength*sizeof(float));
    currentTime = timeSlice;
    frameLoaded = true;
    frameNeedsWriting = true;
}

void NiftiMatrix::getFrame(float *frameOut) throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    if(!frameLoaded) throw NiftiException("Please load frame before getting component.");
    //copy data to output frame
    //TODO change this so that it will work when the matrix contains more than a single frame...
    memcpy((char *)frameOut,(char *)matrix,this->matrixLength*sizeof(float));
}

void NiftiMatrix::setFrame(const int64_t &timeSlice)  throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    flushCurrentFrame();
    this->resetMatrix();

    currentTime = timeSlice;
    frameLoaded = true;
    frameNeedsWriting = true;
}

float NiftiMatrix::getComponent(const int64_t &index, const int64_t &componentIndex) const throw (NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    if(!frameLoaded) throw NiftiException("Please load frame before getting component.");
    if((componentIndex+1)>componentDimensions) throw NiftiException("ComponentIndex exceeds the number of compoents for this nifti Matrix.");

    return matrix[index*componentDimensions+componentIndex];
}

void NiftiMatrix::translateVoxel(const int64_t &i, const int64_t &j, const int64_t &k, int64_t &index) const
{
    index = i+(j+(k*dimensions[2]))*dimensions[1];
}

void NiftiMatrix::setComponent(const int64_t &index, const int64_t &componentIndex, const float &value) throw (NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    reAllocateMatrixIfNeeded();
    if((componentIndex+1)>componentDimensions) throw NiftiException("ComponentIndex exceeds the number of components for this nifti Matrix.");
    matrix[index*componentDimensions+componentIndex] = value;
    frameLoaded = true;
    frameNeedsWriting = true;
}

int64_t NiftiMatrix::calculateFrameLength(const std::vector<int64_t> &dimensionsIn) const
{
    int64_t frameLength = 1;
    for(int i=0;i<MATRIX_FRAME_DIMENSIONS;i++)
        frameLength*=dimensionsIn[i];
    return frameLength;
}

int64_t NiftiMatrix::calculateFrameSizeInBytes(const int64_t &frameLengthIn, const int32_t &valueByteSizeIn, const int64_t &componentDimensionsIn) const
{
        return frameLengthIn*valueByteSizeIn*componentDimensionsIn;
}

int64_t NiftiMatrix::calculateMatrixLength(const int64_t &frameLengthIn, const int64_t &componentDimensionsIn) const
{
    return frameLengthIn*componentDimensionsIn;
}

int64_t NiftiMatrix::calculateMatrixSizeInBytes(const int64_t &frameSizeIn, const int64_t &componentDimensionsIn) const
{
    return frameSizeIn*componentDimensionsIn;
}


void NiftiMatrix::getVolumeFrame(VolumeFile &frameOut, const int64_t timeSlice, const int64_t component)
{
    readFrame(timeSlice);

    //TODO ignoring component for now
    frameOut.setFrame(this->matrix,timeSlice,component);

}

void NiftiMatrix::setVolumeFrame(VolumeFile &frameIn, const int64_t & timeSlice, const int64_t component)
{
    //TODO, this is evil, but fast, as I'm handing it the internal matrix
    setFrame(timeSlice);
    frameIn.getFrame(this->matrix,timeSlice, component);
}

