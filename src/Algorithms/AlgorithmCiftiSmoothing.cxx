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

#include "AlgorithmCiftiSmoothing.h"
#include "AlgorithmException.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmVolumeSmoothing.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "SurfaceFile.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiSmoothing::getCommandSwitch()
{
    return "-cifti-smoothing";
}

AString AlgorithmCiftiSmoothing::getShortDescription()
{
    return "SMOOTH A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    ret->addDoubleParameter(2, "surface-kernel", "the sigma for the gaussian surface smoothing kernel, in mm");
    ret->addDoubleParameter(3, "volume-kernel", "the sigma for the gaussian volume smoothing kernel, in mm");
    ret->addStringParameter(4, "direction", "which dimension to smooth along, ROW or COLUMN");
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(6, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(7, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(8, "-cereb-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    ret->createOptionalParameter(9, "-fix-zeros-volume", "treat values of zero in the volume as missing data");
    
    ret->createOptionalParameter(10, "-fix-zeros-surface", "treat values of zero on the surface as missing data");
    
    ret->setHelpText(
        AString("The input cifti file must have a brain models mapping on the chosen dimension, columns for .dtseries, and ") +
        "either for .dconn.  The fix zeros options will treat values of zero as lack of data, " +
        "and not use that value when generating the smoothed values, but will fill zeros with extrapolated values."
    );
    return ret;
}

void AlgorithmCiftiSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float surfKern = (float)myParams->getDouble(2);
    float volKern = (float)myParams->getDouble(3);
    AString directionName = myParams->getString(4);
    CiftiInterface::CiftiDirection myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiInterface::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiInterface::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(5);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(6);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(7);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(8);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
    }
    bool fixZerosVol = myParams->getOptionalParameter(9)->m_present;
    bool fixZerosSurf = myParams->getOptionalParameter(10)->m_present;
    AlgorithmCiftiSmoothing(myProgObj, myCifti, surfKern, volKern, myDir, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, fixZerosVol, fixZerosSurf);
}

AlgorithmCiftiSmoothing::AlgorithmCiftiSmoothing(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfKern, const float& volKern, CiftiInterface::CiftiDirection myDir, CiftiFile* myCiftiOut, const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf, bool fixZerosVol, bool fixZerosSurf) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = myCifti->getCiftiXML();
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (myDir == CiftiInterface::ALONG_COLUMN)
    {
        if (!myXML.getStructureListsForColumns(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
    } else {
        if (!myXML.getStructureListsForRows(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        const SurfaceFile* mySurf = NULL;
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
        if (myDir == CiftiInterface::ALONG_COLUMN)
        {
            if (mySurf->getNumberOfNodes() != myCifti->getColumnSurfaceNumberOfNodes(surfaceList[whichStruct]))
            {
                throw AlgorithmException(surfType + " surface has the wrong number of vertices");
            }
        } else {
            if (mySurf->getNumberOfNodes() != myCifti->getRowSurfaceNumberOfNodes(surfaceList[whichStruct]))
            {
                throw AlgorithmException(surfType + " surface has the wrong number of vertices");
            }
        }
    }
    myCiftiOut->setCiftiXML(myXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        const SurfaceFile* mySurf = NULL;
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
        AlgorithmMetricSmoothing(NULL, mySurf, &myMetric, surfKern, &myMetricOut, &myRoi, fixZerosSurf);
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
    }
    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
    {
        VolumeFile myVol, myRoi, myVolOut;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &myRoi, true);
        AlgorithmVolumeSmoothing(NULL, &myVol, volKern, &myVolOut, &myRoi, fixZerosVol);
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
    }
}

float AlgorithmCiftiSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiSmoothing::getSubAlgorithmWeight()
{
    return AlgorithmMetricSmoothing::getAlgorithmWeight() * 2 + AlgorithmVolumeSmoothing::getAlgorithmWeight();//if you use a subalgorithm
}
