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

#include "SurfaceResamplingHelper.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

#include <set>
#include <map>

using namespace std;
using namespace caret;

SurfaceResamplingHelper::SurfaceResamplingHelper(const SurfaceResamplingMethodEnum::Enum& myMethod, const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                                 const SurfaceFile* currentAreaSurf, const SurfaceFile* newAreaSurf)
{
    if (!checkSphere(currentSphere) || !checkSphere(newSphere)) throw CaretException("input surfaces to SurfaceResamplingHelper must be spheres");
    SurfaceFile currentSphereMod, newSphereMod;
    changeRadius(100.0f, currentSphere, &currentSphereMod);
    changeRadius(100.0f, newSphere, &newSphereMod);
    if (currentAreaSurf != NULL && currentAreaSurf->getNumberOfNodes() != currentSphere->getNumberOfNodes())
    {
        throw CaretException("area surfaces must have the same number of nodes as the spheres");
    }
    if (newAreaSurf != NULL && newAreaSurf->getNumberOfNodes() != newSphere->getNumberOfNodes())
    {
        throw CaretException("area surfaces must have the same number of nodes as the spheres");
    }
    switch (myMethod)
    {
        case SurfaceResamplingMethodEnum::ADAP_BARY_AREA:
            CaretAssert(currentAreaSurf != NULL && newAreaSurf != NULL);
            if (currentAreaSurf == NULL || newAreaSurf == NULL) throw CaretException("ADAP_BARY_AREA method requires area surfaces");
            computeWeightsAdapBaryArea(&currentSphereMod, &newSphereMod, currentAreaSurf, newAreaSurf);
            break;
        case SurfaceResamplingMethodEnum::BARYCENTRIC:
            computeWeightsBarycentric(&currentSphereMod, &newSphereMod);
            break;
    }
}

void SurfaceResamplingHelper::resampleNormal(const float* input, float* output)
{
    int numNodes = (int)m_weights.size() - 1;
    for (int i = 0; i < numNodes; ++i)
    {
        output[i] = 0.0f;
        WeightElem* end = m_weights[i + 1];
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            output[i] += input[elem->node] * elem->weight;//don't need to divide afterwards, because the weights already sum to 1
        }
    }
}

void SurfaceResamplingHelper::resample3DCoord(const float* input, float* output)
{
    int numNodes = (int)m_weights.size() - 1;
    for (int i = 0; i < numNodes; ++i)
    {
        Vector3D tempvec;
        WeightElem* end = m_weights[i + 1];
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            tempvec += Vector3D(input + elem->node * 3) * elem->weight;//don't need to divide afterwards, because the weights already sum to 1
        }
        int i3 = i * 3;
        output[i3] = tempvec[0];
        output[i3 + 1] = tempvec[1];
        output[i3 + 2] = tempvec[2];
    }
}

void SurfaceResamplingHelper::resamplePopular(const int* input, int* output)
{
    int numNodes = (int)m_weights.size() - 1;
    for (int i = 0; i < numNodes; ++i)
    {
        map<int, float> accum;
        float maxweight = -1.0f;
        int bestlabel = 0;
        WeightElem* end = m_weights[i + 1];
        for (WeightElem* elem = m_weights[i]; elem != end; ++elem)
        {
            int label = input[elem->node];
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

void SurfaceResamplingHelper::computeWeightsAdapBaryArea(const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                                         const SurfaceFile* currentAreaSurf, const SurfaceFile* newAreaSurf)
{
    vector<map<int, float> > forward, reverse, reverse_gather;
    makeBarycentricWeights(currentSphere, newSphere, forward);
    makeBarycentricWeights(newSphere, currentSphere, reverse);
    int numNewNodes = (int)forward.size(), numOldNodes = currentSphere->getNumberOfNodes();
    reverse_gather.resize(numNewNodes);
    for (int oldNode = 0; oldNode < numOldNodes; ++oldNode)//this loop can't be parallelized
    {
        for (map<int, float>::iterator iter = reverse[oldNode].begin(); iter != reverse[oldNode].end(); ++iter)
        {
            map<int, float>::iterator search = reverse_gather[iter->first].find(oldNode);//convert scattering weights to gathering weights
            if (search == reverse_gather[iter->first].end())
            {
                reverse_gather[iter->first][oldNode] = iter->second;
            } else {
                search->second += iter->second;
            }
        }
    }
    vector<map<int, float> > adap_gather(numNewNodes);
    vector<float> currentAreas, newAreas;
    currentAreaSurf->computeNodeAreas(currentAreas);
    newAreaSurf->computeNodeAreas(newAreas);
    for (int newNode = 0; newNode < numNewNodes; ++newNode)//this loop can be parallelized
    {
        set<int> forwardused;//build set of all nodes used by forward weights (really only 3, but this is a bit cleaner and more generic)
        for (map<int, float>::iterator iter = forward[newNode].begin(); iter != forward[newNode].end(); ++iter)
        {
            forwardused.insert(iter->first);
        }
        bool useforward = true;
        for (map<int, float>::iterator iter = reverse_gather[newNode].begin(); iter != reverse_gather[newNode].end(); ++iter)
        {
            if (forwardused.find(iter->first) != forwardused.end())
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
    for (int newNode = 0; newNode < numNewNodes; ++newNode)//this one can
    {
        double weightsum = 0.0f;
        for (map<int, float>::iterator iter = adap_gather[newNode].begin(); iter != adap_gather[newNode].end(); ++iter)
        {
            iter->second *= currentAreas[iter->first] / correctionSum[iter->first];//divide the weights by their scatter sum, then multiply by current areas
            weightsum += iter->second;//and compute the sum
        }
        for (map<int, float>::iterator iter = adap_gather[newNode].begin(); iter != adap_gather[newNode].end(); ++iter)
        {
            iter->second /= weightsum;//and normalize to a sum of 1
        }
    }
    compactWeights(adap_gather);//and compact them into the internal weight storage
}

void SurfaceResamplingHelper::computeWeightsBarycentric(const SurfaceFile* currentSphere, const SurfaceFile* newSphere)
{
    vector<map<int, float> > forward;
    makeBarycentricWeights(currentSphere, newSphere, forward);//barycentric weights should already sum to 1, so we are done
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
    for (int i = 0; i < numNodes3; i += 3)
    {
        Vector3D tempvec = Vector3D(oldCoordData + i).normal() * radius;
        newCoordData[i] = tempvec[0];
        newCoordData[i + 1] = tempvec[1];
        newCoordData[i + 2] = tempvec[2];
    }
    output->setCoordinates(newCoordData.data(), numNodes);
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

void SurfaceResamplingHelper::makeBarycentricWeights(const SurfaceFile* from, const SurfaceFile* to, vector<map<int, float> >& weights)
{
    CaretPointer<SignedDistanceHelper> mySignedHelp = from->getSignedDistanceHelper();
    int numToNodes = to->getNumberOfNodes();
    weights.resize(numToNodes);
    const float* toCoordData = to->getCoordinateData();
    for (int i = 0; i < numToNodes; ++i)
    {
        BarycentricInfo myInfo;
        mySignedHelp->barycentricWeights(toCoordData + i * 3, myInfo);
        if (myInfo.baryWeights[0] != 0.0f) weights[i][myInfo.nodes[0]] = myInfo.baryWeights[0];
        if (myInfo.baryWeights[1] != 0.0f) weights[i][myInfo.nodes[1]] = myInfo.baryWeights[1];
        if (myInfo.baryWeights[2] != 0.0f) weights[i][myInfo.nodes[2]] = myInfo.baryWeights[2];
    }
}
