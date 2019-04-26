#ifndef __CARET_POINT_LOCATOR_H__
#define __CARET_POINT_LOCATOR_H__
#include "CaretAssertion.h"

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

#include "CaretMutex.h"
#include "OctTree.h"
#include "Vector3D.h"

#include <set>
#include <vector>

namespace caret {
    
    struct LocatorInfo
    {
        int64_t index;
        int32_t whichSet;
        Vector3D coords;
        LocatorInfo() { index = -1; whichSet = -1; }
        LocatorInfo(const int64_t& indexIn, const int32_t& whichSetIn, const Vector3D& coordsIn) : index(indexIn), whichSet(whichSetIn), coords(coordsIn) { }
        bool operator==(const LocatorInfo& rhs) const { return (index == rhs.index) && (whichSet == rhs.whichSet); }//ignore coords
        bool operator<(const LocatorInfo& rhs) const
        {
            if (whichSet == rhs.whichSet)//expect multi-set usage with pointsInRange to be rare, but still separate by whichSet if sorted
            {
                return index < rhs.index;
            } else {
                return whichSet < rhs.whichSet;
            }
        }
    };
    
    class CaretPointLocator
    {
        struct Point
        {
            Vector3D m_point;
            int64_t m_index;
            int32_t m_mySet;
            Point(const float point[3], const int64_t index, const int32_t mySet)
            {
                m_point = point;
                m_index = index;
                m_mySet = mySet;
            }
        };
        CaretMutex m_modifyMutex;//thread safety, don't let multiple threads modify the point sets at once
        Oct<LeafVector<Point> >* m_tree;
        int32_t m_nextSetIndex;
        std::vector<int32_t> m_unusedIndexes;
        void addPoint(Oct<LeafVector<Point> >* thisOct, const float point[3], const int64_t index, const int32_t pointSet);
        int32_t newIndex();
        static const int NUM_POINTS_SPLIT = 100;
        void removeSetHelper(Oct<LeafVector<Point> >* thisOct, const int32_t thisSet);
        CaretPointLocator();
    public:
        ///make an empty point locator with given bounding box (bounding box can expand later, but may be less efficient
        CaretPointLocator(const float minBounds[3], const float maxBounds[3]);
        ///make a point locator with the bounding box of this point set, and use this point set as set #0
        CaretPointLocator(const float* coordsIn, const int64_t numCoords);
        ///convenience constructor for vectors
        CaretPointLocator(const std::vector<float> coordsIn) : CaretPointLocator(coordsIn.data(), coordsIn.size() / 3) { }
        ///add a point set, SAVE THE RETURN VALUE because it is how you identify which point set found points belong to
        int32_t addPointSet(const float* coordsIn, const int64_t numCoords);
        int32_t addPointSet(const std::vector<float> coordsIn) { return addPointSet(coordsIn.data(), coordsIn.size() / 3); }
        ///remove a point set by its set number
        void removePointSet(const int32_t whichSet);
        ///returns the index of the closest point, and optionally which point set and the coords
        int64_t closestPoint(const float target[3], LocatorInfo* infoOut = NULL) const;
        int64_t closestPointLimited(const float target[3], const float& maxDist, LocatorInfo* infoOut = NULL) const;
        std::vector<LocatorInfo> pointsInRange(const float target[3], const float& maxDist) const;
        bool anyInRange(const float target[3], const float& maxDist) const;
    };
}

#endif //__CARET_POINT_LOCATOR_H__
