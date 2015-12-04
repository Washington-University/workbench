
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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "AlgorithmSurfaceSmoothing.h"
#include "AlgorithmException.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

using namespace caret;

/**
 * \class caret::AlgorithmSurfaceSmoothing 
 * \brief SURFACE SMOOTHING
 *
 * Smooths a surface.
 */

/**
 * @return Command line switch
 */
AString
AlgorithmSurfaceSmoothing::getCommandSwitch()
{
    return "-surface-smoothing";
}

/**
 * @return Short description of algorithm
 */
AString
AlgorithmSurfaceSmoothing::getShortDescription()
{
    return "SURFACE SMOOTHING";
}

/**
 * @return Parameters for algorithm
 */
OperationParameters*
AlgorithmSurfaceSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface-in", "the surface file to smooth");
    
    ret->addDoubleParameter(2, "smoothing-strength", "smoothing strength (ranges [0.0 - 1.0])");
    
    ret->addIntegerParameter(3, "smoothing-iterations", "smoothing iterations");
    
    ret->addSurfaceOutputParameter(4, "surface-out", "output surface file");
    
    AString helpText = ("Smooths a surface by averaging vertex coordinates with those of the neighboring vertices.");

    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform algorithm
 * @param myParams
 *     Parameters for algorithm
 * @param myProgObj
 *     The progress object
 * @throws
 *     AlgorithmException if errors
 */
void
AlgorithmSurfaceSmoothing::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    SurfaceFile* surfaceIn = myParams->getSurface(1);
    const float strength = myParams->getDouble(2);
    const int32_t iterations = myParams->getInteger(3);
    SurfaceFile* surfaceOut = myParams->getOutputSurface(4);
    
    
    /*
     * Constructs and executes the algorithm 
     */
    AlgorithmSurfaceSmoothing(myProgObj,
                              surfaceIn,
                              surfaceOut,
                              strength,
                              iterations);
    
}

/**
 * Constructor
 *
 * Calling the constructor will execute the algorithm
 *
 * @param myProgObj
 *     Parameters for algorithm
 */
AlgorithmSurfaceSmoothing::AlgorithmSurfaceSmoothing(ProgressObject* myProgObj,
                                                     const SurfaceFile* inputSurfaceFile,
                                                     SurfaceFile* outputSurfaceFile,
                                                     const float strength,
                                                     const int32_t iterations)
   : AbstractAlgorithm(myProgObj)
{
    if ((strength < 0.0)
        || (strength > 1.0)) {
        throw AlgorithmException("Invalid smoothing strength outside [0.0, 1.0]: "
                                 + QString::number(strength, 'f', 5));
    }
    
    if (iterations <= 0) {
        throw AlgorithmException("Invalid iterations value [1, infinity]: "
                                 + QString::number(iterations));
    }
    
    /*
     * Sets the algorithm up to use the progress object, and will 
     * finish the progress object automatically when the algorithm terminates
     */
    LevelProgress myProgress(myProgObj);
    
    *outputSurfaceFile = *inputSurfaceFile;
    
    CaretPointer<TopologyHelper> myTopoHelp = outputSurfaceFile->getTopologyHelper(true);
    
    const int32_t numNodes = outputSurfaceFile->getNumberOfNodes();
    if (numNodes <= 0) {
        return;
    }
    
    /*
     * Storage for coordinates, input and output of each iteration
     */
    std::vector<float> coordsIn(numNodes * 3);
    std::vector<float> coordsOut(numNodes * 3);
    
    /*
     * Copy coordinates from surface
     */
    for (int32_t i = 0; i < numNodes; i++) {
        const float* xyz = outputSurfaceFile->getCoordinate(i);
        
        const int32_t i3 = i * 3;
        coordsIn[i3]   = xyz[0];
        coordsIn[i3+1] = xyz[1];
        coordsIn[i3+2] = xyz[2];
        coordsOut[i3]   = xyz[0];
        coordsOut[i3+1] = xyz[1];
        coordsOut[i3+2] = xyz[2];
    }
    
    std::vector<float> triangleAreas(100);
    std::vector<float> triangleCenters(100*3);
    
    const float inverseStrength = 1.0 - strength;
    
    /*
     * Perform the requested number of iterations
     */
    for (int32_t iter = 1; iter <= iterations; iter++) {
        /*
         * Copy coordinates from previous iteration
         */
        if (iter > 1) {
            coordsIn = coordsOut;
        }
        
        /*
         * Process each node
         */
        for (int32_t iNode = 0; iNode < numNodes; iNode++) {
            /*
             * Get node's neighbors
             */
            int32_t numNeighbors = 0;
            const int32_t* neighbors = myTopoHelp->getNodeNeighbors(iNode, numNeighbors);
            
            if (numNeighbors < 2) {
                coordsOut[iNode*3]   = coordsIn[iNode*3];
                coordsOut[iNode*3+1] = coordsIn[iNode*3+1];
                coordsOut[iNode*3+2] = coordsIn[iNode*3+2];
            }
            else {
                /*
                 * Ensure adequate space for triangle areas and center coordinate
                 */
                if (numNeighbors > static_cast<int32_t>(triangleAreas.size())) {
                    triangleAreas.resize(numNeighbors);
                    triangleCenters.resize(numNeighbors * 3);
                }
                double totalArea = 0.0;
                
                /*
                 * Average node with its neighbors
                 */
                for (int jn = 0; jn < numNeighbors; jn++) {
                    /*
                     * Get two consecutive neighbors
                     */
                    const int32_t n1 = neighbors[jn];
                    int nextNeighborIndex = jn + 1;
                    if (nextNeighborIndex >= numNeighbors) {
                        nextNeighborIndex = 0;
                    }
                    const int32_t n2 = neighbors[nextNeighborIndex];
                    
                    /*
                     * Coordinates of nodes and neighbors
                     */
                    const float* c1 = &coordsIn[iNode*3];
                    const float* c2 = &coordsIn[n1*3];
                    const float* c3 = &coordsIn[n2*3];
                    const float area = MathFunctions::triangleArea(c1,
                                                                   c2,
                                                                   c3);
                    
                    /*
                     * Area of triangle formed by node and neighbors
                     */
                    triangleAreas[jn] = area;
                    totalArea += area;
                    
                    /*
                     * Average of nodes that form triangle
                     */
                    for (int32_t k = 0; k < 3; k++) {
                        triangleCenters[jn*3+k] = (c1[k] + c2[k] + c3[k]) / 3.0;
                    }
                }
                    
                /*
                 * Influence of neighbors
                 */
                float neighborAverageX = 0.0;
                float neighborAverageY = 0.0;
                float neighborAverageZ = 0.0;
                for (int j = 0; j < numNeighbors; j++) {
                    if (triangleAreas[j] > 0.0) {
                        const float weight = triangleAreas[j] / totalArea;
                        neighborAverageX += (weight * triangleCenters[j*3]);
                        neighborAverageY += (weight * triangleCenters[j*3+1]);
                        neighborAverageZ += (weight * triangleCenters[j*3+2]);
                    }
                }
                
                /*
                 * Update coordinates
                 */
                coordsOut[iNode*3]   = ((coordsIn[iNode*3] * inverseStrength)
                                        + (neighborAverageX * strength));
                coordsOut[iNode*3+1] = ((coordsIn[iNode*3+1] * inverseStrength)
                                        + (neighborAverageY * strength));
                coordsOut[iNode*3+2] = ((coordsIn[iNode*3+2] * inverseStrength)
                                        + (neighborAverageZ * strength));
            }
        }
        
        /*
         * Update progress
         */
        const float percentDone = (static_cast<float>(iter)
                                    / static_cast<float>(iterations));
        myProgress.reportProgress(percentDone);//give continuous updates, if it slows things down we can reduce the resolution in the progress framework
        
    }

    /*
     * Copy coordinates into surface
     */
    outputSurfaceFile->setCoordinates(&coordsOut[0]);

    myProgress.reportProgress(1.0f);
}


/**
 * @return Algorithm internal weight
 */
float
AlgorithmSurfaceSmoothing::getAlgorithmInternalWeight()
{
    /*
     * override this if needed, if the progress bar isn't smooth
     */
    return 1.0f;
}

/**
 * @return Algorithm sub-algorithm weight
 */
float
AlgorithmSurfaceSmoothing::getSubAlgorithmWeight()
{
    /*
     * If you use a subalgorithm
     */
    //return AlgorithmInsertNameHere::getAlgorithmWeight()
    return 0.0f;
}

