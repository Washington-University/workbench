#ifndef __FIBER_H__
#define __FIBER_H__

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

#include <QString>
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
        
        /**
         * RGB Color for directional unit vector of fiber
         */
        float m_directionUnitVectorRGB[3];
        
        /**
         * True if the fiber is valid, else false.
         */
        bool m_valid;

        /**
         * Describes why fiber is invalid.
         */
        QString m_invalidMessage;
        
        /**
         * Opacity of fiber drawing used by drawing code.
         * This value IS NOT stored in the file.
         */
        float m_opacityForDrawing;
        
        /** Number of elements per fiber in a fiber orientation's file */
        static const int32_t NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE;
    
    private:
        float fanningEigenvalueToAngle(const float k);
        
        
    };
#ifdef __FIBER_DECLARE__
    const int32_t Fiber::NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE = 7;
#endif // __FIBER_DECLARE__
} // namespace
#endif  //__FIBER_H__
