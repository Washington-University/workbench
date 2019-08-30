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
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"
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
    return "PERFORM REDUCTION OPERATION ON A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiReduce::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to reduce");
    
    ret->addStringParameter(2, "operation", "the reduction operator to use");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    
    OptionalParameter* directionOpt = ret->createOptionalParameter(6, "-direction", "specify what direction to reduce along");
    directionOpt->addStringParameter(1, "direction", "the direction (default ROW)");
    
    OptionalParameter* excludeOpt = ret->createOptionalParameter(4, "-exclude-outliers", "exclude non-numeric values and outliers by standard deviation");
    excludeOpt->addDoubleParameter(1, "sigma-below", "number of standard deviations below the mean to include");
    excludeOpt->addDoubleParameter(2, "sigma-above", "number of standard deviations above the mean to include");
    
    ret->createOptionalParameter(5, "-only-numeric", "exclude non-numeric values");
    
    ret->setHelpText(
        AString("For the specified direction (default ROW), perform a reduction operation along that direction.  ") +
        CiftiXML::directionFromStringExplanation() + "  " +
        "The reduction operators are as follows:\n\n" + ReductionOperation::getHelpInfo()
    );
    return ret;
}

void AlgorithmCiftiReduce::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiIn = myParams->getCifti(1);
    AString opString = myParams->getString(2);
    CiftiFile* ciftiOut = myParams->getOutputCifti(3);
    int direction = CiftiXML::ALONG_ROW;
    OptionalParameter* directionOpt = myParams->getOptionalParameter(6);
    if (directionOpt->m_present)
    {
        direction = CiftiXML::directionFromString(directionOpt->getString(1));//does error checking, throws with message on error
    }
    OptionalParameter* excludeOpt = myParams->getOptionalParameter(4);
    bool onlyNumeric = myParams->getOptionalParameter(5)->m_present;
    bool ok = false;
    ReductionEnum::Enum myReduce = ReductionEnum::fromName(opString, &ok);
    if (!ok) throw AlgorithmException("unrecognized operation string '" + opString + "'");
    if (excludeOpt->m_present)
    {
        if (onlyNumeric) CaretLogWarning("-only-numeric is redundant when -exclude-outliers is specified");
        AlgorithmCiftiReduce(myProgObj, ciftiIn, myReduce, ciftiOut, excludeOpt->getDouble(1), excludeOpt->getDouble(2), direction);
    } else {
        AlgorithmCiftiReduce(myProgObj, ciftiIn, myReduce, ciftiOut, onlyNumeric, direction);
    }
}

AlgorithmCiftiReduce::AlgorithmCiftiReduce(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const ReductionEnum::Enum& myReduce, CiftiFile* ciftiOut,
                                           const bool& onlyNumeric, const int& direction) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& inputXML = ciftiIn->getCiftiXML();
    CiftiXML myOutXML = inputXML;
    if (direction >= myOutXML.getNumberOfDimensions()) throw AlgorithmException("specified reduction direction doesn't exist in input cifti file");
    CiftiScalarsMap newMap;
    newMap.setLength(1);
    newMap.setMapName(0, ReductionEnum::toName(myReduce));
    myOutXML.setMap(direction, newMap);
    ciftiOut->setCiftiXML(myOutXML);
    vector<int64_t> inDims = inputXML.getDimensions();
    if (direction == CiftiXML::ALONG_ROW)
    {
        if (inDims[0] == 1)
        {
            CaretLogWarning("-cifti-reduce is being used for a length=1 reduction on file '" + ciftiIn->getFileName() + "'");
        }
        vector<float> scratchInRow(inDims[0]);
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(inDims.begin() + 1, inDims.end())); !iter.atEnd(); ++iter)
        {// + 1 to exclude row dimension, because getRow/setRow
            ciftiIn->getRow(scratchInRow.data(), *iter);
            float result = -1;
            if (onlyNumeric)
            {
                result = ReductionOperation::reduceOnlyNumeric(scratchInRow.data(), inDims[0], myReduce);
            } else {
                result = ReductionOperation::reduce(scratchInRow.data(), inDims[0], myReduce);
            }
            ciftiOut->setRow(&result, *iter);//if reducing along row, length of output row is 1
        }
    } else {
        if (inDims[direction] == 1)
        {
            CaretLogWarning("-cifti-reduce is being used for a length=1 reduction on file '" + ciftiIn->getFileName() + "'");
        }
        vector<vector<float> > scratchInRows(inDims[direction], vector<float>(inDims[0]));
        vector<float> outRow(inDims[0]), reduceScratch(inDims[direction]);//reduction isn't along row, so out rows will be same length as in rows
        vector<int64_t> otherDims = inDims;
        otherDims.erase(otherDims.begin() + direction);//direction isn't 0
        otherDims.erase(otherDims.begin());//remove row direction because getRow/setRow
        for (MultiDimIterator<int64_t> iter(otherDims); !iter.atEnd(); ++iter)
        {
            vector<int64_t> indexvec = *iter;
            indexvec.insert(indexvec.begin() + direction - 1, -1);//dummy value in place of reduce direction
            for (int64_t i = 0; i < inDims[direction]; ++i)
            {
                indexvec[direction - 1] = i;
                ciftiIn->getRow(scratchInRows[i].data(), indexvec);
            }
            for (int64_t i = 0; i < inDims[0]; ++i)
            {
                for (int64_t j = 0; j < inDims[direction]; ++j)
                {//need reduction input in contiguous array
                    reduceScratch[j] = scratchInRows[j][i];
                }
                if (onlyNumeric)
                {
                    outRow[i] = ReductionOperation::reduceOnlyNumeric(reduceScratch.data(), inDims[direction], myReduce);
                } else {
                    outRow[i] = ReductionOperation::reduce(reduceScratch.data(), inDims[direction], myReduce);
                }
            }
            indexvec[direction - 1] = 0;//only one element along reduce output direction
            ciftiOut->setRow(outRow.data(), indexvec);
        }
    }
}

AlgorithmCiftiReduce::AlgorithmCiftiReduce(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const ReductionEnum::Enum& myReduce, CiftiFile* ciftiOut,
                                           const float& sigmaBelow, const float& sigmaAbove, const int& direction) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& inputXML = ciftiIn->getCiftiXML();
    CiftiXML myOutXML = inputXML;
    if (direction >= myOutXML.getNumberOfDimensions()) throw AlgorithmException("specified reduction direction doesn't exist in input cifti file");
    CiftiScalarsMap newMap;
    newMap.setLength(1);
    newMap.setMapName(0, ReductionEnum::toName(myReduce));
    myOutXML.setMap(direction, newMap);
    ciftiOut->setCiftiXML(myOutXML);
    vector<int64_t> inDims = inputXML.getDimensions();
    if (direction == CiftiXML::ALONG_ROW)
    {
        vector<float> scratchInRow(inDims[0]);
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(inDims.begin() + 1, inDims.end())); !iter.atEnd(); ++iter)
        {// + 1 to exclude row dimension, because getRow/setRow
            ciftiIn->getRow(scratchInRow.data(), *iter);
            float result = ReductionOperation::reduceExcludeDev(scratchInRow.data(), inDims[0], myReduce, sigmaBelow, sigmaAbove);
            ciftiOut->setRow(&result, *iter);//if reducing along row, length of output row is 1
        }
    } else {
        vector<vector<float> > scratchInRows(inDims[direction], vector<float>(inDims[0]));
        vector<float> outRow(inDims[0]), reduceScratch(inDims[direction]);//reduction isn't along row, so out rows will be same length as in rows
        vector<int64_t> otherDims = inDims;
        otherDims.erase(otherDims.begin() + direction);//direction isn't 0
        otherDims.erase(otherDims.begin());//remove row direction because getRow/setRow
        for (MultiDimIterator<int64_t> iter(otherDims); !iter.atEnd(); ++iter)
        {
            vector<int64_t> indexvec = *iter;
            indexvec.insert(indexvec.begin() + direction - 1, -1);//dummy value in place of reduce direction
            for (int64_t i = 0; i < inDims[direction]; ++i)
            {
                indexvec[direction - 1] = i;
                ciftiIn->getRow(scratchInRows[i].data(), indexvec);
            }
            for (int64_t i = 0; i < inDims[0]; ++i)
            {
                for (int64_t j = 0; j < inDims[direction]; ++j)
                {//need reduction input in contiguous array
                    reduceScratch[j] = scratchInRows[j][i];
                }
                outRow[i] = ReductionOperation::reduceExcludeDev(reduceScratch.data(), inDims[direction], myReduce, sigmaBelow, sigmaAbove);
            }
            indexvec[direction - 1] = 0;//only one element along reduce output direction
            ciftiOut->setRow(outRow.data(), indexvec);
        }
    }
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
