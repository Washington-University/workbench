#ifndef __FIBER_ORIENTATION_SAMPLES_VECTOR_H__
#define __FIBER_ORIENTATION_SAMPLES_VECTOR_H__

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






namespace caret {

    /**
     * \struct caret::FiberOrientationSamplesVector
     * \brief Stores a fiber orientation samples vector
     * \ingroup Brain
     */
    struct FiberOrientationSamplesVector {
        
        /** The direction vector */
        float direction[3];
        
        /** The magnitude (length) */
        float magnitude;
        
        /** RGB coloring for vector */
        float rgb[3];
        
        /**
         * Set the RGB color to absolute values of the directional vector
         */
        void setColor() {
            rgb[0] = (direction[0] >= 0.0) ? direction[0] : -direction[0];
            rgb[1] = (direction[1] >= 0.0) ? direction[1] : -direction[1];
            rgb[2] = (direction[2] >= 0.0) ? direction[2] : -direction[2];
        }
        
    };
    
#ifdef __FIBER_ORIENTATION_SAMPLES_VECTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FIBER_ORIENTATION_SAMPLES_VECTOR_DECLARE__

} // namespace
#endif  //__FIBER_ORIENTATION_SAMPLES_VECTOR_H__
