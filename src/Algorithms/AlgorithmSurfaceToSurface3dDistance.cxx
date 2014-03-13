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

#include "AlgorithmSurfaceToSurface3dDistance.h"
#include "AlgorithmException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceToSurface3dDistance::getCommandSwitch()
{
    return "-surface-to-surface-3d-distance";
}

AString AlgorithmSurfaceToSurface3dDistance::getShortDescription()
{
    return "COMPUTE DISTANCE BETWEEN CORRESPONDING VERTICES";
}

OperationParameters* AlgorithmSurfaceToSurface3dDistance::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-comp", "the surface to compare to the reference");
    
    ret->addSurfaceParameter(2, "surface-ref", "the surface to use as the reference");
    
    ret->addMetricOutputParameter(3, "dists-out", "the output distances");
    
    OptionalParameter* vectorOpt = ret->createOptionalParameter(4, "-vectors", "output the displacement vectors");
    vectorOpt->addMetricOutputParameter(1, "vectors-out", "the output vectors");
    
    ret->setHelpText(
        AString("Computes the vector difference between the vertices of each surface with the same index, as (comp - ref), ") +
        "and output the magnitudes, and optionally the displacement vectors."
    );
    return ret;
}

void AlgorithmSurfaceToSurface3dDistance::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* myCompSurf = myParams->getSurface(1);
    SurfaceFile* myRefSurf = myParams->getSurface(2);
    MetricFile* distsOut = myParams->getOutputMetric(3);
    MetricFile* vectorsOut = NULL;
    OptionalParameter* vectorOpt = myParams->getOptionalParameter(4);
    if (vectorOpt->m_present)
    {
        vectorsOut = vectorOpt->getOutputMetric(1);
    }
    AlgorithmSurfaceToSurface3dDistance(myProgObj, myCompSurf, myRefSurf, distsOut, vectorsOut);
}

AlgorithmSurfaceToSurface3dDistance::AlgorithmSurfaceToSurface3dDistance(ProgressObject* myProgObj, const SurfaceFile* myCompSurf, const SurfaceFile* myRefSurf, MetricFile* distsOut, MetricFile* vectorsOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (!myCompSurf->hasNodeCorrespondence(*myRefSurf))
    {
        throw AlgorithmException("input surfaces must have node correspondence");
    }
    int numNodes = myCompSurf->getNumberOfNodes();
    distsOut->setNumberOfNodesAndColumns(numNodes, 1);
    distsOut->setStructure(myCompSurf->getStructure());
    distsOut->setMapName(0, "distance");
    if (vectorsOut != NULL)
    {
        vectorsOut->setNumberOfNodesAndColumns(numNodes, 3);
        vectorsOut->setStructure(myCompSurf->getStructure());
        vectorsOut->setMapName(0, "x displacement");
        vectorsOut->setMapName(1, "y displacement");
        vectorsOut->setMapName(2, "z displacement");
    }
    for (int i = 0; i < numNodes; ++i)
    {
        Vector3D ref = myRefSurf->getCoordinate(i);
        Vector3D comp = myCompSurf->getCoordinate(i);
        Vector3D diff = comp - ref;
        distsOut->setValue(i, 0, diff.length());
        if (vectorsOut != NULL)
        {
            vectorsOut->setValue(i, 0, diff[0]);
            vectorsOut->setValue(i, 1, diff[1]);
            vectorsOut->setValue(i, 2, diff[2]);
        }
    }
}

float AlgorithmSurfaceToSurface3dDistance::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceToSurface3dDistance::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
