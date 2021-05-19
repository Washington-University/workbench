
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __GRAPHICS_FRAMES_PER_SECOND_DECLARE__
#include "GraphicsFramesPerSecond.h"
#undef __GRAPHICS_FRAMES_PER_SECOND_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
#include "ElapsedTimer.h"

using namespace caret;


    
/**
 * \class caret::GraphicsFramesPerSecond 
 * \brief Assists with timing graphics
 * \ingroup Graphics
 */

/**
 * Constructor.
 * @param maximumFrameCount
 *    Number of frames that are averaged to create the frames per second
 */
GraphicsFramesPerSecond::GraphicsFramesPerSecond(const int32_t maximumFrameCount)
: CaretObject(),
m_maximumFrameCount(maximumFrameCount)
{
    m_timer.reset(new ElapsedTimer());
    
    reinitialize();
}

/**
 * Destructor.
 */
GraphicsFramesPerSecond::~GraphicsFramesPerSecond()
{
}

/**
 * @return The frames per second
 */
double
GraphicsFramesPerSecond::getFramesPerSecond() const
{
    /*
     * Get sum of recent frame times
     */
    double sumMilliseconds(0.0);
    int32_t numValidFrames(0);
    for (int32_t i = 0; i < m_maximumFrameCount; i++) {
        CaretAssertVectorIndex(m_frameTimes, i);
        const double& ft = m_frameTimes[i];
        if (ft >= 0.0) {
            sumMilliseconds += ft;
            numValidFrames++;
        }
    }
    
    /*
     * Calculate frames per second
     */
    double framesPerSecond(0.0);
    const double seconds(sumMilliseconds / 1000.0);
    if (seconds > 0.0) {
        framesPerSecond = (numValidFrames / seconds);
    }
    
    return framesPerSecond;
}

/**
 * Call at the beginning of drawing to start timing one frame
 */
void
GraphicsFramesPerSecond::startOfDrawing()
{
    /*
     * Start timing
     */
    m_timer->reset();
    m_timer->start();
}

/**
 * Call at the end of drawing to end timing for one frame
 */
void
GraphicsFramesPerSecond::endOfDrawing()
{
    /*
     * Save frame time
     */
    if (m_frameTimesIndex >= m_maximumFrameCount) {
        m_frameTimesIndex = 0;
    }
    CaretAssertVectorIndex(m_frameTimes, m_frameTimesIndex);
    m_frameTimes[m_frameTimesIndex] = m_timer->getElapsedTimeMilliseconds();
    ++m_frameTimesIndex;
}

/**
 * Reinitialize the instance
 */
void
GraphicsFramesPerSecond::reinitialize()
{
    m_frameTimes.resize(m_maximumFrameCount);
    std::fill(m_frameTimes.begin(), m_frameTimes.end(), -1.0);
    m_frameTimesIndex = 0;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsFramesPerSecond::toString() const
{
    return "GraphicsFramesPerSecond";
}

