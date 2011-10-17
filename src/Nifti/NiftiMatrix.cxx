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

/*NiftiMatrix::NiftiMatrix(const AString &filename) throw (NiftiException)
{
    if(!QFile::exists(filename)) throw NiftiException("Need a valid Nifti file to read/write to!");
    file.setFileName(filename);
    matrixStartOffset = 0;
}

NiftiMatrix::NiftiMatrix(const AString &filename, int64_t &offsetin) throw (NiftiException)
{
    if(!QFile::exists(filename)) throw NiftiException("Need a valid Nifti file to read/write to!");
    file.setFileName(filename);
    matrixStartOffset = offsetin;
}*/

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
    niftiVersion = 0;
    matrixStartOffset = 0;
    matrix = NULL;
    matrixLength = 0;
    layoutSet = false;
    frameLoaded = false;
    currentTime = 0;
}

void NiftiMatrix::setMatrixFile(const QFile &filein){
    this->clearMatrix();
    init();
    file.setFileName(filein.fileName());
}

void NiftiMatrix::setNiftiHeader(Nifti1Header &headerin)
{
    niftiVersion = 1;
    n1Header = headerin;
}

void NiftiMatrix::setNiftiHeader(Nifti2Header &headerin)
{
    niftiVersion = 2;
    n2Header = headerin;
}

void NiftiMatrix::getDataType(NiftiDataTypeEnum::Enum &typeout) const
{
    typeout = niftiDataType;
}

void NiftiMatrix::setDataType(const NiftiDataTypeEnum::Enum &typein)
{
    niftiDataType = typein;
}

#if 0
    array index, byte swap, cast, then data scaling

    0000 1000 0100 1100 0200 1200 0010 1010 0110 1110 0210 1210 0001

    index = 0;
    for t = 0:dim4
            for k = 0:dim3
                    for j = 0:dim2
                            for i = 0:dim1
                                    translateVoxel(i, j, k, t, frame, index);
                                    ++index;
                            end
                    end
            end
    end

    void translateVoxel(i, j, k, t, frame, index)
            for c = 0:components
                    float temp = getComponent(index, component);
                    myVolume.setValue(temp, i, j, k, t, c);
            end
    end
#endif

void NiftiMatrix::getVolumeFrame(VolumeFile &volume, int64_t &timeSlice) throw (NiftiException)
{
    readFrame(timeSlice);
    uint64_t index = 0;
    for(uint32_t k=0;k<dimensions[3];k++)
        for(uint32_t j=0;j<dimensions[2];j++)
            for(uint32_t i=0;i<dimensions[1];i++)
            {
                translateVoxel(i,j,k,index);
                for(int c=0;c<componentDimensions;c++)
                    volume.setValue(matrix[index+c],i,j,k,timeSlice,c);

            }
}

void NiftiMatrix::setVolumeFrame(VolumeFile &volume, int64_t &timeSlice) throw (NiftiException)
{
    //flushCurrentFrame();
    uint64_t index = 0;
    for(uint32_t k=0;k<dimensions[3];k++)
        for(uint32_t j=0;j<dimensions[2];j++)
            for(uint32_t i=0;i<dimensions[1];i++)
            {
                translateVoxel(i,j,k,index);
                for(int c=0;c<componentDimensions;c++)
                    matrix[index+c]=volume.getValue(i,j,k,timeSlice,c);
            }
}

void NiftiMatrix::getMatrixLayoutOnDisk(LayoutType &layout)
{

    layout = (*this);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize, componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::setMatrixLayoutOnDisk(LayoutType &layout)
{
    *(static_cast <LayoutType *> (this)) = layout;
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize, componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::getMatrixLayoutOnDisk(std::vector<int32_t> &dimensionsOut, int &componentDimensionsOut, int &valueByteSizeOut, bool &needsSwappingOut, uint64_t &frameLengthOut, uint64_t &frameSizeOut ) const
{
    dimensionsOut = dimensions;
    componentDimensionsOut = componentDimensions;
    valueByteSizeOut = valueByteSize;
    needsSwappingOut = needsSwapping;
    frameLengthOut = calculateFrameLength(dimensions);
    frameSizeOut = calculateFrameSizeInBytes(frameLength, valueByteSize, componentDimensions);
}

void NiftiMatrix::setMatrixLayoutOnDisk(const std::vector <int32_t> &dimensionsIn, const int &componentDimensionsIn, const int &valueByteSizeIn, const bool &needsSwappingIn )
{
    dimensions = dimensionsIn;
    componentDimensions = componentDimensionsIn;
    valueByteSize = valueByteSizeIn;
    needsSwapping = needsSwappingIn;
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize, componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::getLayoutFromNiftiHeader(const Nifti1Header &headerIn)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    headerIn.getDimensions(this->dimensions);
    headerIn.getComponentDimensions(this->componentDimensions);
    headerIn.getValueByteSize(this->valueByteSize);
    headerIn.getNeedsSwapping(this->needsSwapping);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize, componentDimensions);
    layoutSet = true;
}

void NiftiMatrix::getLayoutFromNiftiHeader(const Nifti2Header &headerIn)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    headerIn.getDimensions(this->dimensions);
    headerIn.getComponentDimensions(this->componentDimensions);
    headerIn.getValueByteSize(this->valueByteSize);
    headerIn.getNeedsSwapping(this->needsSwapping);
    frameLength = calculateFrameLength(dimensions);
    frameSize = calculateFrameSizeInBytes(frameLength, valueByteSize, componentDimensions);
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
}

void NiftiMatrix::readFrame(int64_t timeSlice) throw (NiftiException)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    if(frameLoaded && timeSlice==currentTime) return;
    uint64_t frameOffset = matrixStartOffset+frameSize*timeSlice;
    uint8_t *bytes = NULL;
    bytes = new uint8_t[frameSize];
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
    this->writeFrame();
}

void NiftiMatrix::writeFrame() throw(NiftiException)
{
    if(!frameLoaded) throw NiftiException("Writeframe is called but frame isn't loaded.");
    uint64_t frameOffset = matrixStartOffset+frameSize*this->currentTime;
    file.open(QIODevice::WriteOnly);
    file.seek(frameOffset);
    if(niftiDataType==NIFTI_TYPE_RGB24)
    {
        uint8_t *bytes = NULL;
        bytes = new uint8_t [frameSize];
        for(int i=0;i<frameLength;i++)
        {
            bytes[i]=matrix[i];
        }
        file.write((char *)bytes,frameLength);
        delete [] bytes;
    }
    else //for now, for all other types, we convert to float32, will discuss with John if we want to preserve
         //data type for writing, or if we will have situations where we want to modify nifti files inline
    {
        file.write((char *)matrix,frameSize);
    }
    file.close();
}

void NiftiMatrix::setFrame(float *matrixIn, const uint64_t &matrixLengthIn, const uint64_t &timeSlice)  throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    reAllocateMatrixIfNeeded();
    if(this->matrixLength != matrixLengthIn) throw NiftiException("frame size does not match expected frame size!");
    memcpy(matrix,matrixIn,matrixLength*sizeof(float));
    currentTime = timeSlice;
    frameLoaded = true;
}

void NiftiMatrix::setFrame(const int64_t &timeSlice)  throw(NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    this->resetMatrix();

    currentTime = timeSlice;
    frameLoaded = true;
}

float NiftiMatrix::getComponent(const uint64_t &index, const uint32_t &componentIndex) const throw (NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    if(!frameLoaded) throw NiftiException("Please load frame before getting component.");
    if((componentIndex+1)>componentDimensions) throw NiftiException("ComponentIndex exceeds the number of compoents for this nifti Matrix.");

    return matrix[index*componentDimensions+componentIndex];
}

void NiftiMatrix::translateVoxel(const uint64_t &i, const uint64_t &j, const uint64_t &k, uint64_t &index) const
{
    index = i+(j+(k*dimensions[2]))*dimensions[1];
}

void NiftiMatrix::setComponent(const uint64_t &index, const uint32_t &componentIndex, const float &value) throw (NiftiException)
{
    if(!this->layoutSet) throw NiftiException("Please set layout before setting frame.");
    reAllocateMatrixIfNeeded();
    if((componentIndex+1)>componentDimensions) throw NiftiException("ComponentIndex exceeds the number of components for this nifti Matrix.");
    matrix[index*componentDimensions+componentIndex] = value;
    frameLoaded = true;
}

uint64_t NiftiMatrix::calculateFrameLength(const std::vector<int> &dimensionsIn) const
{
    uint64_t frameLength = 1;
    for(int i=1;i<dimensionsIn[0];i++)
        frameLength*=dimensionsIn[i];
    return frameLength;
}

uint64_t NiftiMatrix::calculateFrameSizeInBytes(const uint64_t &frameLengthIn, const uint32_t &valueByteSizeIn, const uint32_t &componentDimensionsIn) const
{
        return frameLengthIn*valueByteSizeIn*componentDimensionsIn;
}

uint64_t NiftiMatrix::calculateMatrixLength(const uint64_t &frameLengthIn, const uint64_t &componentDimensionsIn) const
{
    return frameLengthIn*componentDimensionsIn;
}

uint64_t NiftiMatrix::calculateMatrixSizeInBytes(const uint64_t &frameSizeIn, const uint64_t &componentDimensionsIn) const
{
    return frameSizeIn*componentDimensionsIn;
}

