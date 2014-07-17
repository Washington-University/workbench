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
#include "LookupTest.h"

#include "CaretCompactLookup.h"
#include "CaretCompact3DLookup.h"

#include <cstdlib>

using namespace caret;
using namespace std;

LookupTest::LookupTest(const AString& identifier) : TestInterface(identifier)
{
}

void LookupTest::execute()
{
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


   /* 3D Lookup Testing */
   {
      const int64_t NUM = 3;
      int64_t array[NUM][3] = {
         { 1, 2, 3 },
         { 7, 5, 4 },
         { 5, 9, 1 }
      };

      CaretCompact3DLookup<int64_t> lookup3D;

      for (int64_t i = 0; i < NUM; i++) {
         lookup3D.at(array[i]) = i;
      }
      
      for (int64_t i = 0; i < NUM; i++) {
         const int64_t* value = lookup3D.find(array[i]);
         if (value == NULL) {
	    setFailed("Item " 
                      + AString::number(i)
                      + " was not found.");
         }
         else {
            if (i != *value) {
                setFailed("Item " 
                       + AString::number(i) 
                       + " with IJK ("
                       + AString::fromNumbers(array[i], 3, ", ")
                       + ") when searched with find() has value "
                       + AString::number(*value));
            }
          }
      }      
   }
}
