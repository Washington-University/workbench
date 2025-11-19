
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

#define __FIBER_ORIENTATION_TRAJECTORY_DECLARE__
#include "FiberOrientationTrajectory.h"
#undef __FIBER_ORIENTATION_TRAJECTORY_DECLARE__

#include "CaretAssert.h"
#include "CaretSparseFile.h"
#include "MathFunctions.h"


using namespace caret;


    
/**
 * \class caret::FiberOrientationTrajectory 
 * \brief Containts fiber trajectory and corresponding fiber orientation.
 */

/**
 * Constructor that accepts a fiber orientation.
 *
 * @param fiberOrientationIndex
 *    Index of the fiber orientation.
 * @param fiberOrientation
 *    Fiber orientation that is associated with this fiber trajectory.
 */
FiberOrientationTrajectory::FiberOrientationTrajectory(const int64_t fiberOrientationIndex,
                                                       const FiberOrientation* fiberOrientation)
: m_fiberOrientationIndex(fiberOrientationIndex),
m_fiberOrientation(fiberOrientation)
{
    CaretAssert(fiberOrientation);
    
    m_fiberFraction = new FiberFractions();
    m_fiberFractionTotalCountFloat = 0.0;
    
    m_totalCountSum = 0.0;
    m_fiberCountsSum.clear();
    m_distanceSum = 0.0;
    m_countForAveraging = 0;
}

/**
 * Destructor.
 */
FiberOrientationTrajectory::~FiberOrientationTrajectory()
{
    delete m_fiberFraction;
}

/**
 * Add a fiber fraction for averaging.
 * 
 * @param fiberFraction
 *    Fiber fraction that is added.
 */
void
FiberOrientationTrajectory::addFiberFractionsForAveraging(const FiberFractions& fiberFraction)
{
    const bool includeZeroTotalCountWhenAveraging = true;
    
    const int64_t numFractions = fiberFraction.fiberFractions.size();
    if ((fiberFraction.totalCount > 0)
        && (numFractions > 0)) {
        
//        const float len = MathFunctions::vectorLength(&fiberFraction.fiberFractions[0]);
//        if (len < 1.0) {
//            std::cout << "Fraction len < 1: " << len << std::endl;
//        }
        
        if (m_fiberCountsSum.empty()) {
            m_fiberCountsSum.resize(numFractions,
                                    0.0);
        }
        else if (static_cast<int64_t>(m_fiberCountsSum.size()) != numFractions) {
            CaretAssertMessage(0,
                               "Sizes should be the same");
            return;
        }
        
        m_totalCountSum += fiberFraction.totalCount;
        
        for (int64_t i = 0; i < numFractions; i++) {
            m_fiberCountsSum[i] += fiberFraction.fiberFractions[i] * fiberFraction.totalCount;
        }
        
        m_distanceSum += fiberFraction.distance;
        
        m_countForAveraging += 1;
    }
    else if (includeZeroTotalCountWhenAveraging) {
        m_countForAveraging += 1;
    }
}

/**
 * Set a fiber fraction.
 *
 * @param fiberFraction
 *    Fiber fraction that is replaced.
 */
void
FiberOrientationTrajectory::setFiberFractions(const FiberFractions& fiberFraction)
{
    *m_fiberFraction = fiberFraction;
    if (m_fiberFraction->fiberFractions.empty()) {
        m_fiberFraction->clear();
    }
    m_fiberFractionTotalCountFloat = m_fiberFraction->totalCount;
}

/**
 * Finish, which will update the fiber fraction using the average of all
 * of the fiber fractions that were added.
 */
void
FiberOrientationTrajectory::finishAveraging()
{
    if (m_countForAveraging > 0) {
        m_fiberFraction->distance = m_distanceSum / m_countForAveraging;
        m_fiberFraction->totalCount = m_totalCountSum / m_countForAveraging;
        m_fiberFractionTotalCountFloat = m_totalCountSum / m_countForAveraging;

        const int64_t numFiberCounts = static_cast<int64_t>(m_fiberCountsSum.size());
        m_fiberFraction->fiberFractions.resize(numFiberCounts);
        if (numFiberCounts > 0) {
            for (int64_t i = 0; i < numFiberCounts; i++) {
                if (m_fiberFractionTotalCountFloat > 0.0) {
                    const float averageCount = m_fiberCountsSum[i] / m_countForAveraging;
                    m_fiberFraction->fiberFractions[i] = (averageCount / m_fiberFractionTotalCountFloat);
                    
                    float sum = (m_fiberFraction->fiberFractions[0]
                                 + m_fiberFraction->fiberFractions[1]
                                 + m_fiberFraction->fiberFractions[2]);
                    if (sum > 1.0) {
                        const float divisor = 1.0 / sum;
                        m_fiberFraction->fiberFractions[0] *= divisor;
                        m_fiberFraction->fiberFractions[1] *= divisor;
                        m_fiberFraction->fiberFractions[2] *= divisor;
//                        float newSum = (m_fiberFraction->fiberFractions[0]
//                                     + m_fiberFraction->fiberFractions[1]
//                                     + m_fiberFraction->fiberFractions[2]);
//                        std::cout << "Sum too big: " << sum << " new sum: " << newSum << std::endl;
                    }
                }
                else {
                    m_fiberFraction->fiberFractions[i] = 0.0;
                }
            }
        }
    }
    else {
        m_fiberFraction->clear();
    }
}


