#ifndef __BORDER__H_
#define __BORDER__H_

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


#include "CaretObjectTracksModification.h"

namespace caret {

    class SurfaceProjectedItem;
    
    class Border : public CaretObjectTracksModification {
        
    public:
        Border();
        
        virtual ~Border();
        
        Border(const Border& obj);

        Border& operator=(const Border& obj);
        
        virtual AString toString() const;
        
        void clear();
        
        AString getName() const;
        
        void setName(const AString& name);
        
        int32_t getNumberOfPoints() const;
        
        const SurfaceProjectedItem* getPoint(const int32_t indx) const;
        
        SurfaceProjectedItem* getPoint(const int32_t indx);
        
        void addPoint(SurfaceProjectedItem* point);
        
        void removePoint(const int32_t indx);
        
        void removeLastPoint();
        
    private:
        void copyHelperBorder(const Border& obj);
        
        AString name;
        
        std::vector<SurfaceProjectedItem*> points;
    };
    
#ifdef __BORDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BORDER_DECLARE__

} // namespace
#endif  //__BORDER__H_
