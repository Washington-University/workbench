#ifndef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
#define __BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_

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


#include "CaretObject.h"

namespace caret {

    class BrowserTabContent;
    
    class BrainOpenGLViewportContent : public CaretObject {
        
    public:
        BrainOpenGLViewportContent(const int viewport[4],
                         BrowserTabContent* browserTabContent);
        ~BrainOpenGLViewportContent();
        
        void getViewport(int viewport[4]) const;
        BrowserTabContent* getBrowserTabContent();
        
    private:
        int viewport[4];
        BrowserTabContent* browserTabContent;
        
        BrainOpenGLViewportContent(const BrainOpenGLViewportContent&);

        BrainOpenGLViewportContent& operator=(const BrainOpenGLViewportContent&);
        
    public:
        virtual AString toString() const;
    };
    
#ifdef __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_VIEWPORT_CONTENT_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VIEWPORT_CONTENT__H_
