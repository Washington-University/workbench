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

#include "AlgorithmSurfaceCurvature.h"
#include "AlgorithmException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceCurvature::getCommandSwitch()
{
    return "-surface-curvature";
}

AString AlgorithmSurfaceCurvature::getShortDescription()
{
    return "CALCULATE CURVATURE OF SURFACE";
}

OperationParameters* AlgorithmSurfaceCurvature::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to compute the curvature of");
    
    OptionalParameter* meanOpt = ret->createOptionalParameter(2, "-mean", "output mean curvature");
    meanOpt->addMetricOutputParameter(1, "mean-out", "mean curvature metric");
    
    OptionalParameter* gaussOpt = ret->createOptionalParameter(3, "-gauss", "output gaussian curvature");
    gaussOpt->addMetricOutputParameter(1, "gauss-out", "gaussian curvature metric");
    
    ret->setHelpText(
        AString("Compute the curvature of the surface, using the method from:\n") +
        "Interactive Texture Mapping by J. Maillot, Yahia, and Verroust, 1993.\n" +
        "ACM-0-98791-601-8/93/008"
    );
    return ret;
}

void AlgorithmSurfaceCurvature::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* meanOut = NULL, *gaussOut = NULL;
    OptionalParameter* meanOpt = myParams->getOptionalParameter(2);
    if (meanOpt->m_present)
    {
        meanOut = meanOpt->getOutputMetric(1);
    }
    OptionalParameter* gaussOpt = myParams->getOptionalParameter(3);
    if (gaussOpt->m_present)
    {
        gaussOut = gaussOpt->getOutputMetric(1);
    }
    AlgorithmSurfaceCurvature(myProgObj, mySurf, meanOut, gaussOut);
}

AlgorithmSurfaceCurvature::AlgorithmSurfaceCurvature(ProgressObject* myProgObj, const SurfaceFile* mySurf, MetricFile* meanOut, MetricFile* gaussOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (meanOut != NULL)
    {
        meanOut->setNumberOfNodesAndColumns(numNodes, 1);
        meanOut->setStructure(mySurf->getStructure());
        meanOut->setColumnName(0, "mean curvature");
    }
    if (gaussOut != NULL)
    {
        gaussOut->setNumberOfNodesAndColumns(numNodes, 1);
        gaussOut->setStructure(mySurf->getStructure());
        gaussOut->setColumnName(0, "gaussian curvature");
    }
    CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
    const float* normalData = mySurf->getNormalData();
    for (int i = 0; i < numNodes; ++i)
    {
        const vector<int32_t>& neighbors = myTopoHelp->getNodeNeighbors(i);
        int numNeigh = (int)neighbors.size();
        float k1 = 0.0f, k2 = 0.0f;
        if (numNeigh > 0)
        {
            Vector3D center = mySurf->getCoordinate(i);
            Vector3D normal = normalData + i * 3;
            Vector3D basisStart;//default constructor is 0 vector
            if (abs(normal[0]) > abs(normal[1]))
            {//a vector not parallel to the normal
                basisStart[1] = 1.0f;
            } else {
                basisStart[0] = 1.0f;
            }
            Vector3D ihat = normal.cross(basisStart).normal();
            Vector3D jhat = normal.cross(ihat);
            float sig_x = 0.0f, sig_xy = 0.0f, sig_y = 0.0f;
            float norm_x = 0.0f, norm_xy = 0.0f, norm_y = 0.0f;
            for (int j = 0; j < numNeigh; ++j)
            {//center node contributes 0 to each sum, so skip it
                Vector3D neighNormal = normalData + neighbors[j] * 3;
                Vector3D neighDiff = Vector3D(mySurf->getCoordinate(neighbors[j])) - center;
                float normProj[2] = { neighNormal.dot(ihat), neighNormal.dot(jhat) };
                float diffProj[2] = { neighDiff.dot(ihat), neighDiff.dot(jhat) };
                sig_x += diffProj[0] * diffProj[0];
                sig_xy += diffProj[0] * diffProj[1];
                sig_y += diffProj[1] * diffProj[1];
                norm_x += normProj[0] * diffProj[0];
                norm_xy += normProj[0] * diffProj[1] + normProj[1] * diffProj[0];
                norm_y += normProj[1] * diffProj[1];
            }
            float sig_xy2 = sig_xy * sig_xy;
            float denom = (sig_x + sig_y) * (-sig_xy2 + sig_x * sig_y);
            if (denom != 0.0f)
            {
                float a = (norm_x * (-sig_xy2 + sig_x * sig_y + sig_y * sig_y) -
                           norm_xy * sig_xy * sig_y +
                           norm_y * sig_xy2) / denom;
                float b = (-norm_x * sig_xy * sig_y +
                           norm_xy * sig_x * sig_y -
                           norm_y * sig_x * sig_xy) / denom;
                float c = (norm_x * sig_xy2 -
                           norm_xy * sig_x * sig_xy +
                           norm_y * (sig_x * sig_x - sig_xy2 + sig_x * sig_y)) / denom;
                float trC = a + c;
                float detC = a * c - b * b;
                float temp = trC * trC - 4 * detC;
                if (temp >= 0.0f)
                {
                    float delta = sqrt(temp);
                    k1 = (trC + delta) / 2;
                    k2 = (trC - delta) / 2;
                }
            }
        }
        if (meanOut != NULL)
        {
            meanOut->setValue(i, 0, (k1 + k2) / 2);
        }
        if (gaussOut != NULL)
        {
            gaussOut->setValue(i, 0, k1 * k2);
        }
    }
}

float AlgorithmSurfaceCurvature::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceCurvature::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
