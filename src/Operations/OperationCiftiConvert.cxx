/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include "OperationCiftiConvert.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "FloatMatrix.h"
#include "GiftiFile.h"
#include "NiftiFile.h"
#include "VolumeFile.h"

#include <limits>
#include <vector>

#include <QFile>

using namespace caret;
using namespace std;

AString OperationCiftiConvert::getCommandSwitch()
{
    return "-cifti-convert";
}

AString OperationCiftiConvert::getShortDescription()
{
    return "CONVERT TO OR FROM CIFTI";
}

OperationParameters* OperationCiftiConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    OptionalParameter* toGiftiExt = ret->createOptionalParameter(1, "-to-gifti-ext", "convert to GIFTI external binary");
    toGiftiExt->addCiftiParameter(1, "cifti-in", "the input cifti file");
    toGiftiExt->addStringParameter(2, "gifti-out", "output - the output gifti file");
    
    OptionalParameter* fromGiftiExt = ret->createOptionalParameter(2, "-from-gifti-ext", "convert a GIFTI made with this command back into a CIFTI");
    fromGiftiExt->addStringParameter(1, "gifti-in", "the input gifti file");
    fromGiftiExt->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    OptionalParameter* fgresetTimeOpt = fromGiftiExt->createOptionalParameter(3, "-reset-timepoints", "reset the mapping along rows to timepoints, taking length from the gifti file");
    fgresetTimeOpt->addDoubleParameter(1, "timestep", "the desired time between frames");
    fgresetTimeOpt->addDoubleParameter(2, "timestart", "the desired time offset of the initial frame");
    OptionalParameter* fromGiftiReplace = fromGiftiExt->createOptionalParameter(4, "-replace-binary", "replace data with a binary file");
    fromGiftiReplace->addStringParameter(1, "binary-in", "the binary file that contains replacement data");
    fromGiftiReplace->createOptionalParameter(2, "-flip-endian", "byteswap the binary file");
    fromGiftiReplace->createOptionalParameter(3, "-transpose", "transpose the binary file");
    
    OptionalParameter* toNifti = ret->createOptionalParameter(3, "-to-nifti", "convert to NIFTI1");
    toNifti->addCiftiParameter(1, "cifti-in", "the input cifti file");
    toNifti->addVolumeOutputParameter(2, "nifti-out", "the output nifti file");
    
    OptionalParameter* fromNifti = ret->createOptionalParameter(4, "-from-nifti", "convert from NIFTI (1 or 2)");
    fromNifti->addVolumeParameter(1, "nifti-in", "the input nifti file");
    fromNifti->addCiftiParameter(2, "cifti-template", "a cifti file with the dimension(s) and mapping(s) that should be used");
    fromNifti->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    OptionalParameter* fnresetTimeOpt = fromNifti->createOptionalParameter(4, "-reset-timepoints", "reset the mapping along rows to timepoints, taking length from the nifti file");
    fnresetTimeOpt->addDoubleParameter(1, "timestep", "the desired time between frames");
    fnresetTimeOpt->addDoubleParameter(2, "timestart", "the desired time offset of the initial frame");
    
    ret->setHelpText(
        AString("This command writes a Cifti file as something that can be more easily used by some other programs.  ") +
        "Only one of -to-gifti-ext or -from-gifti-ext may be specified.  " +
        "The -transpose option is needed if the binary file is in column-major order."
    );
    return ret;
}

void OperationCiftiConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int modes = 0;
    OptionalParameter* toGiftiExt = myParams->getOptionalParameter(1);
    OptionalParameter* fromGiftiExt = myParams->getOptionalParameter(2);
    OptionalParameter* toNifti = myParams->getOptionalParameter(3);
    OptionalParameter* fromNifti = myParams->getOptionalParameter(4);
    if (toGiftiExt->m_present) ++modes;
    if (fromGiftiExt->m_present) ++modes;
    if (toNifti->m_present) ++modes;
    if (fromNifti->m_present) ++modes;
    if (modes != 1)
    {
        throw OperationException("you must specify exactly one conversion mode");
    }
    if (toGiftiExt->m_present)
    {
        CiftiFile* myInFile = toGiftiExt->getCifti(1);
        AString myGiftiName = toGiftiExt->getString(2);
        CiftiHeader myHeader;
        vector<int64_t> myDims;
        myDims.push_back(myInFile->getNumberOfRows());
        myDims.push_back(myInFile->getNumberOfColumns());
        NiftiIntentEnum::Enum myIntent = NiftiIntentEnum::NIFTI_INTENT_CONNECTIVITY_DENSE;
        const CiftiXML& myXML = myInFile->getCiftiXML();
        if (myXML.getColumnMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS ||
            myXML.getRowMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS)
        {
            myIntent = NiftiIntentEnum::NIFTI_INTENT_CONNECTIVITY_DENSE_TIME;
        }
        GiftiDataArray* myArray = new GiftiDataArray(myIntent, NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, myDims, GiftiEncodingEnum::EXTERNAL_FILE_BINARY);
        float* myOutData = myArray->getDataPointerFloat();
        for (int i = 0; i < myInFile->getNumberOfRows(); ++i)
        {
            myInFile->getRow(myOutData + i * myInFile->getNumberOfColumns(), i);
        }
        AString myCiftiXML;
        myXML.writeXML(myCiftiXML);
        myArray->getMetaData()->set("CiftiXML", myCiftiXML);
        GiftiFile myOutFile;
        myOutFile.setEncodingForWriting(GiftiEncodingEnum::EXTERNAL_FILE_BINARY);
        myOutFile.addDataArray(myArray);
        myOutFile.writeFile(myGiftiName);
    }
    if (fromGiftiExt->m_present)
    {
        AString myGiftiName = fromGiftiExt->getString(1);
        GiftiFile myInFile;
        myInFile.readFile(myGiftiName);
        if (myInFile.getNumberOfDataArrays() != 1)
        {
            throw OperationException("input gifti has the wrong number of arrays");
        }
        GiftiDataArray* dataArrayRef = myInFile.getDataArray(0);
        if (dataArrayRef->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32)
        {
            throw OperationException("input gifti has the wrong data type");
        }
        CiftiFile* myOutFile = fromGiftiExt->getOutputCifti(2);
        CiftiXML myXML(dataArrayRef->getMetaData()->get("CiftiXML"));
        int64_t numCols = dataArrayRef->getNumberOfComponents();
        int64_t numRows = dataArrayRef->getNumberOfRows();
        OptionalParameter* fgresetTimeOpt = fromGiftiExt->getOptionalParameter(3);
        if (fgresetTimeOpt->m_present)
        {
            myXML.resetRowsToTimepoints(fgresetTimeOpt->getDouble(1), numCols, fgresetTimeOpt->getDouble(2));
        } else {
            if (myXML.getRowMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS)
            {
                myXML.setRowNumberOfTimepoints(numCols);
            }
            if (myXML.getColumnMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS)
            {
                myXML.setColumnNumberOfTimepoints(numRows);
            }
        }
        if (myXML.getNumberOfColumns() != numCols || myXML.getNumberOfRows() != numRows) throw OperationException("dimensions of input gifti array do not match dimensions in the embedded Cifti XML");
        myOutFile->setCiftiXML(myXML);
        OptionalParameter* fromGiftiReplace = fromGiftiExt->getOptionalParameter(4);
        if (fromGiftiReplace->m_present)
        {
            AString replaceFileName = fromGiftiReplace->getString(1);
            QFile replaceFile(replaceFileName);
            if (replaceFile.size() != (int64_t)(sizeof(float) * numCols * numRows))
            {
                throw OperationException("replacement file is the wrong size, size is " + AString::number(replaceFile.size()) + ", needed " + AString::number(sizeof(float) * numCols * numRows));
            }
            if (!replaceFile.open(QIODevice::ReadOnly))
            {
                throw OperationException("unable to open replacement file for reading");
            }
            OptionalParameter* swapBytes = fromGiftiReplace->getOptionalParameter(2);
            OptionalParameter* transpose = fromGiftiReplace->getOptionalParameter(3);
            int64_t readSize = numCols, numReads = numRows;
            if (transpose->m_present)
            {
                readSize = numRows;
                numReads = numCols;
            }
            CaretArray<float> myScratch(readSize);
            for (int i = 0; i < numReads; ++i)
            {
                if (replaceFile.read((char*)(myScratch.getArray()), sizeof(float) * readSize) != (int64_t)(sizeof(float) * readSize))
                {
                    throw OperationException("short read from replacement file, aborting");
                }
                float tempVal;
                char* tempValPointer = (char*)&tempVal;//copy method isn't as fast, but it is clean
                for (int j = 0; j < readSize; ++j)
                {
                    if (swapBytes->m_present)
                    {
                        char* elemPointer = (char*)(myScratch.getArray() + j);
                        for (int k = 0; k < (int)sizeof(float); ++k)
                        {
                            tempValPointer[k] = elemPointer[sizeof(float) - 1 - k];
                        }
                    } else {
                        tempVal = myScratch[j];
                    }
                    if (transpose->m_present)
                    {
                        int32_t indices[] = {j, i};
                        dataArrayRef->setDataFloat32(indices, tempVal);
                    } else {
                        int32_t indices[] = {i, j};
                        dataArrayRef->setDataFloat32(indices, tempVal);
                    }
                }
            }
        }
        vector<float> scratchRow(numCols);
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numCols; ++j)
            {
                int32_t indices[] = {i, j};
                scratchRow[j] = dataArrayRef->getDataFloat32(indices);
            }
            myOutFile->setRow(scratchRow.data(), i);
        }
    }
    if (toNifti->m_present)
    {
        CiftiFile* myCiftiIn = toNifti->getCifti(1);
        VolumeFile* myNiftiOut = toNifti->getOutputVolume(2);
        vector<int64_t> outDims(4, 1);
        outDims[3] = myCiftiIn->getNumberOfColumns();
        if (outDims[3] > numeric_limits<short>::max()) throw OperationException("cifti rows are too long for nifti1, failing");
        int64_t numRows = myCiftiIn->getNumberOfRows();
        int64_t temp = numRows;
        int index = 0;
        while (temp > numeric_limits<short>::max())
        {
            if (index > 1) throw OperationException("too many cifti columns for nifti1 spatial dimensions, failing");
            outDims[index] = numeric_limits<short>::max();
            temp = (temp - 1) / numeric_limits<short>::max() + 1;//round up
            ++index;
        }
        outDims[index] = temp;
        myNiftiOut->reinitialize(outDims, FloatMatrix::identity(4).getMatrix());
        myNiftiOut->setValueAllVoxels(0.0f);
        int64_t ijk[3] = { 0, 0, 0 };
        vector<float> rowscratch(outDims[3]);
        for (int64_t i = 0; i < numRows; ++i)
        {
            myCiftiIn->getRow(rowscratch.data(), i);
            for (int64_t j = 0; j < outDims[3]; ++j)
            {
                myNiftiOut->setValue(rowscratch[j], ijk, j);
            }
            ++ijk[0];
            if (ijk[0] >= outDims[0])
            {
                ijk[0] = 0;
                ++ijk[1];
                if (ijk[1] >= outDims[1])
                {
                    ijk[1] = 0;
                    ++ijk[2];
                    CaretAssert(i == numRows - 1 || ijk[2] < outDims[2]);//in case it divided exactly
                }
            }
        }
    }
    if (fromNifti->m_present)
    {
        VolumeFile* myNiftiIn = fromNifti->getVolume(1);
        CiftiFile* myTemplate = fromNifti->getCifti(2);
        CiftiFile* myCiftiOut = fromNifti->getOutputCifti(3);
        vector<int64_t> myDims;
        myNiftiIn->getDimensions(myDims);
        if (myDims[4] != 1) throw OperationException("input nifti has multiple components, aborting");
        CiftiXML outXML = myTemplate->getCiftiXML();
        OptionalParameter* fnresetTimeOpt = fromNifti->getOptionalParameter(4);
        if (fnresetTimeOpt->m_present)
        {
            outXML.resetRowsToTimepoints(fnresetTimeOpt->getDouble(1), myDims[3], fnresetTimeOpt->getDouble(2));
        }
        int64_t numRows = outXML.getNumberOfRows(), numCols = outXML.getNumberOfColumns();
        if (myDims[3] != numCols) throw OperationException("input nifti has the wrong size for row length");
        if (numRows > myDims[0] * myDims[1] * myDims[2]) throw OperationException("input nifti is too small for number of rows");
        myCiftiOut->setCiftiXML(outXML);
        vector<float> rowscratch(numCols);
        for (int64_t i = 0; i < numRows; ++i)
        {
            for (int64_t j = 0; j < numCols; ++j)
            {
                rowscratch[j] = myNiftiIn->getFrame(j)[i];
            }
            myCiftiOut->setRow(rowscratch.data(), i);
        }
    }
}
