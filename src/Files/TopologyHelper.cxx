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
/*LICENSE_END*/



#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "CaretAssert.h"
#include <cmath>

using namespace caret;

/**
 * Comparison operator for EdgeInfo
 */
bool 
caret::operator<(const TopologyEdgeInfo& e1, const TopologyEdgeInfo& e2)
{
   if (e1.node1 == e2.node1) {
      return (e1.node2 < e2.node2);
   }
   return (e1.node1 < e2.node1);
}

/**
 * Constructor.  This will contruct the topological information for the TopologyFile.
 *   If "sortNodeInfo" is set the tiles and node neighbors will be sorted
 *   around each node.
 */
TopologyHelper::TopologyHelper(const SurfaceFile* sf,
                               const bool sortNodeInfo)
{
    nodeSortedInfoBuilt = false;

    const int numTiles = sf->getNumberOfTriangles();
    if (numTiles <= 0) {
        return;
    }

    int maxNodeNum = -1;

    //
    // Get the number of nodes
    //
    for (int j = 0; j < numTiles; j++) {
        const int32_t* tribase = sf->getTriangle(j);
        if (tribase[0] > maxNodeNum) maxNodeNum = tribase[0];
        if (tribase[1] > maxNodeNum) maxNodeNum = tribase[1];
        if (tribase[2] > maxNodeNum) maxNodeNum = tribase[2];
    }

    //
    // Node indices start at zero so need to increment
    //
    maxNodeNum++;

    //
    // There may be nodes that have been disconnected and not associated with any tiles
    //
    maxNodeNum = std::max(maxNodeNum, sf->getNumberOfNodes());

    //
    // Initialize the node structures
    //
    nodes.reserve(maxNodeNum);
    for (int i = 0; i < maxNodeNum; i++) {
        nodes.push_back(NodeInfo(i));
    }

    //
    // Keep track of the tiles
    //
    for (int j = 0; j < numTiles; j++) {
        const int32_t* tribase = sf->getTriangle(j);
        if (sortNodeInfo) {
            nodes[tribase[0]].addNeighbors(j, tribase[1], tribase[2]);
            nodes[tribase[1]].addNeighbors(j, tribase[2], tribase[0]);
            nodes[tribase[2]].addNeighbors(j, tribase[0], tribase[1]);
        }
        else {
            nodes[tribase[0]].addNeighbor(tribase[1]);
            nodes[tribase[0]].addNeighbor(tribase[2]);
            nodes[tribase[1]].addNeighbor(tribase[0]);
            nodes[tribase[1]].addNeighbor(tribase[2]);
            nodes[tribase[2]].addNeighbor(tribase[0]);
            nodes[tribase[2]].addNeighbor(tribase[1]);
            nodes[tribase[0]].addTile(j);
            nodes[tribase[1]].addTile(j);
            nodes[tribase[2]].addTile(j);
        }

        addEdgeInfo(j, tribase[0], tribase[1]);
        addEdgeInfo(j, tribase[1], tribase[2]);
        addEdgeInfo(j, tribase[2], tribase[0]);
    }

    if (sortNodeInfo) {
        nodeSortedInfoBuilt = true;
        for (unsigned int k = 0; k < nodes.size(); k++) {
            nodes[k].sortNeighbors();
        }
    }
}

/**
 * Destructor.
 */
TopologyHelper::~TopologyHelper()
{
   nodes.clear();
   topologyEdges.clear();
}

/**
 * Keep track of edges
 */
void
TopologyHelper::addEdgeInfo(const int tileNum, const int node1, const int node2)
{
   TopologyEdgeInfo edge(tileNum, node1, node2);
   
   std::set<TopologyEdgeInfo>::iterator iter = topologyEdges.find(edge);
   if (iter == topologyEdges.end()) {
      topologyEdges.insert(edge);
   }
   else {
      TopologyEdgeInfo& e = (TopologyEdgeInfo&)(*iter);
      e.addTile(tileNum);
      //iter->addTile(tileNum);
   }
}

/**
 * Sort a nodes neighbors
 */
void 
TopologyHelper::NodeInfo::sortNeighbors() 
{
   if ((edges.size() > 0) && sortMe) {
      //
      // Nodes that are on the edge (boundary) of a cut surface must be treated specially when sorting.
      // In this case the sorting needs to start with the link whose first node is not used
      // in any other link.  This is true if all tiles are consistently oriented (and they
      // must be).
      //
      int startEdge = -1;
      for (unsigned int k = 0; k < edges.size(); k++) {
         const int nodeNum = edges[k].node1;
         bool foundNode = false;
         
         //
         // See if nodeNum is used in any other edges
         //
         for (unsigned int m = 0; m < edges.size(); m++) {
            if (m != k) {
               if (edges[m].containsNode(nodeNum) == true) {
                  foundNode = true;
                  break;
               }
            }
         }
         
         if (foundNode == false) {
            if (startEdge >= 0) {
               //std::cerr << "INFO: multiple starting edges for node " << nodeNumber << std::endl;
            }
            else {
               startEdge = k;
            }
         }
      }
      
      bool flag = false;
      if ((nodeNumber >= 44508) && (nodeNumber <= 44507)) { // modify for debugging
         std::cerr << "Start Edge for node " << nodeNumber << " is " << startEdge << std::endl;
         for (unsigned int k = 0; k < edges.size(); k++) {
            std::cerr << "   " << edges[k].node1 << " " << edges[k].node2 << std::endl;
         }
         flag = true;
      }

      // startEdge is less than zero if the node's links are all interior links 
      if (startEdge < 0) {
         startEdge = 0;
      }
      
      int currentNode  = edges[startEdge].node1;
      int nextNeighbor = edges[startEdge].node2;
      neighbors.push_back(currentNode);
      tiles.push_back(edges[startEdge].tileNumber);
      const int firstNode = currentNode;
      
      for (unsigned int i = 1; i < edges.size(); i++) {
         neighbors.push_back(nextNeighbor);
         // find edge with node "nextNeighbor" but without node "currentNode"
         const NodeEdgeInfo* e = findEdgeWithPoint(nextNeighbor, currentNode);
         if (e != NULL) {
            tiles.push_back(e->tileNumber);
            currentNode = nextNeighbor;
            nextNeighbor = e->getOtherNode(nextNeighbor);
            if (nextNeighbor < 0) {
               //std::cerr << "INFO: Unable to find neighbor of"
               //            <<  nextNeighbor << std::endl;
            }
         }
         else {
            nextNeighbor = -1;
            //std::cerr << "INFO: Unable to find edge for for node " 
            //            << nodeNumber << std::endl;
            break;
         }
      }
      
      if ((nextNeighbor != firstNode) && (nextNeighbor >= 0)) {
         neighbors.push_back(nextNeighbor);
      }
      
      if (flag) {
         std::cerr << "Node " << nodeNumber << " neighbors: ";
         for (unsigned int n = 0; n < neighbors.size(); n++) {
            std::cerr << "  " << neighbors[n];
         }
         std::cerr << std::endl;
         std::cerr << "Node " << nodeNumber << " tiles: ";
         for (unsigned int n = 0; n < tiles.size(); n++) {
            std::cerr << "  " << tiles[n];
         }
         std::cerr << std::endl;
      }
   }
   edges.clear();
}

/**
 * See if a node has any neighbors
 */
bool
TopologyHelper::getNodeHasNeighbors(const int nodeNum) const
{
    CaretAssertVectorIndex(nodes, nodeNum);
    return (nodes[nodeNum].neighbors.empty() == false);
}

/**
 * Get the number of neighbors for a node.
 */
int 
TopologyHelper::getNodeNumberOfNeighbors(const int nodeNum) const
{
    CaretAssertVectorIndex(nodes, nodeNum);
    return nodes[nodeNum].neighbors.size();
}

/**
 * Get the number of boundary edges used by node.
 */
void 
TopologyHelper::getNumberOfBoundaryEdgesForAllNodes(std::vector<int>& numBoundaryEdgesPerNode) const
{
   //
   // should already be allocated
   //
   const int numNodes = getNumberOfNodes();
   if (static_cast<int>(numBoundaryEdgesPerNode.size()) < numNodes) {
      numBoundaryEdgesPerNode.resize(numNodes);
   }
   std::fill(numBoundaryEdgesPerNode.begin(), numBoundaryEdgesPerNode.end(), 0);
   
   //
   // Loop through all edges
   //
   for (std::set<TopologyEdgeInfo>::const_iterator iter = topologyEdges.begin(); 
        iter != topologyEdges.end(); iter++) {
      //
      // Get the tiles
      //
      int tile1, tile2;
      iter->getTiles(tile1, tile2);
      
      //
      // If edges has only one tile
      //
      if ((tile1 >= 0) && (tile2 < 0)) {
         //
         // Increment the count for the node
         //
         int node1, node2;
         iter->getNodes(node1, node2);
         numBoundaryEdgesPerNode[node1]++;
         numBoundaryEdgesPerNode[node2]++;
      }
   }
}
      
/**
 * Get the maximum number of neighbors of all nodes.
 */
int
TopologyHelper::getMaximumNumberOfNeighbors() const
{
   unsigned int maxNeighbors = 0;
   int numNodes = getNumberOfNodes();
   for (int i = 0; i < numNodes; i++) {
      const unsigned int num = nodes[i].neighbors.size();
      if (num > maxNeighbors) {
         maxNeighbors = num;
      }
   }
   return maxNeighbors;
}

/**
 * Get the neighboring nodes for a node
 */
void
TopologyHelper::getNodeNeighbors(const int nodeNum,
                                 std::vector<int>& neighborsOut) const
{
    CaretAssertVectorIndex(nodes, nodeNum);
    neighborsOut = nodes[nodeNum].neighbors;
}

/**
 * Get the neighboring nodes for a node, within a given ROI
 */
void
TopologyHelper::getNodeNeighborsInROI(const int nodeNum,
                                 std::vector<int>& neighborsOut, const float *roiValues) const
{
   if ((nodeNum >= 0) && (nodeNum < static_cast<int>(nodes.size()))) {
      //Since we are restricting to an roi, we need to make sure that the neighbors are within the ROI, and
      //if not, build a new neighbor list
      std::vector<int> neighbors = nodes[nodeNum].neighbors;
      //optimization, first, loop through and see any neighbors are outside the roi
      bool neighborOutsideOfRoi = false;
      for(int i = 0;i<(int)neighbors.size();i++)
      {
         if(roiValues[neighbors[i]] == 0.0)
         {
            neighborOutsideOfRoi = true;
            break;
         }
      }

      if(!neighborOutsideOfRoi)
      {
         neighborsOut = nodes[nodeNum].neighbors;
      }
      else
      {
         neighborsOut.clear();

         for(int i = 0;i<(int)neighbors.size();i++)
         {
            if(roiValues[neighbors[i]] == 0.0)//filter out neighbors that are outside of ROI
            {
               continue;
            }
            else
            {
               neighborsOut.push_back(neighbors[i]);
            }
         }
      }
   }
   else {
      neighborsOut.clear();
   }
}

/**
 * Get the neighbors of a node to a specified depth.
 */
void 
TopologyHelper::getNodeNeighborsToDepthIter(const int rootNode, 
                                        const int depth,
                                        std::vector<int>& neighborsOut) const
{
   if (depth < 2)//could make this function private, and remove this test
   {//gracefully handle nonpositives, and do 1 the easy way
      getNodeNeighbors(rootNode, neighborsOut);
      return;
   }
   CaretMutexLocker locked(&usingMarkNodes);//lock after choosing which version to use, because they are both public
   int i, j, k;
   neighborsOut.clear();
   int numNodes = getNumberOfNodes();
   int expected = (7 * (depth + 1) * depth) >> 1;//uses bitshift for cheap divide, equals 7 * depth * (depth + 1) / 2
   if (expected > numNodes) expected = numNodes;
   neighborsOut.reserve(expected);//reserve means make space, but don't change size()
   if ((int)markNodes.size() != numNodes)
   {
      markNodes.resize(numNodes);
      for (i = 0; i < numNodes; ++i)
      {
         markNodes[i] = 0;
      }
   }
   if ((int)nodelist[0].size() != numNodes)
   {//WAY overkill in basically all cases, but no way to know max needed for sure, and branching or clear() might be slower
      nodelist[0].resize(numNodes);
      nodelist[1].resize(numNodes);
   }
   int newind = 1, oldind = 0, whichneigh;
   int oldused, newused = 1, numNeigh;
   std::vector<int>* oldlist, *newlist;
   markNodes[rootNode] = 1;
   nodelist[0][0] = rootNode;
   for (i = 0; i < depth; ++i)
   {
      oldused = newused;
      newused = 0;
      oldlist = nodelist + oldind;//pointer math should be fast
      newlist = nodelist + newind;
      for (j = 0; j < oldused; ++j)
      {
         const std::vector<int>& neighbors = nodes[(*oldlist)[j]].neighbors;
         numNeigh = neighbors.size();
         for (k = 0; k < numNeigh; ++k)
         {
            whichneigh = neighbors[k];
            if (!markNodes[whichneigh])
            {
               markNodes[whichneigh] = 1;
               neighborsOut.push_back(whichneigh);
               (*newlist)[newused++] = whichneigh;
            }
         }
      }
      oldind = newind;//swap the lists
      newind = 1 ^ oldind;//bitwise xor might be faster than minus
   }//done, now clean up markNodes
   numNeigh = neighborsOut.size();
   for (i = 0; i < numNeigh; ++i)
      markNodes[neighborsOut[i]] = 0;
   markNodes[rootNode] = 0;//only zero the nodes that were marked: after completion of call, array will be entirely zeroed
}

/**
 * Get the neighbors of a node to a specified depth.
 */
void 
TopologyHelper::getNodeNeighborsToDepth(const int rootNode, 
                                        const int depth,
                                        std::vector<int>& neighborsOut) const
{
   if (depth < 2)
   {//gracefully handle nonpositives, and do 1 the easy way
      getNodeNeighbors(rootNode, neighborsOut);
      return;
   }
   if (depth > 4)
   {//iterative version is faster for large depths, cutoff found on 64bit machine, but running 32bit executable
      getNodeNeighborsToDepthIter(rootNode, depth, neighborsOut);
      return;
   }
   CaretMutexLocker locked(&usingMarkNodes);//lock after choosing which version to use, because they are both public
   int i;//otherwise, a quick recursive method
   neighborsOut.clear();
   int numNodes = getNumberOfNodes();
   int expected = (7 * (depth + 1) * depth) >> 1;//uses bitshift for cheap divide, equals 7 * depth * (depth + 1) / 2
   if (expected > numNodes) expected = numNodes;
   neighborsOut.reserve(expected);//reserve means make space, but don't change size()
   if ((int)markNodes.size() != numNodes)
   {
      markNodes.resize(numNodes);
      for (i = 0; i < numNodes; ++i)
      {
         markNodes[i] = 0;
      }
   }
   markNodes[rootNode] = depth + 1;
   depthNeighHelper(rootNode, depth, neighborsOut);//uses depth first graph search: fast, but very unordered, if sorted is desired, sort afterwards
   int numNeigh = neighborsOut.size();//NOTE: depth first technically "wastes" some effort by searching nodes to a lesser depth then required
   for (i = 0; i < numNeigh; ++i)
      markNodes[neighborsOut[i]] = 0;
   markNodes[rootNode] = 0;//only zero the nodes that were marked: after completion of call, array will be entirely zeroed
}

void TopologyHelper::depthNeighHelper(int node, int depthrem, std::vector<int>& neighborsOut) const
{
   const std::vector<int>& neighbors = nodes[node].neighbors;
   int i, nextdepth = depthrem - 1, numNeigh = neighbors.size(), thisNeigh, mark;
   if (nextdepth)
   {//splitting here removes a large number of recursive calls and compares
      for (i = 0; i < numNeigh; ++i)
      {
         thisNeigh = neighbors[i];
         mark = markNodes[thisNeigh];
         if (depthrem > mark)
         {
            if (!mark)
            {
               neighborsOut.push_back(thisNeigh);
            }
            markNodes[thisNeigh] = depthrem;
            depthNeighHelper(thisNeigh, nextdepth, neighborsOut);
         }
      }
   } else {//same, but don't recurse
      for (i = 0; i < numNeigh; ++i)
      {
         thisNeigh = neighbors[i];
         if (!markNodes[thisNeigh])
         {
            markNodes[thisNeigh] = 1;
            neighborsOut.push_back(thisNeigh);
         }
      }
   }
}

/**
 * Get the neighbors of a node to a specified depth.
 */
void 
TopologyHelper::getNodeNeighborsToDepthOld(const int rootNode, 
                                        const int depth,
                                        std::vector<int>& neighborsOut) const
{
   neighborsOut.clear();
   
   const int numNodes = getNumberOfNodes();
   std::vector<int> nodeVisited(numNodes, 0);
   
   std::set<int> nodesMarked;
   nodesMarked.insert(rootNode);
   
   for (int dp = 0; dp < depth; dp++) {
      std::set<int> newNodes;
      
      for (std::set<int>::iterator it = nodesMarked.begin();
           it != nodesMarked.end(); it++) {
         const int node = *it; 
         
         if (nodeVisited[node] == 0) {
            nodeVisited[node] = 1;
            //
            // Get all of the nodes neighbors
            //
            const std::vector<int>& neighbors = nodes[node].neighbors;
            const unsigned int numNeighs = neighbors.size();
            
            for (unsigned int j = 0; j < numNeighs; j++) {
               const int n = neighbors[j];
               if (nodeVisited[n] == 0) {
                  newNodes.insert(n);
               }
            }
         }
      }
      
      nodesMarked.insert(newNodes.begin(), newNodes.end());
   }
   
   for (std::set<int>::iterator it = nodesMarked.begin();
        it != nodesMarked.end(); it++) {
      const int node = *it; 
      if (node != rootNode) {
         neighborsOut.push_back(node);
      }
   }
   
/*   
   const int numNodes = getNumberOfNodes();

   //
   // Neighbors found to depth
   //
   std::vector<int> neighborsFound(numNodes, 0);
   
   //
   // Mark root node as found
   //
   neighborsFound[rootNode] = 1;
   
   //
   // Find nodes to the specified depth
   //
   for (int dp = 0; dp <= depth; dp++) {
      for (int i = 0; i < numNodes; i++) {
         //
         // If this node is not marked yet
         //
         if (neighborsFound[i] == 0) {
            //
            // Get all of the nodes neighbors
            //
            int numNeighs = 0;
            const int* nodeNeighbors = getNodeNeighbors(i, numNeighs);
            
            //
            // Add nodes neighbors for next iteration
            //
            for (int k = 0; k < numNeighs; k++) {
               //
               // If a neighbor is marked
               //
               const int n = nodeNeighbors[k];
               if (neighborsFound[n]) {
                  //
                  // Mark me
                  //
                  neighborsFound[i] = 1;
                  break;
               }
            }
         }
      }
   }
   
   for (int i = 0; i < numNodes; i++) {
      if (i != rootNode) {
         if (neighborsFound[i]) {
            neighborsOut.push_back(i);
         }
      }
   }   
*/

/*   
   //
   // Neighbors found to depth
   //
   std::set<int> neighborsFound;
   
   //
   // Keep track of nodes visited so far
   //
   std::vector<int> visited(numNodes, false);
   
   //
   // List of nodes to search at each depth iteration
   //
   std::vector<int> nodesToSearch;
   nodesToSearch.push_back(rootNode);
   
   //
   // Find nodes to the specified depth
   //
   for (int dp = 0; dp <= depth; dp++) {
      //
      // Nodes for next depth iteration (use set so list is unique)
      //
      std::set<int> nodesForNextIteration;
      
      //
      // For nodes that are to be searched
      //
      const int numToSearch = static_cast<int>(nodesToSearch.size());
      for (int i = 0; i < numToSearch; i++) {
         //
         // Node that is to be searched
         //
         const int node = nodesToSearch[i];
         
         //
         // If this node has not already been visited
         //
         if (visited[node] == false) {
            //
            // Mark this node visited
            //
            visited[node] = true;
            
            //
            // Add it to the list of neighbors
            //
            neighborsFound.insert(node);
            
            //
            // Get all of the nodes neighbors
            //
            int numNeighs = 0;
            const int* nodeNeighbors = getNodeNeighbors(node, numNeighs);
            
            //
            // Add nodes neighbors for next iteration
            //
            for (int k = 0; k < numNeighs; k++) {
               const int n = nodeNeighbors[k];
               if (visited[n] == false) {
                  nodesForNextIteration.insert(n);
               }
            }
         }
      }
      
      //
      // Update list of neighbors to search for next iteration
      //
      nodesToSearch.clear();
      nodesToSearch.insert(nodesToSearch.end(),
                           nodesForNextIteration.begin(), nodesForNextIteration.end());
   }

   //
   // Set the output neighbors exluding the input node
   //
   for (std::set<int>::iterator iter = neighborsFound.begin();
        iter != neighborsFound.end(); iter++) {
      const int n = *iter;
      if (n != rootNode) {
         neighborsOut.push_back(n);
      }
   }
*/
}
      
/**
 * Get the neighboring nodes for a node.  Returns a pointer to an array
 * containing the neighbors.
 */
const int*
TopologyHelper::getNodeNeighbors(const int nodeNum, int& numNeighborsOut) const
{
    CaretAssertVectorIndex(nodes, nodeNum);
    numNeighborsOut = (int)nodes[nodeNum].neighbors.size();
    return nodes[nodeNum].neighbors.data();
}

/**
 * Get the tiles that use this node.
 */
void
TopologyHelper::getNodeTiles(const int nodeNum, std::vector<int>& tilesOut) const
{
    CaretAssertVectorIndex(nodes, nodeNum);
    tilesOut = nodes[nodeNum].tiles;
}

const int* TopologyHelper::getNodeTiles(const int nodeNum, int& numTilesOut)
{
    CaretAssertVectorIndex(nodes, nodeNum);
    numTilesOut = (int)nodes[nodeNum].tiles.size();
    return nodes[nodeNum].tiles.data();
}
