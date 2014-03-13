#ifndef __FIBER_ORIENTATION__H__
#define __FIBER_ORIENTATION__H__

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

namespace caret {

    class Fiber;
    
    class FiberOrientation /* : public CaretObject */ {
        
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
         * Fiber orientations are drawn using blending (alpha values).  For
         * blending to work correctly in OpenGL, items must be drawn in 
         * "depth" order from furthest to nearest.
         */
        mutable float m_drawingDepth;
        
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
