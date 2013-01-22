#ifndef __USER_VIEW__H_
#define __USER_VIEW__H_

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

    class UserView : public CaretObject {
        
    public:
        UserView();
        
        virtual ~UserView();
        
        UserView(const UserView&);
        
        UserView& operator=(const UserView&);
        
        AString getName() const;
        
        void getTranslation(float translation[3]) const;
        
        void getRotation(float rotation[4][4]) const;
        
        float getScaling() const;
        
        void setName(const AString& name);
        
        void setTranslation(const float translation[3]);
        
        void setRotation(const float rotation[4][4]);
        
        void setScaling(const float scaling);
        
        AString getAsString() const;
        
        bool setFromString(const AString& s);
        
        void setToIdentity();
        
        bool operator<(const UserView& view) const;
        
    public:
        virtual AString toString() const;
        
    private:
        void copyHelper(const UserView& userView);
        
        AString name;
        
        float translation[3];
        
        float rotation[4][4];
        
        float scaling;
    };
    
#ifdef __USER_VIEW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_VIEW_DECLARE__

} // namespace
#endif  //__USER_VIEW__H_
