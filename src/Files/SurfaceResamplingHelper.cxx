/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*LICENSE_END*/

#include "SurfaceResamplingHelper.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "FastStatistics.h"
#include "GeodesicHelper.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"

#include <algorithm>
#include <set>
#include <map>

using namespace std;
using namespace caret;

SurfaceResamplingHelper::SurfaceResamplingHelper(const SurfaceResamplingMethodEnum::Enum& myMethod, const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                                 const float* currentAreas, const float* newAreas, const float* currentRoi, const bool allowNonSphere)
{
    m_nonsphereAllowed = allowNonSphere;
    SurfaceFile currentSphereMod, newSphereMod;
    const SurfaceFile* useCurrent = currentSphere, *useNew = newSphere;
    if (!allowNonSphere)
    {
        if (!checkSphere(currentSphere) || !checkSphere(newSphere)) throw CaretException("input surfaces to SurfaceResamplingHelper must be spheres");
        changeRadius(100.0f, currentSphere, &currentSphereMod);
        changeRadius(100.0f, newSphere, &newSphereMod);
        useCurrent = &currentSphereMod;
        useNew = &newSphereMod;
    }
    //TODO: warning if nonsphere allowed and distance between surfaces is large at some point?
    //if warning was enabled always, then a highly distorted sphere could trip it, so maybe it would be a good idea anyway, but with a different message
    switch (myMethod)
    {
        case SurfaceResamplingMethodEnum::ADAP_BARY_AREA:
            CaretAssert(currentAreas != NULL && newAreas != NULL);
            if (currentAreas == NULL || newAreas == NULL) throw CaretException("ADAP_BARY_AREA method requires providing vertex areas using anatomical surfaces or vertex area metrics");
            computeWeightsAdapBaryArea(useCurrent, useNew, currentAreas, newAreas, currentRoi);
            break;
        case SurfaceResamplingMethodEnum::BARYCENTRIC:
            computeWeightsBarycentric(useCurrent, useNew, currentRoi);
            break;
    }
}

void SurfaceResamplingHelper::resampleNormal(const float* input, float* output, const float& invalidVal) const
{
    int numNodes = (int)m_weights.size() - 1;
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes; ++i)
    {
        WeightElem* end = m_weights[i + 1], *elem = m_weights[i];
        if (elem != end)
        {
            double accum = 0.0;
            for (; elem != end; ++elem)
            {
                accum += input[elem->node] * elem->weight;//don't need to divide afterwards, because the weights already sum to 1
            }
            output[i] = accum;
        } else {
            output[i] = invalidVal;
        }
    }
}

void SurfaceResamplingHelper::resample3DCoord(const float* input, float* output) const
{
    int numNodes = (int)m_weights.size() - 1;
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes; ++i)
    {
        double tempvec[3] = { 0.0, 0.0, 0.0 };
        WeightElem* end = m_weights[i + 1];
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            const float* coord = input + elem->node * 3;
            tempvec[0] += coord[0] * elem->weight;//don't need to divide afterwards, because the weights already sum to 1
            tempvec[1] += coord[1] * elem->weight;
            tempvec[2] += coord[2] * elem->weight;
        }
        int i3 = i * 3;
        output[i3] = tempvec[0];
        output[i3 + 1] = tempvec[1];
        output[i3 + 2] = tempvec[2];
    }
}

void SurfaceResamplingHelper::resamplePopular(const int32_t* input, int32_t* output, const int32_t& invalidVal) const
{
    int numNodes = (int)m_weights.size() - 1;
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes; ++i)
    {
        map<int32_t, float> accum;
        float maxweight = -1.0f;
        int32_t bestlabel = invalidVal;
        WeightElem* end = m_weights[i + 1];
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            int32_t label = input[elem->node];
            map<int, float>::iterator iter = accum.find(label);
            if (iter == accum.end())
            {
                accum[label] = elem->weight;
                if (elem->weight > maxweight)
                {
                    maxweight = elem->weight;
                    bestlabel = label;
                }
            } else {
                iter->second += elem->weight;
                if (iter->second > maxweight)
                {
                    maxweight = iter->second;
                    bestlabel = label;
                }
            }
        }
        output[i] = bestlabel;
    }
}

void SurfaceResamplingHelper::resampleLargest(const float* input, float* output, const float& invalidVal) const
{
    int numNodes = (int)m_weights.size() - 1;
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes; ++i)
    {
        WeightElem* end = m_weights[i + 1];
        float largest = -1.0f;
        int largestNode = -1;
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            if (elem->weight > largest)
            {
                largest = elem->weight;
                largestNode = elem->node;
            }
        }
        if (largestNode != -1)
        {
            output[i] = input[largestNode];
        } else {
            output[i] = invalidVal;
        }
    }
}

void SurfaceResamplingHelper::resampleLargest(const int32_t* input, int32_t* output, const int32_t& invalidVal) const
{
    int numNodes = (int)m_weights.size() - 1;
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes; ++i)
    {
        WeightElem* end = m_weights[i + 1];
        float largest = -1.0f;
        int largestNode = -1;
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            if (elem->weight > largest)
            {
                largest = elem->weight;
                largestNode = elem->node;
            }
        }
        if (largestNode != -1)
        {
            output[i] = input[largestNode];
        } else {
            output[i] = invalidVal;
        }
    }
}

void SurfaceResamplingHelper::getResampleValidROI(float* output) const
{
    int numNodes = (int)m_weights.size() - 1;
    for (int i = 0; i < numNodes; ++i)
    {
        if (m_weights[i] != m_weights[i + 1])
        {
            output[i] = 1.0f;
        } else {
            output[i] = 0.0f;
        }
    }
}

void SurfaceResamplingHelper::resampleCutSurface(const SurfaceFile* cutSurfaceIn, const SurfaceFile* currentSphere, const SurfaceFile* newSphere, SurfaceFile* surfaceOut)
{
    if (cutSurfaceIn->getNumberOfNodes() != currentSphere->getNumberOfNodes()) throw CaretException("input surface has different number of nodes than input sphere");
    if (!checkSphere(currentSphere) || !checkSphere(newSphere)) throw CaretException("input surfaces to SurfaceResamplingHelper must be spheres");
    SurfaceFile currentSphereMod, newSphereMod;
    changeRadius(100.0f, currentSphere, &currentSphereMod);
    changeRadius(100.0f, newSphere, &newSphereMod);
    SurfaceFile cutCurSphere = *cutSurfaceIn;
    cutCurSphere.setCoordinates(currentSphereMod.getCoordinateData());
    int newNodes = newSphere->getNumberOfNodes();
    vector<BarycentricInfo> newInfo(newSphere->getNumberOfNodes());
#pragma omp CARET_PAR
    {
        CaretPointer<SignedDistanceHelper> mySignedHelp = cutCurSphere.getSignedDistanceHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < newNodes; ++i)
        {
            mySignedHelp->barycentricWeights(newSphereMod.getCoordinate(i), newInfo[i]);
        }
    }
    vector<int> isOnEdge(newNodes, 0);//really used as bool, but avoid bitpacking so it can be modified in parallel
    CaretPointer<TopologyHelper> cutTopoHelp = cutSurfaceIn->getTopologyHelper();//because topology didn't change, and it might have one already - also, don't need separate helpers per thread, not using neighbors to depth
    CaretPointer<TopologyHelper> closedTopoHelp = currentSphere->getTopologyHelper();//ditto
    CaretPointer<TopologyHelper> newTopoHelp = newSphere->getTopologyHelper();//tritto?
    const vector<TopologyEdgeInfo>& cutEdgeInfo = cutTopoHelp->getEdgeInfo();
    vector<int> largestNode(newNodes, -1);
#pragma omp CARET_PARFOR
    for (int i = 0; i < newNodes; ++i)
    {
        float largestWeight = 0.0f, secondWeight = 0.0f;//locate the two nodes with largest barycentric weights
        largestNode[i] = -1;
        int secondNode = -1;
        for (int j = 0; j < 3; ++j)
        {
            if (newInfo[i].baryWeights[j] > largestWeight)
            {
                secondNode = largestNode[i];//shift largest to second
                secondWeight = largestWeight;
                largestWeight = newInfo[i].baryWeights[j];//update largest
                largestNode[i] = newInfo[i].nodes[j];
            } else if (newInfo[i].baryWeights[j] > secondWeight) {
                secondWeight = newInfo[i].baryWeights[j];
                secondNode = newInfo[i].nodes[j];
            }
        }
        switch (newInfo[i].type)
        {
            case BarycentricInfo::NODE:
                if (cutTopoHelp->getNodeTiles(largestNode[i]).size() != closedTopoHelp->getNodeTiles(largestNode[i]).size())
                {
                    isOnEdge[i] = 1;
                }
                break;
            case BarycentricInfo::EDGE:
            {
                const vector<int32_t>& cutEdges = cutTopoHelp->getNodeEdges(largestNode[i]);
                for (int j = 0; j < (int)cutEdges.size(); ++j)
                {
                    const TopologyEdgeInfo& myInfo = cutEdgeInfo[cutEdges[j]];
                    if (myInfo.node1 == largestNode[i])
                    {
                        if (myInfo.node2 == secondNode)
                        {
                            if (myInfo.numTiles == 1)//NOTE: assumes 1 tile means that it is an edge of a cut, could compare to closed instead, but need to search it separately
                            {
                                isOnEdge[i] = 1;
                            }
                            break;
                        }
                    } else {
                        if (myInfo.node1 == secondNode && myInfo.node2 == largestNode[i])
                        {
                            if (myInfo.numTiles == 1)//ditto
                            {
                                isOnEdge[i] = 1;
                            }
                            break;
                        }
                    }
                }
                break;
            }
            case BarycentricInfo::TRIANGLE://is never on a cut edge, do nothing
                break;
        }
    }
    int numNewTris = newSphere->getNumberOfTriangles();
    vector<int> triRemove(numNewTris, 0), nodeDisconnect(newNodes, 0);//again, avoid bitpacking
#pragma omp CARET_PAR
    {
        CaretPointer<GeodesicHelper> closedGeoHelp = currentSphereMod.getGeodesicHelper();
        CaretPointer<GeodesicHelper> cutGeoHelp = cutCurSphere.getGeodesicHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < newNodes; ++i)
        {
            const vector<int32_t>& neighbors = newTopoHelp->getNodeNeighbors(i);
            if (isOnEdge[i])
            {
                bool hasInteriorNeighbor = false;
                for (int j = 0; j < (int)neighbors.size(); ++j)
                {
                    if (!isOnEdge[neighbors[j]])
                    {
                        hasInteriorNeighbor = true;
                        break;
                    }
                }
                if (hasInteriorNeighbor)
                {
                    for (int j = 0; j < (int)neighbors.size(); ++j)
                    {
                        vector<int32_t> closedPath, cutPath;
                        vector<float> closedPathDists, cutPathDists;
                        closedGeoHelp->getPathToNode(largestNode[i], largestNode[neighbors[j]], closedPath, closedPathDists);
                        cutGeoHelp->getPathToNode(largestNode[i], largestNode[neighbors[j]], cutPath, cutPathDists);
                        if (cutPathDists.size() == 0 || cutPathDists.back() > 2.0f * closedPathDists.back())//maybe this cutoff should be tunable
                        {
                            const vector<int32_t>& myTiles = newTopoHelp->getNodeTiles(i);//find tiles on new mesh that share this edge, remove them
                            for (int k = 0; k < (int)myTiles.size(); ++k)
                            {
                                const int32_t* thisTile = newSphere->getTriangle(myTiles[k]);
                                if (thisTile[0] == neighbors[j] || thisTile[1] == neighbors[j] || thisTile[2] == neighbors[j])
                                {
                                    triRemove[myTiles[k]] = 1;
                                }
                            }
                        }
                    }
                } else {
                    nodeDisconnect[i] = 1;//disconnect it completely if it has no interior neighbors
                    const vector<int32_t>& nodeTiles = newTopoHelp->getNodeTiles(i);
                    for (int j = 0; j < (int)nodeTiles.size(); ++j)
                    {
                        triRemove[nodeTiles[j]] = 1;
                    }
                }
            } else {//interior nodes also have to be checked for crossing the cut, since there may not be a node that falls inside the cut
                for (int j = 0; j < (int)neighbors.size(); ++j)
                {
                    vector<int32_t> closedPath, cutPath;
                    vector<float> closedPathDists, cutPathDists;
                    closedGeoHelp->getPathToNode(largestNode[i], largestNode[neighbors[j]], closedPath, closedPathDists);
                    cutGeoHelp->getPathToNode(largestNode[i], largestNode[neighbors[j]], cutPath, cutPathDists);//note: path length of zero means no connection
                    if (cutPathDists.size() == 0 || cutPathDists.back() > 2.0f * closedPathDists.back())//maybe this cutoff should be tunable
                    {
                        const vector<int32_t>& myTiles = newTopoHelp->getNodeTiles(i);//find tiles on new mesh that share this edge, remove them
                        for (int k = 0; k < (int)myTiles.size(); ++k)
                        {
                            const int32_t* thisTile = newSphere->getTriangle(myTiles[k]);
                            if (thisTile[0] == neighbors[j] || thisTile[1] == neighbors[j] || thisTile[2] == neighbors[j])
                            {
                                triRemove[myTiles[k]] = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    int triRemoveCount = 0;
    for (int i = 0; i < numNewTris; ++i)
    {
        if (triRemove[i] != 0) ++triRemoveCount;//parallelizing counting would be silly
    }
    surfaceOut->setNumberOfNodesAndTriangles(newNodes, numNewTris - triRemoveCount);
    surfaceOut->setStructure(cutSurfaceIn->getStructure());
    surfaceOut->setSurfaceType(cutSurfaceIn->getSurfaceType());
    surfaceOut->setSecondaryType(cutSurfaceIn->getSecondaryType());
    int outTri = 0;
    for (int i = 0; i < numNewTris; ++i)
    {
        if (triRemove[i] == 0)
        {
            surfaceOut->setTriangle(outTri, newSphere->getTriangle(i));
            ++outTri;//means this can't be parallel, but it would also be silly
        }
    }
    CaretAssert(outTri == numNewTris - triRemoveCount);
    Vector3D origin(0.0f, 0.0f, 0.0f);//where we move disconnected nodes to
    for (int i = 0; i < newNodes; ++i)//could be parallel, but probably not needed
    {
        if (nodeDisconnect[i] != 0)
        {
            surfaceOut->setCoordinate(i, origin);
        } else {
            Vector3D coord1 = cutSurfaceIn->getCoordinate(newInfo[i].nodes[0]);
            Vector3D coord2 = cutSurfaceIn->getCoordinate(newInfo[i].nodes[1]);
            Vector3D coord3 = cutSurfaceIn->getCoordinate(newInfo[i].nodes[2]);
            Vector3D outCoord = coord1 * newInfo[i].baryWeights[0] + coord2 * newInfo[i].baryWeights[1] + coord3 * newInfo[i].baryWeights[2];
            surfaceOut->setCoordinate(i, outCoord);
        }
    }
}

void SurfaceResamplingHelper::computeWeightsAdapBaryArea(const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                                         const float* currentAreas, const float* newAreas, const float* currentRoi)
{
    vector<map<int, float> > forward, reverse, reverse_gather;
    makeBarycentricWeights(currentSphere, newSphere, forward, NULL);//don't use an roi until after we have done area correction, because area correction MUST ignore ROI
    makeBarycentricWeights(newSphere, currentSphere, reverse, NULL);
    int numNewNodes = (int)forward.size(), numOldNodes = currentSphere->getNumberOfNodes();
    reverse_gather.resize(numNewNodes);
    for (int oldNode = 0; oldNode < numOldNodes; ++oldNode)//this loop can't be parallelized
    {
        for (map<int, float>::iterator iter = reverse[oldNode].begin(); iter != reverse[oldNode].end(); ++iter)//convert scattering weights to gathering weights
        {
            reverse_gather[iter->first][oldNode] = iter->second;
        }
    }
    vector<map<int, float> > adap_gather(numNewNodes);
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int newNode = 0; newNode < numNewNodes; ++newNode)
    {
        set<int> forwardused;//build set of all nodes used by forward weights (really only 3, but this is a bit cleaner and more generic)
        for (map<int, float>::iterator iter = forward[newNode].begin(); iter != forward[newNode].end(); ++iter)
        {
            forwardused.insert(iter->first);
        }
        bool useforward = true;
        for (map<int, float>::iterator iter = reverse_gather[newNode].begin(); iter != reverse_gather[newNode].end(); ++iter)
        {
            if (forwardused.find(iter->first) == forwardused.end())
            {
                useforward = false;//if the reverse scatter weights include something the forward gather weights don't, use reverse scatter
                break;
            }
        }
        if (useforward)
        {
            adap_gather[newNode] = forward[newNode];
        } else {
            adap_gather[newNode] = reverse_gather[newNode];
        }
        for (map<int, float>::iterator iter = adap_gather[newNode].begin(); iter != adap_gather[newNode].end(); ++iter)//begin area correction by multiplying by target node area
        {
            iter->second *= newAreas[newNode];//begin the process of area correction by multiplying by gathering node areas
        }
    }
    vector<float> correctionSum(numOldNodes, 0.0f);
    for (int newNode = 0; newNode < numNewNodes; ++newNode)//this loop is separate because it can't be parallelized
    {
        for (map<int, float>::iterator iter = adap_gather[newNode].begin(); iter != adap_gather[newNode].end(); ++iter)
        {
            correctionSum[iter->first] += iter->second;//now, sum the scattering weights to prepare for first normalization
        }
    }
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int newNode = 0; newNode < numNewNodes; ++newNode)
    {
        double weightsum = 0.0f;
        vector<map<int, float>::iterator> toRemove;
        for (map<int, float>::iterator iter = adap_gather[newNode].begin(); iter != adap_gather[newNode].end(); ++iter)
        {
            if (currentRoi == NULL || currentRoi[iter->first] > 0.0f)
            {
                iter->second *= currentAreas[iter->first] / correctionSum[iter->first];//divide the weights by their scatter sum, then multiply by current areas
                weightsum += iter->second;//and compute the sum
            } else {
                toRemove.push_back(iter);
            }
        }
        int numToRemove = (int)toRemove.size();
        for (int i = 0; i < numToRemove; ++i)
        {
            adap_gather[newNode].erase(toRemove[i]);
        }
        if (weightsum != 0.0f)//this shouldn't happen unless no nodes remain due to roi, or node areas can be zero
        {
            for (map<int, float>::iterator iter = adap_gather[newNode].begin(); iter != adap_gather[newNode].end(); ++iter)
            {
                iter->second /= weightsum;//and normalize to a sum of 1
            }
        }
    }
    compactWeights(adap_gather);//and compact them into the internal weight storage
}

void SurfaceResamplingHelper::computeWeightsBarycentric(const SurfaceFile* currentSphere, const SurfaceFile* newSphere, const float* currentRoi)
{
    vector<map<int, float> > forward;
    makeBarycentricWeights(currentSphere, newSphere, forward, currentRoi);//this should ensure they sum to 1, so we are done
    compactWeights(forward);
}

bool SurfaceResamplingHelper::checkSphere(const SurfaceFile* surface)
{
    int numNodes = surface->getNumberOfNodes();
    CaretAssert(numNodes > 1);
    int numNodes3 = numNodes * 3;
    const float* coordData = surface->getCoordinateData();
    float mindist = Vector3D(coordData).length();
    if (mindist != mindist) throw CaretException("found NaN coordinate in an input sphere");
    float maxdist = mindist;
    const float TOLERANCE = 1.001f;
    for (int i = 3; i < numNodes3; i += 3)
    {
        float tempf = Vector3D(coordData + i).length();
        if (tempf != tempf) throw CaretException("found NaN coordinate in an input sphere");
        if (tempf < mindist)
        {
            mindist = tempf;
        }
        if (tempf > maxdist)
        {
            maxdist = tempf;
        }
    }
    return (mindist * TOLERANCE > maxdist);
}

void SurfaceResamplingHelper::changeRadius(const float& radius, const SurfaceFile* input, SurfaceFile* output)
{
    *output = *input;
    int numNodes = output->getNumberOfNodes();
    int numNodes3 = numNodes * 3;
    vector<float> newCoordData(numNodes3);
    const float* oldCoordData = output->getCoordinateData();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes3; i += 3)
    {
        Vector3D tempvec1 = oldCoordData + i, tempvec2;
        tempvec2 = tempvec1 * (radius / tempvec1.length());
        newCoordData[i] = tempvec2[0];
        newCoordData[i + 1] = tempvec2[1];
        newCoordData[i + 2] = tempvec2[2];
    }
    output->setCoordinates(newCoordData.data());
}

void SurfaceResamplingHelper::compactWeights(const vector<map<int, float> >& weights)
{
    int compactsize = 0;
    int numNodes = (int)weights.size();
    m_weights = CaretArray<WeightElem*>(numNodes + 1);//include a "one-after" pointer
    for (int i = 0; i < numNodes; ++i)
    {
        compactsize += (int)weights[i].size();
    }
    m_storagechunk = CaretArray<WeightElem>(compactsize);
    int curpos = 0;
    for (int i = 0; i < numNodes; ++i)
    {
        m_weights[i] = m_storagechunk + curpos;
        for (map<int, float>::const_iterator iter = weights[i].begin(); iter != weights[i].end(); ++iter)
        {
            m_storagechunk[curpos] = WeightElem(iter->first, iter->second);
            ++curpos;
        }
    }
    CaretAssert(curpos == compactsize);
    m_weights[numNodes] = m_storagechunk + compactsize;
}

void SurfaceResamplingHelper::makeBarycentricWeights(const SurfaceFile* from, const SurfaceFile* to, vector<map<int, float> >& weights, const float* currentRoi)
{
    int numToNodes = to->getNumberOfNodes();
    weights.resize(numToNodes);
    const float* toCoordData = to->getCoordinateData();
    FastStatistics fromEdgeStatistics, toEdgeStatistics;
    from->getNodesSpacingStatistics(fromEdgeStatistics);
    to->getNodesSpacingStatistics(toEdgeStatistics);
    const float warningDistance = 3.0f * max(fromEdgeStatistics.getMean(), toEdgeStatistics.getMean());
    bool doWarn = false;
    if (currentRoi == NULL)
    {
#pragma omp CARET_PAR
        {
            CaretPointer<SignedDistanceHelper> mySignedHelp = from->getSignedDistanceHelper();
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < numToNodes; ++i)
            {
                BarycentricInfo myInfo;
                mySignedHelp->barycentricWeights(toCoordData + i * 3, myInfo);
                if (myInfo.baryWeights[0] != 0.0f) weights[i][myInfo.nodes[0]] = myInfo.baryWeights[0];
                if (myInfo.baryWeights[1] != 0.0f) weights[i][myInfo.nodes[1]] = myInfo.baryWeights[1];
                if (myInfo.baryWeights[2] != 0.0f) weights[i][myInfo.nodes[2]] = myInfo.baryWeights[2];
                if (myInfo.absDistance > warningDistance) doWarn = true; //shouldn't matter if threads collide writing the same value
            }
        }
    } else {
#pragma omp CARET_PAR
        {
            CaretPointer<SignedDistanceHelper> mySignedHelp = from->getSignedDistanceHelper();
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < numToNodes; ++i)
            {
                BarycentricInfo myInfo;
                float weightsum = 0.0f;//there are only 3 weights, so don't bother with double precision
                mySignedHelp->barycentricWeights(toCoordData + i * 3, myInfo);
                if (myInfo.baryWeights[0] != 0.0f && currentRoi[myInfo.nodes[0]] > 0.0f)
                {
                    weights[i][myInfo.nodes[0]] = myInfo.baryWeights[0];
                    weightsum += myInfo.baryWeights[0];
                }
                if (myInfo.baryWeights[1] != 0.0f && currentRoi[myInfo.nodes[1]] > 0.0f)
                {
                    weights[i][myInfo.nodes[1]] = myInfo.baryWeights[1];
                    weightsum += myInfo.baryWeights[1];
                }
                if (myInfo.baryWeights[2] != 0.0f && currentRoi[myInfo.nodes[2]] > 0.0f)
                {
                    weights[i][myInfo.nodes[2]] = myInfo.baryWeights[2];
                    weightsum += myInfo.baryWeights[2];
                }
                if (weightsum != 0.0f)
                {
                    for (map<int, float>::iterator iter = weights[i].begin(); iter != weights[i].end(); ++iter)
                    {
                        iter->second /= weightsum;
                    }
                    if (myInfo.absDistance > warningDistance) doWarn = true;
                }
            }
        }
    }
    if (doWarn)
    {
        if (m_nonsphereAllowed)
        {
            CaretLogWarning("current and new resampling surfaces do not follow the same contour very closely everywhere (or have extreme distortion somewhere), resampling output may have artifacts.  please check whether you used the appropriate current and new surfaces");
        } else {
            CaretLogWarning("current or new resampling spheres seem to have extremely large distortions, please check them manually");
        }
    }
}
