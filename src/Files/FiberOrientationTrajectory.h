#ifndef __FIBER_ORIENTATION_TRAJECTORY__H_
#define __FIBER_ORIENTATION_TRAJECTORY__H_

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

#include <vector>
#include <stdint.h>

#include "CaretAssert.h"
#include "CaretSparseFile.h"

namespace caret {

    class FiberOrientation;
    
    class FiberOrientationTrajectory {
        
    public:
        FiberOrientationTrajectory(const int64_t fiberOrientationIndex,
                                   const FiberOrientation* fiberOrientation);
        
        virtual ~FiberOrientationTrajectory();
        
        void addFiberFractionsForAveraging(const FiberFractions& fiberFraction);
        
        void setFiberFractions(const FiberFractions& fiberFraction);
        
        /**
         * @return the Fiber Orientation.
         */
        inline const FiberOrientation* getFiberOrientation() const {
            return m_fiberOrientation;
        }
        
//        /**
//         * Get the fiber fraction.
//         *
//         * @param indx
//         *    Index of the fiber fraction.
//         * @return 
//         *    Fiber fraction at the given index.
//         */
//        inline const FiberFractions* getFiberFraction() const {
//            return m_fiberFraction;
//        }
        
        /**
         * @return The fiber orientation index.
         */
        inline int64_t getFiberOrientationIndex() const {
            return m_fiberOrientationIndex;
        }
        
        /**
         * @return The total count as a float.
         */
        inline float getFiberFractionTotalCount() const {
            return m_fiberFractionTotalCountFloat;
        }

        /**
         * @return The fiber fractions (proportions).
         */
        inline const std::vector<float>& getFiberFractions() const {
            return m_fiberFraction->fiberFractions;
        }
        
        /**
         * @return The fiber fractions distance.
         */
        inline float getFiberFractionDistance() const {
            return m_fiberFraction->distance;
        }
        
        void finishAveraging();
        
    private:
        FiberOrientationTrajectory(const FiberOrientationTrajectory&);

        FiberOrientationTrajectory& operator=(const FiberOrientationTrajectory&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        const int64_t m_fiberOrientationIndex;
        
        const FiberOrientation* m_fiberOrientation;
        
        FiberFractions* m_fiberFraction;
        
        float m_fiberFractionTotalCountFloat;
        
        double m_totalCountSum;
        std::vector<double> m_fiberCountsSum;
        double m_distanceSum;
        int64_t m_countForAveraging;
        
        // ADD_NEW_MEMBERS_HERE

        friend class CiftiFiberTrajectoryFile;
    };
    
#ifdef __FIBER_ORIENTATION_TRAJECTORY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FIBER_ORIENTATION_TRAJECTORY_DECLARE__

} // namespace
#endif  //__FIBER_ORIENTATION_TRAJECTORY__H_
