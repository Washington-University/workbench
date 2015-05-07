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

#include "AlgorithmVolumeVectorOperation.h"
#include "AlgorithmException.h"

#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeVectorOperation::getCommandSwitch()
{
    return "-volume-vector-operation";
}

AString AlgorithmVolumeVectorOperation::getShortDescription()
{
    return "DO A VECTOR OPERATION ON VOLUME FILES";
}

OperationParameters* AlgorithmVolumeVectorOperation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "vectors-a", "first vector input file");
    
    ret->addVolumeParameter(2, "vectors-b", "second vector input file");
    
    ret->addStringParameter(3, "operation", "what vector operation to do");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output file");
    
    ret->createOptionalParameter(5, "-normalize-a", "normalize vectors of first input");

    ret->createOptionalParameter(6, "-normalize-b", "normalize vectors of second input");
    
    ret->createOptionalParameter(7, "-normalize-output", "normalize output vectors (not valid for dot product)");
    
    ret->createOptionalParameter(8, "-magnitude", "output the magnitude of the result (not valid for dot product)");
    
    AString myText =
        AString("Does a vector operation on two volume files (that must have a multiple of 3 subvolumes).  ") +
        "Either of the inputs may have multiple vectors (more than 3 subvolumes), but not both (at least one must have exactly 3 subvolumes).  " +
        "The -magnitude and -normalize-output options may not be specified together, or with the DOT operation.  " +
        "The <operation> parameter must be one of the following:\n";
    vector<VectorOperation::Operation> opList = VectorOperation::getAllOperations();
    for (int i = 0; i < (int)opList.size(); ++i)
    {
        myText += "\n" + VectorOperation::operationToString(opList[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void AlgorithmVolumeVectorOperation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volumeA = myParams->getVolume(1);
    VolumeFile* volumeB = myParams->getVolume(2);
    AString operString = myParams->getString(3);
    bool ok = false;
    VectorOperation::Operation myOper = VectorOperation::stringToOperation(operString, ok);
    if (!ok) throw AlgorithmException("unrecognized operation string: " + operString);
    VolumeFile* myVolumeOut = myParams->getOutputVolume(4);
    bool normA = myParams->getOptionalParameter(5)->m_present;
    bool normB = myParams->getOptionalParameter(6)->m_present;
    bool normOut = myParams->getOptionalParameter(7)->m_present;
    bool magOut = myParams->getOptionalParameter(8)->m_present;
    AlgorithmVolumeVectorOperation(myProgObj, volumeA, volumeB, myOper, myVolumeOut, normA, normB, normOut, magOut);
}

AlgorithmVolumeVectorOperation::AlgorithmVolumeVectorOperation(ProgressObject* myProgObj, const VolumeFile* volumeA, const VolumeFile* volumeB,
                                                               const VectorOperation::Operation& myOper, VolumeFile* myVolumeOut,
                                                               const bool& normA, const bool& normB, const bool& normOut, const bool& magOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const VolumeSpace& checkSpace = volumeA->getVolumeSpace();
    if (!volumeB->matchesVolumeSpace(checkSpace)) throw AlgorithmException("input files have different volume spaces");
    const vector<int64_t> dimA = volumeA->getDimensions(), dimB = volumeB->getDimensions();
    if (dimA[4] != 1 || dimB[4] != 1) throw AlgorithmException("volumes with multiple components (complex or rgb datatypes) are not supported in vector operations");
    if (dimA[3] % 3 != 0) throw AlgorithmException("number of subvolumes in first input is not a multiple of 3");
    if (dimB[3] % 3 != 0) throw AlgorithmException("number of subvolumes in first input is not a multiple of 3");
    int64_t numVecA = dimA[3] / 3, numVecB = dimB[3] / 3;
    if (numVecA > 1 && numVecB > 1) throw AlgorithmException("both inputs have more than 3 subvolumes (more than 1 vector)");
    if (normOut && magOut) throw AlgorithmException("normalizing the output and taking the magnitude is meaningless");
    bool opScalarResult = VectorOperation::operationReturnsScalar(myOper);
    if (opScalarResult && (normOut || magOut)) throw AlgorithmException("cannot normalize or take magnitude of a scalar result (such as a dot product)");
    bool swapped = false;
    const VolumeFile* multiVec = volumeA, *singleVec = volumeB;
    int64_t numOutVecs = numVecA;
    if (numVecB > 1)
    {
        multiVec = volumeB;
        singleVec = volumeA;
        numOutVecs = numVecB;
        swapped = true;
    }
    int64_t numSubvolsOut = numOutVecs * 3;
    if (opScalarResult || magOut) numSubvolsOut = numOutVecs;
    vector<int64_t> outDims = dimA;
    outDims.resize(3);
    if (numSubvolsOut > 1) outDims.push_back(numSubvolsOut);
    myVolumeOut->reinitialize(outDims, checkSpace.getSform());
    int64_t frameSize = dimA[0] * dimA[1] * dimA[2];
    vector<float> outFrames[3];//let the scalar result case overallocate
    outFrames[0].resize(frameSize);
    outFrames[1].resize(frameSize);
    outFrames[2].resize(frameSize);
    const float* xFrameSingle = singleVec->getFrame(0);
    const float* yFrameSingle = singleVec->getFrame(1);
    const float* zFrameSingle = singleVec->getFrame(2);
    for (int64_t v = 0; v < numOutVecs; ++v)
    {
        const float* xFrameMulti = multiVec->getFrame(v * 3);
        const float* yFrameMulti = multiVec->getFrame(v * 3 + 1);
        const float* zFrameMulti = multiVec->getFrame(v * 3 + 2);
        for (int64_t i = 0; i < frameSize; ++i)
        {
            Vector3D vecA(xFrameMulti[i], yFrameMulti[i], zFrameMulti[i]);
            Vector3D vecB(xFrameSingle[i], yFrameSingle[i], zFrameSingle[i]);
            if (swapped)
            {
                Vector3D tempVec = vecA;
                vecA = vecB;
                vecB = tempVec;
            }
            if (normA) vecA = vecA.normal();
            if (normB) vecB = vecB.normal();
            if (opScalarResult)
            {
                outFrames[0][i] = VectorOperation::doScalarOperation(vecA, vecB, myOper);
            } else {
                Vector3D tempVec = VectorOperation::doVectorOperation(vecA, vecB, myOper);
                if (normOut) tempVec = tempVec.normal();
                if (magOut)
                {
                    outFrames[0][i] = tempVec.length();
                } else {
                    outFrames[0][i] = tempVec[0];
                    outFrames[1][i] = tempVec[1];
                    outFrames[2][i] = tempVec[2];
                }
            }
        }
        if (opScalarResult || magOut)
        {
            myVolumeOut->setFrame(outFrames[0].data(), v);
        } else {
            myVolumeOut->setFrame(outFrames[0].data(), v * 3);
            myVolumeOut->setFrame(outFrames[1].data(), v * 3 + 1);
            myVolumeOut->setFrame(outFrames[2].data(), v * 3 + 2);
        }
    }
}

float AlgorithmVolumeVectorOperation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeVectorOperation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
