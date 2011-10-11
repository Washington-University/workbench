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
#if 0
#include "NiftiMatrix.h"

using namespace caret;

NiftiMatrix::NiftiMatrix()
{
}

NiftiMatrix::NiftiMatrix(const AString &filename) throw (NiftiException)
{
    if(!QFile.exists(filename)) throw NiftiException("Need a valid Nifti file to read/write to!");
    file.setFileName(filename);
    offset = 0;
}

NiftiMatrix::NiftiMatrix(Astring &filename, int64_t &offsetin)
{
    if(!QFile.exists(filename)) throw NiftiException("Need a valid Nifti file to read/write to!");
    file.setFileName(filename);
    offset = offsetin;
}

NiftiMatrix::NiftiMatrix(QFile &filein)
{
    if(!QFile.exists(filename)) throw NiftiException("Need a valid Nifti file to read/write to!");
    file.setFileName(filename);
    offset = 0;
}

NiftiMatrix::NiftiMatrix(QFile &filein, int64_t &offsetin) throw (NiftiException)
{
    file = filein;
    offset = offsetin;
}

void NiftiMatrix::init()
{
    swapNeeded = false;
    niftiVersion = 0;
    matrixStartOffset = 0;
}

void NiftiMatrix::getDataType(NiftiDataTypeEnum &typeout) const throw (NiftiException)
{
    typeout = niftiDataType;
}

void NiftiMatrix::setDataType(NiftiDataTypeEnum &typein) throw (NiftiException)
{
    niftiDataType = typein;
}

void NiftiMatrix::getVolumeFrame(VolumeFile &volume, int64_t &timeslice) throw (NiftiException)
{

}

void NiftiMatrix::setVolumeFrame(const VolumeFile &volume, int64_t &timeslice) throw (NiftiException)
{

}

void NiftiMatrix::LoadFrame(int64_t timeSlice)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    int componentDimensions = 1;
    switch(niftiDataType) {
    case NIFTI_TYPE_FLOAT32:
        valueByteSize = sizeof(float_t);
        break;
    case NIFTI_TYPE_FLOAT64:
        valueByteSize = sizeof(double_t);

        break;
    case NIFTI_TYPE_RGB24:
        valueByteSize = 3;
        componentDimensions = 3;
        break;
    default:
        throw NiftiException("Unsupported Data Type.");
    }

    uint64_t frameLength = 1;
    for(int i=1;i<=dimensions[0];i++)
        frameLength*=dimensions[i];

    uint64_t frameSize = frameLength*valueByteSize;

    uint64_t frameOffset = offset+frameSize*timeSlice;
    uint8_t *bytes = null;
    bytes = new uint8_t [frameSize];

    QFile.seek(frameOffset);
    QFile.read((char *)bytes,frameSize);

    //convert to floats
    //for the special case of RGB, we convert each byte in an RGB array to floats so that we can use the same float matrix for all other functions
    if(matrixLength != frameLength*componentDimensions) {
        if(matrix) delete matrix;
        matrixLength = frameSize*componentDimensions;
        matrix = new float[matrixLength];
    }
    currentTime = timeSlice;
    //convert matrices to floats
    switch (niftiDataType)
    case NIFTI_TYPE_FLOAT32:
        if(swapNeeded) ByteSwapping::swapBytes((float *)bytes,frameLength);
        memcpy((void *)matrix,(void *)bytes, frameSize);
        break;
    case NIFTI_TYPE_FLOAT64:
        if(swapNeeded) ByteSwapping::swapBytes((double *)bytes, frameLength);
        for(int i;i<matrixLength;i++) matrix[i] = ((double *)bytes)[i];
        break;
    case NIFTI_TYPE_RGB24:
        for(int i;i<matrixLength;i++) matrix[i] = bytes[i];
        break;
    default:
        break;
    }
    //cleanup
    delete bytes;
}

//unbuffered, single component at a time, generic, and sloooowww....
float NiftiMatrix::getComponent(const int64_t &index, const int32_t &componentIndex) const throw (NiftiException)
{
    int componentDimensions = 1;
    if(niftiDataType == NIFTI_TYPE_RGB24) componentDimensions = 3;
    if((componentIndex+1)>componentDimensions) throw NiftiException("ComponentIndex exceeds the number of compoents for this nifti Matrix.");

    return matrix[index*componentDimensions+componentIndex];
}

void NiftiMatrix::translateVoxel(const int64_t &i, const int64_t &j, const int64_t &k, int64_t &index) const
{
    index = i+(j+(k*dim[2]))*dim[1];
}

//unbuffered, single component at a time, generic, and sloooowww....
void NiftiMatrix::setComponent(const int64_t &index, const int32_t &component_index, const float &value) throw (NiftiException)
{
    int
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

#endif
