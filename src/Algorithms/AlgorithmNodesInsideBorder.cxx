
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

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stack>

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
    CaretAssert(surfaceFile->getNumberOfNodes() == metricFileInOut->getNumberOfNodes());
    
    std::vector<int32_t> nodesInsideBorder;
    this->findNodesInsideBorder(surfaceFile,
                                border,
                                nodesInsideBorder);
    
    const int32_t numberOfNodesInsideBorder = static_cast<int32_t>(nodesInsideBorder.size());
    for (int32_t i = 0; i < numberOfNodesInsideBorder; i++) {
        const int32_t nodeNumber = nodesInsideBorder[i];
        metricFileInOut->setValue(nodeNumber,
                                  assignToMetricMapIndex,
                                  assignMetricValue);
    }
}

AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj, 
                                                       const SurfaceFile* surfaceFile,
                                                       const Border* border, 
                                                       const int32_t assignToLabelMapIndex,
                                                       const int32_t assignLabelKey,
                                                       LabelFile* labelFileInOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    CaretAssert(labelFileInOut);
    CaretAssert(surfaceFile->getNumberOfNodes() == labelFileInOut->getNumberOfNodes());
    
    std::vector<int32_t> nodesInsideBorder;
    this->findNodesInsideBorder(surfaceFile,
                                border,
                                nodesInsideBorder);
    
    const int32_t numberOfNodesInsideBorder = static_cast<int32_t>(nodesInsideBorder.size());
    for (int32_t i = 0; i < numberOfNodesInsideBorder; i++) {
        const int32_t nodeNumber = nodesInsideBorder[i];
        labelFileInOut->setLabelKey(nodeNumber,
                                    assignToLabelMapIndex,
                                    assignLabelKey);
    }
}

/**
 * Find nodes inside the border.
 *
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param nodesInsideBorderOut
 *    Vector into which nodes inside border are loaded.
 */
void 
AlgorithmNodesInsideBorder::findNodesInsideBorder(const SurfaceFile* surfaceFile,
                                                  const Border* border,
                                                  std::vector<int32_t>& nodesInsideBorderOut)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    
    nodesInsideBorderOut.clear();
    
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
                        pathFromNextNodeToNode.push_back(nextPathNode);
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
     * Remove duplicates.
     */
    this->cleanConnectedNodesPath(connectedPathNodes);
    
    /*
     * Valid that the path nodes are connected.
     */
    this->validateConnectedNodesPath(surfaceFile, 
                                     connectedPathNodes);
    
//    if (connectedPathNodes.size() > 0) {
//        nodesInsideBorderOut.insert(nodesInsideBorderOut.end(),
//                                    connectedPathNodes.begin(),
//                                    connectedPathNodes.end());
//        return;
//    }
    
    /*
     * Log the path.
     */
    int32_t numberOfNodesInConnectedPath = static_cast<int32_t>(connectedPathNodes.size());
    if (numberOfNodesInConnectedPath >= 4) {
        AString text;
        text.reserve(10000);
        text += ("Nodes in path (count="
                 + AString::number(numberOfNodesInConnectedPath)
                 + "):");
        for (int32_t i = 0; i < numberOfNodesInConnectedPath; i++) {
            text += (" " + AString::number(connectedPathNodes[i]));
        }
        std::cout << qPrintable(text) << std::endl;
        
        /*
         * Determine the nodes inside the connected path
         */
        this->findNodesInConnectedNodesPath(surfaceFile,
                                            connectedPathNodes,
                                            nodesInsideBorderOut);
        
        const int32_t numberOfNodesInside = static_cast<int32_t>(nodesInsideBorderOut.size());
        text = "";
        text.reserve(20000);
        text = ("Nodes INSIDE border (count="
                + AString::number(numberOfNodesInside)
                + "):");
        for (int32_t i = 0; i < numberOfNodesInside; i++) {
            text += (" " + AString::number(nodesInsideBorderOut[i]));
        }
        std::cout << qPrintable(text) << std::endl;
    }
    else {
        throw AlgorithmException("Connected path along border is too small "
                                 "as it consists of four or fewer nodes.");
    }
}

/**
 * Find the nodes inside the connected path on the surface.
 *
 * @param surfaceFile
 *    Surface file for nodes inside connected path.
 * @param connectedNodesPath
 *    Connected path for which nodes inside are found.
 * @param nodesInsidePathOut
 *    Vector into which nodes inside connected path are loaded.
 */
void 
AlgorithmNodesInsideBorder::findNodesInConnectedNodesPath(const SurfaceFile* surfaceFile,
                                                          const std::vector<int32_t>& connectedNodesPath,
                                                          std::vector<int32_t>& nodesInsidePathOut)
{
    /*
     * Get the topology helper for the surface with neighbors sorted.
     */
    CaretPointer<TopologyHelper> th = surfaceFile->getTopologyHelper(true);
    
    /*
     * Using three nodes, find a node that is 'on the left'
     * assuming the path is oriented counter-clockwise.
     */
    int32_t startNode = -1;
    const int32_t numberOfNodesInConnectedPath = static_cast<int32_t>(connectedNodesPath.size());
    for (int32_t i = 1; i < (numberOfNodesInConnectedPath - 1); i++) {
        const int prevPathNode = connectedNodesPath[i - 1];
        const int pathNode     = connectedNodesPath[i];
        const int nextPathNode = connectedNodesPath[i + 1];
        
        int numNeighbors;
        const int* neighbors = th->getNodeNeighbors(pathNode, numNeighbors);
        if (numNeighbors > 2) {
            int32_t prevIndex = -1;
            int32_t nextIndex = -1;
            for (int32_t j = 0; j < numNeighbors; j++) {
                if (neighbors[j] == prevPathNode) {
                    prevIndex = j;
                }
                else if (neighbors[j] == nextPathNode) {
                    nextIndex = j;
                }
            }
            
            if ((nextIndex >= 0) && (prevIndex >= 0)) {
                if (nextIndex >= (numNeighbors - 1)) {
                    if (prevIndex > 0) {
                        startNode = neighbors[0];
                    }
                }
                else {
                    if (prevIndex != (nextIndex + 1)) {
                        startNode = neighbors[nextIndex + 1];
                    }
                }
            }
            
            if (startNode >= 0) {
                if (std::find(connectedNodesPath.begin(),
                              connectedNodesPath.end(),
                              startNode) != connectedNodesPath.end()) {
                    startNode = -1;
                }
                
                if (startNode >= 0) {
                    break;
                }
            }
        }
    }
    
    if (startNode < 0) {
        throw AlgorithmException("Failed to find node that is not inside of the connected path.");
    }
    
    /*
     * Track nodes that are found inside and/or have been visited.
     */
    const int32_t numberOfNodes = surfaceFile->getNumberOfNodes();
    std::vector<bool> visited(numberOfNodes, false);
    std::vector<bool> inside(numberOfNodes, false);
    
    /*
     * Mark all nodes in connected path as visited.
     */
    for (int32_t i = 0; i < numberOfNodesInConnectedPath; i++) {
        visited[connectedNodesPath[i]] = true;
    }
    
    /*
     * Mark the starting node as 'inside'.
     */
    inside[startNode] = true;
    
    /*
     * Use a stack to help with search.
     */
    std::stack<int32_t> stack;
    stack.push(startNode);
    
    /*
     * Search until no more to search.
     */
    while (stack.empty() == false) {
        const int32_t nodeNumber = stack.top();
        stack.pop();
        
        /*
         * Has node been visited?
         */
        if (visited[nodeNumber] == false){
            visited[nodeNumber] = true;
            
            /*
             * Set node as inside
             */
            inside[nodeNumber] = true;
            
            /*
             * Get neighbors of this node
             */
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(nodeNumber, numNeighbors);
            
            /*
             * add neighbors to search
             */
            for (int i = 0; i < numNeighbors; i++) {
                const int neighborNode = neighbors[i];
                if (visited[neighborNode] == false) {
                    stack.push(neighborNode);
                }
            }
        }
    }
    
    /*
     * Return nodes inside the path
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        if (inside[i]) {
            nodesInsidePathOut.push_back(i);
        }
    }
}

/**
 * Clean the path by removing any consecutive nodes that are identical.
 * @param connectedNodesPath
 *    Path that is cleaned.
 */
void 
AlgorithmNodesInsideBorder::cleanConnectedNodesPath(std::vector<int32_t>& connectedNodesPath)
{
    std::vector<int32_t> path = connectedNodesPath;
    connectedNodesPath.clear();
    
    /*
     * Unique copy will remove consecutive identical elements
     */
    std::unique_copy(path.begin(),
                     path.end(),
                     back_inserter(connectedNodesPath));
    
//    /*
//     * Remove last node if it is the same as the first node
//     */
//    const int32_t numNodes = static_cast<int32_t>(connectedNodesPath.size());
//    if (numNodes > 1) {
//        if (connectedNodesPath[0] == connectedNodesPath[numNodes - 1]) {
//            connectedNodesPath.resize(numNodes - 1);
//        }
//    }
}

/**
 * Verify that the connect nodes path is fully connected.
 *
 * @param surfaceFile
 *    Surface file for nodes inside connected path.
 * @param connectedNodesPath
 *    Path that is validated.
 */
void 
AlgorithmNodesInsideBorder::validateConnectedNodesPath(const SurfaceFile* surfaceFile,
                                                       const std::vector<int32_t>& connectedNodesPath)
{
    /*
     * Get the topology helper for the surface with neighbors sorted.
     */
    CaretPointer<TopologyHelper> th = surfaceFile->getTopologyHelper(false);
    
    /*
     * Check the path to see that each pair of nodes are connected.
     */
    const int32_t numberOfNodesInPath = static_cast<int32_t>(connectedNodesPath.size());
    for (int32_t i = 0; i < numberOfNodesInPath; i++) {
        int numNeighbors;
        const int node = connectedNodesPath[i];
        const int* neighbors = th->getNodeNeighbors(node, numNeighbors);
        
        int32_t nextNode = -1;
        if (i >= (numberOfNodesInPath - 1)) {
            nextNode = connectedNodesPath[0];
        }
        else {
            nextNode = connectedNodesPath[i + 1];
        }
        
        if (node != nextNode) {
            bool foundIt = false;
            for (int j = 0; j < numNeighbors; j++) {
                if (neighbors[j] == nextNode) {
                    foundIt = true;
                    break;
                }
            }
            
            if (foundIt == false) {
                throw AlgorithmException("Validation of node path along border failed.  Node "
                                         + AString::number(node)
                                         + " should be connected to "
                                         + AString::number(nextNode)
                                         + " but it is not.");
            }
        }
    }
}


