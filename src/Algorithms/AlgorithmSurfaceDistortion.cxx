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

#include "AlgorithmSurfaceDistortion.h"
#include "AlgorithmException.h"

#include "AlgorithmMetricSmoothing.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceDistortion::getCommandSwitch()
{
    return "-surface-distortion";
}

AString AlgorithmSurfaceDistortion::getShortDescription()
{
    return "MEASURE DISTORTION BETWEEN SURFACES";
}

OperationParameters* AlgorithmSurfaceDistortion::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-reference", "the reference surface");
    
    ret->addSurfaceParameter(2, "surface-distorted", "the distorted surface");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output distortion metric");
    
    OptionalParameter* smoothOpt = ret->createOptionalParameter(4, "-smooth", "smooth the area data");
    smoothOpt->addDoubleParameter(1, "sigma", "the size of the smoothing kernel in mm, as sigma by default");
    smoothOpt->createOptionalParameter(2, "-fwhm", "kernel size is FWHM, not sigma");
    
    ret->createOptionalParameter(5, "-caret5-method", "use the surface distortion method from caret5");
    
    ret->createOptionalParameter(6, "-edge-method", "calculate distortion of edge lengths rather than areas");
    
    OptionalParameter* strainOpt = ret->createOptionalParameter(7, "-local-affine-method", "calculate distortion by the local affines between triangles");
    strainOpt->createOptionalParameter(1, "-log2", "apply base-2 log transform");
    
    ret->setHelpText(
        AString("This command, when not using -caret5-method, -edge-method, or -local-affine-method, is equivalent to using -surface-vertex-areas on each surface, ") +
        "smoothing both output metrics with the GEO_GAUSS_EQUAL method on the surface they came from if -smooth is specified, and then using the formula " +
        "'ln(distorted/reference)/ln(2)' on the smoothed results.\n\n" +
        "When using -caret5-method, it uses the surface distortion method from caret5, which takes the base 2 log of the ratio of tile areas, " +
        "then averages those results at each vertex, and then smooths the result on the reference surface.\n\n" +
        "When using -edge-method, the -smooth option is ignored, and the output at each vertex is the average of 'abs(ln(refEdge/distortEdge)/ln(2))' over all edges " +
        "connected to the vertex.\n\n" +
        "When using -local-affine-method, the -smooth option is ignored.  The output is two columns, the first is the area distortion ratio, and the second is anisotropic strain.  " +
        "These are calculated by an affine transform between matching triangles, and then averaged across the triangles of a vertex."
    );
    return ret;
}

void AlgorithmSurfaceDistortion::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* referenceSurf = myParams->getSurface(1);
    SurfaceFile* distortedSurf = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    float smooth = -1.0f;
    OptionalParameter* smoothOpt = myParams->getOptionalParameter(4);
    if (smoothOpt->m_present)
    {
        smooth = (float)smoothOpt->getDouble(1);
        if (smooth <= 0.0f) throw AlgorithmException("smoothing kernel must be positive if specified");
        if (smoothOpt->getOptionalParameter(2)->m_present)
        {
            smooth = smooth / (2.0f * sqrt(2.0f * log(2.0f)));
        }
    }
    int methodCount = 0;
    bool caret5method = myParams->getOptionalParameter(5)->m_present;
    if (caret5method) ++methodCount;
    bool edgeMethod = myParams->getOptionalParameter(6)->m_present;
    if (edgeMethod) ++methodCount;
    bool strainMethod = false;
    bool strainLog2 = false;
    OptionalParameter* strainOpt = myParams->getOptionalParameter(7);
    if (strainOpt->m_present)
    {
        strainMethod = true;
        ++methodCount;
        strainLog2 = strainOpt->getOptionalParameter(1)->m_present;
    }
    if (methodCount > 1) throw AlgorithmException("you may not specify more than one of -caret5-method, -edge-method, or -local-affine-method");
    AlgorithmSurfaceDistortion(myProgObj, referenceSurf, distortedSurf, myMetricOut, smooth, caret5method, edgeMethod, strainMethod, strainLog2);
}

AlgorithmSurfaceDistortion::AlgorithmSurfaceDistortion(ProgressObject* myProgObj, const SurfaceFile* referenceSurf, const SurfaceFile* distortedSurf,
                                                       MetricFile* myMetricOut, const float& smooth, const bool& caret5method, const bool& edgeMethod,
                                                       const bool& strainMethod, const bool& strainLog2) : AbstractAlgorithm(myProgObj)
{
    int methodCount = 0;
    if (caret5method) ++methodCount;
    if (edgeMethod) ++methodCount;
    if (strainMethod) ++methodCount;
    if (methodCount > 1) throw AlgorithmException("you may not use multiple method flags");
    ProgressObject* smoothRef = NULL, *smoothDistort = NULL, *caret5Smooth = NULL;
    if (myProgObj != NULL)
    {
        if (smooth > 0.0f && !edgeMethod && !strainMethod)
        {
            if (caret5method)
            {
                caret5Smooth = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
            } else {
                smoothRef = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
                smoothDistort = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
            }
        }
    }
    LevelProgress myProgress(myProgObj);
    if (!referenceSurf->hasNodeCorrespondence(*distortedSurf)) throw AlgorithmException("input surfaces must have node correspondence");
    int numNodes = referenceSurf->getNumberOfNodes();
    int numOutCols = 1;
    if (strainMethod) numOutCols = 2;
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numOutCols);
    myMetricOut->setStructure(referenceSurf->getStructure());
    if (caret5method)
    {
        myMetricOut->setColumnName(0, "area distortion (caret5)");
        int numTiles = referenceSurf->getNumberOfTriangles();
        vector<float> tilescratch(numTiles), nodescratch(numNodes);
        const float* refCoords = referenceSurf->getCoordinateData(), *distortCoords = distortedSurf->getCoordinateData();
        for (int i = 0; i < numTiles; ++i)
        {
            const int32_t* myTile = referenceSurf->getTriangle(i);
            int32_t offset[3] = { myTile[0] * 3, myTile[1] * 3, myTile[2] * 3 };
            float refarea = MathFunctions::triangleArea(refCoords + offset[0], refCoords + offset[1], refCoords + offset[2]);
            float distortarea = MathFunctions::triangleArea(distortCoords + offset[0], distortCoords + offset[1], distortCoords + offset[2]);
            if (refarea == 0.0f)
            {
                if (distortarea == 0.0f)
                {
                    tilescratch[i] = 0.0f;
                } else {
                    tilescratch[i] = log(10000.0) / log(2.0);//thats what it says
                }
            } else {
                if (distortarea / refarea < 0.00000001)
                {
                    tilescratch[i] = log(0.00000001) / log(2.0);
                } else {
                    tilescratch[i] = log(distortarea / refarea) / log(2.0);
                }
            }
        }
        CaretPointer<TopologyHelper> myhelp = referenceSurf->getTopologyHelper();
        for (int i = 0; i < numNodes; ++i)
        {
            const vector<int32_t>& myTiles = myhelp->getNodeTiles(i);
            int tileCount = (int)myTiles.size();
            double accum = 0.0;
            for (int j = 0; j < tileCount; ++j)
            {
                accum += tilescratch[myTiles[j]];
            }
            if (tileCount == 0)
            {
                nodescratch[i] = 0.0f;//actually, caret5 uses 1 here, but that is wrong, so fix it
            } else {
                nodescratch[i] = accum / tileCount;
            }
        }
        if (smooth > 0.0f)
        {
            MetricFile tempResult;
            tempResult.setNumberOfNodesAndColumns(numNodes, 1);
            tempResult.setValuesForColumn(0, nodescratch.data());//TSC: not sure what the "best" smoothing method here is, but there isn't a "correct" one because this method has flaws
            AlgorithmMetricSmoothing(caret5Smooth, referenceSurf, &tempResult, smooth, myMetricOut, NULL, false, false, -1, NULL, MetricSmoothingObject::GEO_GAUSS_AREA);
            myMetricOut->setStructure(referenceSurf->getStructure());//just in case we change where metric smoothing gets structure from
            myMetricOut->setColumnName(0, "area distortion (caret5)");
        } else {
            myMetricOut->setValuesForColumn(0, nodescratch.data());
        }
    } else if (edgeMethod) {
        myMetricOut->setColumnName(0, "edge distortion");
        CaretPointer<TopologyHelper> myhelp = referenceSurf->getTopologyHelper();
        const float* refCoords = referenceSurf->getCoordinateData(), *distortCoords = distortedSurf->getCoordinateData();
        for (int i = 0; i < numNodes; ++i)
        {
            Vector3D refCenter = refCoords + i * 3;
            Vector3D distortCenter = distortCoords + i * 3;
            const vector<int32_t>& neighbors = myhelp->getNodeNeighbors(i);
            int numNeigh = (int)neighbors.size();
            float accum = 0.0f;
            for (int j = 0; j < numNeigh; ++j)
            {
                Vector3D refNeigh = refCoords + neighbors[j] * 3;
                Vector3D distortNeigh = distortCoords + neighbors[j] * 3;
                float refLength = (refNeigh - refCenter).length();
                float distortLength = (distortNeigh - distortCenter).length();
                float ratio;
                if (refLength < distortLength)
                {
                    ratio = distortLength / refLength;
                } else {
                    ratio = refLength / distortLength;
                }
                accum += log(ratio) / log(2.0f);
            }
            myMetricOut->setValue(i, 0, accum / numNeigh);
        }
    } else if (strainMethod) {
        myMetricOut->setColumnName(0, "area ratio (J)");
        myMetricOut->setColumnName(1, "elongation ratio (R)");
        int numTris = referenceSurf->getNumberOfTriangles();
        vector<FloatMatrix> affines(numTris);
        for (int i = 0; i < numTris; ++i)
        {
            const int32_t* thisTri = referenceSurf->getTriangle(i);
            Vector3D allCoords[2][3];
            for (int j = 0; j < 3; ++j)
            {
                allCoords[0][j] = referenceSurf->getCoordinate(thisTri[j]);
                allCoords[1][j] = distortedSurf->getCoordinate(thisTri[j]);
            }
            float allCoords2D[2][3][2];//ref/dist, vert, x/y
            for (int k = 0; k < 2; ++k)
            {
                Vector3D iHat = (allCoords[k][1] - allCoords[k][0]).normal();//we don't need the triangle normal, so make the 2D basis without cross products
                Vector3D jHat = allCoords[k][2] - allCoords[k][0];
                jHat = (jHat - iHat * iHat.dot(jHat)).normal();
                for (int j = 0; j < 3; ++j)
                {
                    Vector3D tempCoord = allCoords[k][j] - allCoords[k][0];//use a triangle vertex as the origin to make the numbers smaller, less rounding error
                    allCoords2D[k][j][0] = iHat.dot(tempCoord);
                    allCoords2D[k][j][1] = jHat.dot(tempCoord);
                }
            }
            affines[i].resize(2, 2, true);
            FloatMatrix solver(3, 4);//3 equations, 3 unknowns
            for (int k = 0; k < 2; ++k)
            {
                for (int j = 0; j < 3; ++j)
                {
                    solver[j][0] = allCoords2D[0][j][0];//x
                    solver[j][1] = allCoords2D[0][j][1];//y
                    solver[j][2] = 1;
                    solver[j][3] = allCoords2D[1][j][k];//x' or y'
                }
                FloatMatrix solved = solver.reducedRowEchelon();
                affines[i][k][0] = solved[0][3];//x coef
                affines[i][k][1] = solved[1][3];//y coef
            }
        }
        int numNodes = referenceSurf->getNumberOfNodes();
        CaretPointer<TopologyHelper> myTopoHelp = referenceSurf->getTopologyHelper();
        for (int i = 0; i < numNodes; ++i)
        {
            const vector<int32_t>& myTiles = myTopoHelp->getNodeTiles(i);
            double accumJ = 0.0, accumR = 0.0;
            for (int j = 0; j < (int)myTiles.size(); ++j)
            {
                FloatMatrix tempMat = affines[myTiles[j]] * affines[myTiles[j]].transpose();
                float thisJ = sqrt(tempMat[0][0] * tempMat[1][1] - tempMat[0][1] * tempMat[1][0]);//written-out determinant for 2x2, because FloatMatrix doesn't currently have a generic determinant function
                float I1_st = (tempMat[0][0] + tempMat[1][1]) / thisJ;//2D formula
                float thisR;
                if (I1_st <= 2)
                {
                    thisR = 1.0;
                } else {
                    thisR = 0.5 * (I1_st + sqrt(I1_st * I1_st - 4));//convert I1* to (major strain) / (minor strain)
                }
                accumJ += thisJ;//not sure which areas to weight by, so for now do a straight average, relying on smoothness inherent in the measures
                accumR += thisR;//could maybe do some fancy interpolation of the affines in 3D sphere space
            }
            if (strainLog2)
            {
                myMetricOut->setValue(i, 0, log2(accumJ / myTiles.size()));
                myMetricOut->setValue(i, 1, log2(accumR / myTiles.size()));
            } else {
                myMetricOut->setValue(i, 0, accumJ / myTiles.size());
                myMetricOut->setValue(i, 1, accumR / myTiles.size());
            }
        }
    } else {
        myMetricOut->setColumnName(0, "area distortion");
        MetricFile refAreas, distortAreas;
        refAreas.setNumberOfNodesAndColumns(numNodes, 1);
        distortAreas.setNumberOfNodesAndColumns(numNodes, 1);
        vector<float> scratch;
        referenceSurf->computeNodeAreas(scratch);
        refAreas.setValuesForColumn(0, scratch.data());
        distortedSurf->computeNodeAreas(scratch);
        distortAreas.setValuesForColumn(0, scratch.data());
        MetricFile refSmoothed, distortSmoothed;
        const float* refData = refAreas.getValuePointerForColumn(0), *distortData = distortAreas.getValuePointerForColumn(0);
        if (smooth > 0.0f)
        {
            AlgorithmMetricSmoothing(smoothRef, referenceSurf, &refAreas, smooth, &refSmoothed, NULL, false, false, -1, NULL, MetricSmoothingObject::GEO_GAUSS_EQUAL);
            AlgorithmMetricSmoothing(smoothDistort, distortedSurf, &distortAreas, smooth, &distortSmoothed, NULL, false, false, -1, NULL, MetricSmoothingObject::GEO_GAUSS_EQUAL);
            refData = refSmoothed.getValuePointerForColumn(0);
            distortData = distortSmoothed.getValuePointerForColumn(0);
        }
        for (int i = 0; i < numNodes; ++i)
        {
            scratch[i] = log(distortData[i] / refData[i]) / log(2.0f);//should we sanity check these?
        }
        myMetricOut->setValuesForColumn(0, scratch.data());
    }
}

float AlgorithmSurfaceDistortion::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceDistortion::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
