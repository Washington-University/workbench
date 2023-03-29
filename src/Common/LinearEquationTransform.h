#ifndef __LINEAR_EQUATION_TRANSFORM_H__
#define __LINEAR_EQUATION_TRANSFORM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include <memory>

#include "CaretObject.h"

namespace caret {
    class LinearEquationTransform : public CaretObject {
        
    public:
        static std::unique_ptr<LinearEquationTransform> newInstance(const float x1,
                                                                    const float x2,
                                                                    const float y1,
                                                                    const float y2,
                                                                    const float x0,
                                                                    const float y0,
                                                                    AString& errorMessageOut);

        virtual ~LinearEquationTransform();
        
        LinearEquationTransform(const LinearEquationTransform&) = delete;

        LinearEquationTransform& operator=(const LinearEquationTransform&) = delete;
        
        float transform(const float x) const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        LinearEquationTransform(const float x1,
                                const float x2,
                                const float y1,
                                const float y2,
                                const float x0,
                                const float y0);
        
        bool m_valid = false;
        
        AString m_errorMessage;
        
        float m_slope;
        
        float m_intercept;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LINEAR_EQUATION_TRANSFORM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LINEAR_EQUATION_TRANSFORM_DECLARE__

} // namespace
#endif  //__LINEAR_EQUATION_TRANSFORM_H__
