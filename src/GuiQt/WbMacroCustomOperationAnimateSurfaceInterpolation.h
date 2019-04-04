#ifndef __WB_MACRO_CUSTOM_OPERATION_ANIMATE_SURFACE_INTERPOLATION_H__
#define __WB_MACRO_CUSTOM_OPERATION_ANIMATE_SURFACE_INTERPOLATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "WbMacroCustomOperationBase.h"

namespace caret {

    class ModelSurface;
    class ModelWholeBrain;
    class SpecFile;
    class Surface;
    
    class WbMacroCustomOperationAnimateSurfaceInterpolation : public WbMacroCustomOperationBase {
        
    public:
        WbMacroCustomOperationAnimateSurfaceInterpolation();
        
        virtual ~WbMacroCustomOperationAnimateSurfaceInterpolation();
        
        WbMacroCustomOperationAnimateSurfaceInterpolation(const WbMacroCustomOperationAnimateSurfaceInterpolation&) = delete;
        
        WbMacroCustomOperationAnimateSurfaceInterpolation& operator=(const WbMacroCustomOperationAnimateSurfaceInterpolation&) = delete;
        
        virtual bool executeCommand(QWidget* parent,
                                    const WuQMacroExecutorMonitor* executorMonitor,
                                    const WuQMacroExecutorOptions* executorOptions,
                                    const WuQMacroCommand* macroCommand) override;

        virtual WuQMacroCommand* createCommand() override;
        
        
        
        // ADD_NEW_METHODS_HERE

    private:
        bool interpolateSurface(const WuQMacroExecutorMonitor* executorMonitor,
                                const WuQMacroExecutorOptions* executorOptions,
                                const int32_t tabIndex,
                                ModelWholeBrain* wholeBrainModel,
                                const Surface* startSurface,
                                const Surface* endSurface,
                                const float durationSeconds);
        
        void createInterpolationSurface(const Surface* surface);
        
        void deleteInterpolationSurface();
        
        Surface* m_interpolationSurface = NULL;
        
        SpecFile* m_specFile = NULL;
        
        bool m_specFileModificationStatus = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WB_MACRO_CUSTOM_OPERATION_ANIMATE_SURFACE_INTERPOLATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_CUSTOM_OPERATION_ANIMATE_SURFACE_INTERPOLATION_DECLARE__

} // namespace
#endif  //__WB_MACRO_CUSTOM_OPERATION_ANIMATE_SURFACE_INTERPOLATION_H__
