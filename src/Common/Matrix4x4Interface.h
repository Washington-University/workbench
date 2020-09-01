#ifndef __MATRIX4X4_INTERFACE_H__
#define __MATRIX4X4_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

/**
 * \class caret::Matrix4x4Interface
 * \brief Interface so that Matrix4x4 operations can be performed by modules that cannot access Matrix4x4
 * \ingroup Common
 */


#include <memory>





namespace caret {

    class Matrix4x4Interface {
        
    public:
        Matrix4x4Interface() { }
        
        virtual ~Matrix4x4Interface() { }
        
        virtual void multiplyPoint3(float p[3]) const = 0;

        Matrix4x4Interface(const Matrix4x4Interface&) { }

        Matrix4x4Interface& operator=(const Matrix4x4Interface&) { return *this; }
        

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MATRIX4X4_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MATRIX4X4_INTERFACE_DECLARE__

} // namespace
#endif  //__MATRIX4X4_INTERFACE_H__
