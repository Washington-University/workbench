#ifndef __COLOR_FUNCTIONS_H__
#define __COLOR_FUNCTIONS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020  Washington University School of Medicine
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

    class ColorFunctions
    {
    public:
        ///sRGB is 0 to 1
        static void sRGBtoCIEXYZ(const float sRGB[3], float xyzOut[3]);

        static void CIEXYZtoCIELab(const float xyz[3], float LabOut[3]);
        //maybe add the inverses for completeness?

        static void CIEXYZtoCIELuvPrime (const float xyz[3], float LuvOut[3]);
        static void CIELuvPrimetoCIEXYZ (const float Luv[3], float xyzOut[3]);

        //TODO: color deficiency simulation
        static float perceptualDistanceSRGB(const float srgb1[3], const float srgb2[3]);
    };

}//namespace

#endif //__COLOR_FUNCTIONS_H__
