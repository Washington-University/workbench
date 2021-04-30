/*LICENSE_START*/
/*
 *  Copyright (C) 2021  Washington University School of Medicine
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

#include "OperationSurfaceSphereTriangularPatches.h"
#include "OperationException.h"

#include "GeodesicHelper.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace caret;
using namespace std;

AString OperationSurfaceSphereTriangularPatches::getCommandSwitch()
{
    return "-surface-sphere-triangular-patches";
}

AString OperationSurfaceSphereTriangularPatches::getShortDescription()
{
    return "DIVIDE STANDARD SPHERE INTO PATCHES";
}

OperationParameters* OperationSurfaceSphereTriangularPatches::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "sphere", "an undistorted, regularly divided icosahedral sphere");
    
    ret->addIntegerParameter(2, "divisions", "how many pieces to divide each icosahedral edge into, must divide perfectly into the given sphere");
    
    ret->addStringParameter(3, "text-out", "output - text file for the vertex numbers of the patches"); //fake the output formatting
    
    ret->setHelpText(
        AString("Divide the given undistorted sphere into equally-sized triangular patches.  ") +
        "Patches overlap by a border of 1 vertex."
    );
    return ret;
}

namespace
{
    bool checkSphere(const SurfaceFile* surface)
    {
        int numNodes = surface->getNumberOfNodes();
        CaretAssert(numNodes > 1);
        int numNodes3 = numNodes * 3;
        const float* coordData = surface->getCoordinateData();
        float mindist = Vector3D(coordData).length();//first coordinate
        if (mindist != mindist) throw CaretException("found NaN coordinate in the input sphere");
        float maxdist = mindist;
        const float TOLERANCE = 1.001f;
        for (int i = 3; i < numNodes3; i += 3)
        {
            float tempf = Vector3D(coordData + i).length();
            if (tempf != tempf) throw CaretException("found NaN coordinate in the input sphere");
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
    
    struct TriTracker
    {
        vector<vector<int> > triList;
        typedef map<pair<int, int>, vector<int> > tt_maptype;
        tt_maptype edgeLookup;
        void addTri(int a, int b, int c)
        {
            if (findTri(a, b, c) != -1) return;
            int trinum = int(triList.size());
            triList.push_back(vector<int>{a, b, c});
            addEdge(a, b, trinum);//keep order here for sanity, but addEdge reorients for lookup
            addEdge(b, c, trinum);
            addEdge(c, a, trinum);
        }
        int findTri(int a, int b, int c)
        {
            auto result = findEdge(a, b);
            if (result == edgeLookup.end()) return -1;
            for (auto third : result->second)
            {
                for (int i = 0; i < 3; ++i)
                {
                    if (triList[third][i] == c)
                    {
                        return third;
                    }
                }
            }
            return -1;
        }
        void addEdge(int a, int b, int tri)
        {
            if (b < a)
            {
                swap(a, b);
            }
            edgeLookup[make_pair(a, b)].push_back(tri);
        }
        tt_maptype::iterator findEdge(int a, int b)
        {
            if (b < a)
            {
                swap(a, b);
            }
            return edgeLookup.find(make_pair(a, b));
        }
    };
    
    vector<int> tilePatch(const SurfaceFile* mySurf, const int vert1, const int vert2, const int patchSize, const bool patternB, int& vert3out)
    {
        vector<int> ret;
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        const float* coordData = mySurf->getCoordinateData();
        Vector3D coord1(coordData + 3 * vert1), coord2(coordData + 3 * vert2);
        Vector3D edge1dir = (coord2 - coord1);
        edge1dir = (edge1dir - coord1.normal() * edge1dir.dot(coord1.normal())).normal();//find tangent, normalize
        Vector3D left90 = coord1.normal().cross(edge1dir.normal()).normal();
        Vector3D left60 = (edge1dir + left90 * sqrt(3)).normal();//some of these will be from the 5-verts, which will be 72 degrees...
        Vector3D stepDirection = left60;
        int curVert = vert1;
        for (int i = 1; i < patchSize; ++i)
        {
            Vector3D curCoord(coordData + 3 * curVert);
            int bestNeigh = -1;
            float bestDot = -2.0f;
            for (auto neigh : myTopoHelp->getNodeNeighbors(curVert))
            {
                Vector3D neighCoord(coordData + 3 * neigh);
                float thisDot = (neighCoord - curCoord).normal().dot(stepDirection);
                if (thisDot > bestDot || bestNeigh == -1)
                {
                    bestDot = thisDot;
                    bestNeigh = neigh;
                }
            }
            stepDirection = (Vector3D(coordData + 3 * bestNeigh) - curCoord).normal();
            curVert = bestNeigh;
        }
        vert3out = curVert;
        int vertA = vert1, vertB = vert2, vertC = vert3out;
        if (patternB)
        {
            vertA = vert2; vertB = vert3out; vertC = vert1;
        }
        vector<int32_t> bottomEdge, rightEdge, tempRow;//A is top, B is left, arbitrarily choose to tile down the left side first
        vector<float> junk;
        myGeoHelp->getPathToNode(vertA, vertC, rightEdge, junk);
        myGeoHelp->getPathToNode(vertB, vertC, bottomEdge, junk);
        if (int(rightEdge.size()) != patchSize) throw OperationException("input sphere must not be distorted");
        if (int(bottomEdge.size()) != patchSize) throw OperationException("input sphere must not be distorted");
        for (int i = 0; i < patchSize; ++i)
        {
            myGeoHelp->getPathToNode(rightEdge[i], bottomEdge[i], tempRow, junk);
            if (int(tempRow.size()) + i != patchSize) throw OperationException("input sphere must not be distorted");//to be really sure, we could check if any single patch reuses a vertex
            ret.insert(ret.end(), tempRow.begin(), tempRow.end());
        }
        return ret;
    }
}

void OperationSurfaceSphereTriangularPatches::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    int32_t edgeDivisions = myParams->getInteger(2);
    AString outName = myParams->getString(3);
    if (!checkSphere(mySurf))
    {
        throw OperationException("input sphere is not round or not centered on the origin");
    }
    int numNodes = mySurf->getNumberOfNodes();
    CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//prefer not to rely on sorting?
    vector<int> startVerts;
    for (int i = 0; i < numNodes; ++i)
    {
        int neighcount = int(myTopoHelp->getNodeNeighbors(i).size());
        if (neighcount < 5 || neighcount > 6) throw OperationException("input sphere needs to be a regularly divided icosahedron");//reject native mesh
        if (neighcount == 5)
        {
            startVerts.push_back(i);
            if (startVerts.size() > 12) throw OperationException("input sphere needs to be a regularly divided icosahedron");//reject native mesh
        }
    }
    if (startVerts.size() != 12) throw OperationException("input sphere has unexpected topology");//handles?
    const float* coordData = mySurf->getCoordinateData();
    float radius = Vector3D(coordData).length();
    vector<set<int> > icoNeigh(12);
    for (int i = 0; i < 12; ++i)
    {
        Vector3D thisPoint(coordData + startVerts[i] * 3);
        for (int j = 0; j < 12; ++j)
        {
            if (j == i) continue;
            Vector3D otherPoint(coordData + startVerts[j] * 3);
            if ((otherPoint - thisPoint).length() < radius / 0.9f)//could be made less fragile by going straight to geodesic paths - exact number is 0.951...
            {
                icoNeigh[i].insert(j);
            }
        }
        if (icoNeigh[i].size() != 5) throw OperationException("input sphere must not be distorted");
    }
    TriTracker icoTris;
    for (int i = 0; i < 12; ++i)
    {
        int firstNeigh = *(icoNeigh[i].begin());
        int curNeigh = firstNeigh;
        int itertrack = 0;
        do
        {
            bool found = false;
            for (auto neigh2 : icoNeigh[curNeigh])
            {
                if (icoNeigh[i].find(neigh2) != icoNeigh[i].end())
                {
                    Vector3D a(coordData + 3 * startVerts[i]), b(coordData + 3 * startVerts[curNeigh]), c(coordData + 3 * startVerts[neigh2]);
                    if ((b - a).cross(c - a).dot(a) > 0)//use only counter-clockwise triangles, also results in walking "around" the neighbors
                    {
                        icoTris.addTri(i, curNeigh, neigh2);//this prevents adding duplicates
                        found = true;
                        curNeigh = neigh2;
                        break;
                    }
                }
            }
            if (!found) throw OperationException("input sphere has unexpected topology");
            ++itertrack;
            if (itertrack > 5) throw OperationException("input sphere has unexpected topology");
        } while (curNeigh != firstNeigh);
    }
    if (icoTris.triList.size() != 20) throw OperationException("input sphere has unexpected topology");
    int edgeVerts, patchSize;
    CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
    {//don't keep these temporaries around
        vector<int32_t> pathNodes;
        vector<float> junk;
        myGeoHelp->getPathToNode(startVerts[icoTris.triList[0][0]], startVerts[icoTris.triList[0][1]], pathNodes, junk);
        edgeVerts = int(pathNodes.size());//includes start and end
        if (edgeVerts % edgeDivisions != 1) throw OperationException("input sphere appears to use " + AString::number(edgeVerts - 1) + " divisions, which is not divisible by the requested amount");//maybe check for consistency across different pairs first
        patchSize = edgeVerts / edgeDivisions + 1;//round down ignores overlap
    }
    vector<vector<int> > cornerCount(3, vector<int>(12, 0));//track all 3 corners of each ico face, then do a greedy optimization
    vector<int> faceUsed(20, 0), vertUsed(12, 0);
    vector<vector<int> > patches;
    vector<int> openList;//breadth-first
    openList.push_back(0);
    for (int curIndex = 0; curIndex < int(openList.size()); ++curIndex)
    {
        vertUsed[openList[curIndex]] = 2;
        for (auto neigh : icoNeigh[openList[curIndex]])
        {//do all the triangles around a vertex before moving on - could make a dedicated vert to tri lookup instead of the double loop...
            for (auto curTri : icoTris.findEdge(openList[curIndex], neigh)->second)
            {
                if (faceUsed[curTri] == 0)
                {
                    faceUsed[curTri] = 1;
                    int bestcost = -1, bestvert = -1;
                    for (int firstvert = 0; firstvert < 3; ++firstvert)
                    {
                        if (vertUsed[icoTris.triList[curTri][firstvert]] == 0)
                        {//while we are looping over this anyway, complete the outer breadth first search too
                            openList.push_back(icoTris.triList[curTri][firstvert]);
                            vertUsed[icoTris.triList[curTri][firstvert]] = 1;//only push it once
                        }
                        int cost = 0;
                        for (int i = 0; i < 3; ++i)
                        {
                            int temp = 1 + cornerCount[i][icoTris.triList[curTri][(firstvert + i) % 3]];
                            cost += temp * temp;//square it to penalize 3 much more than 2
                        }
                        if (bestcost == -1 || cost < bestcost)
                        {
                            bestcost = cost;
                            bestvert = firstvert;
                        }
                    }
                    //actual tiling, finally
                    int realverts[3] = {startVerts[icoTris.triList[curTri][bestvert]],
                                        startVerts[icoTris.triList[curTri][(bestvert + 1) % 3]],
                                        startVerts[icoTris.triList[curTri][(bestvert + 2) % 3]]};//counterclockwise
                    vector<int32_t> rightEdge, leftEdge;//first vertex is "top", looking from outside sphere
                    vector<float> junk;
                    myGeoHelp->getPathToNode(realverts[0], realverts[1], leftEdge, junk);
                    myGeoHelp->getPathToNode(realverts[0], realverts[2], rightEdge, junk);//NOT counterclockwise
                    if (int(rightEdge.size()) != edgeVerts || int(leftEdge.size()) != edgeVerts) throw OperationException("input sphere needs to be a regularly divided icosahedron and not distorted");
                    int rowBase = realverts[0];
                    for (int i = 0; i < edgeDivisions; ++i)
                    {//start at top, tile left to right
                        int nextBase = leftEdge[(i + 1) * (patchSize - 1)];
                        int pairBase = rowBase;
                        int pairTwo = nextBase;
                        int thirdVert = -1;//second output from tilePatch()
                        for (int j = 0; j < i; ++j)
                        {//do pairs
                            patches.push_back(tilePatch(mySurf, pairBase, pairTwo, patchSize, false, thirdVert));
                            pairTwo = thirdVert;
                            patches.push_back(tilePatch(mySurf, pairBase, pairTwo, patchSize, true, thirdVert));
                            pairBase = thirdVert;
                        }
                        //then the last one
                        patches.push_back(tilePatch(mySurf, pairBase, pairTwo, patchSize, false, thirdVert));
                        //prepare for next row
                        rowBase = nextBase;
                    }
                }
            }
        }
    }
    ofstream outFile(outName.toLocal8Bit().constData());
    if (!outFile) throw OperationException("failed to open output text file");
    for (auto patch : patches)
    {
        bool first = true;
        for (auto vert : patch)
        {
            if (!first) outFile << ",";
            first = false;
            outFile << vert;
        }
        outFile << endl;
    }
    if (!outFile) throw OperationException("error writing to text file");
}
