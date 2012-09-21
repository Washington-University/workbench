#ifndef __FIBER_H__
#define __FIBER_H__

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

#include <stdint.h>

namespace caret {

    /**
     * \struct caret::Fiber
     * \brief  Attributes of a single fiber
     */    
    class Fiber {
    public:
        Fiber(const float* pointerToData);
        
        ~Fiber();
        
        /** 
         * Spatial magnitude of distribution/distance from center 
         * This value is from the fiber orientation file.
         */
        float m_meanF;
        
        /**
         * Spatial variance in magnitude of distribution/distance from center
         * This value is from the fiber orientation file.
         */
        float m_varF;
        
        /** 
         * First spatial angle of distribution.
         * Angle from Positive Z-Axis rotated about a line
         * in the XY-Plane
         * Units is radians.
         * This value is from the fiber orientation file.
         */
        float m_theta;
        
        /** 
         * Second spatial angle of distribution.
         * Aximuthal angle in X-Y Plane, counter-clockwise
         * around positive Z-Axis starting at positive X-Axis.
         * Units is radians.
         * This value is from the fiber orientation file.
         */
        float m_phi;
        
        /**
         * Major fanning eigenvalue
         * This value is from the fiber orientation file.
         */
        float m_k1;
        
        /** 
         * Minor fanning eigenvalue
         * This value is from the fiber orientation file.
         */
        float m_k2;
        
        /** 
         * Angle of anisotropy in uncertainty/fanning distribution on sphere 
         * Units is radians.
         * This value is from the fiber orientation file.
         */
        float m_psi;
        
        /**
         * Angle of fanning for the major axis.
         * Units is radians.
         * This value is computed and is NOT from the fiber orientation file.
         */
        float m_fanningMajorAxisAngle;
        
        /**
         * Angle of fanning for the minor axis.
         * Units is radians.
         * This value is computed and is NOT from the fiber orientation file.
         */
        float m_fanningMinorAxisAngle;
        
        /**
         * Directional unit vector of fiber
         */
        float m_directionUnitVector[3];
        
        /** Number of elements per fiber in a fiber orientation's file */
        static const int32_t NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE;
    
    private:
        float kToAngle(const float k);
        
        
    };
#ifdef __FIBER_DECLARE__
    const int32_t Fiber::NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE = 7;
#endif // __FIBER_DECLARE__
} // namespace
#endif  //__FIBER_H__
