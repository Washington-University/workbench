#ifndef __VOLUME_SPLINE_H__
#define __VOLUME_SPLINE_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "stdint.h"
#include "CaretPointer.h"

namespace caret {
    
    class VolumeSpline
    {
        int64_t m_dims[3];
        CaretArray<float> m_deconv;//don't do lazy deconvolution, it doesn't save much time, and takes more memory and slightly longer if you have to do the whole volume anyway
        void deconvolve(float* data, float* scratch, const int64_t& length);//use CaretArray so that it doesn't reallocate like a vector on copy, and the data is static once computed
    public:
        VolumeSpline();
        VolumeSpline(const float* frame, const int64_t framedims[3]);
        float sample(const float& i, const float& j, const float& k);
        float sample(const float ijk[3]) { return sample(ijk[0], ijk[1], ijk[2]); }
    };
    
}

#endif //__VOLUME_SPLINE_H__
