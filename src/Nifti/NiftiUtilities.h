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
    static AString toString1(
                    const int32_t dim1,
                    const float* floatArray,
                    const int32_t precision);

    static AString toString3(
                    const int32_t dim1,
                    const int32_t dim2,
                    const int32_t dim3,
                    const float* floatArray,
                    const int32_t precision);

    static AString toString2(
                           const int32_t dim1,
                           const int32_t dim2,
                    const float* floatArray,
                    const int32_t precision);

    static AString toString(
                    const float f,
                    const int32_t precision);

    static AString toString(
                    const double d,
                    const int32_t precision);

    static AString toString3(const int32_t dim1,
                           const int32_t dim2,
                           const int32_t dim3,
                           const int32_t* intArray);

    static AString toString2(const int32_t dim1,
                           const int32_t dim2,
                           const int32_t* intArray);

    static AString toString1(const int32_t dim1,
                           const int32_t* intArray);

    static AString toString(const int32_t i);

    static float stringToFloatArray(const AString& dataString);

    static int32_t stringIntArray(const AString& dataString);

    static float toFloat(const AString& s);

    static int32_t toInt(const AString& s);

    static AString encodeXmlSpecialCharacters(const AString& text);

    static AString decodeXmlSpecialCharacters(const AString& text);

    static void readBytesIntoArray(
                    const AString& filename,
                    std::ifstream fis,
                    int8_t byteArray[])
            throw (NiftiException);

    static void readBytesIntoArray(
                    const AString& filename,
                    std::ifstream fis,
                    int8_t byteArray[],
                    const int32_t offsetInArray,
                    const int32_t numberOfBytesToReadIntoArray)
            throw (NiftiException);

    static AString leftJustify(
                    const AString& s,
                    const int32_t numberOfCharacters);

};

} // namespace

#endif // __NIFTIUTILITIES_H__
