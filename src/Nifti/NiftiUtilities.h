#ifndef __NIFTIUTILITIES_H__
#define __NIFTIUTILITIES_H__

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


#include "CaretObject.h"

#include "NiftiException.h"

#include <fstream>
#include <stdint.h>

namespace caret {

/**
 * Methods to help out with GIFTI data file processing.
 */
    class NiftiUtilities : public CaretObject {

private:
    NiftiUtilities();

public:
    virtual ~NiftiUtilities();

public:
    static std::string toString1(
                    const int32_t dim1,
                    const float* floatArray,
                    const int32_t precision);

    static std::string toString3(
                    const int32_t dim1,
                    const int32_t dim2,
                    const int32_t dim3,
                    const float* floatArray,
                    const int32_t precision);

    static std::string toString2(
                           const int32_t dim1,
                           const int32_t dim2,
                    const float* floatArray,
                    const int32_t precision);

    static std::string toString(
                    const float f,
                    const int32_t precision);

    static std::string toString(
                    const double d,
                    const int32_t precision);

    static std::string toString3(const int32_t dim1,
                           const int32_t dim2,
                           const int32_t dim3,
                           const int32_t* intArray);

    static std::string toString2(const int32_t dim1,
                           const int32_t dim2,
                           const int32_t* intArray);

    static std::string toString1(const int32_t dim1,
                           const int32_t* intArray);

    static std::string toString(const int32_t i);

    static float stringToFloatArray(const std::string& dataString);

    static int32_t stringIntArray(const std::string& dataString);

    static float toFloat(const std::string& s);

    static int32_t toInt(const std::string& s);

    static std::string encodeXmlSpecialCharacters(const std::string& text);

    static std::string decodeXmlSpecialCharacters(const std::string& text);

    static void readBytesIntoArray(
                    const std::string& filename,
                    std::ifstream fis,
                    int8_t byteArray[])
            throw (NiftiException);

    static void readBytesIntoArray(
                    const std::string& filename,
                    std::ifstream fis,
                    int8_t byteArray[],
                    const int32_t offsetInArray,
                    const int32_t numberOfBytesToReadIntoArray)
            throw (NiftiException);

    static std::string leftJustify(
                    const std::string& s,
                    const int32_t numberOfCharacters);

};

} // namespace

#endif // __NIFTIUTILITIES_H__
