#ifndef __BROWSER_TAB_CONTENT__H_
#define __BROWSER_TAB_CONTENT__H_

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
#include "SurfaceOverlaySet.h"
#include "YokingTypeEnum.h"

namespace caret {

    class ModelDisplayController;
    
    /// Maintains content in a brower's tab
    class BrowserTabContent : public CaretObject {
        
    public:
        BrowserTabContent(const int32_t tabNumber);
        
        virtual ~BrowserTabContent();
        
        virtual AString toString() const;
        
        AString getName() const;
        
        void setGuiName(const AString& name);
        
        void setUserName(const AString& userName);
        
        ModelDisplayController* getDisplayedModel();
        
        void setDisplayedModel(ModelDisplayController* model);
        
        SurfaceOverlaySet* getSurfaceOverlayAssignment();
        
        int32_t getYokeToTabNumber() const;
        
        void setYokeToTabNumber(const int32_t yokeToTabNumber);
        
        YokingTypeEnum::Enum getYokingType() const;
        
        void setYokingType(const YokingTypeEnum::Enum yokingType);
        
        int32_t getTabNumber() const;
        
    private:
        BrowserTabContent(const BrowserTabContent&);
        
        BrowserTabContent& operator=(const BrowserTabContent&);
        
        /** model displayed in this browser tab */
        ModelDisplayController* displayedModel;
        
        /** Number of this tab */
        int32_t tabNumber;
        
        /** 
         * Name requested by user interface - reflects contents 
         * such as Surface, Volume Slices, etc
         */
        AString guiName;
        
        /**
         * User can set the name of the tab.
         */
        AString userName;
        
        /** Tab number to which this controller is yoked */
        int32_t yokeToTabNumber;
        
        /** Type of yoking */
        YokingTypeEnum::Enum yokingType;
        
        /** The surface overlay assignments */
        SurfaceOverlaySet surfaceOverlayAssignment;
        
    };
    
#ifdef __BROWSER_TAB_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BROWSER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__BROWSER_TAB_CONTENT__H_
