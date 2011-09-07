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
/*LICENSE_END*/

#include "AString.h"
#include <iostream>
std::ostream& operator << (std::ostream &lhs, const AString &rhs) 
{ 
    return lhs << rhs.toStdString(); 
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int32_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint32_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int64_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint64_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}


/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<float>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<double>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const float* array, const int64_t numberOfElements, const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const int32_t* array,
                             const int64_t numberOfElements,
                             const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const double* array,
                             const int64_t numberOfElements,
                             const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

