#ifndef __EVENT_GET_MODEL_TO_DRAW_FOR_WINDOW_H__
#define __EVENT_GET_MODEL_TO_DRAW_FOR_WINDOW_H__

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


#include "Event.h"

namespace caret {

    class ModelDisplayController;
    
    /// Event that gets model and information for drawing a window.
    class EventGetModelToDrawForWindow : public Event {
        
    public:
        EventGetModelToDrawForWindow(const int32_t windowIndex);
        
        virtual ~EventGetModelToDrawForWindow();
        
        int32_t getWindowIndex() const;
        
        int32_t getWindowTabIndex() const;
        
        ModelDisplayController* getModelDisplayController();
        
        void setWindowTabIndex(const int32_t windowTabIndex);
        
        void setModelDisplayController(ModelDisplayController* modelDisplayController);
        
    private:
        EventGetModelToDrawForWindow(const EventGetModelToDrawForWindow&);
        
        EventGetModelToDrawForWindow& operator=(const EventGetModelToDrawForWindow&);
        
        /** index of window for update */
        int32_t windowIndex;
        
        /** Model Display Controller for drawing */
        ModelDisplayController* modelDisplayController;
        
        /** Window tab index */
        int32_t windowTabIndex;
    };

} // namespace

#endif // __EVENT_GET_MODEL_TO_DRAW_FOR_WINDOW_H__
