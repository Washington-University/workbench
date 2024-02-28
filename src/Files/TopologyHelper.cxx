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

#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "CaretAssert.h"
#include <cmath>

using namespace caret;
using namespace std;

TopologyHelperBase::TopologyHelperBase(const SurfaceFile* surfIn, bool sortFlag)
{
    m_numNodes = surfIn->getNumberOfNodes();
    m_numTris = surfIn->getNumberOfTriangles();
    m_nodeInfo.resize(m_numNodes);
    m_boundaryCount.resize(m_numNodes);
    m_tileInfo.resize(m_numTris);
    vector<TopologyEdgeInfo> tempEdgeInfo;
    tempEdgeInfo.reserve(m_numTris * 3);//worst case, to prevent reallocs, we will copy it over later to the exact right size
    for (int32_t i = 0; i < m_numNodes; ++i)
    {//preallocate what should be enough size for most nodes on most surfaces, including freesurfer
        m_nodeInfo[i].m_edges.reserve(8);
        m_nodeInfo[i].m_neighbors.reserve(8);
        m_nodeInfo[i].m_tiles.reserve(8);
        m_nodeInfo[i].m_whichVertex.reserve(8);
    }
    for (int32_t i = 0; i < m_numTris; ++i)
    {
        const int32_t* thisTri = surfIn->getTriangle(i);
        m_nodeInfo[thisTri[0]].addTileInfo(i, 0);
        m_nodeInfo[thisTri[1]].addTileInfo(i, 1);
        m_nodeInfo[thisTri[2]].addTileInfo(i, 2);
    }//node tiles complete, now we can sweep over nodes instead of triangles, making it easier to build node info
    m_maxNeigh = -1;
    m_maxTiles = -1;
    CaretArray<int32_t> scratch(m_numNodes, -1);//mark array for added neighbors
    for (int32_t i = 0; i < m_numNodes; ++i)
    {
        vector<int32_t>& tileList = m_nodeInfo[i].m_tiles;
        vector<int32_t>& vertexList = m_nodeInfo[i].m_whichVertex;
        int neighTiles = (int)tileList.size();
        if (neighTiles > m_maxTiles)
        {
            m_maxTiles = neighTiles;
        }
        for (int j = 0; j < neighTiles; ++j)
        {
            int32_t myTile = tileList[j];
            const int32_t* thisTri = surfIn->getTriangle(myTile);
            int32_t myVert = vertexList[j];
            switch (myVert)
            {
                case 0:
                    if (thisTri[1] > i) processTileNeighbor(tempEdgeInfo, scratch, i, thisTri[1], thisTri[2], myTile, 0, false);//boolean signifies if root, neighbor is same ordering as the cycle of tile nodes
                    if (thisTri[2] > i) processTileNeighbor(tempEdgeInfo, scratch, i, thisTri[2], thisTri[1], myTile, 2, true);
                    break;//the if statement is a trick: processTileNeighbor adds neighbor to both nodes, so by checking that root is less, it does every edge/tile pair exactly once
                case 1://this allows edge info building in a linear pass
                    if (thisTri[2] > i) processTileNeighbor(tempEdgeInfo, scratch, i, thisTri[2], thisTri[0], myTile, 1, false);
                    if (thisTri[0] > i) processTileNeighbor(tempEdgeInfo, scratch, i, thisTri[0], thisTri[2], myTile, 0, true);
                    break;
                case 2:
                    if (thisTri[0] > i) processTileNeighbor(tempEdgeInfo, scratch, i, thisTri[0], thisTri[1], myTile, 2, false);
                    if (thisTri[1] > i) processTileNeighbor(tempEdgeInfo, scratch, i, thisTri[1], thisTri[0], myTile, 1, true);
            }
        }
        vector<int32_t>& myNeighList = m_nodeInfo[i].m_neighbors;
        vector<int32_t>& myEdgeList = m_nodeInfo[i].m_edges;
        int numNeigh = (int)myNeighList.size();
        if (numNeigh > m_maxNeigh)
        {
            m_maxNeigh = numNeigh;
        }
        m_boundaryCount[i] = 0;
        for (int j = 0; j < numNeigh; ++j)
        {
            if (tempEdgeInfo[myEdgeList[j]].numTiles == 1) ++m_boundaryCount[i];
            scratch[myNeighList[j]] = -1;//NOTE: -1 as sentinel because 0 is a valid edge number
        }
    }//neighbor, edge and tile info done
    m_edgeInfo = tempEdgeInfo;//copy edge info into member to get allocation correct
    CaretArray<int32_t> scratch2(m_numTris, -1);
    if (sortFlag)
    {
        for (int32_t i = 0; i < m_numNodes; ++i)
        {
            sortNeighbors(surfIn, i, scratch, scratch2);//not a member function of node info object because I need m_edgeInfo and m_nodeInfo
        }
        m_neighborsSorted = true;
    } else {
        m_neighborsSorted = false;
    }
}

//1) check mark array
//      a) if marked, find edge, add triangle to edge
//      b) if unmarked, make edge from triangle, add neighbor, add reverse neighbor
void TopologyHelperBase::processTileNeighbor(vector<TopologyEdgeInfo>& tempEdgeInfo, CaretArray<int32_t>& scratch, const int32_t& root, const int32_t& neighbor, const int32_t& thirdNode, const int32_t& tile, const int32_t& tileEdge, const bool& reversed)
{
    if (scratch[neighbor] == -1)
    {
        TopologyEdgeInfo tempInfo(root, neighbor, thirdNode, tile, tileEdge, reversed);
        int32_t myEdge = (int32_t)tempEdgeInfo.size();
        tempEdgeInfo.push_back(tempInfo);
        m_nodeInfo[root].addNeighborInfo(neighbor, myEdge);
        m_nodeInfo[neighbor].addNeighborInfo(root, myEdge);
        scratch[neighbor] = myEdge;//use mark array both as "have this neighbor" AND "this is this neighbor's edge"
        m_tileInfo[tile].edges[tileEdge].edge = myEdge;
    } else {
        tempEdgeInfo[scratch[neighbor]].addTile(thirdNode, tile, tileEdge, reversed);
        m_tileInfo[tile].edges[tileEdge].edge = scratch[neighbor];
    }
    m_tileInfo[tile].edges[tileEdge].reversed = reversed;
}

void TopologyHelperBase::sortNeighbors(const SurfaceFile* mySurf, const int32_t& node, CaretArray<int32_t>& nodeScratch, CaretArray<int32_t>& tileScratch)
{
    TopologyHelperBase::NodeInfo& myNodeInfo = m_nodeInfo[node];
    if (myNodeInfo.m_neighbors.size() == 0) return;
    int firstIndex = 0, numNeigh = (int)myNodeInfo.m_neighbors.size();
    for (int i = 0; i < numNeigh; ++i)
    {
        int32_t thisEdge = myNodeInfo.m_edges[i];
        if (m_edgeInfo[thisEdge].numTiles == 1)//there cannot be edge info with zero tiles, we are looking for the edge of a cut
        {
            firstIndex = i;
            if ((m_edgeInfo[thisEdge].node1 == node) != m_edgeInfo[thisEdge].tiles[0].edgeReversed)//this checks if the edge from center to neighbor is oriented with the tile
            {//since edge info is always node1 < node2, we have to xor (this center is node1) and (this edge is reversed compared to its only triangle)
                break;//found one
            }//the reason the break is in the additional if, is so that if we don't find a correctly oriented edge, we still find an edge if one exists
        }
    }
    vector<int32_t> tempNeigh;
    vector<int32_t> tempEdges, tempTiles;//why not sort everything? verts get regenerated in place
    int numTiles = myNodeInfo.m_tiles.size();
    tempNeigh.reserve(numNeigh);
    tempEdges.reserve(numNeigh);
    tempTiles.reserve(numTiles);
    int32_t nextNode = myNodeInfo.m_neighbors[firstIndex];
    int32_t nextEdge = myNodeInfo.m_edges[firstIndex];
    int32_t nextTile;
    bool foundNext = true;
    int tileToUse = 0;
    if (m_edgeInfo[nextEdge].numTiles > 1)
    {
        if (m_edgeInfo[nextEdge].tiles[0].edgeReversed != (m_edgeInfo[nextEdge].node1 == nextNode))
        {
            tileToUse = 1;
        }
    }
    do
    {
        nextTile = m_edgeInfo[nextEdge].tiles[tileToUse].tile;
        int32_t node3 = m_edgeInfo[nextEdge].tiles[tileToUse].node3;
        nextEdge = -1;
        for (int i = 0; i < 3; ++i)
        {
            if (m_edgeInfo[m_tileInfo[nextTile].edges[i].edge].node1 == node)
            {
                if (m_edgeInfo[m_tileInfo[nextTile].edges[i].edge].node2 == node3)
                {
                    nextEdge = m_tileInfo[nextTile].edges[i].edge;
                    break;
                }
            } else {
                if (m_edgeInfo[m_tileInfo[nextTile].edges[i].edge].node1 == node3 && m_edgeInfo[m_tileInfo[nextTile].edges[i].edge].node2 == node)
                {
                    nextEdge = m_tileInfo[nextTile].edges[i].edge;
                    break;
                }
            }
        }
        CaretAssert(nextEdge != -1);
        tempNeigh.push_back(nextNode);
        tempEdges.push_back(nextEdge);
        nodeScratch[nextNode] = 0;//remember, -1 is "unused"
        tempTiles.push_back(nextTile);
        nextNode = node3;//update the next node we are going to use
        tileScratch[nextTile] = 0;
        tileToUse = 0;
        if (tileScratch[m_edgeInfo[nextEdge].tiles[0].tile] == 0 && m_edgeInfo[nextEdge].numTiles > 1)
        {
            tileToUse = 1;
        }
        if (tileScratch[m_edgeInfo[nextEdge].tiles[tileToUse].tile] == 0)
        {
            foundNext = false;
        }
    } while (foundNext);
    for (int i = 0; i < numNeigh; ++i)//clean up scratch array, find any neighbors that are gap-separated or on third+ tile of an edge
    {
        if (nodeScratch[myNodeInfo.m_neighbors[i]] == 0)
        {
            nodeScratch[myNodeInfo.m_neighbors[i]] = -1;
        } else {
            tempNeigh.push_back(myNodeInfo.m_neighbors[i]);
            tempEdges.push_back(myNodeInfo.m_edges[i]);
        }
    }
    CaretAssert(tempNeigh.size() == myNodeInfo.m_neighbors.size());//check against original size
    CaretAssert(tempEdges.size() == myNodeInfo.m_edges.size());
    myNodeInfo.m_neighbors = tempNeigh;//copy over
    myNodeInfo.m_edges = tempEdges;
    for (int i = 0; i < numTiles; ++i)//and find similar tiles
    {
        if (tileScratch[myNodeInfo.m_tiles[i]] == 0)
        {
            tileScratch[myNodeInfo.m_tiles[i]] = -1;
        } else {
            tempTiles.push_back(myNodeInfo.m_tiles[i]);
        }
    }
    CaretAssert(tempTiles.size() == myNodeInfo.m_tiles.size());
    myNodeInfo.m_tiles = tempTiles;
    for (int i = 0; i < numTiles; ++i)//finally, regenerate verts
    {
        const int32_t* myTri = mySurf->getTriangle(myNodeInfo.m_tiles[i]);
        if (myTri[0] == node)
        {
            myNodeInfo.m_whichVertex[i] = 0;
        } else if (myTri[1] == node) {
            myNodeInfo.m_whichVertex[i] = 1;
        } else {
            myNodeInfo.m_whichVertex[i] = 2;
        }
    }
}

TopologyHelper::TopologyHelper(CaretPointer<TopologyHelperBase> myBase) : m_base(myBase), m_nodeInfo(myBase->m_nodeInfo), m_edgeInfo(myBase->m_edgeInfo),
                                                                                    m_tileInfo(myBase->m_tileInfo), m_boundaryCount(myBase->m_boundaryCount)
{//pointer is by-value so that it makes a private copy that can't be pointed elsewhere during this constructor
    m_maxNeigh = m_base->m_maxNeigh;
    m_neighborsSorted = m_base->m_neighborsSorted;
    m_numNodes = m_base->m_numNodes;
}

const vector<int32_t>& TopologyHelper::getNumberOfBoundaryEdgesForAllNodes() const
{
    return m_boundaryCount;
}

int32_t TopologyHelper::getMaximumNumberOfNeighbors() const
{
    return m_maxNeigh;
}

bool TopologyHelper::getNodeHasNeighbors(const int32_t nodeNum) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    return m_nodeInfo[nodeNum].m_neighbors.size() != 0;
}

const vector<int32_t>& TopologyHelper::getNodeNeighbors(const int32_t nodeNum) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    return m_nodeInfo[nodeNum].m_neighbors;
}

const int32_t* TopologyHelper::getNodeNeighbors(const int32_t nodeNum, int32_t& numNeighborsOut) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    numNeighborsOut = (int32_t)m_nodeInfo[nodeNum].m_neighbors.size();
    return m_nodeInfo[nodeNum].m_neighbors.data();
}

int32_t TopologyHelper::getNodeNumberOfNeighbors(const int32_t nodeNum) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    return m_nodeInfo[nodeNum].m_neighbors.size();
}

const vector<int32_t>& TopologyHelper::getNodeTiles(const int32_t nodeNum) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    return m_nodeInfo[nodeNum].m_tiles;
}

const int32_t* TopologyHelper::getNodeTiles(const int32_t nodeNum, int32_t& numTilesOut) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    numTilesOut = (int32_t)m_nodeInfo[nodeNum].m_tiles.size();
    return m_nodeInfo[nodeNum].m_tiles.data();
}

const vector<int32_t>& TopologyHelper::getNodeEdges(const int32_t nodeNum) const
{
    CaretAssertVectorIndex(m_nodeInfo, nodeNum);
    return m_nodeInfo[nodeNum].m_edges;
}

void TopologyHelper::checkArrays() const
{
    if (m_markNodes.size() != m_numNodes)
    {
        m_markNodes = CaretArray<int>(m_numNodes, 0);
        m_nodelist[0] = CaretArray<int32_t>(m_numNodes);
        m_nodelist[1] = CaretArray<int32_t>(m_numNodes);
    }
}

void TopologyHelper::getNodeNeighborsToDepth(const int32_t nodeNum, const int32_t depth, vector<int32_t>& neighborsOut) const
{
    if (depth < 2)
    {
        neighborsOut = getNodeNeighbors(nodeNum);
        return;
    }
    int32_t expected = (7 * depth * (depth + 1)) / 2;
    if (expected > m_numNodes / 2) expected = m_numNodes / 2;
    neighborsOut.clear();
    neighborsOut.reserve(expected);
    CaretArray<int32_t>* curlist = &(m_nodelist[0]), *nextlist = &(m_nodelist[1]), *templist;//using raw pointers instead of CaretArray::operator= because this is single threaded
    int32_t curNum = 1, nextNum = 0;//curnum gets initialized to 1 because it starts with the root node
    CaretMutexLocker locked(&m_usingMarkNodes);//lock before possibly constructing this object's scratch space
    checkArrays();
    m_markNodes[nodeNum] = 1;
    (*curlist)[0] = nodeNum;//just use iterative, because depth-first recursive does unneeded work, and has very little reason to ever be faster
    for (int32_t curdepth = 0; curdepth < depth; ++curdepth)
    {
        for (int32_t i = 0; i < curNum; ++i)
        {
            const vector<int32_t>& nodeNeighbors = m_nodeInfo[(*curlist)[i]].m_neighbors;
            int numNeigh = (int)nodeNeighbors.size();
            for (int j = 0; j < numNeigh; ++j)
            {
                int32_t thisNode = nodeNeighbors[j];
                if (m_markNodes[thisNode] == 0)
                {
                    m_markNodes[thisNode] = 1;
                    (*nextlist)[nextNum] = thisNode;
                    ++nextNum;
                    neighborsOut.push_back(thisNode);
                }
            }
        }
        templist = curlist;
        curlist = nextlist;
        nextlist = templist;
        curNum = nextNum;
        nextNum = 0;//we restart the list by zeroing the count, and just overwrite the old values
    }
    m_markNodes[nodeNum] = 0;//clean up the mark array
    int32_t numNeigh = (int32_t)neighborsOut.size();
    for (int32_t i = 0; i < numNeigh; ++i)
    {
        m_markNodes[neighborsOut[i]] = 0;
    }
}
