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
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(5, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(6, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    OptionalParameter* presmoothSurfOpt = ret->createOptionalParameter(7, "-surface-presmooth", "smooth on the surface before computing the gradient");
    presmoothSurfOpt->addDoubleParameter(1, "surface-kernel", "the sigma for the gaussian surface smoothing kernel, in mm");
    
    OptionalParameter* presmoothVolOpt = ret->createOptionalParameter(8, "-volume-presmooth", "smooth on the surface before computing the gradient");
    presmoothVolOpt->addDoubleParameter(1, "volume-kernel", "the sigma for the gaussian volume smoothing kernel, in mm");
    
    ret->createOptionalParameter(9, "-average-output", "output the average of the gradient magnitude maps instead of each gradient map separately");
    
    ret->setHelpText(
        AString("Performs gradient calculation on each component of the cifti file, and optionally averages the resulting gradients.  ") +
        "You must specify a surface for each surface structure in the cifti file.  The COLUMN direction should be faster, and is the " +
        "direction that works on dtseries.  For dconn, you probably want ROW, unless you are using -average-output."
    );
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
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(4);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(5);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(6);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
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
    bool outputAverage = myParams->getOptionalParameter(9)->m_present;
    AlgorithmCiftiGradient(myProgObj, myCifti, myDir, myCiftiOut, surfKern, volKern, myLeftSurf, myRightSurf, myCerebSurf, outputAverage);//executes the algorithm
}

AlgorithmCiftiGradient::AlgorithmCiftiGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir,
                                               CiftiFile* myCiftiOut, const float& surfKern, const float& volKern, SurfaceFile* myLeftSurf, SurfaceFile* myRightSurf,
                                               SurfaceFile* myCerebSurf, bool outputAverage) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXMLOld& myXML = myCifti->getCiftiXMLOld();
    CiftiXMLOld myNewXML = myXML;
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (myDir == CiftiXMLOld::ALONG_COLUMN)
    {
        if (!myXML.getStructureListsForColumns(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
        if (outputAverage)
        {
            myNewXML.resetRowsToScalars(1);
            myNewXML.setMapNameForRowIndex(0, "gradient average");
        }
    } else {
        if (!myXML.getStructureListsForRows(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
        if (outputAverage)
        {
            myNewXML.applyRowMapToColumns();//dscalar always has brainordinates on columns, so flip it
            myNewXML.resetRowsToScalars(1);
            myNewXML.setMapNameForRowIndex(0, "gradient average");
        }
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        SurfaceFile* mySurf = NULL;
        AString surfType;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                surfType = "left";
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                surfType = "right";
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                surfType = "cerebellum";
                break;
            default:
                throw AlgorithmException("found surface model with incorrect type: " + StructureEnum::toName(surfaceList[whichStruct]));
                break;
        }
        if (mySurf == NULL)
        {
            throw AlgorithmException(surfType + " surface required but not provided");
        }
        if (myDir == CiftiXMLOld::ALONG_COLUMN)
        {
            if (mySurf->getNumberOfNodes() != myXML.getColumnSurfaceNumberOfNodes(surfaceList[whichStruct]))
            {
                throw AlgorithmException(surfType + " surface has the wrong number of vertices");
            }
        } else {
            if (mySurf->getNumberOfNodes() != myXML.getRowSurfaceNumberOfNodes(surfaceList[whichStruct]))
            {
                throw AlgorithmException(surfType + " surface has the wrong number of vertices");
            }
        }
    }
    myCiftiOut->setCiftiXML(myNewXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        SurfaceFile* mySurf = NULL;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                break;
            default:
                break;
        }
        MetricFile myMetric, myRoi, myMetricOut;
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &myRoi);
        AlgorithmMetricGradient(NULL, mySurf, &myMetric, &myMetricOut, NULL, surfKern, &myRoi);
        if (outputAverage)
        {
            int numNodes = myMetricOut.getNumberOfNodes(), numCols = myMetricOut.getNumberOfColumns();
            CaretArray<double> accum(numNodes, 0.0);//use double for numerical stability
            for (int i = 0; i < numCols; ++i)
            {
                const float* column = myMetricOut.getValuePointerForColumn(i);
                for (int j = 0; j < numNodes; ++j)
                {
                    accum[j] += column[j];
                }
            }
            CaretArray<float> temparray(numNodes);//copy result into float array so it can be put into a metric, and then into cifti (yes, really)
            for (int i = 0; i < numNodes; ++i)
            {
                temparray[i] = (float)(accum[i] / numCols);
            }
            myMetricOut.setNumberOfNodesAndColumns(numNodes, 1);
            myMetricOut.setValuesForColumn(0, temparray);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, CiftiXMLOld::ALONG_COLUMN, surfaceList[whichStruct], &myMetricOut);//average always outputs a dtseries, so always along column
        } else {
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
        }
    }
    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
    {
        VolumeFile myVol, myRoi, myVolOut;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &myRoi, true);
        AlgorithmVolumeGradient(NULL, &myVol, &myVolOut, volKern, &myRoi);
        if (outputAverage)
        {
            vector<int64_t> myDims;
            myVolOut.getDimensions(myDims);
            int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
            CaretArray<double> accum(frameSize, 0.0);
            for (int i = 0; i < myDims[3]; ++i)
            {
                const float* myFrame = myVolOut.getFrame(i);
                for (int64_t j = 0; j < frameSize; ++j)
                {
                    accum[j] += myFrame[j];
                }
            }
            CaretArray<float> temparray(frameSize);
            for (int i = 0; i < frameSize; ++i)
            {
                temparray[i] = (float)(accum[i] / myDims[3]);
            }
            myDims.resize(3);
            myVolOut.reinitialize(myDims, myVol.getSform());
            myVolOut.setFrame(temparray);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, CiftiXMLOld::ALONG_COLUMN, volumeList[whichStruct], &myVolOut, true);
        } else {
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
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
