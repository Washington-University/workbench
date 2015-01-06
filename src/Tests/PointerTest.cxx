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
#include "PointerTest.h"
//#include <iostream>

#include "CaretPointer.h"
#include "CaretMutex.h"
#include "CaretOMP.h"
#include <QMutex>

using namespace caret;
using namespace std;

struct DelTestObj
{
    int* m_pointer;
    CaretMutex m_mutex;
    DelTestObj(int* pointerIn)
    {
        m_pointer = pointerIn;
        *m_pointer = 0;
    }
    ~DelTestObj()
    {
        CaretMutexLocker locked(&m_mutex);//so we don't have any race conditions in the test object
        ++(*m_pointer);
    }
};

PointerTest::PointerTest(const AString& identifier) : TestInterface(identifier)
{
}

void PointerTest::execute()
{
    const int ITERATIONS = 500;
    CaretMutex messageMutex;//QString does NOT behave well under threading
    int deltrack1, deltrack2, deltrack3;
    {//scope to control when things get destroyed
        CaretPointer<DelTestObj> myobj1(new DelTestObj(&deltrack1)), myobj2(new DelTestObj(&deltrack2)), myobj3(new DelTestObj(&deltrack3));
        {
            CaretPointer<const DelTestObj> myconstobj = myobj1;//check that pointer to const works, too
        }
        //cout << "pointers: " << myobj1.getPointer() << "\t" << myobj2.getPointer() << "\t" << myobj3.getPointer() << endl;
#pragma omp CARET_PAR
        {
            CaretPointer<DelTestObj> myScratch1, myScratch2, myScratch3;
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < ITERATIONS; ++i)
            {
                myobj1 = myobj1;//ensure self assignment is detected and handled
                
                if (myobj1 == NULL)
                {
                    CaretMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after self assignment, iteration " + AString::number(i));
                }
                
                myScratch1 = myobj1;//simple get and put back
                myobj1 = myScratch1;
                
                if (myScratch1 == NULL)
                {
                    CaretMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after get/replace, iteration " + AString::number(i));
                }
                
                myScratch1 = myobj2;//get, move, put
                myScratch2 = myScratch1;
                myobj2 = myScratch2;
                
                if (myScratch1 == NULL)
                {
                    CaretMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after get/move/replace, iteration " + AString::number(i));
                }
                
                myScratch2 = myobj2;
                myScratch3 = myobj3;
                if (myScratch2 != myScratch3)
                {
                    myobj3 = myScratch2;
                    myobj3 = myScratch3;
                }
                
                if (myScratch3 == NULL)
                {
                    CaretMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after replace shared, iteration " + AString::number(i));
                }
                
                if (deltrack1 != 0 || deltrack2 != 0 || deltrack3 != 0)
                {
                    CaretMutexLocker locked(&messageMutex);
                    setFailed("premature deletion of object detected, iteration " + AString::number(i));
                }
            }
        }
    }
    if (deltrack1 == 0 || deltrack2 == 0 || deltrack3 == 0)
    {
        setFailed("object not deleted (parallel)");
    }
    if (deltrack1 != 1 || deltrack2 != 1 || deltrack3 != 1)
    {
        setFailed("object deleted incorrect number of times (parallel)");
    }
    {//repeat with non-synchronized pointers and no parallel code
        CaretPointerNonsync<DelTestObj> myobj1(new DelTestObj(&deltrack1)), myobj2(new DelTestObj(&deltrack2)), myobj3(new DelTestObj(&deltrack3));
        {
            CaretPointerNonsync<const DelTestObj> myconstobj = myobj1;//check that pointer to const works, too
        }
        //cout << "pointers: " << myobj1.getPointer() << "\t" << myobj2.getPointer() << "\t" << myobj3.getPointer() << endl;
        {
            CaretPointerNonsync<DelTestObj> myScratch1, myScratch2, myScratch3;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                myobj1 = myobj1;//ensure self assignment is detected and handled
                
                if (myobj1 == NULL)
                {
                    setFailed("NULL generated, detected after self assignment, iteration " + AString::number(i));
                }
                
                myScratch1 = myobj1;//simple get and put back
                myobj1 = myScratch1;
                
                if (myScratch1 == NULL)
                {
                    setFailed("NULL generated, detected after get/replace, iteration " + AString::number(i));
                }
                
                myScratch1 = myobj2;//get, move, put
                myScratch2 = myScratch1;
                myobj2 = myScratch2;
                
                if (myScratch1 == NULL)
                {
                    setFailed("NULL generated, detected after get/move/replace, iteration " + AString::number(i));
                }
                
                myScratch2 = myobj2;
                myScratch3 = myobj3;
                if (myScratch2 != myScratch3)
                {
                    myobj3 = myScratch2;
                    myobj3 = myScratch3;
                }
                
                if (myScratch3 == NULL)
                {
                    setFailed("NULL generated, detected after replace shared, iteration " + AString::number(i));
                }
                
                if (deltrack1 != 0 || deltrack2 != 0 || deltrack3 != 0)
                {
                    setFailed("premature deletion of object detected, iteration " + AString::number(i));
                }
            }
        }
    }
    if (deltrack1 == 0 || deltrack2 == 0 || deltrack3 == 0)
    {
        setFailed("object not deleted");
    }
    if (deltrack1 != 1 || deltrack2 != 1 || deltrack3 != 1)
    {
        setFailed("object deleted incorrect number of times");
    }
}
