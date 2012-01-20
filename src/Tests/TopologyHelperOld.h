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



#ifndef __VE_TOPOLOGY_HELPER_H__
#define __VE_TOPOLOGY_HELPER_H__

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include "CaretMutex.h"

namespace caret {

   class SurfaceFile;

   /// Stores the two nodes and two tiles for an edge (link) in the surface.
   class TopologyEdgeInfoOld {
      public:
         /// nodes in the edge
         int node1, node2;
         
         /// tiles used by the edge
         int tile1, tile2;
         
         /// flat to indicate if edge is used by more than two triangles
         bool edgeHasMoreThanTwoTriangles;
         
         /// constructor
         TopologyEdgeInfoOld(const int tileIn, const int node1In, const int node2In) {
            edgeHasMoreThanTwoTriangles = false;
            tile1 = tileIn;
            tile2 = -1;
            node1 = node1In;
            node2 = node2In;
            if (node2 > node1) {
               std::swap(node1, node2);
            }
         }
         
         /// equals operator
         bool operator==(const TopologyEdgeInfoOld& e) {
            return ((node1 == e.node1) && (node2 == e.node2));
         }
         
         /// add a tile to the edge
         void addTile(const int tileIn) {
            if (tile2 < 0) {
               tile2 = tileIn;
            }
            else {
               edgeHasMoreThanTwoTriangles = true;
               /*if (DebugControl::getDebugOn()) {
                  std::cout << "INFO: Edge (" << node1 << ", " << node2
                              << ") is used by more than two tiles" << std::endl;
                  std::cout << "   Triangles: " << tile1 << " " << tile2 
                           << " " << tileIn << std::endl;
               }//*/
            }
         }
         
         /// get the more than two triangles flag
         bool getEdgeUsedByMoreThanTwoTriangles() const { return edgeHasMoreThanTwoTriangles; }
         
         /// get the nodes
         void getNodes(int& n1, int& n2) const { n1 = node1; n2 = node2; }
         
         /// get the tiles
         void getTiles(int& t1, int& t2) const { t1 = tile1; t2 = tile2; }

         /// return positive if the edge is oriented the same as the triangle in
         int getEdgeOrientation(const int nodes[3]) const {
            for (int i = 0; i < 3; i++) {
               if (nodes[i] == node1) {
                  int iNext = i + 1;
                  if (iNext >= 3) iNext = 0;
                  if (nodes[iNext] == node2) {
                     return 1; 
                  }
                  else {
                     return -1;
                  }
               }
            }
            return 0;
         }

   };
         
         
   /// This class is used to determine the node neighbors and edges for a Topology File.
   class TopologyHelperOld {
      public:
         /// Constructor for use with a Caret Topology File
         TopologyHelperOld(const SurfaceFile* sfIn,
                        const bool sortNodeInfo = false);
         
         /// Destructor
         ~TopologyHelperOld();
         
         /// Get the number of nodes
         int getNumberOfNodes() const { return nodes.size(); }
         
         /// See if a node has neighbors
         bool getNodeHasNeighbors(const int nodeNum) const;
         
         /// Get the number of neighbors for a node
         int getNodeNumberOfNeighbors(const int nodeNum) const;

         /// Get the neighbors of a node
         void getNodeNeighbors(const int nodeNum, std::vector<int>& neighborsOut) const;

         /// Get the neighbors of a node
         const std::vector<int>& getNodeNeighbors(const int nodeNum) const;

         /// Get the neighboring nodes for a node.  Returns a pointer to an array
         /// containing the neighbors.
         const int* getNodeNeighbors(const int nodeNum, int& numNeighborsOut) const;
         
         /// Get the node neighbors, restricted by roi
         void getNodeNeighborsInROI(const int nodeNum, std::vector<int>& neighborsOut, const float *roiValues) const;
         
         /// Get the neighbors of a node to a specified depth the old way
         void getNodeNeighborsToDepthOld(const int nodeNum, 
                                    const int depth,
                                    std::vector<int>& neighborsOut) const;
         
         /// Get the neighbors to a specified depth as fast as possible, order unimportant (this is the new code)
         void getNodeNeighborsToDepth(const int nodeNum, 
                                    const int depth,
                                    std::vector<int>& neighborsOut) const;

         /// Get the neighbors to a specified depth preserving the kind of ordering of the old code
         void getNodeNeighborsToDepthIter(const int nodeNum, 
                                    const int depth,
                                    std::vector<int>& neighborsOut) const;

      private:
         void depthNeighHelper(int root, int remdepth, std::vector<int>& neighborsOut) const;
         mutable std::vector<int> markNodes, nodelist[2];//persistent, never cleared, only initialized once, saving bazillions of nanoseconds
         mutable CaretMutex usingMarkNodes;
         //could be templated over bool, which stores bitwise, but we already have three times that many floats and six times the ints in memory
      public:
         
         /// Get the number of boundary edges used by node
         void getNumberOfBoundaryEdgesForAllNodes(std::vector<int>& numBoundaryEdges) const;
         
         /// Get the maximum number of neighbors of all nodes
         int getMaximumNumberOfNeighbors() const;
         
         /// Get the tiles used by a node
         void getNodeTiles(const int nodeNum, std::vector<int>& tilesOut) const;
         
         /// Get the tiles used by a node
         const std::vector<int>& getNodeTiles(const int nodeNum) const;
         
         /// Get the tiles for a node.  Returns a pointer to an array
         /// containing the tiles.
         const int* getNodeTiles(const int nodeNum, int& numTilesOut);
         
         /// get node sorted info validity
         bool getNodeSortedInfoValid() const { return nodeSortedInfoBuilt; }
                     
         /// Get the edge information
         const std::set<TopologyEdgeInfoOld>& getEdgeInfo() const { return topologyEdges; }
         
         /// Get the number of edges
         int getNumberOfEdges() const { return topologyEdges.size(); }

      private:
         
         /// Stores tiles and vertices of an edge (link) for use when sorting nodes by NodeInfo.
         /// These edges are those opposite of a NodeInfo node, this is, given a tile with
         /// nodes A, B, and C, the nodes B and C would be stored in an NodeEdgeInfo for node A. 
         class NodeEdgeInfo {
            public:
               /// nodes in the edge
               int node1, node2;
               
               /// tile used by this edge
               int tileNumber;
               
               /// constructor
               NodeEdgeInfo(const int tileNum, const int node1In, const int node2In) {
                  tileNumber = tileNum;
                  node1 = node1In;
                  node2 = node2In;
               }
               
               /// get the other node in the edge
               int getOtherNode(const int node) const {
                  if (node1 == node) return node2;
                  else               return node1;
               }
               
               /// see if a node is in the edge
               bool containsNode(const int node) const {
                  return ((node1 == node) || (node2 == node));
               }
         };
         
         /// Stores information about a node for sorting
         class NodeInfo {
            public:
               /// this node's number
               int nodeNumber;
               
               /// tile's used by this node
               std::vector<int> tiles;
               
               /// the neighboring nodes of this edge
               std::vector<int> neighbors;
               
               /// the edges used to form tiles with this node
               std::vector<NodeEdgeInfo> edges;
               
               /// allow sorting flag
               bool  sortMe;
               
               /// constructor
               NodeInfo(const int nodeNumberIn) {
                  nodeNumber = nodeNumberIn;
                  sortMe = true;
               }
               
               /// add a neighbor to this node without edge information
               void addNeighbor(const int newNeighbor1) {
                  for (unsigned int i = 0; i < neighbors.size(); i++) {
                     if (neighbors[i] == newNeighbor1) {
                        return;
                     }
                  }
                  neighbors.push_back(newNeighbor1);
                  sortMe = false;
               }
               
               /// add two neighbors that form a tile using this node
               void addNeighbors(const int tileNum, const int newNeighbor1, const int newNeighbor2) {
                  edges.push_back(NodeEdgeInfo(tileNum, newNeighbor1, newNeighbor2));
               }

               /// add a tile to the node
               void addTile(const int tileNum) { tiles.push_back(tileNum); }
               
               /// find an edge that has one endpoint equal to "seekingNode" but
               /// does not contain the node "notNode".
               const NodeEdgeInfo* findEdgeWithPoint(const int seekingNode, 
                                                const int notNode) {
                  for (unsigned int i = 0; i < edges.size(); i++) {
                     if ( ((edges[i].node1 == seekingNode) &&
                           (edges[i].node2 != notNode)) ||
                        ((edges[i].node2 == seekingNode) &&
                           (edges[i].node1 != notNode)) ) {
                        return &edges[i];
                     }
                  }
                  return NULL;
               }

               /// sort the neighbors for this node
               void sortNeighbors();
         };
         
         /// Node storage
         std::vector<NodeInfo> nodes;
         
         /// edge storage
         std::set<TopologyEdgeInfoOld> topologyEdges;
         
         /// add information about an edge
         void addEdgeInfo(const int tileNum, const int node1, const int node2);
         
         /// node sorted info built
         bool nodeSortedInfoBuilt;
         
   };

   bool operator<(const TopologyEdgeInfoOld& e1, const TopologyEdgeInfoOld& e2);

}
#endif // __VE_TOPOLOGY_HELPER_H__


