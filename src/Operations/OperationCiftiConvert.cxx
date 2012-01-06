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
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "GiftiFile.h"
#include "CaretPointer.h"
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
    toGiftiExt->addStringParameter(2, "gifti-out", "the output gifti file");
    OptionalParameter* fromGiftiExt = ret->createOptionalParameter(2, "-from-gifti-ext", "convert a GIFTI made with this command back into a CIFTI");
    fromGiftiExt->addStringParameter(1, "gifti-in", "the input gifti file");
    fromGiftiExt->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    OptionalParameter* fromGiftiReplace = fromGiftiExt->createOptionalParameter(1, "-replace-binary", "replace data with a binary file");
    fromGiftiReplace->addStringParameter(1, "binary-in", "the binary file that contains replacement data");
    fromGiftiReplace->createOptionalParameter(2, "-flip-endian", "byteswap the binary file");
    ret->setHelpText(
        AString("This command writes a Cifti file as something that can be more easily used by some other programs.  Only one of -to-gifti-ext or -from-gifti-ext ") +
        "may be specified."
    );
    return ret;
}

void OperationCiftiConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int modes = 0;
    OptionalParameter* toGiftiExt = myParams->getOptionalParameter(1);
    if (toGiftiExt->m_present) ++modes;
    OptionalParameter* fromGiftiExt = myParams->getOptionalParameter(2);
    if (fromGiftiExt->m_present) ++modes;
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
        if (myDims[0] != myDims[1] ||
            myInFile->hasRowSurfaceData(StructureEnum::CORTEX_LEFT) != myInFile->hasColumnSurfaceData(StructureEnum::CORTEX_LEFT) ||
            myInFile->hasRowSurfaceData(StructureEnum::CORTEX_RIGHT) != myInFile->hasColumnSurfaceData(StructureEnum::CORTEX_RIGHT))
        {//HACK: guess at the right intent type, because it isn't in the nifti header, or in the cifti XML
            myIntent = NiftiIntentEnum::NIFTI_INTENT_CONNECTIVITY_DENSE_TIME;
        }
        GiftiDataArray* myArray = new GiftiDataArray(myIntent, NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, myDims, GiftiEncodingEnum::EXTERNAL_FILE_BINARY);
        float* myOutData = myArray->getDataPointerFloat();
        for (int i = 0; i < myInFile->getNumberOfRows(); ++i)
        {
            myInFile->getRow(myOutData + i * myInFile->getNumberOfColumns(), i);
        }
        CiftiXML myXML;
        myInFile->getCiftiXML(myXML);
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
        CiftiHeader myHeader;
        switch (dataArrayRef->getIntent())
        {
            case NIFTI_INTENT_CONNECTIVITY_DENSE:
                myHeader.initDenseConnectivity();
                break;
            case NIFTI_INTENT_CONNECTIVITY_DENSE_TIME:
                myHeader.initDenseTimeSeries();
                break;
            default:
                throw OperationException("incorrect intent code in input gifti");
        };
        vector<int64_t> myDims = dataArrayRef->getDimensions();
        vector<int64_t> myCiftiDims;
        myCiftiDims.push_back(1);
        myCiftiDims.push_back(1);
        myCiftiDims.push_back(1);
        myCiftiDims.push_back(1);
        myCiftiDims.push_back(myDims[1]);
        myCiftiDims.push_back(myDims[0]);
        myHeader.setDimensions(myDims);
        myOutFile->setHeader(myHeader);
        myOutFile->setCiftiXML(CiftiXML(dataArrayRef->getMetaData()->get("CiftiXML")));
        int64_t rowSize = dataArrayRef->getNumberOfComponents();
        int64_t colSize = dataArrayRef->getNumberOfRows();
        OptionalParameter* fromGiftiReplace = fromGiftiExt->getOptionalParameter(1);
        if (fromGiftiReplace->m_present)
        {
            AString replaceFileName = fromGiftiReplace->getString(1);
            QFile replaceFile(replaceFileName);
            if (replaceFile.size() != (int64_t)(sizeof(float) * rowSize * colSize))
            {
                throw OperationException("replacement file is the wrong size, size is " + AString::number(replaceFile.size()) + ", needed " + AString::number(sizeof(float) * rowSize * colSize));
            }
            if (!replaceFile.open(QIODevice::ReadOnly))
            {
                throw OperationException("unable to open replacement file for reading");
            }
            OptionalParameter* swapBytes = fromGiftiReplace->getOptionalParameter(2);
            CaretArray<float> myScratch(rowSize);
            for (int i = 0; i < colSize; ++i)
            {
                if (replaceFile.read((char*)(myScratch.getArray()), sizeof(float) * rowSize) != (int64_t)(sizeof(float) * rowSize))
                {
                    throw OperationException("short read from replacement file, aborting");
                }
                if (swapBytes->m_present)
                {
                    float tempVal;
                    char* tempValPointer = (char*)&tempVal;//copy method isn't as fast, but it is clean
                    for (int j = 0; j < rowSize; ++j)
                    {
                        char* elemPointer = (char*)(myScratch.getArray() + j);
                        for (int k = 0; k < (int)sizeof(float); ++k)
                        {
                            tempValPointer[k] = elemPointer[sizeof(float) - 1 - k];
                        }
                        myScratch[j] = tempVal;
                    }
                }
                myOutFile->setRow(myScratch.getArray(), i);
            }
        } else {
            float* inputArray = dataArrayRef->getDataPointerFloat();
            for (int i = 0; i < colSize; ++i)
            {
                myOutFile->setRow(inputArray + (i * rowSize), i);
            }
        }
    }
}
