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

#include "BorderTracingHelper.h"

#include "Border.h"
#include "BorderFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

using namespace caret;
using namespace std;

BorderTracingHelper::BorderTracingHelper(const SurfaceFile* surfIn)
{
    m_numNodes = surfIn->getNumberOfNodes();
    m_structure = surfIn->getStructure();
    m_topoHelp = surfIn->getTopologyHelper();//doesn't need sorted, because we may need to walk in "reverse", so don't rely on sorting
}

vector<CaretPointer<Border> > BorderTracingHelper::tracePrivate(vector<int>& marked, const float& placement)
{
    vector<CaretPointer<Border> > ret;
    const vector<TopologyEdgeInfo>& myEdgeInfo = m_topoHelp->getEdgeInfo();
    const vector<TopologyTileInfo>& myTileInfo = m_topoHelp->getTileInfo();
    vector<int> edgeUsed(myEdgeInfo.size(), 0);
    float nodeWeights[3] = { 1.0f - placement, placement, 0.0f };
    while (true)
    {
        bool foundStart = false;
        int curInNode = -1, curEdge = -1, curOutNode = -1;
        for (int i = 0; i < m_numNodes; ++i)
        {
            if (marked[i] != 0)
            {
                const vector<int32_t>& edges = m_topoHelp->getNodeEdges(i);
                int numEdges = (int)edges.size();
                for (int j = 0; j < numEdges; ++j)
                {
                    const TopologyEdgeInfo& thisEdge = myEdgeInfo[edges[j]];
                    int testNode = (thisEdge.node2 == i ? thisEdge.node1 : thisEdge.node2);
                    if (marked[testNode] == 0 && edgeUsed[edges[j]] == 0)
                    {
                        if (!foundStart || thisEdge.numTiles == 1)
                        {
                            foundStart = true;
                            curInNode = i;
                            curEdge = edges[j];
                            curOutNode = testNode;
                            if (thisEdge.numTiles == 1) break;//if we found the end of an open border, stop searching
                        }
                    }
                }
            }
        }
        if (!foundStart) break;
        CaretPointer<Border> newBorder(new Border());//in case something throws
        int startInNode = curInNode, startOutNode = curOutNode;
        int prevThirdNode = -1;
        do
        {
            edgeUsed[curEdge] = 1;//don't start another border from this edge
            const TopologyEdgeInfo& myEdge = myEdgeInfo[curEdge];//to remove some redundant indexing
            CaretPointer<SurfaceProjectedItem> newPoint(new SurfaceProjectedItem());//ditto
            newPoint->setStructure(m_structure);
            newPoint->getBarycentricProjection()->setProjectionSurfaceNumberOfNodes(m_numNodes);
            int32_t triNodes[3] = { curInNode, curOutNode, myEdge.tiles[0].node3 };//always use the first tile, as it will always exist
            newPoint->getBarycentricProjection()->setTriangleNodes(triNodes);
            newPoint->getBarycentricProjection()->setTriangleAreas(nodeWeights);
            newPoint->getBarycentricProjection()->setValid(true);
            newBorder->addPoint(newPoint.releasePointer());//NOTE: addPoint takes ownership of a RAW POINTER - shared_ptr won't release a pointer
            int useTile = 0;
            if (myEdge.tiles[0].node3 == prevThirdNode)
            {
                if (myEdge.numTiles == 1) break;
                useTile = 1;
            }
            int nextNode = myEdge.tiles[useTile].node3;
            int edgeMove = 0;
            if (marked[nextNode] == 0)
            {
                edgeMove = (myEdge.tiles[useTile].edgeReversed == (myEdge.node1 == curInNode) ? 1 : 2);//some magic to find the next edge via the lookups
                prevThirdNode = curOutNode;
                curOutNode = nextNode;
            } else {
                edgeMove = (myEdge.tiles[useTile].edgeReversed == (myEdge.node1 == curInNode) ? 2 : 1);//rather than searching the edges on the tile or such
                prevThirdNode = curInNode;
                curInNode = nextNode;
            }
            int edgeIndex = (myEdge.tiles[useTile].whichEdge + edgeMove) % 3;//modify the edge index within the tile by the magic offset
            curEdge = myTileInfo[myEdge.tiles[useTile].tile].edges[edgeIndex].edge;//and pull out the global index of the edge
            CaretAssert(myEdgeInfo[curEdge].node1 == curInNode || myEdgeInfo[curEdge].node1 == curOutNode);//assert to make sure the magic worked
            CaretAssert(myEdgeInfo[curEdge].node2 == curInNode || myEdgeInfo[curEdge].node2 == curOutNode);
        } while (curInNode != startInNode || curOutNode != startOutNode);
        ret.push_back(newBorder);
    }
    return ret;
}
