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

#include "AlgorithmFociResample.h"
#include "AlgorithmException.h"

#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "SurfaceFile.h"
#include "SurfaceProjector.h"

using namespace caret;
using namespace std;

AString AlgorithmFociResample::getCommandSwitch()
{
    return "-foci-resample";
}

AString AlgorithmFociResample::getShortDescription()
{
    return "PROJECT FOCI TO A DIFFERENT SURFACE";
}

OperationParameters* AlgorithmFociResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addFociParameter(1, "foci-in", "the input foci file");
    
    ret->addFociOutputParameter(2, "foci-out", "the output foci file");
    
    OptionalParameter* leftSurfaceOpt = ret->createOptionalParameter(3, "-left-surfaces", "the left surfaces for resampling");
    leftSurfaceOpt->addSurfaceParameter(1, "current-surf", "the surface the foci are currently projected on");
    leftSurfaceOpt->addSurfaceParameter(2, "new-surf", "the surface to project the foci onto");
    
    OptionalParameter* rightSurfaceOpt = ret->createOptionalParameter(4, "-right-surfaces", "the right surfaces for resampling");
    rightSurfaceOpt->addSurfaceParameter(1, "current-surf", "the surface the foci are currently projected on");
    rightSurfaceOpt->addSurfaceParameter(2, "new-surf", "the surface to project the foci onto");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(5, "-cerebellum-surfaces", "the cerebellum surfaces for resampling");
    cerebSurfaceOpt->addSurfaceParameter(1, "current-surf", "the surface the foci are currently projected on");
    cerebSurfaceOpt->addSurfaceParameter(2, "new-surf", "the surface to project the foci onto");
    
    ret->createOptionalParameter(6, "-discard-distance-from-surface", "ignore the distance the foci are above or below the current surface");
    
    ret->createOptionalParameter(7, "-restore-xyz", "put the original xyz coordinates into the foci, rather than the coordinates obtained from unprojection");
    
    ret->setHelpText(AString("Unprojects foci from the <current-surf> for the structure, then projects them to <new-surf>.  ") +
        "If the foci have meaningful distances above or below the surface, use anatomical surfaces.  " +
        "If the foci should be on the surface, use registered spheres and the options -discard-distance-from-surface and -restore-xyz.");
    return ret;
}

void AlgorithmFociResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    FociFile* fociIn = myParams->getFoci(1);
    FociFile* fociOut = myParams->getOutputFoci(2);
    SurfaceFile* leftCurSurf = NULL, *leftNewSurf = NULL;
    OptionalParameter* leftSurfaceOpt = myParams->getOptionalParameter(3);
    if (leftSurfaceOpt->m_present)
    {
        leftCurSurf = leftSurfaceOpt->getSurface(1);
        leftNewSurf = leftSurfaceOpt->getSurface(2);
    }
    SurfaceFile* rightCurSurf = NULL, *rightNewSurf = NULL;
    OptionalParameter* rightSurfaceOpt = myParams->getOptionalParameter(4);
    if (rightSurfaceOpt->m_present)
    {
        rightCurSurf = rightSurfaceOpt->getSurface(1);
        rightNewSurf = rightSurfaceOpt->getSurface(2);
    }
    SurfaceFile* cerebCurSurf = NULL, *cerebNewSurf = NULL;
    OptionalParameter* cerebSurfaceOpt = myParams->getOptionalParameter(5);
    if (cerebSurfaceOpt->m_present)
    {
        cerebCurSurf = cerebSurfaceOpt->getSurface(1);
        cerebNewSurf = cerebSurfaceOpt->getSurface(2);
    }
    bool discardNormDist = myParams->getOptionalParameter(6)->m_present;
    bool restoryXyz = myParams->getOptionalParameter(7)->m_present;
    AlgorithmFociResample(myProgObj, fociIn, fociOut, leftCurSurf, leftNewSurf, rightCurSurf, rightNewSurf, cerebCurSurf, cerebNewSurf, discardNormDist, restoryXyz);
}

AlgorithmFociResample::AlgorithmFociResample(ProgressObject* myProgObj, const FociFile* fociIn, FociFile* fociOut,
                                             const SurfaceFile* leftCurSurf, const SurfaceFile* leftNewSurf,
                                             const SurfaceFile* rightCurSurf, const SurfaceFile* rightNewSurf,
                                             const SurfaceFile* cerebCurSurf, const SurfaceFile* cerebNewSurf,
                                             const bool& discardNormDist, const bool& restoryXyz) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    checkStructureMatch(leftCurSurf, StructureEnum::CORTEX_LEFT, "current left surface", "-left-surfaces option expects");
    checkStructureMatch(leftNewSurf, StructureEnum::CORTEX_LEFT, "new left surface", "-left-surfaces option expects");
    checkStructureMatch(rightCurSurf, StructureEnum::CORTEX_RIGHT, "current right surface", "-right-surfaces option expects");
    checkStructureMatch(rightNewSurf, StructureEnum::CORTEX_RIGHT, "new right surface", "-right-surfaces option expects");
    checkStructureMatch(cerebCurSurf, StructureEnum::CEREBELLUM, "current cerebellum surface", "-cerebellum-surfaces option expects");
    checkStructureMatch(cerebNewSurf, StructureEnum::CEREBELLUM, "new cerebellum surface", "-cerebellum-surfaces option expects");
    CaretPointer<SurfaceProjector> leftProj, rightProj, cerebProj;
    if (leftNewSurf != NULL) leftProj.grabNew(new SurfaceProjector(leftNewSurf));
    if (rightNewSurf != NULL) rightProj.grabNew(new SurfaceProjector(rightNewSurf));
    if (cerebNewSurf != NULL) cerebProj.grabNew(new SurfaceProjector(cerebNewSurf));
    *(fociOut->getClassColorTable()) = *(fociIn->getClassColorTable());
    *(fociOut->getNameColorTable()) = *(fociIn->getNameColorTable());
    *(fociOut->getFileMetaData()) = *(fociIn->getFileMetaData());
    for (int i = 0; i < fociIn->getNumberOfFoci(); ++i)
    {
        const Focus* thisFocus = fociIn->getFocus(i);
        if (thisFocus->getNumberOfProjections() < 1)
        {
            throw AlgorithmException("focus '" + thisFocus->getName() + "' has no surface projection");
        }
        SurfaceProjector* myProj = NULL;
        const SurfaceFile* unprojFrom = NULL;
        switch (thisFocus->getProjection(0)->getStructure())
        {
            case StructureEnum::CORTEX_LEFT:
                myProj = leftProj;
                unprojFrom = leftCurSurf;
                break;
            case StructureEnum::CORTEX_RIGHT:
                myProj = rightProj;
                unprojFrom = rightCurSurf;
                break;
            case StructureEnum::CEREBELLUM:
                myProj = cerebProj;
                unprojFrom = cerebCurSurf;
                break;
            default:
                throw AlgorithmException("focus '" + thisFocus->getName() + "' has unsupported structure " + StructureEnum::toName(thisFocus->getProjection(0)->getStructure()));
        }
        if (unprojFrom == NULL || myProj == NULL) throw AlgorithmException("focus '" + thisFocus->getName() + "' has structure " +
            StructureEnum::toName(thisFocus->getProjection(0)->getStructure()) + ", but surfaces for that structure were not specified");
        CaretPointer<Focus> newFocus(new Focus(*thisFocus));//start with a copy
        float xyz[3];
        bool result = thisFocus->getProjection(0)->getProjectedPosition(*unprojFrom, xyz, discardNormDist);
        if (!result) throw AlgorithmException("failed to unproject focus '" + thisFocus->getName() + "'");
        newFocus->getProjection(0)->setStereotaxicXYZ(xyz);
        myProj->projectFocus(i, newFocus);
        if (restoryXyz)
        {
            newFocus->getProjection(0)->setStereotaxicXYZ(thisFocus->getProjection(0)->getStereotaxicXYZ());
        }
        fociOut->addFocus(newFocus.releasePointer());
    }
}

float AlgorithmFociResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmFociResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
