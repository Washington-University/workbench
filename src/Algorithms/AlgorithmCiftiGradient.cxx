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

#include "AlgorithmCiftiGradient.h"
#include "AlgorithmException.h"
#include "AlgorithmMetricGradient.h"
#include "AlgorithmVolumeGradient.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "SurfaceFile.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiGradient::getCommandSwitch()
{
    return "-cifti-gradient";
}

AString AlgorithmCiftiGradient::getShortDescription()
{
    return "TAKE GRADIENT OF A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiGradient::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    
    ret->addStringParameter(2, "direction", "which dimension to take the gradient along, ROW or COLUMN");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(4, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    OptionalParameter* leftCorrAreasOpt = leftSurfOpt->createOptionalParameter(2, "-left-corrected-areas", "vertex areas to use instead of computing them from the left surface");
    leftCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(5, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    OptionalParameter* rightCorrAreasOpt = rightSurfOpt->createOptionalParameter(2, "-right-corrected-areas", "vertex areas to use instead of computing them from the right surface");
    rightCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* cerebSurfOpt = ret->createOptionalParameter(6, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    OptionalParameter* cerebCorrAreasOpt = cerebSurfOpt->createOptionalParameter(2, "-cerebellum-corrected-areas", "vertex areas to use instead of computing them from the cerebellum surface");
    cerebCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ParameterComponent* genSurfOpt = ret->createRepeatableParameter(12, "-surface", "specify a surface by structure name");
    genSurfOpt->addStringParameter(1, "structure", "the surface structure name");
    genSurfOpt->addSurfaceParameter(2, "surface", "the surface file");
    OptionalParameter* genCorrAreasOpt = genSurfOpt->createOptionalParameter(3, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    genCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");

    OptionalParameter* presmoothSurfOpt = ret->createOptionalParameter(7, "-surface-presmooth", "smooth on the surface before computing the gradient");
    presmoothSurfOpt->addDoubleParameter(1, "surface-kernel", "the size of the gaussian surface smoothing kernel in mm, as sigma by default");
    
    OptionalParameter* presmoothVolOpt = ret->createOptionalParameter(8, "-volume-presmooth", "smooth on the surface before computing the gradient");
    presmoothVolOpt->addDoubleParameter(1, "volume-kernel", "the size of the gaussian volume smoothing kernel in mm, as sigma by default");
    
    ret->createOptionalParameter(11, "-presmooth-fwhm", "smoothing kernel sizes are FWHM, not sigma");
    
    ret->createOptionalParameter(9, "-average-output", "output the average of the gradient magnitude maps instead of each gradient map separately");
    
    OptionalParameter* vectorOpt = ret->createOptionalParameter(10, "-vectors", "output gradient vectors");
    vectorOpt->addCiftiOutputParameter(1, "vectors-out", "the vectors, as a dscalar file");
    
    AString helpText =
        AString("Performs gradient calculation on each component of the cifti file, and optionally averages the resulting gradients.  ") +
        "The -vectors and -average-output options may not be used together.  " +
        "You must specify a surface for each surface structure in the cifti file.  The COLUMN direction should be faster, and is the " +
        "direction that works on dtseries.  For dconn, you probably want ROW, unless you are using -average-output.\n\n" +
        "The <structure> argument to -surface must be one of the following strings:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        helpText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

void AlgorithmCiftiGradient::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    AString directionName = myParams->getString(2);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXMLOld::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXMLOld::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(3);
    map<StructureEnum::Enum, AlgorithmCiftiGradient::SurfParam> surfParams;
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    MetricFile* myLeftAreas = NULL, *myRightAreas = NULL, *myCerebAreas = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(4);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
        OptionalParameter* leftCorrAreasOpt = leftSurfOpt->getOptionalParameter(2);
        if (leftCorrAreasOpt->m_present)
        {
            myLeftAreas = leftCorrAreasOpt->getMetric(1);
        }
        surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(myLeftSurf, myLeftAreas);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(5);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
        OptionalParameter* rightCorrAreasOpt = rightSurfOpt->getOptionalParameter(2);
        if (rightCorrAreasOpt->m_present)
        {
            myRightAreas = rightCorrAreasOpt->getMetric(1);
        }
        surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(myRightSurf, myRightAreas);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(6);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
        OptionalParameter* cerebCorrAreasOpt = cerebSurfOpt->getOptionalParameter(2);
        if (cerebCorrAreasOpt->m_present)
        {
            myCerebAreas = cerebCorrAreasOpt->getMetric(1);
        }
        surfParams[StructureEnum::CEREBELLUM] = SurfParam(myCerebSurf, myCerebAreas);
    }
    auto genSurfArgs = myParams->getRepeatableParameterInstances(12);
    for (auto instance : genSurfArgs)
    {
        bool ok = false;
        StructureEnum::Enum structure = StructureEnum::fromName(instance->getString(1), &ok);
        if (!ok) throw AlgorithmException("unrecognized structure identifier: " + instance->getString(1));
        if (surfParams.find(structure) != surfParams.end()) throw AlgorithmException("more than one surface argument specified for structure '" + instance->getString(1) + "'");
        auto areasOpt = instance->getOptionalParameter(3);
        if (areasOpt->m_present)
        {
            surfParams[structure] = SurfParam(instance->getSurface(2), areasOpt->getMetric(1));
        } else {
            surfParams[structure] = SurfParam(instance->getSurface(2));
        }
    }
    float surfKern = -1.0f;
    OptionalParameter* presmoothSurfOpt = myParams->getOptionalParameter(7);
    if (presmoothSurfOpt->m_present)
    {
        surfKern = (float)presmoothSurfOpt->getDouble(1);
    }
    float volKern = -1.0f;
    OptionalParameter* presmoothVolOpt = myParams->getOptionalParameter(8);
    if (presmoothVolOpt->m_present)
    {
        volKern = (float)presmoothVolOpt->getDouble(1);
    }
    if (myParams->getOptionalParameter(11)->m_present)
    {
        if (surfKern > 0.0f) surfKern = surfKern / (2.0f * sqrt(2.0f * log(2.0f)));
        if (volKern > 0.0f) volKern = volKern / (2.0f * sqrt(2.0f * log(2.0f)));
    }
    bool outputAverage = myParams->getOptionalParameter(9)->m_present;
    CiftiFile* ciftiVectorsOut = NULL;
    OptionalParameter* vectorOpt = myParams->getOptionalParameter(10);
    if (vectorOpt->m_present)
    {
        ciftiVectorsOut = vectorOpt->getOutputCifti(1);
    }
    AlgorithmCiftiGradient(myProgObj, myCifti, myDir, myCiftiOut, surfKern, volKern,
                           surfParams,
                           outputAverage, ciftiVectorsOut);
}

AlgorithmCiftiGradient::AlgorithmCiftiGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir,
                                               CiftiFile* myCiftiOut, const float& surfKern, const float& volKern,
                                               SurfaceFile* myLeftSurf, SurfaceFile* myRightSurf, SurfaceFile* myCerebSurf,
                                               bool outputAverage,
                                               const MetricFile* myLeftAreas, const MetricFile* myRightAreas, const MetricFile* myCerebAreas,
                                               CiftiFile* ciftiVectorsOut) : AbstractAlgorithm(NULL)
{
    std::map<StructureEnum::Enum, SurfParam> surfParams;
    if (myLeftSurf != NULL) surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(myLeftSurf, myLeftAreas);
    if (myRightSurf != NULL) surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(myRightSurf, myRightAreas);
    if (myCerebSurf != NULL) surfParams[StructureEnum::CEREBELLUM] = SurfParam(myCerebSurf, myCerebAreas);
    AlgorithmCiftiGradient(myProgObj, myCifti, myDir, myCiftiOut, surfKern, volKern, surfParams, outputAverage, ciftiVectorsOut);
}

AlgorithmCiftiGradient::AlgorithmCiftiGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir,
                                               CiftiFile* myCiftiOut, const float& surfKern, const float& volKern,
                                               const std::map<StructureEnum::Enum, SurfParam> surfParams,
                                               bool outputAverage, caret::CiftiFile* ciftiVectorsOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = myCifti->getCiftiXML();
    CiftiXML myNewXML = myXML, myVecXML = myXML;
    if (myXML.getNumberOfDimensions() != 2)
    {
        throw AlgorithmException("cifti gradient only supports 2D cifti");
    }
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("specified direction does not contain brainordinates");
    }
    if (outputAverage && ciftiVectorsOut != NULL)
    {
        throw AlgorithmException("outputting gradient vectors while averaging gradient magnitude is not supported");
    }
    const CiftiBrainModelsMap& myDenseMap = myXML.getBrainModelsMap(myDir);
    vector<StructureEnum::Enum> surfaceList = myDenseMap.getSurfaceStructureList(), volumeList = myDenseMap.getVolumeStructureList();
    if (outputAverage)
    {
        if (myDir == CiftiXML::ALONG_ROW)
        {//dscalar always has brainordinates on columns, so flip the mappings
            myNewXML.setMap(CiftiXML::ALONG_COLUMN, *(myNewXML.getMap(CiftiXML::ALONG_ROW)));
        }
        CiftiScalarsMap magMap;
        magMap.setLength(1);
        magMap.setMapName(0, "gradient average");
        myNewXML.setMap(CiftiXML::ALONG_ROW, magMap);
    } else {
        if (myDir == CiftiXML::ALONG_ROW)
        {//dscalar always has brainordinates on columns, so flip the mappings
            myVecXML.setMap(CiftiXML::ALONG_COLUMN, *(myVecXML.getMap(CiftiXML::ALONG_ROW)));
        }
        CiftiScalarsMap vecMap;
        vecMap.setLength(3 * myXML.getDimensionLength(1 - myDir));
        myVecXML.setMap(CiftiXML::ALONG_ROW, vecMap);
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        AString surfType = StructureEnum::toName(surfaceList[whichStruct]);
        auto iter = surfParams.find(surfaceList[whichStruct]);
        if (iter == surfParams.end()) throw AlgorithmException(surfType + " surface required but not provided");
        const SurfaceFile* mySurf = iter->second.surface;
        const MetricFile* myAreas = iter->second.correctedAreas;
        if (mySurf->getNumberOfNodes() != myDenseMap.getSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
        checkStructureMatch(mySurf, surfaceList[whichStruct], "surface file", "the argument expects");
        if (myAreas != NULL)
        {
            if (myAreas->getNumberOfNodes() != mySurf->getNumberOfNodes())
            {
                throw AlgorithmException(surfType + " surface and vertex area metric have different number of vertices");
            }
            checkStructureMatch(myAreas, surfaceList[whichStruct], "vertex area metric", "the argument expects");
        }
    }
    myCiftiOut->setCiftiXML(myNewXML);
    if (ciftiVectorsOut != NULL)
    {
        ciftiVectorsOut->setCiftiXML(myVecXML);
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        auto iter = surfParams.find(surfaceList[whichStruct]);
        CaretAssert(iter != surfParams.end()); //we checked this above
        SurfaceFile* mySurf = iter->second.surface; //computeNormals() isn't const because it modifies an internal member, is used by metric gradient
        const MetricFile* myAreas = iter->second.correctedAreas;
        MetricFile myMetric, myRoi, myMetricOut, vectorsOut, *vectorPtr = NULL;
        if (ciftiVectorsOut != NULL) vectorPtr = &vectorsOut;
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &myRoi);
        AlgorithmMetricGradient(NULL, mySurf, &myMetric, &myMetricOut, vectorPtr, surfKern, &myRoi, false, -1, myAreas);
        if (outputAverage)
        {
            int numNodes = myMetricOut.getNumberOfNodes(), numCols = myMetricOut.getNumberOfColumns();
            vector<double> accum(numNodes, 0.0);//use double for numerical stability
            for (int i = 0; i < numCols; ++i)
            {
                const float* column = myMetricOut.getValuePointerForColumn(i);
                for (int j = 0; j < numNodes; ++j)
                {
                    accum[j] += column[j];
                }
            }
            vector<float> temparray(numNodes);//copy result into float array so it can be put into a metric, and then into cifti (yes, really)
            for (int i = 0; i < numNodes; ++i)
            {
                temparray[i] = (float)(accum[i] / numCols);
            }
            myMetricOut.setNumberOfNodesAndColumns(numNodes, 1);
            myMetricOut.setValuesForColumn(0, temparray.data());
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, CiftiXML::ALONG_COLUMN, surfaceList[whichStruct], &myMetricOut);//average always outputs a dscalar, so always along column
        } else {
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
            if (ciftiVectorsOut != NULL)
            {//is always a dscalar, so always use column
                AlgorithmCiftiReplaceStructure(NULL, ciftiVectorsOut, CiftiXML::ALONG_COLUMN, surfaceList[whichStruct], &vectorsOut);
            }
        }
    }
    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
    {
        VolumeFile myVol, myRoi, myVolOut, vecVolOut, *vecVolPtr = NULL;
        if (ciftiVectorsOut != NULL) vecVolPtr = &vecVolOut;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &myRoi, true);
        AlgorithmVolumeGradient(NULL, &myVol, &myVolOut, volKern, &myRoi, vecVolPtr);
        if (outputAverage)
        {
            vector<int64_t> myDims;
            myVolOut.getDimensions(myDims);
            int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
            vector<double> accum(frameSize, 0.0);
            for (int64_t i = 0; i < myDims[3]; ++i)
            {
                const float* myFrame = myVolOut.getFrame(i);
                for (int64_t j = 0; j < frameSize; ++j)
                {
                    accum[j] += myFrame[j];
                }
            }
            vector<float> temparray(frameSize);
            for (int64_t i = 0; i < frameSize; ++i)
            {
                temparray[i] = (float)(accum[i] / myDims[3]);
            }
            vector<int64_t> newDims = myDims;
            newDims.resize(3);
            myVolOut.reinitialize(newDims, myVol.getSform());
            myVolOut.setFrame(temparray.data());
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, CiftiXML::ALONG_COLUMN, volumeList[whichStruct], &myVolOut, true);
        } else {
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
            if (ciftiVectorsOut != NULL)
            {
                AlgorithmCiftiReplaceStructure(NULL, ciftiVectorsOut, CiftiXML::ALONG_COLUMN, volumeList[whichStruct], &vecVolOut, true);
            }
        }
    }
}

float AlgorithmCiftiGradient::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiGradient::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
