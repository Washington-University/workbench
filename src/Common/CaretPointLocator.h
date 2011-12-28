#ifndef __CARET_POINT_LOCATOR_H__
#define __CARET_POINT_LOCATOR_H__
#include "CaretAssertion.h"

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

#include "OctTree.h"
#include "Vector3D.h"
#include <vector>

namespace caret {
    
    struct LocatorInfo
    {
        int32_t node, whichSet;
        Vector3D coords;
    };
    
    class CaretPointLocator
    {
        struct Point
        {
            Vector3D m_point;
            int32_t m_index, m_mySet;
            Point(const float point[3], const int32_t index, const int32_t mySet)
            {
                m_point = point;
                m_index = index;
                m_mySet = mySet;
            }
        };
        Oct<LeafVector<Point> >* m_tree;
        int32_t m_nextSetIndex;
        std::vector<int32_t> m_unusedIndexes;
        void addPoint(Oct<LeafVector<Point> >* thisOct, const float point[3], const int32_t index, const int32_t pointSet);
        int32_t newIndex();
        static const int NUM_POINTS_SPLIT = 100;
        void removeSetHelper(Oct<LeafVector<Point> >* thisOct, const int32_t thisSet);
    public:
        ///make an empty point locator with given bounding box (bounding box can expand later, but may be less efficient
        CaretPointLocator(const float minBounds[3], const float maxBounds[3]);
        ///make a point locator with the bounding box of this point set, and use this point set as set #0
        CaretPointLocator(const float* coordsIn, const int32_t numCoords);
        ///add a point set, SAVE THE RETURN VALUE because it is how you identify which point set found points belong to
        int32_t addPointSet(const float* coordsIn, const int32_t numCoords);
        ///remove a point set by its set number
        void removePointSet(const int32_t whichSet);
        ///returns the index of the closest point, and optionally which point set and the coords
        int32_t closestPoint(const float target[3], LocatorInfo* infoOut = NULL) const;
        int32_t closestPointLimited(const float target[3], float maxDist, LocatorInfo* infoOut = NULL) const;
    };
}

#endif //__CARET_POINT_LOCATOR_H__
