
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

