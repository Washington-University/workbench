#ifndef __CHART_POINT_H__
#define __CHART_POINT_H__

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


#include "CaretObject.h"



namespace caret {

    class ChartPoint : public CaretObject {
        
    public:
        ChartPoint(const float x,
                   const float y);
        
        virtual ~ChartPoint();
        
        ChartPoint(const ChartPoint& obj);

        ChartPoint& operator=(const ChartPoint& obj);
        
        const float* getXY() const;
        
        float getX() const;
        
        float getY() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperChartPoint(const ChartPoint& obj);

        float m_xyz[3];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_POINT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_POINT_DECLARE__

} // namespace
#endif  //__CHART_POINT_H__
