
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

#include <iostream>

#define __FIBER_DECLARE__
#include "Fiber.h"
#undef __FIBER_DECLARE__

#include "AString.h"

#include <cmath>

using namespace caret;


    
/**
 * \class caret::Fiber 
 * \brief Data for a single fiber
 */

/**
 * Constructor.
 */
Fiber::Fiber(const float* pointerToData)
{
    /*
     * Retrieve values from the file
     */
    m_meanF = pointerToData[0];
    m_varF  = pointerToData[1];
    m_theta = pointerToData[2];
    m_phi   = pointerToData[3];
    m_k1    = pointerToData[4];
    m_k2    = pointerToData[5];
    m_psi   = pointerToData[6];
    
    /*
     * Set computed values used for visualization
     */
    
    m_fanningMajorAxisAngle = kToAngle(m_k1);
    m_fanningMinorAxisAngle = kToAngle(m_k2);
    
    /*
     * m_theta is angle from Positive-Z rotated about X-Axis
     * looking to negative X.
     *
     * m_phi is angle from positive X-Axis rotated about Z-Axis
     * looking to negative Z.
     *
     * azimuth is angle from positive-Y axis in XY-plane.
     * elevation is angle up/down angle from XY-plane.
     */
    m_directionUnitVector[0] = std::sin(m_theta) * std::cos(m_phi);
    m_directionUnitVector[1] = std::sin(m_theta) * std::sin(m_phi);
    m_directionUnitVector[2] = std::cos(m_theta);
    
//    const float azimuth   = M_PI_2 - m_phi; // along Y-Axis
//    const float elevation = M_PI_2 - m_theta;
//    m_directionUnitVector[0] = std::sin(azimuth) * std::cos(elevation);
//    m_directionUnitVector[1] = std::cos(azimuth) * std::cos(elevation);
//    m_directionUnitVector[2] = std::sin(elevation);
}

/**
 * Destructor.
 */
Fiber::~Fiber()
{
}

float
Fiber::kToAngle(const float k)
{
    float angle = 0.0;
    
    if (k > 0.0) {
        float sigma = 1.0 / std::sqrt(2.0 * k);
        if (sigma > 1.0) sigma = 1.0;
        angle = std::asin(sigma);
    }
    
    return angle;
}

