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

#include "BoundingBox.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"

using namespace std;
using namespace caret;

float SignedDistanceHelper::dist(const float coord[3], WindingLogic myWinding)
{
    CaretMutexLocker locked(&m_mutex);
    CaretSimpleMinHeap<Oct<SignedDistanceHelperBase::TriVector>*, float> myHeap;
    myHeap.push(m_base->m_indexRoot, m_base->m_indexRoot->distToPoint(coord));
    ClosestPointInfo tempInfo, bestInfo;
    float tempf = -1.0f, bestTriDist = -1.0f;
    bool first = true;
    int numChanged = 0;
    while (!myHeap.isEmpty())
    {
        Oct<SignedDistanceHelperBase::TriVector>* curOct = myHeap.pop();
        if (curOct->m_leaf)
        {
            vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
            int numTris = (int)myVecRef.size();
            for (int i = 0; i < numTris; ++i)
            {
                if (m_triMarked[myVecRef[i]] != 1)
                {
                    m_triMarked[myVecRef[i]] = 1;
                    m_triMarkChanged[numChanged++] = myVecRef[i];
                    tempf = unsignedDistToTri(coord, myVecRef[i], tempInfo);
                    if (first || tempf < bestTriDist)
                    {
                        bestInfo = tempInfo;
                        bestTriDist = tempf;
                        first = false;
                    }
                }
            }
        } else {
            for (int ci = 0; ci < 2; ++ci)
            {
                for (int cj = 0; cj < 2; ++cj)
                {
                    for (int ck = 0; ck < 2; ++ck)
                    {
                        tempf = curOct->m_children[ci][cj][ck]->distToPoint(coord);
                        if (first || tempf < bestTriDist)
                        {
                            myHeap.push(curOct->m_children[ci][cj][ck], tempf);
                        }
                    }
                }
            }
        }
    }
    while (numChanged)
    {
        m_triMarked[m_triMarkChanged[--numChanged]] = 0;//need to do this before computeSign
    }
    return bestTriDist * computeSign(coord, bestInfo, myWinding);
}

void SignedDistanceHelper::barycentricWeights(const float coord[3], vector<NodeWeight>& weightsOut)
{
    CaretMutexLocker locked(&m_mutex);
    CaretSimpleMinHeap<Oct<SignedDistanceHelperBase::TriVector>*, float> myHeap;
    myHeap.push(m_base->m_indexRoot, m_base->m_indexRoot->distToPoint(coord));
    ClosestPointInfo tempInfo, bestInfo;
    float tempf = -1.0f, bestTriDist = -1.0f;
    bool first = true;
    int numChanged = 0;
    while (!myHeap.isEmpty())
    {
        Oct<SignedDistanceHelperBase::TriVector>* curOct = myHeap.pop();
        if (curOct->m_leaf)
        {
            vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
            int numTris = (int)myVecRef.size();
            for (int i = 0; i < numTris; ++i)
            {
                if (m_triMarked[myVecRef[i]] != 1)
                {
                    m_triMarked[myVecRef[i]] = 1;
                    m_triMarkChanged[numChanged++] = myVecRef[i];
                    tempf = unsignedDistToTri(coord, myVecRef[i], tempInfo);
                    if (first || tempf < bestTriDist)
                    {
                        bestInfo = tempInfo;
                        bestTriDist = tempf;
                        first = false;
                    }
                }
            }
        } else {
            for (int ci = 0; ci < 2; ++ci)
            {
                for (int cj = 0; cj < 2; ++cj)
                {
                    for (int ck = 0; ck < 2; ++ck)
                    {
                        tempf = curOct->m_children[ci][cj][ck]->distToPoint(coord);
                        if (first || tempf < bestTriDist)
                        {
                            myHeap.push(curOct->m_children[ci][cj][ck], tempf);
                        }
                    }
                }
            }
        }
    }
    while (numChanged)
    {
        m_triMarked[m_triMarkChanged[--numChanged]] = 0;//clean up
    }
    weightsOut.clear();
    switch (bestInfo.type)
    {
        case 2:
            {
                const int32_t* triNodes = m_base->m_surface->getTriangle(bestInfo.triangle);
                Vector3D vert1 = m_base->m_surface->getCoordinate(triNodes[0]);
                Vector3D vert2 = m_base->m_surface->getCoordinate(triNodes[1]);
                Vector3D vert3 = m_base->m_surface->getCoordinate(triNodes[2]);
                Vector3D v21hat = vert2 - vert1;
                float origLength;
                v21hat = v21hat.normal(&origLength);
                float tempf = v21hat.dot(bestInfo.tempPoint - vert1);
                float weight2 = tempf / origLength;
                float weight1 = 1.0f - weight2;
                Vector3D edgepoint = v21hat * tempf + vert1;
                Vector3D v3ehat = vert3 - edgepoint;
                v3ehat = v3ehat.normal(&origLength);
                tempf = v3ehat.dot(bestInfo.tempPoint - edgepoint);
                float weight3 = tempf / origLength;
                tempf = 1.0f - weight3;
                weight1 *= tempf;
                weight2 *= tempf;
                weightsOut.push_back(NodeWeight(triNodes[0], weight1));
                weightsOut.push_back(NodeWeight(triNodes[1], weight2));
                weightsOut.push_back(NodeWeight(triNodes[2], weight3));
            }
            break;
        case 1:
            {
                Vector3D vert1 = m_base->m_surface->getCoordinate(bestInfo.node1);
                Vector3D vert2 = m_base->m_surface->getCoordinate(bestInfo.node2);
                Vector3D v21hat = vert2 - vert1;
                float origLength;
                v21hat = v21hat.normal(&origLength);
                float tempf = v21hat.dot(bestInfo.tempPoint - vert1);
                float weight2 = tempf / origLength;
                float weight1 = 1.0f - weight2;
                weightsOut.push_back(NodeWeight(bestInfo.node1, weight1));
                weightsOut.push_back(NodeWeight(bestInfo.node2, weight2));
            }
            break;
        case 0:
            weightsOut.push_back(NodeWeight(bestInfo.node1, 1.0f));
            break;
    };
}

int SignedDistanceHelper::computeSign(const float coord[3], SignedDistanceHelper::ClosestPointInfo myInfo, WindingLogic myWinding)
{
    Vector3D point = coord;
    Vector3D result = point - myInfo.tempPoint;
    float tempf;
    switch (myWinding)
    {
        case EVEN_ODD:
        case NEGATIVE:
        case NONZERO:
            {
                int numChanged = 0;
                /*const vector<int>& myTiles = m_topoHelp->getNodeTiles(myInfo.node1);
                bool first = true;
                float bestNorm = 0;
                Vector3D tempvec, tempvec2, bestCent;
                for (int i = 0; i < (int)myTiles.size(); ++i)//find the tile of the node with the normal most parallel to the line segment between centroid and point
                {//should be least likely to have an intervening triangle
                    const int32_t* myTileNodes = m_base->m_surface->getTriangle(myTiles[i]);
                    Vector3D vert1 = m_base->m_surface->getCoordinate(myTileNodes[0]);
                    Vector3D vert2 = m_base->m_surface->getCoordinate(myTileNodes[1]);
                    Vector3D vert3 = m_base->m_surface->getCoordinate(myTileNodes[2]);
                    Vector3D centroid = (vert1 + vert2 + vert3) / 3.0f;
                    if (MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, tempvec.m_vec))//make sure the triangle has a valid normal
                    {
                        tempvec2 = point - centroid;
                        tempf = tempvec.dot(tempvec2.normal());
                        if (first || abs(tempf) > abs(bestNorm))
                        {
                            first = false;
                            bestNorm = tempf;
                            bestCent = centroid;
                        }
                    }
                }
                Vector3D mySeg = point - bestCent;//*/
                float positiveZ[3] = {0, 0, 1};
                Vector3D point2 = point + positiveZ;
                int crossCount = 0;
                vector<Oct<SignedDistanceHelperBase::TriVector>*> myStack;
                myStack.push_back(m_base->m_indexRoot);
                while (!myStack.empty())
                {
                    Oct<SignedDistanceHelperBase::TriVector>* curOct = myStack[myStack.size() - 1];
                    myStack.pop_back();
                    if (curOct->m_leaf)
                    {
                        vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
                        int numTris = (int)myVecRef.size();
                        for (int i = 0; i < numTris; ++i)
                        {
                            if (m_triMarked[myVecRef[i]] != 1)
                            {
                                m_triMarked[myVecRef[i]] = 1;
                                m_triMarkChanged[numChanged++] = myVecRef[i];
                                const int32_t* myTileNodes = m_base->m_surface->getTriangle(myVecRef[i]);
                                Vector3D verts[3];
                                verts[0] = m_base->m_surface->getCoordinate(myTileNodes[0]);
                                verts[1] = m_base->m_surface->getCoordinate(myTileNodes[1]);
                                verts[2] = m_base->m_surface->getCoordinate(myTileNodes[2]);
                                Vector3D triNormal;
                                MathFunctions::normalVector(verts[0], verts[1], verts[2], triNormal);
                                float factor = triNormal.dot(positiveZ);
                                if (factor != 0.0f)
                                {
                                    if (triNormal.dot(verts[0] - point) / factor > 0.0f && pointInTri(verts, point, 0, 1))
                                    {
                                        if (triNormal[2] < 0.0f)
                                        {
                                            ++crossCount;
                                        } else {
                                            --crossCount;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        for (int ci = 0; ci < 2; ++ci)
                        {
                            for (int cj = 0; cj < 2; ++cj)
                            {
                                for (int ck = 0; ck < 2; ++ck)
                                {
                                    if (curOct->m_children[ci][cj][ck]->rayIntersects(coord, point2))
                                    {
                                        myStack.push_back(curOct->m_children[ci][cj][ck]);
                                    }
                                }
                            }
                        }
                    }
                }
                while (numChanged)
                {
                    m_triMarked[m_triMarkChanged[--numChanged]] = 0;
                }
                switch (myWinding)
                {
                    case EVEN_ODD:
                        if ((abs(crossCount) & 1) == 1) return -1;//& 1 instead of % 2
                        return 1;
                        break;
                    case NEGATIVE:
                        if (crossCount < 0) return -1;
                        return 1;
                        break;
                    case NONZERO:
                        if (crossCount != 0) return -1;
                        return 1;
                        break;
                    default:
                        return 1;//because compiler can't handle when a switch doesn't accound for an enum value...
                }
            }
            break;
        case NORMALS:
            switch (myInfo.type)
            {
                case 0://node
                    {
                        int curSign = 0;
                        int numChanged = 0;
                        const vector<int>& myTiles = m_topoHelp->getNodeTiles(myInfo.node1);
                        bool first = true;
                        float bestNorm = 0;
                        Vector3D tempvec, tempvec2, bestCent;
                        for (int i = 0; i < (int)myTiles.size(); ++i)//find the tile of the node with the normal most parallel to the line segment between centroid and point
                        {//should be least likely to have an intervening triangle
                            const int32_t* myTileNodes = m_base->m_surface->getTriangle(myTiles[i]);
                            Vector3D vert1 = m_base->m_surface->getCoordinate(myTileNodes[0]);
                            Vector3D vert2 = m_base->m_surface->getCoordinate(myTileNodes[1]);
                            Vector3D vert3 = m_base->m_surface->getCoordinate(myTileNodes[2]);
                            Vector3D centroid = (vert1 + vert2 + vert3) / 3.0f;
                            if (MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, tempvec.m_vec))//make sure the triangle has a valid normal
                            {
                                tempvec2 = point - centroid;
                                tempf = tempvec.dot(tempvec2.normal());
                                if (first || abs(tempf) > abs(bestNorm))
                                {
                                    if (tempf > 0.0f)
                                    {
                                        curSign = 1;
                                    } else {
                                        curSign = -1;
                                    }
                                    first = false;
                                    bestNorm = tempf;
                                    bestCent = centroid;
                                }
                            }
                        }
                        Vector3D mySeg = point - bestCent;
                        float bestDist = mySeg.length();
                        Vector3D segNormal = mySeg.normal();//from the surface to the point, to match the convention of triangles with normals oriented outwards
                        int majAxis = 0, midAxis = 1;//find the axes to use for projecting the triangles, discard the one most aligned with the line segment
                        if (abs(mySeg[1]) < abs(mySeg[0]))
                        {
                            majAxis = 1;
                            midAxis = 0;
                        }
                        if (abs(mySeg[2]) < abs(mySeg[midAxis]))
                        {
                            midAxis = 2;
                        }
                        vector<Oct<SignedDistanceHelperBase::TriVector>*> myStack;
                        myStack.push_back(m_base->m_indexRoot);
                        while (!myStack.empty())
                        {
                            Oct<SignedDistanceHelperBase::TriVector>* curOct = myStack[myStack.size() - 1];
                            myStack.pop_back();
                            if (curOct->m_leaf)
                            {
                                vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
                                int numTris = (int)myVecRef.size();
                                for (int i = 0; i < numTris; ++i)
                                {
                                    if (m_triMarked[myVecRef[i]] != 1)
                                    {
                                        m_triMarked[myVecRef[i]] = 1;
                                        m_triMarkChanged[numChanged++] = myVecRef[i];
                                        const int32_t* myTileNodes = m_base->m_surface->getTriangle(myVecRef[i]);
                                        Vector3D verts[3];
                                        verts[0] = m_base->m_surface->getCoordinate(myTileNodes[0]);
                                        verts[1] = m_base->m_surface->getCoordinate(myTileNodes[1]);
                                        verts[2] = m_base->m_surface->getCoordinate(myTileNodes[2]);
                                        Vector3D triNormal;
                                        MathFunctions::normalVector(verts[0], verts[1], verts[2], triNormal);
                                        float factor = triNormal.dot(segNormal);
                                        if (factor == 0.0f)
                                        {
                                            continue;//skip triangles parallel to the line segment
                                        }
                                        float intersectDist = triNormal.dot(point - verts[0]) / factor;
                                        if (intersectDist > 0.0f && intersectDist < bestDist)
                                        {
                                            Vector3D inPlane = point - intersectDist * segNormal;
                                            if (pointInTri(verts, inPlane, majAxis, midAxis))
                                            {
                                                bestDist = intersectDist;
                                                if (triNormal.dot(mySeg) > 0.0f)
                                                {
                                                    curSign = 1;
                                                } else {
                                                    curSign = -1;
                                                }
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (int ci = 0; ci < 2; ++ci)
                                {
                                    for (int cj = 0; cj < 2; ++cj)
                                    {
                                        for (int ck = 0; ck < 2; ++ck)
                                        {
                                            if (curOct->m_children[ci][cj][ck]->lineSegmentIntersects(coord, bestCent))
                                            {
                                                myStack.push_back(curOct->m_children[ci][cj][ck]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        while (numChanged)
                        {
                            m_triMarked[m_triMarkChanged[--numChanged]] = 0;
                        }
                        return curSign;
                    }
                    break;
                case 1://edge
                    {
                        const vector<TopologyEdgeInfo>& edgeInfo = m_topoHelp->getEdgeInfo();
                        const vector<int>& edges = m_topoHelp->getNodeEdges(myInfo.node1);
                        int whichEdge = -1, numEdges = (int)edges.size();
                        for (int i = 0; i < numEdges; ++i)
                        {
                            if (edgeInfo[edges[i]].node1 == myInfo.node2 || edgeInfo[edges[i]].node2 == myInfo.node2)
                            {
                                whichEdge = edges[i];
                            }
                        }
                        CaretAssert(whichEdge != -1);
                        int tile1 = edgeInfo[whichEdge].tiles[0].tile, tile2 = edgeInfo[whichEdge].tiles[1].tile;
                        Vector3D normalaccum, tempvec;//default constructor initializes it to the zero vector
                        if (tile1 > -1)
                        {
                            const int32_t* tile1nodes = m_base->m_surface->getTriangle(tile1);
                            MathFunctions::normalVector(m_base->m_surface->getCoordinate(tile1nodes[0]),
                                                        m_base->m_surface->getCoordinate(tile1nodes[1]),
                                                        m_base->m_surface->getCoordinate(tile1nodes[2]), tempvec.m_vec);
                            normalaccum += tempvec;
                        }
                        if (tile2 > -1)
                        {
                            const int32_t *tile2nodes = m_base->m_surface->getTriangle(tile2);
                            MathFunctions::normalVector(m_base->m_surface->getCoordinate(tile2nodes[0]),
                                                        m_base->m_surface->getCoordinate(tile2nodes[1]),
                                                        m_base->m_surface->getCoordinate(tile2nodes[2]), tempvec.m_vec);
                            normalaccum += tempvec;
                        }
                        if (normalaccum.dot(result) < 0.0f)
                        {
                            return -1;
                        }
                    }
                    break;
                case 2://face
                    {
                        Vector3D triNormal;
                        const int32_t* triNodes = m_base->m_surface->getTriangle(myInfo.triangle);
                        Vector3D vert1 = m_base->m_surface->getCoordinate(triNodes[0]);
                        Vector3D vert2 = m_base->m_surface->getCoordinate(triNodes[1]);
                        Vector3D vert3 = m_base->m_surface->getCoordinate(triNodes[2]);
                        MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, triNormal.m_vec);
                        if (triNormal.dot(result) < 0.0f)
                        {
                            return -1;
                        }
                    }
                    break;
            }
            break;
    }
    return 1;
}

//Original copyright for PNPOLY, even though my version is entirely rewritten
//Source: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
/**
Copyright (c) 1970-2003, Wm. Randolph Franklin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

1. Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimers.
2. Redistributions in binary form must reproduce the above copyright notice in the documentation
and/or other materials provided with the distribution.
3. The name of W. Randolph Franklin may not be used to endorse or promote products derived from this
Software without specific prior written permission.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

bool SignedDistanceHelper::pointInTri(Vector3D verts[3], Vector3D inPlane, int majAxis, int midAxis)
{
    bool inside = false;
    for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
    {
        if ((verts[i][majAxis] < inPlane[majAxis]) != (verts[j][majAxis] < inPlane[majAxis]))
        {//if one vertex is on one side of the point in the x direction, and the other is on the other side (equal case is treated as greater)
            if ((verts[i][midAxis] - verts[j][midAxis]) / (verts[i][majAxis] - verts[j][majAxis]) * (inPlane[majAxis] - verts[j][majAxis]) + verts[j][midAxis] > inPlane[midAxis])
            {//if the point on the line described by the two vertices with the same x coordinate is above (greater y) than the test point
                inside = !inside;//even/odd winding rule
            }
        }
        j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
    }
    return inside;
}

float SignedDistanceHelper::unsignedDistToTri(const float coord[3], int32_t triangle, ClosestPointInfo& myInfo)
{
    const int32_t* triNodes = m_base->m_surface->getTriangle(triangle);
    Vector3D point = coord, tempPoint;
    Vector3D vert1 = m_base->m_surface->getCoordinate(triNodes[0]);
    Vector3D vert2 = m_base->m_surface->getCoordinate(triNodes[1]);
    Vector3D vert3 = m_base->m_surface->getCoordinate(triNodes[2]);
    Vector3D v21hat = vert2 - vert1;
    float origLength;
    v21hat = v21hat.normal(&origLength);
    int type = 0;//tracks whether it is closest to a node, an edge, or the face
    int32_t node1 = -1, node2 = -1;//tracks which nodes are involved
    float tempf = v21hat.dot(point - vert1);
    if (tempf <= 0.0f)
    {
        tempPoint = vert1;
        node1 = triNodes[0];
    } else if (tempf >= origLength) {
        tempPoint = vert2;
        node1 = triNodes[1];
    } else {
        tempPoint = vert1 + tempf * v21hat;
        node1 = triNodes[0];
        node2 = triNodes[1];
        ++type;
    }
    Vector3D v3that = vert3 - tempPoint;
    v3that = v3that.normal(&origLength);
    tempf = v3that.dot(point - tempPoint);
    if (tempf >= origLength)
    {
        tempPoint = vert3;
        node1 = triNodes[2];
    } else if (tempf >= 0.0f) {
        tempPoint += tempf * v3that;
        node2 = triNodes[2];//don't worry, if this puts it on the face, then it doesn't use node1/node2
        ++type;
    }//if less than 0, keep point the same
    Vector3D result = point - tempPoint;
    myInfo.type = type;
    myInfo.node1 = node1;
    myInfo.node2 = node2;
    myInfo.triangle = triangle;
    myInfo.tempPoint = tempPoint;
    return result.length();
}

SignedDistanceHelper::SignedDistanceHelper(CaretPointer<SignedDistanceHelperBase> myBase)
{
    m_base = myBase;
    m_topoHelp = myBase->m_topoHelp;//because we don't need neighborsToDepth, just share the same one
    int32_t numTris = m_base->m_surface->getNumberOfTriangles();
    m_triMarked = CaretArray<int>(numTris);
    m_triMarkChanged = CaretArray<int>(numTris);
    for (int32_t i = 0; i < numTris; ++i)
    {
        m_triMarked[i] = 0;
    }
}

SignedDistanceHelperBase::SignedDistanceHelperBase(const SurfaceFile* mySurf)
{
    m_surface = mySurf;
    m_topoHelp = mySurf->getTopologyHelper();
    const float* myBB = mySurf->getBoundingBox()->getBounds();
    Vector3D minCoord, maxCoord;
    minCoord[0] = myBB[0]; maxCoord[0] = myBB[1];
    minCoord[1] = myBB[2]; maxCoord[1] = myBB[3];
    minCoord[2] = myBB[4]; maxCoord[2] = myBB[5];
    m_indexRoot = new Oct<TriVector>(minCoord, maxCoord);
    const float* myCoordData = mySurf->getCoordinateData();
    int32_t numTris = mySurf->getNumberOfTriangles();
    for (int32_t i = 0; i < numTris; ++i)
    {
        const int32_t* thisTri = mySurf->getTriangle(i);
        maxCoord = minCoord = myCoordData + thisTri[0] * 3;//set both to the coordinates of the first node in the triangle
        for (int j = 1; j < 3; ++j)
        {
            int32_t thisNode3 = thisTri[j] * 3;
            if (myCoordData[thisNode3] < minCoord[0]) minCoord[0] = myCoordData[thisNode3];
            if (myCoordData[thisNode3 + 1] < minCoord[1]) minCoord[1] = myCoordData[thisNode3 + 1];
            if (myCoordData[thisNode3 + 2] < minCoord[2]) minCoord[2] = myCoordData[thisNode3 + 2];
            if (myCoordData[thisNode3] > maxCoord[0]) maxCoord[0] = myCoordData[thisNode3];
            if (myCoordData[thisNode3 + 1] > maxCoord[1]) maxCoord[1] = myCoordData[thisNode3 + 1];
            if (myCoordData[thisNode3 + 2] > maxCoord[2]) maxCoord[2] = myCoordData[thisNode3 + 2];
        }
        addTriangle(m_indexRoot, i, minCoord, maxCoord);//use bounding box for now as an easy test to capture any chance of the triangle intersecting the Oct
    }
}

void SignedDistanceHelperBase::addTriangle(Oct<TriVector>* thisOct, int32_t triangle, float minCoord[3], float maxCoord[3])
{
    if (thisOct->m_leaf)
    {
        thisOct->m_data.m_triList->push_back(triangle);
        int numTris = (int)thisOct->m_data.m_triList->size();
        if (numTris >= NUM_TRIS_TO_TEST && numTris % NUM_TRIS_TEST_INCR == NUM_TRIS_TO_TEST % NUM_TRIS_TEST_INCR)//the second modulus should const out
        {
            Vector3D tempMinCoord, tempMaxCoord;
            const float* myCoordData = m_surface->getCoordinateData();
            int totalSize = 0;
            int numSplit = 0;
            for (int i = 0; i < numTris; ++i)//gather data on how it would end up splitting
            {
                const int32_t* tempTri = m_surface->getTriangle((*(thisOct->m_data.m_triList))[i]);
                tempMaxCoord = tempMinCoord = myCoordData + tempTri[0] * 3;//set both to the coordinates of the first node in the triangle
                for (int j = 1; j < 3; ++j)
                {
                    int32_t thisNode3 = tempTri[j] * 3;
                    if (myCoordData[thisNode3] < tempMinCoord[0]) tempMinCoord[0] = myCoordData[thisNode3];
                    if (myCoordData[thisNode3 + 1] < tempMinCoord[1]) tempMinCoord[1] = myCoordData[thisNode3 + 1];
                    if (myCoordData[thisNode3 + 2] < tempMinCoord[2]) tempMinCoord[2] = myCoordData[thisNode3 + 2];
                    if (myCoordData[thisNode3] > tempMaxCoord[0]) tempMaxCoord[0] = myCoordData[thisNode3];
                    if (myCoordData[thisNode3 + 1] > tempMaxCoord[1]) tempMaxCoord[1] = myCoordData[thisNode3 + 1];
                    if (myCoordData[thisNode3 + 2] > tempMaxCoord[2]) tempMaxCoord[2] = myCoordData[thisNode3 + 2];
                }
                int minOct[3], maxOct[3];
                thisOct->containingChild(tempMinCoord, minOct);
                thisOct->containingChild(tempMaxCoord, maxOct);
                int splitSize = 8;
                if (minOct[0] == maxOct[0]) splitSize >>= 1;
                if (minOct[1] == maxOct[1]) splitSize >>= 1;
                if (minOct[2] == maxOct[2]) splitSize >>= 1;
                totalSize += splitSize;
                if (splitSize != 8) ++numSplit;
            }
            if (numSplit > 0 && totalSize < 3.0f * numTris)//don't split if all triangles end up in all child octs, and try to balance speedup with memory usage
            {
                thisOct->makeChildren();//do the split
                for (int i = 0; i < numTris; ++i)//gather data on how it would end up splitting
                {
                    const int32_t* tempTri = m_surface->getTriangle((*(thisOct->m_data.m_triList))[i]);
                    tempMaxCoord = tempMinCoord = myCoordData + tempTri[0] * 3;//set both to the coordinates of the first node in the triangle
                    for (int j = 1; j < 3; ++j)
                    {
                        int32_t thisNode3 = tempTri[j] * 3;
                        if (myCoordData[thisNode3] < tempMinCoord[0]) tempMinCoord[0] = myCoordData[thisNode3];
                        if (myCoordData[thisNode3 + 1] < tempMinCoord[1]) tempMinCoord[1] = myCoordData[thisNode3 + 1];
                        if (myCoordData[thisNode3 + 2] < tempMinCoord[2]) tempMinCoord[2] = myCoordData[thisNode3 + 2];
                        if (myCoordData[thisNode3] > tempMaxCoord[0]) tempMaxCoord[0] = myCoordData[thisNode3];
                        if (myCoordData[thisNode3 + 1] > tempMaxCoord[1]) tempMaxCoord[1] = myCoordData[thisNode3 + 1];
                        if (myCoordData[thisNode3 + 2] > tempMaxCoord[2]) tempMaxCoord[2] = myCoordData[thisNode3 + 2];
                    }
                    for (int ci = 0; ci < 2; ++ci)
                    {
                        for (int cj = 0; cj < 2; ++cj)
                        {
                            for (int ck = 0; ck < 2; ++ck)
                            {
                                if (thisOct->m_children[ci][cj][ck]->boundsOverlaps(tempMinCoord, tempMaxCoord))
                                {
                                    addTriangle(thisOct->m_children[ci][cj][ck], (*(thisOct->m_data.m_triList))[i], tempMinCoord, tempMaxCoord);
                                }
                            }
                        }
                    }
                }
                thisOct->m_data.freeData();//and free up some memory
            }
        }
    } else {
        for (int ci = 0; ci < 2; ++ci)
        {
            for (int cj = 0; cj < 2; ++cj)
            {
                for (int ck = 0; ck < 2; ++ck)
                {
                    if (thisOct->m_children[ci][cj][ck]->boundsOverlaps(minCoord, maxCoord))
                    {
                        addTriangle(thisOct->m_children[ci][cj][ck], triangle, minCoord, maxCoord);
                    }
                }
            }
        }
    }
}
