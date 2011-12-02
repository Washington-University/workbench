#ifndef __CUBIC_SPLINE_H__
#define __CUBIC_SPLINE_H__

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

namespace caret {

    class CubicSpline
    {
        float m_weights[4];
        CubicSpline();
    public:
        ///takes as input the fraction in [0, 1] along the middle (used) range of the spline, low and high edge set whether it doesn't have p[0] or p[3] to use, respectively
        static CubicSpline hermite(float frac, bool lowEdge, bool highEdge);

        //splines will be reused, so this part should be fast for the majority case (testing for if it is an edge case would slow it down for the majority case)
        ///evaluate the spline with these samples
        inline float evaluate(const float p0, const float p1, const float p2, const float p3) const
        {
            return p0 * m_weights[0] + p1 * m_weights[1] + p2 * m_weights[2] + p3 * m_weights[3];
        }
    };

}

#endif //__CUBIC_SPLINE_H__