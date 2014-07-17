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
#include "HeapTest.h"
#include "CaretHeap.h"
#include <cstdlib>

using namespace caret;
using namespace std;

HeapTest::HeapTest(const AString& identifier) : TestInterface(identifier)
{
}

void HeapTest::execute()
{
    CaretMinHeap<int, int> myHeap;
    const int NUM_ELEMS = 500;
    const int NUM_REMOVE = 30;
    int testarray[NUM_ELEMS][2];
    int64_t indexes[NUM_ELEMS];
    bool markarray[NUM_ELEMS];
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        testarray[i][0] = i + 5;//data
        testarray[i][1] = rand() % 1000;//key
        markarray[i] = false;
        indexes[i] = myHeap.push(testarray[i][0], testarray[i][1]);
    }
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        testarray[i][1] = rand() % 1000;//new key
        myHeap.changekey(indexes[i], testarray[i][1]);
    }
    int prevkey = 0, thiskey = 0, thisdata = 0;
    for (int i = 0; i < NUM_REMOVE; ++i)
    {
        int which = rand() % NUM_ELEMS;
        while (markarray[which]) which = rand() % NUM_ELEMS;
        thisdata = myHeap.remove(indexes[which], &thiskey);
        if (thisdata < 5 || thisdata > NUM_ELEMS + 4)
        {
            setFailed("data DESTROYED at " + AString::number(i));
            continue;//don't let it segfault trying to mark it
        }
        if (which != thisdata - 5)
        {
            setFailed("data/index pairing lost at " + AString::number(i));
        }
        if (testarray[thisdata - 5][1] != thiskey)
        {
            setFailed("data scrambled at " + AString::number(i));
        }
        if (markarray[thisdata - 5])
        {
            setFailed("element duplicated at " + AString::number(i));
        }
        markarray[thisdata - 5] = true;
    }
    thiskey = 0;
    for (int i = 0; i < NUM_ELEMS - NUM_REMOVE; ++i)
    {
        prevkey = thiskey;
        thisdata = myHeap.pop(&thiskey);
        if (i != 0 && thiskey < prevkey)
        {
            setFailed("key ordering failed at " + AString::number(i));
        }
        if (thisdata < 5 || thisdata > NUM_ELEMS + 4)
        {
            setFailed("data DESTROYED at " + AString::number(i));
            continue;//don't let it segfault trying to mark it
        }
        if (testarray[thisdata - 5][1] != thiskey)
        {
            setFailed("data scrambled at " + AString::number(i));
        }
        if (markarray[thisdata - 5])
        {
            setFailed("element duplicated at " + AString::number(i));
        }
        markarray[thisdata - 5] = true;
    }
    if (myHeap.size() != 0) setFailed("heap should be empty, but isn't");
    CaretSimpleMinHeap<int, int> mySimpleHeap;
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        testarray[i][1] = rand() % 1000;//key
        markarray[i] = false;
        mySimpleHeap.push(testarray[i][0], testarray[i][1]);
    }
    thiskey = 0;
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        prevkey = thiskey;
        thisdata = mySimpleHeap.pop(&thiskey);
        if (i != 0 && thiskey < prevkey)
        {
            setFailed("key ordering failed at " + AString::number(i));
        }
        if (thisdata < 5 || thisdata > NUM_ELEMS + 4)
        {
            setFailed("data DESTROYED at " + AString::number(i));
            continue;//don't let it segfault trying to mark it
        }
        if (testarray[thisdata - 5][1] != thiskey)
        {
            setFailed("data scrambled at " + AString::number(i));
        }
        if (markarray[thisdata - 5])
        {
            setFailed("element duplicated at " + AString::number(i));
        }
        markarray[thisdata - 5] = true;
    }
    if (mySimpleHeap.size() != 0) setFailed("heap should be empty, but isn't");
    CaretMaxHeap<int, int> myMaxHeap;
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        testarray[i][1] = rand() % 1000;//key
        markarray[i] = false;
        indexes[i] = myMaxHeap.push(testarray[i][0], testarray[i][1]);
    }
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        testarray[i][1] = rand() % 1000;//new key
        myMaxHeap.changekey(indexes[i], testarray[i][1]);
    }
    for (int i = 0; i < NUM_REMOVE; ++i)
    {
        int which = rand() % NUM_ELEMS;
        while (markarray[which]) which = rand() % NUM_ELEMS;
        thisdata = myMaxHeap.remove(indexes[which], &thiskey);
        if (thisdata < 5 || thisdata > NUM_ELEMS + 4)
        {
            setFailed("data DESTROYED at " + AString::number(i));
            continue;//don't let it segfault trying to mark it
        }
        if (which != thisdata - 5)
        {
            setFailed("data/index pairing lost at " + AString::number(i));
        }
        if (testarray[thisdata - 5][1] != thiskey)
        {
            setFailed("data scrambled at " + AString::number(i));
        }
        if (markarray[thisdata - 5])
        {
            setFailed("element duplicated at " + AString::number(i));
        }
        markarray[thisdata - 5] = true;
    }
    thiskey = 0;
    for (int i = 0; i < NUM_ELEMS - NUM_REMOVE; ++i)
    {
        prevkey = thiskey;
        thisdata = myMaxHeap.pop(&thiskey);
        if (i != 0 && thiskey > prevkey)
        {
            setFailed("key ordering failed at " + AString::number(i));
        }
        if (thisdata < 5 || thisdata > NUM_ELEMS + 4)
        {
            setFailed("data DESTROYED at " + AString::number(i));
            continue;//don't let it segfault trying to mark it
        }
        if (testarray[thisdata - 5][1] != thiskey)
        {
            setFailed("data scrambled at " + AString::number(i));
        }
        if (markarray[thisdata - 5])
        {
            setFailed("element duplicated at " + AString::number(i));
        }
        markarray[thisdata - 5] = true;
    }
    if (myMaxHeap.size() != 0) setFailed("heap should be empty, but isn't");
    CaretSimpleMaxHeap<int, int> mySimpleMaxHeap;
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        testarray[i][1] = rand() % 1000;//key
        markarray[i] = false;
        mySimpleMaxHeap.push(testarray[i][0], testarray[i][1]);
    }
    thiskey = 0;
    for (int i = 0; i < NUM_ELEMS; ++i)
    {
        prevkey = thiskey;
        thisdata = mySimpleMaxHeap.pop(&thiskey);
        if (i != 0 && thiskey > prevkey)
        {
            setFailed("key ordering failed at " + AString::number(i));
        }
        if (thisdata < 5 || thisdata > NUM_ELEMS + 4)
        {
            setFailed("data DESTROYED at " + AString::number(i));
            continue;//don't let it segfault trying to mark it
        }
        if (testarray[thisdata - 5][1] != thiskey)
        {
            setFailed("data scrambled at " + AString::number(i));
        }
        if (markarray[thisdata - 5])
        {
            setFailed("element duplicated at " + AString::number(i));
        }
        markarray[thisdata - 5] = true;
    }
    if (mySimpleMaxHeap.size() != 0) setFailed("heap should be empty, but isn't");
}
