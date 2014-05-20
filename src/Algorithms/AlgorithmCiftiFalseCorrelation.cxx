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

#include "AlgorithmCiftiFalseCorrelation.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmMetricFalseCorrelation.h"
#include "CiftiFile.h"
#include "SurfaceFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiFalseCorrelation::getCommandSwitch()
{
    return "-cifti-false-correlation";
}

AString AlgorithmCiftiFalseCorrelation::getShortDescription()
{
    return "COMPARE CORRELATION LOCALLY AND ACROSS/THROUGH SULCI/GYRI";
}

OperationParameters* AlgorithmCiftiFalseCorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to use for correlation");
    
    ret->addDoubleParameter(2, "3D-dist", "maximum 3D distance to check around each vertex");
    
    ret->addDoubleParameter(3, "geo-outer", "maximum geodesic distance to use for neighboring correlation");
    
    ret->addDoubleParameter(4, "geo-inner", "minimum geodesic distance to use for neighboring correlation");
    
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti dscalar file");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(6, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    OptionalParameter* dumpLeftOpt = leftSurfOpt->createOptionalParameter(2, "-dump-text", "dump the raw measures used to a text file");
    dumpLeftOpt->addStringParameter(1, "text-out", "the output text file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(7, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    OptionalParameter* dumpRightOpt = rightSurfOpt->createOptionalParameter(2, "-dump-text", "dump the raw measures used to a text file");
    dumpRightOpt->addStringParameter(1, "text-out", "the output text file");
    
    OptionalParameter* cerebSurfOpt = ret->createOptionalParameter(8, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    OptionalParameter* dumpCerebOpt = cerebSurfOpt->createOptionalParameter(2, "-dump-text", "dump the raw measures used to a text file");
    dumpCerebOpt->addStringParameter(1, "text-out", "the output text file");
    
    ret->setHelpText(
        AString("This is where you set the help text.  DO NOT add the info about what the command line format is, ") +
        "and do not give the command switch, short description, or the short descriptions of parameters.  Do not indent, " +
        "add newlines, or format the text in any way other than to separate paragraphs within the help text prose."
    );
    return ret;
}

void AlgorithmCiftiFalseCorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiIn = myParams->getCifti(1);
    float max3D = (float)myParams->getDouble(2);
    float maxgeo = (float)myParams->getDouble(3);
    float mingeo = (float)myParams->getDouble(4);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(5);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    AString leftDumpName, rightDumpName, cerebDumpName;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(6);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
        OptionalParameter* dumpLeftOpt = leftSurfOpt->getOptionalParameter(2);
        if (dumpLeftOpt->m_present)
        {
            leftDumpName = dumpLeftOpt->getString(1);
        }
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(7);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
        OptionalParameter* dumpRightOpt = rightSurfOpt->getOptionalParameter(2);
        if (dumpRightOpt->m_present)
        {
            rightDumpName = dumpRightOpt->getString(1);
        }
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(8);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
        OptionalParameter* dumpCerebOpt = cerebSurfOpt->getOptionalParameter(2);
        if (dumpCerebOpt->m_present)
        {
            cerebDumpName = dumpCerebOpt->getString(1);
        }
    }
    AlgorithmCiftiFalseCorrelation(myProgObj, myCiftiIn, max3D, maxgeo, mingeo, myCiftiOut, myLeftSurf, leftDumpName, myRightSurf, rightDumpName, myCerebSurf, cerebDumpName);
}

AlgorithmCiftiFalseCorrelation::AlgorithmCiftiFalseCorrelation(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const float& max3D, const float& maxgeo, const float& mingeo,
                                                               CiftiFile* myCiftiOut, const SurfaceFile* myLeftSurf, const AString& leftDumpName,
                                                               const SurfaceFile* myRightSurf, const AString& rightDumpName, const SurfaceFile* myCerebSurf, const AString& cerebDumpName) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXMLOld& myXML = myCiftiIn->getCiftiXMLOld();
    CiftiXMLOld myNewXML = myXML;
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (!myXML.getStructureLists(CiftiXMLOld::ALONG_COLUMN, surfaceList, volumeList))
    {
        throw AlgorithmException("input cifti does not contain brainordinates along columns");
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
        if (mySurf->getNumberOfNodes() != myXML.getSurfaceNumberOfNodes(CiftiXMLOld::ALONG_COLUMN, surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
    }
    myNewXML.resetRowsToScalars(5);
    myNewXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 0, "correlation ratio");
    myNewXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 1, "non-neighborhood correlation");
    myNewXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 2, "average neighborhood correlation");
    myNewXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 3, "3D distance to non-neighborhood vertex");
    myNewXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 4, "non-neighborhood vertex number");
    myCiftiOut->setCiftiXML(myNewXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        const SurfaceFile* mySurf = NULL;
        AString dumpName;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                dumpName = leftDumpName;
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                dumpName = rightDumpName;
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                dumpName = cerebDumpName;
                break;
            default:
                break;
        }
        MetricFile myMetric, myRoi, myMetricOut;
        AlgorithmCiftiSeparate(NULL, myCiftiIn, CiftiXMLOld::ALONG_COLUMN, surfaceList[whichStruct], &myMetric, &myRoi);
        AlgorithmMetricFalseCorrelation(NULL, mySurf, &myMetric, &myMetricOut, max3D, maxgeo, mingeo, &myRoi, dumpName);
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, CiftiXMLOld::ALONG_COLUMN, surfaceList[whichStruct], &myMetricOut);
    }
    int64_t offset[3];
    vector<int64_t> dims(3);
    vector<vector<float> > sform;
    AlgorithmCiftiSeparate::getCroppedVolSpaceAll(myCiftiIn, CiftiXMLOld::ALONG_COLUMN, dims.data(), sform, offset);
    dims.push_back(5);
    VolumeFile zeros(dims, sform);
    zeros.setValueAllVoxels(0.0f);
    AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, CiftiXMLOld::ALONG_COLUMN, &zeros, true);
}

float AlgorithmCiftiFalseCorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiFalseCorrelation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
