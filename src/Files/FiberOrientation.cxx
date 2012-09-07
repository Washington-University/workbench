
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

#define __FIBER_ORIENTATION_DECLARE__
#include "FiberOrientation.h"
#undef __FIBER_ORIENTATION_DECLARE__

#include "Fiber.h"

using namespace caret;


    
/**
 * \class caret::FiberOrientation 
 * \brief Groups fibers at a particular spatial coordinate
 */

/**
 * Constructor.
 */
FiberOrientation::FiberOrientation(const int32_t numberOfFibers,
                                                       float* pointerToData)
: CaretObject()
{
    m_xyz = pointerToData;
    m_numberOfFibers = numberOfFibers;

    /*
     * Point to 1st element after XYZ in CIFTI file row
     */
    float* offset = pointerToData + 3;
    
    /*
     * Step between successive fibers in CIFTI file row
     */
    const int64_t step  = sizeof(struct Fiber) / 4;
    
    /*
     * Point to each fiber in the CIFTI file row 
     */
    for (int32_t i = 0; i < m_numberOfFibers; i++) {
        m_fibers.push_back(reinterpret_cast<Fiber*>(offset));
        offset += step;
    }
}

/**
 * Destructor.
 */
FiberOrientation::~FiberOrientation()
{
    
}

/**
 * @return Is this fiber orientation group valid?
 */
bool
FiberOrientation::isValid() const
{
    if (m_xyz != NULL) {
        if (m_numberOfFibers > 0) {
            return true;
        }
    }
    
    return false;
}


