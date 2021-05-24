#ifndef __ELAPSED_TIMER__H_
#define __ELAPSED_TIMER__H_

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

#ifdef CARET_OS_WINDOWS

#include "windows.h"

#else

#include <sys/time.h>

#endif

#include "CaretObject.h"


namespace caret {

    //getTimeOfDay() isn't cross-platform, so use some ifdefs for windows
#ifdef CARET_OS_WINDOWS
   struct MyTimeStore
   {
      uint64_t m_tickCount;//can store return from GetTickCount64() which doesn't reset at 49 days, but is vista and above only
   };//also useful for detecting and correcting for a wrap, can just add (uint64_t)1<<32
#else
   struct MyTimeStore
   {
      struct timeval m_timeVal;
   };
#endif
    /// An elapsed timer
    class ElapsedTimer : public CaretObject {
        
    public:
        ElapsedTimer();
        
        virtual ~ElapsedTimer();
        
        void start();
        
        void reset();
        
        double getElapsedTimeSeconds() const;
        
        double getElapsedTimeMilliseconds() const;
        
        void printSeconds(const AString& message);
        
        bool isStarted() const;
        
    private:
        ElapsedTimer(const ElapsedTimer&);

        ElapsedTimer& operator=(const ElapsedTimer&);
        
    public:
        virtual AString toString() const;
        
    private:
        MyTimeStore m_startTime;
        
        bool m_started;
    };
    
#ifdef __ELAPSED_TIMER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ELAPSED_TIMER_DECLARE__

} // namespace
#endif  //__ELAPSED_TIMER__H_
