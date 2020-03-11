#ifndef __PALETTE_NEW_H__
#define __PALETTE_NEW_H__

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

#include <vector>

namespace caret {
    
    class PaletteNew
    {
    public:
        struct ScalarColor
        {
            float scalar;
            float color[3];//RGB, no A
            ScalarColor(const float scalar, const float color[3])
            {
                this->scalar = scalar;
                for (int i = 0; i < 3; ++i)
                {
                    this->color[i] = color[i];
                }
            }
            ScalarColor(const float scalar, const float red, const float green, const float blue)
            {
                this->scalar = scalar;
                color[0] = red;
                color[1] = green;
                color[2] = blue;
            }
            ScalarColor()
            {
                scalar = 0.0f;
                for (int i = 0; i < 3; ++i)
                {
                    color[i] = 0.0f;
                }
            }
        };
        
        PaletteNew(std::vector<ScalarColor> posRange, float zeroColor[3], std::vector<ScalarColor> negRange);
        
        void getPaletteColor(const float scalar, float rgbOut[3]) const;
    private:
        class PaletteRange
        {
            static constexpr int BUCKETS = 256;//keep this at a 2^n to minimize float rounding during lookup
            std::vector<ScalarColor> m_controlPoints;
            int m_lookup[BUCKETS + 1];//this is used only to narrow the binary search range, frequently making it unnecessary to search
            float m_lowPoint, m_highPoint;//less indirection for critical values
        public:
            PaletteRange(const std::vector<ScalarColor> controlPoints);
            
            void getPaletteColor(const float scalar, float rgbOut[3]) const;
        };
        
        PaletteRange m_posRange, m_negRange;
        float m_zeroColor[3];
    };
    
}//namespace

#endif //__PALETTE_NEW_H__
