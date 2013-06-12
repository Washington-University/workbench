
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
 * Constructor.
 */
FiberOrientationTrajectory::FiberOrientationTrajectory(const FiberOrientation* fiberOrientation,
                                                       const int64_t rowIndex)
: m_fiberOrientation(fiberOrientation),
m_rowIndex(rowIndex)
{
    m_totalCount = 0;
    m_fiberFractions.clear();
    m_distance = 0.0;
    m_itemCount = 0;
}

/**
 * Destructor.
 */
FiberOrientationTrajectory::~FiberOrientationTrajectory()
{
    
}

void
FiberOrientationTrajectory::addFiberFractions(const FiberFractions& fiberFraction)
{
    m_totalCount += fiberFraction.totalCount;
    const int64_t num = static_cast<int64_t>(fiberFraction.fiberFractions.size());
    if (m_fiberFractions.empty()) {
        m_fiberFractions.resize(num, 0.0);
    }
    else {
        CaretAssert(m_fiberFractions.size() == fiberFraction.fiberFractions.size());
    }
    
    for (int64_t i = 0; i < num; i++) {
        m_fiberFractions[i] += fiberFraction.fiberFractions[i];
    }
    
    m_distance += fiberFraction.distance;
    
    m_itemCount++;
}

void
FiberOrientationTrajectory::averageFiberFractions()
{
    if (m_itemCount > 1) {
        m_totalCount /= m_itemCount;
        
        const int64_t num = static_cast<int64_t>(m_fiberFractions.size());
        for (int64_t i = 0; i < num; i++) {
            m_fiberFractions[i] /= m_itemCount;
        }
        
        m_distance /= m_itemCount;
    }
    
}

