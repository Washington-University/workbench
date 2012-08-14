#ifndef __TOPOLOGY_HELPER_H__
#define __TOPOLOGY_HELPER_H__

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

#include <vector>
#include "CaretPointer.h"

namespace caret {

    class SurfaceFile;
    
    struct TopologyEdgeInfo
    {
        struct Tile
        {
            int32_t tile;
            int32_t node3;
            int32_t whichEdge;//whether this is edge 0 (0-1), 1 (1-2), or 2 (2-0)
            bool edgeReversed;//whether ordering the nodes as 1, 2, 3 results in a flipped tile compared to topology
        };
        int32_t node1, node2;
        int32_t numTiles;
        Tile tiles[2];//should be so amazingly rare (and inherently bad) for an edge to have 3 triangles that it isn't worth making this a vector
        TopologyEdgeInfo()//also, a vector would have poor data locality, each edge would have its tiles list in an unrelated spot to the previous edge
        {
            numTiles = 0;
        }
        TopologyEdgeInfo(const int32_t& firstNode, const int32_t& secondNode, const int32_t& thirdNode, const int32_t& tile, const int32_t& whichEdge, const bool& reversed)
        {
            node1 = firstNode;//always called with firstNode less than secondNode, so don't need to swap
            node2 = secondNode;
            tiles[0].tile = tile;
            tiles[0].node3 = thirdNode;
            tiles[0].edgeReversed = reversed;
            tiles[0].whichEdge = whichEdge;
            numTiles = 1;
        }
        void addTile(const int32_t& thirdNode, const int32_t& tile, const int32_t& whichEdge, const bool& reversed)
        {
            if (numTiles < 2)
            {
                tiles[numTiles].tile = tile;
                tiles[numTiles].node3 = thirdNode;
                tiles[numTiles].edgeReversed = reversed;
                tiles[numTiles].whichEdge = whichEdge;
            }
            ++numTiles;
        }
    };
    
    struct TopologyTileInfo
    {
        struct Edge
        {
            int32_t edge;
            bool reversed;
        };
        Edge edges[3];
    };
    
    class TopologyHelperBase
    {
        TopologyHelperBase();//prevent default, copy, assign
        TopologyHelperBase(const TopologyHelperBase&);
        TopologyHelperBase& operator=(const TopologyHelperBase&);
        void processTileNeighbor(std::vector<TopologyEdgeInfo>& tempEdgeInfo, CaretArray<int32_t>& scratch, const int32_t& root, const int32_t& neighbor, const int32_t& thirdNode, const int32_t& tile, const int32_t& tileEdge, const bool& reversed);
        void sortNeighbors(const SurfaceFile* mySurf, const int32_t& node, CaretArray<int32_t>& nodeScratch, CaretArray<int32_t>& tileScratch);
        struct NodeInfo
        {
            std::vector<int32_t> m_neighbors;
            std::vector<int32_t> m_edges;//index into the topology edges vector, matched with neighbors
            std::vector<int32_t> m_tiles;
            std::vector<int32_t> m_whichVertex;//stores which tile vertex this node is, matched to m_tiles
            void addTileInfo(int32_t tile, int32_t vertexNum)//don't take edge info yet because it is built after neighbor info
            {
                m_tiles.push_back(tile);
                m_whichVertex.push_back(vertexNum);
            }
            void addNeighborInfo(int32_t neighbor, int32_t edge)//after we have tile info, we then generate neighbor info with the help of a mark array
            {
                m_neighbors.push_back(neighbor);
                m_edges.push_back(edge);
            }
        };
        std::vector<NodeInfo> m_nodeInfo;
        std::vector<TopologyEdgeInfo> m_edgeInfo;
        std::vector<TopologyTileInfo> m_tileInfo;
        std::vector<int32_t> m_boundaryCount;
        int32_t m_maxNeigh, m_maxTiles, m_numNodes, m_numTris;
        bool m_neighborsSorted;
    public:
        TopologyHelperBase(const SurfaceFile* surfIn, bool sortNeighbors = false);
        bool isNodeInfoSorted() const {
            return m_neighborsSorted;
        }
        friend class TopologyHelper;
    };
    
    /// This class is used to determine the node neighbors and edges for a Topology File.
    class TopologyHelper {
        CaretPointer<TopologyHelperBase> m_base;
        mutable CaretArray<int> m_markNodes, m_nodelist[2];//persistent, never cleared, only initialized once, saving bazillions of nanoseconds
        mutable CaretMutex m_usingMarkNodes;
        bool m_neighborsSorted;
        int32_t m_numNodes, m_maxNeigh;
        const std::vector<TopologyHelperBase::NodeInfo>& m_nodeInfo;//references for convenience instead of using the m_base pointer
        const std::vector<TopologyEdgeInfo>& m_edgeInfo;
        const std::vector<TopologyTileInfo>& m_tileInfo;
        const std::vector<int32_t>& m_boundaryCount;
        
        void checkArrays() const;//used to make the thread arrays for neighbors to depth lazy (not allocated until first needed)
        
        TopologyHelper();//prevent default, copy, assign, prolly not needed since there are reference members
        TopologyHelper(const TopologyHelper& right);
        TopologyHelper& operator=(const TopologyHelper& right);
    public:
        /// Constructor for use with a TopologyHelperBase (the only way, for now)
        TopologyHelper(CaretPointer<TopologyHelperBase> myBase);

        /// Get the number of nodes
        int32_t getNumberOfNodes() const {
            return m_numNodes;
        }

        /// See if a node has neighbors
        bool getNodeHasNeighbors(const int32_t nodeNum) const;

        /// Get the number of neighbors for a node
        int32_t getNodeNumberOfNeighbors(const int32_t nodeNum) const;

        /// Get the neighbors of a node
        const std::vector<int32_t>& getNodeNeighbors(const int32_t nodeNum) const;

        /// Get the neighboring nodes for a node.  Returns a pointer to an array
        /// containing the neighbors.
        const int32_t* getNodeNeighbors(const int32_t nodeNum, int32_t& numNeighborsOut) const;
        
        ///get the edges of a node
        const std::vector<int32_t>& getNodeEdges(const int32_t nodeNum) const;

        /// Get the neighbors to a specified depth
        void getNodeNeighborsToDepth(const int32_t nodeNum,
                                    const int32_t depth,
                                    std::vector<int32_t>& neighborsOut) const;

        /// Get the number of boundary edges used by node
        const std::vector<int32_t>& getNumberOfBoundaryEdgesForAllNodes() const;

        /// Get the maximum number of neighbors of all nodes
        int32_t getMaximumNumberOfNeighbors() const;

        /// Get the tiles used by a node
        const std::vector<int32_t>& getNodeTiles(const int32_t nodeNum) const;

        /// Get the tiles for a node.  Returns a pointer to an array
        /// containing the tiles.
        const int32_t* getNodeTiles(const int32_t nodeNum, int32_t& numTilesOut) const;

        /// get node sorted info validity
        bool isNodeInfoSorted() const {
            return m_neighborsSorted;
        }

        /// Get the edge information
        const std::vector<TopologyEdgeInfo>& getEdgeInfo() const {
            return m_edgeInfo;
        }

        /// Get the tile information
        const std::vector<TopologyTileInfo>& getTileInfo() const {
            return m_tileInfo;
        }

        /// Get the number of edges
        int32_t getNumberOfEdges() const {
            return m_edgeInfo.size();
        }

    };

}

#endif //__TOPOLOGY_HELPER_H__
