
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
 *
 *  There are two frame-per-second timers in an instance of this class.
 *
 * (1) An elapsed timer that user starts and stops by making calls to
 * 'startOfDrawing()' and 'endOfDrawing()'. By using these functions the
 * user can repeatedly measure a portion of code.  While this can be used
 * to measure all of the graphics drawing (such as calling at start and end
 * of paintGL) it only measures the drawing time and not the time for
 * swapping buffers, performing computations, etc. that are outside of
 * the graphics loop.
 *
 * (2) A an elapsed timer that is update once in the graphics loop and read
 * as desired.  By updating in the graphics loop, it measures all time
 * required to produce a frame of graphics including buffer swapping,
 * computation, user-interface updates, etc.  When graphics are being
 * continuously updated, such as user using mouse for pan or rotate,
 * this timer should be fairly accurate during that time.  If there have been
 * no graphics update for a while the frames-per-second may be slower
 * than the true fps.  This timer reports that average of the last 'N' graphics
 * updates.
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
    m_startEndTimer.reset(new ElapsedTimer());
    m_sinceLastTimer.reset(new ElapsedTimer());
    m_sinceLastFrameTimes.resize(m_maximumFrameCount);
    std::fill(m_sinceLastFrameTimes.begin(),
              m_sinceLastFrameTimes.end(),
              -1.0);
    
    reinitialize();
}

/**
 * Destructor.
 */
GraphicsFramesPerSecond::~GraphicsFramesPerSecond()
{
}

/**
 * Compute the frames per second for a given group of frame times.
 * @param frameTimes
 *    The frame times
 * @return The frames per second
 */
double
GraphicsFramesPerSecond::getFPS(const std::vector<double>& frameTimes) const
{
    /*
     * Get sum of recent frame times
     */
    double sumMilliseconds(0.0);
    int32_t numValidFrames(0);
    for (int32_t i = 0; i < m_maximumFrameCount; i++) {
        CaretAssertVectorIndex(frameTimes, i);
        const double& ft = frameTimes[i];
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
 * @return The frames per second between calls to 'startOfDrawing()' and
 * 'endOfDrawing()'.  If these calls are only made in graphics drawing and
 * do not include buffer swapping, the times will be innacturately too fast.
 */
double
GraphicsFramesPerSecond::getStartEndFramesPerSecond() const
{
    return getFPS(m_startStopFrameTimes);
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
    m_startEndTimer->reset();
    m_startEndTimer->start();
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
    if (m_startStopFrameTimesIndex >= m_maximumFrameCount) {
        m_startStopFrameTimesIndex = 0;
    }
    CaretAssertVectorIndex(m_startStopFrameTimes, m_startStopFrameTimesIndex);
    m_startStopFrameTimes[m_startStopFrameTimesIndex] = m_startEndTimer->getElapsedTimeMilliseconds();
    ++m_startStopFrameTimesIndex;
}

/**
 * Reinitialize the instance
 */
void
GraphicsFramesPerSecond::reinitialize()
{
    m_startStopFrameTimes.resize(m_maximumFrameCount);
    std::fill(m_startStopFrameTimes.begin(), m_startStopFrameTimes.end(), -1.0);
    m_startStopFrameTimesIndex = 0;
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

/**
 * Update average of time since this function was last called.  This should be called
 * once during each time the graphics is being drawn.
 */
void
GraphicsFramesPerSecond::updateSinceLastFramesPerSecond()
{
    if (m_sinceLastTimer->isStarted()) {
        /*
         * Since graphics are updated (as needed), we exclude any
         * long updates that are most likley due to:
         * (1) The user interacting (rotate, pan, etc);
         * (2) User does nothing for a while;
         * (3) User starts interacting again.
         *
         * If we do not exclude these long updates we will
         * have some big jumps in the timing since the time
         * doing nothing would be measured
         */
        const double userDoesNothingMilliseconds(5.0 * 1000.0);
        const double sinceLastMilliseconds(m_sinceLastTimer->getElapsedTimeMilliseconds());
        if (sinceLastMilliseconds < userDoesNothingMilliseconds) {
            if (m_sinceLastFrameTimesIndex >= m_maximumFrameCount) {
                m_sinceLastFrameTimesIndex = 0;
            }
            CaretAssertVectorIndex(m_sinceLastFrameTimes, m_sinceLastFrameTimesIndex);
            m_sinceLastFrameTimes[m_sinceLastFrameTimesIndex] = sinceLastMilliseconds;
            ++m_sinceLastFrameTimesIndex;
        }
    }

    m_sinceLastTimer->reset();
    m_sinceLastTimer->start();
}


/**
 * @return Average of time since between calls to 'updateSinceLastFramesPerSecond()'.
 */
double
GraphicsFramesPerSecond::getSinceLastFramesPerSecond() const
{
    double fpsOut(0.0);
    
    if (m_sinceLastTimer->isStarted()) {
        fpsOut = getFPS(m_sinceLastFrameTimes);

    }

    return fpsOut;
}


