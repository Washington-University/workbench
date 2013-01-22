
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
{
    m_xyz[0] = pointerToData[0];
    m_xyz[1] = pointerToData[1];
    m_xyz[2] = pointerToData[2];
    m_numberOfFibers = numberOfFibers;

    /*
     * Point to 1st element after XYZ in CIFTI file row
     */
    float* offset = pointerToData + FiberOrientation::NUMBER_OF_ELEMENTS_IN_FILE;
    
    /*
     * Point to each fiber in the CIFTI file row 
     */
    for (int32_t i = 0; i < m_numberOfFibers; i++) {
        Fiber* fiber = new Fiber(offset);
        m_fibers.push_back(fiber);
        offset += Fiber::NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE;
        
        if (fiber->m_valid == false) {
            if (m_invalidMessage.isEmpty() == false) {
                m_invalidMessage += "; ";
            }
            m_invalidMessage += ("Index="
                                 + AString::number(i)
                                 + ": "
                                 + fiber->m_invalidMessage);
        }
    }
    
    m_valid = m_invalidMessage.isEmpty();
}

/**
 * Destructor.
 */
FiberOrientation::~FiberOrientation()
{
    for (int32_t i = 0; i < m_numberOfFibers; i++) {
        delete m_fibers[i];
    }
    m_fibers.clear();
}

