#ifndef __FIBER_ORIENTATION_SAMPLES_LOADER_H__
#define __FIBER_ORIENTATION_SAMPLES_LOADER_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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


#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "FiberOrientationSamplesVector.h"


namespace caret {

    class Brain;
    class FiberOrientation;
    class VolumeFile;
    
    class FiberOrientationSamplesLoader : public CaretObject, public EventListenerInterface  {
        
    public:
        FiberOrientationSamplesLoader();
        
        virtual ~FiberOrientationSamplesLoader();
        
        bool getFiberOrientationSphericalSamplesVectors(Brain* brain,
                                            std::vector<FiberOrientationSamplesVector>& xVectors,
                                            std::vector<FiberOrientationSamplesVector>& yVectors,
                                            std::vector<FiberOrientationSamplesVector>& zVectors,
                                            FiberOrientation* &fiberOrientationOut,
                                            AString& errorMessageOut);
        
        void reset();
        
        void receiveEvent(Event* event);
        
    private:
        FiberOrientationSamplesLoader(const FiberOrientationSamplesLoader&);

        FiberOrientationSamplesLoader& operator=(const FiberOrientationSamplesLoader&);
        
        bool loadSphericalOrientationVolumes(Brain* brain,
                                             AString& errorMessageOut);
        
        /** Tried to load sample volumes since last reset (they may or may not be valid) */
        bool m_sampleVolumesLoadAttemptValid;
        
        /** Sample volumes were loaded and are valid */
        bool m_sampleVolumesValid;
        
        /* sample magnitude volumes */
        VolumeFile* m_sampleMagnitudeVolumes[3];
        
        /* sample theta angle volumes */
        VolumeFile* m_sampleThetaVolumes[3];
        
        /* sample phi angle volumes */
        VolumeFile* m_samplePhiVolumes[3];
        
        /* last identified location */
        float m_lastIdentificationXYZ[3];
        
        /* last identification valid */
        bool m_lastIdentificationValid;
        
    };
    
#ifdef __FIBER_ORIENTATION_SAMPLES_LOADER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FIBER_ORIENTATION_SAMPLES_LOADER_DECLARE__

} // namespace
#endif  //__FIBER_ORIENTATION_SAMPLES_LOADER_H__
