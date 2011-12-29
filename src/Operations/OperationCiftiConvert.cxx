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
#include <vector>

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
        AString("This is where you set the help text.  DO NOT add the info about what the command line format is, ") +
        "and do not give the command switch, short description, or the short descriptions of parameters.  Do not indent, " +
        "add newlines, or format the text in any way other than to separate paragraphs within the help text prose."
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
        GiftiDataArray* myArray = new GiftiDataArray(myHeader.getIntent(), NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, myDims, GiftiEncodingEnum::EXTERNAL_FILE_BINARY);
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
        throw OperationException("cifti writing not yet supported");
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
        //CiftiFile* myOutFile = fromGiftiExt->getOutputCifti(2);
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
        CiftiMatrix myMatrix;
        myMatrix.setup(myDims);
    }
}
