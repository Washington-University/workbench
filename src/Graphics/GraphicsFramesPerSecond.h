#ifndef __GRAPHICS_FRAMES_PER_SECOND_H__
#define __GRAPHICS_FRAMES_PER_SECOND_H__

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


#include <memory>
#include <vector>

#include "CaretObject.h"



namespace caret {

    class ElapsedTimer;
    
    class GraphicsFramesPerSecond : public CaretObject {
        
    public:
        GraphicsFramesPerSecond(const int32_t maximumFrameCount = 20);
        
        virtual ~GraphicsFramesPerSecond();
        
        GraphicsFramesPerSecond(const GraphicsFramesPerSecond&) = delete;

        GraphicsFramesPerSecond& operator=(const GraphicsFramesPerSecond&) = delete;
        
        double getFramesPerSecond() const;
        
        double getAverageFramesPerSecond() const;
        
        double getAverageIntervalFramesPerSecond() const;
        
        void startOfDrawing();
        
        void endOfDrawing();

        void reinitialize();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void updateAverageIntervalFramesPerSecond();
        
        double getFPS(const std::vector<double>& frameTimes) const;
        
        const int32_t m_maximumFrameCount;
        
        std::unique_ptr<ElapsedTimer> m_startEndTimer;
        
        std::unique_ptr<ElapsedTimer> m_intervalTimer;
        
        std::vector<double> m_startStopFrameTimes;
        
        int32_t m_startStopFrameTimesIndex = 0;
        
        mutable std::vector<double> m_intervalFrameTimes;
        
        mutable int32_t m_intervalFrameTimesIndex = 0;
        
        double m_mostRecentFramesPerSecond = 0.0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_FRAMES_PER_SECOND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_FRAMES_PER_SECOND_DECLARE__

} // namespace
#endif  //__GRAPHICS_FRAMES_PER_SECOND_H__
