#ifndef __CARET_RGB_H__
#define __CARET_RGB_H__

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


#include <array>
#include <memory>

namespace caret {

    class CaretRgb : public std::array<int32_t, 3> {
        
    public:
        CaretRgb();
        
        CaretRgb(const int32_t red,
                 const int32_t green,
                 const int32_t blue);
        
        virtual ~CaretRgb();
        
        /** @return The red component */
        int32_t red() const { return (*this)[0]; }  /* could use "at(0)". at() does bounds testing and throws exception */
        
        /** @return The green component */
        int32_t green() const { return (*this)[1]; }
        
        /** @return The blue component */
        int32_t blue() const { return (*this)[2]; }
        
//        CaretRgb(const CaretRgb& obj);

//        CaretRgb& operator=(const CaretRgb& obj);
//
//        bool operator==(const CaretRgb& obj) const;
        

        // ADD_NEW_METHODS_HERE

    private:
//        void copyHelperCaretRgb(const CaretRgb& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_RGB_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_RGB_DECLARE__

} // namespace
#endif  //__CARET_RGB_H__
