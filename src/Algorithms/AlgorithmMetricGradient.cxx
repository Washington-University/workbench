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

#include "AlgorithmMetricGradient.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmMetricGradient::getCommandSwitch()
{
    return "-metric-gradient";
}

AString AlgorithmMetricGradient::getShortDescription()
{
    return "SURFACE GRADIENT OF A METRIC FILE";
}

AlgorithmParameters* AlgorithmMetricGradient::getParameters()
{
    AlgorithmParameters* ret = new AlgorithmParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to compute the gradient on");
    ret->addMetricParameter(2, "metric-in", "the metric to compute the gradient of");
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    OptionalParameter* presmooth = ret->createOptionalParameter(4, "-presmooth", "smooth the metric before computing the gradient");
    presmooth->addDoubleParameter(5, "presmoothing", "how much smoothing to apply");
    OptionalParameter* roiOption = ret->createOptionalParameter(6, "-roi", "select a region of interest to take the gradient of");
    roiOption->addMetricParameter(7, "roi-metric", "the area to smooth within, as a metric");
    OptionalParameter* columnSelect = ret->createOptionalParameter(8, "-column", "select a single column to compute the gradient of");
    columnSelect->addIntParameter(9, "column-num", "the column number");
    ret->createOptionalParameter(10, "-average-normals", "average the normals of each node with its neighbors before using them to compute the gradient");
    //that option has no parameters to take, so don't store the return value
    ret->setHelpText(
        AString("Takes the gradient of a metric file.  At each node, the immediate neighbors are unfolded onto a plane tangent to the surface at the node.  ") +
        "The gradient is computed by a regression between the unfolded positions of the nodes and their values.  " +
        "The gradient is then given by the slopes of the regression, and reconstructed as a 3d gradient vector.  " +
        "Currently, only the magnitude of the resulting vector is output.  " + 
        "By default, takes the gradient of all columns, with no presmoothing, across the whole surface, without averaging the normals of the surface among neighbors."
    );
    return ret;
}

void AlgorithmMetricGradient::useParameters(AlgorithmParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetricIn = myParams->getMetric(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    float myPresmooth = -1.0f;//negative or zero means no smoothing
    OptionalParameter* presmooth = myParams->getOptionalParameter(4);
    if (presmooth->m_present)
    {
        myPresmooth = (float)presmooth->getDouble(5);
    }
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(6);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getMetric(7);
    }
    int32_t myColumn = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(8);
    if (columnSelect->m_present)
    {
        myColumn = (int32_t)columnSelect->getInt(9);//todo: subtract one for 1-based conventions?
    }
    OptionalParameter* avgNormals = myParams->getOptionalParameter(10);
    bool myAvgNormals = avgNormals->m_present;
    AlgorithmMetricGradient(myProgObj, mySurf, myMetricIn, myMetricOut, myPresmooth, myAvgNormals, myRoi, myColumn);//executes the algorithm
}

AlgorithmMetricGradient::AlgorithmMetricGradient(ProgressObject* myProgObj, SurfaceFile* mySurf, MetricFile* myMetricIn, MetricFile* myMetricOut, const float myPresmooth, const bool myAvgNormals, MetricFile* myRoi, const int32_t myColumn) : AbstractAlgorithm(myProgObj)
{
    ProgressObject* smoothProgress = NULL;
    if (myProgObj != NULL && myPresmooth > 0.0f)
    {
        smoothProgress = myProgObj->addAlgorithm(AlgorithmMetricSmoothing::getAlgorithmWeight());
    }
    LevelProgress myProgress(myProgObj);
    int32_t numNodes = mySurf->getNumberOfNodes();
    if (myMetricIn->getNumberOfNodes() != numNodes)
    {//TODO: write these as static functions into an AlgorithmHelper class?
        throw AlgorithmException("metric does not match surface in number of nodes");
    }
    if (myRoi != NULL && myRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("roi metric does not match surface in number of nodes");
    }
    int32_t numColumns = myMetricIn->getNumberOfColumns();
    if (myColumn < -1 || myColumn >= numColumns)
    {
        throw AlgorithmException("invalid column number");
    }
    MetricFile* toProcess = myMetricIn;
    if (myPresmooth > 0.0f)
    {
        toProcess = new MetricFile();
        AlgorithmMetricSmoothing(smoothProgress, mySurf, myMetricIn, toProcess, myPresmooth, myRoi, myColumn);
    }
    mySurf->computeNormals(myAvgNormals);
    bool haveWarned = false, haveFailed = false;//print warning or failure messages only once
    if (myColumn == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
        const float* myRoiColumn;
        if (myRoi != NULL)
        {
            myRoiColumn = myRoi->getValuePointerForColumn(0);
        }
        for (int32_t col = 0; col < numColumns; ++col)
        {
            float somevec[3], xhat[3], yhat[3];
            float sanity;
            const float* myMetricColumn = myMetricIn->getValuePointerForColumn(col);
            CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
            for (int32_t i = 0; i < numNodes; ++i)
            {
                if (myRoi != NULL && myRoiColumn[i] <= 0.0f)
                {
                    myMetricOut->setValue(i, col, 0.0f);
                    continue;
                }
                int32_t numNeigh;
                const int32_t* myNeighbors = myTopoHelp->getNodeNeighbors(i, numNeigh);
                const float* myNormal = mySurf->getNormalVector(i);
                const float* myCoord = mySurf->getCoordinate(i);//TODO: make this not require a function call
                float nodeValue = myMetricColumn[i];
                somevec[2] = 0.0;
                if (myNormal[0] > myNormal[1])
                {//generate a vector not parallel to normal
                    somevec[0] = 0.0;
                    somevec[1] = 1.0;
                } else {
                    somevec[0] = 1.0;
                    somevec[1] = 0.0;
                }
                MathFunctions::crossProduct(myNormal, somevec, xhat);
                MathFunctions::normalizeVector(xhat);
                MathFunctions::crossProduct(myNormal, xhat, yhat);
                MathFunctions::normalizeVector(yhat);//xhat, yhat are orthogonal unit vectors describing the coord system with k = surface normal
                if (numNeigh >= 2)
                {
                    FloatMatrix myRegress = FloatMatrix::zeros(3, 4);
                    for (int32_t j = 0; j < numNeigh; ++j)
                    {
                        int32_t whichNode = myNeighbors[j];
                        if (myRoi == NULL || myRoiColumn[whichNode] > 0.0f)
                        {
                            float tempf = myMetricColumn[whichNode] - nodeValue;
                            const float* neighCoord = mySurf->getCoordinate(whichNode);//TODO: make this not require a function call
                            MathFunctions::subtractVectors(neighCoord, myCoord, somevec);
                            float origMag = MathFunctions::vectorLength(somevec);//save the original length
                            float xmag = MathFunctions::dotProduct(xhat, somevec);//dot product to get the direction in 2d
                            float ymag = MathFunctions::dotProduct(yhat, somevec);
                            float mag2d = sqrt(xmag * xmag + ymag * ymag);//get the new magnitude, to divide out
                            xmag *= origMag / mag2d;//normalize the 2d vector and multiply by original length to unfold
                            ymag *= origMag / mag2d;
                            myRegress[0][0] += xmag * xmag;//gather A'A and A'b sums for regression
                            myRegress[0][1] += xmag * ymag;
                            myRegress[0][2] += xmag;
                            myRegress[1][1] += ymag * ymag;
                            myRegress[1][2] += ymag;
                            myRegress[2][2] += 1.0f;
                            myRegress[0][3] += xmag * tempf;
                            myRegress[1][3] += ymag * tempf;
                            myRegress[2][3] += tempf;
                        }
                    }
                    myRegress[1][0] = myRegress[0][1];//complete the symmetric elements
                    myRegress[2][0] = myRegress[0][2];
                    myRegress[2][1] = myRegress[1][2];
                    myRegress[2][2] += 1.0f;//include center (metric and coord differences will be zero, so this is all that is needed)
                    FloatMatrix myRref = myRegress.reducedRowEchelon();
                    somevec[0] = xhat[0] * myRref[0][3] + yhat[0] * myRref[1][3];
                    somevec[1] = xhat[1] * myRref[0][3] + yhat[1] * myRref[1][3];
                    somevec[2] = xhat[2] * myRref[0][3] + yhat[2] * myRref[1][3];//somevec is now our surface gradient
                    sanity = somevec[0] + somevec[1] + somevec[2];
                }
                if (numNeigh > 0 && (numNeigh < 2 || sanity != sanity))
                {
                    if (!haveWarned && myRoi != NULL)
                    {//don't issue this warning with an ROI, because it is somewhat expected
                        haveWarned = true;
                        CaretLogWarning("WARNING: gradient calculation found a NaN/inf with regression method");
                    }
                    float xgrad = 0.0f, ygrad = 0.0f;
                    int32_t totalNeigh = 0;
                    for (int32_t j = 0; j < numNeigh; ++j)
                    {
                        int32_t whichNode = myNeighbors[j];
                        if (myRoi == NULL || myRoiColumn[whichNode] > 0.0f)
                        {
                            ++totalNeigh;
                            float tempf = myMetricColumn[whichNode] - nodeValue;
                            const float* neighCoord = mySurf->getCoordinate(whichNode);//TODO: make this not require a function call
                            MathFunctions::subtractVectors(neighCoord, myCoord, somevec);
                            float origMag = MathFunctions::vectorLength(somevec);//save the original length
                            float xmag = MathFunctions::dotProduct(xhat, somevec);//dot product to get the direction in 2d
                            float ymag = MathFunctions::dotProduct(yhat, somevec);
                            float mag2d = sqrt(xmag * xmag + ymag * ymag);//get the new magnitude, to divide out
                            tempf /= origMag * mag2d;//difference divided by distance gives point estimate of gradient magnitude, also divide by magnitude of 2d vector to normalize it at the same time
                            xgrad += xmag * tempf;//point estimate of gradient magnitude times normalized projected direction gives 2d estimate of gradient
                            ygrad += ymag * tempf;//average point estimates for each neighbor to estimate local gradient
                        }
                    }
                    xgrad /= totalNeigh;//average
                    ygrad /= totalNeigh;
                    somevec[0] = xhat[0] * xgrad + yhat[0] * ygrad;//unproject back into 3d
                    somevec[1] = xhat[1] * xgrad + yhat[1] * ygrad;
                    somevec[2] = xhat[2] * xgrad + yhat[2] * ygrad;
                }
                if (numNeigh <= 0 || sanity != sanity)
                {
                    if (!haveFailed)
                    {
                        haveFailed = true;
                        CaretLogWarning("Failed to compute gradient for a node with standard and fallback methods, outputting ZERO, check your surface for disconnected nodes or other strangeness");
                    }
                    somevec[0] = 0.0f;
                    somevec[1] = 0.0f;
                    somevec[2] = 0.0f;
                }
                //TODO: save the vector components in something
                myMetricOut->setValue(i, col, MathFunctions::vectorLength(somevec));
            }
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        const float* myRoiColumn;
        if (myRoi != NULL)
        {
            myRoiColumn = myRoi->getValuePointerForColumn(0);
        }
        float somevec[3], xhat[3], yhat[3];
        float sanity;
        const float* myMetricColumn = myMetricIn->getValuePointerForColumn(myColumn);
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoi != NULL && myRoiColumn[i] <= 0.0f)
            {
                myMetricOut->setValue(i, myColumn, 0.0f);
                continue;
            }
            int32_t numNeigh;
            const int32_t* myNeighbors = myTopoHelp->getNodeNeighbors(i, numNeigh);
            const float* myNormal = mySurf->getNormalVector(i);
            const float* myCoord = mySurf->getCoordinate(i);//TODO: make this not require a function call
            float nodeValue = myMetricColumn[i];
            somevec[2] = 0.0;
            if (myNormal[0] > myNormal[1])
            {//generate a vector not parallel to normal
            somevec[0] = 0.0;
            somevec[1] = 1.0;
            } else {
                somevec[0] = 1.0;
                somevec[1] = 0.0;
            }
            MathFunctions::crossProduct(myNormal, somevec, xhat);
            MathFunctions::normalizeVector(xhat);
            MathFunctions::crossProduct(myNormal, xhat, yhat);
            MathFunctions::normalizeVector(yhat);//xhat, yhat are orthogonal unit vectors describing the coord system with k = surface normal
            if (numNeigh >= 2)
            {
                FloatMatrix myRegress = FloatMatrix::zeros(3, 4);
                for (int32_t j = 0; j < numNeigh; ++j)
                {
                    int32_t whichNode = myNeighbors[j];
                    if (myRoi == NULL || myRoiColumn[whichNode] > 0.0f)
                    {
                        float tempf = myMetricColumn[whichNode] - nodeValue;
                        const float* neighCoord = mySurf->getCoordinate(whichNode);//TODO: make this not require a function call
                        MathFunctions::subtractVectors(neighCoord, myCoord, somevec);
                        float origMag = MathFunctions::vectorLength(somevec);//save the original length
                        float xmag = MathFunctions::dotProduct(xhat, somevec);//dot product to get the direction in 2d
                        float ymag = MathFunctions::dotProduct(yhat, somevec);
                        float mag2d = sqrt(xmag * xmag + ymag * ymag);//get the new magnitude, to divide out
                        xmag *= origMag / mag2d;//normalize the 2d vector and multiply by original length to unfold
                        ymag *= origMag / mag2d;
                        myRegress[0][0] += xmag * xmag;//gather A'A and A'b sums for regression
                        myRegress[0][1] += xmag * ymag;
                        myRegress[0][2] += xmag;
                        myRegress[1][1] += ymag * ymag;
                        myRegress[1][2] += ymag;
                        myRegress[2][2] += 1.0f;
                        myRegress[0][3] += xmag * tempf;
                        myRegress[1][3] += ymag * tempf;
                        myRegress[2][3] += tempf;
                    }
                }
                myRegress[1][0] = myRegress[0][1];//complete the symmetric elements
                myRegress[2][0] = myRegress[0][2];
                myRegress[2][1] = myRegress[1][2];
                myRegress[2][2] += 1.0f;//include center (metric and coord differences will be zero, so this is all that is needed)
                FloatMatrix myRref = myRegress.reducedRowEchelon();
                somevec[0] = xhat[0] * myRref[0][3] + yhat[0] * myRref[1][3];
                somevec[1] = xhat[1] * myRref[0][3] + yhat[1] * myRref[1][3];
                somevec[2] = xhat[2] * myRref[0][3] + yhat[2] * myRref[1][3];//somevec is now our surface gradient
                sanity = somevec[0] + somevec[1] + somevec[2];
            }
            if (numNeigh > 0 && (numNeigh < 2 || sanity != sanity))
            {
                if (!haveWarned && myRoi != NULL)
                {//don't issue this warning with an ROI, because it is somewhat expected
                haveWarned = true;
                CaretLogWarning("WARNING: gradient calculation found a NaN/inf with regression method");
                }
                float xgrad = 0.0f, ygrad = 0.0f;
                int32_t totalNeigh = 0;
                for (int32_t j = 0; j < numNeigh; ++j)
                {
                    int32_t whichNode = myNeighbors[j];
                    if (myRoi == NULL || myRoiColumn[whichNode] > 0.0f)
                    {
                        ++totalNeigh;
                        float tempf = myMetricColumn[whichNode] - nodeValue;
                        const float* neighCoord = mySurf->getCoordinate(whichNode);//TODO: make this not require a function call
                        MathFunctions::subtractVectors(neighCoord, myCoord, somevec);
                        float origMag = MathFunctions::vectorLength(somevec);//save the original length
                        float xmag = MathFunctions::dotProduct(xhat, somevec);//dot product to get the direction in 2d
                        float ymag = MathFunctions::dotProduct(yhat, somevec);
                        float mag2d = sqrt(xmag * xmag + ymag * ymag);//get the new magnitude, to divide out
                        tempf /= origMag * mag2d;//difference divided by distance gives point estimate of gradient magnitude, also divide by magnitude of 2d vector to normalize it at the same time
                        xgrad += xmag * tempf;//point estimate of gradient magnitude times normalized projected direction gives 2d estimate of gradient
                        ygrad += ymag * tempf;//average point estimates for each neighbor to estimate local gradient
                    }
                }
                xgrad /= totalNeigh;//average
                ygrad /= totalNeigh;
                somevec[0] = xhat[0] * xgrad + yhat[0] * ygrad;//unproject back into 3d
                somevec[1] = xhat[1] * xgrad + yhat[1] * ygrad;
                somevec[2] = xhat[2] * xgrad + yhat[2] * ygrad;
            }
            if (numNeigh <= 0 || sanity != sanity)
            {
                if (!haveFailed)
                {
                    haveFailed = true;
                    CaretLogWarning("Failed to compute gradient for a node with standard and fallback methods, outputting ZERO, check your surface for disconnected nodes or other strangeness");
                }
                somevec[0] = 0.0f;
                somevec[1] = 0.0f;
                somevec[2] = 0.0f;
            }
            //TODO: save the vector components in something
            myMetricOut->setValue(i, myColumn, MathFunctions::vectorLength(somevec));
        }
    }
    if (myPresmooth > 0.0f)
    {
        delete toProcess;
    }
}

float AlgorithmMetricGradient::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricGradient::getSubAlgorithmWeight()
{
    return AlgorithmMetricSmoothing::getAlgorithmWeight();//if you use a subalgorithm
}
