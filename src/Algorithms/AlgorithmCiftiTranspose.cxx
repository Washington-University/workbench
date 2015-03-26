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

#include "AlgorithmCiftiTranspose.h"
#include "AlgorithmException.h"
#include "CiftiFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiTranspose::getCommandSwitch()
{
    return "-cifti-transpose";
}

AString AlgorithmCiftiTranspose::getShortDescription()
{
    return "TRANSPOSE A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiTranspose::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the input cifti file");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(3, "-mem-limit", "restrict memory usage");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("The input must be a 2-dimensional cifti file.  ") +
        "The output is a cifti file where every row in the input is a column in the output."
    );
    return ret;
}

void AlgorithmCiftiTranspose::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiIn = myParams->getCifti(1);
    CiftiFile* ciftiOut = myParams->getOutputCifti(2);
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(3);
    float memLimitGB = -1.0f;
    if (memLimitOpt->m_present)
    {
        memLimitGB = (float)memLimitOpt->getDouble(1);
        if (memLimitGB < 0.0f)
        {
            throw AlgorithmException("memory limit cannot be negative");
        }
    }
    AlgorithmCiftiTranspose(myProgObj, ciftiIn, ciftiOut, memLimitGB);
}

AlgorithmCiftiTranspose::AlgorithmCiftiTranspose(ProgressObject* myProgObj, const CiftiFile* ciftiIn, CiftiFile* ciftiOut, const float& memLimitGB) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& inXML = ciftiIn->getCiftiXML();
    if (inXML.getNumberOfDimensions() != 2)
    {
        throw AlgorithmException("cifti transpose only supports 2D cifti");
    }
    CiftiXML outXML;
    outXML.setNumberOfDimensions(2);
    outXML.setMap(0, *(inXML.getMap(1)));
    outXML.setMap(1, *(inXML.getMap(0)));
    ciftiOut->setCiftiXML(outXML);
    int rowSize = outXML.getDimensionLength(CiftiXML::ALONG_ROW), colSize = outXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
    int64_t outRowBytes = rowSize * sizeof(float);
    int numCacheRows = colSize;
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = memLimitGB * 1024 * 1024 * 1024 / outRowBytes;
        if (numCacheRows < 1) numCacheRows = 1;
        if (numCacheRows > colSize) numCacheRows = colSize;
    }
    vector<vector<float> > cacheRows(numCacheRows, vector<float>(rowSize));
    vector<float> scratchInRow(colSize);
    for (int i = 0; i < colSize; i += numCacheRows)//loop through cache chunks
    {
        int end = i + numCacheRows;
        if (end > colSize) end = colSize;
        for (int j = 0; j < rowSize; ++j)//loop through all input rows
        {
            ciftiIn->getRow(scratchInRow.data(), j);
            for (int k = i; k < end; ++k)
            {
                cacheRows[k - i][j] = scratchInRow[k];
            }
        }
        for (int k = i; k < end; ++k)
        {
            ciftiOut->setRow(cacheRows[k - i].data(), k);
        }
    }
}

float AlgorithmCiftiTranspose::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiTranspose::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
