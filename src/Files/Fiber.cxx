
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

#define __FIBER_DECLARE__
#include "Fiber.h"
#undef __FIBER_DECLARE__

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
    m_meanF = pointerToData[0];
    m_varF  = pointerToData[1];
    m_theta = pointerToData[2];
    m_phi   = pointerToData[3];
    m_k1    = kToAngle(pointerToData[4]);
    m_k2    = kToAngle(pointerToData[5]);
    m_psi   = pointerToData[6];
    
    
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

