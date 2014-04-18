/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "OperationNiftiConvert.h"
#include "OperationException.h"

#include "MultiDimIterator.h"
#include "NiftiIO.h"

using namespace caret;
using namespace std;

AString OperationNiftiConvert::getCommandSwitch()
{
    return "-nifti-convert";
}

AString OperationNiftiConvert::getShortDescription()
{
    return "CONVERT BETWEEN NIFTI VERSIONS";
}

OperationParameters* OperationNiftiConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "input", "the input nifti file");
    
    ret->addIntegerParameter(2, "version", "the nifti version to write as");
    
    ret->addStringParameter(3, "output", "output - the output nifti file");//fake the output formatting, we don't have a "generic nifti" type
    
    ret->setHelpText(
        AString("Convert a nifti file to another nifti version.\n\n") +
        "If you want to convert a CIFTI file to a different CIFTI version, use -cifti-convert -version-convert."
    );
    return ret;
}

//hidden templated function to do the reading and writing
template<typename T>
void operationNiftiConvertHelper(NiftiIO& inputIO, NiftiIO& outputIO, const int64_t& maxMem)
{
    const vector<int64_t>& dims = inputIO.getDimensions();
    int64_t totalBytes = (int)sizeof(T) * inputIO.getNumComponents(), totalElems = inputIO.getNumComponents();
    int fullDims = 0;
    for (; fullDims < (int)dims.size() && totalBytes * dims[fullDims] < maxMem; ++fullDims)
    {
        totalBytes *= dims[fullDims];
        totalElems *= dims[fullDims];
    }
    vector<int64_t> remainDims(dims.begin() + fullDims, dims.end());
    vector<T> scratchmem(totalElems);//this is the main purpose of the template...
    for (MultiDimIterator<int64_t> myiter(remainDims); !myiter.atEnd(); ++myiter)
    {
        inputIO.readData(scratchmem.data(), fullDims, *myiter);//...which results in these templating over the desired type
        outputIO.writeData(scratchmem.data(), fullDims, *myiter);
    }
}

void OperationNiftiConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString inFileName = myParams->getString(1);
    int outVer = (int)myParams->getInteger(2);
    AString outFileName = myParams->getString(3);
    const int64_t maxMem = 1<<29;//half gigabyte
    NiftiIO inputIO, outputIO;
    inputIO.openRead(inFileName);
    const NiftiHeader& inHeader = inputIO.getHeader();
    outputIO.writeNew(outFileName, inHeader, outVer);
    double junk1, junk2;
    if (inHeader.getDataScaling(junk1, junk2))//if it rescales, we need to use a datatype other than the on-disk one to preserve values
    {//alternatively, we could give read/write access to the unscaled values, but that is a little distasteful
        operationNiftiConvertHelper<long double>(inputIO, outputIO, maxMem);//use long double to reduce rounding problems, even if it is often massive overkill
    } else {
        switch (inHeader.getDataType())
        {
            case NIFTI_TYPE_UINT8:
            case NIFTI_TYPE_RGB24:
                operationNiftiConvertHelper<uint8_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_INT8:
                operationNiftiConvertHelper<int8_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_UINT16:
                operationNiftiConvertHelper<uint16_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_INT16:
                operationNiftiConvertHelper<int16_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_UINT32:
                operationNiftiConvertHelper<uint32_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_INT32:
                operationNiftiConvertHelper<int32_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_UINT64:
                operationNiftiConvertHelper<uint64_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_INT64:
                operationNiftiConvertHelper<int64_t>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_FLOAT32:
            case NIFTI_TYPE_COMPLEX64:
                operationNiftiConvertHelper<float>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_FLOAT64:
            case NIFTI_TYPE_COMPLEX128:
                operationNiftiConvertHelper<double>(inputIO, outputIO, maxMem);
                break;
            case NIFTI_TYPE_FLOAT128:
            case NIFTI_TYPE_COMPLEX256:
                operationNiftiConvertHelper<long double>(inputIO, outputIO, maxMem);
                break;
            default:
                throw OperationException("unsupported nifti datatype");
        }
    }
}
