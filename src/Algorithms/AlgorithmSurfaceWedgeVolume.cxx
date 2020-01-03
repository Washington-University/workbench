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

#include "AlgorithmSurfaceWedgeVolume.h"
#include "AlgorithmException.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceWedgeVolume::getCommandSwitch()
{
    return "-surface-wedge-volume";
}

AString AlgorithmSurfaceWedgeVolume::getShortDescription()
{
    return "MEASURE PER-VERTEX VOLUME BETWEEN SURFACES";
}

OperationParameters* AlgorithmSurfaceWedgeVolume::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "inner-surface", "the inner surface");
    
    ret->addSurfaceParameter(2, "outer-surface", "the outer surface");
    
    ret->addMetricOutputParameter(3, "metric", "the output metric");
    
    ret->setHelpText(
        AString("Compute the volume of each vertex's area from one surface to another.  ") +
        "The surfaces must have vertex correspondence, and have consistent triangle orientation."
    );
    return ret;
}

void AlgorithmSurfaceWedgeVolume::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* innerSurf = myParams->getSurface(1);
    SurfaceFile* outerSurf = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    AlgorithmSurfaceWedgeVolume(myProgObj, innerSurf, outerSurf, myMetricOut);
}

AlgorithmSurfaceWedgeVolume::AlgorithmSurfaceWedgeVolume(ProgressObject* myProgObj, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, MetricFile* myMetricOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = innerSurf->getNumberOfNodes();
    if (numNodes != outerSurf->getNumberOfNodes())
    {
        throw AlgorithmException("input surfaces have different number of nodes");
    }
    myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
    myMetricOut->setStructure(innerSurf->getStructure());
    myMetricOut->setColumnName(0, "per-vertex volume between surfaces");
    vector<float> tempCol(numNodes, 0.0f);
    const float* innerCoords = innerSurf->getCoordinateData();
    const float* outerCoords = outerSurf->getCoordinateData();
    int numTiles = innerSurf->getNumberOfTriangles();
    for (int i = 0; i < numTiles; ++i)
    {
        const int32_t* innerTile = innerSurf->getTriangle(i);
        Vector3D innerPoints[3], outerPoints[3];
        for (int j = 0; j < 3; ++j)
        {
            innerPoints[j] = innerCoords + innerTile[j] * 3;
            outerPoints[j] = outerCoords + innerTile[j] * 3;
        }
        Vector3D refPoint = innerPoints[0];//use an "origin" for the vector field that is near the polyhedron for numerical stability - also, this makes several sides have zero contribution
        float volume = 0.0f;//I am ignoring the 1/3 in the vector field and the 1/2 for area of triangle for now, because they apply to everything so we can multiple later
        //NOTE: the inner surface triangle has zero contribution due to choice of reference point
        volume += (outerPoints[0] - refPoint).dot((outerPoints[1] - outerPoints[0]).cross(outerPoints[2] - outerPoints[1]));//we must be VERY careful with orientations
        int m = 2;
        for (int k = 0; k < 3; ++k)//walk the side quadrilaterals as 2->0, 0->1, 1->2
        {//add the average of the two triangulations of the quadrilateral
            Vector3D mref = innerPoints[m] - refPoint, kref = innerPoints[k] - refPoint;//precalculate reference points on the white surface for use in both volumes
            Vector3D whiteEdge = innerPoints[k] - innerPoints[m];//precalculate this frequently used edge also
            if (k != 0 && m != 0) volume += 0.5f * mref.dot(whiteEdge.cross(outerPoints[k] - innerPoints[k]));//don't calculate them when their contribution is 0, due to containing the reference point
            if (m != 0) volume += 0.5f * mref.dot((outerPoints[m] - innerPoints[m]).cross(innerPoints[m] - outerPoints[k]));
            if (k != 0 && m != 0) volume += 0.5f * kref.dot(whiteEdge.cross(outerPoints[m] - innerPoints[k]));
            if (k != 0) volume += 0.5f * kref.dot((outerPoints[k] - innerPoints[k]).cross(outerPoints[m] - outerPoints[k]));
            m = k;
        }
        volume /= 18.0f;//1/3 for vector field, 1/2 for triangle, and 1/3 to split the tile volume among its 3 vertices
        for (int j = 0; j < 3; ++j)
        {
            tempCol[innerTile[j]] += volume;
        }
    }
    myMetricOut->setValuesForColumn(0, tempCol.data());
}

float AlgorithmSurfaceWedgeVolume::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceWedgeVolume::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
