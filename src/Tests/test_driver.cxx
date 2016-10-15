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

//program for running tests

#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "TestInterface.h"
#include "SessionManager.h"
#include "CaretHttpManager.h"
#include "CaretCommandLine.h"
#include "CaretException.h"

//tests
#include "CiftiFileTest.h"
#include "DotTest.h"
#include "GeodesicHelperTest.h"
#include "HttpTest.h"
#include "HeapTest.h"
#include "LookupTest.h"
#include "MathExpressionTest.h"
#include "NiftiTest.h"
#include "PointerTest.h"
#include "ProgressTest.h"
#include "QuatTest.h"
#include "StatisticsTest.h"
#include "TimerTest.h"
#include "TopologyHelperTest.h"
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
    srand(time(NULL));
    {
        QCoreApplication myApp(argc, argv);
        caret_global_commandLine_init(argc, argv);
        SessionManager::createSessionManager(ApplicationTypeEnum::APPLICATION_TYPE_COMMAND_LINE);
        vector<TestInterface*> mytests;
        mytests.push_back(new CiftiFileTest("ciftifile"));
        mytests.push_back(new DotTest("dotsimd"));
        mytests.push_back(new GeodesicHelperTest("geohelp"));
        mytests.push_back(new HeapTest("heap"));
        mytests.push_back(new HttpTest("http"));
        mytests.push_back(new LookupTest("lookup"));
        mytests.push_back(new MathExpressionTest("mathexpression"));
        mytests.push_back(new NiftiFileTest("niftifile"));
        mytests.push_back(new NiftiHeaderTest("niftiheader"));
        mytests.push_back(new PointerTest("pointer"));
        mytests.push_back(new ProgressTest("progress"));
        mytests.push_back(new QuatTest("quaternion"));
        mytests.push_back(new StatisticsTest("statistics"));
        mytests.push_back(new TimerTest("timer"));
        mytests.push_back(new TopologyHelperTest("topohelp"));
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
