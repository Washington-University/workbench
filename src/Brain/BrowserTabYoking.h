#ifndef __BROWSER_TAB_YOKING__H_
#define __BROWSER_TAB_YOKING__H_

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
#include "YokingTypeEnum.h"

namespace caret {

    class BrowserTabContent;
    class ModelDisplayControllerYokingGroup;
    
    /**
     * \class BrowserTabYoking 
     * \brief Controls the yoking status for a browser tab.
     *
     * Controls the yoking status for a browser tab.
     */
    
    class BrowserTabYoking : public CaretObject {

    public:
        BrowserTabYoking(BrowserTabContent* parentBrowserTabContent,
                         ModelDisplayControllerYokingGroup* selectedYokingGroup);
        
        virtual ~BrowserTabYoking();
        
        BrowserTabContent* getParentBrowserTabContent();
        
        ModelDisplayControllerYokingGroup* getSelectedYokingGroup();
        
        void setSelectedYokingGroup(ModelDisplayControllerYokingGroup* selectedYokingGroup);
        
    private:
        BrowserTabYoking(const BrowserTabYoking&);

        BrowserTabYoking& operator=(const BrowserTabYoking&);
        
    public:
        virtual AString toString() const;
        
    private:
        BrowserTabContent* parentBrowserTabContent;
        
        ModelDisplayControllerYokingGroup* selectedYokingGroup;
    };
    
#ifdef __BROWSER_TAB_YOKING_DECLARE__
#endif // __BROWSER_TAB_YOKING_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_YOKING__H_
