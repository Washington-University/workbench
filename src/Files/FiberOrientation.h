#ifndef __FIBER_ORIENTATION__H__
#define __FIBER_ORIENTATION__H__

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


#include "CaretObject.h"

namespace caret {

    class Fiber;
    
    class FiberOrientation : public CaretObject {
        
    public:
        FiberOrientation(const int32_t numberOfFibers,
                                   float* pointerToData);
        
        virtual ~FiberOrientation();

        /** 
         * XYZ coordinates at spatial center of distribution.
         * When valid, points to memory in a CIFTI file.
         */
        float m_xyz[3];
        
        /** 
         * Number of fibers in this group.
         * (number of elements in member 'fibers').
         */
        int32_t m_numberOfFibers;
        
        /** 
         * Pointers to all fibers in this group.
         * When valid, points to memory in a CIFTI file
         */
        std::vector<Fiber*> m_fibers;

        /**
         * True if the fiber is valid, else false.
         */
        bool m_valid;
        
        /**
         * Describes why fiber is invalid.
         */
        QString m_invalidMessage;
        
        /** 
         * Number of elements per fiber in a fiber orientation's file
         * (excluding the Fibers).
         *
         * At this time, this is the XYZ.
         * The value for this constant MUST be updated if elements are
         * added to a fiber orientation.
         */
        static const int32_t NUMBER_OF_ELEMENTS_IN_FILE;
        
    private:
        FiberOrientation(const FiberOrientation&);

        FiberOrientation& operator=(const FiberOrientation&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FIBER_ORIENTATION_DECLARE__
    const int32_t FiberOrientation::NUMBER_OF_ELEMENTS_IN_FILE = 3;
#endif // __FIBER_ORIENTATION_DECLARE__

} // namespace
#endif  //__FIBER_ORIENTATION__H__
