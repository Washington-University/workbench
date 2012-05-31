#ifndef __VOLUME_SURFACE_OUTLINE_SET_MODEL__H_
#define __VOLUME_SURFACE_OUTLINE_SET_MODEL__H_

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

#include "BrainConstants.h"
#include "CaretColorEnum.h"
#include "CaretObject.h"

namespace caret {

    class Brain;
    class Surface;
    class VolumeSurfaceOutlineModel;
    
    class VolumeSurfaceOutlineSetModel : public CaretObject {
        
    public:
        VolumeSurfaceOutlineSetModel();
        
        virtual ~VolumeSurfaceOutlineSetModel();
        
        void copyVolumeSurfaceOutlineSetModel(VolumeSurfaceOutlineSetModel* setModel);
        
        int32_t getNumberOfDislayedVolumeSurfaceOutlines() const;
        
        void setNumberOfDisplayedVolumeSurfaceOutlines(const int32_t numberDisplayed);
        
        VolumeSurfaceOutlineModel* getVolumeSurfaceOutlineModel(const int32_t indx);
        
        const VolumeSurfaceOutlineModel* getVolumeSurfaceOutlineModel(const int32_t indx) const;
        
        void selectSurfacesAfterSpecFileLoaded(Brain* brain,
                                               const bool searchForTabs);
        
    private:
        VolumeSurfaceOutlineSetModel(const VolumeSurfaceOutlineSetModel&);

        VolumeSurfaceOutlineSetModel& operator=(const VolumeSurfaceOutlineSetModel&);
        
        void addSurfaceOutline(Surface* surface,
                               const float thickness,
                               const int32_t browserTabIndex,
                               const CaretColorEnum::Enum color,
                               int32_t& outlineIndex);
        
        VolumeSurfaceOutlineModel* outlineModels[BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES];
        
        int32_t numberOfDisplayedVolumeSurfaceOutlines;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_SET_MODEL__H_
