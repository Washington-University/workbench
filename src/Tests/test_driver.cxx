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

//test driver for trying CTest

#include <QApplication>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "TestInterface.h"
#include "SessionManager.h"
#include "CaretHttpManager.h"

//tests
#include "HttpTest.h"
#include "NiftiTest.h"
#include "NiftiMatrixTest.h"
#include "ProgressTest.h"
#include "TimerTest.h"
#include "VolumeFileTest.h"
#include "XnatTest.h"

using namespace std;
using namespace caret;

void freeTestList(vector<TestInterface*>& mylist)
{
    for (int i = 0; i < (int)mylist.size(); ++i)
    {
        delete mylist[i];
    }
}

int main(int argc, char** argv)
{
    {
        QApplication myApp(argc, argv, false);
        SessionManager::createSessionManager();
        vector<TestInterface*> mytests;
        mytests.push_back(new HttpTest("http"));
        mytests.push_back(new NiftiFileTest("niftifile"));
        mytests.push_back(new NiftiHeaderTest("niftiheader"));
        mytests.push_back(new NiftiMatrixTest("niftimatrix"));
        mytests.push_back(new ProgressTest("progress"));
        mytests.push_back(new TimerTest("timer"));
        mytests.push_back(new VolumeFileTest("volumefile"));
        mytests.push_back(new XnatTest("xnat"));
        if (argc < 2)
        {
            cout << "No test specified, please specify one of the following:" << endl;
            for (int i = 0; i < (int)mytests.size(); ++i)
            {
                cout << mytests[i]->getIdentifier() << endl;
            }
            freeTestList(mytests);
            return 1;//no test specified, fail
        }
        int failCount = 0;
        for (int i = 1; i < argc; ++i)
        {
            for (int j = 0; j < (int)mytests.size(); ++j)
            {
                if (mytests[j]->getIdentifier() == AString(argv[i]) || "all" == AString(argv[i]))
                {
                    try
                    {
                        mytests[j]->execute();
                    } catch (CaretException& e) {
                        ++failCount;
                        cout << "Test " << mytests[j]->getIdentifier() << " failed, exception: " << e.whatString() << endl;
                        continue;//skip trying failed() and getFailMessage()
                    }
                    if (mytests[j]->failed())
                    {
                        ++failCount;
                        cout << "Test " << mytests[j]->getIdentifier() << " failed: " << mytests[j]->getFailMessage() << endl;
                    }
                }
            }
        }
        freeTestList(mytests);
        if (failCount != 0)
        {
            cout << "Total of " << failCount << " tests failed!" << endl;
            return 1;
        }
        SessionManager::deleteSessionManager();
        CaretHttpManager::deleteHttpManager();
        myApp.processEvents();
    }
    CaretObject::printListOfObjectsNotDeleted(true);
    return 0;
}
