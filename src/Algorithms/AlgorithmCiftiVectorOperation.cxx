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

#include "AlgorithmCiftiVectorOperation.h"
#include "AlgorithmException.h"

#include "CiftiFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiVectorOperation::getCommandSwitch()
{
    return "-cifti-vector-operation";
}

AString AlgorithmCiftiVectorOperation::getShortDescription()
{
    return "DO A VECTOR OPERATION ON CIFTI FILES";
}

OperationParameters* AlgorithmCiftiVectorOperation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "vectors-a", "first vector input file");
    
    ret->addCiftiParameter(2, "vectors-b", "second vector input file");
    
    ret->addStringParameter(3, "operation", "what vector operation to do");
    
    ret->addCiftiOutputParameter(4, "cifti-out", "the output file");
    
    ret->createOptionalParameter(5, "-normalize-a", "normalize vectors of first input");

    ret->createOptionalParameter(6, "-normalize-b", "normalize vectors of second input");
    
    ret->createOptionalParameter(7, "-normalize-output", "normalize output vectors (not valid for dot product)");
    
    ret->createOptionalParameter(8, "-magnitude", "output the magnitude of the result (not valid for dot product)");
    
    AString myText =
        AString("Does a vector operation on two cifti files (that must have a multiple of 3 columns).  ") +
        "Either of the inputs may have multiple vectors (more than 3 columns), but not both (at least one must have exactly 3 columns).  " +
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

void AlgorithmCiftiVectorOperation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiA = myParams->getCifti(1);
    CiftiFile* ciftiB = myParams->getCifti(2);
    AString operString = myParams->getString(3);
    bool ok = false;
    VectorOperation::Operation myOper = VectorOperation::stringToOperation(operString, ok);
    if (!ok) throw AlgorithmException("unrecognized operation string: " + operString);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(4);
    bool normA = myParams->getOptionalParameter(5)->m_present;
    bool normB = myParams->getOptionalParameter(6)->m_present;
    bool normOut = myParams->getOptionalParameter(7)->m_present;
    bool magOut = myParams->getOptionalParameter(8)->m_present;
    AlgorithmCiftiVectorOperation(myProgObj, ciftiA, ciftiB, myOper, myCiftiOut, normA, normB, normOut, magOut);
}

AlgorithmCiftiVectorOperation::AlgorithmCiftiVectorOperation(ProgressObject* myProgObj, const CiftiFile* ciftiA, const CiftiFile* ciftiB,
                                                             const VectorOperation::Operation& myOper, CiftiFile* myCiftiOut,
                                                             const bool& normA, const bool& normB, const bool& normOut, const bool& magOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& xmlA = ciftiA->getCiftiXML(), &xmlB = ciftiB->getCiftiXML();
    if (xmlA.getNumberOfDimensions() != 2 || xmlB.getNumberOfDimensions() != 2)
    {
        throw AlgorithmException("cifti vector operation only supports 2D cifti");
    }
    if (!xmlA.getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(*xmlB.getMap(CiftiXML::ALONG_COLUMN)))
    {
        throw AlgorithmException("input cifti files have non-matching mappings along column");
    }
    int64_t numColA = xmlA.getDimensionLength(CiftiXML::ALONG_ROW), numColB = xmlB.getDimensionLength(CiftiXML::ALONG_ROW);
    if (numColA % 3 != 0) throw AlgorithmException("number of columns of first input is not a multiple of 3");
    if (numColB % 3 != 0) throw AlgorithmException("number of columns of second input is not a multiple of 3");
    int numVecA = numColA / 3, numVecB = numColB / 3;
    if (numVecA > 1 && numVecB > 1) throw AlgorithmException("both inputs have more than 3 columns (more than 1 vector)");
    if (normOut && magOut) throw AlgorithmException("normalizing the output and taking the magnitude is meaningless");
    bool opScalarResult = VectorOperation::operationReturnsScalar(myOper);
    if (opScalarResult && (normOut || magOut)) throw AlgorithmException("cannot normalize or take magnitude of a scalar result (such as a dot product)");
    bool swapped = false;
    const CiftiFile* multiVec = ciftiA, *singleVec = ciftiB;
    int numOutVecs = numVecA;
    if (numVecB > 1)
    {
        multiVec = ciftiB;
        singleVec = ciftiA;
        numOutVecs = numVecB;
        swapped = true;
    }
    CiftiXML outXML = multiVec->getCiftiXML();
    int numColsOut = numOutVecs * 3;
    if (opScalarResult || magOut)
    {
        numColsOut = numOutVecs;
        CiftiScalarsMap outRowMap;
        outRowMap.setLength(numColsOut);
        outXML.setMap(CiftiXML::ALONG_ROW, outRowMap);
    }
    myCiftiOut->setCiftiXML(outXML);
    vector<float> outRow(numColsOut), multiRow(numOutVecs * 3);
    int64_t numRows = xmlA.getDimensionLength(CiftiXML::ALONG_COLUMN);
    for (int64_t row = 0; row < numRows; ++row)
    {
        multiVec->getRow(multiRow.data(), row);
        Vector3D vecSingle;
        singleVec->getRow(vecSingle, row);
        for (int64_t v = 0; v < numOutVecs; ++v)
        {
            Vector3D vecA, vecB;
            if (swapped)
            {
                vecA = multiRow.data() + v * 3;
                vecB = vecSingle;
            } else {
                vecA = vecSingle;
                vecB = multiRow.data() + v * 3;
            }
            if (normA) vecA = vecA.normal();
            if (normB) vecB = vecB.normal();
            if (opScalarResult)
            {
                outRow[v] = VectorOperation::doScalarOperation(vecA, vecB, myOper);
            } else {
                Vector3D tempVec = VectorOperation::doVectorOperation(vecA, vecB, myOper);
                if (normOut) tempVec = tempVec.normal();
                if (magOut)
                {
                    outRow[v] = tempVec.length();
                } else {
                    outRow[v * 3] = tempVec[0];
                    outRow[v * 3 + 1] = tempVec[1];
                    outRow[v * 3 + 2] = tempVec[2];
                }
            }
        }
        myCiftiOut->setRow(outRow.data(), row);
    }
}

float AlgorithmCiftiVectorOperation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiVectorOperation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
