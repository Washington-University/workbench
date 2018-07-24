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

#include "GeodesicHelper.h"

#include "CaretAssert.h"
#include "CaretHeap.h"
#include "CaretMutex.h"
#include "FastStatistics.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <cmath>
#include <stdint.h>

using namespace caret;
using namespace std;

GeodesicHelperBase::GeodesicHelperBase(const SurfaceFile* surfaceIn, const float* correctedAreas)
{
    CaretPointer<TopologyHelperBase> topoBase(new TopologyHelperBase(surfaceIn));
    TopologyHelper topoHelpIn(topoBase);//leave this building one privately, to not introduce even worse dependencies regarding SurfaceFile
    m_corrAreaSmallestFactor = 1.0f;
    numNodes = surfaceIn->getNumberOfNodes();
    nodeNeighbors.resize(numNodes);
    distances.resize(numNodes);
    nodeCoords.resize(numNodes);
    vector<float> sqrtCorrAreas;//each edge has 2 vertices that influence it - assume that each influences a piece of the edge with a ratio depending on the square roots of the vertex areas
    vector<float> sqrtVertAreas;//we also assume isometric expansion at each vertex
    if (correctedAreas != NULL)//this gives an estimated original length of curLength * (sqrt(origA) + sqrt(origB))/(sqrt(curA) + sqrt(curB))
    {
        surfaceIn->computeNodeAreas(sqrtVertAreas);
        sqrtCorrAreas.resize(numNodes);
        for (int i = 0; i < numNodes; ++i)
        {
            sqrtCorrAreas[i] = sqrt(correctedAreas[i]);
            sqrtVertAreas[i] = sqrt(sqrtVertAreas[i]);
        }
    }
    Vector3D tempvec;
    float tempf, abmag, efmag, cdmag;
    double nodeSpacingAccum = 0.0f;//since we may be using corrected areas, find average node spacing manually
    int32_t numEdges = 0;
    bool firstCorrArea = true;//if all corrected vertex areas are significantly larger than 1, we can make A* faster by multiplying all euclidean distances by it, so find the actual smallest
    for (int32_t i = 0; i < numNodes; ++i)
    {//get neighbors
        vector<int32_t>& neighbors = nodeNeighbors[i];
        neighbors = topoHelpIn.getNodeNeighbors(i);
        nodeCoords[i] = surfaceIn->getCoordinate(i);
        const Vector3D baseCoord = nodeCoords[i];
        int numNeigh = (int)neighbors.size();
        distances[i].resize(numNeigh);
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            Vector3D neighCoord = surfaceIn->getCoordinate(neighbors[j]);
            tempvec = baseCoord - neighCoord;
            distances[i][j] = tempvec.length();//precompute for speed in other calls
            if (correctedAreas != NULL)
            {
                float correctionFactor = (sqrtCorrAreas[i] + sqrtCorrAreas[neighbors[j]]) / (sqrtVertAreas[i] + sqrtVertAreas[neighbors[j]]);
                if (firstCorrArea || correctionFactor < m_corrAreaSmallestFactor)
                {
                    m_corrAreaSmallestFactor = correctionFactor;//if this is zero anywhere, it just means that the euclidean part of the heuristic must be ignored (worst case, it does dijkstra)
                    firstCorrArea = false;
                }
                distances[i][j] *= correctionFactor;
            }
            if (i < neighbors[j])
            {
                nodeSpacingAccum += distances[i][j];
                ++numEdges;
            }
        }//so few floating point operations, this should turn out symmetric
    }
    m_avgNodeSpacing = nodeSpacingAccum / numEdges;
    std::vector<int32_t> tempneigh2;
    std::vector<float> tempdist2;
    nodeNeighbors2.resize(numNodes);
    distances2.resize(numNodes);
    neighbors2PathInfo.resize(numNodes);
    const vector<TopologyEdgeInfo>& myEdgeInfo = topoHelpIn.getEdgeInfo();
    CaretAssert(numEdges == (int32_t)myEdgeInfo.size());//SurfaceFile checks for triangles with duplicated nodes
    for (int i = 0; i < numEdges; ++i)
    {
        if (myEdgeInfo[i].numTiles < 2)
        {
            continue;//skip edges that have only one triangle
        }
        int32_t neigh1Node, neigh2Node, baseNode, farNode;
        neigh1Node = myEdgeInfo[i].node1;
        neigh2Node = myEdgeInfo[i].node2;
        baseNode = myEdgeInfo[i].tiles[0].node3;
        farNode = myEdgeInfo[i].tiles[1].node3;
        Vector3D neigh1Coord = nodeCoords[neigh1Node];
        Vector3D neigh2Coord = nodeCoords[neigh2Node];
        Vector3D baseCoord = nodeCoords[baseNode];
        Vector3D farCoord = nodeCoords[farNode];
        CrawlInfo tempInfo;
        tempInfo.edgeNodes[0] = neigh1Node;
        tempInfo.edgeNodes[1] = neigh2Node;
        const int32_t num_reserve = 8;//uses 8 in case it is used on a mesh with haphazard topology
        nodeNeighbors2[baseNode].reserve(num_reserve);//reserve should be fast if capacity is already num_reserve, and better than reallocating at 2 and 4, if vector allocation is naive doubling
        nodeNeighbors2[farNode].reserve(num_reserve);//in the extremely rare case of a node with more than num_reserve neighbors, a second allocation plus copy isn't much of a cost
        distances2[baseNode].reserve(num_reserve);
        distances2[farNode].reserve(num_reserve);
        neighbors2PathInfo[baseNode].reserve(num_reserve);
        neighbors2PathInfo[farNode].reserve(num_reserve);
        Vector3D abhat = (neigh2Coord - neigh1Coord).normal(&abmag);//a is neigh1, b is neigh2, b - a = (vector)ab
        Vector3D ac = farCoord - neigh1Coord;//c is farnode, c - a = (vector)ac
        Vector3D ad = abhat * abhat.dot(ac);//d is the point on the shared edge that farnode (c) is closest to
        Vector3D d = neigh1Coord + ad;//this way we can "unfold" the triangles by projecting the distance of cd, from point d, along the unit vector of the base node to closest point on shared edge
        Vector3D ea = neigh1Coord - baseCoord;//e is the base node, a - e = (vector)ea
        tempvec = abhat * abhat.dot(ea);//find vector fa, f being the point on shared edge closest to e, the base node
        Vector3D efhat = (ea - tempvec).normal(&efmag);//and subtract it to obtain only the perpendicular, normalize to get unit vector
        cdmag = (d - farCoord).length();//get the length from shared edge to far point
        Vector3D g = d + efhat * cdmag;//get point g, the unfolded position of farnode
        Vector3D eg = g - baseCoord;//this is the vector from base (e) to far node after unfolding (g), this is our distance, as long as the tetralateral is convex
        tempf = efmag / (efmag + cdmag);//now we need to check that the path stays inside the tetralateral (ie, that it is convex)
        Vector3D eh = eg * tempf;//this is a vector from e (base node) to the point on the shared edge that the full path (eg) crosses
        Vector3D ah = eh - ea;//eh - ea = eh + ae = ae + eh = ah, vector from neigh1 to the point on shared edge the path goes through
        tempf = ah.dot(abhat);//get the component along ab so we can test that it is positive and less than |ab|
        if (tempf <= 0.0f || tempf >= abmag) continue;//tetralateral is concave or triangular (degenerate), our path is invalid or not shorter, so consider next edge
        tempInfo.edgeWeight = 1.0f - tempf / abmag;//if tempf is almost zero, then the weight of point a (neigh1) is almost 1
        tempf = eg.length();//this is our path length
        tempInfo.pieceDists[1] = eh.length();//we currently add things to farNode's neighbor info before baseNode's
        if (correctedAreas != NULL)//apply area correction approximation
        {
            float correctionFactor = (sqrtCorrAreas[baseNode] + sqrtCorrAreas[farNode]) / (sqrtVertAreas[baseNode] + sqrtVertAreas[farNode]);
            if (correctionFactor < m_corrAreaSmallestFactor)
            {
                m_corrAreaSmallestFactor = correctionFactor;//if this is zero anywhere, it just means that the euclidean part of the heuristic must be ignored (worst case, it does dijkstra)
            }
            tempf *= correctionFactor;
            tempInfo.pieceDists[1] *= correctionFactor;
        }//for now, assume it only depends on the expansion of the endpoints, and affects each part equally
        tempInfo.pieceDists[0] = tempf - tempInfo.pieceDists[1];
        nodeNeighbors2[farNode].push_back(baseNode);//record it at both ends, because we are looping through edges
        distances2[farNode].push_back(tempf);
        neighbors2PathInfo[farNode].push_back(tempInfo);
        
        float tempf2 = tempInfo.pieceDists[0];//swap the piece distances around for the baseNode info
        tempInfo.pieceDists[0] = tempInfo.pieceDists[1];
        tempInfo.pieceDists[1] = tempf2;
        nodeNeighbors2[baseNode].push_back(farNode);
        distances2[baseNode].push_back(tempf);
        neighbors2PathInfo[baseNode].push_back(tempInfo);
    }
}

GeodesicHelper::GeodesicHelper(const CaretPointer<const GeodesicHelperBase>& baseIn)
{
    m_myBase = baseIn;//copy the pointer so it doesn't get changed or deleted while we get its members
    //get references and info from base
    numNodes = m_myBase->numNodes;
    m_avgNodeSpacing = m_myBase->m_avgNodeSpacing;
    m_corrAreaSmallestFactor = m_myBase->m_corrAreaSmallestFactor;
    distances = m_myBase->distances.data();
    distances2 = m_myBase->distances2.data();
    nodeNeighbors = m_myBase->nodeNeighbors.data();
    nodeNeighbors2 = m_myBase->nodeNeighbors2.data();
    nodeCoords = m_myBase->nodeCoords.data();
    neighbors2PathInfo = m_myBase->neighbors2PathInfo.data();
    //allocate private scratch space
    marked.resize(numNodes, 0);//initialize once, each internal function (dijkstra methods) tracks elements changed, and resets only those (except in the case of whole surface)
    m_heapIdent.resize(numNodes);//the idea is to make it faster for the more likely case of small areas of the surface for functions that have limits, by removing the runtime term based solely on surface size
    outputStore.resize(numNodes);
    output = outputStore.data();//because we have a function that does a pointer swap to compute distances directly in the output array
    changed.resize(numNodes);
    parentStore.resize(numNodes);
    parent = parentStore.data();//ditto for parents
    heurVal.resize(numNodes);
}

void GeodesicHelper::getNodesToGeoDist(const int32_t node, const float maxdist, std::vector<int32_t>& nodesOut, std::vector<float>& distsOut, const bool smoothflag)
{//public methods sanity check, private methods process
    nodesOut.clear();
    distsOut.clear();
    CaretAssert(node < numNodes && node >= 0);
    if (node >= numNodes || maxdist < 0.0f || node < 0) return;//check what we asserted so release doesn't do strange things
    CaretMutexLocker locked(&inUse);//let sanity checks go multithreaded, as if it mattered
    dijkstra(node, maxdist, nodesOut, distsOut, smoothflag);
}

void GeodesicHelper::getNodesToGeoDist(const int32_t node, const float maxdist, std::vector<int32_t>& nodesOut, std::vector<float>& distsOut, std::vector<int32_t>& parentsOut, const bool smoothflag)
{//public methods sanity check, private methods process
    nodesOut.clear();
    distsOut.clear();
    CaretAssert(node < numNodes && node >= 0);
    if (node >= numNodes || maxdist < 0.0f || node < 0) return;
    CaretMutexLocker locked(&inUse);//we need the parents array to stay put, so don't scope this
    dijkstra(node, maxdist, nodesOut, distsOut, smoothflag);
    int32_t mysize = (int32_t)nodesOut.size();
    parentsOut.resize(mysize);
    for (int32_t i = 0; i < mysize; ++i)
    {
        parentsOut[i] = parent[nodesOut[i]];
    }
}

void GeodesicHelper::dijkstra(const int32_t root, const float maxdist, std::vector<int32_t>& nodes, std::vector<float>& dists, bool smooth)
{
    int32_t i, j, whichnode, whichneigh, numNeigh, numChanged = 0;
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    marked[root] |= 4;
    parent[root] = -1;//idiom for end of path
    changed[numChanged++] = root;
    m_active.clear();
    m_heapIdent[root] = m_active.push(root, 0.0f);
    //we keep values greater than maxdist off the heap, so anything pulled from the heap which is unmarked belongs in the list
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();
        nodes.push_back(whichnode);
        dists.push_back(output[whichnode]);
        marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4)
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
                if (tempf <= maxdist)
                {//keep it off the heap if it is too far
                    if (!(marked[whichneigh] & 4))
                    {
                        marked[whichneigh] |= 4;
                        changed[numChanged++] = whichneigh;
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                    } else if (tempf < output[whichneigh]) {
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        m_active.changekey(m_heapIdent[whichneigh], tempf);
                    }
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];
                    if (tempf <= maxdist)
                    {//keep it off the heap if it is too far
                        if (!(marked[whichneigh] & 4))
                        {
                            marked[whichneigh] |= 4;
                            changed[numChanged++] = whichneigh;
                            output[whichneigh] = tempf;
                            parent[whichneigh] = whichnode;
                            m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                        } else if (tempf < output[whichneigh]) {
                            output[whichneigh] = tempf;
                            parent[whichneigh] = whichnode;
                            m_active.changekey(m_heapIdent[whichneigh], tempf);
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of arrays
    }
}

void GeodesicHelper::dijkstra(const int32_t root, bool smooth)
{//straightforward dijkstra, no cutoffs, full surface
    int32_t i, j, whichnode, whichneigh, numNeigh;
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    m_heapIdent[root] = m_active.push(root, 0.0f);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();
        marked[whichnode] |= 1;
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];
                if (!(marked[whichneigh] & 4))
                {
                    marked[whichneigh] |= 4;
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                } else if (tempf < output[whichneigh]) {
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    m_active.changekey(m_heapIdent[whichneigh], tempf);
                }
            }
        }
        if (smooth)
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];
                    if (!(marked[whichneigh] & 4))
                    {
                        marked[whichneigh] |= 4;
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                    } else if (tempf < output[whichneigh]) {
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        m_active.changekey(m_heapIdent[whichneigh], tempf);
                    }
                }
            }
        }
    }
    for (i = 0; i < numNodes; ++i)
    {
        marked[i] = 0;
    }
}

void GeodesicHelper::getGeoFromNode(const int32_t node, float* valuesOut, const bool smoothflag)
{
    CaretAssert(node >= 0 && node < numNodes && valuesOut != NULL);
    if (node < 0 || node >= numNodes || !valuesOut)
    {
        return;
    }
    CaretMutexLocker locked(&inUse);//don't screw with member variables while in use
    float* temp = output;//swap out the output pointer to avoid allocation
    output = valuesOut;
    dijkstra(node, smoothflag);
    output = temp;//restore the pointer to the original memory
}

void GeodesicHelper::getGeoFromNode(const int32_t node, float* valuesOut, int32_t* parentsOut, const bool smoothflag)
{
    CaretAssert(node >= 0 && node < numNodes && valuesOut != NULL && parentsOut != NULL);
    if (node < 0 || node >= numNodes || !valuesOut || !parentsOut)
    {
        return;
    }
    CaretMutexLocker locked(&inUse);//don't screw with member variables while in use
    float* temp = output;//swap out the output pointer to avoid allocation
    int32_t* tempi = parent;
    output = valuesOut;
    parent = parentsOut;
    dijkstra(node, smoothflag);
    output = temp;//restore the pointers to the original memory
    parent = tempi;
}

void GeodesicHelper::getGeoFromNode(const int32_t node, std::vector<float>& valuesOut, const bool smoothflag)
{
    CaretAssert(node >= 0 && node < numNodes);
    if (node < 0 || node >= numNodes)
    {
        valuesOut.clear();//empty array is error condition
        return;
    }
    CaretMutexLocker locked(&inUse);
    float* temp = output;//swap the output pointer to avoid copy
    valuesOut.resize(numNodes);
    output = valuesOut.data();
    dijkstra(node, smoothflag);
    output = temp;//restore
}

void GeodesicHelper::getGeoFromNode(const int32_t node, std::vector<float>& valuesOut, std::vector<int32_t>& parentsOut, const bool smoothflag)
{
    CaretAssert(node >= 0 && node < numNodes);
    if (node < 0 || node >= numNodes)
    {
        return;
    }
    CaretMutexLocker locked(&inUse);
    float* temp = output;//swap out the output pointer to avoid copying into the vector afterwards
    int32_t* tempi = parent;
    valuesOut.resize(numNodes);
    parentsOut.resize(numNodes);
    output = valuesOut.data();
    parent = parentsOut.data();
    dijkstra(node, smoothflag);
    output = temp;//restore the pointers to the original memory
    parent = tempi;
}

void GeodesicHelper::dijkstra(const int32_t root, const std::vector<int32_t>& interested, bool smooth)
{
    int32_t i, j, whichnode, whichneigh, numNeigh, numChanged = 0, remain = 0;
    const int32_t* neighbors;
    float tempf;
    j = interested.size();
    for (i = 0; i < j; ++i)
    {
        whichnode = interested[i];
        if (!marked[whichnode])
        {
            ++remain;
            marked[whichnode] = 2;//interested, not expanded, no valid value
            changed[numChanged++] = whichnode;
        }
    }
    output[root] = 0.0f;
    if (!marked[root])
    {
        changed[numChanged++] = root;
    }
    marked[root] |= 4;
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    m_heapIdent[root] = m_active.push(root, 0.0f);
    while (remain && !m_active.isEmpty())
    {
        whichnode = m_active.pop();
        if (marked[whichnode] & 2)
        {
            --remain;
        }
        marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4), so already in changed list
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
                if (!(marked[whichneigh] & 4))
                {
                    if (!marked[whichneigh])
                    {
                        changed[numChanged++] = whichneigh;
                    }
                    marked[whichneigh] |= 4;
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                } else if (tempf < output[whichneigh]) {
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    m_active.changekey(m_heapIdent[whichneigh], tempf);
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];
                    if (!(marked[whichneigh] & 4))
                    {
                        if (!marked[whichneigh])
                        {
                            changed[numChanged++] = whichneigh;
                        }
                        marked[whichneigh] |= 4;
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                    } else if (tempf < output[whichneigh]) {
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        m_active.changekey(m_heapIdent[whichneigh], tempf);
                    }
                }
            }
        }
    }
    for (i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of arrays
    }
}

int32_t GeodesicHelper::dijkstra(const vector<int32_t>& startList, const vector<int32_t>& endList, const float& maxDist, bool smooth)
{
    int32_t i, j, whichnode, whichneigh, numNeigh, numChanged = 0, ret = -1;
    const int32_t* neighbors;
    float tempf;
    m_active.clear();
    j = (int32_t)startList.size();
    for (i = 0; i < j; ++i)
    {
        if (marked[startList[i]] == 0)
        {
            output[startList[i]] = 0.0f;
            changed[numChanged++] = startList[i];
            marked[startList[i]] = 4;//has valid value
            parent[startList[i]] = -1;//idiom for end of path
            m_heapIdent[startList[i]] = m_active.push(startList[i], 0.0f);
        }
    }
    j = (int32_t)endList.size();
    for (i = 0; i < j; ++i)
    {
        if (marked[endList[i]] == 0)
        {
            changed[numChanged++] = endList[i];
            marked[endList[i]] = 8;//stopping point
        }
    }
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();
        if ((marked[whichnode] & 8) != 0)//we have found the closest node in the endList, we are done
        {
            ret = whichnode;
            break;
        }
        marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4), so already in changed list
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];
                if (tempf <= maxDist)
                {
                    if (!(marked[whichneigh] & 4))
                    {
                        parent[whichneigh] = whichnode;
                        if (!marked[whichneigh])
                        {
                            changed[numChanged++] = whichneigh;
                        }
                        marked[whichneigh] |= 4;
                        output[whichneigh] = tempf;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                    } else if (tempf < output[whichneigh]) {
                        m_active.changekey(m_heapIdent[whichneigh], tempf);
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                    }
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];
                    if (tempf <= maxDist)
                    {
                        if (!(marked[whichneigh] & 4))
                        {
                            parent[whichneigh] = whichnode;
                            if (!marked[whichneigh])
                            {
                                changed[numChanged++] = whichneigh;
                            }
                            marked[whichneigh] |= 4;
                            output[whichneigh] = tempf;
                            m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                        } else if (tempf < output[whichneigh]) {
                            m_active.changekey(m_heapIdent[whichneigh], tempf);
                            output[whichneigh] = tempf;
                            parent[whichneigh] = whichnode;
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of arrays
    }
    return ret;
}

int32_t GeodesicHelper::closest(const int32_t& root, const char* roi, const float& maxdist, float& distOut, bool smooth)
{
    int32_t i, j, whichnode, whichneigh, numNeigh, numChanged = 0, ret = -1;
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    changed[numChanged++] = root;
    marked[root] |= 4;
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    m_heapIdent[root] = m_active.push(root, 0.0f);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();
        if (roi[whichnode] != 0)//we have found the closest node in the roi to the root, we are done
        {
            distOut = output[whichnode];
            ret = whichnode;
            break;
        }
        marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4), so already in changed list
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
                if (tempf <= maxdist)
                {
                    if (!(marked[whichneigh] & 4))
                    {
                        parent[whichneigh] = whichnode;
                        if (!marked[whichneigh])
                        {
                            changed[numChanged++] = whichneigh;
                        }
                        marked[whichneigh] |= 4;
                        output[whichneigh] = tempf;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                    } else if (tempf < output[whichneigh]) {
                        m_active.changekey(m_heapIdent[whichneigh], tempf);
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                    }
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
                    if (tempf <= maxdist)
                    {
                        if (!(marked[whichneigh] & 4))
                        {
                            parent[whichneigh] = whichnode;
                            if (!marked[whichneigh])
                            {
                                changed[numChanged++] = whichneigh;
                            }
                            marked[whichneigh] |= 4;
                            output[whichneigh] = tempf;
                            m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                        } else if (tempf < output[whichneigh]) {
                            m_active.changekey(m_heapIdent[whichneigh], tempf);
                            output[whichneigh] = tempf;
                            parent[whichneigh] = whichnode;
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of arrays
    }
    return ret;
}

int32_t GeodesicHelper::closest(const int32_t& root, const char* roi, bool smooth)
{
    int32_t i, j, whichnode, whichneigh, numNeigh, numChanged = 0, ret = -1;
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    changed[numChanged++] = root;
    marked[root] |= 4;
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    m_heapIdent[root] = m_active.push(root, 0.0f);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();
        if (roi[whichnode] != 0)//we have found the closest node in the roi to the root, we are done
        {
            ret = whichnode;
            break;
        }
        marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4), so already in changed list
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
                if (!(marked[whichneigh] & 4))
                {
                    parent[whichneigh] = whichnode;
                    if (!marked[whichneigh])
                    {
                        changed[numChanged++] = whichneigh;
                    }
                    marked[whichneigh] |= 4;
                    output[whichneigh] = tempf;
                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                } else if (tempf < output[whichneigh]) {
                    m_active.changekey(m_heapIdent[whichneigh], tempf);
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
                    if (!(marked[whichneigh] & 4))
                    {
                        parent[whichneigh] = whichnode;
                        if (!marked[whichneigh])
                        {
                            changed[numChanged++] = whichneigh;
                        }
                        marked[whichneigh] |= 4;
                        output[whichneigh] = tempf;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                    } else if (tempf < output[whichneigh]) {
                        m_active.changekey(m_heapIdent[whichneigh], tempf);
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                    }
                }
            }
        }
    }
    for (i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of arrays
    }
    return ret;
}

void GeodesicHelper::aStar(const int32_t root, const int32_t endpoint, bool smooth)
{
    int32_t whichnode, whichneigh, numNeigh, numChanged = 0;
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    changed[numChanged++] = root;
    marked[root] |= 4;//has value in output
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    float remainEucl = (nodeCoords[root] - nodeCoords[endpoint]).length();
    heurVal[root] = remainEucl * m_corrAreaSmallestFactor;
    m_heapIdent[root] = m_active.push(root, heurVal[root]);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();//we use a modifiable heap, so we don't need to check for duplicates
        marked[whichnode] |= 1;//frozen - will already be in changed list, due to being in heap
        if (whichnode == endpoint) break;
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];
                if (!(marked[whichneigh] & 4))
                {
                    heurVal[whichneigh] = m_corrAreaSmallestFactor * (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    changed[numChanged++] = whichneigh;//having a valid value will be the first marking, so set changed
                    marked[whichneigh] |= 4;
                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf + heurVal[whichneigh]);
                } else if (tempf < output[whichneigh]) {
                    m_active.changekey(m_heapIdent[whichneigh], tempf + heurVal[whichneigh]);
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];
                    if (!(marked[whichneigh] & 4))
                    {
                        heurVal[whichneigh] = m_corrAreaSmallestFactor * (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        changed[numChanged++] = whichneigh;//having a valid value will be the first marking, so set changed
                        marked[whichneigh] |= 4;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf + heurVal[whichneigh]);
                    } else if (tempf < output[whichneigh]) {
                        m_active.changekey(m_heapIdent[whichneigh], tempf + heurVal[whichneigh]);
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                    }
                }
            }
        }
    }
    for (int32_t i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of marked array
    }
}

float GeodesicHelper::linePenalty(const Vector3D& pos, const Vector3D& linep1, const Vector3D& linep2, const bool& segment)
{
    if (segment)
    {
        return pos.distToLineSegment(linep1, linep2);
    } else {
        return pos.distToLine(linep1, linep2);
    }
}

float GeodesicHelper::lineHeuristic(const Vector3D& pos, const Vector3D& linep1, const Vector3D& linep2, const float& remainEucl, const bool& segment)
{
    float tempf;//to make it consistent, assume it minimizes the line penalty by taking a straight line towards the line (segment), encountering the endpoint en route, or on the line (segment)
    if (segment)//the distance heuristic assumes a different path to make it consistent versus path length, and the sum of consistent heuristics is consistent for the sum of the penalties
    {
        tempf = m_corrAreaSmallestFactor * pos.distToLineSegment(linep1, linep2);//all euclidean distances must be modified by the smallest area correction factor
    } else {
        tempf = m_corrAreaSmallestFactor * pos.distToLine(linep1, linep2);
    }
    if (remainEucl < tempf)
    {
        return m_corrAreaSmallestFactor * remainEucl * (2.0f * tempf - remainEucl);//hits the endpoint en route
    } else {
        return tempf * tempf;//reaches the line (segment), then finds the endpoint with no additional penalty
    }
}

void GeodesicHelper::aStarLine(const int32_t& root, const int32_t& endpoint, const Vector3D& linep1, const Vector3D& linep2, const bool& segment)
{
    int32_t whichnode, whichneigh, numNeigh, numChanged = 0;
    float penaltyScale = 0.5f / m_avgNodeSpacing;//to prevent change in scale from changing the optimal path - 0.5f is ostensibly for averaging between endpoints, but is largely arbitrary
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    changed[numChanged++] = root;
    marked[root] |= 4;//has value in output
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    float remainEucl = (nodeCoords[root] - nodeCoords[endpoint]).length();
    heurVal[root] = m_corrAreaSmallestFactor * remainEucl + penaltyScale * lineHeuristic(nodeCoords[root], linep1, linep2, remainEucl, segment);
    m_heapIdent[root] = m_active.push(root, heurVal[root]);//to get consistent heuristic, assume it can take straight line to endpoint for total distance, and straight line to target line (segment) at the same time
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();//we use a modifiable heap, so we don't need to check for duplicates
        marked[whichnode] |= 1;//frozen - will already be in changed list, due to being in heap
        if (whichnode == endpoint) break;
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j] + penaltyScale * distances[whichnode][j] * (linePenalty(nodeCoords[whichnode], linep1, linep2, segment) + linePenalty(nodeCoords[whichneigh], linep1, linep2, segment));
                if (!(marked[whichneigh] & 4))
                {
                    remainEucl = (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
                    heurVal[whichneigh] = m_corrAreaSmallestFactor * remainEucl + penaltyScale * lineHeuristic(nodeCoords[whichneigh], linep1, linep2, remainEucl, segment);
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    changed[numChanged++] = whichneigh;//having a valid value will be the first marking, so set changed
                    marked[whichneigh] |= 4;
                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf + heurVal[whichneigh]);
                } else if (tempf < output[whichneigh]) {
                    m_active.changekey(m_heapIdent[whichneigh], tempf + heurVal[whichneigh]);
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                }
            }
        }
    }
    for (int32_t i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of marked array
    }
}

void GeodesicHelper::aStarData(const int32_t& root, const int32_t& endpoint, const float* data, const float& followStrength, const float* roiData, const bool& smooth)
{//NOTE: for consistent behavior, data must not contain negatives (or anything non-numeric)
    int32_t whichnode, whichneigh, numNeigh, numChanged = 0;
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    changed[numChanged++] = root;
    marked[root] |= 4;//has value in output
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    heurVal[root] = m_corrAreaSmallestFactor * (nodeCoords[root] - nodeCoords[endpoint]).length();//data could be zero all the way along the optimal path, so euclidean distance is the only consistent heuristic
    m_heapIdent[root] = m_active.push(root, heurVal[root]);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();//we use a modifiable heap, so we don't need to check for duplicates
        marked[whichnode] |= 1;//frozen - will already be in changed list, due to being in heap
        if (whichnode == endpoint) break;
        neighbors = nodeNeighbors[whichnode].data();
        numNeigh = (int32_t)nodeNeighbors[whichnode].size();
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if ((roiData == NULL || roiData[whichneigh] > 0.0f) && !(marked[whichneigh] & 1))
            {//skip floating point math if frozen or outside roi
                tempf = output[whichnode] + distances[whichnode][j] * (1.0f + followStrength * (data[whichnode] + data[whichneigh]));//integrate 1 + strength * value to get distance plus path-integrated data
                if (!(marked[whichneigh] & 4))
                {
                    heurVal[whichneigh] = m_corrAreaSmallestFactor * (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                    changed[numChanged++] = whichneigh;//having a valid value will be the first marking, so set changed
                    marked[whichneigh] |= 4;
                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf + heurVal[whichneigh]);
                } else if (tempf < output[whichneigh]) {
                    m_active.changekey(m_heapIdent[whichneigh], tempf + heurVal[whichneigh]);
                    output[whichneigh] = tempf;
                    parent[whichneigh] = whichnode;
                }
            }
        }
        if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
        {
            neighbors = nodeNeighbors2[whichnode].data();
            numNeigh = (int32_t)nodeNeighbors2[whichnode].size();
            const GeodesicHelperBase::CrawlInfo* pathInfo = neighbors2PathInfo[whichnode].data();
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if ((roiData == NULL || roiData[whichneigh] > 0.0f) && !(marked[whichneigh] & 1))
                {//skip floating point math if frozen or outside roi
                    tempf = output[whichnode] + distances2[whichnode][j] + followStrength * (data[whichnode] * pathInfo[j].pieceDists[0] + data[whichneigh] * pathInfo[j].pieceDists[1]
                                + distances2[whichnode][j] * (data[pathInfo[j].edgeNodes[0]] * pathInfo[j].edgeWeight + data[pathInfo[j].edgeNodes[1]] * (1.0f - pathInfo[j].edgeWeight)));
                    if (!(marked[whichneigh] & 4))
                    {
                        heurVal[whichneigh] = m_corrAreaSmallestFactor * (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                        changed[numChanged++] = whichneigh;//having a valid value will be the first marking, so set changed
                        marked[whichneigh] |= 4;
                        m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf + heurVal[whichneigh]);
                    } else if (tempf < output[whichneigh]) {
                        m_active.changekey(m_heapIdent[whichneigh], tempf + heurVal[whichneigh]);
                        output[whichneigh] = tempf;
                        parent[whichneigh] = whichnode;
                    }
                }
            }
        }
    }
    for (int32_t i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of marked array
    }
}

void GeodesicHelper::getGeoToTheseNodes(const int32_t root, const std::vector<int32_t>& ofInterest, std::vector<float>& distsOut, bool smoothflag)
{
    CaretAssert(root >= 0 && root < numNodes);
    if (root < 0 || root >= numNodes)
    {
        distsOut.clear();//empty array is error condition
        return;
    }
    int32_t i, mysize = ofInterest.size(), node;
    for (i = 0; i < mysize; ++i)
    {//needs to do a linear scan of this array later anyway, so lets sanity check it
        node = ofInterest[i];
        if (node < 0 || node >= numNodes)
        {
            distsOut.clear();//empty array is error condition
            return;
        }
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    dijkstra(root, ofInterest, smoothflag);
    distsOut.resize(mysize);
    for (i = 0; i < mysize; ++i)
    {
        distsOut[i] = output[ofInterest[i]];
    }
}

void GeodesicHelper::getPathToNode(const int32_t root, const int32_t endpoint, vector<int32_t>& pathNodesOut, vector<float>& pathDistsOut, bool smoothflag)
{
    CaretAssert(root >= 0 && root < numNodes && endpoint >= 0 && endpoint < numNodes);
    pathNodesOut.clear();
    pathDistsOut.clear();
    if (root < 0 || root >= numNodes || endpoint < 0 || endpoint >= numNodes)
    {
        return;
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    parent[endpoint] = -2;//sentinel value that DOESN'T mean end of path
    aStar(root, endpoint, smoothflag);
    if (parent[endpoint] == -2)//check for invalid value
    {
        return;
    }
    vector<int32_t> tempReverse;
    int32_t next = endpoint;
    while (next != root)
    {
        tempReverse.push_back(next);
        next = parent[next];
    }
    tempReverse.push_back(next);
    int32_t tempSize = (int32_t)tempReverse.size();
    for (int32_t i = tempSize - 1; i >= 0; --i)
    {
        int32_t tempNode = tempReverse[i];
        pathNodesOut.push_back(tempNode);
        pathDistsOut.push_back(output[tempNode]);
    }
}

void GeodesicHelper::getPathBetweenNodeLists(const vector<int32_t>& startList, const vector<int32_t>& endList, const float& maxDist, vector<int32_t>& pathNodesOut, vector<float>& pathDistsOut, bool smoothflag)
{
    pathNodesOut.clear();
    pathDistsOut.clear();
    for (size_t i = 0; i < startList.size(); ++i)
    {
        if (startList[i] < 0 || startList[i] >= numNodes) return;
    }
    for (size_t i = 0; i < endList.size(); ++i)
    {
        if (endList[i] < 0 || endList[i] >= numNodes) return;
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    int32_t pathEnd = dijkstra(startList, endList, maxDist, smoothflag);//not sure if A* with a PointLocator would be any faster, so don't add a dependency
    if (pathEnd == -1) return;//no path found
    vector<int32_t> tempReverse;
    int32_t next = pathEnd;
    while (next != -1)//-1 is "parent" of start node
    {
        tempReverse.push_back(next);
        next = parent[next];
    }
    tempReverse.push_back(next);
    int32_t tempSize = (int32_t)tempReverse.size();
    for (int32_t i = tempSize - 1; i >= 0; --i)
    {
        int32_t tempNode = tempReverse[i];
        pathNodesOut.push_back(tempNode);
        pathDistsOut.push_back(output[tempNode]);
    }
}

void GeodesicHelper::getPathAlongLine(const int32_t root, const int32_t endpoint, const Vector3D& linep1, const Vector3D& linep2, vector<int32_t>& pathNodesOut, vector<float>& pathDistsOut)
{
    CaretAssert(root >= 0 && root < numNodes && endpoint >= 0 && endpoint < numNodes);
    pathNodesOut.clear();
    pathDistsOut.clear();
    if (root < 0 || root >= numNodes || endpoint < 0 || endpoint >= numNodes)
    {
        return;
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    parent[endpoint] = -2;//sentinel value that DOESN'T mean end of path
    aStarLine(root, endpoint, linep1, linep2, false);
    if (parent[endpoint] == -2)//check for invalid value
    {
        return;
    }
    vector<int32_t> tempReverse;
    int32_t next = endpoint;
    while (next != root)
    {
        tempReverse.push_back(next);
        next = parent[next];
    }
    tempReverse.push_back(next);
    int32_t tempSize = (int32_t)tempReverse.size();
    for (int32_t i = tempSize - 1; i >= 0; --i)
    {
        int32_t tempNode = tempReverse[i];
        pathNodesOut.push_back(tempNode);
        pathDistsOut.push_back(output[tempNode]);
    }
}

void GeodesicHelper::getPathAlongLineSegment(const int32_t root, const int32_t endpoint, const Vector3D& linep1, const Vector3D& linep2, vector<int32_t>& pathNodesOut, vector<float>& pathDistsOut)
{
    CaretAssert(root >= 0 && root < numNodes && endpoint >= 0 && endpoint < numNodes);
    pathNodesOut.clear();
    pathDistsOut.clear();
    if (root < 0 || root >= numNodes || endpoint < 0 || endpoint >= numNodes)
    {
        return;
    }
    vector<int32_t> ofInterest(1, endpoint);
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    parent[endpoint] = -2;//sentinel value that DOESN'T mean end of path
    aStarLine(root, endpoint, linep1, linep2, true);
    if (parent[endpoint] == -2)//check for invalid value
    {
        return;
    }
    vector<int32_t> tempReverse;
    int32_t next = endpoint;
    while (next != root)
    {
        tempReverse.push_back(next);
        next = parent[next];
    }
    tempReverse.push_back(next);
    int32_t tempSize = (int32_t)tempReverse.size();
    for (int32_t i = tempSize - 1; i >= 0; --i)
    {
        int32_t tempNode = tempReverse[i];
        pathNodesOut.push_back(tempNode);
        pathDistsOut.push_back(output[tempNode]);
    }
}

void GeodesicHelper::getPathFollowingData(const int32_t root, const int32_t endpoint, const float* data, vector<int32_t>& pathNodesOut, vector<float>& pathDistsOut,
                                          const float& followStrength, const float* roiData, const bool& followMaximum, const bool& smoothFlag)
{
    CaretAssert(root >= 0 && root < numNodes && endpoint >= 0 && endpoint < numNodes);
    pathNodesOut.clear();
    pathDistsOut.clear();
    if (root < 0 || root >= numNodes || endpoint < 0 || endpoint >= numNodes)
    {
        return;
    }
    if (roiData != NULL && !(roiData[root] > 0.0f && roiData[endpoint] > 0.0f))
    {
        return;
    }
    vector<float> rescaledData(numNodes);
    float minimum = 0.0f, maximum = 0.0f;
    bool first = true;
    for (int i = 0; i < numNodes; ++i)//first normalize contrast in the roi, so we are not sensitive to the absolute values of the data
    {
        if (roiData == NULL || roiData[i] > 0.0f)
        {
            if (first)
            {
                first = false;
                minimum = data[i];
                maximum = data[i];
            } else {
                if (data[i] < minimum) minimum = data[i];
                if (data[i] > maximum) maximum = data[i];
            }
        }
    }
    if (minimum == maximum)//no contrast, will do simple shortest path
    {
        maximum = minimum + 1.0f;//so map it all to minimum value to prevent divide by 0
    }//doesn't matter what value it is mapped to if there is no contrast, rescaled data is integrated along path length, and path length is always a factor
    float range = maximum - minimum;
    for (int i = 0; i < numNodes; ++i)
    {
        if (roiData == NULL || roiData[i] > 0.0f)
        {
            if (followMaximum)//also deal with follow minimum vs maximum
            {
                rescaledData[i] = (maximum - data[i]) / range;
            } else {
                rescaledData[i] = (data[i] - minimum) / range;
            }
        }
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    parent[endpoint] = -2;//sentinel value that DOESN'T mean end of path
    aStarData(root, endpoint, rescaledData.data(), followStrength, roiData, smoothFlag);
    if (parent[endpoint] == -2)//check for invalid value
    {
        return;
    }
    vector<int32_t> tempReverse;
    int32_t next = endpoint;
    while (next != root)
    {
        tempReverse.push_back(next);
        next = parent[next];
    }
    tempReverse.push_back(next);
    int32_t tempSize = (int32_t)tempReverse.size();
    for (int32_t i = tempSize - 1; i >= 0; --i)
    {
        int32_t tempNode = tempReverse[i];
        pathNodesOut.push_back(tempNode);
        pathDistsOut.push_back(output[tempNode]);
    }
}

int32_t GeodesicHelper::getClosestNodeInRoi(const int32_t& root, const char* roi, const float& maxdist, float& distOut, bool smoothflag)
{
    CaretAssert(root >= 0 && root < numNodes && maxdist >= 0.0f);
    if (root < 0 || root >= numNodes || maxdist < 0.0f)
    {
        return -1;
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    return closest(root, roi, maxdist, distOut, smoothflag);
}

int32_t GeodesicHelper::getClosestNodeInRoi(const int32_t& root, const char* roi, vector<int32_t>& pathNodesOut, vector<float>& pathDistsOut, bool smoothflag)
{
    CaretAssert(root >= 0 && root < numNodes);
    pathNodesOut.clear();
    pathDistsOut.clear();
    if (root < 0 || root >= numNodes)
    {
        return -1;
    }
    CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
    int32_t ret = closest(root, roi, smoothflag);
    if (ret == -1) return ret;
    vector<int32_t> tempReverse;
    int32_t next = ret;
    while (next != root)
    {
        tempReverse.push_back(next);
        next = parent[next];
    }
    tempReverse.push_back(next);
    int32_t tempSize = (int32_t)tempReverse.size();
    for (int32_t i = tempSize - 1; i >= 0; --i)
    {
        int32_t tempNode = tempReverse[i];
        pathNodesOut.push_back(tempNode);
        pathDistsOut.push_back(output[tempNode]);
    }
    return ret;
}
