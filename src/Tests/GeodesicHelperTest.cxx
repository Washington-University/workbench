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
#include "GeodesicHelperTest.h"

#include "GeodesicHelper.h"
#include "SurfaceFile.h"

#include <cstdlib>

using namespace caret;
using namespace std;

GeodesicHelperTest::GeodesicHelperTest(const AString& identifier): TestInterface(identifier)
{
}

namespace
{
    void checkNodeLists(GeodesicHelperTest* theTest, const AString& condition, const vector<int32_t>& first, const vector<int32_t>& second)
    {
        if (first.size() != second.size())
        {
            theTest->setFailed(condition + ", found different size node lists");
            return;
        }
        for (size_t i = 0; i < first.size(); ++i)
        {
            if (first[i] != second[i])
            {
                theTest->setFailed(condition + ", found different node at position " + AString::number(i) + " of " + AString::number(first.size()));
                return;
            }
        }
    }
}

void GeodesicHelperTest::execute()
{
    SurfaceFile mySurf;
    mySurf.readFile(m_default_path + "/gifti/Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL_FLIRT.clean.73730.surf.gii");
    CaretPointer<GeodesicHelper> normalHelp = mySurf.getGeodesicHelper();
    vector<float> areas;
    mySurf.computeNodeAreas(areas);
    int numNodes = (int)areas.size();
    for (int i = 0; i < numNodes; ++i)
    {
        areas[i] *= 0.25f;//use a power of 4 so as to not lose any precision (won't be denormals or max exponent, but we do take the square root), so answers should be exactly the same
    }//WARNING: line-following will not give same answer with corrected areas, because the line following penalty is euclidean distance
    CaretPointer<GeodesicHelperBase> quarterHelpBase(new GeodesicHelperBase(&mySurf, areas.data()));
    CaretPointer<GeodesicHelper> quarterHelp(new GeodesicHelper(quarterHelpBase));
    for (int i = 0; i < numNodes; ++i)
    {
        areas[i] *= 16.0f;//ditto
    }
    CaretPointer<GeodesicHelperBase> quadHelpBase(new GeodesicHelperBase(&mySurf, areas.data()));
    CaretPointer<GeodesicHelper> quadHelp(new GeodesicHelper(quadHelpBase));
    vector<float> followData(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        followData[i] = 1.0f + ((float)rand()) / RAND_MAX;
    }
    const int TEST_SAMPLES = 10;
    vector<float> distsNorm, distsQuarter, distsQuad;
    vector<int32_t> nodesNorm, nodesQuarter, nodesQuad;
    for (int i = 0; !failed() && i < TEST_SAMPLES; ++i)
    {
        int32_t startNode = rand() % numNodes;
        const float MAX_GEO_DIST = 20.0f;
        normalHelp->getNodesToGeoDist(startNode, MAX_GEO_DIST, nodesNorm, distsNorm);
        quarterHelp->getNodesToGeoDist(startNode, MAX_GEO_DIST * 0.5f, nodesQuarter, distsQuarter);
        quadHelp->getNodesToGeoDist(startNode, MAX_GEO_DIST * 2.0f, nodesQuad, distsQuad);
        checkNodeLists(this, "Comparing normal to quarter areas, getNodesToGeoDist", nodesNorm, nodesQuarter);
        checkNodeLists(this, "Comparing normal to quad areas, getNodesToGeoDist", nodesNorm, nodesQuad);
        
        int32_t endNode = rand() % numNodes;
        normalHelp->getPathFollowingData(startNode, endNode, followData.data(), nodesNorm, distsNorm);
        quarterHelp->getPathFollowingData(startNode, endNode, followData.data(), nodesQuarter, distsQuarter);
        quadHelp->getPathFollowingData(startNode, endNode, followData.data(), nodesQuad, distsQuad);
        checkNodeLists(this, "Comparing normal to quarter areas, getPathFollowingData", nodesNorm, nodesQuarter);
        checkNodeLists(this, "Comparing normal to quad areas, getPathFollowingData", nodesNorm, nodesQuad);
    }
}
