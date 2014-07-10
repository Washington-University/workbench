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

#include <vector>

#include "XnatTest.h"
#include "CiftiFile.h"

using namespace caret;
using namespace std;

XnatTest::XnatTest(const AString& identifier) : TestInterface(identifier)
{
}

void XnatTest::execute()
{
    setFailed("no authentication info present"); return;//comment this out to run the test
    AString myUrl("");//test url goes here
    AString user("");//test auth goes here
    AString pass("");
    CiftiFile myXnat;
    myXnat.openURL(myUrl, user, pass);
    vector<float> myData;
    myData.resize(myXnat.getNumberOfColumns());
    myXnat.getRow(myData.data(), 0);
    if (myXnat.getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        setFailed("opened file does not have brain models along column");
    }
    const CiftiBrainModelsMap& myDenseMap = myXnat.getCiftiXML().getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    int64_t myRow = myDenseMap.getIndexForNode(547, StructureEnum::CORTEX_RIGHT);
    if (myRow == -1)
    {
        setFailed("did not find specified node in specified structure");
    }
    myXnat.getRow(myData.data(), myRow);
}
