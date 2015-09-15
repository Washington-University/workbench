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

#include "AlgorithmCiftiPairwiseCorrelation.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "FileInformation.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiPairwiseCorrelation::getCommandSwitch()
{
    return "-cifti-pairwise-correlation";
}

AString AlgorithmCiftiPairwiseCorrelation::getShortDescription()
{
    return "CORRELATE PAIRED ROWS BETWEEN TWO CIFTI FILES";
}

OperationParameters* AlgorithmCiftiPairwiseCorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-a", "first input cifti file");
    
    ret->addCiftiParameter(2, "cifti-b", "second input cifti file");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "output cifti file");
    
    ret->createOptionalParameter(4, "-fisher-z", "apply fisher small z transform (ie, artanh) to correlation");
    
    ret->createOptionalParameter(5, "-override-mapping-check", "don't check the mappings for compatibility, only check length");
    
    ret->setHelpText(
        AString("For each row in <cifti-a>, correlate it with the same row in <cifti-b>, and put the result in the same row of <cifti-out>, which has only one column.")
    );
    return ret;
}

void AlgorithmCiftiPairwiseCorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiA = myParams->getCifti(1);
    CiftiFile* myCiftiB = myParams->getCifti(2);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(3);
    bool fisherZ = myParams->getOptionalParameter(4)->m_present;
    bool overrideMappingCheck = myParams->getOptionalParameter(5)->m_present;
    AlgorithmCiftiPairwiseCorrelation(myProgObj, myCiftiA, myCiftiB, myCiftiOut, fisherZ, overrideMappingCheck);
}

AlgorithmCiftiPairwiseCorrelation::AlgorithmCiftiPairwiseCorrelation(ProgressObject* myProgObj, const CiftiFile* myCiftiA, const CiftiFile* myCiftiB, CiftiFile* myCiftiOut,
                                                                     const bool& fisherZ, const bool& overrideMappingCheck) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld outXML = myCiftiA->getCiftiXMLOld();
    int64_t numRows = myCiftiA->getNumberOfRows(), rowLength = myCiftiA->getNumberOfColumns();
    if (overrideMappingCheck)
    {
        if (numRows != myCiftiB->getNumberOfRows()) throw AlgorithmException("column length must match between the input files");
    } else {
        if (!outXML.matchesForColumns(myCiftiB->getCiftiXMLOld())) throw AlgorithmException("mapping along columns must match between the input files");
    }
    if (rowLength != myCiftiB->getNumberOfColumns()) throw AlgorithmException("row length must match between the input files");
    outXML.resetRowsToScalars(1);
    outXML.setMapNameForRowIndex(0, "pairwise correlation");
    myCiftiOut->setCiftiXML(outXML);
    vector<float> rowA(rowLength), rowB(rowLength), columnOut(numRows);
    for (int64_t i = 0; i < numRows; ++i)
    {
        float rrsA, rrsB;
        myCiftiA->getRow(rowA.data(), i);
        myCiftiB->getRow(rowB.data(), i);
        preprocess(rowA.data(), rowLength, rrsA);
        preprocess(rowB.data(), rowLength, rrsB);
        columnOut[i] = correlate(rowA.data(), rrsA, rowB.data(), rrsB, rowLength, fisherZ);
    }
    myCiftiOut->setColumn(columnOut.data(), 0);
}

void AlgorithmCiftiPairwiseCorrelation::preprocess(float* row, const int64_t length, float& rrsOut)
{
    double accum = 0.0;
    for (int64_t i = 0; i < length; ++i)
    {
        accum += row[i];
    }
    float mean = accum / length;
    accum = 0.0;
    for (int64_t i = 0; i < length; ++i)
    {
        row[i] -= mean;
        accum += row[i] * row[i];
    }
    rrsOut = sqrt(accum);
}

float AlgorithmCiftiPairwiseCorrelation::correlate(const float* rowA, const float& rrsA, const float* rowB, const float& rrsB, const int64_t& length, const bool& fisherZ)
{
    double r = 0.0;
    for (int64_t i = 0; i < length; ++i)
    {
        r += rowA[i] * rowB[i];
    }
    r /= rrsA * rrsB;
    if (fisherZ)
    {
        if (r > 0.999999) r = 0.999999;//prevent inf
        if (r < -0.999999) r = -0.999999;//prevent -inf
        return 0.5 * log((1 + r) / (1 - r));
    } else {
        if (r > 1.0) r = 1.0;//don't output anything silly
        if (r < -1.0) r = -1.0;
        return r;
    }
}

float AlgorithmCiftiPairwiseCorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiPairwiseCorrelation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
