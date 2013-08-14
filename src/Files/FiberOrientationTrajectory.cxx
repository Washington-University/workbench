
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __FIBER_ORIENTATION_TRAJECTORY_DECLARE__
#include "FiberOrientationTrajectory.h"
#undef __FIBER_ORIENTATION_TRAJECTORY_DECLARE__

#include "CaretAssert.h"
#include "CaretSparseFile.h"

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
 * Add a fiber fraction.
 * 
 * @param fiberFraction
 *    Fiber fraction that is added.
 */
void
FiberOrientationTrajectory::addFiberFractions(const FiberFractions& fiberFraction)
{
    const bool includeZeroTotalCountWhenAveraging = true;
    
    const int64_t numFractions = fiberFraction.fiberFractions.size();
    if ((fiberFraction.totalCount > 0)
        && (numFractions > 0)) {
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
 * Finish, which will update the fiber fraction using the average of all
 * of the fiber fractions that were added.
 */
void
FiberOrientationTrajectory::finish()
{
    if (m_countForAveraging > 0) {
        m_fiberFraction->distance = m_distanceSum / m_countForAveraging;
        m_fiberFraction->totalCount = m_totalCountSum / m_countForAveraging;
        m_fiberFractionTotalCountFloat = m_totalCountSum / m_countForAveraging;

        const int64_t numFiberCounts = static_cast<int64_t>(m_fiberCountsSum.size());
        m_fiberFraction->fiberFractions.resize(numFiberCounts);
        if (numFiberCounts > 0) {
            for (int64_t i = 0; i < numFiberCounts; i++) {
                if (m_fiberFraction->totalCount > 0.0) {
                    const float averageCount = m_fiberCountsSum[i] / m_countForAveraging;
                    m_fiberFraction->fiberFractions[i] = (averageCount / m_fiberFraction->totalCount);
                }
                else {
                    m_fiberFraction->fiberFractions[i] = 0.0;
                }
            }
        }
    }
    else {
        m_fiberFraction->zero();
    }
}


