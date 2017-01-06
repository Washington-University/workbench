/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

/*
 * For the function TriInfi::vertRayHit():
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

#include "RibbonMappingHelper.h"

#include "CaretException.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "VolumeSpace.h"

#include <cmath>

using namespace caret;
using namespace std;

//private namespace for implementation details
namespace
{
    
    struct TriInfo
    {
        Vector3D m_xyz[3];
        float m_planeEq[3];//x coef, y coef, const : z = [0] * x + [1] * y + [2]
        bool vertRayHit(const float* xyz);//true if a +z ray from point hits this triangle
        TriInfo(const float* xyz1, const float* xyz2, const float* xyz3);
        TriInfo() {};
    };
    
    struct QuadInfo
    {
        TriInfo m_tris[2][2];
        int vertRayHit(const float* xyz);//+z ray intersect: 0 if never, 1 if only 1 of the 2 triangulations, 2 if both
        QuadInfo(const float* xyz1, const float* xyz2, const float* xyz3, const float* xyz4);
        QuadInfo() {};
    };
    
    struct PolyInfo
    {
        std::vector<TriInfo> m_tris;
        std::vector<QuadInfo> m_quads;
        PolyInfo(const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int32_t node, const bool& thinColumn = false);//surfaces MUST be in node correspondence, otherwise SEVERE strangeness, possible crashes
        PolyInfo() {};
        int isInside(const float* xyz);//0 for no, 2 for yes, 1 for if only half the triangulations (between the two triangulations of one of the quad faces)
    private:
        void addTri(const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int32_t* myTri, const int rootIndex, const bool& thinColumn);//adds the tri for each surface, plus the quad
    };
    
    void PolyInfo::addTri(const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int32_t* myTri, const int rootIndex, const bool& thinColumn)
    {//tries to orient normals consistently with respect to inside/outside, but it doesn't currently matter
        int root = myTri[rootIndex], node2 = myTri[(rootIndex + 1) % 3], node3 = myTri[(rootIndex + 2) % 3];
        if (thinColumn)
        {
            Vector3D innerRoot = innerSurf->getCoordinate(root), inner2 = innerSurf->getCoordinate(node2), inner3 = innerSurf->getCoordinate(node3);
            Vector3D outerRoot = outerSurf->getCoordinate(root), outer2 = outerSurf->getCoordinate(node2), outer3 = outerSurf->getCoordinate(node3);
            //use an ordering trick of the 3 vertices so that the summing order is always 01, 12, or 20 - doesn't actually matter (at least on x86), but hey
            Vector3D innerEdge2 = (innerRoot + inner2) / 2, innerEdge3 = (inner3 + innerRoot) / 2;
            Vector3D outerEdge2 = (outerRoot + outer2) / 2, outerEdge3 = (outer3 + outerRoot) / 2;
            //for consistency in floating point rounding, we need to generate the sum of the 3 coordinates in the same order regardless of rootIndex
            //so, use the original triangle definition and 0, 1, 2 order
            //this doesn't matter for edges because there only 2 coordinates are added together
            Vector3D innerCenter = (Vector3D(innerSurf->getCoordinate(myTri[0])) + 
                                    Vector3D(innerSurf->getCoordinate(myTri[1])) +
                                    Vector3D(innerSurf->getCoordinate(myTri[2]))) / 3;
            Vector3D outerCenter = (Vector3D(outerSurf->getCoordinate(myTri[0])) + 
                                    Vector3D(outerSurf->getCoordinate(myTri[1])) +
                                    Vector3D(outerSurf->getCoordinate(myTri[2]))) / 3;
            m_tris.push_back(TriInfo(innerRoot, innerEdge3, innerCenter));
            m_tris.push_back(TriInfo(innerRoot, innerCenter, innerEdge2));
            m_tris.push_back(TriInfo(outerRoot, outerEdge2, outerCenter));
            m_tris.push_back(TriInfo(outerRoot, outerCenter, outerEdge3));
            m_quads.push_back(QuadInfo(innerEdge2, innerCenter, outerCenter, outerEdge2));
            m_quads.push_back(QuadInfo(innerCenter, innerEdge3, outerEdge3, outerCenter));
        } else {
            m_tris.push_back(TriInfo(innerSurf->getCoordinate(root), innerSurf->getCoordinate(node3), innerSurf->getCoordinate(node2)));
            m_tris.push_back(TriInfo(outerSurf->getCoordinate(root), outerSurf->getCoordinate(node2), outerSurf->getCoordinate(node3)));
            m_quads.push_back(QuadInfo(innerSurf->getCoordinate(node2), innerSurf->getCoordinate(node3), outerSurf->getCoordinate(node3), outerSurf->getCoordinate(node2)));
        }
    }

    int PolyInfo::isInside(const float* xyz)
    {
        int i, temp, numQuads = (int)m_quads.size();
        bool toggle = false;
        for (i = 0; i < numQuads; ++i)
        {
            temp = m_quads[i].vertRayHit(xyz);
            if (temp == 1) return 1;//means only one of the two triangulations has a hit, therefore, we can return early
            if (temp) toggle = !toggle;//even/odd winding rule
        }
        int numTris = (int)m_tris.size();
        for (i = 0; i < numTris; ++i)
        {
            if (m_tris[i].vertRayHit(xyz)) toggle = !toggle;
        }
        if (toggle) return 2;
        return 0;
    }

    PolyInfo::PolyInfo(const caret::SurfaceFile* innerSurf, const caret::SurfaceFile* outerSurf, const int32_t node, const bool& thinColumn)
    {
        CaretPointer<TopologyHelper> myTopoHelp = innerSurf->getTopologyHelper();
        int numTiles;
        const int* myTiles = myTopoHelp->getNodeTiles(node, numTiles);
        for (int i = 0; i < numTiles; ++i)
        {
            const int32_t* myTri = innerSurf->getTriangle(myTiles[i]);
            if (myTri[0] == node)
            {
                addTri(innerSurf, outerSurf, myTri, 0, thinColumn);
            } else {
                if (myTri[1] == node)
                {
                    addTri(innerSurf, outerSurf, myTri, 1, thinColumn);
                } else {
                    addTri(innerSurf, outerSurf, myTri, 2, thinColumn);
                }
            }
        }
    }

    QuadInfo::QuadInfo(const float* xyz1, const float* xyz2, const float* xyz3, const float* xyz4)
    {
        m_tris[0][0] = TriInfo(xyz1, xyz2, xyz3);
        m_tris[0][1] = TriInfo(xyz1, xyz3, xyz4);
        m_tris[1][0] = TriInfo(xyz1, xyz2, xyz4);
        m_tris[1][1] = TriInfo(xyz2, xyz3, xyz4);
    }

    int QuadInfo::vertRayHit(const float* xyz)
    {
        int ret = 0;
        if (m_tris[0][0].vertRayHit(xyz) != m_tris[0][1].vertRayHit(xyz)) ++ret;
        if (m_tris[1][0].vertRayHit(xyz) != m_tris[1][1].vertRayHit(xyz)) ++ret;
        return ret;
    }

    TriInfo::TriInfo(const float* xyz1, const float* xyz2, const float* xyz3)
    {
        m_xyz[0] = xyz1; m_xyz[1] = xyz2; m_xyz[2] = xyz3;
        FloatMatrix myRref;
        myRref.resize(3, 4);
        for (int i = 0; i < 3; ++i)//ax + by + c = z
        {
            myRref[i][0] = m_xyz[i][0];//coefficient of a
            myRref[i][1] = m_xyz[i][1];//coefficient of b
            myRref[i][2] = 1;//coefficient of c
            myRref[i][3] = m_xyz[i][2];//what it equals
        }
        FloatMatrix myResult = myRref.reducedRowEchelon();
        m_planeEq[0] = myResult[0][3];//a
        m_planeEq[1] = myResult[1][3];//b
        m_planeEq[2] = myResult[2][3];//c
        float sanity = m_planeEq[0] + m_planeEq[1] + m_planeEq[2];
        if (!MathFunctions::isNumeric(sanity))
        {
            m_planeEq[0] = sanity;//make sure the first element easily identifies vertical triangles
        }
    }

    bool TriInfo::vertRayHit(const float* xyz)
    {
        if (!MathFunctions::isNumeric(m_planeEq[0]))
        {//plane is vertical, nothing can hit it
            return false;
        }
        float planeZ = xyz[0] * m_planeEq[0] + xyz[1] * m_planeEq[1] + m_planeEq[2];//ax + by + c = z
        if (xyz[2] >= planeZ)
        {//point is above the plane
            return false;
        }//test if the x, y projection has the point inside the triangle
        //below logic copied from PNPOLY by Wm. Randolph Franklin, swapped x for y, and slightly rewritten, for the special case of 3 vertices
        bool inside = false;
        for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
        {
            if ((m_xyz[i][0] < xyz[0]) != (m_xyz[j][0] < xyz[0]))
            {//if one vertex is on one side of the point in the x direction, and the other is on the other side (equal case is treated as greater)
                int ti, tj;
                if (m_xyz[i][0] < m_xyz[j][0])//reorient the segment consistently to get a consistent answer
                {
                    ti = i; tj = j;
                } else {
                    ti = j; tj = i;
                }
                if ((m_xyz[ti][1] - m_xyz[tj][1]) / (m_xyz[ti][0] - m_xyz[tj][0]) * (xyz[0] - m_xyz[tj][0]) + m_xyz[tj][1] > xyz[1])
                {//if the point on the line described by the two vertices with the same x coordinate is above (greater y) than the test point
                    inside = !inside;//even/odd winding rule again
                }
            }
            j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
        }
        return inside;
    }
    
    float computeVoxelFraction(const VolumeSpace& myVolSpace, const int64_t* ijk, PolyInfo& myPoly, const int divisions,
                               const Vector3D& ivec, const Vector3D& jvec, const Vector3D& kvec)
    {
        Vector3D myLowCorner;
        myVolSpace.indexToSpace(ijk[0] - 0.5f, ijk[1] - 0.5f, ijk[2] - 0.5f, myLowCorner);
        int inside = 0;
        Vector3D istep = ivec / divisions;
        Vector3D jstep = jvec / divisions;
        Vector3D kstep = kvec / divisions;
        myLowCorner += istep * 0.5f + jstep * 0.5f + kstep * 0.5f;
        for (int i = 0; i < divisions; ++i)
        {
            Vector3D tempVeci = myLowCorner + istep * i;
            for (int j = 0; j < divisions; ++j)
            {
                Vector3D tempVecj = tempVeci + jstep * j;
                for (int k = 0; k < divisions; ++k)
                {
                    Vector3D thisPoint = tempVecj + kstep * k;
                    inside += myPoly.isInside(thisPoint);
                }
            }
        }
        return ((float)inside) / (divisions * divisions * divisions * 2);
    }
    
}

void RibbonMappingHelper::computeWeightsRibbon(vector<vector<VoxelWeight> >& myWeightsOut, const VolumeSpace& myVolSpace, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf,
                                               const float* roiFrame, const int& numDivisions, const bool& thinColumn)
{
    if (!innerSurf->hasNodeCorrespondence(*outerSurf))
    {
        throw CaretException("input surfaces to ribbon mapping do not have vertex correspondence");
    }
    if (numDivisions < 1)
    {
        throw CaretException("number of voxel subdivisions must be positive for ribbon mapping");
    }
    int64_t numNodes = outerSurf->getNumberOfNodes();
    myWeightsOut.resize(numNodes);
    Vector3D origin, ivec, jvec, kvec;//these are the spatial projections of the ijk unit vectors (also, the offset that specifies the origin)
    myVolSpace.getSpacingVectors(ivec, jvec, kvec, origin);
    const float* outerCoords = outerSurf->getCoordinateData();
    const float* innerCoords = innerSurf->getCoordinateData();
    const int64_t* myDims = myVolSpace.getDims();
#pragma omp CARET_PAR
    {
        int maxVoxelCount = 10;//guess for preallocating vectors
        CaretPointer<TopologyHelper> myTopoHelp = innerSurf->getTopologyHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int64_t node = 0; node < numNodes; ++node)
        {
            myWeightsOut[node].clear();
            myWeightsOut[node].reserve(maxVoxelCount);
            float tempf;
            int64_t node3 = node * 3;
            PolyInfo myPoly(innerSurf, outerSurf, node, thinColumn);//build the polygon
            Vector3D minIndex, maxIndex, tempvec;
            myVolSpace.spaceToIndex(innerCoords + node3, minIndex);//find the bounding box in VOLUME INDEX SPACE, starting with the center nodes
            maxIndex = minIndex;
            myVolSpace.spaceToIndex(outerCoords + node3, tempvec);
            for (int i = 0; i < 3; ++i)
            {
                if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
            }
            int numNeigh;
            const int* myNeighList = myTopoHelp->getNodeNeighbors(node, numNeigh);//and now the neighbors
            for (int j = 0; j < numNeigh; ++j)
            {
                int neigh3 = myNeighList[j] * 3;
                myVolSpace.spaceToIndex(outerCoords + neigh3, tempvec);
                for (int i = 0; i < 3; ++i)
                {
                    if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                    if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
                }
                myVolSpace.spaceToIndex(innerCoords + neigh3, tempvec);
                for (int i = 0; i < 3; ++i)
                {
                    if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                    if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
                }
            }
            int startIndex[3], endIndex[3];
            for (int i = 0; i < 3; ++i)
            {
                startIndex[i] = (int)ceil(minIndex[i] - 0.5f);//give an extra half voxel in order to get anything which could have some polygon in it
                endIndex[i] = (int)floor(maxIndex[i] + 0.5f) + 1;//ditto, plus the one-after end convention
                if (startIndex[i] < 0) startIndex[i] = 0;//keep it inside the volume boundaries
                if (endIndex[i] > myDims[i]) endIndex[i] = myDims[i];
            }
            int64_t ijk[3];
            for (ijk[0] = startIndex[0]; ijk[0] < endIndex[0]; ++ijk[0])
            {
                for (ijk[1] = startIndex[1]; ijk[1] < endIndex[1]; ++ijk[1])
                {
                    for (ijk[2] = startIndex[2]; ijk[2] < endIndex[2]; ++ijk[2])
                    {
                        if (roiFrame == NULL || roiFrame[myVolSpace.getIndex(ijk)] > 0.0f)
                        {
                            tempf = computeVoxelFraction(myVolSpace, ijk, myPoly, numDivisions, ivec, jvec, kvec);
                            if (tempf != 0.0f)
                            {
                                myWeightsOut[node].push_back(VoxelWeight(tempf, ijk));
                            }
                        }
                    }
                }
            }
            if ((int)myWeightsOut[node].size() > maxVoxelCount)
            {//capacity() would use more memory
                maxVoxelCount = myWeightsOut[node].size();
            }
        }
    }
}
