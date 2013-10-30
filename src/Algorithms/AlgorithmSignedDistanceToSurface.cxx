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

#include "AlgorithmSignedDistanceToSurface.h"
#include "AlgorithmException.h"
#include "CaretOMP.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmSignedDistanceToSurface::getCommandSwitch()
{
    return "-signed-distance-to-surface";
}

AString AlgorithmSignedDistanceToSurface::getShortDescription()
{
    return "COMPUTE SIGNED DISTANCE FROM ONE SURFACE TO ANOTHER";
}

OperationParameters* AlgorithmSignedDistanceToSurface::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-comp", "the comparison surface to measure the signed distance on");
    
    ret->addSurfaceParameter(2, "surface-ref", "the reference surface that defines the signed distance function");
    
    ret->addMetricOutputParameter(3, "metric", "the output metric");
    
    OptionalParameter* windingMethodOpt = ret->createOptionalParameter(4, "-winding", "winding method for point inside surface test");
    windingMethodOpt->addStringParameter(1, "method", "name of the method (default EVEN_ODD)");
    
    ret->setHelpText(
        AString("Compute the signed distance function of the reference surface at every vertex on the comparison surface.  ") +
        "NOTE: this relation is NOT symmetric, the line from a vertex to the closest point on the 'ref' surface " +
        "(the one that defines the signed distance function) will only align with the normal of the 'ref' surface.  Valid specifiers for winding methods are as follows:\n\n" + 
        "EVEN_ODD (default)\nNEGATIVE\nNONZERO\nNORMALS\n\nThe NORMALS method uses the normals of triangles and edges, or the closest triangle hit by a ray from the point.  " +
        "This method may be slightly faster, but is only reliable for a closed surface that does not cross through itself.  All other methods count entry (positive) and " +
        "exit (negative) crossings of a vertical ray from the point, then counts as inside if the total is odd, negative, or nonzero, respectively."
    );
    return ret;
}

void AlgorithmSignedDistanceToSurface::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* testSurf = myParams->getSurface(1);
    SurfaceFile* levelSetSurf = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    SignedDistanceHelper::WindingLogic myWinding = SignedDistanceHelper::EVEN_ODD;
    OptionalParameter* windingMethodOpt = myParams->getOptionalParameter(4);
    if (windingMethodOpt->m_present)
    {
        AString methodName = windingMethodOpt->getString(1);
        if (methodName == "EVEN_ODD")
        {
            myWinding = SignedDistanceHelper::EVEN_ODD;
        } else if (methodName == "NEGATIVE") {
            myWinding = SignedDistanceHelper::NEGATIVE;
        } else if (methodName == "NONZERO") {
            myWinding = SignedDistanceHelper::NONZERO;
        } else if (methodName == "NORMALS") {
            myWinding = SignedDistanceHelper::NORMALS;
        } else {
            throw AlgorithmException("unrecognized winding method");
        }
    }
    AlgorithmSignedDistanceToSurface(myProgObj, testSurf, levelSetSurf, myMetricOut, myWinding);
}

AlgorithmSignedDistanceToSurface::AlgorithmSignedDistanceToSurface(ProgressObject* myProgObj, const SurfaceFile* testSurf, const SurfaceFile* levelSetSurf, MetricFile* myMetricOut, SignedDistanceHelper::WindingLogic myWinding) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = testSurf->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
    myMetricOut->setStructure(testSurf->getStructure());
#pragma omp CARET_PAR
    {
        CaretPointer<SignedDistanceHelper> myHelp = levelSetSurf->getSignedDistanceHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < numNodes; ++i)
        {
            myMetricOut->setValue(i, 0, myHelp->dist(testSurf->getCoordinate(i), myWinding));
        }
    }
}

float AlgorithmSignedDistanceToSurface::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSignedDistanceToSurface::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
