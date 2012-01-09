#ifndef __DISPLAY_PROPERTIES_INFORMATION__H_
#define __DISPLAY_PROPERTIES_INFORMATION__H_

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


#include "DisplayProperties.h"

namespace caret {

    class DisplayPropertiesInformation : public DisplayProperties {
        
    public:
        DisplayPropertiesInformation(Brain* brain);
        
        virtual ~DisplayPropertiesInformation();
        
        void reset();
        
        void update();
        
        bool isContralateralIdentificationEnabled() const;
        
        void setContralateralIdentificationEnabled(const bool enabled);

    private:
        DisplayPropertiesInformation(const DisplayPropertiesInformation&);

        DisplayPropertiesInformation& operator=(const DisplayPropertiesInformation&);
        
        bool contralateralIdentificationEnabled;

    };
    
#ifdef __DISPLAY_PROPERTIES_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_INFORMATION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_INFORMATION__H_
