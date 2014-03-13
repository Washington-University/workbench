#ifndef __FIBER_ORIENTATION_SAMPLES_LOADER_H__
#define __FIBER_ORIENTATION_SAMPLES_LOADER_H__

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
