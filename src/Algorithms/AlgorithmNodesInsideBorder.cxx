
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <iostream>

#define __ALGORITHM_NODES_INSIDE_BORDER_DECLARE__
#include "AlgorithmNodesInsideBorder.h"
#undef __ALGORITHM_NODES_INSIDE_BORDER_DECLARE__

#include "AlgorithmException.h"
#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GeodesicHelper.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "TopologyHelper.h"

using namespace caret;


    
/**
 * \class caret::AlgorithmNodesInsideBorder 
 * \brief Assign attribute values to nodes within a closed border.
 *
 * Identify nodes within a closed border and assign metric or
 * label values to those nodes.
 */

/**
 * Constructor.
 * 
 * @param myProgObj
 *    
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param assignToMetricMapIndex
 *    Map index in metric file to which assigments are made for
 *    nodes inside the border.
 * @param assignMetricValue
 *    Metric value assigned to nodes within the border.
 * @param metricFileInOut
 *    Metric file that has map set with nodes inside border.
 */
AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj, 
                           const SurfaceFile* surfaceFile,
                           const Border* border, 
                           const int32_t assignToMetricMapIndex,
                           const float assignMetricValue,
                           MetricFile* metricFileInOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    CaretAssert(metricFileInOut);

    const int32_t numberOfNodes = surfaceFile->getNumberOfNodes();
}

//AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj, 
//                           const SurfaceFile* surfaceFile,
//                           const Border* border, 
//                           const int32_t assignToLabelMapIndex,
//                           const AString& assignLabelName,
//                           LabelFile* labelFileInOut)
//: AbstractAlgorithm(myProgObj)
//{
//    
//}

/**
 * Find nodes inside the border.
 *
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param nodesInBorderOut
 *    Vector into which nodes inside border are loaded.
 */
void 
AlgorithmNodesInsideBorder::findNodesInsideBorder(const SurfaceFile* surfaceFile,
                                                  const Border* border,
                                                  std::vector<int32_t>& nodesInBorderOut)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    
    nodesInBorderOut.clear();
    
    /*
     * Move border points to the nearest nodes.
     */
    std::vector<int32_t> nodesAlongBorder;
    const int32_t originalNumberOfBorderPoints = border->getNumberOfPoints();
    for (int32_t i = 0; i < originalNumberOfBorderPoints; i++) {
        const SurfaceProjectedItem* spi = border->getPoint(i);
        float xyz[3];
        if (spi->getProjectedPosition(*surfaceFile, xyz, true)) {
            const int32_t nearestNode = surfaceFile->closestNode(xyz);
            if (nearestNode >= 0) {
                nodesAlongBorder.push_back(nearestNode);
            }
        }
    }
    
    /*
     * Make sure first and last nodes are not the same
     */
    int32_t numberOfPointsInBorder = static_cast<int32_t>(nodesAlongBorder.size());
    if (nodesAlongBorder.size() < 4) {
        throw AlgorithmException("Border is too small.  "
                                 "When moved to nearest nodes, border consists of four or fewer nodes.");
    }
    if (nodesAlongBorder[0] == nodesAlongBorder[numberOfPointsInBorder - 1]) {
        nodesAlongBorder.resize(numberOfPointsInBorder - 1);
    }
    numberOfPointsInBorder = static_cast<int32_t>(nodesAlongBorder.size());
    
    /*
     * Geodesic helper for surface
     */
    CaretPointer<GeodesicHelper> geodesicHelper = surfaceFile->getGeodesicHelper();
    
    /*
     * Find connected path along node neighbors
     */
    std::vector<int32_t> connectedPathNodes;
    for (int32_t i = 0; i < numberOfPointsInBorder; i++) {
        const int node = nodesAlongBorder[i];
        int nextNode = -1;
        const bool lastNodeFlag = (i >= (numberOfPointsInBorder - 1));
        if (lastNodeFlag) {
            nextNode = nodesAlongBorder[0];
        }
        else {
            nextNode = nodesAlongBorder[i + 1];
        }

        /*
         * Find path from node to next node
         */
        connectedPathNodes.push_back(node);
        if (node != nextNode) {
            std::vector<float> distances;
            std::vector<int32_t> parentNodes;
            geodesicHelper->getGeoFromNode(node, 
                                           distances, 
                                           parentNodes, 
                                           false);
            
            bool doneFlag = false;
            std::vector<int32_t> pathFromNextNodeToNode;
            int32_t pathNode = nextNode;
            while (doneFlag == false) {
                int32_t nextPathNode = parentNodes[pathNode];
                if (nextPathNode >= 0) {
                    if (nextPathNode == node) {
                        doneFlag = true;
                    }
                    else {
                        pathFromNextNodeToNode.push_back(pathNode);
                    }
                }
                else {
                    doneFlag = true;
                }
                
                if (doneFlag == false) {
                    pathNode = nextPathNode;
                }
            }
            
            const int32_t numberOfPathNodes = static_cast<int32_t>(pathFromNextNodeToNode.size());
            for (int32_t i = (numberOfPathNodes - 1); i >= 0; i--) {
                connectedPathNodes.push_back(pathFromNextNodeToNode[i]);
            }
        }
    }
    
    /*
     * Log the path.
     */
    const int32_t numberOfNodesInConnectedPath = static_cast<int32_t>(connectedPathNodes.size());
    AString text;
    text.reserve(10000);
    text += "Nodes in path:";
    for (int32_t i = 0; i < numberOfNodesInConnectedPath; i++) {
        text += (" " + connectedPathNodes[i]);
    }
    std::cout << qPrintable(text) << std::endl;
}

