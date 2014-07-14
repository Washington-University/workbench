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
#include <DescriptiveStatistics.h>
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include <iostream>
#include <stdint.h>

using namespace caret;
using namespace std;

GeodesicHelperBase::GeodesicHelperBase(const SurfaceFile* surfaceIn)
{
    CaretPointer<TopologyHelperBase> topoBase(new TopologyHelperBase(surfaceIn));
    TopologyHelper topoHelpIn(topoBase);//leave this building one privately, to not introduce even worse dependencies regarding SurfaceFile
    numNodes = surfaceIn->getNumberOfNodes();
    DescriptiveStatistics nodeSpacingStats;
    surfaceIn->getNodesSpacingStatistics(nodeSpacingStats);
    m_avgNodeSpacing = nodeSpacingStats.getMean();
    //allocate
    numNeighbors = new int32_t[numNodes];
    nodeNeighbors = new int32_t*[numNodes];
    distances = new float*[numNodes];
    nodeCoords.resize(numNodes);
    //const float* coords = surfaceIn->getCoordinate(0);//hack previously needed for old code, before it used edgeInfo
    float d[3], g[3], ac[3], abhat[3], abmag, ad[3], efhat[3], efmag, ea[3], cdmag, eg[3], eh[3], ah[3], tempvec[3], tempf;
    for (int32_t i = 0; i < numNodes; ++i)
    {//get neighbors
        const int32_t* neighbors = topoHelpIn.getNodeNeighbors(i, numNeighbors[i]);
        nodeCoords[i] = surfaceIn->getCoordinate(i);
        const float* baseCoord = nodeCoords[i];
        nodeNeighbors[i] = new int32_t[numNeighbors[i]];
        distances[i] = new float[numNeighbors[i]];
        for (int32_t j = 0; j < numNeighbors[i]; ++j)
        {
            nodeNeighbors[i][j] = neighbors[j];
            const float* neighCoord = surfaceIn->getCoordinate(neighbors[j]);
            coordDiff(baseCoord, neighCoord, tempvec);
            distances[i][j] = std::sqrt(tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2]);//precompute for speed in other calls
        }//so few floating point operations, this should turn out symmetric
    }
    std::vector<int32_t> tempneigh2;
    std::vector<float> tempdist2;
    nodeNeighbors2 = new int32_t*[numNodes];
    numNeighbors2 = new int32_t[numNodes];
    distances2 = new float*[numNodes];
    //begin edge info based code
    vector<vector<int32_t> > nodeNeighbors2Vec;
    vector<vector<float> > distances2Vec;
    nodeNeighbors2Vec.resize(numNodes);
    distances2Vec.resize(numNodes);
    const vector<TopologyEdgeInfo>& myEdgeInfo = topoHelpIn.getEdgeInfo();
    int numEdges = myEdgeInfo.size();
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
        const float* neigh1Coord = nodeCoords[neigh1Node];
        const float* neigh2Coord = nodeCoords[neigh2Node];
        const float* baseCoord = nodeCoords[baseNode];
        const float* farCoord = nodeCoords[farNode];
        const int32_t num_reserve = 8;//uses 8 in case it is used on a mesh with haphazard topology, these vectors go away when the constructor terminates anyway
        nodeNeighbors2Vec[baseNode].reserve(num_reserve);//reserve should be fast if capacity is already num_reserve, and better than reallocating at 2 and 4, if vector allocation is naive doubling
        nodeNeighbors2Vec[farNode].reserve(num_reserve);//in the extremely rare case of a node with more than num_reserve neighbors, a second allocation plus copy isn't much of a cost
        distances2Vec[baseNode].reserve(num_reserve);
        distances2Vec[farNode].reserve(num_reserve);
        coordDiff(neigh2Coord, neigh1Coord, abhat);//a is neigh1, b is neigh2, b - a = (vector)ab
        abmag = normalize(abhat);
        coordDiff(farCoord, neigh1Coord, ac);//c is farnode, c - a = (vector)ac
        tempf = dotProd(abhat, ac);
        ad[0] = abhat[0] * tempf;//d is the point on the shared edge that farnode (c) is closest to
        ad[1] = abhat[1] * tempf;//this way we can "unfold" the triangles by projecting the distance of cd, from point d, along the unit vector of the base node to closest point on shared edge,
        ad[2] = abhat[2] * tempf;
        d[0] = neigh1Coord[0] + ad[0];//and now we have the point d
        d[1] = neigh1Coord[1] + ad[1];
        d[2] = neigh1Coord[2] + ad[2];
        coordDiff(neigh1Coord, baseCoord, ea);//e is the base node, a - e = (vector)ea
        tempf = dotProd(abhat, ea);//find the component of ea perpendicular to the shared edge
        tempvec[0] = abhat[0] * tempf;//find vector fa, f being the point on shared edge closest to e, the base node
        tempvec[1] = abhat[1] * tempf;
        tempvec[2] = abhat[2] * tempf;
        efhat[0] = ea[0] - tempvec[0];//and subtract it to obtain only the perpendicular
        efhat[1] = ea[1] - tempvec[1];
        efhat[2] = ea[2] - tempvec[2];
        efmag = normalize(efhat);//normalize to get unit vector
        coordDiff(&d[0], farCoord, tempvec);//this is vector cd, perpendicular to shared edge, from shared edge to far point
        cdmag = normalize(tempvec);//get its magnitude
        tempvec[0] = efhat[0] * cdmag;//vector dg, from shared edge at closest point to c (far node), to the unfolded position of farnode (g)
        tempvec[1] = efhat[1] * cdmag;
        tempvec[2] = efhat[2] * cdmag;
        g[0] = d[0] + tempvec[0];//add vector dg to point d to get point g, the unfolded position of farnode
        g[1] = d[1] + tempvec[1];
        g[2] = d[2] + tempvec[2];
        coordDiff(&g[0], baseCoord, eg);//this is the vector from base (e) to far node after unfolding (g), this is our distance, as long as the tetralateral is convex
        tempf = efmag / (efmag + cdmag);//now we need to check that the path stays inside the tetralateral (ie, that it is convex)
        eh[0] = eg[0] * tempf;//this is a vector from e (base node) to the point on the shared edge that the full path (eg) crosses
        eh[1] = eg[1] * tempf;//this is because the lengths of the two perpendiculars to the root and far nodes from the shared edge establishes proportionality
        eh[2] = eg[2] * tempf;
        ah[0] = eh[0] - ea[0];//eh - ea = eh + ae = ae + eh = ah, vector from neigh to the point on shared edge the path goes through
        ah[1] = eh[1] - ea[1];
        ah[2] = eh[2] - ea[2];
        tempf = dotProd(ah, abhat);//get the component along ab so we can test that it is positive and less than |ab|
        if (tempf <= 0.0f || tempf >= abmag) continue;//tetralateral is concave or triangular (degenerate), our path is invalid or not shorter, so consider next edge
        tempf = normalize(eg);//this is our path length
        nodeNeighbors2Vec[farNode].push_back(baseNode);
        distances2Vec[farNode].push_back(tempf);
        nodeNeighbors2Vec[baseNode].push_back(farNode);
        distances2Vec[baseNode].push_back(tempf);
    }
    for (int i = 0; i < numNodes; ++i)
    {//copy it from vector into straight dynamic array, because now it won't change again, to use a bit less memory
        numNeighbors2[i] = nodeNeighbors2Vec[i].size();
        nodeNeighbors2[i] = new int32_t[numNeighbors2[i]];
        distances2[i] = new float[numNeighbors2[i]];
        for (int32_t j = 0; j < numNeighbors2[i]; ++j)
        {
            nodeNeighbors2[i][j] = nodeNeighbors2Vec[i][j];
            distances2[i][j] = distances2Vec[i][j];
        }
    }
}

GeodesicHelper::GeodesicHelper(const CaretPointer<const GeodesicHelperBase>& baseIn)
{
    m_myBase = baseIn;//copy the pointer so it doesn't get changed or deleted while we get its members
    //get references and info from base
    numNodes = m_myBase->numNodes;
    distances = m_myBase->distances;
    distances2 = m_myBase->distances2;
    numNeighbors = m_myBase->numNeighbors;
    numNeighbors2 = m_myBase->numNeighbors2;
    nodeNeighbors = m_myBase->nodeNeighbors;
    nodeNeighbors2 = m_myBase->nodeNeighbors2;
    nodeCoords = m_myBase->nodeCoords.data();
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
        if (!(marked[whichnode] & 1))
        {
            nodes.push_back(whichnode);
            dists.push_back(output[whichnode]);
            marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4)
            neighbors = nodeNeighbors[whichnode];
            numNeigh = numNeighbors[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if marked
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
                neighbors = nodeNeighbors2[whichnode];
                numNeigh = numNeighbors2[whichnode];
                for (j = 0; j < numNeigh; ++j)
                {
                    whichneigh = neighbors[j];
                    if (!(marked[whichneigh] & 1))
                    {//skip floating point math if marked
                        tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
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
        if (!(marked[whichnode] & 1))
        {
            marked[whichnode] |= 1;
            neighbors = nodeNeighbors[whichnode];
            numNeigh = numNeighbors[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if marked
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
                neighbors = nodeNeighbors2[whichnode];
                numNeigh = numNeighbors2[whichnode];
                for (j = 0; j < numNeigh; ++j)
                {
                    whichneigh = neighbors[j];
                    if (!(marked[whichneigh] & 1))
                    {//skip floating point math if marked
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
    }
    for (i = 0; i < numNodes; ++i)
    {
        marked[i] = 0;
    }
}

float** GeodesicHelper::getGeoAllToAll(const bool smooth)
{
    long long bytes = (((long long)numNodes) * numNodes * (sizeof(float) + sizeof(int32_t)) + numNodes * (sizeof(float*) + sizeof(int32_t*))) * 100;//fixed point
    short index = 0;
    static const char *labels[9] = {" Bytes", " Kilobytes", " Megabytes", " Gigabytes", " Terabytes", " Petabytes", " Exabytes", " Zettabytes", " Yottabytes"};
    while (index < 8 && bytes > 80000)
    {//add 2 zeroes, thats 800.00
        ++index;
        bytes = bytes >> 10;
    }
    CaretMutexLocker locked(&inUse);//don't sit there with memory allocated but locked out of computation, lock early - also before status messages
    std::cout << "attempting to allocate " << bytes / 100 << "." << bytes % 100 << labels[index] << "...";
    std::cout.flush();
    int32_t i = -1, j;
    bool fail = false;
    float** ret = NULL;
    int32_t** parents = NULL;
    try
    {
        ret = new float*[numNodes];
        if (ret != NULL)
        {
            for (i = 0; i < numNodes; ++i)
            {
                ret[i] = new float[numNodes];
                if (ret[i] == NULL)
                {
                    fail = true;
                    break;//have to break so it doesn't increment i
                }
            }
        }
    } catch (std::bad_alloc e) {//should catch if new likes to throw exceptions instead of returning null
        fail = true;
    }
    if (ret == NULL)
    {
        std::cout << "failed" << std::endl;
        return NULL;
    }
    if (fail)
    {
        std::cout << "failed" << std::endl;
        for (j = 0; j < i; ++j) delete[] ret[j];
        if (i > -1) delete[] ret;
        return NULL;
    }
    i = -1;
    try
    {
        parents = new int32_t*[numNodes];
        if (parents != NULL)
        {
            for (i = 0; i < numNodes; ++i)
            {
                parents[i] = new int32_t[numNodes];
                if (parents[i] == NULL)
                {
                    fail = true;
                    break;//have to break so it doesn't increment i
                }
            }
        }
    } catch (std::bad_alloc e) {//should catch if new likes to throw exceptions instead of returning null
        fail = true;
    }
    if (parents == NULL)
    {
        std::cout << "failed" << std::endl;
        for (i = 0; i < numNodes; ++i) delete[] ret[i];
        delete[] ret;
        return NULL;
    }
    if (fail)
    {
        std::cout << "failed" << std::endl;
        for (j = 0; j < i; ++j) delete[] parents[j];
        if (i > -1) delete[] parents;
        for (i = 0; i < numNodes; ++i) delete[] ret[i];
        delete[] ret;
        return NULL;
    }
    std::cout << "success" << std::endl;
    alltoall(ret, parents, smooth);
    for (i = 0; i < numNodes; ++i) delete[] parents[i];
    delete[] parents;
    return ret;
}

void GeodesicHelper::alltoall(float** out, int32_t** parents, bool smooth)
{//propagates info about shortest paths not containing root to other roots, hopefully making the problem tractable
    int32_t root, i, j, whichnode, whichneigh, numNeigh, remain, midpoint, midrevparent, endparent, prevdots = 0, dots;
    const int32_t* neighbors;
    float tempf, tempf2;
    for (i = 0; i < numNodes; ++i)
    {
        for (j = 0; j < numNodes; ++j)
        {
            out[i][j] = -1.0f;
        }
    }
    remain = numNodes;
    std::cout << "|0%      calculating geodesic distances      100%|" << std::endl;
    //            ..................................................
    for (root = 0; root < numNodes; ++root)
    {
        //std::cout << root << std::endl;
        dots = (50 * root) / numNodes;//simple progress indicator
        while (prevdots < dots)
        {
            std::cout << '.';
            std::cout.flush();
            ++prevdots;
        }
        if (root != 0)
        {
            remain = 0;
            for (i = 0; i < numNodes; ++i)//find known values
            {
                if (out[root][i] > 0.0f)
                {
                    marked[i] = 2;//mark that we already have a value, skip calculation, but not yet added to active list
                } else {
                    marked[i] = 0;
                    ++remain;//count how many more we need to compute so we can stop early
                }
            }
        }//marking done, dijkstra time
        m_active.clear();
        out[root][root] = 0.0f;
        parents[root][root] = -1;//idiom for end of path
        m_heapIdent[root] = m_active.push(root, 0.0f);
        while (remain && !m_active.isEmpty())
        {
            whichnode = m_active.pop();
            if (!(marked[whichnode] & 1))
            {
                if (!(marked[whichnode] & 2)) --remain;
                marked[whichnode] |= 1;
                neighbors = nodeNeighbors[whichnode];
                numNeigh = numNeighbors[whichnode];
                for (j = 0; j < numNeigh; ++j)
                {
                    whichneigh = neighbors[j];
                    if (marked[whichneigh] & 2)
                    {//already has a value and parent for this root
                        if (!(marked[whichneigh] & 8))
                        {//not yet in active list
                            m_heapIdent[whichneigh] = m_active.push(whichneigh, out[root][whichneigh]);
                            marked[whichneigh] |= 8;
                        }
                    } else {
                        if (!(marked[whichneigh] & 1))
                        {//skip floating point math if marked
                            tempf = out[root][whichnode] + distances[whichnode][j];
                            if (!(marked[whichneigh] & 4))
                            {
                                out[root][whichneigh] = tempf;
                                parents[root][whichneigh] = whichnode;
                                marked[whichneigh] |= 4;
                                m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                            } else if (tempf < out[root][whichneigh]) {
                                out[root][whichneigh] = tempf;
                                parents[root][whichneigh] = whichnode;
                                m_active.changekey(m_heapIdent[whichneigh], tempf);
                            }
                        }
                    }
                }
                if (smooth)
                {
                    neighbors = nodeNeighbors2[whichnode];
                    numNeigh = numNeighbors2[whichnode];
                    for (j = 0; j < numNeigh; ++j)
                    {
                        whichneigh = neighbors[j];
                        if (marked[whichneigh] & 2)
                        {//already has a value and parent for this root
                            if (!(marked[whichneigh] & 8))
                            {//not yet in active list
                                m_heapIdent[whichneigh] = m_active.push(whichneigh, out[root][whichneigh]);
                                marked[whichneigh] |= 8;
                            }
                        } else {
                            if (!(marked[whichneigh] & 1))
                            {//skip floating point math if marked
                                tempf = out[root][whichnode] + distances2[whichnode][j];
                                if (!(marked[whichneigh] & 4))
                                {
                                    out[root][whichneigh] = tempf;
                                    parents[root][whichneigh] = whichnode;
                                    marked[whichneigh] |= 4;
                                    m_heapIdent[whichneigh] = m_active.push(whichneigh, tempf);
                                } else if (tempf < out[root][whichneigh]) {
                                    out[root][whichneigh] = tempf;
                                    parents[root][whichneigh] = whichnode;
                                    m_active.changekey(m_heapIdent[whichneigh], tempf);
                                }
                            }
                        }
                    }
                }
            }
        }//dijkstra done...lotsa brackets...now to propagate the information gained to other roots
        for (i = root + 1; i < numNodes; ++i)
        {//any node smaller than root already has all distances calculated for entire surface
            if (!(marked[i] & 2))
            {//if endpoint already had distance to root precomputed, all available info from this node has been propagated previously
                midrevparent = i;
                midpoint = parents[root][i];
                endparent = midpoint;
                tempf = out[root][i];
                while (midpoint != root)
                {
                    tempf2 = tempf - out[root][midpoint];
                    if (midpoint > root)
                    {//try to be swap-friendly by not setting values in finished columns
                        out[midpoint][i] = tempf2;//use midpoint as root, parent of endpoint is endparent
                        parents[midpoint][i] = endparent;
                    }
                    out[i][midpoint] = tempf2;//use endpoint as root (so, reverse the path), parent of midpoint is midrevparent
                    parents[i][midpoint] = midrevparent;
                    midrevparent = midpoint;//step along path
                    midpoint = parents[root][midpoint];
                }
                out[i][root] = out[root][i];//finally, fill the transpose
                parents[i][root] = midrevparent;
            }
        }//propagation of best paths to other roots complete, dijkstra again
    }
    while (prevdots < 50)
    {
        std::cout << '.';
        ++prevdots;
    }
    std::cout << std::endl;
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
        if (!(marked[whichnode] & 1))
        {
            if (marked[whichnode] & 2)
            {
                --remain;
            }
            marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4), so already in changed list
            neighbors = nodeNeighbors[whichnode];
            numNeigh = numNeighbors[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if marked
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
                neighbors = nodeNeighbors2[whichnode];
                numNeigh = numNeighbors2[whichnode];
                for (j = 0; j < numNeigh; ++j)
                {
                    whichneigh = neighbors[j];
                    if (!(marked[whichneigh] & 1))
                    {//skip floating point math if marked
                        tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
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
    }
    for (i = 0; i < numChanged; ++i)
    {
        marked[changed[i]] = 0;//minimize reinitialization of arrays
    }
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
        if (!(marked[whichnode] & 1))//this test actually shouldn't be needed, due to heap with modifiable keys...
        {
            if (roi[whichnode] != 0)//we have found the closest node in the roi to the root, we are done
            {
                distOut = output[whichnode];
                ret = whichnode;
                break;
            }
            marked[whichnode] |= 1;//anything pulled from heap will already be marked as having a valid value (flag 4), so already in changed list
            neighbors = nodeNeighbors[whichnode];
            numNeigh = numNeighbors[whichnode];
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
                            output[whichneigh] = tempf;
                            m_active.changekey(m_heapIdent[whichneigh], tempf);
                        }
                    }
                }
            }
            if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
            {
                neighbors = nodeNeighbors2[whichnode];
                numNeigh = numNeighbors2[whichnode];
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
                                output[whichneigh] = tempf;
                                m_active.changekey(m_heapIdent[whichneigh], tempf);
                            }
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
    heurVal[root] = remainEucl;
    m_heapIdent[root] = m_active.push(root, heurVal[root]);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();//we use a modifiable heap, so we don't need to check for duplicates
        marked[whichnode] |= 1;//frozen - will already be in changed list, due to being in heap
        if (whichnode == endpoint) break;
        neighbors = nodeNeighbors[whichnode];
        numNeigh = numNeighbors[whichnode];
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j];
                if (!(marked[whichneigh] & 4))
                {
                    heurVal[whichneigh] = (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
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
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                whichneigh = neighbors[j];
                if (!(marked[whichneigh] & 1))
                {//skip floating point math if frozen
                    tempf = output[whichnode] + distances2[whichnode][j];
                    if (!(marked[whichneigh] & 4))
                    {
                        heurVal[whichneigh] = (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
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

void GeodesicHelper::aStarLine(const int32_t root, const int32_t endpoint, const Vector3D& linep1, const Vector3D& linep2, const bool& segment)
{
    int32_t whichnode, whichneigh, numNeigh, numChanged = 0;
    float penaltyScale = 0.5f / m_myBase->m_avgNodeSpacing;//to prevent change in scale from changing the optimal path - 0.5f is ostensibly for averaging between endpoints, but is largely arbitrary
    const int32_t* neighbors;
    float tempf;
    output[root] = 0.0f;
    changed[numChanged++] = root;
    marked[root] |= 4;//has value in output
    parent[root] = -1;//idiom for end of path
    m_active.clear();
    float remainEucl = (nodeCoords[root] - nodeCoords[endpoint]).length();
    heurVal[root] = remainEucl + remainEucl * penaltyScale * linePenalty(nodeCoords[root], linep1, linep2, segment);
    m_heapIdent[root] = m_active.push(root, heurVal[root]);
    while (!m_active.isEmpty())
    {
        whichnode = m_active.pop();//we use a modifiable heap, so we don't need to check for duplicates
        marked[whichnode] |= 1;//frozen - will already be in changed list, due to being in heap
        if (whichnode == endpoint) break;
        neighbors = nodeNeighbors[whichnode];
        numNeigh = numNeighbors[whichnode];
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if frozen
                tempf = output[whichnode] + distances[whichnode][j] + penaltyScale * distances[whichnode][j] * (linePenalty(nodeCoords[whichnode], linep1, linep2, segment) + linePenalty(nodeCoords[whichneigh], linep1, linep2, segment));
                if (!(marked[whichneigh] & 4))
                {
                    remainEucl = (nodeCoords[whichneigh] - nodeCoords[endpoint]).length();
                    heurVal[whichneigh] = remainEucl + remainEucl * penaltyScale * linePenalty(nodeCoords[whichneigh], linep1, linep2, segment);
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
