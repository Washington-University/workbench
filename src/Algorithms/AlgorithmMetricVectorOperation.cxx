/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AlgorithmMetricVectorOperation.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "MetricFile.h"
#include "Vector3D.h"

#include <algorithm>

using namespace caret;
using namespace std;

AString AlgorithmMetricVectorOperation::getCommandSwitch()
{
    return "-metric-vector-operation";
}

AString AlgorithmMetricVectorOperation::getShortDescription()
{
    return "DO A VECTOR OPERATION ON METRIC FILES";
}

OperationParameters* AlgorithmMetricVectorOperation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addMetricParameter(1, "vectors-a", "first vector input file");
    
    ret->addMetricParameter(2, "vectors-b", "second vector input file");
    
    ret->addStringParameter(3, "operation", "what vector operation to do");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output file");
    
    ret->createOptionalParameter(9, "-match-maps", "expect both input files to have the same number of vectors, and operate pairwise");
    
    ret->createOptionalParameter(5, "-normalize-a", "normalize vectors of first input");
    
    ret->createOptionalParameter(6, "-normalize-b", "normalize vectors of second input");
    
    ret->createOptionalParameter(7, "-normalize-output", "normalize output vectors (not valid for dot product)");
    
    ret->createOptionalParameter(8, "-magnitude", "output the magnitude of the result (not valid for dot product)");
    
    AString myText =
        AString("Does a vector operation on two metric files (that must have a multiple of 3 columns).  ") +
        "Without -match-maps, either of the inputs may have multiple vectors (more than 3 columns), but not both (at least one must have exactly 3 columns).  " +
        "The -magnitude and -normalize-output options may not be specified together, or with an operation that returns a scalar (dot product).  " +
        "The <operation> parameter must be one of the following:\n";
    vector<VectorOperation::Operation> opList = VectorOperation::getAllOperations();
    for (int i = 0; i < (int)opList.size(); ++i)
    {
        myText += "\n" + VectorOperation::operationToString(opList[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void AlgorithmMetricVectorOperation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* metricA = myParams->getMetric(1);
    MetricFile* metricB = myParams->getMetric(2);
    AString operString = myParams->getString(3);
    bool ok = false;
    VectorOperation::Operation myOper = VectorOperation::stringToOperation(operString, ok);
    if (!ok) throw AlgorithmException("unrecognized operation string: " + operString);
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    bool matchMaps = myParams->getOptionalParameter(9)->m_present;
    bool normA = myParams->getOptionalParameter(5)->m_present;
    bool normB = myParams->getOptionalParameter(6)->m_present;
    bool normOut = myParams->getOptionalParameter(7)->m_present;
    bool magOut = myParams->getOptionalParameter(8)->m_present;
    AlgorithmMetricVectorOperation(myProgObj, metricA, metricB, myOper, myMetricOut, normA, normB, normOut, magOut, matchMaps);
}

AlgorithmMetricVectorOperation::AlgorithmMetricVectorOperation(ProgressObject* myProgObj, const MetricFile* metricA, const MetricFile* metricB, const VectorOperation::Operation& myOper,
                                                               MetricFile* myMetricOut, const bool& normA, const bool& normB, const bool& normOut, const bool& magOut, const bool matchMaps) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    StructureEnum::Enum checkStruct = metricA->getStructure();
    int numNodes = metricA->getNumberOfNodes();
    if (numNodes != metricB->getNumberOfNodes()) throw AlgorithmException("inputs have different numbers of nodes");
    int numColA = metricA->getNumberOfColumns(), numColB = metricB->getNumberOfColumns();
    if (numColA % 3 != 0) throw AlgorithmException("number of columns of '" + metricA->getFileName() + "' is not a multiple of 3");
    if (numColB % 3 != 0) throw AlgorithmException("number of columns of '" + metricB->getFileName() + "' is not a multiple of 3");
    int numVecA = numColA / 3, numVecB = numColB / 3;
    if (matchMaps)
    {
        if (numVecA != numVecB)
        {
            throw AlgorithmException("-match-maps specified, but inputs have different numbers of columns (vectors)");
        }
    } else {
        if (numVecA > 1 && numVecB > 1)
        {
            if (numVecA == numVecB)
            {
                throw AlgorithmException("both inputs have more than 3 columns (more than 1 vector), did you want -match-maps?");
            } else {
                throw AlgorithmException("both inputs have more than 3 columns (more than 1 vector)");
            }
        }
    }
    if (normOut && magOut) throw AlgorithmException("normalizing the output and taking the magnitude is meaningless");
    bool opScalarResult = VectorOperation::operationReturnsScalar(myOper);
    if (opScalarResult && (normOut || magOut)) throw AlgorithmException("cannot normalize or take magnitude of a scalar result (such as a dot product)");
    if (checkStruct == StructureEnum::INVALID)
    {
        CaretLogWarning("first input vector file has INVALID structure");
    } else {
        checkStructureMatch(metricB, checkStruct, "second input vector file", "the first has");
    }
    int numInVecs = max(numVecA, numVecB);
    vector<vector<float> > outCols;
    if (opScalarResult || magOut)
    {
        outCols.resize(1, vector<float>(numNodes));
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numInVecs);
    } else {
        outCols.resize(3, vector<float>(numNodes));
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numInVecs * 3);
    }
    myMetricOut->setStructure(checkStruct);
    for (int v = 0; v < numInVecs; ++v)
    {
        const float* vecAx, *vecAy, *vecAz, *vecBx, *vecBy, *vecBz;
        if (numVecA > 1)
        {
            vecAx = metricA->getValuePointerForColumn(v * 3);
            vecAy = metricA->getValuePointerForColumn(v * 3 + 1);
            vecAz = metricA->getValuePointerForColumn(v * 3 + 2);
        } else {
            vecAx = metricA->getValuePointerForColumn(0);
            vecAy = metricA->getValuePointerForColumn(1);
            vecAz = metricA->getValuePointerForColumn(2);
        }
        if (numVecB > 1)
        {
            vecBx = metricB->getValuePointerForColumn(v * 3);
            vecBy = metricB->getValuePointerForColumn(v * 3 + 1);
            vecBz = metricB->getValuePointerForColumn(v * 3 + 2);
        } else {
            vecBx = metricB->getValuePointerForColumn(0);
            vecBy = metricB->getValuePointerForColumn(1);
            vecBz = metricB->getValuePointerForColumn(2);
        }
        for (int i = 0; i < numNodes; ++i)
        {
            Vector3D vecA(vecAx[i], vecAy[i], vecAz[i]),
                     vecB(vecBx[i], vecBy[i], vecBz[i]);
            if (normA) vecA = vecA.normal();
            if (normB) vecB = vecB.normal();
            if (opScalarResult)
            {
                outCols[0][i] = VectorOperation::doScalarOperation(vecA, vecB, myOper);
            } else {
                Vector3D tempVec = VectorOperation::doVectorOperation(vecA, vecB, myOper);
                if (normOut) tempVec = tempVec.normal();
                if (magOut)
                {
                    outCols[0][i] = tempVec.length();
                } else {
                    outCols[0][i] = tempVec[0];
                    outCols[1][i] = tempVec[1];
                    outCols[2][i] = tempVec[2];
                }
            }
        }
        if (opScalarResult || magOut)
        {
            myMetricOut->setValuesForColumn(v, outCols[0].data());
        } else {
            myMetricOut->setValuesForColumn(v * 3, outCols[0].data());
            myMetricOut->setValuesForColumn(v * 3 + 1, outCols[1].data());
            myMetricOut->setValuesForColumn(v * 3 + 2, outCols[2].data());
        }
    }
}

float AlgorithmMetricVectorOperation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricVectorOperation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
