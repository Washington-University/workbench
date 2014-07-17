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
#include "TopologyHelperTest.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "TopologyHelperOld.h"

#include <cstdlib>

using namespace caret;
using namespace std;

TopologyHelperTest::TopologyHelperTest(const AString& identifier): TestInterface(identifier)
{
}

void TopologyHelperTest::execute()
{
    SurfaceFile mySurf;
    mySurf.readFile(m_default_path + "/gifti/Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL_FLIRT.clean.73730.surf.gii");
    CaretPointer<TopologyHelper> myNewTopoHelp = mySurf.getTopologyHelper();
    CaretPointer<TopologyHelperOld> myOldTopoHelp(new TopologyHelperOld(&mySurf));
    int numNodes = mySurf.getNumberOfNodes();
    CaretArray<int> myMarkedNew(numNodes, -1), myMarkedOld(numNodes, -1);
    const int TEST_SAMPLES = 5000, TEST_DEPTH = 5;
    for (int i = 0; i < TEST_SAMPLES; ++i)
    {
        int selectedNode = rand() % numNodes;
        vector<int32_t> newNeigh = myNewTopoHelp->getNodeNeighbors(selectedNode);//copy out rather than using const reference so we can reuse them for depth
        vector<int> oldNeigh = myOldTopoHelp->getNodeNeighbors(selectedNode);
        int newSize = (int)newNeigh.size(), oldSize = (int)oldNeigh.size();
        if (newSize != oldSize)
        {
            setFailed("size difference at node " + AString::number(selectedNode) + ", new: " + AString::number(newSize) + ", old: " + AString::number(oldSize));
        }
        for (int i = 0; i < newSize; ++i)
        {
            myMarkedNew[newNeigh[i]] = selectedNode;
        }
        for (int i = 0; i < oldSize; ++i)
        {
            myMarkedOld[oldNeigh[i]] = selectedNode;
            if (myMarkedNew[oldNeigh[i]] != selectedNode)
            {
                setFailed("new topology helper missed neighbor " + AString::number(oldNeigh[i]) + " of node " + AString::number(selectedNode));
            }
        }
        for (int i = 0; i < newSize; ++i)
        {
            if (myMarkedOld[newNeigh[i]] != selectedNode)
            {
                setFailed("new topology helper added neighbor " + AString::number(newNeigh[i]) + " to node " + AString::number(selectedNode));
            }
        }
        myNewTopoHelp->getNodeNeighborsToDepth(selectedNode, TEST_DEPTH, newNeigh);
        myOldTopoHelp->getNodeNeighborsToDepth(selectedNode, TEST_DEPTH, oldNeigh);
        newSize = (int)newNeigh.size();
        oldSize = (int)oldNeigh.size();
        if (newSize != oldSize)
        {
            setFailed("depth " + AString::number(TEST_DEPTH) + " size difference at node " + AString::number(selectedNode) + ", new: " + AString::number(newSize) + ", old: " + AString::number(oldSize));
        }
        for (int i = 0; i < newSize; ++i)
        {
            myMarkedNew[newNeigh[i]] = selectedNode;
        }
        for (int i = 0; i < oldSize; ++i)
        {
            myMarkedOld[oldNeigh[i]] = selectedNode;
            if (myMarkedNew[oldNeigh[i]] != selectedNode)
            {
                setFailed("new topology helper depth " + AString::number(TEST_DEPTH) + " missed neighbor " + AString::number(oldNeigh[i]) + " of node " + AString::number(selectedNode));
            }
        }
        for (int i = 0; i < newSize; ++i)
        {
            if (myMarkedOld[newNeigh[i]] != selectedNode)
            {
                setFailed("new topology helper depth " + AString::number(TEST_DEPTH) + " added neighbor " + AString::number(newNeigh[i]) + " to node " + AString::number(selectedNode));
            }
        }
    }
}
