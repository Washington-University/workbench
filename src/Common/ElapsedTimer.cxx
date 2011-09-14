
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#include <sys/time.h>

#define __ELAPSED_TIMER_DECLARE__
#include "ElapsedTimer.h"
#undef __ELAPSED_TIMER_DECLARE__

#include "CaretAssert.h"

using namespace caret;


/**
 * Constructor.
 */
ElapsedTimer::ElapsedTimer()
: CaretObject()
{
    this->startTimeSeconds = -1;
    this->startTimeMicroseconds = -1;
}

/**
 * Destructor.
 */
ElapsedTimer::~ElapsedTimer()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ElapsedTimer::toString() const
{
    return "ElapsedTimer";
}

/**
 * Start the timer.
 */
void 
ElapsedTimer::start()
{
    struct timeval timeVal;
    
    gettimeofday(&timeVal, NULL);
    
    this->startTimeSeconds = timeVal.tv_sec;
    this->startTimeMicroseconds = timeVal.tv_usec;    
}

/**
 * Get the elapsed time in seconds.
 *
 * @return Elapsed time in seconds.
 */
double 
ElapsedTimer::getElapsedTimeSeconds() const
{
    CaretAssertMessage(this->startTimeSeconds, "Timer has not been started");
    
    struct timeval timeVal;
    
    gettimeofday(&timeVal, NULL);
    
    const double diffSeconds      = timeVal.tv_sec - this->startTimeSeconds;
    double diffMicroseconds = timeVal.tv_usec - this->startTimeMicroseconds;
    if (diffMicroseconds < 0) {
        diffMicroseconds += 1000000;
    }
    
    const double diffTime = diffSeconds + (diffMicroseconds / 1000000.0);
    return diffTime;
}

/**
 * Get the elapsed time in milliseconds.
 *
 * @return Elapsed time in milliseconds.
 */
double 
ElapsedTimer::getElapsedTimeMilliseconds() const
{
    const double diffTimeMicro = this->getElapsedTimeSeconds() * 1000.0;
    return diffTimeMicro;
}

