#ifndef __BORDER_LENGTH_HELPER_H__
#define __BORDER_LENGTH_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "CaretPointer.h"

#include <vector>

namespace caret
{

    class Border;
    class GeodesicHelper;
    class SurfaceFile;
    
    class BorderLengthHelper
    {
        const SurfaceFile* m_surface;
        CaretPointer<GeodesicHelper> m_geoHelp;
        const float* m_corrAreas;
        std::vector<float> m_currAreas;
    public:
        BorderLengthHelper(const SurfaceFile* mySurf, const float* correctedAreas = NULL);
        float length(const Border* myBorder);
        ~BorderLengthHelper();
    };
    
}

#endif //__BORDER_LENGTH_HELPER_H__
