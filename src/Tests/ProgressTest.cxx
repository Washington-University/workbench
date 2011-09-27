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

#include <iostream>

#include "EventManager.h"
#include "ProgressTest.h"

using namespace std;
using namespace caret;

TestAlgorithm::TestAlgorithm(ProgressObject* myproginfo, bool testOver) : AbstractAlgorithm(myproginfo)
{
   m_failed = false;
   const float oddEndWeight = 17.0f;
   LevelProgress myLevel = myproginfo->startLevel(oddEndWeight);//to test strange values
   if (myproginfo->getCurrentProgressFraction() != 0.0f || myproginfo->getCurrentProgressPercent() != 0.0f)
   {
      cout << "initial timer progress is not zero!" << endl;
      m_failed = true;
   }
   AString teststring = AString("some random text");
   myLevel.setTask(teststring);
   if (myproginfo->getTaskDescription() != teststring)
   {
      cout << "task description returns a different string than set" << endl;
      m_failed = true;
   }
   float tooSmall = 0.0005f * oddEndWeight;//progress should NOT update at less than 0.1%, or 0.001 * total
   myLevel.reportProgress(tooSmall);
   if (myproginfo->getCurrentProgressFraction() != 0.0f || myproginfo->getCurrentProgressPercent() != 0.0f)
   {
      cout << "progress update of " << tooSmall / oddEndWeight * 100.0f << "% triggers update!" << endl;
      m_failed = true;
   }
   float someProgressFraction = 0.10f;
   float someProgress = someProgressFraction * oddEndWeight;
   myLevel.reportProgress(someProgress);
   if (myproginfo->getCurrentProgressFraction() < someProgressFraction * 0.95f || myproginfo->getCurrentProgressPercent() < someProgressFraction * 95.0f ||
      myproginfo->getCurrentProgressFraction() > someProgressFraction * 1.05f || myproginfo->getCurrentProgressPercent() > someProgressFraction * 105.0f)
   {
      cout << "progress update of " << someProgress / oddEndWeight * 100.0f << "% returns result of " << myproginfo->getCurrentProgressFraction() << " and " << myproginfo->getCurrentProgressPercent() << "% !" << endl;
      m_failed = true;
   }
   if (testOver)
   {
      myLevel.reportProgress(oddEndWeight * 1.3f);
      if (myproginfo->getCurrentProgressFraction() > 1.0f || myproginfo->getCurrentProgressPercent() > 100.0f)
      {
         cout << "output if weight goes over is " << myproginfo->getCurrentProgressFraction() << " and " << myproginfo->getCurrentProgressPercent() << "% !" << endl;
         m_failed = true;
      }
   }
}

TestAlgorithm2::TestAlgorithm2(ProgressObject* myproginfo): AbstractAlgorithm(myproginfo)
{
   //do nothing, simulate ignoring the object
}

ProgressTest::ProgressTest(const AString& identifier): TestInterface(identifier)
{
}

void ProgressTest::execute()
{
   ProgressObject myprog1(TestAlgorithm::getAlgorithmWeight()), myprog2(TestAlgorithm::getAlgorithmWeight()), myprog3(TestAlgorithm2::getAlgorithmWeight());
   TestAlgorithm myalg(&myprog1, false);
   if (myprog1.getCurrentProgressFraction() != 1.0f || myprog1.getCurrentProgressPercent() != 100.0f)
   {
      setFailed(AString("Finished state reports ") + AString::number(myprog1.getCurrentProgressFraction()) + AString(" and ") + AString::number(myprog1.getCurrentProgressPercent()) + AString("%"));
   }
   if (myalg.m_failed)
   {
      setFailed("Algorithm reported failure internally");
   }
   TestAlgorithm myalg2(&myprog2, true);
   if (myalg2.m_failed)
   {
      setFailed("Algorithm reported failure internally");
   }
   {
      TestAlgorithm2 myalg3(&myprog3);
   }
   if (myprog3.getCurrentProgressFraction() != 1.0f || myprog3.getCurrentProgressPercent() != 100.0f)
   {
      setFailed("ignored progress object does not register as completed upon algorithm destruction");
   }
}
