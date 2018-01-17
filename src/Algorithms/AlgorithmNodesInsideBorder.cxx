
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

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <stack>
#include <utility>
#include <vector>

#include "AlgorithmNodesInsideBorder.h"

#include "AlgorithmException.h"
#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretOMP.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiFile.h"
#include "GeodesicHelper.h"
#include "LabelFile.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "TopologyHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmNodesInsideBorder::getCommandSwitch()
{
    return "-border-to-rois";//maybe the command should be in a separate file, and the nodes inside border code should be a helper class?
}

AString AlgorithmNodesInsideBorder::getShortDescription()
{
    return "MAKE METRIC ROIS FROM BORDERS";
}

OperationParameters* AlgorithmNodesInsideBorder::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface the borders are drawn on");
    
    ret->addBorderParameter(2, "border-file", "the border file");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric file");
    
    OptionalParameter* borderOpt = ret->createOptionalParameter(4, "-border", "create ROI for only one border");
    borderOpt->addStringParameter(1, "name", "the name of the border");
    
    ret->createOptionalParameter(5, "-inverse", "use inverse selection (outside border)");
    
    ret->setHelpText(
        AString("By default, draws ROIs inside all borders in the border file, as separate metric columns.")
    );
    return ret;
}

void AlgorithmNodesInsideBorder::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    BorderFile* myBorderFile = myParams->getBorder(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    OptionalParameter* borderOpt = myParams->getOptionalParameter(4);
    bool inverse = myParams->getOptionalParameter(5)->m_present;
    int numNodes = mySurf->getNumberOfNodes();
    if (mySurf->getStructure() == StructureEnum::INVALID) throw AlgorithmException("surface file needs a valid structure to find the correct borders in the file");
    if (myBorderFile->getNumberOfNodes() > 0)//-1 number of nodes is the signal that this is an old-format, potentially multistructure border file
    {
        if (myBorderFile->getStructure() != mySurf->getStructure()) throw AlgorithmException("border file and surface file have different structures");
        if (myBorderFile->getNumberOfNodes() != numNodes) throw AlgorithmException("border file and surface file have different number of vertices");
    }
    if (borderOpt->m_present)
    {
        AString findName = borderOpt->getString(1);
        int numBorders = myBorderFile->getNumberOfBorders();
        vector<float> scratchCol(numNodes, (inverse ? 1.0f : 0.0f));
        bool found = false;
        for (int i = 0; i < numBorders; ++i)
        {
            const Border* thisBorder = myBorderFile->getBorder(i);
            if (thisBorder->getName() == findName && thisBorder->getStructure() == mySurf->getStructure())
            {
                found = true;//don't use inverse in this call, because we need to merge results, and having them inverted first would just make things more confusing
                vector<int32_t> insideNodes = findNodesInsideBorder(mySurf, thisBorder, false);
                for (int index = 0; index < (int)insideNodes.size(); ++index)
                {
                    scratchCol[insideNodes[index]] = (inverse ? 0.0f : 1.0f);
                }
            }
        }
        if (!found) throw AlgorithmException("border name not found for surface structure");
        myMetricOut->setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
        myMetricOut->setStructure(mySurf->getStructure());
        myMetricOut->setColumnName(0, findName);
        myMetricOut->setValuesForColumn(0, scratchCol.data());
    } else {
        int numBorderParts = myBorderFile->getNumberOfBorders();
        map<pair<AString, AString>, int> borderCounter;//for grouping border parts with this structure into borders, with indexes into borderPartGroups
        vector<vector<int> > borderPartGroups;//stores the border parts of borders, in order of first part encountered in file
        for (int i = 0; i < numBorderParts; ++i)
        {
            const Border* thisBorder = myBorderFile->getBorder(i);
            if (thisBorder->getStructure() == mySurf->getStructure())
            {
                map<pair<AString, AString>, int>::iterator iter = borderCounter.find(make_pair(thisBorder->getName(), thisBorder->getClassName()));
                if (iter == borderCounter.end())
                {
                    borderCounter[make_pair(thisBorder->getName(), thisBorder->getClassName())] = (int)borderPartGroups.size();
                    borderPartGroups.push_back(vector<int>(1, i));
                } else {
                    borderPartGroups[iter->second].push_back(i);
                }
            }
        }
        int numBorders = (int)borderPartGroups.size();
        if (numBorders == 0) throw AlgorithmException("no borders match the structure of the surface");
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numBorders);
        myMetricOut->setStructure(mySurf->getStructure());
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < numBorders; ++i)//parallel mainly because the method used to be inefficient (repeatedly uses geodesic to full surface)
        {
            myMetricOut->setColumnName(i, myBorderFile->getBorder(borderPartGroups[i][0])->getName());//should be safe to do in parallel
            vector<float> scratchCol(numNodes, (inverse ? 1.0f : 0.0f));
            int numParts = (int)borderPartGroups[i].size();
            for (int j = 0; j < numParts; ++j)
            {
                const Border* thisBorder = myBorderFile->getBorder(borderPartGroups[i][j]);
                vector<int32_t> insideNodes = findNodesInsideBorder(mySurf, thisBorder, false);
                for (int index = 0; index < (int)insideNodes.size(); ++index)
                {
                    scratchCol[insideNodes[index]] = (inverse ? 0.0f : 1.0f);
                }
            }
            myMetricOut->setValuesForColumn(i, scratchCol.data());//ditto
        }
    }
}

/**
 * \class caret::AlgorithmNodesInsideBorder 
 * \brief Assign attribute values to nodes within a closed border.
 *
 * Identify nodes within a closed border and assign scalar or
 * label values to those nodes.
 *
 * Identifying nodes within a border can be troublesome for several reasons.
 * (1) Some borders are very narrow and opposite sides of a border may
 * be adjacent (like a figure eight) such that there are multiple closed 
 * regions.  (2) Borders may self intersect.  (3) One end of a border may 
 * overlay the other end of the border.  (4) Combinations of 1, 2, and/or 3.
 *
 * The original implementation of this algorithm assumed the border was 
 * oriented in a counter-clockwise orientation:  
 * (1) Identified a node to the left (inside) the border and filled
 * the region using the border as a boundary.
 * (2) If the number of selected nodes was greater than half then number
 * of nodes in the surface, it indicated that the border must have been
 * a clockwise orientation and the node selection was inverted.
 *
 * Corrections were made for a couple of the problems listed above but
 * when combinations of the problems were present the algorithm failed.
 * 
 * Since finding nodes inside a border was too challenging, it was realized
 * that finding nodes outside a border was much easier.  So, the algorithm
 * was re-written to identify nodes outside the border and then the selection
 * is inverted.
 * (1) Find the center of gravity (average coordinate) of the border.
 * (2) Find the surface node furthest from the border's center of gravity.
 * (3) Perform a connected fill operation but stopping anytime a border
 *     node is encountered.
 * (4) Invert the selection (verifying that the selected number of nodes
 *     is greater than half the number of nodes in the surface).
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
 * @param isInverseSelection
 *    Invert the selection.
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
                                                       const bool isInverseSelection,
                                                       const int32_t assignToMetricMapIndex,
                                                       const float assignMetricValue,
                                                       MetricFile* metricFileInOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    CaretAssert(metricFileInOut);
    
    if (surfaceFile->getNumberOfNodes() != metricFileInOut->getNumberOfNodes()) throw AlgorithmException("metric file must be initialized to same number of nodes");//a method that requires this really shouldn't be public
    
    std::vector<int32_t> nodesInsideBorder = findNodesInsideBorder(surfaceFile, border, isInverseSelection);
    
    const int32_t numberOfNodesInsideBorder = static_cast<int32_t>(nodesInsideBorder.size());
    for (int32_t i = 0; i < numberOfNodesInsideBorder; i++) {
        const int32_t nodeNumber = nodesInsideBorder[i];
        metricFileInOut->setValue(nodeNumber,
                                  assignToMetricMapIndex,
                                  assignMetricValue);
    }
}

/**
 * Constructor.
 *
 * @param myProgObj
 *
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param isInverseSelection
 *    Invert the selection.
 * @param assignToLabelMapIndex
 *    Map index in label file to which assigments are made for
 *    nodes inside the border.
 * @param assignLabelKey
 *    Label key assigned to nodes within the border.
 * @param labelFileInOut
 *    Label file that has map set with nodes inside border.
 */
AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                                       const SurfaceFile* surfaceFile,
                                                       const Border* border, 
                                                       const bool isInverseSelection,
                                                       const int32_t assignToLabelMapIndex,
                                                       const int32_t assignLabelKey,
                                                       LabelFile* labelFileInOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    CaretAssert(labelFileInOut);
    
    if (surfaceFile->getNumberOfNodes() != labelFileInOut->getNumberOfNodes()) throw AlgorithmException("metric file must be initialized to same number of nodes");//a method that requires this really shouldn't be public
    
    std::vector<int32_t> nodesInsideBorder = findNodesInsideBorder(surfaceFile, border, isInverseSelection);
    
    const int32_t numberOfNodesInsideBorder = static_cast<int32_t>(nodesInsideBorder.size());
    for (int32_t i = 0; i < numberOfNodesInsideBorder; i++) {
        const int32_t nodeNumber = nodesInsideBorder[i];
        labelFileInOut->setLabelKey(nodeNumber,
                                    assignToLabelMapIndex,
                                    assignLabelKey);
    }
}

/**
 * Constructor.
 *
 * @param myProgObj
 *
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param isInverseSelection
 *    Invert the selection.
 * @param assignToCiftiScalarMapIndex
 *    Map index in cifti scalar file to which assigments are made for
 *    nodes inside the border.
 * @param assignScalarValue
 *    Scalar value assigned to nodes within the border.
 * @param ciftiScalarFileInOut
 *    CIFTI scalar file that has map set with nodes inside border.
 */
AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                           const SurfaceFile* surfaceFile,
                           const Border* border,
                           const bool isInverseSelection,
                           const int32_t assignToCiftiScalarMapIndex,
                           const float assignScalarValue,
                           CiftiBrainordinateScalarFile* ciftiScalarFileInOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    CaretAssert(ciftiScalarFileInOut);
    
    std::vector<int32_t> nodesInsideBorder = findNodesInsideBorder(surfaceFile, border, isInverseSelection);
    
    std::vector<float> surfaceMapData(surfaceFile->getNumberOfNodes(),
                                      0.0);
    
    const int32_t numberOfNodesInsideBorder = static_cast<int32_t>(nodesInsideBorder.size());
    for (int32_t i = 0; i < numberOfNodesInsideBorder; i++) {
        const int32_t nodeIndex = nodesInsideBorder[i];
        CaretAssertVectorIndex(surfaceMapData, nodeIndex);
        surfaceMapData[nodeIndex] = assignScalarValue;
    }
    ciftiScalarFileInOut->setMapDataForSurface(assignToCiftiScalarMapIndex,
                                               surfaceFile->getStructure(),
                                               surfaceMapData);
}

/**
 * Constructor.
 *
 * @param myProgObj
 *
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param isInverseSelection
 *    Invert the selection.
 * @param assignToCiftiScalarMapIndex
 *    Map index in cifti scalar file to which assigments are made for
 *    nodes inside the border.
 * @param assignLabelKey
 *    Label key assigned to nodes within the border.
 * @param ciftiLabelFileInOut
 *    CIFTI label file that has map set with nodes inside border.
 */
AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                                       const SurfaceFile* surfaceFile,
                                                       const Border* border,
                                                       const bool isInverseSelection,
                                                       const int32_t assignToCiftiLabelMapIndex,
                                                       const int32_t assignLabelKey,
                                                       CiftiBrainordinateLabelFile* ciftiLabelFileInOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    CaretAssert(ciftiLabelFileInOut);
    
    std::vector<int32_t> nodesInsideBorder = this->findNodesInsideBorder(surfaceFile, border, isInverseSelection);
    
    std::vector<float> surfaceMapData(surfaceFile->getNumberOfNodes(),
                                      0.0);
    
    const int32_t numberOfNodesInsideBorder = static_cast<int32_t>(nodesInsideBorder.size());
    for (int32_t i = 0; i < numberOfNodesInsideBorder; i++) {
        const int32_t nodeIndex = nodesInsideBorder[i];
        CaretAssertVectorIndex(surfaceMapData, nodeIndex);
        surfaceMapData[nodeIndex] = assignLabelKey;
    }
    ciftiLabelFileInOut->setMapDataForSurface(assignToCiftiLabelMapIndex,
                                               surfaceFile->getStructure(),
                                               surfaceMapData);
    
}

/**
 * Constructor for getting a vector containing indices of nodes inside
 * the border.
 *
 * @param myProgObj
 *
 * @param surfaceFile
 *    Surface file for nodes inside border.
 * @param border
 *    Border for which nodes inside are found.
 * @param isInverseSelection
 *    Invert the selection.
 * @param assignToCiftiScalarMapIndex
 *    Map index in cifti scalar file to which assigments are made for
 *    nodes inside the border.
 * @param assignLabelKey
 *    Label key assigned to nodes within the border.
 * @param ciftiLabelFileInOut
 *    CIFTI label file that has map set with nodes inside border.
 */
AlgorithmNodesInsideBorder::AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                                       const SurfaceFile* surfaceFile,
                                                       const Border* border,
                                                       const bool isInverseSelection,
                                                       std::vector<int32_t>& nodesInsideBorderOut)
: AbstractAlgorithm(myProgObj)
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    
    nodesInsideBorderOut = this->findNodesInsideBorder(surfaceFile, border, isInverseSelection);
}

std::vector<int32_t>
AlgorithmNodesInsideBorder::findNodesInsideBorder(const SurfaceFile* mySurf, const Border* myBorder, const bool& inverse)
{
    std::vector<int32_t> nodesInsideBorderOut;
    if (myBorder->getNumberOfPoints() == 0) return nodesInsideBorderOut;//we could throw, but whatever
    
    /*
     * Move border points to the nearest nodes.
     */
    std::vector<int32_t> unconnectedNodesPath = moveBorderPointsToNearestNodes(mySurf, myBorder);

    /*
     * Convert the unconnected nodes path into a connected nodes path
     */
    std::vector<int32_t> connectedNodesPath = createConnectedNodesPath(mySurf, myBorder, unconnectedNodesPath);
    
    int32_t numberOfNodesInConnectedPath = static_cast<int32_t>(connectedNodesPath.size());
    if (numberOfNodesInConnectedPath < 1) {
        throw AlgorithmException("Connected path contains no vertices.");
    }
    
    /*
     * Find nodes that are OUTSIDE the connected path
     */
    std::vector<int32_t> nodesOutsidePath = findNodesOutsideOfConnectedPath(mySurf, connectedNodesPath);

    /*
     * Identify nodes INSIDE the connected path
     */
    const int32_t numberOfSurfaceNodes = mySurf->getNumberOfNodes();
    std::vector<bool> nodeInsideRegionFlags(numberOfSurfaceNodes,
                                            true);
    
    const int32_t numberOfNodesOutsidePath = static_cast<int32_t>(nodesOutsidePath.size());
    for (int32_t i = 0; i < numberOfNodesOutsidePath; i++) {
        const int32_t nodeIndex = nodesOutsidePath[i];
        CaretAssertVectorIndex(nodeInsideRegionFlags, nodeIndex);
        nodeInsideRegionFlags[nodeIndex] = false;
    }
    
    /*
     * Handle inverse selection or possibility outside selection was
     * accidentally inside selection
     */
    bool doInverseFlag = inverse;
    
    const int32_t halfNumberOfSurfaceNodes = numberOfSurfaceNodes / 2;
    if (numberOfNodesOutsidePath < halfNumberOfSurfaceNodes) {
        doInverseFlag = ( ! doInverseFlag);
    }
    
    const CaretPointer<TopologyHelper> th = mySurf->getTopologyHelper();
    /*
     * Identify nodes
     */
    for (int32_t i = 0; i < numberOfSurfaceNodes; i++) {
        bool insideFlag = nodeInsideRegionFlags[i];
        if (doInverseFlag) {
            insideFlag = ( ! insideFlag);
        }

        if (th->getNodeHasNeighbors(i)) {
            if (std::find(connectedNodesPath.begin(),
                          connectedNodesPath.end(),
                          i) != connectedNodesPath.end()) {
                insideFlag = false;
            }
        }
        else {
            insideFlag = false;
        }

        if (insideFlag) {
            nodesInsideBorderOut.push_back(i);
        }
    }
    return nodesInsideBorderOut;
}

std::vector<int32_t>
AlgorithmNodesInsideBorder::moveBorderPointsToNearestNodes(const SurfaceFile* mySurf, const Border* myBorder)
{
    std::vector<int32_t> nodeIndicesFollowingBorder;
    
    /*
     * Move border points to the nearest nodes.
     */
    const int32_t originalNumberOfBorderPoints = myBorder->getNumberOfPoints();
    for (int32_t i = 0; i < originalNumberOfBorderPoints; i++) {
        const SurfaceProjectedItem* spi = myBorder->getPoint(i);
        float xyz[3];
        if (spi->getProjectedPosition(*mySurf, xyz, true)) {
            const int32_t nearestNode = mySurf->closestNode(xyz);
            if (nearestNode >= 0) {
                nodeIndicesFollowingBorder.push_back(nearestNode);
            }
        }
    }
    
    CaretAssert(myBorder->getNumberOfPoints() == (int)nodeIndicesFollowingBorder.size());//required for the connecting code to use following line segment logic
    return nodeIndicesFollowingBorder;
}


std::vector<int32_t>
AlgorithmNodesInsideBorder::createConnectedNodesPath(const SurfaceFile* mySurf, const Border* myBorder, const std::vector<int32_t>& unconnectedNodesPath)
{
    std::vector<int32_t> connectedNodesPathOut;
    
    /*
     * Geodesic helper for surface
     */
    CaretPointer<GeodesicHelper> geodesicHelper = mySurf->getGeodesicHelper();
    
    /*
     * Get the topology helper for the surface
     */
    CaretPointer<TopologyHelper> th = mySurf->getTopologyHelper();
    
    /*
     * Find connected path along node neighbors
     */
    const int32_t numberOfNodesInUnconnectedPath = static_cast<int32_t>(unconnectedNodesPath.size());
    for (int32_t i = 0; i < numberOfNodesInUnconnectedPath; i++) {
        const int node = unconnectedNodesPath[i];
        connectedNodesPathOut.push_back(node);
        
        int nextIndex = -1;
        const bool lastNodeFlag = (i >= (numberOfNodesInUnconnectedPath - 1));
        if (lastNodeFlag) {
            nextIndex = 0;
        }
        else {
            nextIndex = i + 1;
        }
        int nextNode = unconnectedNodesPath[nextIndex];
        
        
        /*if (node != nextNode) {
            bool doGeodesicSearch = true;
            
            const std::vector<int32_t> neighbors = th->getNodeNeighbors(node);
            if (std::find(neighbors.begin(),
                          neighbors.end(),
                          nextNode) != neighbors.end()) {
                connectedNodesPathOut.push_back(nextNode);
                doGeodesicSearch = false;
            }
            
            if (doGeodesicSearch) {
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
                    connectedNodesPathOut.push_back(pathFromNextNodeToNode[i]);
                }
            }
        }//*/
        vector<int32_t> path;
        vector<float> dists;
        Vector3D nodePos, nextPos;
        myBorder->getPoint(i)->getProjectedPosition(*mySurf, nodePos, true);//really means unproject
        myBorder->getPoint(nextIndex)->getProjectedPosition(*mySurf, nextPos, true);
        geodesicHelper->getPathAlongLineSegment(node, nextNode, nodePos, nextPos, path, dists);
        connectedNodesPathOut.insert(connectedNodesPathOut.end(), path.begin(), path.end());
    }
    
    /*
     * Remove duplicates.
     */
    cleanNodePath(connectedNodesPathOut);
    
    /*
     * Valid that the path nodes are connected.
     */
    validateConnectedNodesPath(mySurf, connectedNodesPathOut);
    
    return connectedNodesPathOut;
}

/**
 * Find the nodes OUTSIDE the given connected path.
 *
 * @param connectedNodesPath
 *    Connected path for which nodes inside are found.
 * @param nodesOutsidePathOut
 *    Vector into which nodes OUTSIDE connected path are loaded. 
 */
std::vector<int32_t>
AlgorithmNodesInsideBorder::findNodesOutsideOfConnectedPath(const SurfaceFile* mySurf, const std::vector<int32_t>& connectedNodesPath)
{
    std::vector<int32_t> nodesOutsidePathOut;

    /*
     * Track nodes that are found inside and/or have been visited.
     */
    const int32_t numberOfNodes = mySurf->getNumberOfNodes();
    std::vector<int> nodeSearchStatus(numberOfNodes, 0);
    std::vector<bool> insideBorderFlag(numberOfNodes, false);
    
    /*
     * Mark all nodes in connected path as visited.
     */
    const int32_t numberOfNodesInConnectedPath = static_cast<int32_t>(connectedNodesPath.size());
    for (int32_t i = 0; i < numberOfNodesInConnectedPath; i++) {
        CaretAssertVectorIndex(nodeSearchStatus, connectedNodesPath[i]);
        nodeSearchStatus[connectedNodesPath[i]] = 1;
    }
    
    /*
     * Get the topology helper for the surface with neighbors sorted.
     */
    CaretPointer<TopologyHelper> th = mySurf->getTopologyHelper(true);
    
    /*
     * Find the node that is furthest from the connected path's
     * center of gravity (average coordinate)
     */
    int32_t startNode = findNodeFurthestFromConnectedPathCenterOfGravity(mySurf, connectedNodesPath, nodeSearchStatus);
    
    if (startNode < 0) {
        throw AlgorithmException("Failed to find vertex that is not inside of the connected path.");
    }
    
    /*
     * Mark the starting node as 'inside'.
     */
    CaretAssertVectorIndex(insideBorderFlag, startNode);
    insideBorderFlag[startNode] = true;
    
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
        CaretAssertVectorIndex(nodeSearchStatus, nodeNumber);
        if (nodeSearchStatus[nodeNumber] == 0) {
            nodeSearchStatus[nodeNumber] = 1;
            
            /*
             * Set node as inside
             */
            CaretAssertVectorIndex(insideBorderFlag, nodeNumber);
            insideBorderFlag[nodeNumber] = true;
            
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
                CaretAssertVectorIndex(nodeSearchStatus, neighborNode);
                if (nodeSearchStatus[neighborNode] != 1) {
                    stack.push(neighborNode);
                }
            }
        }
    }
    
    /*
     * Return nodes inside the path
     */
    int32_t insideCount = 0;
    for (int32_t i = 0; i < numberOfNodes; i++) {
        CaretAssertVectorIndex(insideBorderFlag, i);
        if (insideBorderFlag[i]) {
            nodesOutsidePathOut.push_back(i);
            insideCount++;
        }
    }
    return nodesOutsidePathOut;
}

/**
 * Find an unvisited node that is FURTHEST from the center-of-gravity of
 * the the nodes in the connected path.
 *
 * @param connectedNodesPath
 *     Path of connected nodes.
 * @param nodeSearchStatus
 *     Search status of the nodes.
 */
int32_t
AlgorithmNodesInsideBorder::findNodeFurthestFromConnectedPathCenterOfGravity(const SurfaceFile* mySurf, const std::vector<int32_t>& connectedNodesPath,
                                                                             std::vector<int>& nodeSearchStatus)
{
    double sumX = 0.0;
    double sumY = 0.0;
    double sumZ = 0.0;
    double sumCount = 0.0;
    
    const int32_t numberOfNodesInConnectedPath = static_cast<int32_t>(connectedNodesPath.size());
    
    for (int32_t i = 1; i < (numberOfNodesInConnectedPath - 1); i++) {
        const float* xyz = mySurf->getCoordinate(connectedNodesPath[i]);
        sumX += xyz[0];
        sumY += xyz[1];
        sumZ += xyz[2];
        sumCount++;
    }
    
    if (sumCount >= 1.0) {
        const float cog[3] = {
            (float)(sumX / sumCount),
            (float)(sumY / sumCount),
            (float)(sumZ / sumCount)
        };
        
        /*
         * Get the topology helper for the surface with neighbors sorted.
         */
        CaretPointer<TopologyHelper> topologyHelper = mySurf->getTopologyHelper(true);
        float maxDist = -1.0;
        int32_t maxDistNodeIndex = -1;
        
        const int32_t numberOfNodes = mySurf->getNumberOfNodes();
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (nodeSearchStatus[i] == 0) {
                if (topologyHelper->getNodeHasNeighbors(i)) {
                    const float* coordXYZ = mySurf->getCoordinate(i);
                    const float distSQ = MathFunctions::distanceSquared3D(cog,
                                                                          coordXYZ);
                    if (distSQ > maxDist) {
                        maxDist = distSQ;
                        maxDistNodeIndex = i;
                    }
                }
            }
        }
        
        return maxDistNodeIndex;
    }
    
    return -1;
}

/**
 * Clean the path by removing any consecutive nodes that are identical.
 *
 * @param nodePath
 *    Path that is cleaned.
 */
void 
AlgorithmNodesInsideBorder::cleanNodePath(std::vector<int32_t>& nodePath)
{
    std::vector<int32_t> path = nodePath;
    nodePath.clear();
    
    /*
     * Unique copy will remove consecutive identical elements
     */
    std::unique_copy(path.begin(),
                     path.end(),
                     back_inserter(nodePath));
}

/**
 * Verify that the connect nodes path is fully connected.
 *
 * @param connectedNodesPath
 *    Path that is validated.
 */
void 
AlgorithmNodesInsideBorder::validateConnectedNodesPath(const SurfaceFile* mySurf, const std::vector<int32_t>& connectedNodesPath)
{
    /*
     * Get the topology helper for the surface with neighbors sorted.
     */
    CaretPointer<TopologyHelper> th = mySurf->getTopologyHelper(false);
    
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
                throw AlgorithmException("Validation of vertex path along border failed.  Vertex "
                                         + AString::number(node)
                                         + " should be connected to "
                                         + AString::number(nextNode)
                                         + " but it is not.");
            }
        }
    }
}


