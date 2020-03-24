
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

#define __CARET_RGB_DECLARE__
#include "CaretRgb.h"
#undef __CARET_RGB_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::CaretRgb 
 * \brief An RGB triplet using std::array
 * \ingroup Common
 */

/**
 * Constructor that defaults to black
 */
CaretRgb::CaretRgb()
: CaretRgb(0, 0, 0)
{
}

/**
 * Constructor with RGB triplet
 * @param red
 *     Red component (0 to 255)
 * @param green
 *     Green component (0 to 255)
 * @param blue
 *     Blue component (0 to 255)
 */
CaretRgb::CaretRgb(const int32_t red,
                   const int32_t green,
                   const int32_t blue)
: std::array<int32_t, 3>()
{
    (*this)[0] = red;
    (*this)[1] = green;
    (*this)[2] = blue;
}

/**
 * Destructor.
 */
CaretRgb::~CaretRgb()
{
}

///**
// * Copy constructor.
// * @param obj
// *    Object that is copied.
// */
//CaretRgb::CaretRgb(const CaretRgb& obj)
//: std::array<int32_t, 3>(obj)
//{
//    this->copyHelperCaretRgb(obj);
//}
//
///**
// * Assignment operator.
// * @param obj
// *    Data copied from obj to this.
// * @return
// *    Reference to this object.
// */
//CaretRgb&
//CaretRgb::operator=(const CaretRgb& obj)
//{
//    if (this != &obj) {
//        std::array<int32_t, 3>::operator=(obj);
//        this->copyHelperCaretRgb(obj);
//    }
//    return *this;
//}

///**
// * Helps with copying an object of this type.
// * @param obj
// *    Object that is copied.
// */
//void
//CaretRgb::copyHelperCaretRgb(const CaretRgb& obj)
//{
//
//}

///**
// * Equality operator.
// * @param obj
// *    Instance compared to this for equality.
// * @return 
// *    True if this instance and 'obj' instance are considered equal.
// */
//bool
//CaretRgb::operator==(const CaretRgb& obj) const
//{
//    if (this == &obj) {
//        return true;    
//    }
//
//    /* perform equality testing HERE and return true if equal ! */
//
//    return false;    
//}

