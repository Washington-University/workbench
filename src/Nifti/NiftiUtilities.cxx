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


#include "NiftiUtilities.h"

using namespace caret;

/**
 * Private constructor prevents instantiation.
 *
 */
NiftiUtilities::NiftiUtilities()
    : CaretObject()
{
}

/**
 * Destructor
 */
NiftiUtilities::~NiftiUtilities()
{
}

/**
 * Convert a float array to a string with each value separated
 * by a space.
 * @param floatArray - The float array.
 * @param precision - Digits right of decimal point.
 * @return  String containing the float array values.
 * @throws IllegalArgumentException if precision is less than zero.
 *
 */
QString
NiftiUtilities::toString1(
                   const int32_t dim1,
                   const float* floatArray,
                   const int32_t precision)
{
   return QString("");
}

/**
 * Convert a 3-dim float array to a string with each value separated
 * by a space.
 * @param floatArray - The float array.
 * @param precision - Digits right of decimal point.
 * @return  String containing the float array values.
 * @throws IllegalArgumentException if precision is less than zero.
 *
 */
QString
NiftiUtilities::toString3(
                   const int32_t dim1,
                   const int32_t dim2,
                   const int32_t dim3,
                   const float* floatArray,
                   const int32_t precision)
{
      return QString("");
}

/**
 * Convert a 2-dim float array to a string with each value separated
 * by a space.
 * @param floatArray - The float array.
 * @param precision - Digits right of decimal point.
 * @return  String containing the float array values.
 * @throws IllegalArgumentException if precision is less than zero.
 *
 */
QString
NiftiUtilities::toString2(
                   const int32_t dim1,
                   const int32_t dim2,
                   const float* floatArray,
                   const int32_t precision)
{
      return QString("");
}

/**
 * Convert a float value to a string.
 * @param f - The float value.
 * @param precision - Digits to right of decimal point.
 * @return  String containing the float's value.
 * @throws IllegalArgumentException if precision is less than zero.
 *
 */
QString
NiftiUtilities::toString(
                   const float f,
                   const int32_t precision)
{
      return QString("");
}

/**
 * Convert a float value to a string.
 * @param d - The double value.
 * @param precision - Digits to right of decimal point.
 * @return  String containing the float's value.
 * @throws IllegalArgumentException if precision is less than zero.
 *
 */
QString
NiftiUtilities::toString(
                   const double d,
                   const int32_t precision)
{
      return QString("");
}

/**
 * Convert a 3-dim int array to a string with each value separated
 * by a space.
 * @param intArray - The int array.
 * @return  String containing the int array values.
 *
 */
QString
NiftiUtilities::toString3(const int32_t dim1,
                          const int32_t dim2,
                          const int32_t dim3,
                          const int32_t* intArray)
{
      return QString("");
}

/**
 * Convert a 2-dim int array to a string with each value separated
 * by a space.
 * @param intArray - The int array.
 * @return  String containing the int array values.
 *
 */
QString
NiftiUtilities::toString2(const int32_t dim1,
                         const int32_t dim2,
                         const int32_t* intArray)
{
      return QString("");
}

/**
 * Convert an int array to a string with each value separated
 * by a space.
 * @param intArray - The int array.
 * @return  String containing the int array values.
 *
 */
QString
NiftiUtilities::toString1(const int32_t dim1,
                          const int32_t* intArray)
{
      return QString("");
}

/**
 * Convert a int value to a string.
 * @param i - The float value.
 * @return  String containing the float's value.
 *
 */
QString
NiftiUtilities::toString(const int32_t i)
{
      return QString("");
}

/**
 * Get the characters received as a float array.
 *
 * @param dataString - String containing the data.
 * @return characters read into a float array.
 *
 */
float
NiftiUtilities::stringToFloatArray(const QString& dataString)
{
   return 0.0f;
}

/**
 * Get the characters received as a int array.  Any
 * non-integers are ignored.
 *
 * @param dataString - String containing the data.0
 * @return characters read into a float array.
 *
 */
int32_t
NiftiUtilities::stringIntArray(const QString& dataString)
{
   return 0;
}

/**
 * Convert a string to a float and log conversion exceptions.
 * @param s - String to convert.
 * @return Float value from the string.
 *
 */
float
NiftiUtilities::toFloat(const QString& s)
{
   return 0.0f;
}

/**
 * Convert a string to a integer and log conversion exceptions.
 * @param s - String to convert.
 * @return Integer value from the string.
 *
 */
int32_t
NiftiUtilities::toInt(const QString& s)
{
   return 0;
}

/**
 * Encode any special characters whose use is restricted
 * by XML.
 * @param text  Text whose special characters are encoded.
 * @return  Text with any special characters encode.
 *
 */
QString
NiftiUtilities::encodeXmlSpecialCharacters(const QString& text)
{
      return QString("");
}

/**
 * Decode any special characters whose use is restricted
 * by XML.
 * @param text  Text whose special characters are decoded.
 * @return  Text with special characters decoded.
 *
 */
QString
NiftiUtilities::decodeXmlSpecialCharacters(const QString& text)
{
      return QString("");
}

/**
 * Read from the stream until "byteArray.length" number of bytes have 
 * been read.
 * 
 * @param filenanme  Name of file being read.
 * @param is         The InputStream
 * @param byteArray  Bytes are read into this array.
 * @throws  CaretFileException if an IOException or an insufficient
 *                             number of bytes are read.
 *
 */
void
NiftiUtilities::readBytesIntoArray(
                   const QString& filename,
                   std::ifstream fis,
                   int8_t* byteArray)
            throw (NiftiException)
{
}

/**
 * Read from the stream until desired number of bytes have 
 * been read.
 * 
 * @param filenanme  Name of file being read.
 * @param is         The InputStream
 * @param byteArray  Bytes are read into this array.
 * @param offsetInArray Bytes read a placed into byteArray at this offset.
 * @param numberOfBytesToReadIntoArray  Number of bytes to read.
 * @throws  CaretFileException if an IOException or an insufficient
 *                             number of bytes are read.
 *
 */
void
NiftiUtilities::readBytesIntoArray(
                   const QString& filename,
                   std::ifstream fis,
                   int8_t* byteArray,
                   const int32_t offsetInArray,
                   const int32_t numberOfBytesToReadIntoArray)
            throw (NiftiException)
{
}

/**
 * Left justify a string and pad with blanks on the right.  If the input
 * string is longer than "numberOfCharacters", the the returned string
 * will be the length of the input string.
 * 
 * @param s - string to left justify.
 * @param numberOfCharacters - desired length of left justified string.
 * @return Input string with padding on the right so that the string
 *    contains "numberOfCharacters".
 *
 */
QString
NiftiUtilities::leftJustify(
                   const QString& s,
                   const int32_t numberOfCharacters)
{
      return QString("");
}

