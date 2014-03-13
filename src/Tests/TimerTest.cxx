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
#include "TimerTest.h"

#include "ElapsedTimer.h"

#ifdef CARET_OS_WINDOWS

#include "windows.h"

#else

#include "unistd.h"

#endif

using namespace caret;

TimerTest::TimerTest(const AString& identifier) : TestInterface(identifier)
{
}

void TimerTest::execute()
{
   ElapsedTimer mytimer;
   mytimer.start();
#ifdef CARET_OS_WINDOWS
   Sleep(1000);//milliseconds on windows
#else
   sleep(1);//seconds on *nix
#endif
   double elapsedMilli = mytimer.getElapsedTimeMilliseconds();
   double elapsed = mytimer.getElapsedTimeSeconds();
   AString mymessage = "Timer innacurate";
   bool failed = false;
   if (elapsed < 0.95)
   {
      failed = true;
      mymessage += ", seconds value too low (";
      mymessage += AString::number(elapsed);
      mymessage += ")";
   }
   if (elapsedMilli < 950.0)
   {
      failed = true;
      mymessage += ", milliseconds value too low (";
      mymessage += AString::number(elapsedMilli);
      mymessage += ")";
   }
   if (elapsed > 1.1)//allow a little extra room to go over
   {
      failed = true;
      mymessage += ", seconds value too high (";
      mymessage += AString::number(elapsed);
      mymessage += ")";
   }
   if (elapsedMilli > 1100.0)
   {
      failed = true;
      mymessage += ", milliseconds value too high (";
      mymessage += AString::number(elapsedMilli);
      mymessage += ")";
   }
   if (failed)
   {
      setFailed(mymessage);
   }
}
