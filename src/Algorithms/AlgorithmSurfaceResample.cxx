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

#include "AlgorithmSurfaceResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "SurfaceFile.h"
#include "SurfaceResamplingHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceResample::getCommandSwitch()
{
    return "-surface-resample";
}

AString AlgorithmSurfaceResample::getShortDescription()
{
    return "RESAMPLE A SURFACE TO A DIFFERENT MESH";
}

OperationParameters* AlgorithmSurfaceResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-in", "the surface file to resample");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface with the mesh that the input surface is currently on");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that is in register with <current-sphere> and has the desired output mesh");
    
    ret->addStringParameter(4, "method", "the method name");
    
    ret->addSurfaceOutputParameter(5, "surface-out", "the output surface file");
    
    //TODO: figure out if this should be here
    OptionalParameter* areaSurfsOpt = ret->createOptionalParameter(6, "-area-surfs", "specify surfaces to do vertex area correction based on");
    areaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant surface with <current-sphere> mesh");
    areaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant surface with <new-sphere> mesh");
    
    AString myHelpText =
        AString("Resamples a surface file, given two spherical surfaces that are in register.  ") +
        "If -area-surfs are not specified, the sphere surfaces are used for area correction, if the method used does area correction.  " +
        "This option is not used in normal circumstances, but is provided for completeness.\n\n" +
        "The <method> argument must be one of the following:\n\n";
    
    vector<SurfaceResamplingMethodEnum::Enum> allEnums;
    SurfaceResamplingMethodEnum::getAllEnums(allEnums);
    for (int i = 0; i < (int)allEnums.size(); ++i)
    {
        myHelpText += SurfaceResamplingMethodEnum::toName(allEnums[i]) + "\n";
    }
    
    myHelpText += AString("\nThe BARYCENTRIC method is recommended for anatomical surfaces, unless they are fairly rough, in order to minimize smoothing.  ") +
        "Instead of resampling a spherical surface, the -surface-sphere-project-unproject command is recommended when applicable.";
    ret->setHelpText(myHelpText);
    return ret;
}

void AlgorithmSurfaceResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* surfaceIn = myParams->getSurface(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum myMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(4), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid method name");
    }
    SurfaceFile* surfaceOut = myParams->getOutputSurface(5);
    SurfaceFile* curArea = curSphere, *newArea = newSphere;
    OptionalParameter* areaSurfsOpt = myParams->getOptionalParameter(6);
    if (areaSurfsOpt->m_present)
    {
        switch(myMethod)
        {
            case SurfaceResamplingMethodEnum::BARYCENTRIC:
                CaretLogInfo("This method does not use area correction, -area-surfs is not needed");
                break;
            default:
                break;
        }
        curArea = areaSurfsOpt->getSurface(1);
        newArea = areaSurfsOpt->getSurface(2);
    }
    AlgorithmSurfaceResample(myProgObj, surfaceIn, curSphere, newSphere, myMethod, surfaceOut, curArea, newArea);
}

AlgorithmSurfaceResample::AlgorithmSurfaceResample(ProgressObject* myProgObj, const SurfaceFile* surfaceIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                                   const SurfaceResamplingMethodEnum::Enum& myMethod, SurfaceFile* surfaceOut, const SurfaceFile* curArea, const SurfaceFile* newArea) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (surfaceIn->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("input surface has different number of nodes than input sphere");
    switch (myMethod)
    {
        case SurfaceResamplingMethodEnum::BARYCENTRIC:
            break;
        default:
            if (curArea == NULL || newArea == NULL) throw AlgorithmException("specified method does area correction, but no area surfaces given");
            if (curSphere->getNumberOfNodes() != curArea->getNumberOfNodes()) throw AlgorithmException("current area surface has different number of nodes than current sphere");
            if (newSphere->getNumberOfNodes() != newArea->getNumberOfNodes()) throw AlgorithmException("new area surface has different number of nodes than new sphere");
    }
    int numNewNodes = newSphere->getNumberOfNodes();
    *surfaceOut = *newSphere;
    surfaceOut->setStructure(newSphere->getStructure());
    surfaceOut->setSecondaryType(surfaceIn->getSecondaryType());
    surfaceOut->setSurfaceType(surfaceIn->getSurfaceType());
    vector<float> coordScratch(numNewNodes * 3, 0.0f);
    SurfaceResamplingHelper myHelp(myMethod, curSphere, newSphere, curArea, newArea);
    myHelp.resample3DCoord(surfaceIn->getCoordinateData(), coordScratch.data());
    surfaceOut->setCoordinates(coordScratch.data());
}

float AlgorithmSurfaceResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
