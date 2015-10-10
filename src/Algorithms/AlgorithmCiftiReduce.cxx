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

#include "AlgorithmCiftiReduce.h"
#include "AlgorithmException.h"
#include "CiftiFile.h"
#include "ReductionOperation.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiReduce::getCommandSwitch()
{
    return "-cifti-reduce";
}

AString AlgorithmCiftiReduce::getShortDescription()
{
    return "PERFORM REDUCTION OPERATION ALONG CIFTI ROWS";
}

OperationParameters* AlgorithmCiftiReduce::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to reduce");
    
    ret->addStringParameter(2, "operation", "the reduction operator to use");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    
    OptionalParameter* excludeOpt = ret->createOptionalParameter(4, "-exclude-outliers", "exclude non-numeric values and outliers by standard deviation");
    excludeOpt->addDoubleParameter(1, "sigma-below", "number of standard deviations below the mean to include");
    excludeOpt->addDoubleParameter(2, "sigma-above", "number of standard deviations above the mean to include");
    
    ret->createOptionalParameter(5, "-only-numeric", "exclude non-numeric values");
    
    ret->setHelpText(
        AString("For each cifti row, takes the data along a row as a vector, and performs the specified reduction on it, putting the result ") +
        "into the single output column in that row.  The reduction operators are as follows:\n\n" + ReductionOperation::getHelpInfo()
    );
    return ret;
}

void AlgorithmCiftiReduce::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiIn = myParams->getCifti(1);
    AString opString = myParams->getString(2);
    CiftiFile* ciftiOut = myParams->getOutputCifti(3);
    OptionalParameter* excludeOpt = myParams->getOptionalParameter(4);
    bool onlyNumeric = myParams->getOptionalParameter(5)->m_present;
    bool ok = false;
    ReductionEnum::Enum myReduce = ReductionEnum::fromName(opString, &ok);
    if (!ok) throw AlgorithmException("unrecognized operation string '" + opString + "'");
    if (excludeOpt->m_present)
    {
        if (onlyNumeric) throw AlgorithmException("-exclude-outliers and -only-numeric may not be specified together");
        AlgorithmCiftiReduce(myProgObj, ciftiIn, myReduce, ciftiOut, excludeOpt->getDouble(1), excludeOpt->getDouble(2));
    } else {
        AlgorithmCiftiReduce(myProgObj, ciftiIn, myReduce, ciftiOut, onlyNumeric);
    }
}

AlgorithmCiftiReduce::AlgorithmCiftiReduce(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const ReductionEnum::Enum& myReduce, CiftiFile* ciftiOut, const bool& onlyNumeric) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int64_t numRows = ciftiIn->getNumberOfRows();
    int64_t numCols = ciftiIn->getNumberOfColumns();
    if (numCols < 1 || numRows < 1) throw AlgorithmException("input must have at least 1 column and 1 row");
    CiftiXML myOutXML = ciftiIn->getCiftiXML();
    if (myOutXML.getNumberOfDimensions() != 2)
    {
        throw AlgorithmException("cifti reduce only supports 2D cifti");
    }
    CiftiScalarsMap newMap;
    newMap.setLength(1);
    newMap.setMapName(0, ReductionEnum::toName(myReduce));
    myOutXML.setMap(CiftiXML::ALONG_ROW, newMap);
    ciftiOut->setCiftiXML(myOutXML);
    vector<float> scratchRow(numCols), outCol(numRows);
    for (int64_t i = 0; i < numRows; ++i)
    {
        ciftiIn->getRow(scratchRow.data(), i);
        if (onlyNumeric)
        {
            outCol[i] = ReductionOperation::reduceOnlyNumeric(scratchRow.data(), numCols, myReduce);
        } else {
            outCol[i] = ReductionOperation::reduce(scratchRow.data(), numCols, myReduce);
        }
    }
    ciftiOut->setColumn(outCol.data(), 0);
}

AlgorithmCiftiReduce::AlgorithmCiftiReduce(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const ReductionEnum::Enum& myReduce, CiftiFile* ciftiOut, const float& sigmaBelow, const float& sigmaAbove) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int64_t numRows = ciftiIn->getNumberOfRows();
    int64_t numCols = ciftiIn->getNumberOfColumns();
    if (numCols < 1 || numRows < 1) throw AlgorithmException("input must have at least 1 column and 1 row");
    CiftiXML myOutXML = ciftiIn->getCiftiXML();
    if (myOutXML.getNumberOfDimensions() != 2)
    {
        throw AlgorithmException("cifti reduce only supports 2D cifti");
    }
    CiftiScalarsMap newMap;
    newMap.setLength(1);
    newMap.setMapName(0, ReductionEnum::toName(myReduce));
    myOutXML.setMap(CiftiXML::ALONG_ROW, newMap);
    ciftiOut->setCiftiXML(myOutXML);
    vector<float> scratchRow(numCols), outCol(numRows);
    for (int64_t i = 0; i < numRows; ++i)
    {
        ciftiIn->getRow(scratchRow.data(), i);
        outCol[i] = ReductionOperation::reduceExcludeDev(scratchRow.data(), numCols, myReduce, sigmaBelow, sigmaAbove);
    }
    ciftiOut->setColumn(outCol.data(), 0);
}

float AlgorithmCiftiReduce::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiReduce::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
