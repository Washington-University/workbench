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
#include "PointerTest.h"
#include <iostream>

#include "CaretPointer.h"
#include "CaretMutex.h"
#include "CaretOMP.h"

using namespace caret;
using namespace std;

struct DelTestObj
{
    mutable int* m_pointer;//so that we can test const pointers in the future
    mutable CaretMutex m_mutex;
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
    QMutex messageMutex;//QString does NOT behave well under threading
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
                    QMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after self assignment, iteration " + AString::number(i));
                }
                
                myScratch1 = myobj1;//simple get and put back
                myobj1 = myScratch1;
                
                if (myScratch1 == NULL)
                {
                    QMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after get/replace, iteration " + AString::number(i));
                }
                
                myScratch1 = myobj2;//get, move, put
                myScratch2 = myScratch1;
                myobj2 = myScratch2;
                
                if (myScratch1 == NULL)
                {
                    QMutexLocker locked(&messageMutex);
                    setFailed("NULL generated, detected after get/move/replace, iteration " + AString::number(i));
                }
                
                if (deltrack1 != 0 || deltrack2 != 0 || deltrack3 != 0)
                {
                    QMutexLocker locked(&messageMutex);
                    setFailed("premature deletion of object detected, iteration " + AString::number(i));
                }
                //would need a mutex or critical section in order to swap two globals while ensuring they dont get dropped for valid reasons, defeating the purpose
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
