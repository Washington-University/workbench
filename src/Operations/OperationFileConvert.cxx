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

#include "OperationFileConvert.h"
#include "OperationException.h"

#include "BorderFile.h"
#include "Border.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"
#include "NiftiIO.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationFileConvert::getCommandSwitch()
{
    return "-file-convert";
}

AString OperationFileConvert::getShortDescription()
{
    return "CHANGE VERSION OF FILE FORMAT";
}

OperationParameters* OperationFileConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    OptionalParameter* borderConv = ret->createOptionalParameter(1, "-border-version-convert", "write a border file with a different version");
    borderConv->addBorderParameter(1, "border-in", "the input border file");
    borderConv->addIntegerParameter(2, "out-version", "the format version to write as, 1 or 3 (2 doesn't exist)");
    borderConv->addStringParameter(3, "border-out", "output - the output border file");//fake the output formatting, the auto-output code will use whatever version it wants
    OptionalParameter* borderSurfOpt = borderConv->createOptionalParameter(4, "-surface", "must be specified if the input is version 1");
    borderSurfOpt->addSurfaceParameter(1, "surface", "use this surface file for structure and number of vertices, ignore borders on other structures");
    
    OptionalParameter* niftiConv = ret->createOptionalParameter(2, "-nifti-version-convert", "write a nifti file with a different version");
    niftiConv->addStringParameter(1, "input", "the input nifti file");//VolumeFile isn't "generic nifti", so we use NiftiIO directly
    niftiConv->addIntegerParameter(2, "version", "the nifti version to write as");
    niftiConv->addStringParameter(3, "output", "output - the output nifti file");//fake the output formatting, we don't have a "generic nifti" type
    
    OptionalParameter* ciftiConv = ret->createOptionalParameter(3, "-cifti-version-convert", "write a cifti file with a different version");
    ciftiConv->addCiftiParameter(1, "cifti-in", "the input cifti file");
    ciftiConv->addStringParameter(2, "version", "the cifti version to write as");
    ciftiConv->addStringParameter(3, "cifti-out", "output - the output cifti file");//fake the output formatting so we can just call writeFile and be done with it (and also not add a layer of provenance)
    
    ret->setHelpText(
        AString("You may only specify one top-level option.")
    );
    return ret;
}

namespace _operation_file_convert//hide helpers in a file-specific namespace
{
//hidden templated function to do generic nifti reading and writing
    template<typename T>
    void niftiConvertHelper(NiftiIO& inputIO, NiftiIO& outputIO, const int64_t& maxMem)
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
}
using namespace _operation_file_convert;

void OperationFileConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    OptionalParameter* borderConv = myParams->getOptionalParameter(1);
    OptionalParameter* niftiConv = myParams->getOptionalParameter(2);
    OptionalParameter* ciftiConv = myParams->getOptionalParameter(3);
    int numChosen = 0;
    if (borderConv->m_present) ++numChosen;
    if (niftiConv->m_present) ++numChosen;
    if (ciftiConv->m_present) ++numChosen;
    if (numChosen != 1) throw OperationException("you must choose exactly one top level option");
    if (borderConv->m_present)
    {
        const BorderFile* borderIn = borderConv->getBorder(1);
        int outVersion = (int)borderConv->getInteger(2);
        AString outFilename = borderConv->getString(3);
        BorderFile borderOut;
        int numBorders = borderIn->getNumberOfBorders();
        if (borderIn->getNumberOfNodes() < 1)//version 1 border file
        {
            OptionalParameter* surfOpt = borderConv->getOptionalParameter(4);
            if (!surfOpt->m_present) throw OperationException("version 1 border files require the -surface suboption for conversion");
            SurfaceFile* convSurf = surfOpt->getSurface(1);
            StructureEnum::Enum myStructure = convSurf->getStructure();
            borderOut.setStructure(myStructure);
            borderOut.setNumberOfNodes(convSurf->getNumberOfNodes());
            for (int i = 0; i < numBorders; ++i)
            {
                const Border* thisBorder = borderIn->getBorder(i);
                if (thisBorder->getStructure() == myStructure)
                {
                    borderOut.addBorder(new Border(*thisBorder));//takes ownership of RAW POINTER
                }
            }
        } else {
            borderOut.setStructure(borderIn->getStructure());
            borderOut.setNumberOfNodes(borderIn->getNumberOfNodes());
            for (int i = 0; i < numBorders; ++i)
            {
                borderOut.addBorder(new Border(*borderIn->getBorder(i)));//ditto
            }
        }
        borderOut.writeFile(outFilename, outVersion);
    }
    if (niftiConv->m_present)
    {
        AString inFileName = niftiConv->getString(1);
        int outVer = (int)niftiConv->getInteger(2);
        AString outFileName = niftiConv->getString(3);
        const int64_t maxMem = 1<<29;//half gigabyte - will usually be much less, could be an option
        NiftiIO inputIO, outputIO;
        inputIO.openRead(inFileName);
        const NiftiHeader& inHeader = inputIO.getHeader();
        outputIO.writeNew(outFileName, inHeader, outVer);//NOTE: this keeps data scaling fields, data type, extensions, header fields we ignore, etc
        double mult, offset;//if the offset is large compared to the scale, it is nontrivial to deduce a suitable type to preserve sufficient precision to round back to the unscaled input values
        if (inHeader.getDataScaling(mult, offset))//we can't use the on-disk type because we don't provide access to the unscaled (wrong) values
        {//alternatively, we could give read/write access to the unscaled values, but that is distasteful
            niftiConvertHelper<long double>(inputIO, outputIO, maxMem);//so, always use long double to reduce rounding problems, even if it is often massive overkill
        } else {
            switch (inHeader.getDataType())
            {
                case NIFTI_TYPE_UINT8:
                case NIFTI_TYPE_RGB24:
                    niftiConvertHelper<uint8_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_INT8:
                    niftiConvertHelper<int8_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_UINT16:
                    niftiConvertHelper<uint16_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_INT16:
                    niftiConvertHelper<int16_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_UINT32:
                    niftiConvertHelper<uint32_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_INT32:
                    niftiConvertHelper<int32_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_UINT64:
                    niftiConvertHelper<uint64_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_INT64:
                    niftiConvertHelper<int64_t>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_FLOAT32:
                case NIFTI_TYPE_COMPLEX64:
                    niftiConvertHelper<float>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_FLOAT64:
                case NIFTI_TYPE_COMPLEX128:
                    niftiConvertHelper<double>(inputIO, outputIO, maxMem);
                    break;
                case NIFTI_TYPE_FLOAT128:
                case NIFTI_TYPE_COMPLEX256:
                    niftiConvertHelper<long double>(inputIO, outputIO, maxMem);
                    break;
                default:
                    throw OperationException("unsupported nifti datatype");
            }
        }
    }
    if (ciftiConv->m_present)
    {
        CiftiFile* ciftiIn = ciftiConv->getCifti(1);
        AString versionString = ciftiConv->getString(2);
        AString outFileName = ciftiConv->getString(3);
        ciftiIn->writeFile(outFileName, CiftiVersion(versionString));//also handles complications like writing to the same file as it is set to read on-disk from
    }
}
