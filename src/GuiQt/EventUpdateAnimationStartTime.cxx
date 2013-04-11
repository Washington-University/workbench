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

#include "EventUpdateAnimationStartTime.h"

using namespace caret;
/**
 * \class caret::EventUpdateAnimationStartTime 
 * \brief Event (deprecated for now) that is used to update the animation start time
 * \addtogroup GuiQt
 */

/**
 * Constructor.
 * @param doRepaint
 *    If true, a repaint is performed and this event does not
 *    return until painting is complete.  If false, an update
 *    is performed which schedules a repaint but does not dictate
 *    when the repaint is performed.
 */
EventUpdateAnimationStartTime::EventUpdateAnimationStartTime()
: Event(EventTypeEnum::EVENT_UPDATE_ANIMATION_START_TIME)
{    
}

/*
 * Destructor.
 */
EventUpdateAnimationStartTime::~EventUpdateAnimationStartTime()
{
    
}

double 
EventUpdateAnimationStartTime::getStartTime()
{
    return this->startTime;
}

void
EventUpdateAnimationStartTime::setStartTime(double time)
{
    this->startTime = time;
}
