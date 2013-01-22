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
#include "LookupTest.h"

#include "CaretCompactLookup.h"

#include <cstdlib>
#include <ctime>

using namespace caret;
using namespace std;

LookupTest::LookupTest(const AString& identifier) : TestInterface(identifier)
{
}

void LookupTest::execute()
{
    srand(time(NULL));
    const int LOOKUP_SIZE = 500;
    const int LOOKUP_START = -200;
    const int NUM_INSERT = 400;
    CaretCompactLookup<int> myLookup;
    vector<int> checkLookup(LOOKUP_SIZE, -1);
    for (int i = 0; i < NUM_INSERT; ++i)
    {
        int position = rand() % LOOKUP_SIZE;
        int value = rand();
        checkLookup[position] = value;
        myLookup[position + LOOKUP_START] = value;
    }
    const CaretCompactLookup<int>& constRef = myLookup;
    for (int i = 0; i < LOOKUP_SIZE; ++i)
    {
        CaretCompactLookup<int>::const_iterator iter = constRef.find(i + LOOKUP_START);
        if (iter == constRef.end())
        {
            if (checkLookup[i] != -1) setFailed("lost value for key " + AString::number(i + LOOKUP_START));
        } else {
            if (checkLookup[i] == -1) setFailed("created unset value for key " + AString::number(i + LOOKUP_START));
            if (*iter != checkLookup[i]) setFailed("value corrupted for key " + AString::number(i + LOOKUP_START));
        }
    }
    for (int i = 0; i < LOOKUP_SIZE; ++i)
    {
        CaretCompactLookup<int>::iterator iter = myLookup.find(i + LOOKUP_START);
        if (iter == myLookup.end())
        {
            if (checkLookup[i] != -1) setFailed("lost value for key " + AString::number(i + LOOKUP_START));
        } else {
            if (checkLookup[i] == -1) setFailed("created unset value for key " + AString::number(i + LOOKUP_START));
            if (*iter != checkLookup[i]) setFailed("value corrupted for key " + AString::number(i + LOOKUP_START));
        }
    }
}
