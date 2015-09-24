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

#include "Border.h"
#include "BorderLengthHelper.h"
#include "CaretAssert.h"
#include "CaretException.h"
#include "GeodesicHelper.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "Vector3D.h"

#include <cmath>
#include <set>

using namespace caret;
using namespace std;

BorderLengthHelper::BorderLengthHelper(const SurfaceFile* mySurf, const float* correctedAreas)
{
    m_surface = mySurf;
    m_corrAreas = correctedAreas;
    mySurf->computeNodeAreas(m_currAreas);
    if (correctedAreas == NULL)
    {
        m_geoHelp = mySurf->getGeodesicHelper();
    } else {
        CaretPointer<GeodesicHelperBase> myBase(new GeodesicHelperBase(mySurf, correctedAreas));
        m_geoHelp.grabNew(new GeodesicHelper(myBase));
    }
}

namespace
{
    float segmentLength(const SurfaceProjectedItem* start, const SurfaceProjectedItem* end, GeodesicHelper* myGeoHelp, const SurfaceFile* mySurf, const float* correctedAreas, const float* currAreas)
    {
        const SurfaceProjectionBarycentric* startBary = start->getBarycentricProjection(), *endBary = end->getBarycentricProjection();
        CaretAssert(startBary->isValid() && endBary->isValid());
        const int32_t* startNodes = startBary->getTriangleNodes(), *endNodes = endBary->getTriangleNodes();
        const float* nodeCoords = mySurf->getCoordinateData();
        Vector3D startPos, endPos;
        startBary->unprojectToSurface(*mySurf, startPos, 0.0f, true);//in case something silly is in the signed distance
        endBary->unprojectToSurface(*mySurf, endPos, 0.0f, true);
        int numSame = 0;
        int sameVerts[3] = {-1, -1, -1};
        set<int32_t> startSet(startNodes, startNodes + 3), endSet(endNodes, endNodes + 3);//use sets in case a strange border file repeats vertices in a projection
        for (set<int32_t>::iterator iter = startSet.begin(); iter != startSet.end(); ++iter)
        {
            if (endSet.find(*iter) != endSet.end())
            {
                sameVerts[numSame] = *iter;
                ++numSame;
            }
        }
        if (numSame == (int)startSet.size() || numSame == (int)endSet.size())
        {//if vertices are repeated, and all of one set match, count it as same triangle
            numSame = 3;
        }
        switch (numSame)
        {
            case 3://same triangle
                if (correctedAreas != NULL)//apply area correction approximation - currently, just average the vertex areas and sqrt
                {//we could integrate along the path, but expansion should be fairly smooth anyway
                    float correctionFactor = sqrt(correctedAreas[startNodes[0]] + correctedAreas[startNodes[1]] + correctedAreas[startNodes[2]]) /
                                             sqrt(currAreas[startNodes[0]] + currAreas[startNodes[1]] + currAreas[startNodes[2]]);
                    return (startPos - endPos).length() * correctionFactor;
                } else {
                    return (startPos - endPos).length();
                }
            case 2://use unfolding of triangle
            {
                Vector3D neigh1Coord = nodeCoords + sameVerts[0] * 3;//neigh1 and neigh2 are the shared vertices
                Vector3D neigh2Coord = nodeCoords + sameVerts[1] * 3;
                float abmag, efmag;
                Vector3D abhat = (neigh2Coord - neigh1Coord).normal(&abmag);//a is neigh1, b is neigh2, b - a = (vector)ab
                Vector3D ac = endPos - neigh1Coord;//c is endpoint, c - a = (vector)ac
                Vector3D ad = abhat * abhat.dot(ac);//d is the point on the shared edge that endpoint (c) is closest to
                Vector3D d = neigh1Coord + ad;//this way we can "unfold" the triangles by projecting the distance of cd, from point d, along the unit vector of the startpoint to closest point on shared edge
                Vector3D ea = neigh1Coord - startPos;//e is the startpoint, a - e = (vector)ea
                Vector3D tempvec = abhat * abhat.dot(ea);//find vector fa, f being the point on shared edge closest to e, the startpoint
                Vector3D efhat = (ea - tempvec).normal(&efmag);//and subtract it to obtain only the perpendicular, normalize to get unit vector
                float cdmag = (d - endPos).length();//get the length from shared edge to endpoint
                Vector3D g = d + efhat * cdmag;//get point g, the unfolded position of endpoint
                Vector3D eg = g - startPos;//this is the vector from base (e) to endpoint after unfolding (g), this is our distance, as long as the tetralateral is convex
                float tempf = efmag / (efmag + cdmag);//now we need to check that the path stays inside the tetralateral (ie, that it is convex)
                Vector3D eh = eg * tempf;//this is a vector from e (startpoint) to the point on the shared edge that the full path (eg) crosses
                Vector3D ah = eh - ea;//eh - ea = eh + ae = ae + eh = ah, vector from neigh1 to the point on shared edge the path goes through
                tempf = ah.dot(abhat);//get the component along ab so we can test that it is positive and less than |ab|
                if (tempf <= 0.0f)
                {
                    tempf = 0.0f;
                } else if (tempf >= abmag) {//tetralateral is concave or triangular (degenerate), so clamp the point through the shared edge to an endpoint
                    tempf = abmag;
                }
                Vector3D crosspoint = neigh1Coord + abhat * tempf;
                if (correctedAreas != NULL)//apply area correction approximation - currently, just average the vertex areas on the shared edge and sqrt
                {//we could compute the amount of the path in each triangle exactly, and integrate along the path, but expansion should be fairly smooth anyway
                    float correctionFactor = sqrt(correctedAreas[sameVerts[0]] + correctedAreas[sameVerts[1]]) / sqrt(currAreas[sameVerts[0]] + currAreas[sameVerts[1]]);
                    return ((crosspoint - startPos).length() + (endPos - crosspoint).length()) * correctionFactor;
                } else {
                    return ((crosspoint - startPos).length() + (endPos - crosspoint).length());
                }
            }
            case 1://brute force tests of to vertex, geodesic, to end
            case 0:
            {
                float bestLength = -1.0f;
                for (int i = 0; i < 3; ++i)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        vector<int32_t> pathNodes;
                        vector<float> pathDists;
                        myGeoHelp->getPathToNode(startNodes[i], endNodes[j], pathNodes, pathDists, true);//we want the distance, not the path, so use the more accurate distances
                        float startLength, endLength;
                        if (correctedAreas != NULL)//apply area correction approximation - currently, just average the vertex areas and sqrt
                        {//we could integrate along the path, but expansion should be fairly smooth anyway
                            float startCorrFactor = sqrt(correctedAreas[startNodes[0]] + correctedAreas[startNodes[1]] + correctedAreas[startNodes[2]]) /
                                                    sqrt(currAreas[startNodes[0]] + currAreas[startNodes[1]] + currAreas[startNodes[2]]);
                            float endCorrFactor = sqrt(correctedAreas[endNodes[0]] + correctedAreas[endNodes[1]] + correctedAreas[endNodes[2]]) /
                                                    sqrt(currAreas[endNodes[0]] + currAreas[endNodes[1]] + currAreas[endNodes[2]]);
                            startLength = (Vector3D(nodeCoords + startNodes[i] * 3) - startPos).length() * startCorrFactor;
                            endLength = (endPos - Vector3D(nodeCoords + endNodes[j] * 3)).length() * endCorrFactor;
                        } else {
                            startLength = (Vector3D(nodeCoords + startNodes[i] * 3) - startPos).length();
                            endLength = (endPos - Vector3D(nodeCoords + endNodes[j] * 3)).length();
                        }
                        float totalLength = pathDists.back() + startLength + endLength;
                        if (totalLength < bestLength || bestLength == -1.0f)
                        {
                            bestLength = totalLength;
                        }
                    }
                }
                return bestLength;
            }
            default://should never be other values
                CaretAssert(false);
                throw CaretException("internal error, tell the developers what you just tried to do");
        }
    }
}

float BorderLengthHelper::length(const Border* myBorder)
{
    int numPoints = myBorder->getNumberOfPoints();
    double totalLength = 0.0f;
    for (int i = 1; i < numPoints; ++i)
    {
        totalLength += segmentLength(myBorder->getPoint(i - 1), myBorder->getPoint(i), m_geoHelp, m_surface, m_corrAreas, m_currAreas.data());
    }
    if (myBorder->isClosed() && numPoints > 1)
    {
        totalLength += segmentLength(myBorder->getPoint(numPoints - 1), myBorder->getPoint(0), m_geoHelp, m_surface, m_corrAreas, m_currAreas.data());
    }
    return totalLength;
}

BorderLengthHelper::~BorderLengthHelper()
{//for CaretPointer destructor to not be instantiated on a forward declaration
}
