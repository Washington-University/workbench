/*LICENSE_START*/
/*
 *  Copyright (C) Washington University School of Medicine
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

/*This code can be used with the Oxford FMRIB Software Library and/or
 * Multimodal Surface Matching under the MIT license instead of the
 * above GPL license.  The MIT license follows:*/
/*
 *  Copyright (C) Washington University School of Medicine
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

/*
 * For the function pointInTri():
 * Original copyright for PNPOLY, though my version is entirely rewritten and modified
 * Source: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
 */
/*
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

#include "BoundingBox.h"
#include "CaretHeap.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include <cmath>

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
        Oct<SignedDistanceHelperBase::TriVector>* curOct = myHeap.pop(&tempf);
        if (first || tempf < bestTriDist)
        {
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
    }
    while (numChanged)
    {
        m_triMarked[m_triMarkChanged[--numChanged]] = 0;//need to do this before computeSign
    }
    return bestTriDist * computeSign(coord, bestInfo, myWinding);
}

float SignedDistanceHelper::distLimited(const float coord[3], const float limit, bool& validOut, SignedDistanceHelper::WindingLogic myWinding)
{
    CaretMutexLocker locked(&m_mutex);
    validOut = false;
    CaretSimpleMinHeap<Oct<SignedDistanceHelperBase::TriVector>*, float> myHeap;
    myHeap.push(m_base->m_indexRoot, m_base->m_indexRoot->distToPoint(coord));
    ClosestPointInfo tempInfo, bestInfo;
    float tempf = -1.0f, bestTriDist = limit;
    int numChanged = 0;
    while (!myHeap.isEmpty())
    {
        Oct<SignedDistanceHelperBase::TriVector>* curOct = myHeap.pop(&tempf);
        if (tempf < bestTriDist)
        {
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
                        if (tempf < bestTriDist)
                        {
                            bestInfo = tempInfo;
                            bestTriDist = tempf;
                            validOut = true;
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
                            if (tempf < bestTriDist)
                            {
                                myHeap.push(curOct->m_children[ci][cj][ck], tempf);
                            }
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
    if (validOut)
    {
        return bestTriDist * computeSign(coord, bestInfo, myWinding);
    } else {
        return NAN;//should never be used in this case, so give a nan
    }
}

void SignedDistanceHelper::barycentricWeights(const float coord[3], BarycentricInfo& baryInfoOut)
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
        Oct<SignedDistanceHelperBase::TriVector>* curOct = myHeap.pop(&tempf);
        if (first || tempf < bestTriDist)
        {
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
    }
    while (numChanged)
    {
        m_triMarked[m_triMarkChanged[--numChanged]] = 0;//clean up
    }
    baryInfoOut.triangle = bestInfo.triangle;
    baryInfoOut.point = bestInfo.tempPoint;
    baryInfoOut.absDistance = bestTriDist;
    const int32_t* triNodes = m_base->getTriangle(bestInfo.triangle);
    baryInfoOut.nodes[0] = triNodes[0];
    baryInfoOut.nodes[1] = triNodes[1];
    baryInfoOut.nodes[2] = triNodes[2];
    switch (bestInfo.type)
    {
        case 2:
            {
                baryInfoOut.type = BarycentricInfo::TRIANGLE;
                Vector3D vert1 = m_base->getCoordinate(triNodes[0]);
                Vector3D vert2 = m_base->getCoordinate(triNodes[1]);
                Vector3D vert3 = m_base->getCoordinate(triNodes[2]);
                Vector3D vp1 = vert1 - bestInfo.tempPoint;
                Vector3D vp2 = vert2 - bestInfo.tempPoint;
                Vector3D vp3 = vert3 - bestInfo.tempPoint;
                float weight1 = vp2.cross(vp3).length();
                float weight2 = vp1.cross(vp3).length();
                float weight3 = vp1.cross(vp2).length();
                float weightsum = weight1 + weight2 + weight3;
                baryInfoOut.baryWeights[0] = weight1 / weightsum;
                baryInfoOut.baryWeights[1] = weight2 / weightsum;
                baryInfoOut.baryWeights[2] = weight3 / weightsum;
            }
            break;
        case 1:
            {
                baryInfoOut.type = BarycentricInfo::EDGE;
                Vector3D vert1 = m_base->getCoordinate(bestInfo.node1);
                Vector3D vert2 = m_base->getCoordinate(bestInfo.node2);
                Vector3D v21hat = vert2 - vert1;
                float origLength;
                v21hat = v21hat.normal(&origLength);
                float tempf = v21hat.dot(bestInfo.tempPoint - vert1);
                float weight2 = tempf / origLength;
                float weight1 = 1.0f - weight2;
                for (int i = 0; i < 3; ++i)
                {
                    if (triNodes[i] == bestInfo.node1)
                    {
                        baryInfoOut.baryWeights[i] = weight1;
                    } else if (triNodes[i] == bestInfo.node2) {
                        baryInfoOut.baryWeights[i] = weight2;
                    } else {
                        baryInfoOut.baryWeights[i] = 0.0f;
                    }
                }
            }
            break;
        case 0:
            baryInfoOut.type = BarycentricInfo::NODE;
                for (int i = 0; i < 3; ++i)
                {
                    if (triNodes[i] == bestInfo.node1)
                    {
                        baryInfoOut.baryWeights[i] = 1.0f;
                    } else {
                        baryInfoOut.baryWeights[i] = 0.0f;
                    }
                }
            break;
    };
    for (int i = 0; i < 3; ++i)
    {
        if (baryInfoOut.baryWeights[i] < 0.0f)
        {
            baryInfoOut.baryWeights[i] = 0.0f;
        }
    }
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
                                const int32_t* myTileNodes = m_base->getTriangle(myVecRef[i]);
                                Vector3D verts[3];
                                verts[0] = m_base->getCoordinate(myTileNodes[0]);
                                verts[1] = m_base->getCoordinate(myTileNodes[1]);
                                verts[2] = m_base->getCoordinate(myTileNodes[2]);
                                Vector3D triNormal;
                                MathFunctions::normalVector(verts[0], verts[1], verts[2], triNormal);
                                float factor = triNormal[2];//equivalent to dot product with positiveZ
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
                        const vector<int>& myTiles = m_base->m_topoHelp->getNodeTiles(myInfo.node1);
                        bool first = true;
                        float bestNorm = 0;
                        Vector3D tempvec, tempvec2, bestCent;
                        for (int i = 0; i < (int)myTiles.size(); ++i)//find the tile of the node with the normal most parallel to the line segment between centroid and point
                        {//should be least likely to have an intervening triangle
                            const int32_t* myTileNodes = m_base->getTriangle(myTiles[i]);
                            Vector3D vert1 = m_base->getCoordinate(myTileNodes[0]);
                            Vector3D vert2 = m_base->getCoordinate(myTileNodes[1]);
                            Vector3D vert3 = m_base->getCoordinate(myTileNodes[2]);
                            Vector3D centroid = (vert1 + vert2 + vert3) / 3.0f;
                            if (MathFunctions::normalVector(vert1, vert2, vert3, tempvec))//make sure the triangle has a valid normal
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
                                        const int32_t* myTileNodes = m_base->getTriangle(myVecRef[i]);
                                        Vector3D verts[3];
                                        verts[0] = m_base->getCoordinate(myTileNodes[0]);
                                        verts[1] = m_base->getCoordinate(myTileNodes[1]);
                                        verts[2] = m_base->getCoordinate(myTileNodes[2]);
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
                        const vector<TopologyEdgeInfo>& edgeInfo = m_base->m_topoHelp->getEdgeInfo();
                        const vector<int>& edges = m_base->m_topoHelp->getNodeEdges(myInfo.node1);
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
                            const int32_t* tile1nodes = m_base->getTriangle(tile1);
                            MathFunctions::normalVector(m_base->getCoordinate(tile1nodes[0]),
                                                        m_base->getCoordinate(tile1nodes[1]),
                                                        m_base->getCoordinate(tile1nodes[2]), tempvec);
                            normalaccum += tempvec;
                        }
                        if (tile2 > -1)
                        {
                            const int32_t *tile2nodes = m_base->getTriangle(tile2);
                            MathFunctions::normalVector(m_base->getCoordinate(tile2nodes[0]),
                                                        m_base->getCoordinate(tile2nodes[1]),
                                                        m_base->getCoordinate(tile2nodes[2]), tempvec);
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
                        const int32_t* triNodes = m_base->getTriangle(myInfo.triangle);
                        Vector3D vert1 = m_base->getCoordinate(triNodes[0]);
                        Vector3D vert2 = m_base->getCoordinate(triNodes[1]);
                        Vector3D vert3 = m_base->getCoordinate(triNodes[2]);
                        MathFunctions::normalVector(vert1, vert2, vert3, triNormal);
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

bool SignedDistanceHelper::pointInTri(Vector3D verts[3], Vector3D inPlane, int majAxis, int midAxis)
{
    bool inside = false;
    for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
    {
        if ((verts[i][majAxis] < inPlane[majAxis]) != (verts[j][majAxis] < inPlane[majAxis]))
        {//if one vertex is on one side of the point in the x direction, and the other is on the other side (equal case is treated as greater)
            int ti, tj;
            if (verts[i][majAxis] < verts[j][majAxis])//reorient the segment consistently to get a consistent answer
            {
                ti = i; tj = j;
            } else {
                ti = j; tj = i;
            }
            if ((verts[ti][midAxis] - verts[tj][midAxis]) / (verts[ti][majAxis] - verts[tj][majAxis]) * (inPlane[majAxis] - verts[tj][majAxis]) + verts[tj][midAxis] > inPlane[midAxis])
            {//if the point on the line described by the two vertices with the same x coordinate is above (greater y) than the test point
                inside = !inside;//even/odd winding rule
            }
        }
        j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
    }
    return inside;
}

///"dumb" implementation, projects to plane, test if inside while finding closest point on each edge
///there are faster implementations out there, but this is easier to follow
float SignedDistanceHelper::unsignedDistToTri(const float coord[3], int32_t triangle, ClosestPointInfo& myInfo)
{
    const int32_t* triNodes = m_base->getTriangle(triangle);
    Vector3D point = coord;
    Vector3D verts[3];
    int type = 0;//tracks whether it is closest to a node, an edge, or the face
    int32_t node1 = -1, node2 = -1;//tracks which nodes are involved
    Vector3D bestPoint;
    verts[0] = m_base->getCoordinate(triNodes[0]);
    verts[1] = m_base->getCoordinate(triNodes[1]);
    verts[2] = m_base->getCoordinate(triNodes[2]);
    Vector3D v10 = verts[1] - verts[0];
    Vector3D xhat = v10.normal();
    Vector3D v20 = verts[2] - verts[0];
    float sanity;
    Vector3D yhat = (v20 - xhat * xhat.dot(v20)).normal(&sanity);//now we have our orthogonal basis vectors for projection
    if (sanity == 0.0f || abs(xhat.dot(yhat)) > 0.01f)//if our triangle is (mostly) degenerate, find the closest point on its edges instead of trying to project to the NaN plane
    {
        bool first = true;
        float bestLengthSqr = -1.0f;//track best squared length from edge to original point
        for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
        {
            float length;
            Vector3D norm = (verts[j] - verts[i]).normal(&length);
            Vector3D mypoint;
            int temptype = 0, tempnode1, tempnode2 = -1;
            if (length > 0.0f)
            {
                Vector3D diff = point - verts[i];
                float dot = norm.dot(diff);
                if (dot <= 0.0f)
                {
                    mypoint = verts[i];
                    tempnode1 = triNodes[i];
                } else if (dot >= length) {
                    mypoint = verts[j];
                    tempnode1 = triNodes[j];
                } else {
                    mypoint = verts[i] + dot * norm;
                    temptype = 1;
                    tempnode1 = triNodes[i];
                    tempnode2 = triNodes[j];
                }
            } else {
                temptype = 0;
                tempnode1 = triNodes[i];
                mypoint = verts[i];
            }
            float tempdistsqr = (point - mypoint).lengthsquared();
            if (first || tempdistsqr < bestLengthSqr)
            {
                first = false;
                type = temptype;
                bestLengthSqr = tempdistsqr;
                bestPoint = mypoint;
                node1 = tempnode1;
                node2 = tempnode2;
            }
            j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
        }
    } else {
        float vertxy[3][2];
        for (int i = 0; i < 3; ++i)//project everything to the new plane with basis vectors xhat, yhat
        {
            vertxy[i][0] = xhat.dot(verts[i] - verts[0]);
            vertxy[i][1] = yhat.dot(verts[i] - verts[0]);
        }
        bool inside = true;
        float p[2] = { xhat.dot(point - verts[0]), yhat.dot(point - verts[0]) };
        float bestxy[2];
        float bestDist = -1.0f;
        for (int i = 0, j = 2, k = 1; i < 3; ++i)//start with the wraparound case
        {
            float norm[2] = { vertxy[j][0] - vertxy[i][0], vertxy[j][1] - vertxy[i][1] };
            float diff[2] = { p[0] - vertxy[i][0], p[1] - vertxy[i][1] };
            float direction[2] = { vertxy[k][0] - vertxy[i][0], vertxy[k][1] - vertxy[i][1] };
            float edgelen = sqrt(norm[0] * norm[0] + norm[1] * norm[1]);
            if (edgelen != 0.0f)
            {
                norm[0] /= edgelen; norm[1] /= edgelen;
                float dot = direction[0] * norm[0] + direction[1] * norm[1];
                direction[0] -= dot * norm[0];//direction is orthogonal to norm, in the direction of the third vertex
                direction[1] -= dot * norm[1];
                if (diff[0] * direction[0] + diff[1] * direction[1] < 0.0f)//if dot product with (projected point - vert[i]) is negative
                {//we are outside the triangle, find the projection to this edge and break if it is the second time or otherwise known to be finished
                    if (bestDist < 0.0f)
                    {
                        inside = false;
                        dot = diff[0] * norm[0] + diff[1] * norm[1];
                        if (dot <= 0.0f)//if closest point on this edge is an endpoint, it is possible for another edge that we count as outside of to have a closer point
                        {
                            type = 0;
                            node1 = triNodes[i];
                            bestPoint = verts[i];
                            bestxy[0] = vertxy[i][0]; bestxy[1] = vertxy[i][1];
                        } else if (dot >= edgelen) {
                            type = 0;
                            node1 = triNodes[j];
                            bestPoint = verts[j];
                            bestxy[0] = vertxy[j][0]; bestxy[1] = vertxy[j][1];
                        } else {//if closest point on the edge is in the middle of the edge, nothing can be closer, break
                            type = 1;
                            node1 = triNodes[i];
                            node2 = triNodes[j];
                            bestxy[0] = vertxy[i][0] + dot * norm[0];
                            bestxy[1] = vertxy[i][1] + dot * norm[1];
                            break;
                        }
                        diff[0] = p[0] - bestxy[0]; diff[1] = p[1] - bestxy[1];
                        bestDist = diff[0] * diff[0] + diff[1] * diff[1];
                    } else {
                        int tempnode1;
                        Vector3D tempbestPoint;
                        float tempxy[2];
                        inside = false;
                        dot = diff[0] * norm[0] + diff[1] * norm[1];
                        if (dot <= 0.0f)
                        {
                            tempnode1 = triNodes[i];
                            tempbestPoint = verts[i];
                            tempxy[0] = vertxy[i][0]; tempxy[1] = vertxy[i][1];
                        } else if (dot >= edgelen) {
                            tempnode1 = triNodes[j];
                            tempbestPoint = verts[j];
                            tempxy[0] = vertxy[j][0]; tempxy[1] = vertxy[j][1];
                        } else {//again, middle of edge always wins, don't bother with the extra test
                            type = 1;
                            node1 = triNodes[i];
                            node2 = triNodes[j];
                            bestxy[0] = vertxy[i][0] + dot * norm[0];
                            bestxy[1] = vertxy[i][1] + dot * norm[1];
                            break;
                        }
                        diff[0] = p[0] - tempxy[0]; diff[1] = p[1] - tempxy[1];
                        float tempdist = diff[0] * diff[0] + diff[1] * diff[1];
                        if (tempdist < bestDist)
                        {
                            type = 0;//if it were in the middle of the edge, we wouldn't be here
                            node1 = tempnode1;
                            bestPoint = tempbestPoint;
                            bestxy[0] = tempxy[0]; bestxy[0] = tempxy[0];
                        }
                        break;//this is our second time outside an edge, we have now covered all 3 possible endpoints, so break
                    }
                }
            } else {
                if (diff[0] * direction[0] + diff[1] * direction[1] < 0.0f)//since we don't have an edge, we don't need to othrogonalize direction, or project to the edge
                {
                    inside = false;
                    type = 0;
                    node1 = triNodes[i];
                    bestPoint = verts[i];
                    break;
                }
            }
            k = j;
            j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
        }
        if (inside)
        {
            bestxy[0] = p[0]; bestxy[1] = p[1];
            type = 2;
        }
        if (type != 0)
        {
            bestPoint = bestxy[0] * xhat + bestxy[1] * yhat + verts[0];
        }
    }
    Vector3D result = point - bestPoint;
    myInfo.type = type;
    myInfo.node1 = node1;
    myInfo.node2 = node2;
    myInfo.triangle = triangle;
    myInfo.tempPoint = bestPoint;
    return result.length();
}

SignedDistanceHelper::SignedDistanceHelper(CaretPointer<SignedDistanceHelperBase> myBase)
{
    m_base = myBase;
    int32_t numTris = m_base->m_numTris;
    m_triMarked = CaretArray<int>(numTris);
    m_triMarkChanged = CaretArray<int>(numTris);
    for (int32_t i = 0; i < numTris; ++i)
    {
        m_triMarked[i] = 0;
    }
}

SignedDistanceHelperBase::SignedDistanceHelperBase(const SurfaceFile* mySurf)
{
    m_topoHelp = mySurf->getTopologyHelper();
    const float* myBB = mySurf->getBoundingBox()->getBounds();
    Vector3D minCoord, maxCoord;
    minCoord[0] = myBB[0]; maxCoord[0] = myBB[1];
    minCoord[1] = myBB[2]; maxCoord[1] = myBB[3];
    minCoord[2] = myBB[4]; maxCoord[2] = myBB[5];
    m_indexRoot.grabNew(new Oct<TriVector>(minCoord, maxCoord));
    const float* myCoordData = mySurf->getCoordinateData();
    m_numNodes = mySurf->getNumberOfNodes();
    int32_t numNodes3 = m_numNodes * 3;
    m_coordList.resize(numNodes3);
    for (int32_t i = 0; i < numNodes3; ++i)
    {
        m_coordList[i] = myCoordData[i];
    }
    m_numTris = mySurf->getNumberOfTriangles();
    m_triangleList.resize(m_numTris * 3);
    for (int32_t i = 0; i < m_numTris; ++i)
    {
        int32_t i3 = i * 3;
        const int32_t* thisTri = mySurf->getTriangle(i);
        m_triangleList[i3] = thisTri[0];
        m_triangleList[i3 + 1] = thisTri[1];
        m_triangleList[i3 + 2] = thisTri[2];
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
            const float* myCoordData = m_coordList.data();
            int totalSize = 0;
            int numSplit = 0;
            for (int i = 0; i < numTris; ++i)//gather data on how it would end up splitting
            {
                const int32_t* tempTri = getTriangle((*(thisOct->m_data.m_triList))[i]);
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
                    const int32_t* tempTri = getTriangle((*(thisOct->m_data.m_triList))[i]);
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

const float* SignedDistanceHelperBase::getCoordinate(const int32_t nodeIndex) const
{
    CaretAssert(nodeIndex >= 0 && nodeIndex < m_numNodes);
    return m_coordList.data() + (nodeIndex * 3);
}

const int32_t* SignedDistanceHelperBase::getTriangle(const int32_t tileIndex) const
{
    CaretAssert(tileIndex >= 0 && tileIndex < m_numTris);
    return m_triangleList.data() + (tileIndex * 3);
}

SignedDistanceHelperBase::~SignedDistanceHelperBase()
{//so we don't need to put TopologyHelper.h in our header, due to the smart pointer destructor
}
